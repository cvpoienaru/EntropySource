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

#include <pool/entropy_block_digest.h>

#include <stdlib.h>
#include <string.h>

#include <global/defs.h>
#include <global/math_defs.h>
#include <crypto/digest.h>

/**
 * Computes the digest for the given data set.
 *
 * @param digest_type The digest type represents the digest algorithm code which
 * identifies a particular digest algorithm used to compute the digest.
 * @param data The data set for which the digest must be computed.
 * @param digest_data The output digest data.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_compute_digest_1(
	const int digest_type,
	const char *data,
	char **digest_data)
{
	int ret = ES_FAILURE;
	struct es_digest *digest = NULL;

	/* Perform sanity checks. */
	if(es_validate_digest_type(digest_type) != ES_SUCCESS)
		goto exit;

	if(!data)
		goto exit;

	/* Create a new digest. */
	digest = es_create_digest(digest_type);
	if(!digest)
		goto exit;

	/* Update the digest internal buffer using the specified data. */
	if(es_update_digest(digest, data) != ES_SUCCESS)
		goto exit;

	/* Get the string representation of the digest internal buffer. */
	*digest_data = es_get_digest_string(digest);
	if(!digest_data)
		goto exit;

	/* Update the operation status. */
	ret = ES_SUCCESS;

exit:
	/* If the operation failed, clear & free the digest data buffer. */
	if(ret == ES_FAILURE && digest_data && *digest_data) {
		memset(*digest_data, 0, es_get_digest_size(digest->type));
		*digest_data[0] = '\0';
	}

	/* Destroy the digest. */
	if(digest)
		es_destroy_digest(&digest);

	return ret;
}

/**
 * Computes the digest for the two given data sets.
 *
 * @param digest_type The digest type represents the digest algorithm code which
 * identifies a particular digest algorithm used to compute the digest.
 * @param data_1 The first data set for which the digest must be computed.
 * @param data_2 The second data set for which the digest must be computed.
 * @param digest_data The output digest data.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_compute_digest_2(
	const int digest_type,
	const char *data_1,
	const char *data_2,
	char **digest_data)
{
	int i;
	int data_1_size;
	int data_2_size;
	int minimum_digest_size;
	int ret = ES_FAILURE;
	char *destination_data = NULL;
	char *source_data = NULL;

	/* Perform sanity checks. */
	if(es_validate_digest_type(digest_type) != ES_SUCCESS)
		goto exit;

	if(!data_1)
		goto exit;

	if(!data_2)
		goto exit;

	/* Compute the minimum size of the two data sets. */
	data_1_size = strlen(data_1);
	data_2_size = strlen(data_2);
	minimum_digest_size = es_min(data_1_size, data_2_size);

	/* Decide which data set is the destination set and which is the source. */
	destination_data = (data_1_size > data_2_size)
		? strdup(data_1)
		: strdup(data_2);
	source_data = (data_1_size < data_2_size)
		? strdup(data_1)
		: strdup(data_2);

	/* Combine the two data sets using a secure function like XOR. */
	for(i = 0; i < minimum_digest_size; ++i)
		destination_data[i] ^= source_data[i];

	/* Compute the digest for the combined data set. */
	if(es_compute_digest_1(
			digest_type,
			destination_data,
			digest_data) != ES_SUCCESS)
		goto exit;

	/* Update the operation status. */
	ret = ES_SUCCESS;

exit:
	/* Clear & free the destination data set. */
	if(destination_data) {
		memset(destination_data, 0, strlen(destination_data));
		free(destination_data);
		destination_data = NULL;
	}

	/* Clear & free the source data set. */
	if(source_data) {
		memset(source_data, 0, strlen(source_data));
		free(source_data);
		source_data = NULL;
	}

	return ret;
}
