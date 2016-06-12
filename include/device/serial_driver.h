/**
 * Copyright (c) 2016, Codrin-Victor Poienaru <cvpoienaru@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * This software is provided by the copyright holders and contributors "as is"
 * and any express or implied warranties, including, but not limited to, the
 * implied warranties of merchantability and fitness for a particular purpose
 * are disclaimed. In no event shall the copyright holder or contributors be
 * liable for any direct, indirect, incidental, special, exemplary, or
 * consequential damages (including, but not limited to, procurement of
 * substitute goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether in
 * contract, strict liability, or tort (including negligence or otherwise)
 * arising in any way out of the use of this software, even if advised of the
 * possibility of such damage.
 */

#ifndef ENTROPY_SOURCE_DEVICE_SERIAL_DRIVER_H_
#define ENTROPY_SOURCE_DEVICE_SERIAL_DRIVER_H_

#include <stdlib.h>
#include <termios.h>

#include <global/defs.h>
#include <device/serial_bundle.h>
#include <device/descriptor.h>

/** Represents a special code marking the start of the serial data transfer. */
#define ES_SERIAL_START_TRANSFER_CODE '^'

/** Represents a special code marking the end of the serial data transfer. */
#define ES_SERIAL_STOP_TRANSFER_CODE '$'

/**
 * Initializes the device.
 *
 * @param descriptor The device descriptor associated with the connected device.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_device(struct es_device_descriptor *descriptor);

/**
 * Reads data from the device.
 *
 * @param descriptor The device descriptor associated with the connected device.
 * @param size The number of bytes to read from the device.
 * @param buffer The buffer where to store the data read from the device.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_read_device_data(
	struct es_device_descriptor *descriptor,
	const int size,
	char *buffer);

#endif /* ENTROPY_SOURCE_DEVICE_SERIAL_DRIVER_H_ */
