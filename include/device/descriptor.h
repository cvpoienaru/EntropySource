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

#ifndef ENTROPY_SOURCE_DEVICE_DESCRIPTOR_H_
#define ENTROPY_SOURCE_DEVICE_DESCRIPTOR_H_

#include <stdlib.h>
#include <termios.h>

#include <global/defs.h>
#include <device/serial_bundle.h>

/** Represents the definition of a basic device descriptor. */
struct es_device_descriptor {
	/** The file descriptor associated with the connected device. */
	int fd;

	/** TRUE if the device should stop the update loop, FALSE otherwise. */
	int stop;

	/** The serial bundle associated with the connected device. */
	struct es_serial_bundle *serial_bundle;
};

/**
 * Allocates memory for a device descriptor.
 *
 * @param port_name The serial port name that identifies the port to which the
 * device is connected.
 * @param baud_rate The baud rate of the serial port to which the device is
 * connected.
 * @return The address of a newly allocated device descriptor if the operation
 * was successfull, NULL otherwise.
 */
struct es_device_descriptor* es_alloc_device_descriptor(
	const char *port_name,
	const speed_t baud_rate);

/**
 * Frees the memory used by a device descriptor.
 *
 * @param descriptor The device descriptor to be freed.
 */
void es_free_device_descriptor(struct es_device_descriptor **descriptor);

/**
 * Initializes a device descriptor with the default values.
 *
 * @param descriptor The device descriptor to be initialized.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_device_descriptor(struct es_device_descriptor *descriptor);

/**
 * Creates a device descriptor.
 *
 * @param port_name The serial port name that identifies the port to which the
 * device is connected.
 * @param baud_rate The baud rate of the serial port to which the device is
 * connected.
 * @return The address of a newly allocated device descriptor if the operation
 * was successfull, NULL otherwise.
 */
struct es_device_descriptor* es_create_device_descriptor(
	const char *port_name,
	const speed_t baud_rate);

/**
 * Destroys a device descriptor.
 *
 * @param descriptor The device descriptor to be destroyed.
 */
void es_destroy_device_descriptor(struct es_device_descriptor **descriptor);

/**
 * Validates a device descriptor.
 *
 * @param descriptor The device descriptor to be validated.
 */
const int es_validate_device_descriptor(
	struct es_device_descriptor *descriptor);

#endif /* ENTROPY_SOURCE_DEVICE_DESCRIPTOR_H_ */
