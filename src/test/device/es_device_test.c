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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include <global/defs.h>
#include <global/conversion.h>
#include <device/serial_bundle.h>
#include <device/descriptor.h>
#include <device/serial_driver.h>

/** Represents the device buffer size for the current instance. */
#define ES_DEVICE_BUFFER_SIZE 65

int main(int argc, char **argv)
{
	int i;
	int buffer_size;
	int ret = ES_FAILURE;
	char buffer[ES_DEVICE_BUFFER_SIZE];
	struct es_device_descriptor *descriptor = NULL;
	const speed_t baud_rate = B9600;

	/* Perform sanity checks. */
	if(argc != 2) {
		printf("Usage: %s <port_name>\n", argv[0]);
		goto exit;
	}

	/* Create a new device descriptor for the connected device. */
	descriptor = es_create_device_descriptor(argv[1], baud_rate);
	if(!descriptor) {
		perror("Cannot create the device descriptor.");
		goto exit;
	}

	/* Initialize the device using de device descriptor. */
	if(es_init_device(descriptor) != ES_SUCCESS) {
		perror("Cannot initialize the device.");
		goto exit;
	}

	/* Clear the buffer. */
	memset(buffer, 0, ES_DEVICE_BUFFER_SIZE);

	/* Read device data. */
	if(es_read_device_data(
			descriptor,
			ES_DEVICE_BUFFER_SIZE,
			buffer) != ES_SUCCESS) {
		perror("Cannot read device data.");
		goto exit;
	}

	/* Print device data information. */
	buffer_size = strlen(buffer);
	printf(
		"Buffer size: %d bytes = %d bits\n",
		buffer_size,
		(int)es_convert_bytes_to_bits(buffer_size));

	printf("Buffer content:\n");
	for(i = 0; i < ES_DEVICE_BUFFER_SIZE; ++i) {
		printf("%c ", buffer[i]);
	}
	printf("\n");

	/* Update the operation status. */
	ret = ES_SUCCESS;

exit:
	/* Destroy the device descriptor. */
	if(descriptor)
		es_destroy_device_descriptor(&descriptor);

	return ret;
}
