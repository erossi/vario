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

/*! \file i2c.h */

#ifndef I2C_DEF
#define I2C_DEF

#include <stdint.h>

#define I2C_GC_RESET 0
#define I2C_TIMEOUT 0xff

#define READ 1
#define WRITE 0

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

void i2c_init(void);
void i2c_shut(void);
uint8_t i2c_mXm(uint8_t *data, const uint8_t size, uint8_t stop);
uint8_t i2c_gc(const uint8_t call);
#endif
