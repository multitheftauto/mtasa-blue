/*
** LUA-VEC
** Lua Vectors
** See Copyright Notice in lua.h
*/

#ifndef lvector_h
#define lvector_h


#include "lgc.h"
#include "lobject.h"
#include "lstate.h"


LUAI_FUNC Vector *luaVec_new (lua_State *L, float x, float y, float z, float w);
LUAI_FUNC void luaVec_free (lua_State *L, GCObject *o);


#endif
