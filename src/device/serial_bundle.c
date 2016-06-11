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

#include <device/serial_bundle.h>

#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include <global/defs.h>

/**
 * Allocates memory for a serial bundle.
 *
 * @param port_name The serial port name that identifies the port to which the
 * device is connected.
 * @return The address of a newly allocated serial bundle if the operation was
 * successfull, NULL otherwise.
 */
struct es_serial_bundle* es_alloc_serial_bundle(
	const char *port_name)
{
	int status = ES_FAILURE;
	struct es_serial_bundle *bundle = NULL;

	/* Perform sanity checks. */
	if(!port_name)
		goto exit;

	/* Allocate memory for the serial bundle structure. */
	bundle = (struct es_serial_bundle*)malloc(sizeof(struct es_serial_bundle));
	if(!bundle)
		goto exit;

	/* Allocate memory for the serial bundle port name. */
	bundle->port_name = strdup(port_name);
	if(!bundle->port_name)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, free the partially allocated serial bundle. */
	if(status == ES_FAILURE && bundle)
		es_free_serial_bundle(&bundle);

	return bundle;
}

/**
 * Frees the memory used by a serial bundle.
 *
 * @param bundle The serial bundle to be freed.
 */
void es_free_serial_bundle(struct es_serial_bundle **bundle)
{
	/* Perform sanity checks. */
	if(!bundle || !(*bundle))
		return;

	/* If allocated, free the serial bundle port name. */
	if((*bundle)->port_name)
		free((*bundle)->port_name);

	/* Free the serial bundle structure. */
	free(*bundle);
	*bundle = NULL;
}

/**
 * Initializes a serial bundle with the default values.
 *
 * @param bundle The bundle to be initialized.
 * @param baud_rate The baud rate of the serial port to which the device is
 * connected.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
int es_init_serial_bundle(
	struct es_serial_bundle *bundle,
	const speed_t baud_rate)
{
	/* Perform sanity checks. */
	if(!bundle)
		return ES_FAILURE;

	/* Initialize the structure fields with their default values. */
	bundle->baud_rate = baud_rate;
	bundle->vmin = ES_SERIAL_VMIN;
	bundle->vtime = ES_SERIAL_VTIME;
	bundle->restart_time = ES_SERIAL_RESTART_TIME;

	return ES_SUCCESS;
}

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
	const speed_t baud_rate)
{
	int status = ES_FAILURE;
	struct es_serial_bundle *bundle = NULL;

	/* Perform sanity checks. */
	if(!port_name)
		goto exit;

	/* Allocate memory for the new serial bundle. */
	bundle = es_alloc_serial_bundle(port_name);
	if(!bundle)
		goto exit;

	/* Initialize the serial bundle fields with their default values. */
	if(es_init_serial_bundle(bundle, baud_rate) != ES_SUCCESS)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, free the partially created serial bundle. */
	if(status == ES_FAILURE && bundle)
		es_destroy_serial_bundle(&bundle);

	return bundle;
}

/**
 * Destroys a serial bundle.
 *
 * @param bundle The serial bundle to be destroyed.
 */
void es_destroy_serial_bundle(struct es_serial_bundle **bundle)
{
	/* Free the given serial bundle. */
	es_free_serial_bundle(bundle);
}

/**
 * Validates a serial bundle.
 *
 * @param bundle The serial bundle to be validated.
 */
int es_validate_serial_bundle(struct es_serial_bundle *bundle)
{
	/* Perform sanity checks. */
	if(!bundle)
		return ES_FAILURE;

	/* Perform field validation. */
	if(!bundle->port_name)
		return ES_FAILURE;

	if(bundle->vmin < 0)
		return ES_FAILURE;

	if(bundle->vtime < 0)
		return ES_FAILURE;

	if(bundle->restart_time < 0)
		return ES_FAILURE;

	return ES_SUCCESS;
}
