/**
 * Copyright (c) 2016, Codrin-Victor Poienaru <cvpoienaru@gmail.com>.
 * All rights reserved.
 *
 * Special thanks for the original design inspiring this module goes to:
 * Tod E. Kurt;
 * Chris Heydrick (also inspired by Tod E. Kurt's design);
 * See acknowledgement file for more info.
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

#include <device/serial_driver.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>

#include <global/defs.h>
#include <global/math_defs.h>
#include <device/serial_bundle.h>
#include <device/descriptor.h>

/**
 * Configures the parameters of the terminal associated with the connected
 * device.
 *
 * @param bundle The serial bundle associated with the connected device.
 * @param fd The file descriptor associated with the connected device.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
static const int es_configure_terminal(
	struct es_serial_bundle *bundle,
	const int fd)
{
	struct termios toptions;

	/* Clear the terminal structure. */
	memset((void*)&toptions, 0, sizeof(struct termios));

	/* Get the options set for the current terminal. */
	if(tcgetattr(fd, &toptions) != 0)
		return ES_FAILURE;

	/* Set the baud rate for the current terminal on the input/output lines. */
	if(cfsetispeed(&toptions, bundle->baud_rate) != 0)
		return ES_FAILURE;

	if(cfsetospeed(&toptions, bundle->baud_rate) != 0)
		return ES_FAILURE;

	/* Set 8 bits for the character size mask, no parity and no stop bits. */
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;

	/* Set no hardware flow control. */
	toptions.c_cflag &= ~CRTSCTS;

	/* Enable the receiver and ignore status lines. */
	toptions.c_cflag |= CREAD | CLOCAL;

	/* Disable the input/output flow control and the restart chars. */
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY);

	/*
	 * Disable the canonical input, echo, visually erase chars and
	 * terminal-generated signals.
	 */
	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	/* Disable the output processing. */
	toptions.c_oflag &= ~OPOST;

	/*
	 * Set the minimum number of characters and the minimum time to wait before
	 * the read system call returns.
	 */
	toptions.c_cc[VMIN] = bundle->vmin;
	toptions.c_cc[VTIME] = bundle->vtime;

	/* Commit the terminal configuration. */
	if(tcsetattr(fd, TCSANOW, &toptions) != 0)
		return ES_FAILURE;

	return ES_SUCCESS;
}

/**
 * Initializes the device serial port.
 *
 * @param bundle The serial bundle associated with the serial port to be
 * initialized.
 * @param fd The file descriptor associated with the serial port to be
 * initialized.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
static const int es_init_device_serial_port(
	struct es_serial_bundle *bundle,
	int *fd)
{
	/* Perform sanity checks. */
	if(!bundle)
		return ES_FAILURE;

	if(!fd)
		return ES_FAILURE;

	if(es_validate_serial_bundle(bundle) != ES_SUCCESS)
		return ES_FAILURE;

	/* Open the file associated with the connected device. */
	*fd = open(bundle->port_name, O_RDWR | O_NOCTTY);
	if(*fd < 0)
		return ES_FAILURE;

	/*
	 * Configure the parameters of the terminal associated with the connected
	 * device.
	 */
	if(es_configure_terminal(bundle, *fd) != ES_SUCCESS)
		return ES_FAILURE;

	return ES_SUCCESS;
}

/**
 * Initializes the device.
 *
 * @param descriptor The device descriptor associated with the connected device.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_device(struct es_device_descriptor *descriptor)
{
	/* Perform sanity checks. */
	if(!descriptor)
		return ES_FAILURE;

	if(es_validate_serial_bundle(descriptor->serial_bundle) != ES_SUCCESS)
		return ES_FAILURE;

	/* Initialize the device serial port. */
	if(es_init_device_serial_port(
			descriptor->serial_bundle,
			&descriptor->fd) != ES_SUCCESS)
		return ES_FAILURE;

	/* Wait for the device to reset. */
	if(usleep(descriptor->serial_bundle->restart_time * 1000) != 0)
		return ES_FAILURE;

	/* Flush anything already in the serial buffer. */
	tcflush(descriptor->fd, TCIFLUSH);

	return ES_SUCCESS;
}

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
	char *buffer)
{
	int ret = ES_FAILURE;
	int buffer_size = 0;
	int copy_size = 0;
	int rbytes = 0;
	char data_transfer_code;
	char *tmp_buffer = NULL;

	/* Perform sanity checks. */
	if(!descriptor)
		goto exit;

	if(es_validate_device_descriptor(descriptor) != ES_SUCCESS)
		goto exit;

	if(!buffer)
		goto exit;

	if(size <= 0)
		goto exit;

	/* Clear the buffer. */
	memset(buffer, 0, size * sizeof(char));

	/* Allocate memory for a temporary buffer. */
	tmp_buffer = (char*)malloc(size * sizeof(char));
	if(!tmp_buffer)
		goto exit;

	/* Begin the data transfer by sending the start transfer code. */
	data_transfer_code = ES_SERIAL_START_TRANSFER_CODE;
	if(write(descriptor->fd, &data_transfer_code, sizeof(char)) < 0)
		goto exit;

	while(TRUE) {
		/* If we have already read the specified number of bits ... */
		if(buffer_size == size - 1) {
			/* Stop the data transfer by sending the end transfer code. */
			data_transfer_code = ES_SERIAL_STOP_TRANSFER_CODE;
			if(write(descriptor->fd, &data_transfer_code, sizeof(char)) < 0)
				goto exit;

			/* Terminate the buffer with a NULL character and exit. */
			buffer[buffer_size] = '\0';
			break;
		}

		/* Clear the temporary buffer. */
		memset(tmp_buffer, 0, size * sizeof(char));

		/* Read data from the device. */
		if((rbytes = read(descriptor->fd, tmp_buffer, size)) < 0)
			goto exit;

		/* Append the read data to the main buffer. */
		copy_size = es_min(rbytes, size - buffer_size - 1);
		buffer_size += copy_size;
		strncat(buffer, tmp_buffer, copy_size);
	}

	/* Update the operation status. */
	ret = ES_SUCCESS;

exit:
	/* If the operation failed, clear the main buffer. */
	if(ret == ES_FAILURE) {
		memset(buffer, 0, size * sizeof(char));
		buffer[0] = '\0';
	}

	/* Clear & free the temporary buffer. */
	if(tmp_buffer) {
		memset(tmp_buffer, 0, size * sizeof(char));
		free(tmp_buffer);
		tmp_buffer = NULL;
	}

	return ret;
}
