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

#ifndef ENTROPY_SOURCE_COMMUNICATION_SSL_CONTEXT_H_
#define ENTROPY_SOURCE_COMMUNICATION_SSL_CONTEXT_H_

#include <stdlib.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <global/defs.h>

#define ES_SSL_SERVER 0
#define ES_SSL_CLIENT 1

struct es_ssl_context {
	SSL_CTX *context;
	int type;
	int runnable;
};

struct es_ssl_context* es_alloc_ssl_context(const int ssl_type);

void es_free_ssl_context(struct es_ssl_context **context);

const int es_init_ssl_context(
	struct es_ssl_context *context,
	const int ssl_type);

struct es_ssl_context* es_create_ssl_context(const int ssl_type);

void es_destroy_ssl_context(struct es_ssl_context **context);

const int es_validate_ssl_context(struct es_ssl_context *context);

const int es_validate_ssl_type(const int ssl_type);

const int es_configure_server_ssl_context(
	struct es_ssl_context *context,
	const char *certificate_name,
	const char *key_name);

#endif /* ENTROPY_SOURCE_COMMUNICATION_SSL_CONTEXT_H_ */
