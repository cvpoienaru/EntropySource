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

#ifndef ENTROPY_SOURCE_GLOBAL_DEFS_H_
#define ENTROPY_SOURCE_GLOBAL_DEFS_H_

#include <stdlib.h>

#include <glib.h>

/**
 * Represents the boolean FALSE value.
 * We must define this value here if not already defined, otherwise some modules
 * might not compile.
 */
#ifndef FALSE
#define FALSE 0
#endif

/**
 * Represents the boolean TRUE value.
 * We must define this value here if not already defined, otherwise some modules
 * might not compile.
 */
#ifndef TRUE
#define TRUE !(FALSE)
#endif

/** Represents the definition of the debug mode. */
#define ES_DEBUG 1

/** Represents the definition of a successfull operation. */
#define ES_SUCCESS EXIT_SUCCESS

/** Represents the definition of an unsuccessfull operation. */
#define ES_FAILURE EXIT_FAILURE

/** Represents the definition of a non-matchable descriptor. */
#define ES_DEFAULT_DESCRIPTOR -1

#endif /* ENTROPY_SOURCE_GLOBAL_DEFS_H_ */
