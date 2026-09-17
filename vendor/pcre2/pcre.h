/*
 * pcre.h - Stub header for backwards compatibility
 *
 * This file exists so that code which includes <pcre.h> (e.g., vendor/ehs)
 * continues to compile after migrating from PCRE1 to PCRE2. All actual PCRE
 * usage in this project goes through either the pcrecpp_compat.h wrapper
 * or the pme wrapper, both of which have been ported to PCRE2.
 *
 * Do NOT add new code that includes this file directly. Use pcre2.h instead.
 */
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
