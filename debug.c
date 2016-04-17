/* Copyright (C) 2014-2016 Enrico Rossi

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "debug.h"

/*! Print a flash-stored string to the terminal.
 *
 * \param string MUST be a PSTR() string.
 */
void debug_print_P(PGM_P string)
{
	if (debug->active) {
		strcpy_P(debug->buffer, string);
		usart_printstr(DEBUG_SERIAL_PORT, NULL);
	}
}

/*! Print the string.
 *
 * The string to be printed can be stored in the
 * buffer. Since the buffer points to the usart buffer
 * there is no need to pass anything to the usart port.
 *
 * \param string if NULL print the buffer.
 */
void debug_print(const char *string)
{
	if (debug->active) {
		if (string)
			strcpy(debug->buffer, string);

		usart_printstr(DEBUG_SERIAL_PORT, NULL);
	}
}

/*! Print the release version. */
void debug_version(void)
{
	debug_print_P(PSTR("Vario "));
	debug_print_P(PSTR(GITREL));
	debug_print_P(PSTR("\n"));
}

/*! Print the greetings and release number. */
static void hello(void)
{
        debug_print_P(PSTR("\n\n\n"));
	debug_version();
        debug_print_P(PSTR("GNU LGPL v2.1\n\n"));
}

/*! Resume call.
 *
 * \ingroup sleep_group
 */
void debug_resume(void)
{
	usart_resume(DEBUG_SERIAL_PORT);
	debug->active = 1;
}

/*! Suspend call.
 *
 * \ingroup sleep_group
 */
void debug_suspend(void)
{
	debug->active = 0;
	usart_suspend(DEBUG_SERIAL_PORT);
}

/*! Initialize the debug_t structure and ask if
 * debug is active.
 */
void debug_init(void)
{
	usart_init();
	debug = malloc(sizeof(struct debug_t));
	debug->buffer = usart->tx0_buffer;
	debug_resume();
	hello();
}
