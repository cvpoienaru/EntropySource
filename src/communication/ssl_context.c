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

#include <communication/ssl_context.h>

#include <stdlib.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <global/defs.h>

struct es_ssl_context* es_alloc_ssl_context(const int ssl_type)
{
	int status = ES_FAILURE;
	struct es_ssl_context *context = NULL;

	if(es_validate_ssl_type(ssl_type) != ES_SUCCESS)
		goto exit;

	context = (struct es_ssl_context*)malloc(sizeof(struct es_ssl_context));
	if(!context)
		goto exit;

	const SSL_METHOD *method = (ssl_type == ES_SSL_SERVER)
		? TLSv1_2_server_method()
		: TLSv1_2_client_method();

	if(!method)
		goto exit;

	context->context = SSL_CTX_new(method);
	if(!context->context)
		goto exit;

	status = ES_SUCCESS;

exit:
	if(status == ES_FAILURE && context)
		es_free_ssl_context(&context);

	return context;
}

void es_free_ssl_context(struct es_ssl_context **context)
{
	if(!context || !(*context))
		return;

	if((*context)->context)
		SSL_CTX_free((*context)->context);

	free(*context);
	*context = NULL;
}

const int es_init_ssl_context(
	struct es_ssl_context *context,
	const int ssl_type)
{
	if(!context)
		return ES_FAILURE;

	if(es_validate_ssl_type(ssl_type) != ES_SUCCESS)
		return ES_FAILURE;

	context->type = ssl_type;
	context->runnable = TRUE;

	return ES_SUCCESS;
}

struct es_ssl_context* es_create_ssl_context(const int ssl_type)
{
	int status = ES_FAILURE;
	struct es_ssl_context *context = NULL;

	if(es_validate_ssl_type(ssl_type) != ES_SUCCESS)
		goto exit;

	context = es_alloc_ssl_context(ssl_type);
	if(!context)
		goto exit;

	if(es_init_ssl_context(context, ssl_type) != ES_SUCCESS)
		goto exit;

	status = ES_SUCCESS;

exit:
	if(status == ES_FAILURE && context)
		es_destroy_ssl_context(&context);

	return context;
}

void es_destroy_ssl_context(struct es_ssl_context **context)
{
	es_free_ssl_context(context);
}

const int es_validate_ssl_context(struct es_ssl_context *context)
{
	if(!context)
		return ES_FAILURE;

	if(!context->context)
		return ES_FAILURE;

	if(es_validate_ssl_type(context->type) != ES_SUCCESS)
		return ES_FAILURE;

	return ES_SUCCESS;
}

const int es_validate_ssl_type(const int ssl_type)
{
	switch(ssl_type) {
		case ES_SSL_SERVER:
		case ES_SSL_CLIENT:
			return ES_SUCCESS;

		default:
			return ES_FAILURE;
	}
}

const int es_configure_server_ssl_context(
	struct es_ssl_context *context,
	const char *certificate_name,
	const char *key_name)
{
	if(!context)
		return ES_FAILURE;

	if(es_validate_ssl_context(context) != ES_SUCCESS)
		return ES_FAILURE;

	if(!certificate_name)
		return ES_FAILURE;

	if(!key_name)
		return ES_FAILURE;

	if(SSL_CTX_use_certificate_file(
			context->context,
			certificate_name,
			SSL_FILETYPE_PEM) <= 0)
		return ES_FAILURE;

	if(SSL_CTX_use_PrivateKey_file(
			context->context,
			key_name,
			SSL_FILETYPE_PEM) <= 0)
		return ES_FAILURE;

	if(!SSL_CTX_check_private_key(context->context))
		return ES_FAILURE;

	return ES_SUCCESS;
}
