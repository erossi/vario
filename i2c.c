/* Copyright (C) 2011-2016 Enrico Rossi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdint.h>
#include <util/twi.h>
#include <avr/io.h>
#include "i2c.h"

/* defines */
#define START 1
#define STOP 2
#define SLA 3
#define DATA 4
#define ACK 5
#define NACK 6

/*! Perform an i2c operation.
 *
 * \return the i2c status register properly masked.
 */
uint8_t i2c_send(const uint8_t code)
{
	switch (code) {
		/* valid also as restart */
		case START:
			TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
			loop_until_bit_is_set(TWCR, TWINT);
			break;
		case STOP:
			TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
			break;
		case SLA:
		case DATA:
			/* clear interrupt to start transmission
			 * and send the content of TWDR
			 */
			TWCR = _BV(TWINT) | _BV(TWEN); 
			loop_until_bit_is_set(TWCR, TWINT);
			break;
		/* read a byte and ACK it */
		case ACK:
			TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
			loop_until_bit_is_set(TWCR, TWINT);
			break;
		/* read a byte and NACK it */
		case NACK:
			TWCR = _BV(TWINT) | _BV(TWEN);
			loop_until_bit_is_set(TWCR, TWINT);
			break;
		default:
			break;
	}

	return(TW_STATUS);
}

/*! Initialize the i2c bus.
 *
 * See the datasheet for SCL speed.
 * Prescaler value (1, 4, 16, 64)
 *
 * SCL freq = CPU FREQ / (16 + 2 * TWBR * Prescaler)
 * (16 + 2 * TWBR * Prescaler) = CPU FREQ / SCL freq
 * 2 * TWBR * Prescaler = (CPU FREQ / SCL freq) - 16
 * TWBR * Prescaler = ((CPU FREQ / SCL freq) - 16)/2
 *
 * SCLf(max) = CPUf/16
 *
 * 16Mhz CLK, 100Khz I2C bus, Prescaler = 4, TWBR = 18
 * 16Mhz CLK, 10Khz I2C bus, Prescaler = 4, TWBR = 198
 * 1Mhz CLK, 10Khz I2C bus, Prescaler = 1, TWBR = 42
 */
void i2c_init(void)
{
#if (F_CPU == 1000000UL)
	/* Prescaler 1 */
	TWSR = 0;
	TWBR = 42;
#elif (F_CPU == 16000000UL)
	/* Prescaler 8 */
	TWSR |= _BV(TWPS0);
	TWBR = 99;
#else
#error I2C clock rate unsupported
#endif
}

/*! Shutdown the i2c bus.
 */
void i2c_shut(void)
{
	TWSR = 0;
	TWBR = 0;
}

/*! i2c Master Trasmitter/Receive Mode.
 *
 * Dependent on the LSB of the address perform both
 * the mtm/mrm functions.
 *
 * \param *data the pointer to the block of byte, the 1st
 * is the address of the slave.
 *
 * \param size the size of data.
 * \param stop the stop at the end of the communication.
 */
uint8_t i2c_mXm(uint8_t *data, const uint8_t size, uint8_t stop)
{
	uint8_t i, err;

	/* START */
	err = i2c_send(START);

	/* if start acknoledge */
	if ((err == TW_START) || (err == TW_REP_START)) {
		/* Send address adn get if the slave ACK or not */
		TWDR = *data;
		err = i2c_send(SLA);
	}

	/* if read operation */
	if (*data & TW_READ) {
		/* if the address is ACK */
		if (err == TW_MR_SLA_ACK) {
			/* Receive data */
			for (i = 1; i < (size - 1); i++) {
				/* read a byte and send ACK */
				err = i2c_send(ACK);
				*(data + i) = TWDR;
			}

			/* read a byte and send NACK */
			err = i2c_send(NACK);
			*(data + size - 1) = TWDR;
		} else {
			stop = TRUE;
		}
	} else {
		/* if the address is ACK */
		if (err == TW_MT_SLA_ACK)
			/* send data */
			for (i = 1; i < size; i++) {
				TWDR = *(data+i);
				err = i2c_send(DATA);

				/* if data is not ACK */
				if (err != TW_MT_DATA_ACK)
					break;
			}

		/* if client NACK on ADDR or DATA */
		if ((err == TW_MT_SLA_NACK) || (err == TW_MT_DATA_NACK))
			/* send the stop */
			stop = TRUE;
	}

	/* if TX data is ACK or RX data is NACK then ok */
	if ((err == TW_MT_DATA_ACK) || (err == TW_MR_DATA_NACK))
		err = 0;

	/* send the STOP if required */
	if (stop)
		i2c_send(STOP);

	return(err);
}

/*! I2C General Call
 *
 * ex. i2c_gc(I2C_GC_RESET);
 */
uint8_t i2c_gc(const uint8_t call)
{
	uint8_t err;
	uint8_t *buffer;

	buffer = malloc(2);
	*buffer = 0;
	err = 0;

	switch(call) {
		case I2C_GC_RESET:
		default:
			/* Send the General Call reset */
			*(buffer + 1) = 0x06;
			err = i2c_mXm(buffer, 2, TRUE);
	}

	free (buffer);
	return (err);
}
