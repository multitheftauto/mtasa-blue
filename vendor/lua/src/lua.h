#pragma once

#include "luaconf.h"

#include "LuaAPI.h"

/* option for multiple returns in `lua_pcall' and `lua_call' */
#define LUA_MULTRET	(-1)

#define LUA_REGISTRYINDEX	(-10000)
#define LUA_GLOBALSINDEX	(-10002)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))

/* thread status; 0 is OK */
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5

#define lua_close(L) Vlua_close((L))

/*
** MTA Specific stuff written by Oli for pre C Function call hooking
*/
#define lua_registerPreCallHook(f) Vlua_registerPreCallHook((f))
#define lua_registerPostCallHook(f) Vlua_registerPostCallHook((f))
#define lua_registerUndumpHook(f) Vlua_registerUndumpHook((f))

/*
** basic types
*/
#define LUA_TNONE		(-1)

#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8

#define lua_isnumber(L,idx) Vlua_isnumber((L), (idx))

#define lua_type(L,idx) Vlua_type((L), (idx))
#define lua_toboolean(L,idx) Vlua_toboolean((L), (idx))
#define lua_tolstring(L,idx,len) Vlua_tolstring((L), (idx), (len))
#define lua_objlen(L,idx) Vlua_objlen((L), (idx))
#define lua_touserdata(L,idx) Vlua_touserdata((L), (idx))
#define lua_topointer(L,idx) Vlua_topointer((L), (idx))
#define lua_tonumber(L,idx) Vlua_tonumber((L), (idx))
#define lua_typename(L,tp) Vlua_typename((L), (tp))

#define lua_setfield(L,idx,k) Vlua_setfield((L),(idx),(k))
#define lua_settable(L,idx) Vlua_settable((L), (idx))
#define lua_rawseti(L,idx,n) Vlua_rawseti((L), (idx), (n))
#define lua_rawset(L,idx) Vlua_rawset((L), (idx))
#define lua_setmetatable(L,objindex) Vlua_setmetatable((L), (objindex))

#define lua_gettable(L,idx) Vlua_gettable((L), (idx))
#define lua_newuserdata(L,sz) Vlua_newuserdata((L), (sz))
#define lua_rawget(L,idx) Vlua_rawget((L), (idx))
#define lua_createtable(L,narr,nrec) Vlua_createtable((L), (narr), (nrec))
#define lua_getfield(L,idx,k) Vlua_getfield((L), (idx), (k))
#define lua_rawgeti(L,idx,n) Vlua_rawgeti((L), (idx), (n))

#define lua_pushnil(L) Vlua_pushnil((L))
#define lua_pushnumber(L,n) Vlua_pushnumber((L), (n))
#define lua_pushinteger(L,n) Vlua_pushinteger((L), (n))
#define lua_pushboolean(L,b) Vlua_pushboolean((L), (b))
#define lua_pushlstring(L,s,l) Vlua_pushlstring((L), (s), (l)) 
#define lua_pushstring(L,s) Vlua_pushstring((L), (s))
#define lua_pushlightuserdata(L,p) Vlua_pushlightuserdata((L), (p))
#define lua_pushcclosure(L,fn,n) Vlua_pushcclosure((L), (fn), (n))
#define lua_insert(L,idx) Vlua_insert((L), (idx))

// MTA Specific functions.
// ChrML: Added function to get the main state from a lua state that is a coroutine
#define lua_getmainstate(L) Vlua_getmainstate((L))
#define lua_getmtasaowner(L) Vlua_getmtasaowner((L))

#define lua_replace(L,idx) Vlua_replace((L), (idx))
#define lua_remove(L,idx) Vlua_remove((L), (idx))
#define lua_pushvalue(L,idx) Vlua_pushvalue((L), (idx))
#define lua_gettop(L) Vlua_gettop((L))
#define lua_settop(L,idx) Vlua_settop((L), (idx))
#define lua_checkstack(L,sz) Vlua_checkstack((L), (sz))
#define lua_getstackgap(L) Vlua_getstackgap((L))         // MTA addition

#define lua_next(L,idx) Vlua_next((L), (idx))

#define lua_concat(L,n) Vlua_concat((L), (n))

#define lua_call(L,nargs,nresults) Vlua_call((L), (nargs), (nresults))
#define lua_pcall(L,nargs,nresults,errfunc) Vlua_pcall((L), (nargs), (nresults), (errfunc))

// MTA specific: Returns the number of expected results in a C call.
// Note that this will no longer be reliable if another C function is
// called before calling lua_ncallresult.
// It will also not be reliable in case of incorrectly called functions
// e.g.
//   local a, b = tostring(3)
// will return 2, despite tostring only returning one number
#define lua_ncallresult(L) Vlua_ncallresult((L))

#define lua_error(L) Vlua_error((L))

/*
** garbage-collection function and options
*/
#define LUA_GCSTOP		0
#define LUA_GCRESTART		1
#define LUA_GCCOLLECT		2
#define LUA_GCCOUNT		3
#define LUA_GCCOUNTB		4
#define LUA_GCSTEP		5
#define LUA_GCSETPAUSE		6
#define LUA_GCSETSTEPMUL	7

#define lua_gc(L,what,data) Vlua_gc((L), (what), (data))

#define lua_open(mtasaowner)	    luaL_newstate(mtasaowner)
#define lua_getgccount(L)	lua_gc(L, LUA_GCCOUNT, 0)

#define lua_newtable(L)		lua_createtable(L, 0, 0)
#define lua_pop(L,n)		lua_settop(L, -(n)-1)
#define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)
#define lua_strlen(L,i)		lua_objlen(L, (i))
#define lua_isnil(L,n)		(lua_type(L, (n)) == LUA_TNIL)
#define lua_istable(L,n)	(lua_type(L, (n)) == LUA_TTABLE)
#define lua_isfunction(L,n)	(lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_getglobal(L,s)	lua_getfield(L, LUA_GLOBALSINDEX, (s))
#define lua_setglobal(L,s)	lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_pushcfunction(L,f)	lua_pushcclosure(L, (f), 0)
#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))
#define lua_pushliteral(L, s)	\
	lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)
#define lua_isnoneornil(L, n)	(lua_type(L, (n)) <= 0)

/*
** Event codes
*/
#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILRET 4

/*
** Event masks
*/
#define LUA_MASKCALL	(1 << LUA_HOOKCALL)
#define LUA_MASKRET	(1 << LUA_HOOKRET)
#define LUA_MASKLINE	(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT	(1 << LUA_HOOKCOUNT)

#define lua_getstack(L,level,ar) Vlua_getstack((L), (level), (ar))
#define lua_getinfo(L,what,ar) Vlua_getinfo((L), (what), (ar))
#define lua_sethook(L,func,mask,count) Vlua_sethook((L), (func), (mask), (count))
#define lua_addtotalbytes(L,n) Vlua_addtotalbytes((L), (n))

typedef Vlua_Debug lua_Debug;
