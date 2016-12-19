/* Copyright (C) 2016 Enrico Rossi
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

/*! \file i2c_usi.h */

#ifndef I2C_USI
#define I2C_USI

#include <stdint.h>
#include "USI_TWI_Master.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

void i2c_init(void);
void i2c_shut(void);
uint8_t i2c_mXm(const uint8_t addr, const uint16_t lenght,
		uint8_t *data, const uint8_t stop);

#endif
