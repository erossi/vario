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

/*! \file debug.h
 * \brief Functions to activare and display (rs232) debug.
 */

#ifndef DBG_H
#define DBG_H

#include <avr/pgmspace.h>
#include "usart.h"

/*! Macro example for create a function macro in 2 pass.
 *
 * The scope should be \#x -> f(x)
 */
#define QUOTEME_(x) #x
/*! Macro example 2nd pass. */
#define QUOTEME(x) QUOTEME_(x)

/*! UnComment if the usart_init should be called during
 * the init.
 *
 * usart init have to be called only once in the program.
 *
 * Better if you define this in the Makefile to avoid to
 * enable it twice in the software.
 *
 * #define DEBUG_REQUIRE_USART_INIT
 */

/*! Serial port to send the debug output.
 *
 * Select 0 or 1
 */
#define DEBUG_SERIAL_PORT 0
#define DEBUG_TXBUF tx1_buffer
#define DEBUG_RXBUF rx1_buffer
#define DEBUG_RXFLAG FL_RX1

/*! rx use the IRQ. */
#define DEBUG_RX_USEIRQ

/*! Maximum number of char a line can be */
#define MAX_LINE_LENGHT USART1_TXBUF_SIZE

/*! Number of char a substring can be made of.
Substrings are used to compose the line when, for example,
conversions have to be made from int to string etc. */
#define MAX_STRING_LENGHT 20

/*! Unused. */
#define PRINT_VALUE_X_LINE 16

/*! Seconds to wait for an answer (y/n) */
#define SEC_FOR_Y 5

/*! GITREL Environment check */
#ifndef GITREL
#define GITREL "unknown"
#endif

/*! True False */
#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

/*! The main debug structure, it has to be allocated,
  eventually, if debug is not active, you can avoid the
  allocation of the two members char *.
  */
struct debug_t {
	/*! is debug active, shall we print the output? */
	uint8_t active;
	char *buffer;
};

/*! Global */
volatile struct debug_t *debug;

void debug_print_P(PGM_P string);
void debug_print(const char *string);
void debug_version(void);
void debug_resume(void);
void debug_suspend(void);
void debug_init(void);

#endif
