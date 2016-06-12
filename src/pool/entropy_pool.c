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

#include <pool/entropy_pool.h>

#include <stdlib.h>
#include <pthread.h>

#include <global/defs.h>
#include <global/alloc_type.h>
#include <global/free_type.h>
#include <collections/queue.h>
#include <crypto/digest.h>
#include <pool/entropy_block_digest.h>
#include <pool/entropy_block.h>

/**
 * Free the specified complex queue element.
 *
 * @param element The complex queue element to be freed.
 */
static inline void free_queue_element(void **element)
{
	/* Perform sanity checks. */
	if(!element || !(*element))
		return;

	/* Free the specified complex queue element. */
	free(*element);
	*element = NULL;
}

/**
 * Allocates memory for an entropy pool.
 *
 * @param pool_size The number of entropy blocks to be stored in an entropy
 * pool.
 * @param block_size The number of entropy bytes to be stored in an entropy
 * block.
 * @param alloc_type The alloc type used for internal arrays.
 * @return The address of a newly allocated entropy pool if the operation was
 * successfull, NULL otherwise.
 */
struct es_entropy_pool* es_alloc_entropy_pool(
	const int pool_size,
	const int block_size,
	const int alloc_type)
{
	int i;
	int status = ES_FAILURE;
	int *index = NULL;
	struct es_entropy_pool *pool = NULL;

	/* Perform sanity checks. */
	if(pool_size <= 0)
		goto exit;

	if(block_size <= 0)
		goto exit;

	if(es_validate_alloc_type(alloc_type) != ES_SUCCESS)
		goto exit;

	/* Allocate memory for the entropy pool structure. */
	pool = (struct es_entropy_pool*)malloc(sizeof(struct es_entropy_pool));
	if(!pool)
		goto exit;

	/* Create a new dirty queue. */
	pool->dirty_queue = es_create_queue();
	if(!pool->dirty_queue)
		goto exit;

	/* Create a new clean queue. */
	pool->clean_queue = es_create_queue();
	if(!pool->clean_queue)
		goto exit;

	/* Allocate memory for the internal entropy block array. */
	pool->blocks = (struct es_entropy_block**)malloc(
		pool_size * sizeof(struct es_entropy_block*));
	if(!pool->blocks)
		goto exit;

	for(i = 0; i < pool_size; ++i) {
		/* Create a new entropy block. */
		pool->blocks[i] = es_create_entropy_block(block_size, alloc_type);
		if(!pool->blocks[i])
			goto exit;

		/* Allocate memory for the entropy block associated index. */
		index = (int*)malloc(sizeof(int));
		if(!index)
			goto exit;

		/* Push the entropy block associated index into the dirty queue. */
		*index = i;
		if(es_push_queue(pool->dirty_queue, index) != ES_SUCCESS)
			goto exit;
	}

	/* Initialize the underlying mutex. */
	if(pthread_mutex_init(&pool->mutex, NULL))
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, free the partially allocated entropy pool. */
	if(status == ES_FAILURE && pool)
		es_free_entropy_pool(&pool, pool_size);

	return pool;
}

/**
 * Frees the memory used by an entropy pool.
 *
 * @param pool The entropy pool to be freed.
 * @param size The number of entropy blocks to be stored in an entropy pool.
 */
void es_free_entropy_pool(struct es_entropy_pool **pool, const int size)
{
	int i;

	/* Perform sanity checks. */
	if(!pool || !(*pool))
		return;

	if((*pool)->blocks) {
		/* Destroy all entropy blocks. */
		for(i = 0; i < size; ++i) {
			if((*pool)->blocks[i])
				es_destroy_entropy_block(&(*pool)->blocks[i]);
		}

		/* Free the internal entropy block array. */
		free((*pool)->blocks);
	}

	/* Destroy the dirty queue. */
	if((*pool)->dirty_queue)
		es_destroy_queue(&(*pool)->dirty_queue, free_queue_element);

	/* Destroy the clean queue. */
	if((*pool)->clean_queue)
		es_destroy_queue(&(*pool)->clean_queue, free_queue_element);

	/* Destroy the mutex associated with the current entropy pool. */
	pthread_mutex_destroy(&(*pool)->mutex);

	/* Free the entropy pool structure. */
	free(*pool);
	*pool = NULL;
}

/**
 * Initializes an etropy pool with the default values.
 *
 * @param pool The entropy pool to be initialized.
 * @param size The number of entropy blocks to be stored in an entropy pool.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_entropy_pool(struct es_entropy_pool *pool, const int size)
{
	/* Perform sanity checks. */
	if(!pool)
		return ES_FAILURE;

	if(size <= 0)
		return ES_FAILURE;

	/* Initialize the structure fields with their default values. */
	pool->size = size;

	return ES_SUCCESS;
}

/**
 * Creates an entropy pool.
 *
 * @param pool_size The number of entropy blocks to be stored in an entropy
 * pool.
 * @param block_size The number of entropy bytes to be stored in an entropy
 * block.
 * @param alloc_type The alloc type used for internal arrays.
 * @return The address of a newly allocated entropy pool if the operation was
 * successfull, NULL otherwise.
 */
struct es_entropy_pool* es_create_entropy_pool(
	const int pool_size,
	const int block_size,
	const int alloc_type)
{
	int status = ES_FAILURE;
	struct es_entropy_pool *pool = NULL;

	/* Perform sanity checks. */
	if(pool_size <= 0)
		goto exit;

	if(block_size <= 0)
		goto exit;

	if(es_validate_alloc_type(alloc_type) != ES_SUCCESS)
		goto exit;

	/* Allocate memory for the new entropy pool. */
	pool = es_alloc_entropy_pool(pool_size, block_size, alloc_type);
	if(!pool)
		goto exit;

	/* Initialize the entropy pool fields with their default values. */
	if(es_init_entropy_pool(pool, pool_size) != ES_SUCCESS)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, destroy the partially created entropy pool. */
	if(status == ES_FAILURE && pool)
		es_destroy_entropy_pool(&pool);

	return pool;
}

/**
 * Destroys an entropy pool.
 *
 * @param pool The entropy pool to be destroyed.
 */
void es_destroy_entropy_pool(struct es_entropy_pool **pool)
{
	/* Perform sanity checks. */
	if(!pool || !(*pool))
		return;

	/* Free the given entropy pool. */
	es_free_entropy_pool(pool, (*pool)->size);
}

/**
 * Validates an entropy pool.
 *
 * @param pool The entropy pool to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_entropy_pool(struct es_entropy_pool *pool)
{
	/* Perform sanity checks. */
	if(!pool)
		return ES_FAILURE;

	/* Perform field validation. */
	if(!pool->blocks)
		return ES_FAILURE;

	if(es_validate_queue(pool->dirty_queue) != ES_SUCCESS)
		return ES_FAILURE;

	if(es_validate_queue(pool->clean_queue) != ES_SUCCESS)
		return ES_FAILURE;

	if(pool->size <= 0)
		return ES_FAILURE;

	return ES_SUCCESS;
}
