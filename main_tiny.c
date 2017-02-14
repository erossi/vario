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
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lps25.h"
#include "buzz.h"

int main(void)
{
	buzz_init();
	sei();

	/* wait for the capacitor to charge and set the
	 * coorect i2c address (0.5 sec).
	 */
	_delay_ms(1000);

	if (lps25_init())
		beep(10);
	else
		beep(2);

	lps25_fifo_mean_mode();

	while(1) {
		if (bit_is_set(PINB, PB4)) {
			lps25_pressure();
			beep((int8_t)((-lps25->dHpa) * 8.3));
		}
	}

	return(0);
}
