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

#ifndef ENTROPY_SOURCE_COMMUNICATION_SSL_REQUEST_BUNDLE_H_
#define ENTROPY_SOURCE_COMMUNICATION_SSL_REQUEST_BUNDLE_H_

#include <stdlib.h>

#include <global/defs.h>
#include <communication/ssl_defs.h>
#include <communication/ssl_descriptor.h>

struct es_ssl_request_bundle {
	struct es_ssl_descriptor *descriptor;
	es_process_ssl_server_request_function process_request;
};

struct es_ssl_request_bundle* es_alloc_ssl_request_bundle(void);

void es_free_ssl_request_bundle(struct es_ssl_request_bundle **bundle);

const int es_init_ssl_request_bundle(
	struct es_ssl_request_bundle *bundle,
	struct es_ssl_descriptor *descriptor,
	es_process_ssl_server_request_function process_request);

struct es_ssl_request_bundle* es_create_ssl_request_bundle(
	struct es_ssl_descriptor *descriptor,
	es_process_ssl_server_request_function process_request);

void es_destroy_ssl_request_bundle(struct es_ssl_request_bundle **bundle);

const int es_validate_ssl_request_bundle(struct es_ssl_request_bundle *bundle);

#endif /* ENTROPY_SOURCE_COMMUNICATION_SSL_REQUEST_BUNDLE_H_ */
