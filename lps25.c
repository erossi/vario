/*
    LPS25H STMicroelectronics MEMS pressure sensor.
    Copyright (C) 2016 Enrico Rossi

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "lps25.h"

/*! Read registers.
 *
 * One or more registers can be read.
 *
 * \param reg the register address.
 * \param buffer the content read.
 * \size how many byte to read.
 * \return TRUE = error.
 * \note to actually read more than one register, the MSB of the
 * register's address must be set.
 */
uint8_t register_read(uint8_t reg, uint8_t *data, uint8_t size)
{
	uint8_t err;
	uint8_t *buffer;

	/* assert the MSB if more than one register */
	if (size > 1)
		reg |= (1<<7);

	buffer = malloc(size + 1);
	*buffer = LPS25_ADDR;
	*(buffer + 1) = reg;
	err = FALSE;

#ifdef I2C_USI
	if (USI_TWI_Start_Transceiver_With_Data(buffer, 2, FALSE))
		*buffer = LPS25_ADDR | 1;
	else
		err = USI_TWI_Get_State_Info();

	if (!err && USI_TWI_Start_Transceiver_With_Data(buffer, size + 1, TRUE))
		memcpy(data, buffer + 1, size);
	else
		err = USI_TWI_Get_State_Info();
#else
	err = i2c_mXm(buffer, 2, FALSE);

	if (!err) {
		/* set addr to read */
		*buffer = LPS25_ADDR | 1;
		err = i2c_mXm(buffer, size + 1, TRUE);
		memcpy(data, buffer + 1, size);
	}
#endif

	free (buffer);
	return (err);
}

/*! Write a single register */
uint8_t register_write(const uint8_t reg, const uint8_t value)
{
	uint8_t err = 0;
	uint8_t *buffer;

	buffer = malloc(3);
	*buffer = LPS25_ADDR;
	*(buffer + 1) = reg;
	*(buffer + 2) = value;

#ifdef I2C_USI
	if (USI_TWI_Start_Transceiver_With_Data(buffer, 3, TRUE))
		err = FALSE;
	else
		err = USI_TWI_Get_State_Info();
#else
	err = i2c_mXm(buffer, 3, TRUE);
#endif

	free(buffer);
	return (err);
}

/* Setup the FIFO in FIFO mean mode.
 *
 * Config as per datasheet.
 *
 * RES_CONF(0x10) = 0x05 (set AVGT=16, AVGP=32, or less)
 *  Pressure 32 sample average,
 *  Temperature 16 sample average.
 *
 * FIFO_CTRL (0x2E) = 0xC1 (Set FIFO Mean mode with average
 * on 2 samples or more, up to 0xDF)
 *
 * CTRL_REG2 (0x21) = 0x40 (FIFO enabled, decimation disabled)
 *
 * CTRL_REG1 (0x20) = 0x90 (ODR = 1 Hz, power-on device)
 * CTRL_REG4 enable the IRQ pin when data is ready.
 *
 */
void lps25_fifo_mean_mode(void)
{
	register_write(LPS25_RES_CONF,
			(1 << LPS25_AVGT0) | (1 << LPS25_AVGP0));
	register_write(LPS25_FIFO_CTRL,
			(1 << LPS25_F_MODE1) | (1 << LPS25_F_MODE2) |
			(1 << LPS25_WTM_POINT0));
	register_write(LPS25_CTRL_REG2,
			(1 << LPS25_FIFO_EN));
	register_write(LPS25_CTRL_REG1,
			(1 << LPS25_PD) | (1 << LPS25_ODR0));
	register_write(LPS25_CTRL_REG4,
			(1 << LPS25_P1_DRDY));
}

/* Update the temperature.
 *
 * Read((u8*)pu8, TEMP_OUT_ADDR, 2); // @0x2B(OUT_L)~0x2C(OUT_H)
 * Temp_Reg_s16 = ((u16) pu8[1]<<8) | pu8[0]; // make a SIGNED 16 bit variable
 * Temperature_DegC = 42.5 + Temp_Reg_s16 / 480; // offset and scale
 */
uint8_t lps25_temperature(void)
{
	int16_t temp_out;
	uint8_t err;

	err = register_read(LPS25_TEMP_OUT, lps25->TEMP_OUT, 2);

	if (!err) {
		temp_out = ((uint16_t)lps25->TEMP_OUT[1] << 8) | (uint16_t)lps25->TEMP_OUT[0];
		lps25->temperature = 42.5 + (float)temp_out / 480;
	} else {
		lps25->temperature = -99;
	}

	return(err);
}

/*
 * Read((u8*)pu8, PRESS_OUT_ADDR, 3); // @0x28(OUT_XL)~0x29(OUT_L)~0x2A(OUT_H)
 * Pressure_Reg_s32 = ((u32)pu8[2]<<16)|((u32)pu8[1]<<8)|pu8[0]; // make a SIGNED 32 bit
 * Pressure_mb = Pressure_Reg_s32 / 4096; // scale
 *
 * 7. Check the temperature and pressure values make sense
 * Reading fixed 760 hPa, means the sensing element is damaged.
 */
uint8_t lps25_pressure(void)
{
	int32_t hpout;
	uint8_t err;

	err = register_read(LPS25_PRESS_OUT, lps25->PRESS_OUT, 3);

	if (!err) {
		hpout = ((int32_t)lps25->PRESS_OUT[2] << 16) | ((int32_t)lps25->PRESS_OUT[1] << 8) | (int32_t)lps25->PRESS_OUT[0];
		lps25->Hpa = (float)hpout / 4096;
	} else {
		lps25->Hpa = 0;
	}

	return(err);
}

/*! Suspend the device.
 *
 * Write 0 to the PD bit of the CTRL_REG1 register.
 * @note This is the default at boot.
 */
uint8_t lps25_suspend(void)
{
	uint8_t err, buffer;

	err = register_read(LPS25_CTRL_REG1, &buffer, 1);
	/* PD bit = 0 */
	buffer &= ~_BV(LPS25_PD);

	if (!err)
		err = register_write(LPS25_CTRL_REG1, buffer);

	return(!err);
}

/*! Resume the device from stdby
 *
 * Write 1 to the PD bit of the CTRL_REG1 register.
 */
uint8_t lps25_resume(void)
{
	uint8_t err, buffer;

	/* Read the register */
	err = register_read(LPS25_CTRL_REG1, &buffer, 1);
	/* PD bit = 1 */
	buffer |= _BV(LPS25_PD);

	/* write the register */
	if (!err)
		err = register_write(LPS25_CTRL_REG1, buffer);

	return(!err);
}

/*! Set the temperature resolution mode to 64.
 *
 * See Table 17. Temperature resolution configuration
 */
uint8_t temperature_init(void)
{
	uint8_t err, buffer;

	err = register_read(LPS25_RES_CONF, &buffer, 1);
	buffer |= (1<<LPS25_AVGT0) | (1<<LPS25_AVGT1);

	if (!err)
		err = register_write(LPS25_RES_CONF, buffer);

	return(err);
}

/*!
 * See Table 16. Pressure resolution configuration
 */
uint8_t pressure_init(void)
{
	uint8_t err, buffer;

	err = register_read(LPS25_RES_CONF, &buffer, 1);
	buffer |= (1<<LPS25_AVGP0) | (1<<LPS25_AVGP1);

	if (!err)
		err = register_write(LPS25_RES_CONF, buffer);

	return(err);
}

/* Initialize the device and clear the struct.
 *
 */
uint8_t lps25_init(void)
{
	uint8_t err, buffer;

	lps25 = malloc(sizeof(struct lps25_t));
	lps25->temperature = -99;
	lps25->Hpa = 0;
	lps25->dHpa = 0;

	lps25->TEMP_OUT = malloc(2);
	lps25->PRESS_OUT = malloc(3);

#ifdef I2C_USI
	USI_TWI_Master_Initialise();
#else
	i2c_init();
#endif

	_delay_ms(1);
	err = register_read(LPS25_WHO_AM_I, &buffer, 1);

	if (!err && (buffer != 0xbd))
		err = 0xff;

	/* reset */
	if (!err) {
		err = register_write(LPS25_CTRL_REG2,
				(1 << LPS25_BOOT) | (1 << LPS25_SWRESET));

		do {
			register_read(LPS25_CTRL_REG2, &buffer, 1);
		} while (buffer & (1 << LPS25_BOOT));

		/* wait an additional 5ms */
		_delay_ms(5);
	}

	return(err);
}

uint8_t lps25_shut(void)
{
	free(lps25->PRESS_OUT);
	free(lps25->TEMP_OUT);
	free(lps25);
	return(lps25_suspend());
}

/*! Initiate a one shot sample
 */
uint8_t lps25_oneshot(void)
{
	uint8_t err, buffer;

	buffer = TRUE;
	/* 1.Power down the device (clean start) */
	err = register_write(LPS25_CTRL_REG1, 0x00);

	/* 2. Turn on the pressure sensor analog front end in single shot mode */
	if (!err)
		// err = register_write(LPS25_CTRL_REG1, 0xB0);
		err = register_write(LPS25_CTRL_REG1, 0x84);

	/* 3. Run one-shot measurement (temperature and pressure),
	 * the set bit will be reset by the sensor itself after execution
	 * (self-clearing bit) */
	if (!err) {
		err = register_write(LPS25_CTRL_REG2, 0x01);

		/* 4. Wait until the measurement is completed */
		do {
			_delay_ms(10);
			err = register_read(LPS25_CTRL_REG2, &buffer, 1);

			if (err)
				break;
		} while (buffer & 1);
	}

	if (!err) {
		err = lps25_pressure();
		err = lps25_temperature();
	}

	/*
	 * lps25_suspend();
	 */

	return(err);
}
