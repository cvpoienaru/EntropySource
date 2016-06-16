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

#ifndef ENTROPY_SOURCE_COLLECTIONS_HASHTABLE_H_
#define ENTROPY_SOURCE_COLLECTIONS_HASHTABLE_H_

#include <stdlib.h>

#include <global/defs.h>
#include <global/free_type.h>

/** Represents a function pointer definition for computing key hashcodes. */
typedef unsigned int (*es_compute_hashcode_function)(const void *key);

/**
 * Represents a function pointer definition for checking if two values are
 * equal.
 */
typedef int (*es_check_equal_keys_function)(
	const void *value_1,
	const void *value_2);

/** Represents the definition of a basic hashtable abstraction. */
struct es_hashtable {
	/** The underlying hashtable data structure from GLib. */
	GHashTable *hashtable;

	/** A pointer to the function used to computer key hashcodes. */
	es_compute_hashcode_function es_compute_hashcode;

	/** A pointer to the function used to check if two values are equal. */
	es_check_equal_keys_function es_check_equal_keys;

	/** A pointer to the function used to free key elements. */
	es_free_library_function es_free_key;

	/** A pointer to the function used to free value elements. */
	es_free_library_function es_free_value;
};

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
	es_free_library_function es_free_value);

/**
 * Frees the memory used by a hashtable.
 *
 * @param hashtable The hashtable to be freed.
 */
void es_free_hashtable(struct es_hashtable **hashtable);

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
	es_free_library_function es_free_value);

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
	es_free_library_function es_free_value);

/**
 * Destroys a hashtable.
 *
 * @param hashtable The hashtable to be destroyed.
 */
void es_destroy_hashtable(struct es_hashtable **hashtable);

/**
 * Validates a hashtable.
 *
 * @param hashtable The hashtable to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_hashtable(struct es_hashtable *hashtable);

/**
 * Checks if a specified key exists in the hashtable.
 *
 * @param hashtable The hashtable where to search for the key.
 * @param key The key to be searched in the hashcode.
 * @return TRUE if the key exists in the hashtable, FALSE otherwise.
 */
const int es_contains_hashtable(
	struct es_hashtable *hashtable,
	const void *key);

/**
 * Gets a hashtable value associated with the specified key.
 *
 * @param hashtable The hashtable where to get the value.
 * @param key The key associated with the value.
 * @return The address of a hashtable element if the key exists, NULL otherwise.
 */
void* es_get_hashtable_value(struct es_hashtable *hashtable, const void *key);

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
	void *value);

/**
 * Removes a key-value pair from a hashtable.
 *
 * @param hashtable The hashtable where to do the remove operation.
 * @param key The key associated with the pair to be removed.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_remove_hashtable(
	struct es_hashtable *hashtable,
	const void *key);

#endif /* ENTROPY_SOURCE_COLLECTIONS_HASHTABLE_H_ */
