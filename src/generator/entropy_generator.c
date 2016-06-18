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

#include <generator/entropy_generator.h>

#include <stdlib.h>
#include <stdio.h>
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

/**
 * Gets the index of an entropy block from either the dirty queue or the clean
 * queue with respect to the specified block state.
 *
 * @param pool The entropy pool from which to extract the entropy block index.
 * @param state The desired entropy block state.
 * @return The address of an entropy block index found in the specified block
 * state if successfull, NULL otherwise.
 */
static int* es_get_entropy_block_index(
	struct es_entropy_pool *pool,
	const int state)
{
	int *index = NULL;
	struct es_queue *queue = NULL;

	/* Perform sanity checks. */
	if(!pool)
		return index;

	if(es_validate_entropy_pool(pool) != ES_SUCCESS)
		return index;

	/* Select the desired queue to perform the operation. */
	switch(state) {
		case ES_CLEAN_BLOCK_STATE:
			queue = pool->clean_queue;
			break;

		case ES_DIRTY_BLOCK_STATE:
			queue = pool->dirty_queue;
			break;
	}

	/* Atomic queue extract operation. */
	pthread_mutex_lock(&pool->mutex);
	if(!es_check_queue_is_empty(queue))
		index = es_pop_queue(queue);
	pthread_mutex_unlock(&pool->mutex);

	return index;
}

/**
 * Gets the index of a dirty entropy block from the dirty queue.
 *
 * @param pool The pool from which to extract the dirty block index.
 * @return The address of a dirty entropy block index if successfull, NULL
 * otherwise.
 */
int* es_get_dirty_entropy_block_index(struct es_entropy_pool *pool)
{
	/* Get the index of a dirty entropy block from the dirty queue. */
	return es_get_entropy_block_index(pool, ES_DIRTY_BLOCK_STATE);
}

/**
 * Gets the index of a clean entropy block from the clean queue.
 *
 * @param pool The pool from where to extract the clean block index.
 * @return The address of a clean entropy block index if successfull, NULL
 * otherwise.
 */
int* es_get_clean_entropy_block_index(struct es_entropy_pool *pool)
{
	/* Get the index of a clean entropy block from the clean queue. */
	return es_get_entropy_block_index(pool, ES_CLEAN_BLOCK_STATE);
}

/**
 * Consumes a clean entropy block.
 *
 * @param pool The pool from where to extract the clean block to be consumed.
 * @param content The content of the clean entropy block extracted.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_consume_entropy_block(struct es_entropy_pool *pool, char **content)
{
	int status;
	int *index = NULL;
	struct es_entropy_block *block = NULL;

	/* The default content value when exiting should be null. */
	*content = NULL;

	/* Perform sanity checks. */
	if(!pool)
		return ES_FAILURE;

	if(es_validate_entropy_pool(pool) != ES_SUCCESS)
		return ES_FAILURE;

	/*
	 * Blocking call simulation for clean entropy block extraction.
	 * At a given point in time it is possible that no clean entropy block is
	 * available, so we wait until one becomes.
	 */
	while(TRUE) {
		/* Extract a clean entropy block index. */
		index = es_get_clean_entropy_block_index(pool);
		if(index)
			break;

		/* If no such index was found, sleep for a predefined amount of time. */
		sleep(ES_REQUEST_THREAD_SLEEP);
	}

	block = pool->blocks[*index];

	/* Atomic entropy block content request operation. */
	pthread_mutex_lock(&block->mutex);
	status = es_request_entropy_block_content(block, content);
	pthread_mutex_unlock(&block->mutex);

	/* Atomic queue push operation. */
	pthread_mutex_lock(&pool->mutex);
	if(status != ES_SUCCESS) {
		/* Something went very wrong ... */
		if(block)
			es_destroy_entropy_block(&block);

		free(index);
	} else {
		es_push_queue(pool->dirty_queue, index);
	}
	pthread_mutex_unlock(&pool->mutex);

	return status;
}

/**
 * Cleans the entropy block specified by the given index.
 *
 * @param bundle The entropy bundle associated with the current device thread.
 * @param index The index of the entropy block to be cleaned.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_clean_entropy_block(
	struct es_entropy_bundle *bundle,
	const int index)
{
	int ret = ES_SUCCESS;
	char buffer[ES_READ_BUFFER_SIZE];
	struct es_entropy_block *block = NULL;

	/* Perform sanity checks. */
	if(!bundle)
		return ES_FAILURE;
	
	if(index < 0)
		return ES_FAILURE;

	block = bundle->pool->blocks[index];

	/* Atomic block cleaning operation. */
	pthread_mutex_lock(&block->mutex);
	while(block->state == ES_DIRTY_BLOCK_STATE) {
		/* Clear the reading buffer. */
		memset(buffer, 0, ES_READ_BUFFER_SIZE);

		/* Read data from the device. */
		if(es_read_device_data(
				bundle->descriptor,
				ES_READ_BUFFER_SIZE,
				buffer) != ES_SUCCESS) {
			ret = ES_FAILURE;
			break;
		}

		/* Update the entropy block content. */
		if(es_update_entropy_block_content(block, buffer) != ES_SUCCESS) {
			ret = ES_FAILURE;
			break;
		}
	}
	pthread_mutex_unlock(&block->mutex);

	return ret;
}

/**
 * Cleans the entire entropy pool specified in the entropy bundle parameter.
 *
 * @param bundle The entropy bundle associated with the current device thread.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_clean_entropy_pool(struct es_entropy_bundle *bundle)
{
	int ret = ES_FAILURE;
	int *index = NULL;
	struct es_entropy_block *block = NULL;

	/* Perform sanity checks. */
	if(!bundle)
		return ret;

	while(TRUE) {
		/* Checks if the current device thread should stop gracefully. */
		if(!bundle->descriptor->runnable)
			break;

		/* Extract a dirty entropy block index from the dirty queue. */
		index = es_get_dirty_entropy_block_index(bundle->pool);

		if(index) {
			/* Clean the entropy block indentified by the extracted index. */
			ret = es_clean_entropy_block(bundle, *index);

			/* Atomic queue push operation. */
			pthread_mutex_lock(&bundle->pool->mutex);
			if(ret != ES_SUCCESS) {
				/* Something went very wrong ... */
				block = bundle->pool->blocks[*index];
				if(block)
					es_destroy_entropy_block(&block);

				free(index);
			} else {
				es_push_queue(bundle->pool->clean_queue, index);
			}
			pthread_mutex_unlock(&bundle->pool->mutex);

			if(ret != ES_SUCCESS)
				continue;

			if(ES_DEBUG) {
				printf(
					"Entropy block %d size: %d bytes\n",
					*index,
					strlen(bundle->pool->blocks[*index]->content));
				printf(
					"Entropy block %d content:\n%s\n",
					*index,
					bundle->pool->blocks[*index]->content);
			}
		} else {
			/* No blocks to be cleaned were found. */
			/* TODO: Cache some device readings in this case. */

			/* Sleep until some dirty blocks become available. */
			if(ES_DEBUG) {
				printf("All blocks are clean. Nothing to do ... Sleep\n");
			}

			sleep(ES_DEVICE_THREAD_SLEEP);
		}
	}

	ret = ES_SUCCESS;
	return ret;
}

