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
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <resolv.h>
#include <netdb.h>
#include <malloc.h>
#include <errno.h>
#include <fcntl.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <global/defs.h>
#include <communication/ssl_init.h>
#include <communication/ssl_context.h>
#include <communication/ssl_descriptor.h>
#include <communication/ssl_client.h>
#include <communication/messages.h>

#define ES_CLIENT_BUFFER_SIZE 512
#define ES_ENTROPY_FILE_BUFFER_SIZE 2048
#define ES_KERNEL_ENTROPY_POOL "/dev/urandom"

static int es_read_entropy_file(
	const char *entropy_file,
	char *buffer,
	const int buffer_size)
{
	int fd = -1;
	int ret = ES_FAILURE;
	int r_bytes = 0;

	if(!entropy_file)
		goto exit;

	if(!buffer)
		goto exit;

	fd = open(entropy_file, O_RDONLY);
	if(fd < 0)
		goto exit;

	r_bytes = read(fd, buffer, buffer_size);
	if(r_bytes == 0)
		goto exit;

	ret = ES_SUCCESS;

exit:
	if(fd > 0)
		close(fd);

	return ret;
}

static int es_update_kernel_entropy_pool(
	const char *buffer,
	const int buffer_size)
{
	int urandom_fd;
	int w_bytes;

	if(!buffer)
		return ES_FAILURE;

	if(buffer_size < 0)
		return ES_FAILURE;

	urandom_fd = open(ES_KERNEL_ENTROPY_POOL, O_NONBLOCK | O_WRONLY);
	if(urandom_fd < 0)
		return ES_FAILURE;

	w_bytes = 0;
	while(w_bytes != buffer_size) {
		w_bytes += write(urandom_fd, buffer, buffer_size);
	}

	return ES_SUCCESS;
}

int main(int argc, char **argv)
{
	int ret = ES_FAILURE;
	int buffer_size;
	char buffer[ES_CLIENT_BUFFER_SIZE];
	struct es_ssl_context *context = NULL;
	struct es_ssl_descriptor *descriptor = NULL;
	struct es_ssl_descriptor *lb_descriptor = NULL;
	struct es_pair pair;

	if(argc < 3) {
		printf("Usage: %s <hostname> <port> [<entropy_file>]", argv[0]);
		return ES_FAILURE;
	}

	if(argc == 4) {
		memset(buffer, 0, ES_CLIENT_BUFFER_SIZE);

		printf("Read entropy file ... \n");
		if(es_read_entropy_file(
				argv[3],
				buffer,
				ES_CLIENT_BUFFER_SIZE) != ES_SUCCESS)
			goto exit;

		printf("Updating entropy pool with: %s ...\n", buffer);

		buffer_size = strlen(buffer);
		if(es_update_kernel_entropy_pool(buffer, buffer_size) != ES_SUCCESS)
			goto exit;
	}

	es_ssl_init();

	context = es_create_ssl_context(ES_SSL_CLIENT);
	if(!context) {
		perror("Cannot create SSL context.");
		goto exit;
	}

	lb_descriptor = es_create_ssl_descriptor(context);
	if(!lb_descriptor) {
		perror("Cannot create SSL descriptor.");
		goto exit;
	}

	if(es_create_ssl_client(
			lb_descriptor,
			argv[1],
			atoi(argv[2])) != ES_SUCCESS) {
		perror("Cannot create SSL client.");
		goto exit;
	}

	memset(buffer, 0, ES_CLIENT_BUFFER_SIZE);
	strcpy(buffer, "Hello");

	if(es_write_ssl_descriptor(
			lb_descriptor,
			buffer,
			strlen(buffer)) != ES_SUCCESS) {
		perror("Cannot write on SSL socket.");
		goto exit;
	}

	memset(buffer, 0, ES_CLIENT_BUFFER_SIZE);

	if(es_read_ssl_descriptor(
			lb_descriptor,
			buffer,
			ES_CLIENT_BUFFER_SIZE) != ES_SUCCESS) {
		perror("Cannot read from SSL socket.");
		goto exit;
	}

	memcpy(&pair, buffer, sizeof(struct es_pair));

	printf("Connected to load balancer ...\n");
	printf("Received: Hostname = %s; Port = %d;\n", pair.hostname, pair.port);

	descriptor = es_create_ssl_descriptor(context);
	if(!descriptor) {
		perror("Cannot create SSL descriptor.");
		goto exit;
	}

	if(es_create_ssl_client(
			descriptor,
			pair.hostname,
			pair.port) != ES_SUCCESS) {
		perror("Cannot create SSL client.");
		goto exit;
	}

	memset(buffer, 0, ES_CLIENT_BUFFER_SIZE);
	strcpy(buffer, "Hello");

	if(es_write_ssl_descriptor(
			descriptor,
			buffer,
			strlen(buffer)) != ES_SUCCESS) {
		perror("Cannot write on SSL socket.");
		goto exit;
	}

	memset(buffer, 0, ES_CLIENT_BUFFER_SIZE);

	if(es_read_ssl_descriptor(
			descriptor,
			buffer,
			ES_CLIENT_BUFFER_SIZE) != ES_SUCCESS) {
		perror("Cannot read from SSL socket.");
		goto exit;
	}

	buffer_size = strlen(buffer);
	printf("Connected to entropy server ... \n");
	printf("Received: %s\n", buffer);
	printf("Updating entropy pool with: %s ...\n", buffer);
	if(es_update_kernel_entropy_pool(buffer, buffer_size) != ES_SUCCESS)
		goto exit;

	ret = ES_SUCCESS;

exit:
	if(context)
		es_destroy_ssl_context(&context);

	if(descriptor)
		es_destroy_ssl_descriptor(&descriptor);

	return ret;
}
