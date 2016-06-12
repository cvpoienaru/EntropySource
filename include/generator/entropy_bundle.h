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

#ifndef ENTROPY_SOURCE_GENERATOR_ENTROPY_BUNDLE_H_
#define ENTROPY_SOURCE_GENERATOR_ENTROPY_BUNDLE_H_

#include <stdlib.h>

#include <global/defs.h>
#include <device/descriptor.h>
#include <device/serial_driver.h>
#include <pool/entropy_pool.h>

/** Structure defining a basic entropy bundle. */
struct es_entropy_bundle {
	/** The entropy pool associated with the entropy bundle. */
	struct es_entropy_pool *pool;

	/** The device descriptor associated with the entropy bundle. */
	struct es_device_descriptor *descriptor;
};

/**
 * Allocates memory for an entropy bundle.
 *
 * @return The address of a newly allocated entropy bundle if the operation was
 * successfull, NULL otherwise.
 */
struct es_entropy_bundle* es_alloc_entropy_bundle(void);

/**
 * Frees the memory used by an entropy bundle.
 *
 * @param bundle The entropy bundle to be freed.
 */
void es_free_entropy_bundle(struct es_entropy_bundle **bundle);

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
	struct es_device_descriptor *descriptor);

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
	struct es_device_descriptor *descriptor);

/**
 * Destroys an entropy bundle.
 *
 * @param bundle The entropy bundle to be destroyed.
 */
void es_destroy_entropy_bundle(struct es_entropy_bundle **bundle);

/**
 * Validates an entropy bundle.
 *
 * @param bundle The entropy bundle to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_entropy_bundle(struct es_entropy_bundle *bundle);

#endif /* ENTROPY_SOURCE_GENERATOR_ENTROPY_BUNDLE_H_ */
