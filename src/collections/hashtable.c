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

#include <collections/hashtable.h>

#include <stdlib.h>

#include <global/defs.h>
#include <global/free_type.h>

/**
 * Allocates memory for a hashtable.
 *
 * @param es_compute_hashcode A pointer to the function used to computer key
 * hashcodes.
 * @param es_check_equal_keys A pointer to the function used to check if two
 * values are equal.
 * @param es_free_key A pointer to the function used to free key elements.
 * @param es_free_value A pointer to the function used to free value elements.
 * @return The address of a newly allocated hashtable if the operation was
 * successfull, NULL otherwise.
 */
struct es_hashtable* es_alloc_hashtable(
	es_compute_hashcode_function es_compute_hashcode,
	es_check_equal_keys_function es_check_equal_keys,
	es_free_library_function es_free_key,
	es_free_library_function es_free_value)
{
	int status = ES_FAILURE;
	struct es_hashtable *hashtable = NULL;

	/* Perform sanity checks. */
	if(!es_compute_hashcode)
		goto exit;

	if(!es_check_equal_keys)
		goto exit;

	/* Allocate memory for the hashtable structure. */
	hashtable = (struct es_hashtable*)malloc(sizeof(struct es_hashtable));
	if(!hashtable)
		goto exit;

	/* Create a new underlying hashtable. */
	hashtable->hashtable = g_hash_table_new_full(
		es_compute_hashcode,
		es_check_equal_keys,
		es_free_key,
		es_free_value);
	if(!hashtable->hashtable)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, free the partially allocated hashtable. */
	if(status == ES_FAILURE && hashtable)
		es_free_hashtable(&hashtable);

	return hashtable;
}

/**
 * Frees the memory used by a hashtable.
 *
 * @param hashtable The hashtable to be freed.
 */
void es_free_hashtable(struct es_hashtable **hashtable)
{
	/* Perform sanity checks. */
	if(!hashtable || !(*hashtable))
		return;

	if((*hashtable)->hashtable) {
		/* Free the underlying hashtable and all of its key-value pairs. */
		g_hash_table_destroy((*hashtable)->hashtable);
		(*hashtable)->hashtable = NULL;
	}

	/* Free the hashtable structure. */
	free(*hashtable);
	*hashtable = NULL;
}

/**
 * Initializes a hashtable with the default values.
 *
 * @param hashtable The hashtable to be initialized.
 * @param es_compute_hashcode A pointer to the function used to computer key
 * hashcodes.
 * @param es_check_equal_keys A pointer to the function used to check if two
 * values are equal.
 * @param es_free_key A pointer to the function used to free key elements.
 * @param es_free_value A pointer to the function used to free value elements.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_hashtable(
	struct es_hashtable *hashtable,
	es_compute_hashcode_function es_compute_hashcode,
	es_check_equal_keys_function es_check_equal_keys,
	es_free_library_function es_free_key,
	es_free_library_function es_free_value)
{
	/* Perform sanity checks. */
	if(!hashtable)
		return ES_FAILURE;

	if(!es_compute_hashcode)
		return ES_FAILURE;

	if(!es_check_equal_keys)
		return ES_FAILURE;

	/* Initialize the structure fields with their default values. */
	hashtable->es_compute_hashcode = es_compute_hashcode;
	hashtable->es_check_equal_keys = es_check_equal_keys;
	hashtable->es_free_key = es_free_key;
	hashtable->es_free_value = es_free_value;

	return ES_SUCCESS;
}

/**
 * Creates a new hashtable.
 *
 * @param es_compute_hashcode A pointer to the function used to computer key
 * hashcodes.
 * @param es_check_equal_keys A pointer to the function used to check if two
 * values are equal.
 * @param es_free_key A pointer to the function used to free key elements.
 * @param es_free_value A pointer to the function used to free value elements.
 * @return The address of a newly allocated hashtable if the operation was
 * successfull, NULL otherwise.
 */
struct es_hashtable* es_create_hashtable(
	es_compute_hashcode_function es_compute_hashcode,
	es_check_equal_keys_function es_check_equal_keys,
	es_free_library_function es_free_key,
	es_free_library_function es_free_value)
{
	int status = ES_FAILURE;
	struct es_hashtable *hashtable = NULL;

	/* Perform sanity checks. */
	if(!es_compute_hashcode)
		goto exit;

	if(!es_check_equal_keys)
		goto exit;

	/* Allocate memory for the new hashtable. */
	hashtable = es_alloc_hashtable(
		es_compute_hashcode,
		es_check_equal_keys,
		es_free_key,
		es_free_value);
	if(!hashtable)
		goto exit;

	/* Initialize the hashtable fields with their default values. */
	if(es_init_hashtable(
			hashtable,
			es_compute_hashcode,
			es_check_equal_keys,
			es_free_key,
			es_free_value) != ES_FAILURE)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, destroy the partially created hashtable. */
	if(status == ES_FAILURE && hashtable)
		es_destroy_hashtable(&hashtable);

	return hashtable;
}

/**
 * Destroys a hashtable.
 *
 * @param hashtable The hashtable to be destroyed.
 */
void es_destroy_hashtable(struct es_hashtable **hashtable)
{
	/* Free the given hashtable. */
	es_free_hashtable(hashtable);
}

/**
 * Validates a hashtable.
 *
 * @param hashtable The hashtable to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_hashtable(struct es_hashtable *hashtable)
{
	/* Perform sanity checks. */
	if(!hashtable)
		return ES_FAILURE;

	/* Perform field validation. */
	if(!hashtable->hashtable)
		return ES_FAILURE;

	if(!hashtable->es_compute_hashcode)
		return ES_FAILURE;

	if(!hashtable->es_check_equal_keys)
		return ES_FAILURE;

	return ES_SUCCESS;
}

/**
 * Checks if a specified key exists in the hashtable.
 *
 * @param hashtable The hashtable where to search for the key.
 * @param key The key to be searched in the hashcode.
 * @return TRUE if the key exists in the hashtable, FALSE otherwise.
 */
const int es_contains_hashtable(
	struct es_hashtable *hashtable,
	const void *key)
{
	/* Perform sanity checks. */
	if(!hashtable)
		return FALSE;

	if(es_validate_hashtable(hashtable) != ES_SUCCESS)
		return FALSE;

	/* Check if the specified key exists in the hashtable. */
	return g_hash_table_contains(hashtable->hashtable, key);
}

/**
 * Gets a hashtable value associated with the specified key.
 *
 * @param hashtable The hashtable where to get the value.
 * @param key The key associated with the value.
 * @return The address of a hashtable element if the key exists, NULL otherwise.
 */
void* es_get_hashtable_value(struct es_hashtable *hashtable, const void *key)
{
	/* Perform sanity checks. */
	if(!hashtable)
		return NULL;

	if(es_validate_hashtable(hashtable) != ES_SUCCESS)
		return NULL;

	/* Get the hashtable value associated with the specified key. */
	return g_hash_table_lookup(hashtable->hashtable, key);
}

/**
 * Inserts a key-value pair in a hashtable.
 *
 * @param hashtable The hashtable where to do the insert operation.
 * @param key The key of the pair to be inserted.
 * @param value The value of the pair to be inserted.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_insert_hashtable(
	struct es_hashtable *hashtable,
	void *key,
	void *value)
{
	/* Perform sanity checks. */
	if(!hashtable)
		return ES_FAILURE;

	if(es_validate_hashtable(hashtable) != ES_SUCCESS)
		return ES_FAILURE;

	/* Inserts the specified key-value pair in the hashtable. */
	g_hash_table_insert(hashtable->hashtable, key, value);

	return ES_SUCCESS;
}

/**
 * Removes a key-value pair from a hashtable.
 *
 * @param hashtable The hashtable where to do the remove operation.
 * @param key The key associated with the pair to be removed.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_remove_hashtable(
	struct es_hashtable *hashtable,
	const void *key)
{
	/* Perform sanity checks. */
	if(!hashtable)
		return ES_FAILURE;

	if(es_validate_hashtable(hashtable) != ES_SUCCESS)
		return ES_FAILURE;

	/* Removes the specified key-value pair from the hashtable. */
	g_hash_table_remove(hashtable->hashtable, key);

	return ES_SUCCESS;
}
