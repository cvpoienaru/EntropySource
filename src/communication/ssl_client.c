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

#include <communication/ssl_client.h>

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

static int es_create_client(const char *hostname, const int port)
{
	int sd = -1;
	struct hostent *host;
	struct sockaddr_in addr;

	if(!(host = gethostbyname(hostname)))
		return -1;

	sd = socket(PF_INET, SOCK_STREAM, 0);
	if(sd < 0)
		return -1;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = *(long*)(host->h_addr);
	if(connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
		return -1;

	return sd;
}

int es_create_ssl_client(
	struct es_ssl_descriptor *descriptor,
	const char *hostname,
	const int port)
{
	int sd = -1;

	if(!descriptor)
		return ES_FAILURE;

	if(!hostname)
		return ES_FAILURE;

	if(port <= 0)
		return ES_FAILURE;

	if(es_validate_ssl_descriptor(descriptor) != ES_SUCCESS)
		return ES_FAILURE;

	if((sd = es_create_client(hostname, port)) < 0)
		return ES_FAILURE;

	if(es_set_ssl_fd_descriptor(descriptor, sd) != ES_SUCCESS)
		return ES_FAILURE;

	if(es_connect_ssl_descriptor(descriptor) != ES_SUCCESS)
		return ES_FAILURE;

	return ES_SUCCESS;
}
