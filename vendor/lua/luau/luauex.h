#pragma once

#include "VM/include/lua.h"

LUA_API int lua_nresults(lua_State* L);

LUA_API int Vluau_getstackgap(lua_State* L);

LUA_API void lua_addtotalbytes(lua_State *L, int n);