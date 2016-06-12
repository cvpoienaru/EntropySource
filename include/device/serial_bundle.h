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

#ifndef ENTROPY_SOURCE_DEVICE_SERIAL_BUNDLE_H_
#define ENTROPY_SOURCE_DEVICE_SERIAL_BUNDLE_H_

#include <stdlib.h>
#include <termios.h>

#include <global/defs.h>

/**
 * Represents the minimum number of bytes to be read until the system call
 * returns.
 * Multiply the value by 8 to get the value in bits.
 */
#define ES_SERIAL_VMIN 8

/** Represents the minimum waiting time until the read system call returns. */
#define ES_SERIAL_VTIME 0

/** Represents the restart time for the device connected to the serial port. */
#define ES_SERIAL_RESTART_TIME 2000

/** Represents the definition of a basic serial bundle. */
struct es_serial_bundle {
	/**
	 * The serial port name that identifies the port to which the device is
	 * connected.
	 */
	char *port_name;

	/** The baud rate of the serial port to which the device is connected. */
	speed_t baud_rate;

	/**
	 * The minimum number of characters to be read until the system call
	 * returns.
	 */
	int vmin;

	/** The minimum waiting time until the read system call returns. */
	int vtime;

	/** The restart time for the device connected to the serial port. */
	int restart_time;
};

/**
 * Allocates memory for a serial bundle.
 *
 * @param port_name The serial port name that identifies the port to which the
 * device is connected.
 * @return The address of a newly allocated serial bundle if the operation was
 * successfull, NULL otherwise.
 */
struct es_serial_bundle* es_alloc_serial_bundle(
	const char *port_name);

/**
 * Frees the memory used by a serial bundle.
 *
 * @param bundle The serial bundle to be freed.
 */
void es_free_serial_bundle(struct es_serial_bundle **bundle);

/**
 * Initializes a serial bundle with the default values.
 *
 * @param bundle The bundle to be initialized.
 * @param baud_rate The baud rate of the serial port to which the device is
 * connected.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_serial_bundle(
	struct es_serial_bundle *bundle,
	const speed_t baud_rate);

/**
 * Creates a serial bundle.
 *
 * @param port_name The serial port name that identifies the port to which the
 * device is connected.
 * @param baud_rate The baud rate of the serial port to which the device is
 * connected.
 * @return The address of a newly allocated serial bundle if the operation was
 * successfull, NULL otherwise.
 */
struct es_serial_bundle* es_create_serial_bundle(
	const char *port_name,
	const speed_t baud_rate);

/**
 * Destroys a serial bundle.
 *
 * @param bundle The serial bundle to be destroyed.
 */
void es_destroy_serial_bundle(struct es_serial_bundle **bundle);

/**
 * Validates a serial bundle.
 *
 * @param bundle The serial bundle to be validated.
 */
const int es_validate_serial_bundle(struct es_serial_bundle *bundle);

#endif /* ENTROPY_SOURCE_DEVICE_SERIAL_BUNDLE_H_ */
