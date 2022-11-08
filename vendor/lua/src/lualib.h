#pragma once

#include "LuaAPI.h"

#define LUA_COLIBNAME	"coroutine"
#define luaopen_base(L) Vluaopen_base((L))

#define LUA_MATHLIBNAME	"math"
#define luaopen_math(L) Vluaopen_math((L))

#define LUA_STRLIBNAME	"string"
#define luaopen_string(L) Vluaopen_string((L))

#define LUA_TABLIBNAME	"table"
#define luaopen_table(L) Vluaopen_table((L))

#define LUA_DBLIBNAME	"debug"
#define luaopen_debug(L) Vluaopen_debug((L))

/* Additional luautf8 library */
#define luaopen_utf8(L) Vluaopen_utf8((L))

#define LUA_OSLIBNAME	"os"
#define luaopen_os(L) Vluaopen_os((L))
