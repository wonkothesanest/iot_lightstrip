/*
 * util.h
 *
 *  Created on: Jul 26, 2018
 *      Author: dustin
 */

#ifndef MAIN_DRIVER_UTIL_H_
#define MAIN_DRIVER_UTIL_H_

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

/* Convert string s to int out.
 *
 * @param[out] out The converted int. Cannot be NULL.
 *
 * @param[in] s Input string to be converted.
 *
 *     The format is the same as strtol,
 *     except that the following are inconvertible:
 *
 *     - empty string
 *     - leading whitespace
 *     - any trailing characters that are not part of the number
 *
 *     Cannot be NULL.
 *
 * @param[in] base Base to interpret string in. Same range as strtol (2 to 36).
 *
 * @return Indicates if the operation succeeded, or why it failed.
 * Source:
 * https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c
 */
str2int_errno str2int(int *out, char *s, int base);


#endif /* MAIN_DRIVER_UTIL_H_ */
