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
#include <avr/wdt.h>
#include <util/delay.h>
#include "lps25.h"
#include "buzz.h"

/* 1mt O2 15C = 0.12hPa
 *
 */

/* globals
 * dms is Deci Meters per Second altitude change.
 * ex. dms = 10 -> 1 m/s climbing up
 */
volatile int8_t dms;
// volatile uint8_t button;

/*! IRQ int4 PB4 data ready
 */
ISR(PCINT0_vect)
{
	if (bit_is_set(PINB, PB4))
		lps25_pressure();
}

/*! IRQ Watchdog vector
 */
ISR(WDT_vect)
{
	if (bit_is_set(PINB, PB4))
		lps25_pressure();
}

/*! Single tone beep
 *
 * Used during initialization.
 */
void beep(uint8_t i)
{
	do {
		buzz_play(3000, 50);
		_delay_ms(200);
		buzz_stop();
		_delay_ms(200);
	} while (--i);
}

/*! Main
 *
 * \note: There is the possibility that
 * the voltage is lower than 1.8V where the
 * MCU is powered but the sensor is not.
 */
int main(void)
{
	uint8_t mute = TRUE;

	// Disable Watchdog (not cleared on reset)
	wdt_disable();

	/* wait for the capacitor to charge and
	 * the I2C sensor to be ready (0.5s).
	 */
	_delay_ms(1000);

	// Initialize the buzzer
	buzz_init();

	// Initialize the I2C sensor.
	if (lps25_init())
		// Error
		beep(10);
	else
		// Ok
		beep(2);

	// This will generate an IRQ before we can serve it.
	lps25_fifo_mean_mode();

	// configure IRQ
	GIMSK |= (1 << PCIE);   // pin change interrupt enable
	PCMSK |= (1 << PCINT4); // pin change interrupt enabled for PCINT4

	// Set sleep mode
	set_sleep_mode(SLEEP_MODE_IDLE);

	// Enable watchdog before IRQ, every second
	wdt_enable(WDTO_1S);

	sei();

	// check if an IRQ has been missed.
	if (bit_is_set(PINB, PB4))
		lps25_pressure();

	while(1) {
		// start sleep procedure
		// Wakeup 7 times/sec. depending on
		// the sensor number of samples per second.
		sleep_enable();
		sleep_cpu();
		sleep_disable();

		// Reset the watchdog
		wdt_reset();

		// 1hPa = 8.3m
		// 1m/s = 1/8.3 hPa/s = 0.12 hPa/s
		// 7Hz dms rounded to 0.02 hPa @7Hz
		dms = (-lps25->dHpa) * 500;

		/* About sound:
		 * STATE -> UP, DOWN, FLAT
		 *  UP: beep with changing in freq and period.
		 *  DOWN: sound changing only in freq continuosly.
		 *  FLAT: no sound.
		 *
		 * BEEPS: number of wakeups the sound should continue.
		 *  If 7Hz -> +1ms = 3 cycle sound and 3 cycle no sound.
		 *  Left 1 cycle -> who cares.
		 *  note: if sleep stop sound, then all this is useless.
		 */
		if (((dms > 5) || (dms < -25)) && mute) {
			buzz_play((4000 + dms * 10), 50);
			mute = FALSE;
			// FIXME
			// if sleep stop sound then the
			// software should be delayed here for the
			// amount of time it is suppose to beep.
			_delay_ms(100);
		} else {
			buzz_stop();
			mute = TRUE;
		}
	}

	return(0);
}
