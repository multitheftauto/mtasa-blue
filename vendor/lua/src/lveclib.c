/*
** Vector math library (LUA-VEC)
** See Copyright Notice in lua.h
*/

#include <stdlib.h>
#include <math.h>

#define lveclib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int vec_new(lua_State *L) {
  float x = (float)lua_tonumber(L, 1);
  float y = (float)lua_tonumber(L, 2);
  float z = (float)lua_tonumber(L, 3);
  float w = (float)lua_tonumber(L, 4);
  lua_pushvec(L, x, y, z, w);
  return 1;
}

static int vec_call(lua_State *L) {
  /* In __call, first argument is the table itself, subsequent args are x, y, z, w */
  float x = (float)lua_tonumber(L, 2);
  float y = (float)lua_tonumber(L, 3);
  float z = (float)lua_tonumber(L, 4);
  float w = (float)lua_tonumber(L, 5);
  lua_pushvec(L, x, y, z, w);
  return 1;
}

static int vec_dot(lua_State *L) {
  const float *v1 = luaL_checkvec(L, 1);
  const float *v2 = luaL_checkvec(L, 2);
  lua_pushnumber(L, (lua_Number)(v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3]));
  return 1;
}

static int vec_cross(lua_State *L) {
  const float *v1 = luaL_checkvec(L, 1);
  const float *v2 = luaL_checkvec(L, 2);
  lua_pushvec(L,
              v1[1] * v2[2] - v1[2] * v2[1],
              v1[2] * v2[0] - v1[0] * v2[2],
              v1[0] * v2[1] - v1[1] * v2[0],
              0.0f);
  return 1;
}

static int vec_length(lua_State *L) {
  const float *v = luaL_checkvec(L, 1);
  lua_pushnumber(L, (lua_Number)sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]));
  return 1;
}

static int vec_lengthSquared(lua_State *L) {
  const float *v = luaL_checkvec(L, 1);
  lua_pushnumber(L, (lua_Number)(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]));
  return 1;
}

static int vec_normalize(lua_State *L) {
  const float *v = luaL_checkvec(L, 1);
  float lenSq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
  if (lenSq > 0.0f) {
    float s = 1.0f / sqrtf(lenSq);
    lua_pushvec(L, v[0] * s, v[1] * s, v[2] * s, v[3] * s);
  } else {
    lua_pushvec(L, 0.0f, 0.0f, 0.0f, 0.0f);
  }
  return 1;
}

static int vec_distance(lua_State *L) {
  const float *v1 = luaL_checkvec(L, 1);
  const float *v2 = luaL_checkvec(L, 2);
  float dx = v1[0] - v2[0];
  float dy = v1[1] - v2[1];
  float dz = v1[2] - v2[2];
  float dw = v1[3] - v2[3];
  lua_pushnumber(L, (lua_Number)sqrtf(dx * dx + dy * dy + dz * dz + dw * dw));
  return 1;
}

static int vec_distanceSquared(lua_State *L) {
  const float *v1 = luaL_checkvec(L, 1);
  const float *v2 = luaL_checkvec(L, 2);
  float dx = v1[0] - v2[0];
  float dy = v1[1] - v2[1];
  float dz = v1[2] - v2[2];
  float dw = v1[3] - v2[3];
  lua_pushnumber(L, (lua_Number)(dx * dx + dy * dy + dz * dz + dw * dw));
  return 1;
}

static int vec_unpack(lua_State *L) {
  const float *v = luaL_checkvec(L, 1);
  if (v[3] == 0.0f) {
    lua_pushnumber(L, (lua_Number)v[0]);
    lua_pushnumber(L, (lua_Number)v[1]);
    lua_pushnumber(L, (lua_Number)v[2]);
    return 3;
  }
  lua_pushnumber(L, (lua_Number)v[0]);
  lua_pushnumber(L, (lua_Number)v[1]);
  lua_pushnumber(L, (lua_Number)v[2]);
  lua_pushnumber(L, (lua_Number)v[3]);
  return 4;
}

static int vec_abs(lua_State *L) {
  const float *v = luaL_checkvec(L, 1);
  lua_pushvec(L, fabsf(v[0]), fabsf(v[1]), fabsf(v[2]), fabsf(v[3]));
  return 1;
}

static int vec_floor(lua_State *L) {
  const float *v = luaL_checkvec(L, 1);
  lua_pushvec(L, floorf(v[0]), floorf(v[1]), floorf(v[2]), floorf(v[3]));
  return 1;
}

static int vec_ceil(lua_State *L) {
  const float *v = luaL_checkvec(L, 1);
  lua_pushvec(L, ceilf(v[0]), ceilf(v[1]), ceilf(v[2]), ceilf(v[3]));
  return 1;
}

static const luaL_Reg veclib[] = {
  {"create",            vec_new},
  {"new",               vec_new},
  {"dot",               vec_dot},
  {"getDotProduct",     vec_dot},
  {"cross",             vec_cross},
  {"getCrossProduct",   vec_cross},
  {"length",            vec_length},
  {"getLength",         vec_length},
  {"lengthSquared",     vec_lengthSquared},
  {"getLengthSquared",  vec_lengthSquared},
  {"normalize",         vec_normalize},
  {"getNormalized",     vec_normalize},
  {"distance",          vec_distance},
  {"getDistance",       vec_distance},
  {"distanceSquared",   vec_distanceSquared},
  {"getDistanceSquared",vec_distanceSquared},
  {"unpack",            vec_unpack},
  {"getComponents",     vec_unpack},
  {"abs",               vec_abs},
  {"floor",             vec_floor},
  {"ceil",              vec_ceil},
  {NULL, NULL}
};

/*
** Open veclib
*/
LUALIB_API int luaopen_vec(lua_State *L) {
  luaL_register(L, LUA_VECLIBNAME, veclib);

  /* Make the vector table directly callable: vector(x, y, z) */
  lua_newtable(L);
  lua_pushcfunction(L, vec_call);
  lua_setfield(L, -2, "__call");
  lua_setmetatable(L, -2);

  /* Set global alias 'vector' */
  lua_pushvalue(L, -1);
  lua_setglobal(L, "vector");

  /* Numeric constants */
  lua_pushvec(L, 0.0f, 0.0f, 0.0f, 0.0f);
  lua_setfield(L, -2, "zero");
  lua_pushvec(L, 1.0f, 1.0f, 1.0f, 1.0f);
  lua_setfield(L, -2, "one");
  lua_pushvec(L, 0.0f, 1.0f, 0.0f, 0.0f);
  lua_setfield(L, -2, "forward");
  lua_pushvec(L, 1.0f, 0.0f, 0.0f, 0.0f);
  lua_setfield(L, -2, "right");
  lua_pushvec(L, 0.0f, 0.0f, 1.0f, 0.0f);
  lua_setfield(L, -2, "up");

  return 1;
}
