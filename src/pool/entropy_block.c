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

#include <pool/entropy_block.h>

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <global/defs.h>
#include <global/math_defs.h>
#include <global/alloc_type.h>
#include <crypto/digest.h>
#include <pool/entropy_block_digest.h>

/**
 * Allocates memory for an entropy array.
 *
 * @param size The size of the entropy array to be allocated.
 * @param alloc_type The alloc type used for allocating the entropy array.
 * @return The address of a newly allocated entropy array if the operation was
 * successfull, NULL otherwise.
 */
static char* es_alloc_entropy_array(const int size, const int alloc_type)
{
	char *array = NULL;

	switch(alloc_type) {
		case ES_NORMAL_ALLOC:
			/*
			 * Normal allocation used for the current entropy array. Checks if
			 * the operation succeeded or not should be done in the caller
			 * function.
			 */
			array = (char*)malloc(size * sizeof(char));
			break;

		case ES_CLEAN_ALLOC:
			/*
			 * Clean allocation used for the current entropy array. Checks if
			 * the operation succeeded or not should be done in the caller
			 * function.
			 */
			array = (char*)calloc(size, sizeof(char));
			break;

		default:
			/*
			 * No allocation to be done in this case, which means the entropy
			 * array will be NULL, which means the operation failed.
			 */
			break;
	}

	return array;
}

/**
 * Clears the contents of a given entropy array.
 *
 * @param array The array to be cleared.
 */
inline static void es_clear_entropy_array(char *array)
{
	/* Clear the contents of the specified entropy array. */
	memset(array, 0, sizeof(char) * strlen(array));
}

/**
 * Free the memory used by an entropy array. A clean operation is performed
 * first to ensure no sensitive information (in this case, entropy bytes) are
 * in danger of being leaked.
 *
 * @param array The array to be freed.
 */
static void es_free_entropy_array(char **array)
{
	/* Clear the contents of the specified entropy array. */
	es_clear_entropy_array(*array);

	/* Free the entropy array. */
	free(*array);
	*array = NULL;
}

/**
 * Compute the entropy percentage of the specified array in relation to the
 * maximum block threshold.
 *
 * @param array The array for which the percentage will be computed.
 * @param size The size of the specified array.
 * @return The entropy percentage of the specified array.
 */
static inline const double es_compute_array_entropy_percentage(
	const char *array,
	const int size)
{
	return ((double)(strlen(array) * ES_MAXIMUM_BLOCK_THRESHOLD)) / size;
}

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
	const int alloc_type)
{
	int status = ES_FAILURE;
	struct es_entropy_block *block = NULL;

	/* Perform sanity checks. */
	if(size <= 0)
		goto exit;

	if(es_validate_alloc_type(alloc_type) != ES_SUCCESS)
		goto exit;

	/* Allocate memory for the entropy block structure. */
	block = (struct es_entropy_block*)malloc(sizeof(struct es_entropy_block));
	if(!block)
		goto exit;

	/* Allocate memory for the main entropy array. */
	block->content = es_alloc_entropy_array(size, alloc_type);
	if(!block->content)
		goto exit;

	/*
	 * Allocate memory for the internal buffer. The size of the buffer is equal
	 * to the size of the main entropy array.
	 */
	block->buffer = es_alloc_entropy_array(size, alloc_type);
	if(!block->buffer)
		goto exit;

	/* Initialize the underlying mutex. */
	if(pthread_mutex_init(&block->mutex, NULL))
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, free the partially allocated entropy block. */
	if(status == ES_FAILURE && block)
		es_free_entropy_block(&block);

	return block;
}

/**
 * Frees the memory used by an entropy block.
 *
 * @param block The entropy block to be freed.
 */
void es_free_entropy_block(struct es_entropy_block **block)
{
	/* Perform sanity checks. */
	if(!block || !(*block))
		return;

	/* Free both the main entropy array and the internal buffer. */
	if((*block)->content)
		es_free_entropy_array(&(*block)->content);
	if((*block)->buffer)
		es_free_entropy_array(&(*block)->buffer);

	/* Destroy the mutex associated with the current entropy block. */
	pthread_mutex_destroy(&block->mutex);

	/* Free the entropy block structure. */
	free(*block);
	*block = NULL;
}

/**
 * Initializes an etropy block with the default values.
 *
 * @param block The entropy block to be initialized.
 * @param size The number of entropy bytes to be stored in an entropy block.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_entropy_block(struct es_entropy_block *block, const int size)
{
	/* Perform sanity checks. */
	if(!block)
		return ES_FAILURE;

	if(size <= 0)
		return ES_FAILURE;

	/* Initialize the structure fields with their default values. */
	block->size = size;
	block->state = ES_DIRTY_BLOCK_STATE;
	block->threshold = ES_MINIMUM_BLOCK_THRESHOLD;
	block->digest_type = ES_SHA512_DIGEST;

	return ES_SUCCESS;
}

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
	const int alloc_type)
{
	int status = ES_FAILURE;
	struct es_entropy_block *block = NULL;

	/* Perform sanity checks. */
	if(size <= 0)
		goto exit;

	if(es_validate_alloc_type(alloc_type) != ES_SUCCESS)
		goto exit;

	/* Allocate memory for the new entropy block. */
	block = es_alloc_entropy_block(size, alloc_type);
	if(!block)
		goto exit;

	/* Initialize the entropy block fields with their default values. */
	if(es_init_entropy_block(block, size) != ES_SUCCESS)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, destroy the partially created entropy block. */
	if(status == ES_FAILURE && block)
		es_destroy_entropy_block(&block);

	return block;
}

/**
 * Destroys an entropy block.
 *
 * @param block The entropy block to be destroyed.
 */
void es_destroy_entropy_block(struct es_entropy_block **block)
{
	/* Free the given entropy block. */
	es_free_entropy_block(block);
}

/**
 * Validates an entropy block.
 *
 * @param block The entropy block to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_entropy_block(struct es_entropy_block *block)
{
	/* Perform sanity checks. */
	if(!block)
		return ES_FAILURE;

	/* Perform field validation. */
	if(!block->content)
		return ES_FAILURE;

	if(!block->buffer)
		return ES_FAILURE;

	if(es_validate_entropy_block_state(block->state) != ES_SUCCESS)
		return ES_FAILURE;

	if(block->threshold < ES_MINIMUM_BLOCK_THRESHOLD
			|| ES_MINIMUM_BLOCK_THRESHOLD > ES_MAXIMUM_BLOCK_THRESHOLD)
		return ES_FAILURE;

	if(!es_validate_digest_type(block->digest_type) != ES_SUCCESS)
		return ES_FAILURE;

	return ES_SUCCESS;
}

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
	const char *content)
{
	int buffer_size = 0;
	int copy_size = 0;
	double block_percentage = 0.0;
	char *digest = NULL;

	/* Perform sanity checks. */
	if(!block)
		return ES_FAILURE;

	if(!es_validate_entropy_block(block) != ES_SUCCESS)
		return ES_FAILURE;

	if(!content)
		return ES_FAILURE;

	/*
	 * Append the given content to the entropy block buffer. In some cases there
	 * might be only a partial copy of the given content into the buffer (when
	 * the entropy block buffer has fewer empty positions than the number needed
	 * to completely append the given content). The copy size is then the
	 * minimum value between the size of the given content and the remaining
	 * space in the entropy block buffer.
	 */
	buffer_size = strlen(block->buffer);
	copy_size = es_min(strlen(content), block->size - buffer_size - 1);
	strncat(block->buffer, content, copy_size);
	block->buffer[buffer_size - 1] = '\0';

	/*
	 * Compute the entropy percentage for the entropy block buffer. If the
	 * percentage is lower than the threshold, then the update ends here. Only
	 * if the percentage is higher or at least equal with the threshold should
	 * the buffer content be mixed with the main entropy array.
	 */
	block_percentage = es_compute_array_entropy_percentage(
		block->buffer,
		block->size);
	if(block->threshold > block_percentage)
		return ES_SUCCESS;

	/*
	 * Mix both the main entropy array and the entropy block buffer by computing
	 * their digest.
	 */
	if(es_compute_digest_2(
			block->digest_type,
			block->content,
			block->buffer,
			&digest) != ES_SUCCESS)
		return ES_FAILURE;

	/*
	 * The entropy block buffer and the main entropy array must be cleared in
	 * order to avoid any leaks of sensitive information (in this case, entropy
	 * bytes).
	 */
	es_clear_entropy_array(block->content);
	es_clear_entropy_array(block->buffer);

	/* Copy the contents of the computed digest into the main entropy array. */
	strncpy(block->content, digest, block->size - 1);
	block->content[block->size - 1] = '\0';

	/* Free the digest array. */
	free(digest);

	/*
	 * Change the block state to clean now that the new content has been
	 * written.
	 */
	block->state = ES_CLEAN_BLOCK_STATE;

	return ES_SUCCESS;
}

/**
 * Validates the specified entropy block state.
 *
 * @param block_state The block state to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
inline const int es_validate_entropy_block_state(const int block_state)
{
	switch(block_state) {
		case ES_CLEAN_BLOCK_STATE:
		case ES_DIRTY_BLOCK_STATE:
			/* Block state is valid. */
			return ES_SUCCESS;

		default:
			/* Block state is invalid. */
			return ES_FAILURE;
	}
}
