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

#include "lps25.h"
#include "buzz.h"
#include "uart.h"

int main(void)
{
	uint8_t i;
	uint16_t tone;

	uart_init(0);
        uart_printstr(0, "Vario test software.\n");
	buzz_init();

	if (lps25_init())
		beep(2500);

	while(1) {
		/*
		for (i=0; i<20; i++) {
			tone = (uint16_t)(1000+i*100);
			beep(tone);
		}
		*/
	}

	return(0);
}
