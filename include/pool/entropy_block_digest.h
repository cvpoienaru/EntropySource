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

#ifndef ENTROPY_SOURCE_POOL_ENTROPY_BLOCK_DIGEST_H_
#define ENTROPY_SOURCE_POOL_ENTROPY_BLOCK_DIGEST_H_

#include <stdlib.h>

#include <crypto/digest.h>

/**
 * Represents a function pointer definition for computing simple digests.
 */
typedef const int (*es_digest_func_1)(
	const int digest_type,
	const char *data,
	char **digest_data);

/**
 * Represents a function pointer definition for computing complex digests
 * between two data sets.
 */
typedef const int (*es_digest_func_2)(
	const int digest_type,
	const char *data_1,
	const char *data_2,
	char **digest_data);

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
	char **digest_data);

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
	char **digest_data);

#endif /* ENTROPY_SOURCE_POOL_ENTROPY_BLOCK_DIGEST_H_ */