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

#include <stdint.h>
#include "i2c_usi.h"

/*! Initialize the i2c bus.
 */
void i2c_init(void)
{
	USI_TWI_Master_Initialise();
}

/*! Shutdown the i2c bus.
 */
void i2c_shut(void)
{
}

/*! i2c Master Trasmitter/Receive Mode.
 *
 * Dependent on the LSB of the address perform both
 * the mtm/mrm functions.
 *
 * \param addr the i2c slave address.
 * \param lenght the number of byte to send or
 * the max lenght the number of byte to receive.
 *
 * \param *data the pointer to the block of byte.
 * \param stop the stop at the end of the communication.
 *
 */
uint8_t i2c_mXm(uint8_t addr, const uint16_t lenght,
		uint8_t *data, uint8_t stop)
{
	uint8_t err;

	err = USI_TWI_Start_Transceiver_With_Data(&addr, 1, FALSE);
	err = err && USI_TWI_Start_Transceiver_With_Data(data, lenght, stop);

	return(err);
}
