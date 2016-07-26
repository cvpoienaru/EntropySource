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

#include <communication/ssl_descriptor.h>

#include <stdlib.h>
#include <unistd.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <global/defs.h>
#include <communication/ssl_context.h>

struct es_ssl_descriptor* es_alloc_ssl_descriptor(
	struct es_ssl_context *context)
{
	int status = ES_FAILURE;
	struct es_ssl_descriptor *descriptor = NULL;

	if(!context)
		goto exit;

	if(es_validate_ssl_context(context) != ES_SUCCESS)
		goto exit;

	descriptor = (struct es_ssl_descriptor*)malloc(
		sizeof(struct es_ssl_descriptor));
	if(!descriptor)
		goto exit;

	descriptor->ssl = SSL_new(context->context);
	if(!descriptor->ssl)
		goto exit;

	status = ES_SUCCESS;

exit:
	if(status == ES_FAILURE && descriptor)
		es_free_ssl_descriptor(&descriptor);

	return descriptor;
}

void es_free_ssl_descriptor(struct es_ssl_descriptor **descriptor)
{
	if(!descriptor || !(*descriptor))
		return;

	if((*descriptor)->ssl)
		SSL_free((*descriptor)->ssl);

	free(*descriptor);
	*descriptor = NULL;
}

const int es_init_ssl_descriptor(
	struct es_ssl_descriptor *descriptor,
	struct es_ssl_context *context)
{
	if(!descriptor)
		return ES_FAILURE;

	if(!context)
		return ES_FAILURE;

	if(es_validate_ssl_context(context) != ES_SUCCESS)
		return ES_FAILURE;

	descriptor->context = context;

	return ES_SUCCESS;
}

struct es_ssl_descriptor* es_create_ssl_descriptor(
	struct es_ssl_context *context)
{
	int status = ES_FAILURE;
	struct es_ssl_descriptor *descriptor = NULL;

	if(!context)
		goto exit;

	if(es_validate_ssl_context(context) != ES_SUCCESS)
		goto exit;

	descriptor = es_alloc_ssl_descriptor(context);
	if(!descriptor)
		goto exit;

	if(es_init_ssl_descriptor(descriptor, context) != ES_SUCCESS)
		goto exit;

	status = ES_SUCCESS;

exit:
	if(status == ES_FAILURE && descriptor)
		es_destroy_ssl_descriptor(&descriptor);

	return descriptor;
}

void es_destroy_ssl_descriptor(struct es_ssl_descriptor **descriptor)
{
	int socket_descriptor = ES_DEFAULT_DESCRIPTOR;

	if(!descriptor || !(*descriptor))
		return;

	if(es_validate_ssl_descriptor(*descriptor) == ES_SUCCESS) {
		if(es_get_ssl_fd_descriptor(
				*descriptor,
				&socket_descriptor) == ES_SUCCESS)
			close(socket_descriptor);
	}

	es_free_ssl_descriptor(descriptor);
}

const int es_validate_ssl_descriptor(struct es_ssl_descriptor *descriptor)
{
	if(!descriptor)
		return ES_FAILURE;

	if(!descriptor->ssl)
		return ES_FAILURE;

	if(!descriptor->context)
		return ES_FAILURE;

	if(es_validate_ssl_context(descriptor->context) != ES_SUCCESS)
		return ES_FAILURE;

	return ES_SUCCESS;
}

const int es_get_ssl_fd_descriptor(
	struct es_ssl_descriptor *descriptor,
	int *socket_descriptor)
{
	*socket_descriptor = ES_DEFAULT_DESCRIPTOR;

	if(!descriptor)
		return ES_FAILURE;

	if(es_validate_ssl_descriptor(descriptor) != ES_SUCCESS)
		return ES_FAILURE;

	if((*socket_descriptor = SSL_get_fd(descriptor->ssl)) < 0)
		return ES_FAILURE;

	return ES_SUCCESS;
}

const int es_set_ssl_fd_descriptor(
	struct es_ssl_descriptor *descriptor,
	const int socket_descriptor)
{
	if(!descriptor)
		return ES_FAILURE;

	if(es_validate_ssl_descriptor(descriptor) != ES_SUCCESS)
		return ES_FAILURE;

	if(socket_descriptor < 0)
		return ES_FAILURE;

	if(SSL_set_fd(descriptor->ssl, socket_descriptor) != 1)
		return ES_FAILURE;

	return ES_SUCCESS;
}

const int es_accept_ssl_descriptor(struct es_ssl_descriptor *descriptor)
{
	if(!descriptor)
		return ES_FAILURE;

	if(es_validate_ssl_descriptor(descriptor) != ES_SUCCESS)
		return ES_FAILURE;

	if(SSL_accept(descriptor->ssl) < 0)
		return ES_FAILURE;

	return ES_SUCCESS;
}

const int es_read_ssl_descriptor(
	struct es_ssl_descriptor *descriptor,
	void *buffer,
	const int size)
{
	if(!descriptor)
		return ES_FAILURE;

	if(es_validate_ssl_descriptor(descriptor) != ES_SUCCESS)
		return ES_FAILURE;

	if(!buffer)
		return ES_FAILURE;

	if(size < 0)
		return ES_FAILURE;

	if(SSL_read(descriptor->ssl, buffer, size) <= 0)
		return ES_FAILURE;

	return ES_SUCCESS;
}

const int es_write_ssl_descriptor(
	struct es_ssl_descriptor *descriptor,
	const void *buffer,
	const int size)
{
	if(!descriptor)
		return ES_FAILURE;

	if(es_validate_ssl_descriptor(descriptor) != ES_SUCCESS)
		return ES_FAILURE;

	if(!buffer)
		return ES_FAILURE;

	if(size < 0)
		return ES_FAILURE;

	if(SSL_write(descriptor->ssl, buffer, size) <= 0)
		return ES_FAILURE;

	return ES_SUCCESS;
}

const int es_connect_ssl_descriptor(
	struct es_ssl_descriptor *descriptor)
{
	if(!descriptor)
		return ES_FAILURE;

	if(es_validate_ssl_descriptor(descriptor) != ES_SUCCESS)
		return ES_FAILURE;

	if(SSL_connect(descriptor->ssl) < 0)
		return ES_FAILURE;

	return ES_SUCCESS;
}
