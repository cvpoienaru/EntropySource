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

#include <communication/ssl_server.h>

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include <pthread.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <global/defs.h>
#include <communication/ssl_defs.h>
#include <communication/ssl_context.h>
#include <communication/ssl_descriptor.h>

static const int es_create_ssl_server_listener(const int port, int *listener_d)
{
	int ret = ES_FAILURE;
	struct sockaddr_in addr;

	*listener_d = ES_DEFAULT_DESCRIPTOR;

	if(port < 0)
		goto exit;

	memset(&addr, 0, sizeof(addr));

	*listener_d = socket(PF_INET, SOCK_STREAM, 0);
	if(*listener_d < 0)
		goto exit;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(*listener_d, (struct sockaddr*)&addr, sizeof(addr)) != 0)
		goto exit;

	if(listen(*listener_d, ES_DEFAULT_BACKLOG_SIZE) != 0)
		goto exit;

	ret = ES_SUCCESS;

exit:
	if(ret == ES_FAILURE && *listener_d > 0)
		close(*listener_d);

	return ret;
}

static const int es_accept_ssl_server_connection(
	struct es_ssl_context *context,
	const int listener_d,
	struct es_ssl_descriptor **descriptor)
{
	int ret = ES_FAILURE;
	int client_d = ES_DEFAULT_DESCRIPTOR;
	socklen_t addr_len;
	struct sockaddr_in addr;

	if(!context)
		goto exit;

	if(es_validate_ssl_context(context) != ES_SUCCESS)
		goto exit;

	if(listener_d < 0)
		goto exit;

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr_len = sizeof(addr);

	client_d = accept(listener_d, (struct sockaddr*)&addr, &addr_len);
	if(client_d < 0)
		goto exit;

	*descriptor = es_create_ssl_descriptor(context);
	if(!*descriptor)
		goto exit;

	if(es_set_ssl_fd_descriptor(*descriptor, client_d) != ES_SUCCESS)
		goto exit;

	ret = ES_SUCCESS;

exit:
	if(ret == ES_FAILURE) {
		if(*descriptor)
			es_destroy_ssl_descriptor(descriptor);

		if(client_d > 0)
			close(client_d);
	}

	return ret;
}

static const int es_handle_ssl_request(
	struct es_ssl_descriptor *descriptor,
	es_process_ssl_server_request_function process_request)
{
	int ret = ES_FAILURE;
	char in_buffer[ES_DEFAULT_CONNECTION_BUFFER_SIZE];
	char out_buffer[ES_DEFAULT_CONNECTION_BUFFER_SIZE];
	int out_buffer_size = 0;

	if(!descriptor)
		goto exit;

	if(es_validate_ssl_descriptor(descriptor) != ES_SUCCESS)
		goto exit;

	if(!process_request)
		goto exit;

	if(es_accept_ssl_descriptor(descriptor) != ES_SUCCESS)
		goto exit;

	memset(in_buffer, 0, ES_DEFAULT_CONNECTION_BUFFER_SIZE);
	memset(out_buffer, 0, ES_DEFAULT_CONNECTION_BUFFER_SIZE);

	if(es_read_ssl_descriptor(
			descriptor,
			in_buffer,
			ES_DEFAULT_CONNECTION_BUFFER_SIZE) != ES_SUCCESS)
		goto exit;

	if(process_request(
			in_buffer,
			strlen(in_buffer),
			out_buffer,
			&out_buffer_size) != ES_SUCCESS)
		goto exit;

	if(es_write_ssl_descriptor(
			descriptor,
			out_buffer,
			out_buffer_size) != ES_SUCCESS)
		goto exit;

	ret = ES_SUCCESS;

exit:
	memset(in_buffer, 0, ES_DEFAULT_CONNECTION_BUFFER_SIZE);
	memset(out_buffer, 0, ES_DEFAULT_CONNECTION_BUFFER_SIZE);

	return ret;
}

const int es_run_ssl_server(
	struct es_ssl_context *context,
	const int port,
	es_process_ssl_server_request_function process_request)
{
	int listener_d;
	struct es_ssl_descriptor *descriptor = NULL;

	if(port < 0)
		goto exit;

	if(es_create_ssl_server_listener(port, &listener_d) != ES_SUCCESS)
		goto exit;

	if(listener_d < 0)
		goto exit;

	while(TRUE) {
		if(!context->runnable)
			break;

		if(es_accept_ssl_server_connection(
				context,
				listener_d,
				&descriptor) != ES_SUCCESS)
			goto exit;

		if(es_handle_ssl_request(descriptor, process_request) != ES_SUCCESS)
			goto exit;

		es_destroy_ssl_descriptor(&descriptor);
	}

exit:
	return ES_SUCCESS;
}
