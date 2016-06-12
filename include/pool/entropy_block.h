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

#ifndef ENTROPY_SOURCE_POOL_ENTROPY_BLOCK_H_
#define ENTROPY_SOURCE_POOL_ENTROPY_BLOCK_H_

#include <stdlib.h>
#include <pthread.h>

#include <global/defs.h>
#include <crypto/digest.h>
#include <pool/entropy_block_digest.h>

/**
 * Indicates that the entropy block is clean and can be used to fulfill client
 * requests according to the block threshold.
 */
#define ES_CLEAN_BLOCK_STATE 0

/**
 * Indicates that the entropy block is dirty and cannot be used to fulfill
 * client requests. The block must be "cleaned", meaning it has to be filled
 * with a given amount of entropy, acording to the block threshold, in order to
 * be usable again.
 */
#define ES_DIRTY_BLOCK_STATE 1

/**
 * Minimum block threshold expressed in percentage points relative to the total
 * block size.
 */
#define ES_MINIMUM_BLOCK_THRESHOLD 0.0

/**
 * Maximum block threshold expressed in percentage points relative to the total
 * block size.
 */
#define ES_MAXIMUM_BLOCK_THRESHOLD 100.0

/**
 * Structure defining the basic entropy block.
 */
struct es_entropy_block {
	/** The number of entropy bytes to be stored in an entropy block. */
	int size;

	/** The entropy block internal array for storing actual entropy bytes. */
	char *content;

	/**
	 * The entropy block buffer for storing temporary entropy bytes until the
	 * threshold is reached. After exceeding the threshold, the bytes will be
	 * mixed with those already in store in the main array and the buffer
	 * will be cleared.
	 */
	char *buffer;

	/**
	 * Indicates the state of the current entropy block. The state is either
	 * clean (ES_CLEAN_BLOCK_STATE) or dirty (ES_DIRTY_BLOCK_STATE).
	 */
	int state;

	/**
	 * The threshold of the current entropy block expressed in percentage
	 * points. The value must be between ES_MINIMUM_BLOCK_THRESHOLD (meaning the
	 * bytes are written directly to the main array after being wrote to
	 * the buffer) and ES_MAXIMUM_BLOCK_THRESHOLD (meaning that the buffer must
	 * be full for the bytes to be written to the main array).
	 */
	double threshold;

	/**
	 * Indicates the digest type for the current entropy block. The digest type
	 * is used to determine the digest function to be applied when mixing the
	 * entropy bytes stored in the main entropy array and those stored in the
	 * entropy buffer.
	 */
	int digest_type;

	/**
	 * The current block mutex used for mutual exclusion between read and write
	 * operations applied to the same block.
	 */
	pthread_mutex_t mutex;
};

/**
 * Allocates memory for an entropy block.
 *
 * @param size The number of entropy bytes to be stored in an entropy block.
 * @param alloc_type The alloc type used for internal arrays.
 * @return The address of a newly allocated entropy block if the operation was
 * successfull, NULL otherwise.
 */
struct es_entropy_block* es_alloc_entropy_block(
	const int size,
	const int alloc_type);

/**
 * Frees the memory used by an entropy block.
 *
 * @param block The entropy block to be freed.
 */
void es_free_entropy_block(struct es_entropy_block **block);

/**
 * Initializes an etropy block with the default values.
 *
 * @param block The entropy block to be initialized.
 * @param size The number of entropy bytes to be stored in an entropy block.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_entropy_block(struct es_entropy_block *block, const int size);

/**
 * Creates an entropy block.
 *
 * @param size The number of entropy bytes to be stored in an entropy block.
 * @param alloc_type The alloc type used for internal arrays.
 * @return The address of a newly allocated entropy block if the operation was
 * successfull, NULL otherwise.
 */
struct es_entropy_block* es_create_entropy_block(
	const int size,
	const int alloc_type);

/**
 * Destroys an entropy block.
 *
 * @param block The entropy block to be destroyed.
 */
void es_destroy_entropy_block(struct es_entropy_block **block);

/**
 * Validates an entropy block.
 *
 * @param block The entropy block to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_entropy_block(struct es_entropy_block *block);

/**
 * Updates the contents of the specified entropy block with the new content
 * array. Depending on the value of the block threshold, the content may be
 * written to the internal buffer or directly to the main array.
 *
 * @param block The entropy block to be updated.
 * @param content The contents to be written to the specified entropy block.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_update_entropy_block_content(
	struct es_entropy_block *block,
	const char *content);

/**
 * Validates the specified entropy block state.
 *
 * @param block_state The block state to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_entropy_block_state(const int block_state);

#endif /* ENTROPY_SOURCE_POOL_ENTROPY_BLOCK_H_ */
