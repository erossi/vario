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
#include <avr/io.h>
#include <util/delay.h>
#include "lps25.h"

/* Read a register.
 *
 * \return TRUE = error.
 */
uint8_t register_read(uint8_t reg, uint8_t *buffer)
{
	uint8_t err;

	err = i2c_mtm(LPS25_ADDR, 1, &reg, FALSE);

	if (!err)
		err = i2c_mrm(LPS25_ADDR, 1, buffer, TRUE);

	return(err);
}

uint8_t register_write(uint8_t reg, const uint8_t value)
{
	uint8_t err;

	err = i2c_mtm(LPS25_ADDR, 1, &reg, FALSE);

	if (!err)
		err = i2c_mtm(LPS25_ADDR, 1, (uint8_t *) &value, TRUE);

	return(err);
}

/* Setup the FIFO in FIFO mean mode.
 *
 * Config as per datasheet.
 *
 * RES_CONF (10h) = 05h
 *  Pressure 32 sample average,
 *  Temperature 16 sample average.
 *
 * FIFO_CTRL (2Eh) = C0h
 * CTRL_REG2 (21h) = 40h
 *
 */
void lps25_fifo_mean_mode(void)
{
	register_write(LPS25_R_RES_CONF,
			((1<<LPS25_B_AVGT0) | (1<<LPS25_B_AVGP0)));
	register_write(LPS25_R_FIFO_CTRL,
			((1<<LPS25_B_F_MODE1) | (1<<LPS25_B_F_MODE2)));
	register_write(LPS25_R_CTRL_REG2,
			(1<<LPS25_B_FIFO_EN));
}

/*! Set the fifo mean mode watermark
 *
 * Watermark represent the number of elements used
 * to calculate the average in the fifo mean mode.
 */
void set_watermark(void)
{
}

/* Update the temperature.
 *
 */
uint8_t lps25_temperature(void)
{
	int16_t temp_out;
	uint8_t byte, err;

	err = register_read(LPS25_R_TEMP_OUT_H, &byte);

	if (!err) {
		temp_out = (byte << 8);
		err = register_read(LPS25_R_TEMP_OUT_L, &byte);
		temp_out |= byte;
	}

	if (!err)
		lps25->temperature = 42.5 + (temp_out / 480);
	else
		lps25->temperature = -99;

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

	err = register_read(LPS25_R_CTRL_REG1, &buffer);
	/* PD bit = 0 */
	buffer &= ~_BV(LPS25_B_PD);

	if (!err)
		err = register_write(LPS25_R_CTRL_REG1, buffer);

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
	err = register_read(LPS25_R_CTRL_REG1, &buffer);
	/* PD bit = 1 */
	buffer |= _BV(LPS25_B_PD);

	/* write the register */
	if (!err)
		err = register_write(LPS25_R_CTRL_REG1, buffer);

	return(!err);
}

/*! Set the temperature resolution mode to 64.
 *
 * See Table 17. Temperature resolution configuration
 */
uint8_t temperature_init(void)
{
	uint8_t err, buffer;

	err = register_read(LPS25_R_RES_CONF, &buffer);
	buffer |= (1<<LPS25_B_AVGT0) | (1<<LPS25_B_AVGT1);

	if (!err)
		err = register_write(LPS25_R_RES_CONF, buffer);

	return(err);
}

/*!
 * See Table 16. Pressure resolution configuration
 */
uint8_t pressure_init(void)
{
	uint8_t err, buffer;

	err = register_read(LPS25_R_RES_CONF, &buffer);
	buffer |= (1<<LPS25_B_AVGP0) | (1<<LPS25_B_AVGP1);

	if (!err)
		err = register_write(LPS25_R_RES_CONF, buffer);

	return(err);
}

/*! Initiate a one shot sample
 */
uint8_t one_shot(void)
{
	uint8_t err, buffer;

	err = register_read(LPS25_R_CTRL_REG2, &buffer);
	buffer |= (1<<LPS25_B_ONE_SHOT);

	if (!err)
		err = register_write(LPS25_R_CTRL_REG2, buffer);

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
	lps25->altitude = 0;

	i2c_init();
	_delay_ms(1);
	i2c_gc(I2C_GC_RESET);
	err = register_read(LPS25_R_WHO_AM_I, &buffer);

	if (!err && (buffer == 0xbd))
		return(TRUE);
	else
		return(FALSE);
}

uint8_t lps25_shut(void)
{
	free(lps25);
	return(lps25_suspend());
}
