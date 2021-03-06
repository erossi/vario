/*
    LPS25H STMicroelectronics MEMS pressure sensor.
    Copyright (C) 2016 Enrico Rossi

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
*/

#ifndef _LPS25_h_
#define _LPS25_h_

#if defined (__AVR_ATtiny85__)
#include "USI_TWI_Master.h"
#define I2C_USI
#else
#include "i2c.h"
#endif

/*
 * addr = 0b101110x[r/w]
 * x = SA0 pin
 * add = 0xb8 or 0xba
 */

#define LPS25_ADDR 0xba

/* Registers */
#define LPS25_REF_P 0x08
#define LPS25_REF_P_XL 0x08
#define LPS25_REF_P_L 0x09
#define LPS25_REF_P_H 0x0A
#define LPS25_WHO_AM_I 0x0F
#define LPS25_RES_CONF 0x10
#define LPS25_CTRL_REG1 0x20
#define LPS25_CTRL_REG2 0x21
#define LPS25_CTRL_REG3 0x22
#define LPS25_CTRL_REG4 0x23
#define LPS25_INTERRUPT_CFG 0x24
#define LPS25_INT_SOURCE 0x25
#define LPS25_STATUS_REG 0x27
#define LPS25_PRESS_OUT 0x28
#define LPS25_PRESS_OUT_XL 0x28
#define LPS25_PRESS_OUT_L 0x29
#define LPS25_PRESS_OUT_H 0x2A
#define LPS25_TEMP_OUT 0x2B
#define LPS25_TEMP_OUT_L 0x2B
#define LPS25_TEMP_OUT_H 0x2C
#define LPS25_FIFO_CTRL 0x2E
#define LPS25_FIFO_STATUS 0x2F
#define LPS25_THS_P 0x30
#define LPS25_THS_P_L 0x30
#define LPS25_THS_P_H 0x31
#define LPS25_RPDS 0x39
#define LPS25_RPDS_L 0x39
#define LPS25_RPDS_H 0x3A

/* Register RES_CONF */
#define LPS25_AVGP0 0
#define LPS25_AVGP1 1
#define LPS25_AVGT0 2
#define LPS25_AVGT1 3
/* Register CTRL_REG1 */
#define LPS25_SIM 0
#define LPS25_RESET_AZ 1
#define LPS25_BDU 2
#define LPS25_DIFF_EN 3
#define LPS25_ODR0 4
#define LPS25_ODR1 5
#define LPS25_ODR2 6
#define LPS25_PD 7
/* Register CTRL_REG2 */
#define LPS25_ONE_SHOT 0
#define LPS25_AUTO_ZERO 1
#define LPS25_SWRESET	2
#define LPS25_FIFO_MEAN_DEC 4
#define LPS25_WTM_EN 5
#define LPS25_FIFO_EN 6
#define LPS25_BOOT 7
/* Register CTRL_REG4 */
#define LPS25_P1_DRDY 0

/* Register FIFO_CTRL */
#define LPS25_WTM_POINT0 0
#define LPS25_WTM_POINT1 1
#define LPS25_WTM_POINT2 2
#define LPS25_WTM_POINT3 3
#define LPS25_WTM_POINT4 4
#define LPS25_F_MODE0 5
#define LPS25_F_MODE1 6
#define LPS25_F_MODE2 7

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

struct lps25_t {
	float temperature;
	float Hpa;
	float dHpa;

	union {
		uint8_t PO[4];
		uint32_t POUT;
	};

	union {
		uint8_t TO[2];
		int16_t TOUT;
	};
};

/* globals */
struct lps25_t *lps25;

uint8_t lps25_init(void);
uint8_t lpslps25_25_shut(void);
uint8_t lpslps25_25_suspend(void);
uint8_t lps25_resume(void);
uint8_t lps25_pressure(void);
uint8_t lps25_temperature(void);
uint8_t lps25_oneshot(void);
void lps25_fifo_mean_mode(void);

#endif
