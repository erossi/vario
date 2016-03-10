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

#include <util/delay.h>
#include "lps25.h"

uint8_t lps25_init(void)
{
	uint8_t err, buffer;

	i2c_init();
	_delay_ms(1);
	i2c_gc(I2C_GC_RESET);
	buffer = LPS25_R_WHO_AM_I;

	err = i2c_mtm(LPS25_ADDR, 1, &buffer, FALSE);

	if (!err)
		err = i2c_mrm(LPS25_ADDR, 1, &buffer, TRUE);

	if (!err && (buffer == 0xbd))
		return(TRUE);
	else
		return(FALSE);
}

void lps25_shut(void)
{
}

void lps25_suspend(void)
{
}

void lps25_resume(void)
{
}

