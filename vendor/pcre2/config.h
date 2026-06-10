/*
 * config.h for PCRE2 - Windows/MSVC build
 *
 * This config is used when building PCRE2 with the premake build system.
 * It enables 8-bit code unit width with Unicode/UTF support.
 */

/* Standard C headers available on all MSVC targets */
#define HAVE_ASSERT_H       1
#define HAVE_INTTYPES_H     1
#define HAVE_LIMITS_H       1
#define HAVE_STDINT_H       1
#define HAVE_STDIO_H        1
#define HAVE_STDLIB_H       1
#define HAVE_STRING_H       1
#define HAVE_WINDOWS_H      1

/* Size and limit defaults */
#define HEAP_LIMIT          20000000
#define LINK_SIZE           2
#define MATCH_LIMIT         10000000
#define MATCH_LIMIT_DEPTH   MATCH_LIMIT
#define MAX_NAME_COUNT      10000
#define MAX_NAME_SIZE       128
#define MAX_VARLOOKBEHIND   255
#define NEWLINE_DEFAULT     2
#define PARENS_NEST_LIMIT   250

/* Enable 8-bit code unit width only */
#define SUPPORT_PCRE2_8     1

/* Enable Unicode and UTF support */
#define SUPPORT_UNICODE     1

/* Version info (from pcre2.h) */
#define PACKAGE             "pcre2"
#define PACKAGE_NAME        "PCRE2"
#define PACKAGE_VERSION     "10.48"
#define PACKAGE_STRING      "PCRE2 10.48"
#define PACKAGE_TARNAME     "pcre2"
#define VERSION             "10.48"
