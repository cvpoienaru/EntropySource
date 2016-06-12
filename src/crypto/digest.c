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

#include <crypto/digest.h>

#include <stdlib.h>
#include <string.h>

#include <global/defs.h>

/**
 * Allocates memory for a digest.
 *
 * @param digest_type The digest type is one of the above digest algorithm
 * codes representing the chosen digest algorithm.
 * @return The address of a newly allocated digest if the operation was
 * successfull, NULL otherwise.
 */
struct es_digest* es_alloc_digest(const int digest_type)
{
	int status = ES_FAILURE;
	struct es_digest *digest = NULL;

	/* Perform sanity checks. */
	if(es_validate_digest_type(digest_type) != ES_SUCCESS)
		goto exit;

	/* Allocate memory for the digest structure. */
	digest = (struct es_digest*)malloc(sizeof(struct es_digest));
	if(!digest)
		goto exit;

	/* Create a new underlying digest algorithm. */
	digest->algorithm = g_checksum_new(digest_type);
	if(!digest->algorithm)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, free the partially allocated digest. */
	if(status == ES_FAILURE && digest)
		es_free_digest(&digest);

	return digest;
}

/**
 * Frees the memory used by a digest.
 *
 * @param digest The digest to be freed.
 */
void es_free_digest(struct es_digest **digest)
{
	/* Perform sanity checks. */
	if(!digest || !(*digest))
		return;

	/* If created, destroy the underlying digest algorithm. */
	if((*digest)->algorithm)
		g_checksum_free((*digest)->algorithm);

	/* Free the digest structure. */
	free(*digest);
	*digest = NULL;
}

/**
 * Initializes a digest with the default values.
 *
 * @param digest The digest to be initialized.
 * @param digest_type The digest type is one of the above digest algorithm
 * codes representing the chosen digest algorithm.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_init_digest(struct es_digest *digest, const int digest_type)
{
	/* Perform sanity checks. */
	if(!digest)
		return ES_FAILURE;

	if(es_validate_digest_type(digest_type) != ES_SUCCESS)
		return ES_FAILURE;

	/* Initialize the structure fields with their default values. */
	digest->type = digest_type;

	return ES_SUCCESS;
}

/**
 * Creates a digest.
 *
 * @param digest_type The digest type is one of the above digest algorithm
 * codes representing the chosen digest algorithm.
 * @return The address of a newly allocated digest if the operation was
 * successfull, NULL otherwise.
 */
struct es_digest* es_create_digest(const int digest_type)
{
	int status = ES_FAILURE;
	struct es_digest *digest = NULL;

	/* Perform sanity checks. */
	if(es_validate_digest_type(digest_type) != ES_SUCCESS)
		goto exit;

	/* Allocate memory for the new digest. */
	digest = es_alloc_digest(digest_type);
	if(!digest)
		goto exit;

	/* Initialize the digest fields with their default values. */
	if(es_init_digest(digest, digest_type) != ES_SUCCESS)
		goto exit;

	/* Update the operation status. */
	status = ES_SUCCESS;

exit:
	/* If the operation failed, free the partially created digest. */
	if(status == ES_FAILURE && digest)
		es_destroy_digest(&digest);

	return digest;
}

/**
 * Destroys a digest.
 *
 * @param digest The digest to be destroyed.
 */
void es_destroy_digest(struct es_digest **digest)
{
	/* Free the given digest. */
	es_free_digest(digest);
}

/**
 * Validates the specified digest type.
 *
 * @param digest_type The digest type is one of the above digest algorithm
 * codes representing the chosen digest algorithm.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
inline const int es_validate_digest_type(const int digest_type)
{
	switch(digest_type) {
		case ES_MD5_DIGEST:
		case ES_SHA1_DIGEST:
		case ES_SHA256_DIGEST:
		case ES_SHA512_DIGEST:
			/* Digest type is valid. */
			return ES_SUCCESS;

		default:
			/* Digest type is invalid. */
			return ES_FAILURE;
	}
}

/**
 * Validates a digest.
 *
 * @param digest The digest to be validated.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_digest(struct es_digest *digest)
{
	/* Perform sanity checks. */
	if(!digest)
		return ES_FAILURE;

	/* Perform field validation. */
	if(!digest->algorithm)
		return ES_FAILURE;

	if(es_validate_digest_type(digest->type) != ES_SUCCESS)
		return ES_FAILURE;

	return ES_SUCCESS;
}

/**
 * Updates the digest internal buffer using the specified data.
 *
 * @param digest The digest to be updated.
 * @param data The data used to update the digest.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_update_digest(struct es_digest *digest, const char *data)
{
	/* Perform sanity checks. */
	if(!digest)
		return ES_FAILURE;

	if(es_validate_digest(digest) != ES_SUCCESS)
		return ES_FAILURE;

	if(!data)
		return ES_FAILURE;

	/* Update the digest internal buffer using the specified data. */
	g_checksum_update(
		digest->algorithm,
		(const unsigned char*)data,
		strlen(data));

	return ES_SUCCESS;
}

/**
 * Gets the string representation of the digest internal buffer.
 *
 * @param digest The digest used to get the string representation.
 * @return The address of a string buffer if the operation was successfull,
 * NULL otherwise.
 */
char* es_get_digest_string(struct es_digest *digest)
{
	char *digest_data = NULL;

	/* Perform sanity checks. */
	if(!digest)
		return digest_data;

	if(es_validate_digest(digest) != ES_SUCCESS)
		return digest_data;

	/* Get the string representation of the digest internal buffer. */
	digest_data = strdup(g_checksum_get_string(digest->algorithm));

	return digest_data;
}

/**
 * Computes the digest size based on the digest type.
 *
 * @param digest_type The digest type is one of the above digest algorithm
 * codes representing the chosen digest algorithm.
 * @return The digest size based on the digest type.
 */
inline const int es_get_digest_size(const int digest_type)
{
	/* Compute the digest size based on the digest type. */
	return g_checksum_type_get_length(digest_type);
}
