/* Copyright (C) 2017 Enrico Rossi
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
#include "debug.h"

void print_lps25_hpa(void)
{
	debug->buffer = dtostrf(lps25->Hpa, 5, 2, debug->buffer);
	debug_print(NULL);
	debug_print_P(PSTR(" "));
	debug->buffer = dtostrf(lps25->dHpa, 5, 2, debug->buffer);
	debug_print(NULL);
	debug_print_P(PSTR(" "));
	debug->buffer = dtostrf(((-lps25->dHpa) * 8.3), 5, 2, debug->buffer);
	debug_print(NULL);
	debug_print_P(PSTR("\n"));
}

int main(void)
{
	debug_init();
	sei();

	if (lps25_init())
		debug_print_P(PSTR("I2C ok\n"));
	else
		debug_print_P(PSTR("I2C Err.\n"));

	lps25_fifo_mean_mode();

	while(1) {
		if (bit_is_set(PINC, PC0)) {
			lps25_pressure();
			print_lps25_hpa();
		}
	}

	return(0);
}
