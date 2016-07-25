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
	debug_print_P(PSTR("Temp: "));
	debug->buffer = dtostrf(lps25->temperature, 3, 2, debug->buffer);
	debug_print(NULL);
	debug_print_P(PSTR("\n"));
	debug_print_P(PSTR("Reg PRESSOUT: "));
	sprintf(debug->buffer, "%#02x", lps25->PRESS_OUT[0]);
	debug_print(NULL);
	sprintf(debug->buffer, "%02x", lps25->PRESS_OUT[1]);
	debug_print(NULL);
	sprintf(debug->buffer, "%02x\n", lps25->PRESS_OUT[2]);
	debug_print(NULL);
	debug_print_P(PSTR("Reg TEMPOUT: "));
	sprintf(debug->buffer, "%#02x", lps25->TEMP_OUT[0]);
	debug_print(NULL);
	sprintf(debug->buffer, "%02x\n", lps25->TEMP_OUT[1]);
	debug_print(NULL);
	debug_print_P(PSTR("\n"));
}

void dump_all(void)
{
	uint8_t *memory;
	uint8_t i;

	memory = malloc(0x80);

	if (lps25_dump_all(memory)) {
		debug_print_P(PSTR("ERROR\n"));
	} else {
		for (i=0x8; i<0x3b; i++) {
			sprintf(debug->buffer, "%#02x: %#02x %#02x\n", i, memory[i], memory[i+0x40]);
			debug_print(NULL);
		}
	}

	free(memory);
}

int main(void)
{
	debug_init();
	buzz_init();

	if (!lps25_init())
		beep(2500);

	if (lps25_oneshot())
		debug_print_P(PSTR("ERROR\n"));
	else
		print_lps25();
	/*
	lps25_fifo_mean_mode();
	lps25_resume();
	*/

	while(1) {
		/*
		if (lps25_oneshot())
			debug_print_P(PSTR("ERROR\n"));
		else
			print_lps25();

		dump_all();
		*/
		_delay_ms(10000);
	}

	return(0);
}
