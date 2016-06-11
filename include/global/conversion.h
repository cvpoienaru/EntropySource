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

#ifndef ENTROPY_SOURCE_GLOBAL_CONVERSION_H_
#define ENTROPY_SOURCE_GLOBAL_CONVERSION_H_

#include <stdlib.h>

#include <global/defs.h>

/** Represents the number of bits that form a byte. */
#define ES_BITS_IN_BYTE 8

/**
 * Converts the specified number of bytes to bits.
 *
 * @param bytes The number of bytes to be converted to bits.
 * @return The number of bits coresponding to the specified number of bytes.
 */
const long es_convert_bytes_to_bits(const long bytes);

/**
 * Converts the specified number of bits to bytes.
 *
 * @param bits The number of bits to be converted to bytes.
 * @param remainder Output parameter representing the remainder of the division.
 * @return The number of bytes coresponding to the specified number of bits.
 */
const long es_convert_bits_to_byte(const long bits, int *remainder);

#endif /* ENTROPY_SOURCE_GLOBAL_CONVERSION_H_ */
