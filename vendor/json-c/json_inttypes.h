// This file is modified by MTA, by reverting json-c commit 1ee1210 (See https://github.com/multitheftauto/mtasa-blue/commit/ee7d32a7abcfaf6dc748485c94f03fec4df64b17) and will continue to deviate from json-c master version.
// Description: "Not relevant for us and causes build error"


/**
 * @file
 * @brief Do not use, json-c internal, may be changed or removed at any time.
 */
#ifndef _json_inttypes_h_
#define _json_inttypes_h_

#include "json_config.h"

#ifdef JSON_C_HAVE_INTTYPES_H
/* inttypes.h includes stdint.h */
#include <inttypes.h>

#else
#include <stdint.h>

#define PRId64 "I64d"
#define SCNd64 "I64d"
#define PRIu64 "I64u"

#endif

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#endif