/* config.h for CMake builds */

/* #undef HAVE_DIRENT_H */
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
/* #undef HAVE_UNISTD_H */
/* #undef HAVE_WINDOWS_H */
#define HAVE_STDINT_H 1                                                   
#define HAVE_INTTYPES_H 1    

/* #undef HAVE_TYPE_TRAITS_H */
/* #undef HAVE_BITS_TYPE_TRAITS_H */

/* #undef HAVE_BCOPY */
#define HAVE_MEMMOVE 1
#define HAVE_STRERROR 1
#define HAVE_STRTOLL 1
/* #undef HAVE_STRTOQ */
#define HAVE__STRTOI64 1

/* #undef PCRE_STATIC */

#define SUPPORT_PCRE8 1
/* #undef SUPPORT_PCRE16 */
/* #undef SUPPORT_PCRE32 */
/* #undef SUPPORT_JIT */
/* #undef SUPPORT_PCREGREP_JIT */
#define SUPPORT_UTF 1
/* #undef SUPPORT_UCP */
/* #undef EBCDIC */
/* #undef EBCDIC_NL25 */
/* #undef BSR_ANYCRLF */
/* #undef NO_RECURSE */

#define HAVE_LONG_LONG 1
#define HAVE_UNSIGNED_LONG_LONG 1

/* #undef SUPPORT_LIBBZ2 */
/* #undef SUPPORT_LIBZ */
/* #undef SUPPORT_LIBEDIT */
/* #undef SUPPORT_LIBREADLINE */

/* #undef SUPPORT_VALGRIND */
/* #undef SUPPORT_GCOV */

#define NEWLINE			3338
#define POSIX_MALLOC_THRESHOLD	10
#define LINK_SIZE		2
#define PARENS_NEST_LIMIT       250
#define MATCH_LIMIT		10000000
#define MATCH_LIMIT_RECURSION	MATCH_LIMIT
#define PCREGREP_BUFSIZE        20480

#define MAX_NAME_SIZE	32
#define MAX_NAME_COUNT	10000

/* end config.h for CMake builds */
