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
#include <signal.h>
#include <pthread.h>

#include <global/defs.h>
#include <global/alloc_type.h>
#include <generator/entropy_bundle.h>
#include <generator/entropy_generator.h>
#include <pool/entropy_block.h>
#include <pool/entropy_pool.h>
#include <device/descriptor.h>
#include <device/serial_driver.h>
#include <communication/ssl_init.h>
#include <communication/ssl_context.h>
#include <communication/ssl_descriptor.h>
#include <communication/ssl_server.h>

#define ES_BLOCK_SIZE 65
#define ES_POOL_SIZE 32
#define ES_DEVICE_COUNT 1

struct es_entropy_server_ssl_bundle {
	struct es_ssl_context *context;
	int port;
	es_process_ssl_server_request_function process_request;
};

static struct es_entropy_pool *pool = NULL;
static struct es_entropy_bundle **bundles = NULL;
static struct es_entropy_server_ssl_bundle ssl_bundle;

static void es_signal_handler(int signum)
{
	int i;

	for(i = 0; i < ES_DEVICE_COUNT; ++i) {
		bundles[i]->descriptor->runnable = FALSE;
	}
}

static void es_init_signal_handler(void)
{
	struct sigaction action;

	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = es_signal_handler;

	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);
	sigaction(SIGTSTP, &action, NULL);
}

static void* es_fill_entropy_blocks(void *arg)
{
	struct es_entropy_bundle *bundle = (struct es_entropy_bundle*)arg;

	if(!bundle)
		pthread_exit(NULL);

	return (es_clean_entropy_pool(bundle) != ES_SUCCESS) ? arg : NULL;
}

static void* es_run_ssl_server_thread(void *arg)
{
	struct es_entropy_server_ssl_bundle *bundle =
		(struct es_entropy_server_ssl_bundle*)arg;

	if(!bundle)
		pthread_exit(NULL);

	return (es_run_ssl_server(
			bundle->context,
			bundle->port,
			bundle->process_request) != ES_SUCCESS)
		? arg
		: NULL;
}

static const int es_collect_entropy(
	struct es_entropy_bundle **bundles,
	int bundle_count)
{
	int i;
	int ret = ES_FAILURE;
	pthread_t thread;
	pthread_t *threads = NULL;

	threads = (pthread_t*)malloc((bundle_count + 1) * sizeof(pthread_t));
	if(!threads)
		goto exit;

	for(i = 0; i < bundle_count; ++i) {
		if(pthread_create(&thread, NULL, es_fill_entropy_blocks, bundles[i]))
			goto exit;

		threads[i] = thread;
	}

	if(pthread_create(&thread, NULL, es_run_ssl_server_thread, &ssl_bundle))
		goto exit;

	threads[bundle_count] = thread;
	for(i = 0; i < bundle_count + 1; ++i) {
		if(pthread_join(threads[i], NULL))
			goto exit;
	}

	ret = ES_SUCCESS;

exit:
	if(threads)
		free(threads);
	return ret;
}

static const int process_request(
	const void *in_buff,
	const int in_buff_size,
	void *out_buff,
	int *out_buff_size)
{
	char *temp = NULL;
	temp = malloc(ES_DEFAULT_CONNECTION_BUFFER_SIZE * sizeof(char));
	if(!temp)
		return ES_FAILURE;
	/*printf("Received: %s\n", (const char*)in_buff);
	strcpy((char*)out_buff, "Hello back.");
	*out_buff_size = strlen((char*)out_buff);*/

	if(es_consume_entropy_block(pool, &temp) != ES_SUCCESS)
		return ES_FAILURE;

	strcpy((char*)out_buff, temp);
	*out_buff_size = strlen((char*)out_buff);
	((char*)out_buff)[*out_buff_size - 1] = '\0';

	printf("Sending: %s\nString Length: %d\n", (char*)out_buff, *out_buff_size);

	free(temp);
	return ES_SUCCESS;
}

int main(int argc, char **argv)
{
	int ret = ES_FAILURE;
	int i;
	struct es_device_descriptor *descriptor = NULL;
	struct es_entropy_bundle *bundle = NULL;
	struct es_ssl_context *context = NULL;

	if(argc != 5) {
		printf("Usage: %s <device_port_name> <ssl_port> <cert_file> \
			<key_file>\n",
			argv[0]);
		goto exit;
	}

	pool = es_create_entropy_pool(ES_POOL_SIZE, ES_BLOCK_SIZE, ES_CLEAN_ALLOC);
	if(!pool) {
		perror("Cannot allocate entropy pool.");
		goto exit;
	}

	bundles = (struct es_entropy_bundle**)malloc(
		ES_DEVICE_COUNT * sizeof(struct entropy_bundle*));
	if(!bundles) {
		perror("Cannot allocate bundle collection.");
		goto exit;
	}

	for(i = 0; i < ES_DEVICE_COUNT; ++i) {
		descriptor = es_create_device_descriptor(argv[1], B9600);
		if(!descriptor) {
			perror("Cannot allocate device descriptor.");
			goto exit;
		}

		if(es_init_device(descriptor) != ES_SUCCESS) {
			perror("Cannot init the Arduino board.");
			goto exit;
		}

		bundle = es_create_entropy_bundle(pool, descriptor);
		if(!bundle) {
			perror("Cannot allocate entropy bundle.");
			goto exit;
		}

		bundles[i] = bundle;
	}

	es_ssl_init();

	context = es_create_ssl_context(ES_SSL_SERVER);
	if(!context) {
		perror("Cannot create SSL context.");
		goto exit;
	}

	if(es_configure_server_ssl_context(
			context,
			argv[3],
			argv[4]) != ES_SUCCESS) {
		perror("Cannot configure SSL context.");
		goto exit;
	}

	ssl_bundle.context = context;
	ssl_bundle.port = atoi(argv[2]);
	ssl_bundle.process_request = process_request;

	es_init_signal_handler();

	if(es_collect_entropy(bundles, ES_DEVICE_COUNT) != ES_SUCCESS) {
		perror("Cannot collect entropy from bundles.");
		goto exit;
	}

	ret = ES_SUCCESS;

exit:
	if(pool)
		es_destroy_entropy_pool(&pool);

	if(bundles) {
		for(i = 0; i < ES_DEVICE_COUNT; ++i) {
			if(bundles[i]) {
				if(bundles[i]->descriptor)
					es_destroy_device_descriptor(&bundles[i]->descriptor);

				es_destroy_entropy_bundle(&bundle);
			}
		}
	}

	return ret;
}