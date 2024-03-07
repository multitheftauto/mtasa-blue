#include "LuaAPI.h"

#include <cassert>

#include "../lua51/lua.h"
#include "../lua51/lualib.h"
#include "../lua51/lauxlib.h"

#ifdef VLUA_WIN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#define VLUA_IMPORT_FN(ret,nm) const char* nm##_name = #nm; ret (*nm)

static struct
{
    VLUA_IMPORT_FN(void, lua_close)(lua_State* L);

    // MTA hooks
    VLUA_IMPORT_FN(void, lua_registerPreCallHook) (lua_PreCallHook f);
    VLUA_IMPORT_FN(void, lua_registerPostCallHook) (lua_PostCallHook f);
    VLUA_IMPORT_FN(void, lua_registerUndumpHook) (lua_UndumpHook f);

    VLUA_IMPORT_FN(int, lua_isnumber)(lua_State* L, int idx);
    VLUA_IMPORT_FN(int, lua_type)(lua_State* L, int idx);
    VLUA_IMPORT_FN(int, lua_toboolean)(lua_State* L, int idx);
    VLUA_IMPORT_FN(const char*, lua_tolstring)(lua_State* L, int idx, size_t* len);
    VLUA_IMPORT_FN(size_t, lua_objlen) (struct lua_State *L, int idx);
    VLUA_IMPORT_FN(void*, lua_touserdata)(lua_State* L, int idx);
    VLUA_IMPORT_FN(const void*, lua_topointer)(lua_State* L, int idx);
    VLUA_IMPORT_FN(lua_Number, lua_tonumber) (struct lua_State *L, int idx);
    VLUA_IMPORT_FN(const char*, lua_typename)(lua_State* L, int tp);
    VLUA_IMPORT_FN(void, lua_setfield) (lua_State *L, int idx, const char *k);
    VLUA_IMPORT_FN(void, lua_settable) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void, lua_rawseti) (lua_State *L, int idx, int n);
    VLUA_IMPORT_FN(void, lua_rawset) (lua_State *L, int idx);
    VLUA_IMPORT_FN(int, lua_setmetatable) (lua_State *L, int objindex);
    VLUA_IMPORT_FN(void, lua_gettable) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void*, lua_newuserdata) (lua_State *L, size_t sz);
    VLUA_IMPORT_FN(void, lua_rawget) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void, lua_createtable) (lua_State *L, int narr, int nrec);
    VLUA_IMPORT_FN(void, lua_getfield) (lua_State *L, int idx, const char *k);
    VLUA_IMPORT_FN(void, lua_rawgeti) (lua_State *L, int idx, int n);
    VLUA_IMPORT_FN(void, lua_pushnil) (lua_State *L);
    VLUA_IMPORT_FN(void, lua_pushnumber) (lua_State *L, lua_Number n);
    VLUA_IMPORT_FN(void, lua_pushinteger) (lua_State *L, lua_Integer n);
    VLUA_IMPORT_FN(void, lua_pushboolean) (lua_State *L, int b);
    VLUA_IMPORT_FN(void, lua_pushlstring) (lua_State *L, const char *s, size_t l);
    VLUA_IMPORT_FN(void, lua_pushstring) (lua_State *L, const char *s);
    VLUA_IMPORT_FN(void, lua_pushlightuserdata) (lua_State *L, void *p);
    VLUA_IMPORT_FN(void, lua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
    VLUA_IMPORT_FN(void, lua_insert) (lua_State *L, int idx);
    VLUA_IMPORT_FN(lua_State*, lua_getmainstate) (lua_State* L);
    VLUA_IMPORT_FN(void*, lua_getmtasaowner)(lua_State* L);
    VLUA_IMPORT_FN(void, lua_replace) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void, lua_remove) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void, lua_pushvalue) (lua_State *L, int idx);
    VLUA_IMPORT_FN(int, lua_gettop) (lua_State *L);
    VLUA_IMPORT_FN(void, lua_settop) (lua_State *L, int idx);
    VLUA_IMPORT_FN(int, lua_checkstack) (lua_State *L, int sz);
    VLUA_IMPORT_FN(int, lua_getstackgap) (lua_State *L);         // MTA addition
    VLUA_IMPORT_FN(int, lua_next) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void, lua_call) (lua_State *L, int nargs, int nresults);
    VLUA_IMPORT_FN(int, lua_pcall) (lua_State *L, int nargs, int nresults, int errfunc);
    VLUA_IMPORT_FN(int, lua_ncallresult) (lua_State* L);
    VLUA_IMPORT_FN(int, lua_error) (lua_State *L);
    VLUA_IMPORT_FN(int, lua_gc) (lua_State *L, int what, int data);
    VLUA_IMPORT_FN(int, lua_getstack) (lua_State *L, int level, lua_Debug *ar);
    VLUA_IMPORT_FN(int, lua_getinfo) (lua_State *L, const char *what, lua_Debug *ar);
    VLUA_IMPORT_FN(int, lua_sethook) (lua_State *L, lua_Hook func, int mask, int count);
    VLUA_IMPORT_FN(void, lua_addtotalbytes) (lua_State *L, int n);

    // Libraries
    VLUA_IMPORT_FN(int, luaopen_base) (lua_State *L);
    VLUA_IMPORT_FN(int, luaopen_math) (lua_State *L);
    VLUA_IMPORT_FN(int, luaopen_string) (lua_State *L);
    VLUA_IMPORT_FN(int, luaopen_table) (lua_State *L);
    VLUA_IMPORT_FN(int, luaopen_debug) (lua_State *L);
    VLUA_IMPORT_FN(int, luaopen_utf8) (lua_State *L);
    VLUA_IMPORT_FN(int, luaopen_os) (lua_State *L);

    // Aux libraries
    VLUA_IMPORT_FN(void, luaL_register) (struct lua_State *L, const char *libname, const struct luaL_Reg *l);
    VLUA_IMPORT_FN(lua_State*, luaL_newstate) (void *mtasaowner);
    VLUA_IMPORT_FN(int, luaL_ref) (struct lua_State *L, int t);
    VLUA_IMPORT_FN(void, luaL_unref) (struct lua_State *L, int t, int ref);

    VLUA_IMPORT_FN(int, luaL_loadbuffer) (struct lua_State *L, const char *buff, size_t sz, const char *name);
    VLUA_IMPORT_FN(int, luaL_error) (struct lua_State *L, const char *fmt, ...);
    VLUA_IMPORT_FN(int, luaL_callmeta) (struct lua_State *L, int obj, const char *e);

    VLUA_IMPORT_FN(void, luaL_where) (struct lua_State *L, int level);
    VLUA_IMPORT_FN(void, lua_concat) (struct lua_State *L, int n);

    // Debug
#if defined(LUA_USE_APICHECK)
    VLUA_IMPORT_FN(int, luaX_is_apicheck_enabled)();
#endif
} g_Lua51;

Vlua_StateFns VLua51_Functions = {};

#ifdef VLUA_WIN
static HMODULE VLua_hmodule = nullptr;
#else
static void* VLua_hmodule = nullptr;
#endif

static_assert(sizeof(void*) == sizeof(void(*)()), "Invalid pointer size");
#ifdef VLUA_WIN
static_assert(sizeof(void*) == sizeof(HMODULE), "Invalid handle size");
#endif

bool VLua51_init()
{
    assert(VLua_hmodule == nullptr);

    const static char* Vlua51_libname = VLUA_PLATFORM VLUA_LIB_PREFIX "lua5.1" VLUA_LIB_SUFFIX VLUA_LIB_EXTENSION;

#ifdef VLUA_WIN
    VLua_hmodule = LoadLibrary(Vlua51_libname);    
#else
    VLua_hmodule = dlopen(Vlua51_libname, RTLD_LAZY);
#endif

    if (VLua_hmodule == nullptr)
    {
        return false;
    }

    const static size_t ptrBlockSize = sizeof(void*) * 2;
    const size_t fnsNum = sizeof(g_Lua51) / ptrBlockSize;

    for (size_t i = 0; i < fnsNum; ++i)
    {
        const char* fnName = reinterpret_cast<const char**>(&g_Lua51)[i*2 + 0];
#ifdef VLUA_WIN
        FARPROC& pFn = reinterpret_cast<FARPROC*>(&g_Lua51)[i*2 + 1];
        pFn = GetProcAddress(VLua_hmodule, fnName);
#else
        void*& pFn = reinterpret_cast<void**>(&g_Lua51)[i*2 + 1];
        pFn = dlsym(VLua_hmodule, fnName);
#endif
        assert(pFn != nullptr);  
    }

    // Remap Lua 5.1 functions to the MTA-compatible interface
    VLua51_Functions = {
        g_Lua51.lua_close,

        // MTA hooks
        g_Lua51.lua_registerPreCallHook,
        g_Lua51.lua_registerPostCallHook,
        g_Lua51.lua_registerUndumpHook,

        g_Lua51.lua_isnumber,
        g_Lua51.lua_type,
        g_Lua51.lua_toboolean,
        g_Lua51.lua_tolstring,
        g_Lua51.lua_objlen,
        g_Lua51.lua_touserdata,
        g_Lua51.lua_topointer,
        g_Lua51.lua_tonumber,
        g_Lua51.lua_typename,
        g_Lua51.lua_setfield,
        g_Lua51.lua_settable,
        g_Lua51.lua_rawseti,
        g_Lua51.lua_rawset,
        g_Lua51.lua_setmetatable,
        g_Lua51.lua_gettable,
        g_Lua51.lua_newuserdata,
        g_Lua51.lua_rawget,
        g_Lua51.lua_createtable,
        g_Lua51.lua_getfield,
        g_Lua51.lua_rawgeti,
        g_Lua51.lua_pushnil,
        g_Lua51.lua_pushnumber,
        g_Lua51.lua_pushinteger,
        g_Lua51.lua_pushboolean,
        g_Lua51.lua_pushlstring,
        g_Lua51.lua_pushstring,
        g_Lua51.lua_pushlightuserdata,
        g_Lua51.lua_pushcclosure,
        g_Lua51.lua_insert,
        g_Lua51.lua_getmainstate,
        g_Lua51.lua_getmtasaowner,
        g_Lua51.lua_replace,
        g_Lua51.lua_remove,
        g_Lua51.lua_pushvalue,
        g_Lua51.lua_gettop,
        g_Lua51.lua_settop,
        g_Lua51.lua_checkstack,
        g_Lua51.lua_getstackgap,
        g_Lua51.lua_next,
        g_Lua51.lua_call,
        g_Lua51.lua_pcall,
        g_Lua51.lua_ncallresult,
        g_Lua51.lua_error,
        g_Lua51.lua_gc,
        reinterpret_cast<int(*) (lua_State*, int, Vlua_Debug*)>(g_Lua51.lua_getstack),
        reinterpret_cast<int(*) (lua_State*, const char*, Vlua_Debug*)>(g_Lua51.lua_getinfo),
        reinterpret_cast<int(*) (lua_State*, Vlua_Hook, int, int)>(g_Lua51.lua_sethook),
        g_Lua51.lua_addtotalbytes,

        // Libraries
        g_Lua51.luaopen_base,
        g_Lua51.luaopen_math,
        g_Lua51.luaopen_string,
        g_Lua51.luaopen_table,
        g_Lua51.luaopen_debug,
        g_Lua51.luaopen_utf8,
        g_Lua51.luaopen_os,

        // Aux libraries
        g_Lua51.luaL_register,
        g_Lua51.luaL_newstate,
        g_Lua51.luaL_ref,
        g_Lua51.luaL_unref,

        g_Lua51.luaL_loadbuffer,
        g_Lua51.luaL_error,
        g_Lua51.luaL_callmeta,

        g_Lua51.luaL_where,
        g_Lua51.lua_concat,

    #if defined(LUA_USE_APICHECK)
        g_Lua51.luaX_is_apicheck_enabled
    #endif
    };

    return true;
}

void VLua51_close()
{
    assert(VLua_hmodule != nullptr);
#ifdef VLUA_WIN
    FreeLibrary(VLua_hmodule);
#else
    dlclose(VLua_hmodule);
#endif
}
