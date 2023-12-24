#pragma once

#include <limits.h>
#include <stddef.h>

#define VLUA_IDSIZE	60

#define VLUA51_MAGIC_VALUE 0x5989C266
#define VLUAU_MAGIC_VALUE 0x85088902

#if defined(_WIN32)
#define VLUA_WIN
#endif

#if defined(MTA_DEBUG)
    #define VLUA_LIB_SUFFIX  "_d"
#else
    #define VLUA_LIB_SUFFIX
#endif

#if defined(WIN32)
    #define VLUA_LIB_PREFIX
    #define VLUA_LIB_EXTENSION   ".dll"
#else
    #define VLUA_LIB_PREFIX
    #define VLUA_LIB_EXTENSION   ".so"
#endif

#ifdef VLUA_EXPORT
    // MTA is expecting 'C' name mangling
    #define VLUA_API extern "C"
#else
    #define VLUA_API extern
#endif 

/* more often than not the libs go together with the core */
#define VLUALIB_API	VLUA_API

typedef int (*lua_CFunction) (struct lua_State *L);

/* type of numbers in Lua */
typedef double lua_Number;
typedef int lua_Integer; // Take int instead of ptrdiff_t to be consistent with Luau

enum ELuaVersion
{
    VLUA_U = 0,
    VLUA_5_1   
};

// Vlua specific functions
VLUA_API bool       (VluaL_init)();
VLUA_API void       (VluaL_close)();

VLUA_API void       (Vlua_close) (struct lua_State *L);

/*
** MTA Specific stuff written by Oli for pre C Function call hooking
*/
typedef int (*lua_PreCallHook) ( lua_CFunction f, struct lua_State* L );
VLUA_API void Vlua_registerPreCallHook ( lua_PreCallHook f );

typedef void (*lua_PostCallHook) ( lua_CFunction f, struct lua_State* L );
VLUA_API void Vlua_registerPostCallHook ( lua_PostCallHook f );

// MTA Specific
typedef int (*lua_UndumpHook) ( const char* p, size_t n );
VLUA_API void Vlua_registerUndumpHook ( lua_UndumpHook f );

VLUA_API int             (Vlua_isnumber) (struct lua_State *L, int idx);

VLUA_API int             (Vlua_type) (struct lua_State *L, int idx);
VLUA_API int             (Vlua_toboolean) (struct lua_State *L, int idx);
VLUA_API const char     *(Vlua_tolstring) (struct lua_State *L, int idx, size_t *len);
VLUA_API size_t          (Vlua_objlen) (struct lua_State *L, int idx);
VLUA_API void	       *(Vlua_touserdata) (struct lua_State *L, int idx);
VLUA_API const void     *(Vlua_topointer) (struct lua_State *L, int idx);
VLUA_API lua_Number      (Vlua_tonumber) (struct lua_State *L, int idx);
VLUA_API const char     *(Vlua_typename) (struct lua_State *L, int tp);

VLUA_API void  (Vlua_setfield) (struct lua_State *L, int idx, const char *k);
VLUA_API void  (Vlua_settable) (struct lua_State *L, int idx);
VLUA_API void  (Vlua_rawseti) (struct lua_State *L, int idx, int n);
VLUA_API void  (Vlua_rawset) (struct lua_State *L, int idx);
VLUA_API int   (Vlua_setmetatable) (struct lua_State *L, int objindex);

VLUA_API void  (Vlua_gettable) (struct lua_State *L, int idx);
VLUA_API void *(Vlua_newuserdata) (struct lua_State *L, size_t sz);
VLUA_API void  (Vlua_rawget) (struct lua_State *L, int idx);
VLUA_API void  (Vlua_createtable) (struct lua_State *L, int narr, int nrec);
VLUA_API void  (Vlua_getfield) (struct lua_State *L, int idx, const char *k);
VLUA_API void  (Vlua_rawgeti) (struct lua_State *L, int idx, int n);

VLUA_API void  (Vlua_pushnil) (struct lua_State *L);
VLUA_API void  (Vlua_pushnumber) (struct lua_State *L, lua_Number n);
VLUA_API void  (Vlua_pushinteger) (struct lua_State *L, lua_Integer n);
VLUA_API void  (Vlua_pushboolean) (struct lua_State *L, int b);
VLUA_API void  (Vlua_pushlstring) (struct lua_State *L, const char *s, size_t l);
VLUA_API void  (Vlua_pushstring) (struct lua_State *L, const char *s);
VLUA_API void  (Vlua_pushlightuserdata) (struct lua_State *L, void *p);
VLUA_API void  (Vlua_pushcclosure) (struct lua_State *L, lua_CFunction fn, int n);
VLUA_API void  (Vlua_insert) (struct lua_State *L, int idx);

// MTA Specific functions.
// ChrML: Added function to get the main state from a lua state that is a coroutine
VLUA_API struct lua_State* (Vlua_getmainstate) (struct lua_State* L);
VLUA_API void *Vlua_getmtasaowner(struct lua_State* L);

VLUA_API void  (Vlua_replace) (struct lua_State *L, int idx);
VLUA_API void  (Vlua_remove) (struct lua_State *L, int idx);
VLUA_API void  (Vlua_pushvalue) (struct lua_State *L, int idx);
VLUA_API int   (Vlua_gettop) (struct lua_State *L);
VLUA_API void  (Vlua_settop) (struct lua_State *L, int idx);
VLUA_API int   (Vlua_checkstack) (struct lua_State *L, int sz);
VLUA_API int   (Vlua_getstackgap) (struct lua_State *L);         // MTA addition

VLUA_API int   (Vlua_next) (struct lua_State *L, int idx);

VLUA_API void  (Vlua_concat) (struct lua_State *L, int n);

VLUA_API void  (Vlua_call) (struct lua_State *L, int nargs, int nresults);
VLUA_API int   (Vlua_pcall) (struct lua_State *L, int nargs, int nresults, int errfunc);

// MTA specific: Returns the number of expected results in a C call.
// Note that this will no longer be reliable if another C function is
// called before calling lua_ncallresult.
// It will also not be reliable in case of incorrectly called functions
// e.g.
//   local a, b = tostring(3)
// will return 2, despite tostring only returning one number
VLUA_API int (Vlua_ncallresult) (struct lua_State* L);

VLUA_API int   (Vlua_error) (struct lua_State *L);

VLUA_API int (Vlua_gc) (struct lua_State *L, int what, int data);

struct Vlua_Debug 
{
  int event;
  const char *name;	/* (n) */
  const char *namewhat;	/* (n) `global', `local', `field', `method' */
  const char *what;	/* (S) `Lua', `C', `main', `tail' */
  const char *source;	/* (S) */
  int currentline;	/* (l) */
  int nups;		/* (u) number of upvalues */
  int linedefined;	/* (S) */
  int lastlinedefined;	/* (S) */
  char short_src[VLUA_IDSIZE]; /* (S) */
  /* private part */
  int i_ci;  /* active function */
};

/* Functions to be called by the debuger in specific events */
typedef void (*Vlua_Hook) (struct lua_State *L, struct Vlua_Debug *ar);

VLUA_API int Vlua_getstack (struct lua_State *L, int level, struct Vlua_Debug *ar);
VLUA_API int Vlua_getinfo (struct lua_State *L, const char *what, struct Vlua_Debug *ar);
VLUA_API int Vlua_sethook (struct lua_State *L, Vlua_Hook func, int mask, int count);
VLUA_API void Vlua_addtotalbytes(struct lua_State *L, int n);

/*
	Lua Lib
*/
VLUALIB_API int (Vluaopen_base) (struct lua_State *L);
VLUALIB_API int (Vluaopen_math) (struct lua_State *L);
VLUALIB_API int (Vluaopen_string) (struct lua_State *L);
VLUALIB_API int (Vluaopen_table) (struct lua_State *L);
VLUALIB_API int (Vluaopen_debug) (struct lua_State *L);
VLUALIB_API int Vluaopen_utf8(struct lua_State *L);
VLUALIB_API int (Vluaopen_os) (struct lua_State *L);

/*
	Lua Aux Lib
*/
VLUALIB_API void (VluaL_register) (struct lua_State *L, const char *libname,
                                const struct luaL_Reg *l);

VLUALIB_API struct lua_State *(VluaL_newstate) (void *mtasaowner, ELuaVersion version = ELuaVersion::VLUA_5_1);

VLUALIB_API int (VluaL_ref) (struct lua_State *L, int t);
VLUALIB_API void (VluaL_unref) (struct lua_State *L, int t, int ref);

VLUALIB_API int (VluaL_loadbuffer) (struct lua_State *L, const char *buff, size_t sz,
                                  const char *name);

VLUALIB_API void (VluaL_where) (lua_State *L, int level);

VLUALIB_API int (VluaL_error) (struct lua_State *L, const char *fmt, ...);

VLUALIB_API int (VluaL_callmeta) (struct lua_State *L, int obj, const char *e);

#if defined(LUA_USE_APICHECK)
VLUA_API int VluaX_is_apicheck_enabled();
#endif

struct Vlua_StateFns
{
    void (*Vlua_close)(lua_State* L);

    // MTA hooks
    void (*Vlua_registerPreCallHook) (lua_PreCallHook f);
    void (*Vlua_registerPostCallHook) (lua_PostCallHook f);
    void (*Vlua_registerUndumpHook) (lua_UndumpHook f);

    int (*Vlua_isnumber)(lua_State* L, int idx);
    int (*Vlua_type)(lua_State* L, int idx);
    int (*Vlua_toboolean)(lua_State* L, int idx);
    const char* (*Vlua_tolstring)(lua_State* L, int idx, size_t* len);
    size_t (*Vlua_objlen) (struct lua_State *L, int idx);
    void* (*Vlua_touserdata)(lua_State* L, int idx);
    const void* (*Vlua_topointer)(lua_State* L, int idx);
    lua_Number (*Vlua_tonumber) (struct lua_State *L, int idx);
    const char* (*Vlua_typename)(lua_State* L, int tp);
    void  (*Vlua_setfield) (lua_State *L, int idx, const char *k);
    void  (*Vlua_settable) (lua_State *L, int idx);
    void  (*Vlua_rawseti) (lua_State *L, int idx, int n);
    void  (*Vlua_rawset) (lua_State *L, int idx);
    int   (*Vlua_setmetatable) (lua_State *L, int objindex);
    void  (*Vlua_gettable) (lua_State *L, int idx);
    void *(*Vlua_newuserdata) (lua_State *L, size_t sz);
    void  (*Vlua_rawget) (lua_State *L, int idx);
    void  (*Vlua_createtable) (lua_State *L, int narr, int nrec);
    void  (*Vlua_getfield) (lua_State *L, int idx, const char *k);
    void  (*Vlua_rawgeti) (lua_State *L, int idx, int n);
    void  (*Vlua_pushnil) (lua_State *L);
    void  (*Vlua_pushnumber) (lua_State *L, lua_Number n);
    void  (*Vlua_pushinteger) (lua_State *L, lua_Integer n);
    void  (*Vlua_pushboolean) (lua_State *L, int b);
    void  (*Vlua_pushlstring) (lua_State *L, const char *s, size_t l);
    void  (*Vlua_pushstring) (lua_State *L, const char *s);
    void  (*Vlua_pushlightuserdata) (lua_State *L, void *p);
    void  (*Vlua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
    void  (*Vlua_insert) (lua_State *L, int idx);
    lua_State* (*Vlua_getmainstate) (lua_State* L);
    void *(*Vlua_getmtasaowner)(lua_State* L);
    void  (*Vlua_replace) (lua_State *L, int idx);
    void  (*Vlua_remove) (lua_State *L, int idx);
    void  (*Vlua_pushvalue) (lua_State *L, int idx);
    int   (*Vlua_gettop) (lua_State *L);
    void  (*Vlua_settop) (lua_State *L, int idx);
    int   (*Vlua_checkstack) (lua_State *L, int sz);
    int   (*Vlua_getstackgap) (lua_State *L);         // MTA addition
    int   (*Vlua_next) (lua_State *L, int idx);
    void  (*Vlua_call) (lua_State *L, int nargs, int nresults);
    int   (*Vlua_pcall) (lua_State *L, int nargs, int nresults, int errfunc);
    int (*Vlua_ncallresult) (lua_State* L);
    int   (*Vlua_error) (lua_State *L);
    int (*Vlua_gc) (lua_State *L, int what, int data);
    int (*Vlua_getstack) (lua_State *L, int level, Vlua_Debug *ar);
    int (*Vlua_getinfo) (lua_State *L, const char *what, Vlua_Debug *ar);
    int (*Vlua_sethook) (lua_State *L, Vlua_Hook func, int mask, int count);
    void (*Vlua_addtotalbytes) (lua_State *L, int n);

    // Libraries
    int (*Vluaopen_base) (lua_State *L);
    int (*Vluaopen_math) (lua_State *L);
    int (*Vluaopen_string) (lua_State *L);
    int (*Vluaopen_table) (lua_State *L);
    int (*Vluaopen_debug) (lua_State *L);
    int (*Vluaopen_utf8) (lua_State *L);
    int (*Vluaopen_os) (lua_State *L);

    // Aux libraries
    void (*VluaL_register) (struct lua_State *L, const char *libname, const struct luaL_Reg *l);
    lua_State *(*VluaL_newstate) (void *mtasaowner);
    int (*VluaL_ref) (struct lua_State *L, int t);
    void (*VluaL_unref) (struct lua_State *L, int t, int ref);

    int (*VluaL_loadbuffer) (struct lua_State *L, const char *buff, size_t sz, const char *name);
    int (*VluaL_error) (struct lua_State *L, const char *fmt, ...);
    int (*VluaL_callmeta) (struct lua_State *L, int obj, const char *e);

    void (*VluaL_where) (struct lua_State *L, int level);
    void (*Vlua_concat) (struct lua_State *L, int n);

    // Debug
#if defined(LUA_USE_APICHECK)
    int (*VluaX_is_apicheck_enabled)();
#endif
};
