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

#include <stdlib.h>
#include <stdint.h>
#include <util/delay.h>
#include "USI_TWI_Master.h"

#define LPS25_R_WHO_AM_I 0x0F
#define LPS25_ADDR 0xba

int main(void)
{
	uint8_t* buffer;

	buffer = malloc(3);
	USI_TWI_Master_Initialise();
	*buffer = LPS25_ADDR;
	*(buffer + 1) = LPS25_R_WHO_AM_I;
	USI_TWI_Start_Transceiver_With_Data(buffer, 2, FALSE);
	*buffer = LPS25_ADDR | 1;
	USI_TWI_Start_Transceiver_With_Data(buffer, 2, TRUE);

	while(1) {
		_delay_ms(1000);
	}
}
