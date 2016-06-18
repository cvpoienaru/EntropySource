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

#include <device/descriptor.h>

#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include <global/defs.h>
#include <device/serial_bundle.h>

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
	const speed_t baud_rate)
{
	int status = ES_FAILURE;
	struct es_device_descriptor *descriptor = NULL;

	/* Perform sanity checks. */
	if(!port_name)
		goto exit;

	/* Allocate memory for the device descriptor structure. */
	descriptor = (struct es_device_descriptor*)malloc(
		sizeof(struct es_device_descriptor));
	if(!descriptor)
		goto exit;

	/* Create a new serial bundle for the device descriptor. */
	descriptor->serial_bundle = es_create_serial_bundle(port_name, baud_rate);
	if(!descriptor->serial_bundle)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/*
	 * If the operation failed, free the partially allocated device descriptor.
	 */
	if(status == ES_FAILURE && descriptor)
		es_free_device_descriptor(&descriptor);

	return descriptor;
}

/**
 * Frees the memory used by a device descriptor.
 *
 * @param descriptor The device descriptor to be freed.
 */
void es_free_device_descriptor(struct es_device_descriptor **descriptor)
{
	/* Perform sanity checks. */
	if(!descriptor || !(*descriptor))
		return;

	/* If created, destroy the serial bundle. */
	if((*descriptor)->serial_bundle)
		es_destroy_serial_bundle(&(*descriptor)->serial_bundle);

	/* Free the device descriptor structure. */
	free(*descriptor);
	*descriptor = NULL;
}

/**
 * Initializes a device descriptor with the default values.
 *
 * @param descriptor The device descriptor to be initialized.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_device_descriptor(struct es_device_descriptor *descriptor)
{
	/* Perform sanity checks. */
	if(!descriptor)
		return ES_FAILURE;

	/* Initialize the structure fields with their default values. */
	descriptor->fd = ES_DEFAULT_DESCRIPTOR;
	descriptor->runnable = TRUE;

	return ES_SUCCESS;
}

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
	const speed_t baud_rate)
{
	int status = ES_FAILURE;
	struct es_device_descriptor *descriptor = NULL;

	/* Perform sanity checks. */
	if(!port_name)
		goto exit;

	/* Allocate memory for the new device descriptor. */
	descriptor = es_alloc_device_descriptor(port_name, baud_rate);
	if(!descriptor)
		goto exit;

	/* Initialize the device descriptor fields with their default values. */
	if(es_init_device_descriptor(descriptor) != ES_SUCCESS)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/*
	 * If the operation failed, destroy the partially created device
	 * descriptor.
	 */
	if(status == ES_FAILURE && descriptor)
		es_destroy_device_descriptor(&descriptor);

	return descriptor;
}

/**
 * Destroys a device descriptor.
 *
 * @param descriptor The device descriptor to be destroyed.
 */
void es_destroy_device_descriptor(struct es_device_descriptor **descriptor)
{
	/* Free the given device descriptor. */
	es_free_device_descriptor(descriptor);
}

/**
 * Validates a device descriptor.
 *
 * @param descriptor The device descriptor to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_device_descriptor(
	struct es_device_descriptor *descriptor)
{
	/* Perform sanity checks. */
	if(!descriptor)
		return ES_FAILURE;

	/* Perform field validation. */
	if(es_validate_serial_bundle(descriptor->serial_bundle) != ES_SUCCESS)
		return ES_FAILURE;

	if(descriptor->fd < 0)
		return ES_FAILURE;

	if(descriptor->stop != TRUE && descriptor->stop != FALSE)
		return ES_FAILURE;

	return ES_SUCCESS;
}
