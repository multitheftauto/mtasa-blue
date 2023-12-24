#pragma once

#include <stddef.h>
#include <stdio.h>

#include "lua.h"

#define luaL_getn(L,i)          ((int)lua_objlen(L, i))

typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;

#define luaL_register(L,libname,l) VluaL_register((L), (libname), (l))

#define luaL_newstate(mtasaowner) VluaL_newstate((mtasaowner))

#define luaL_ref(L,t) VluaL_ref((L), (t))
#define luaL_unref(L,t,ref) VluaL_unref((L), (t), (ref))

#define luaL_loadbuffer(L,buff,sz,name) VluaL_loadbuffer((L), (buff), (sz), (name))

#define luaL_error(L,fmt,...) VluaL_error((L), (fmt), ##__VA_ARGS__)

#define luaL_where(L,level) VluaL_where((L), (level))

#define luaL_callmeta(L,obj,e) VluaL_callmeta((L), (obj), (e))




#define luaL_typename(L,i)	lua_typename(L, lua_type(L,(i)))

#define lua_ref(L,lock) ((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
      (lua_pushstring(L, "unlocked references are obsolete"), lua_error(L), 0))

#define lua_unref(L,ref)        luaL_unref(L, LUA_REGISTRYINDEX, (ref))

#define lua_getref(L,ref)       lua_rawgeti(L, LUA_REGISTRYINDEX, (ref))

#define luaL_reg	luaL_Reg
