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

#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "usart.h"

/*! Start the usart port.
 *
 * example of initializations
 * Tx only without Rx
 * UCSR0B = _BV(TXEN0);
 * Tx and Rx only without interrupt
 * UCSR0B = _BV(TXEN0) | _BV(RXEN0);
 * Rx only with interrupt
 * UCSR0B = _BV(RXCIE0) | _BV(RXEN0);
 * Rx with interrupt and Tx normal
 * UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
 * Rx and Tx with interrupt
 * UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXCIE0) | _BV(TXEN0);
 *
 * \parameters port the serial port number.
 * \bug port should be used with multiport.
 */
void usart_resume(const uint8_t port)
{
	usart->tx0Idx = 0;
	usart->tx0_buffer[0] = 0;

	/* improve baud rate error by using 2x clk */
#if F_CPU < 2000000UL && defined(U2X0)
	UCSR0A = _BV(U2X0);
	UBRR0L = (F_CPU / (8UL * USART0_BAUD)) - 1;
#else
	UBRR0L = (F_CPU / (16UL * USART0_BAUD)) - 1;
#endif

	/*! tx enable, 8n1 */
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
	UCSR0B = _BV(TXEN0);
}

/*! Disable the usart port. */
void usart_suspend(const uint8_t port)
{
	UCSR0B = 0;
}

/*! Init the usart port.
 *
 * Allocates the global usart pointer.
 *
 * \note does NOT start the port(s).
 * \warning this should be called only once in the code
 * to avoid memory leak.
 */
void usart_init(void)
{
	/* Initialize the struct */
	usart = malloc(sizeof(struct usart_t));
	usart->tx0_buffer = malloc(USART0_TXBUF_SIZE);
}

/*! Send character c down the USART Tx, wait until tx holding register
 * is empty.
 *
 * \parameter port the serial port.
 * \parameter c the char to send.
 */
void usart_putchar(const uint8_t port, const char c)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}

/*! Send a C (NUL-terminated) string down the USART Tx.
 *
 * \parameter port the serial port.
 * \parameter s the string to send. If there is no string then
 * send the content of the tx_buffer.
 * \note The tx_buffer left unclean.
 */
void usart_printstr(const uint8_t port, const char *s)
{
	/* If no char then print the buffer */
	if (!s)
		s = usart->tx0_buffer;

	while (*s) {
#ifdef DOS_CRLF
		if (*s == '\n')
			usart_putchar(port, '\r');
#endif
		usart_putchar(port, *s++);
	}
}
