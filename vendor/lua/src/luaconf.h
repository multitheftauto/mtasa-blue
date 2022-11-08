#pragma once

#include <limits.h>
#include <stddef.h>

#define LUA_IDSIZE	60

#define LUA_QL(x)	"'" x "'"

#define LUA_NUMBER_FMT		"%.14g"
#define lua_number2str(s,n)	sprintf((s), LUA_NUMBER_FMT, (n))
#define LUAI_MAXNUMBER2STR	32 /* 16 digits, sign, point, and \0 */

#if defined(LUA_USE_APICHECK)
/////////////////////////////////////////////////////////////////////////
// MTA addition for testing if apicheck will function as expected, and generating more useful crash dumps
    #undef assert
    #define assert(_Expression) (void)( (!!(_Expression)) || ( *((int*)NULL) = 0) )
	#define luaX_is_apicheck_enabled() VluaX_is_apicheck_enabled()
/////////////////////////////////////////////////////////////////////////
#define luai_apicheck(L,o)	{ (void)L; assert(o); }
#else
#define luai_apicheck(L,o)	{ (void)L; }
#endif

