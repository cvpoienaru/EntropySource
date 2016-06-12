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

#ifndef ENTROPY_SOURCE_CRYPTO_DIGEST_H_
#define ENTROPY_SOURCE_CRYPTO_DIGEST_H_

#include <stdlib.h>
#include <string.h>

#include <global/defs.h>

/** The MD5 digest algorithm code. */
#define ES_MD5_DIGEST G_CHECKSUM_MD5

/** The SHA-1 digest algorithm code. */
#define ES_SHA1_DIGEST G_CHECKSUM_SHA1

/** The SHA-2 256-bit digest algorithm code. */
#define ES_SHA256_DIGEST G_CHECKSUM_SHA256

/** The SHA-2 512-bit digest algorithm code. */
#define ES_SHA512_DIGEST G_CHECKSUM_SHA512

/** Represents the definition of a basic digest abstraction. */
struct es_digest {
	/** 
	 * The digest type is one of the above digest algorithm codes representing
	 * the chosen digest algorithm.
	 */
	int type;

	/** The underlying digest algorithm from GLib. */
	GChecksum *algorithm;
};

/**
 * Allocates memory for a digest.
 *
 * @param digest_type The digest type is one of the above digest algorithm
 * codes representing the chosen digest algorithm.
 * @return The address of a newly allocated digest if the operation was
 * successfull, NULL otherwise.
 */
struct es_digest* es_alloc_digest(const int digest_type);

/**
 * Frees the memory used by a digest.
 *
 * @param digest The digest to be freed.
 */
void es_free_digest(struct es_digest **digest);

/**
 * Initializes a digest with the default values.
 *
 * @param digest The digest to be initialized.
 * @param digest_type The digest type is one of the above digest algorithm
 * codes representing the chosen digest algorithm.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_digest(struct es_digest *digest, const int digest_type);

/**
 * Creates a digest.
 *
 * @param digest_type The digest type is one of the above digest algorithm
 * codes representing the chosen digest algorithm.
 * @return The address of a newly allocated digest if the operation was
 * successfull, NULL otherwise.
 */
struct es_digest* es_create_digest(const int digest_type);

/**
 * Destroys a digest.
 *
 * @param digest The digest to be destroyed.
 */
void es_destroy_digest(struct es_digest **digest);

/**
 * Validates the specified digest type.
 *
 * @param digest_type The digest type is one of the above digest algorithm
 * codes representing the chosen digest algorithm.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_digest_type(const int digest_type);

/**
 * Validates a digest.
 *
 * @param digest The digest to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_digest(struct es_digest *digest);

/**
 * Updates the digest internal buffer using the specified data.
 *
 * @param digest The digest to be updated.
 * @param data The data used to update the digest.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_update_digest(struct es_digest *digest, const char *data);

/**
 * Gets the string representation of the digest internal buffer.
 *
 * @param digest The digest used to get the string representation.
 * @return The address of a string buffer if the operation was successfull,
 * NULL otherwise.
 */
const char* es_get_string_digest(struct es_digest *digest);

/**
 * Computes the digest size based on the digest type.
 *
 * @param digest_type The digest type is one of the above digest algorithm
 * codes representing the chosen digest algorithm.
 * @return The digest size based on the digest type.
 */
const int es_get_digest_size(const int digest_type);

#endif /* ENTROPY_SOURCE_CRYPTO_DIGEST_H_ */
