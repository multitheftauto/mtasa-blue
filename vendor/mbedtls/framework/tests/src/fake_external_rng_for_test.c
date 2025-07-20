/** \file fake_external_rng_for_test.c
 *
 * Helper functions to test external functions:
 * - mbedtls_psa_external_get_random()
 * - mbedtls_platform_get_entropy()
 *
 * These functions are provided only for test purposes and they should not be
 * used for production.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <test/fake_external_rng_for_test.h>

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)

#include <test/random.h>
#include <psa/crypto.h>

static int test_insecure_external_rng_enabled = 0;

void mbedtls_test_enable_insecure_external_rng(void)
{
    test_insecure_external_rng_enabled = 1;
}

void mbedtls_test_disable_insecure_external_rng(void)
{
    test_insecure_external_rng_enabled = 0;
}

psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    (void) context;

    if (!test_insecure_external_rng_enabled) {
        return PSA_ERROR_INSUFFICIENT_ENTROPY;
    }

    /* This implementation is for test purposes only!
     * Use the libc non-cryptographic random generator. */
    mbedtls_test_rnd_std_rand(NULL, output, output_size);
    *output_length = output_size;
    return PSA_SUCCESS;
}

#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */

#if defined(MBEDTLS_PLATFORM_GET_ENTROPY_ALT)

#include <test/random.h>
#include <mbedtls/entropy.h>

static int platform_get_entropy_force_failure;
static size_t platform_get_entropy_forced_entropy_content = SIZE_MAX;
static size_t platform_get_entropy_forced_output_len = SIZE_MAX;
static size_t platform_get_entropy_call_count;

void mbedtls_test_platform_get_entropy_reset()
{
    platform_get_entropy_call_count = 0;
    platform_get_entropy_force_failure = 0;
    platform_get_entropy_forced_entropy_content = SIZE_MAX;
    platform_get_entropy_forced_output_len = SIZE_MAX;
}

void mbedtls_test_platform_get_entropy_set_force_failure(int val)
{
    platform_get_entropy_force_failure = (val != 0);
}

void mbedtls_test_platform_get_entropy_set_output_len(size_t val)
{
    platform_get_entropy_forced_output_len = val;
}

void mbedtls_test_platform_get_entropy_set_entropy_content(size_t val)
{
    platform_get_entropy_forced_entropy_content = val;
}

size_t mbedtls_test_platform_get_entropy_get_call_count()
{
    return platform_get_entropy_call_count;
}

int mbedtls_platform_get_entropy(unsigned char *output, size_t output_size,
                                 size_t *output_len, size_t *entropy_content)
{
    platform_get_entropy_call_count++;

    /* Return a failure if we were requested to. */
    if (platform_get_entropy_force_failure != 0) {
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }

    /* Return less data than requested if we were requested to. */
    if (platform_get_entropy_forced_output_len < SIZE_MAX) {
        /* Prevent buffer overrun */
        if (platform_get_entropy_forced_output_len > output_size) {
            return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
        }
        output_size = platform_get_entropy_forced_output_len;
    }

    mbedtls_test_rnd_std_rand(NULL, output, output_size);

    *output_len = output_size;
    if (entropy_content != NULL) {
        if (platform_get_entropy_forced_entropy_content < SIZE_MAX) {
            *entropy_content = platform_get_entropy_forced_entropy_content;
        } else {
            *entropy_content = output_size * 8;
        }
    }

    return 0;
}

#endif /* MBEDTLS_PLATFORM_GET_ENTROPY_ALT */
