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

#ifndef ENTROPY_SOURCE_COMMUNICATION_SSL_DESCRIPTOR_H_
#define ENTROPY_SOURCE_COMMUNICATION_SSL_DESCRIPTOR_H_

#include <stdlib.h>
#include <unistd.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <global/defs.h>
#include <communication/ssl_context.h>

struct es_ssl_descriptor {
	SSL *ssl;
	struct es_ssl_context *context;
};

struct es_ssl_descriptor* es_alloc_ssl_descriptor(
	struct es_ssl_context *context);

void es_free_ssl_descriptor(struct es_ssl_descriptor **descriptor);

const int es_init_ssl_descriptor(
	struct es_ssl_descriptor *descriptor,
	struct es_ssl_context *context);

struct es_ssl_descriptor* es_create_ssl_descriptor(
	struct es_ssl_context *context);

void es_destroy_ssl_descriptor(struct es_ssl_descriptor **descriptor);

const int es_validate_ssl_descriptor(struct es_ssl_descriptor *descriptor);

const int es_get_ssl_fd_descriptor(
	struct es_ssl_descriptor *descriptor,
	int *socket_descriptor);

const int es_set_ssl_fd_descriptor(
	struct es_ssl_descriptor *descriptor,
	const int socket_descriptor);

const int es_accept_ssl_descriptor(struct es_ssl_descriptor *descriptor);

const int es_read_ssl_descriptor(
	struct es_ssl_descriptor *descriptor,
	void *buffer,
	const int size);

const int es_write_ssl_descriptor(
	struct es_ssl_descriptor *descriptor,
	const void *buffer,
	const int size);

const int es_connect_ssl_descriptor(
	struct es_ssl_descriptor *descriptor);

#endif /* ENTROPY_SOURCE_COMMUNICATION_SSL_DESCRIPTOR_H_ */
