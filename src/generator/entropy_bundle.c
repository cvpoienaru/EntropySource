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

#include <generator/entropy_bundle.h>

#include <stdlib.h>

#include <global/defs.h>
#include <device/descriptor.h>
#include <device/serial_driver.h>
#include <pool/entropy_pool.h>

/**
 * Allocates memory for an entropy bundle.
 *
 * @return The address of a newly allocated entropy bundle if the operation was
 * successfull, NULL otherwise.
 */
struct es_entropy_bundle* es_alloc_entropy_bundle(void)
{
	struct es_entropy_bundle *bundle = NULL;

	/* Allocate memory for the entropy bundle structure. */
	bundle = (struct es_entropy_bundle*)malloc(
		sizeof(struct es_entropy_bundle));

	return bundle;
}

/**
 * Frees the memory used by an entropy bundle.
 *
 * @param bundle The entropy bundle to be freed.
 */
void es_free_entropy_bundle(struct es_entropy_bundle **bundle)
{
	/* Perform sanity checks. */
	if(!bundle || !(*bundle))
		return;

	/* Free the entropy bundle structure. */
	free(*bundle);
	*bundle = NULL;
}

/**
 * Initializes an entropy bundle with the default values.
 *
 * @param bundle The entropy bundle to be initialized.
 * @param pool The entropy pool associated with the entropy bundle.
 * @param descriptor The device descriptor associated with the entropy bundle.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_entropy_bundle(
	struct es_entropy_bundle *bundle,
	struct es_entropy_pool *pool,
	struct es_device_descriptor *descriptor)
{
	/* Perform sanity checks. */
	if(!bundle)
		return ES_FAILURE;

	if(!pool)
		return ES_FAILURE;

	if(!descriptor)
		return ES_FAILURE;

	if(es_validate_entropy_pool(pool) != ES_SUCCESS)
		return ES_FAILURE;

	if(es_validate_device_descriptor(descriptor) != ES_SUCCESS)
		return ES_FAILURE;

	/* Initialize the structure fields with their default values. */
	bundle->pool = pool;
	bundle->descriptor = descriptor;

	return ES_SUCCESS;
}

/**
 * Creates an entropy bundle.
 *
 * @param pool The entropy pool associated with the entropy bundle.
 * @param descriptor The device descriptor associated with the entropy bundle.
 * @return The address of a newly allocated entropy bundle if the operation was
 * successfull, NULL otherwise.
 */
struct es_entropy_bundle* es_create_entropy_bundle(
	struct es_entropy_pool *pool,
	struct es_device_descriptor *descriptor)
{
	int status = ES_FAILURE;
	struct es_entropy_bundle *bundle = NULL;

	/* Perform sanity checks. */
	if(!pool)
		goto exit;

	if(!descriptor)
		goto exit;

	if(es_validate_entropy_pool(pool) != ES_SUCCESS)
		goto exit;

	if(es_validate_device_descriptor(descriptor) != ES_SUCCESS)
		goto exit;

	/* Allocate memory for the new entropy bundle. */
	bundle = es_alloc_entropy_bundle();
	if(!bundle)
		goto exit;

	/* Initialize the entropy bundle fields with their default values. */
	if(es_init_entropy_bundle(bundle, pool, descriptor) != ES_SUCCESS)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, destroy the partially created entropy bundle. */
	if(status == ES_FAILURE && bundle)
		es_destroy_entropy_bundle(&bundle);

	return bundle;
}

/**
 * Destroys an entropy bundle.
 *
 * @param bundle The entropy bundle to be destroyed.
 */
void es_destroy_entropy_bundle(struct es_entropy_bundle **bundle)
{
	/* Free the given entropy bundle. */
	es_free_entropy_bundle(bundle);
}

/**
 * Validates an entropy bundle.
 *
 * @param bundle The entropy bundle to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_entropy_bundle(struct es_entropy_bundle *bundle)
{
	/* Perform sanity checks. */
	if(!bundle)
		return ES_FAILURE;

	/* Perform field validation. */
	if(!bundle->pool)
		return ES_FAILURE;

	if(!bundle->descriptor)
		return ES_FAILURE;

	if(es_validate_entropy_pool(bundle->pool) != ES_SUCCESS)
		return ES_FAILURE;

	if(es_validate_device_descriptor(bundle->descriptor) != ES_SUCCESS)
		return ES_FAILURE;

	return ES_SUCCESS;
}
