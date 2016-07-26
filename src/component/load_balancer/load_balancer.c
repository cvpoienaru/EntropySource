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
#include <collections/hashtable.h>
#include <communication/ssl_init.h>
#include <communication/ssl_context.h>
#include <communication/ssl_descriptor.h>
#include <communication/ssl_server.h>
#include <communication/messages.h>

#define ES_CLIENT_BUFFER_SIZE 512
#define ES_ENTROPY_SERVER_HOSTNAME "127.0.0.1"
#define ES_ENTROPY_SERVER_PORT 10105
#define ES_ENTROPY_SERVER_BLOCK_COUNT 32

static const int process_request(
	const void *in_buff,
	const int in_buff_size,
	void *out_buff,
	int *out_buff_size)
{
	struct es_pair pair;

	strcpy(pair.hostname, ES_ENTROPY_SERVER_HOSTNAME);
	pair.port = ES_ENTROPY_SERVER_PORT;
	pair.block_count = ES_ENTROPY_SERVER_BLOCK_COUNT;

	memcpy(out_buff, &pair, sizeof(struct es_pair));
	*out_buff_size = sizeof(struct es_pair);

	printf("Sending: Hostname = %s; Port = %d;\n", pair.hostname, pair.port);

	return ES_SUCCESS;
}

int main(int argc, char **argv)
{
	int ret = ES_FAILURE;
	struct es_ssl_context *context = NULL;

	if(argc != 4) {
		printf("Usage: %s <ssl_port> <cert_file> <key_file>\n", argv[0]);
		goto exit;
	}

	es_ssl_init();

	context = es_create_ssl_context(ES_SSL_SERVER);
	if(!context) {
		perror("Cannot create SSL context.");
		goto exit;
	}

	if(es_configure_server_ssl_context(
			context,
			argv[2],
			argv[3]) != ES_SUCCESS) {
		perror("Cannot configure SSL context.");
		goto exit;
	}

	if(es_run_ssl_server(
			context,
			atoi(argv[1]),
			process_request) != ES_SUCCESS) {
		perror("Cannot run SSL server.");
		goto exit;
	
	}

	ret = ES_SUCCESS;

exit:
	if(context)
		es_destroy_ssl_context(&context);

	return ret;
}