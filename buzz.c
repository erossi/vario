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

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "buzz.h"

ISR(TIMER0_COMPB_vect) {
        beeps--;
}

ISR(TIMER0_OVF_vect) {
        beeps--;
}

/*! Play a tone with the buzzer.
 *
 * Using the counter we have:
 * FCPU the cpu speed
 * Prescaler = the counter prescaler
 *
 * Fstep the frequency of a single step of the counter.
 * Fstep = FCPU / Prescaler
 *
 * c is the number of steps of the counter to get to the desired frequency.
 * c = Fstep(hz)/freq(hz)
 *
 * Example for arduino 16Mhz CPU
 * Prescaler = 256
 * Fstep 62.5 Khz
 * to play a 4 Khz wave it has to count:
 * c = 62.5Khz/4Khz = 15.6 steps
 *
 * Example for AtTiny:
 * 8Mhz CPU Prescaler = 64
 * 1Mhz CPU Prescaler = 8
 * Fstep = 125Khz
 * to play a 4Khz wave it has to count:
 * c = 125Khz/4Khz = 31.25 steps
 *
 *                    _         _
 * Duty 90% = _______| |_______| |
 *            COMPB--^ ^--OVF
 *
 * @param freq Frequenzy in Hz
 * @param duty the duty cycle in % (0-100)
 */
void buzz_play(const uint16_t freq, const uint8_t duty)
{
	uint8_t c;

#if (F_CPU == 1000000UL)
	c = (uint8_t)(125000/freq);
	OCR0A = c;
	OCR0B = (uint8_t)(c*duty/100);
	/* Clear counter */
	TCNT0 = 0x00;
	/* Start with prescaler 8 */
	TCCR0B |= _BV(CS01);
#elif (F_CPU == 8000000UL)
	c = (uint8_t)(125000/freq);
	OCR0A = c;
	OCR0B = (uint8_t)(c*duty/100);
	/* Clear counter */
	TCNT0 = 0x00;
	/* Start with prescaler 64 */
	TCCR0B |= _BV(CS01) | _BV(CS00);
#elif (F_CPU == 16000000UL)
	c = (uint8_t)(62500/freq);
	OCR0A = c;
	OCR0B = (uint8_t)(c*duty/100);
	/* Clear counter */
	TCNT0 = 0x00;
	/* Start with prescaler 256 */
	TCCR0B |= _BV(CS02);
#else
#error Counter clock rate unsupported
#endif
}

/*! stop the counter, leave the WGM set */
void buzz_stop(void)
{
	TCCR0B = _BV(WGM02);
}

/*!
 * the Data Direction Register (DDR) bit corresponding to
 * the OC0A and OC0B pins must be set in order to enable the
 * output driver.
 *
 * On Arduino:
 * OC0A PD6
 * OC0B PD5
 *
 * On AtTiny:
 * OC0A PB0
 * OC0B PB1
 */
void buzz_init(void)
{
#ifdef ARDUINO
	DDRD |= _BV(PD5);
	TCCR0A = _BV(COM0B1) | _BV(COM0B0) | _BV(WGM01) | _BV(WGM00);
	TCCR0B = _BV(WGM02);
	/* Ena IRQ Compare Match B and Overflow */
	/* TIMSK0 = _BV(OCIE0B) | _BV(TOIE0);
	*/
#else
	DDRB |= _BV(PB1);
	TCCR0A = _BV(COM0B1) | _BV(COM0B0) | _BV(WGM01) | _BV(WGM00);
	TCCR0B = _BV(WGM02);
	/* Ena IRQ Compare Match B and Overflow */
	/* TIMSK = _BV(OCIE0B) | _BV(TOIE0);
	 */
#endif
}

void buzz_shut(void)
{
#ifdef ARDUINO
        TIMSK0 = 0;
        TCCR0B = 0;
        TCCR0A = 0;
        OCR0A = 0;
        OCR0B = 0;
        DDRD &= ~_BV(PD5);
#else
	TIMSK = 0;
	TCCR0B = 0;
	TCCR0A = 0;
	OCR0A = 0;
	OCR0B = 0;
	DDRB &= ~_BV(PB1);
#endif
}
