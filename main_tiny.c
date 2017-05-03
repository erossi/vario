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
#include <avr/sleep.h>
#include <util/delay.h>
#include "lps25.h"
#include "buzz.h"

/* 1mt O2 15C = 0.12hPa
 *
 */

/* global */
volatile int8_t dms;
// volatile uint8_t button;

/*! IRQ int4 PB4 data ready
 */
ISR(PCINT0_vect)
{
	if (bit_is_set(PINB, PB4))
		lps25_pressure();
}

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
	uint8_t mute;

	mute = TRUE;
	buzz_init();

	/* wait for the capacitor to charge and set the
	 * correct i2c address (0.5 sec).
	 */
	_delay_ms(1000);

	if (lps25_init())
		beep(10);
	else
		beep(2);

	// This will generate an IRQ before we can serve it.
	lps25_fifo_mean_mode();

	// configure IRQ
	GIMSK |= (1 << PCIE);   // pin change interrupt enable
	PCMSK |= (1 << PCINT4); // pin change interrupt enabled for PCINT4

	// Set sleep mode
	set_sleep_mode(SLEEP_MODE_IDLE);
	sei();

	// serve the 1st IRQ
	if (bit_is_set(PINB, PB4))
		lps25_pressure();

	while(1) {
		/*
		 * start sleep procedure
		 */
		sleep_enable();
		sleep_cpu();
		sleep_disable();

		// 1hPa = 8.3m
		// 1m/s = 1/8.3 hPa/s = 0.12 hPa/s
		// 7Hz dms rounded to 0.02 hPa @7Hz
		dms = (-lps25->dHpa) * 500;

		if (((dms > 5) || (dms < -25)) && mute) {
			buzz_play((4000 + dms * 10), 50);
			mute = FALSE;
			_delay_ms(100);
		} else {
			buzz_stop();
			mute = TRUE;
		}
	}

	return(0);
}
