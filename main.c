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
#include <util/delay.h>
#include "lps25.h"
#include "buzz.h"
#include "debug.h"

void print_lps25(void)
{
	debug_print_P(PSTR("Hpa: "));
	debug->buffer = dtostrf(lps25->Hpa, 5, 2, debug->buffer);
	debug_print(NULL);
	debug_print_P(PSTR("\n"));
	debug_print_P(PSTR("dHpa: "));
	debug->buffer = dtostrf(lps25->dHpa, 5, 2, debug->buffer);
	debug_print(NULL);
	debug_print_P(PSTR("\n"));
	debug_print_P(PSTR("Alt: "));
	debug->buffer = dtostrf(lps25->altitude, 5, 2, debug->buffer);
	debug_print(NULL);
	debug_print_P(PSTR("\n"));
	debug_print_P(PSTR("Temp: "));
	debug->buffer = dtostrf(lps25->temperature, 3, 2, debug->buffer);
	debug_print(NULL);
	debug_print_P(PSTR("\n"));
}

int main(void)
{
	uint8_t i;
	uint16_t tone;

	debug_init();
	buzz_init();

	if (lps25_init())
		beep(2500);

	lps25_fifo_mean_mode();
	lps25_resume();

	while(1) {
		lps25_temperature();
		print_lps25();
		_delay_ms(500);
	}

	return(0);
}
