
/* Include the platform-specific config which defines HAVE_INTTYPES_H etc. */
#include "config.h"

/* Map upstream HAVE_* defines to JSON_C_HAVE_* names used by json_inttypes.h */
#ifdef HAVE_INTTYPES_H
#define JSON_C_HAVE_INTTYPES_H 1
#endif
#ifdef HAVE_STDINT_H
#define JSON_C_HAVE_STDINT_H 1
#endif
