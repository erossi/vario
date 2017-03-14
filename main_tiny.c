/* Copyright (C) 2016, 2017 Enrico Rossi
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

/* 1mt O2 15C = 0.12hPa
 *
 */

void beep(uint8_t i)
{
	do {
		buzz_play(3000, 50);
		_delay_ms(200);
		buzz_stop();
		_delay_ms(200);
	} while (--i);
}

int main(void)
{
	int8_t dms;

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
			// 1hPa = 8.3m
			// 1m/s = 1/8.3 hPa/s = 0.12 hPa/s
			// 7Hz dms rounded to 0.02 hPa @7Hz
			dms = (-lps25->dHpa) * 500;

			if ((dms > 5) || (dms < -5))
				buzz_play((3000 + dms * 10), 50);
			else
				buzz_stop();
		}
	}

	return(0);
}
