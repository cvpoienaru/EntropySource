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

#include <global/math_defs.h>

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <global/defs.h>

/**
 * Determines the minimum of two long numbers.
 *
 * @param x The first number to be compared.
 * @param y The second number to be compared.
 * @return The minimum of the two numbers.
 */
inline const long es_min(const long x, const long y)
{
	return (x < y) ? x : y;
}

/**
 * Determines the maximum of two long numbers.
 *
 * @param x The first number to be compared.
 * @param y The second number to be compared.
 * @return The maximum of the two numbers.
 */
inline const long es_max(const long x, const long y)
{
	return (x > y) ? x : y;
}

/**
 * Determines the minimum of two double numbers.
 *
 * @param x The first number to be compared.
 * @param y The second number to be compared.
 * @return The minimum of the two numbers.
 */
inline const double es_mind(const double x, const double y)
{
	return (x < y) ? x : y;
}

/**
 * Determines the maximum of two double numbers.
 *
 * @param x The first number to be compared.
 * @param y The second number to be compared.
 * @return The maximum of the two numbers.
 */
inline const double es_maxd(const double x, const double y)
{
	return (x > y) ? x : y;
}

/**
 * Checks if the specified two numbers are equal.
 *
 * @param x The first number to be compared.
 * @param y The second number to be compared.
 * @return TRUE if the numbers are equal relative to the tolerance threshold,
 * FALSE otherwise.
 */
inline const int es_equald(const double x, const double y)
{
	return fabs(x - y) < ES_DOUBLE_EPSILON ? TRUE : FALSE;
}
