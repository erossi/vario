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
	/* Test delta-meters, 1hPa = 8.3 meters */
	debug_print_P(PSTR("dmt: "));
	debug->buffer = dtostrf(((-lps25->dHpa) * 8.3), 5, 2, debug->buffer);
	debug_print(NULL);
	debug_print_P(PSTR("\n"));
	debug_print_P(PSTR("Temp: "));
	debug->buffer = dtostrf(lps25->temperature, 3, 2, debug->buffer);
	debug_print(NULL);
	debug_print_P(PSTR("\n"));
	// POUT
	debug_print_P(PSTR("POUT: "));
	sprintf(debug->buffer, "%#02x", lps25->PO[3]);
	debug_print(NULL);
	sprintf(debug->buffer, "%02x", lps25->PO[2]);
	debug_print(NULL);
	sprintf(debug->buffer, "%02x", lps25->PO[1]);
	debug_print(NULL);
	sprintf(debug->buffer, "%02x\n", lps25->PO[0]);
	debug_print(NULL);
	// TOUT
	debug_print_P(PSTR("TOUT: "));
	sprintf(debug->buffer, "%#04x\n\n", lps25->TOUT);
	debug_print(NULL);
}

int main(void)
{
	debug_init();
	buzz_init();
	sei();

	if (lps25_init())
		beep(10);
	else
		beep(2);

	if (lps25_oneshot())
		debug_print_P(PSTR("ERROR\n"));
	else
		print_lps25();

	lps25_fifo_mean_mode();
	// lps25_resume();

	while(1) {
		if (bit_is_set(PINC, PC0)) {
			lps25_pressure();
			lps25_temperature();
			beep((int8_t)((-lps25->dHpa) * 8.3));
			print_lps25();
		}
	}

	return(0);
}
