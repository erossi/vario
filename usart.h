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

/*! \file usart.h
 * \brief RS232 - IO functions.
 */

#ifndef _USART_H_
#define _USART_H_

/*! USART 0 baud rate */
#define USART0_BAUD 9600
/*! IO Buffers and masks */
#define USART0_TXBUF_SIZE 64
/*! IO Buffers and masks */
#define USART0_TXBUF_MASK ( USART0_TXBUF_SIZE - 1 )

#ifndef CR
#define CR 0xd
#define LF 0xa
#endif

#ifndef USART0_EOL
#define USART0_EOL CR
#define USART0_EOL_YES
#endif

/*! Dos add CR to an LF */
#define DOS_CRLF

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*! Structure with IO buffers and indexes */
struct usart_t {
	/*! transmit buffer. */
        char *tx0_buffer;
	/*! flags. */
        volatile uint8_t tx0Idx;
};

/*! Filter incoming char per serial port? */
#define USART_CHARFILTER_0
#define USART_CHARFILTER_1

/*! Global USART rxtx buffers pointer used inside the ISR routine. */
volatile struct usart_t *usart;

void usart_resume(const uint8_t port);
void usart_suspend(const uint8_t port);
void usart_init(void);
char usart_getchar(const uint8_t port, const uint8_t locked);
void usart_putchar(const uint8_t port, const char c);
void usart_printstr(const uint8_t port, const char *s);
uint8_t usart_getmsg(const uint8_t port, char *s);
uint8_t usart_getnmsg(const uint8_t port, char *s, const uint8_t size);
void usart_clear_rx_buffer(const uint8_t port);

#endif
