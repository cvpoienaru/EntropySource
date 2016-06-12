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

#ifndef ENTROPY_SOURCE_POOL_ENTROPY_POOL_H_
#define ENTROPY_SOURCE_POOL_ENTROPY_POOL_H_

#include <stdlib.h>
#include <pthread.h>

#include <global/defs.h>
#include <global/free_type.h>
#include <collections/queue.h>
#include <crypto/digest.h>
#include <pool/entropy_block_digest.h>
#include <pool/entropy_block.h>

/** Structure defining the basic entropy pool. */
struct es_entropy_pool {
	/** The number of entropy blocks to be stored in an entropy pool. */
	int size;

	/** The array that stores references to the entropy blocks. */
	struct es_entropy_block **blocks;

	/** The queue used to keep references to dirty blocks. */
	struct es_queue *dirty_queue;

	/** The queue used to keep references to clean blocks. */
	struct es_queue *clean_queue;

	/**
	 * The current pool mutex used for mutual exclusion between read and write
	 * operations applied to the pool.
	 */
	pthread_mutex_t mutex;
};

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
	const int alloc_type);

/**
 * Frees the memory used by an entropy pool.
 *
 * @param pool The entropy pool to be freed.
 * @param size The number of entropy blocks to be stored in an entropy pool.
 */
void es_free_entropy_pool(struct es_entropy_pool **pool, const int size);

/**
 * Initializes an etropy pool with the default values.
 *
 * @param pool The entropy pool to be initialized.
 * @param size The number of entropy blocks to be stored in an entropy pool.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_entropy_pool(struct es_entropy_pool *pool, const int size);

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
	const int alloc_type);

/**
 * Destroys an entropy pool.
 *
 * @param pool The entropy pool to be destroyed.
 */
void es_destroy_entropy_pool(struct es_entropy_pool **pool);

/**
 * Validates an entropy pool.
 *
 * @param pool The entropy pool to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_entropy_pool(struct es_entropy_pool *pool);

#endif /* ENTROPY_SOURCE_POOL_ENTROPY_POOL_H_ */
