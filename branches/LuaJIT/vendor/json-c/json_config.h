#if defined(_MSC_VER) && _MSC_VER <= 1700
	#undef JSON_C_HAVE_INTTYPES_H
#else
	#define JSON_C_HAVE_INTTYPES_H 1
#endif
