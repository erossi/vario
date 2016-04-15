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

#include "i2c.h"

/*
 * addr = 0b101110x[r/w]
 * x = SA0 pin
 * add = 0xb8 or 0xba
 */

#define LPS25_ADDR 0xba

/* Registers */
#define LPS25_R_REF_P_XL 0x08
#define LPS25_R_REF_P_L 0x09
#define LPS25_R_REF_P_H 0x0A
#define LPS25_R_WHO_AM_I 0x0F
#define LPS25_R_RES_CONF 0x10
#define LPS25_R_CTRL_REG1 0x20
#define LPS25_R_CTRL_REG2 0x21
#define LPS25_R_CTRL_REG3 0x22
#define LPS25_R_CTRL_REG4 0x23
#define LPS25_R_INTERRUPT_CFG 0x24
#define LPS25_R_INT_SOURCE 0x25
#define LPS25_R_STATUS_REG 0x27
#define LPS25_R_PRESS_OUT_XL 0x28
#define LPS25_R_PRESS_OUT_L 0x29
#define LPS25_R_PRESS_OUT_H 0x2A
#define LPS25_R_TEMP_OUT_L 0x2B
#define LPS25_R_TEMP_OUT_H 0x2C
#define LPS25_R_FIFO_CTRL 0x2E
#define LPS25_R_FIFO_STATUS 0x2F
#define LPS25_R_THS_P_L 0x30
#define LPS25_R_THS_P_H 0x31
#define LPS25_R_RPDS_L 0x39
#define LPS25_R_RPDS_H 0x3A

/* bit */
#define LPS25_B_PD 7

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

struct lps25_t {
	float temperature;
	float dHpa;
	float Hpa;
	unsigned int altitude;
};

/* globals */
volatile struct lps25_t *lps25;

uint8_t lps25_init(void);
uint8_t lpslps25_25_shut(void);
uint8_t lpslps25_25_suspend(void);
uint8_t lps25_resume(void);

#endif
