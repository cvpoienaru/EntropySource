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

#ifndef ENTROPY_SOURCE_GENERATOR_ENTROPY_GENERATOR_H_
#define ENTROPY_SOURCE_GENERATOR_ENTROPY_GENERATOR_H_

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <global/defs.h>
#include <global/math_defs.h>
#include <collections/queue.h>
#include <generator/entropy_bundle.h>
#include <pool/entropy_block.h>
#include <pool/entropy_pool.h>
#include <device/descriptor.h>
#include <device/serial_driver.h>

/** Represents the read buffer size in bytes. */
#define ES_READ_BUFFER_SIZE 8

/** Represents the device thread sleep time in seconds. */
#define ES_DEVICE_THREAD_SLEEP 1

/** Represents the request thread sleep time in seconds. */
#define ES_REQUEST_THREAD_SLEEP 1

/**
 * Gets the index of a dirty entropy block from the dirty queue.
 *
 * @param pool The pool from which to extract the dirty block index.
 * @return The address of a dirty entropy block index if successfull, NULL
 * otherwise.
 */
int* es_get_dirty_entropy_block_index(struct es_entropy_pool *pool);

/**
 * Gets the index of a clean entropy block from the clean queue.
 *
 * @param pool The pool from where to extract the clean block index.
 * @return The address of a clean entropy block index if successfull, NULL
 * otherwise.
 */
int* es_get_clean_entropy_block_index(struct es_entropy_pool *pool);

/**
 * Consumes a clean entropy block.
 *
 * @param pool The pool from where to extract the clean block to be consumed.
 * @param content The content of the clean entropy block extracted.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_consume_entropy_block(
	struct es_entropy_pool *pool,
	char **content);

/**
 * Cleans the entropy block specified by the given index.
 *
 * @param bundle The entropy bundle associated with the current device thread.
 * @param index The index of the entropy block to be cleaned.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_clean_entropy_block(
	struct es_entropy_bundle *bundle,
	const int index);

/**
 * Cleans the entire entropy pool specified in the entropy bundle parameter.
 *
 * @param bundle The entropy bundle associated with the current device thread.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_clean_entropy_pool(struct es_entropy_bundle *bundle);

#endif /* ENTROPY_SOURCE_GENERATOR_ENTROPY_GENERATOR_H_ */