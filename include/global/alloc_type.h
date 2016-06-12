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

#ifndef ENTROPY_SOURCE_GLOBAL_ALLOC_TYPE_H_
#define ENTROPY_SOURCE_GLOBAL_ALLOC_TYPE_H_

#include <stdlib.h>

#include <global/defs.h>

/** Indicates that the alloc type is unknown. */
#define ES_NO_ALLOC 0

/** Indicates that the alloc type is a normal malloc. */
#define ES_NORMAL_ALLOC 1

/** Indicates that the alloc type is a clean alloc using calloc. */
#define ES_CLEAN_ALLOC 2

/**
 * Validates the specified alloc type.
 *
 * @param alloc_type The alloc type is one of the above alloc type codes
 * representing the chosen alloc function used to allocate memory.
 * @return ES_SUCCESS if the operation was successfull, ES_FAILURE otherwise.
 */
const int es_validate_alloc_type(const int alloc_type);

#endif /* ENTROPY_SOURCE_GLOBAL_ALLOC_TYPE_H_ */
