/*
** LUA-VEC
** Lua Vectors
** See Copyright Notice in lua.h
*/

#define lvector_c
#define LUA_CORE

#include "lua.h"

#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lvector.h"

static GCObject *head = NULL;

Vector *luaVec_new (lua_State *L, float x, float y, float z, float w) {
  Vector *v;
  if (head) {
    v = gco2v(head);
    head = head->gch.next;
  } else {
    v = luaM_new(L, Vector);
  }
  luaC_link(L, obj2gco(v), LUA_TVEC);
  v->vec[0] = x;
  v->vec[1] = y;
  v->vec[2] = z;
  v->vec[3] = w;
  return v;
}

void luaVec_free (lua_State *L, GCObject *o) {
  o->gch.next = head;
  head = o;
}
