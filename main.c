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

volatile uint8_t beeps;

ISR(TIMER0_COMPB_vect) {
        beeps--;
}

ISR(TIMER0_OVF_vect) {
        beeps--;
}

void buzz_play(const uint16_t freq, const uint8_t duty)
{
	uint8_t c;

	/* Fstep = 16Mhz / 256 = 62500hz
	 * 40 step = 62500hz / 40 = 1562.5 hz (Ftop)
	 *                    _         _
	 * Duty 90% = _______| |_______| |
	 *            COMPB--^ ^--OVF
	 */
	c = (uint8_t)(62500/freq);
	OCR0A = c;
	OCR0B = (uint8_t)(c*duty/100);
	/* Clear counter */
	TCNT0 = 0x00;
	/* prescaler 256 */
	TCCR0B |= _BV(CS02);
}

void buzz_stop(void)
{
	/* stop the counter, leave the WGM set */
	TCCR0B = _BV(WGM02);
}

/*!
 * the Data Direction Register (DDR) bit corresponding to
 * the OC0A and OC0B pins must be set in order to enable the
 * output driver.
 */
void buzz_init(void)
{
	/* OC0A PB0
	 * OC0B PB1
	 */
	DDRB |= _BV(PB1);
	TCCR0A = _BV(COM0B1) | _BV(COM0B0) | _BV(WGM01) | _BV(WGM00);
	TCCR0B = _BV(WGM02);
	/* Ena IRQ Compare Match B and Overflow */
	/* TIMSK = _BV(OCIE0B) | _BV(TOIE0);
	 */
}

void buzz_shut(void)
{
	TIMSK = 0;
	TCCR0B = 0;
	TCCR0A = 0;
	OCR0A = 0;
	OCR0B = 0;
	DDRB &= ~_BV(PB1);
}

void beep(uint16_t tone)
{
	uint8_t i;

	buzz_play(tone, 20);
	i = (uint8_t)((3000-tone)/10);
	i++;

	while (i) {
		_delay_ms(10);
		i--;
	}

	buzz_stop();
	i = (uint8_t)((3000-tone)/10);
	i++;

	while (i) {
		_delay_ms(10);
		i--;
	}
}

int main(void)
{
	uint8_t i;
	uint16_t tone;

	buzz_init();

	while(1) {
		for (i=0; i<20; i++) {
			tone = (uint16_t)(1000+i*100);
			beep(tone);
		}
	}

	return(0);
}
