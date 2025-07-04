/* Common definitions used by test drivers. */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_CRYPTO_TEST_DRIVERS_TEST_DRIVER_COMMON_H
#define PSA_CRYPTO_TEST_DRIVERS_TEST_DRIVER_COMMON_H

#include "mbedtls/build_info.h"

/* Use the same formatting for error code definitions as the standard
 * error values, which must have a specific sequence of tokens for
 * interoperability between implementations of different parts of PSA.
 * This means no space between the cast and the - operator.
 * This contradicts our code style, so we temporarily disable style checking.
 *
 * *INDENT-OFF*
 */

/** Error code that test drivers return when they detect that an input
 * parameter was not initialized properly. This normally indicates a
 * bug in the core.
 */
#define PSA_ERROR_TEST_DETECTED_BAD_INITIALIZATION ((psa_status_t)-0x0201)

/* *INDENT-ON* */

#endif /* test_driver_common.h */
