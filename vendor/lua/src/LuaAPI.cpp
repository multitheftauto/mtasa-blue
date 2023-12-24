#include "LuaAPI.h"

#include <assert.h>
#include <stdint.h>
#include <cstdarg>
#include <cstdio>

#ifdef VLUA_PROFILER
#else
    #define VLUA_CHECK_STATE(name)
#endif

// Extract a Lua magic value
#define VLUA_MAGIC_VALUE (*(reinterpret_cast<size_t*>(L) - 1))
// Fast Lua version index extraction. Can be changed to support new versions. See LUAI_EXTRASPACE and luai_userstateopen.
#define VLUA_VERSION_IDX static_cast<size_t>(VLUA_MAGIC_VALUE == VLUA51_MAGIC_VALUE)

extern Vlua_StateFns VLuau_Functions; // Luau functions
extern Vlua_StateFns VLua51_Functions; // Lua 5.1 functions

static Vlua_StateFns* Vlua_StateClasses[2] = {       
    &VLuau_Functions, // Luau
    &VLua51_Functions // Lua 5.1
};

extern bool VLuau_init();
extern void VLuau_close();

extern bool VLua51_init();
extern void VLua51_close();

bool VluaL_init()
{
    if (!VLuau_init())
    {
        printf("Could not load VLuau module\n");
        return false;
    }

    if (!VLua51_init())
    {
        printf("Could not load VLua51 module\n");
        return false;
    }
    
    return true;
}

void VluaL_close()
{
    VLuau_close();
    VLua51_close();
}

void (Vlua_close)(lua_State* L)
{
    VLUA_CHECK_STATE(lua_close);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_close(L);
}

void Vlua_registerPreCallHook(lua_PreCallHook f)
{
    Vlua_StateClasses[ELuaVersion::VLUA_5_1]->Vlua_registerPreCallHook(f);
}

void Vlua_registerPostCallHook(lua_PostCallHook f)
{
    Vlua_StateClasses[ELuaVersion::VLUA_5_1]->Vlua_registerPostCallHook(f);
}

void Vlua_registerUndumpHook(lua_UndumpHook f)
{
    Vlua_StateClasses[ELuaVersion::VLUA_5_1]->Vlua_registerUndumpHook(f);
}

int (Vlua_isnumber)(lua_State* L, int idx)
{
    VLUA_CHECK_STATE(lua_isnumber);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_isnumber(L, idx);
}

int (Vlua_type)(lua_State* L, int idx)
{
    VLUA_CHECK_STATE(lua_type);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_type(L, idx);
}

int (Vlua_toboolean)(lua_State* L, int idx)
{
    VLUA_CHECK_STATE(lua_toboolean);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_toboolean(L, idx);
}

const char* (Vlua_tolstring)(lua_State* L, int idx, size_t* len)
{
    VLUA_CHECK_STATE(lua_tolstring);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_tolstring(L, idx, len);
}

size_t (Vlua_objlen) (struct lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_objlen);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_objlen(L, idx);
}

void* (Vlua_touserdata)(lua_State* L, int idx)
{
    VLUA_CHECK_STATE(lua_touserdata);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_touserdata(L, idx);
}

const void* (Vlua_topointer)(lua_State* L, int idx)
{
    VLUA_CHECK_STATE(lua_topointer);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_topointer(L, idx);
}

lua_Number (Vlua_tonumber) (struct lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_tonumber);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_tonumber(L, idx);
}

const char* (Vlua_typename)(lua_State* L, int tp)
{
    VLUA_CHECK_STATE(lua_typename);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_typename(L, tp);
}

void  (Vlua_setfield) (lua_State *L, int idx, const char *k)
{
    VLUA_CHECK_STATE(lua_setfield);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_setfield(L, idx, k);
}

void  (Vlua_settable) (lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_settable);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_settable(L, idx);
}

void  (Vlua_rawseti) (lua_State *L, int idx, int n)
{
    VLUA_CHECK_STATE(lua_rawseti);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_rawseti(L, idx, n);
}

void  (Vlua_rawset) (lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_rawset);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_rawset(L, idx);
}

int   (Vlua_setmetatable) (lua_State *L, int objindex)
{
    VLUA_CHECK_STATE(lua_setmetatable);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_setmetatable(L, objindex);
}

void  (Vlua_gettable) (lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_gettable);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_gettable(L, idx);
}

void *(Vlua_newuserdata) (lua_State *L, size_t sz)
{
    VLUA_CHECK_STATE(lua_newuserdata);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_newuserdata(L, sz);
}

void  (Vlua_rawget) (lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_rawget);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_rawget(L, idx);
}

void  (Vlua_createtable) (lua_State *L, int narr, int nrec)
{
    VLUA_CHECK_STATE(lua_createtable);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_createtable(L, narr, nrec);
}

void  (Vlua_getfield) (lua_State *L, int idx, const char *k)
{
    VLUA_CHECK_STATE(lua_getfield);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_getfield(L, idx, k);
}

void  (Vlua_rawgeti) (lua_State *L, int idx, int n)
{
    VLUA_CHECK_STATE(lua_rawgeti);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_rawgeti(L, idx, n);
}

void  (Vlua_pushnil) (lua_State *L)
{
    VLUA_CHECK_STATE(lua_pushnil);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_pushnil(L);
}

void  (Vlua_pushnumber) (lua_State *L, lua_Number n)
{
    VLUA_CHECK_STATE(lua_pushnumber);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_pushnumber(L, n);
}

void  (Vlua_pushinteger) (lua_State *L, lua_Integer n)
{
    VLUA_CHECK_STATE(lua_pushinteger);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_pushinteger(L, n);
}

void  (Vlua_pushboolean) (lua_State *L, int b)
{
    VLUA_CHECK_STATE(lua_pushboolean);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_pushboolean(L, b);
}

void  (Vlua_pushlstring) (lua_State *L, const char *s, size_t l)
{
    VLUA_CHECK_STATE(lua_pushlstring);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_pushlstring(L, s, l);
}

void  (Vlua_pushstring) (lua_State *L, const char *s)
{
    VLUA_CHECK_STATE(lua_pushstring);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_pushstring(L, s);
}

void  (Vlua_pushlightuserdata) (lua_State *L, void *p)
{
    VLUA_CHECK_STATE(lua_pushlightuserdata);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_pushlightuserdata(L, p);
}

//

void  (Vlua_pushcclosure) (lua_State *L, lua_CFunction fn, int n)
{
    VLUA_CHECK_STATE(lua_pushcclosure);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_pushcclosure(L, fn, n);
}

void  (Vlua_insert) (lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_insert);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_insert(L, idx);
}

// MTA Specific functions.
// ChrML: Added function to get the main state from a lua state that is a coroutine
lua_State* (Vlua_getmainstate) (lua_State* L)
{
    VLUA_CHECK_STATE(lua_getmainstate);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_getmainstate(L);
}

void *Vlua_getmtasaowner(lua_State* L)
{
    VLUA_CHECK_STATE(lua_getmtasaowner);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_getmtasaowner(L);
}

void  (Vlua_replace) (lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_replace);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_replace(L, idx);
}

void  (Vlua_remove) (lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_remove);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_remove(L, idx);
}

void  (Vlua_pushvalue) (lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_pushvalue);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_pushvalue(L, idx);
}

int   (Vlua_gettop) (lua_State *L)
{
    VLUA_CHECK_STATE(lua_gettop);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_gettop(L);
}

void  (Vlua_settop) (lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_settop);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_settop(L, idx);
}

int   (Vlua_checkstack) (lua_State *L, int sz)
{
    VLUA_CHECK_STATE(lua_checkstack);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_checkstack(L, sz);
}

int   (Vlua_getstackgap) (lua_State *L)         // MTA addition
{
    VLUA_CHECK_STATE(lua_getstackgap);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_getstackgap(L);
}

int   (Vlua_next) (lua_State *L, int idx)
{
    VLUA_CHECK_STATE(lua_next);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_next(L, idx);
}

void   (Vlua_concat) (lua_State *L, int n)
{
    VLUA_CHECK_STATE(lua_next);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_concat(L, n);
}

void  (Vlua_call) (lua_State *L, int nargs, int nresults)
{
    VLUA_CHECK_STATE(lua_call);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_call(L, nargs, nresults);
}

int   (Vlua_pcall) (lua_State *L, int nargs, int nresults, int errfunc)
{
    VLUA_CHECK_STATE(lua_pcall);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_pcall(L, nargs, nresults, errfunc);
}

// MTA specific: Returns the number of expected results in a C call.
// Note that this will no longer be reliable if another C function is
// called before calling lua_ncallresult.
// It will also not be reliable in case of incorrectly called functions
// e.g.
//   local a, b = tostring(3)
// will return 2, despite tostring only returning one number
int (Vlua_ncallresult) (lua_State* L)
{
    VLUA_CHECK_STATE(lua_ncallresult);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_ncallresult(L);
}

int   (Vlua_error) (lua_State *L)
{
    VLUA_CHECK_STATE(lua_error);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_error(L);
}

int (Vlua_gc) (lua_State *L, int what, int data)
{
    VLUA_CHECK_STATE(lua_gc);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_gc(L, what, data);
}

int Vlua_getstack (lua_State *L, int level, Vlua_Debug *ar)
{
    VLUA_CHECK_STATE(lua_getstack);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_getstack(L, level, ar);
}

int Vlua_getinfo (lua_State *L, const char *what, Vlua_Debug *ar)
{
    VLUA_CHECK_STATE(lua_getinfo);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_getinfo(L, what, ar);
}

int Vlua_sethook (lua_State *L, Vlua_Hook func, int mask, int count)
{
    VLUA_CHECK_STATE(lua_sethook);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_sethook(L, func, mask, count);
}

void Vlua_addtotalbytes(lua_State *L, int n)
{
    VLUA_CHECK_STATE(lua_addtotalbytes);
    Vlua_StateClasses[VLUA_VERSION_IDX]->Vlua_addtotalbytes(L, n);
}

/*
	Lua Lib
*/
int (Vluaopen_base) (lua_State *L)
{    
    VLUA_CHECK_STATE(luaopen_base);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vluaopen_base(L);
}

int (Vluaopen_math) (lua_State *L)
{
    VLUA_CHECK_STATE(luaopen_math);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vluaopen_math(L);
}

int (Vluaopen_string) (lua_State *L)
{
    VLUA_CHECK_STATE(luaopen_string);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vluaopen_string(L);
}

int (Vluaopen_table) (lua_State *L)
{
    VLUA_CHECK_STATE(luaopen_table);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vluaopen_table(L);
}

int (Vluaopen_debug) (lua_State *L)
{
    VLUA_CHECK_STATE(luaopen_debug);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vluaopen_debug(L);
}

int Vluaopen_utf8(lua_State *L)
{
    VLUA_CHECK_STATE(luaopen_utf8);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vluaopen_utf8(L);
}

int (Vluaopen_os) (lua_State *L)
{
    VLUA_CHECK_STATE(luaopen_os);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->Vluaopen_os(L);
}

/*
	Lua Aux Lib
*/
void (VluaL_register) (struct lua_State *L, const char *libname, const struct luaL_Reg *l)
{
    VLUA_CHECK_STATE(luaL_register);
    Vlua_StateClasses[VLUA_VERSION_IDX]->VluaL_register(L, libname, l);
}

lua_State *(VluaL_newstate) (void *mtasaowner, ELuaVersion version)
{
    return Vlua_StateClasses[static_cast<size_t>(version)]->VluaL_newstate(mtasaowner);
}

int (VluaL_ref) (struct lua_State *L, int t)
{
    VLUA_CHECK_STATE(luaL_ref);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->VluaL_ref(L, t);
}

void (VluaL_unref) (struct lua_State *L, int t, int ref)
{
    VLUA_CHECK_STATE(luaL_unref);
    Vlua_StateClasses[VLUA_VERSION_IDX]->VluaL_unref(L, t, ref);
}

int (VluaL_loadbuffer) (struct lua_State *L, const char *buff, size_t sz, const char *name)
{
    VLUA_CHECK_STATE(luaL_loadbuffer);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->VluaL_loadbuffer(L, buff, sz, name);
}

VLUALIB_API void(VluaL_where)(lua_State* L, int level)
{
    VLUA_CHECK_STATE(luaL_loadbuffer);
    Vlua_StateClasses[VLUA_VERSION_IDX]->VluaL_where(L, level);
}

int (VluaL_error) (struct lua_State *L, const char *fmt, ...)
{
    VLUA_CHECK_STATE(luaL_error);

    va_list argp;
    va_start(argp, fmt);
    int res = Vlua_StateClasses[VLUA_VERSION_IDX]->VluaL_error(L, fmt, argp);
    va_end(argp);

    return res;
}

int (VluaL_callmeta) (struct lua_State *L, int obj, const char *e)
{
    VLUA_CHECK_STATE(luaL_callmeta);
    return Vlua_StateClasses[VLUA_VERSION_IDX]->VluaL_callmeta(L, obj, e);
}

#if defined(LUA_USE_APICHECK)
int VluaX_is_apicheck_enabled()
{
    return Vlua_StateClasses[ELuaVersion::VLUA_5_1]->VluaX_is_apicheck_enabled();
}
#endif