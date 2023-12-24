#include "LuaAPI.h"

#include <cassert>
#include <algorithm>
#include <cstring>

#include "../luau/VM/include/lualib.h"
#include "../luau/Compiler/include/luacode.h"

#ifdef VLUA_WIN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#define VLUA_IMPORT_FN(ret,nm) const char* nm##_name = #nm; ret (*nm)

static struct
{
    VLUA_IMPORT_FN(int, lua_type) (lua_State* L, int idx);
    VLUA_IMPORT_FN(int, lua_objlen) (struct lua_State *L, int idx);
    VLUA_IMPORT_FN(double, lua_tonumberx) (lua_State* L, int idx, int* isnum);
    VLUA_IMPORT_FN(int, lua_gettable) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void*, lua_newuserdatatagged)(lua_State* L, size_t sz, int tag);
    VLUA_IMPORT_FN(int, lua_rawget) (lua_State *L, int idx);
    VLUA_IMPORT_FN(int, lua_getfield) (lua_State *L, int idx, const char *k);
    VLUA_IMPORT_FN(int, lua_rawgeti) (lua_State *L, int idx, int n);
    VLUA_IMPORT_FN(void, lua_pushcclosurek) (lua_State* L, lua_CFunction fn, const char* debugname, int nup, lua_Continuation cont);
    VLUA_IMPORT_FN(lua_State*, lua_mainthread) (lua_State* L);
    VLUA_IMPORT_FN(void*, lua_getthreaddata) (lua_State* L);
    VLUA_IMPORT_FN(void, lua_error) (lua_State *L);
    VLUA_IMPORT_FN(int, lua_gc) (lua_State *L, int what, int data);
    VLUA_IMPORT_FN(int, lua_getinfo) (lua_State* L, int level, const char* what, lua_Debug* ar);
    VLUA_IMPORT_FN(void, lua_settop) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void, lua_pushboolean) (lua_State *L, int b);
    VLUA_IMPORT_FN(int, luaopen_coroutine) (lua_State* L);
    VLUA_IMPORT_FN(int, luaopen_base) (lua_State *L);
    VLUA_IMPORT_FN(void, lua_setfield) (lua_State *L, int idx, const char *k);
    VLUA_IMPORT_FN(lua_State*, lua_newstate) (lua_Alloc f, void* ud);
    VLUA_IMPORT_FN(void, lua_setthreaddata) (lua_State* L, void* data);
    VLUA_IMPORT_FN(int, lua_ref) (lua_State* L, int idx);
    VLUA_IMPORT_FN(void, lua_unref) (lua_State* L, int ref);
    VLUA_IMPORT_FN(char*, luau_compile) (const char* source, size_t size, lua_CompileOptions* options, size_t* outsize);
    VLUA_IMPORT_FN(int, luau_load) (lua_State* L, const char* chunkname, const char* data, size_t size, int env);
    VLUA_IMPORT_FN(void, luaL_errorL) (lua_State* L, const char* fmt, ...);

    VLUA_IMPORT_FN(void, lua_close)(lua_State* L);
    VLUA_IMPORT_FN(int, lua_isnumber)(lua_State* L, int idx);  
    VLUA_IMPORT_FN(int, lua_toboolean)(lua_State* L, int idx);
    VLUA_IMPORT_FN(const char*, lua_tolstring)(lua_State* L, int idx, size_t* len);
    VLUA_IMPORT_FN(const void*, lua_topointer)(lua_State* L, int idx);
    VLUA_IMPORT_FN(const char*, lua_typename)(lua_State* L, int tp);
    VLUA_IMPORT_FN(void, lua_settable) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void, lua_rawseti) (lua_State *L, int idx, int n);
    VLUA_IMPORT_FN(void, lua_rawset) (lua_State *L, int idx);
    VLUA_IMPORT_FN(int, lua_setmetatable) (lua_State *L, int objindex);
    VLUA_IMPORT_FN(void, lua_createtable) (lua_State *L, int narr, int nrec);
    VLUA_IMPORT_FN(void, lua_pushnil) (lua_State *L);
    VLUA_IMPORT_FN(void, lua_pushnumber) (lua_State *L, lua_Number n);
    VLUA_IMPORT_FN(void, lua_pushinteger) (lua_State *L, lua_Integer n);
    VLUA_IMPORT_FN(void, lua_pushlstring) (lua_State *L, const char *s, size_t l);
    VLUA_IMPORT_FN(void, lua_pushstring) (lua_State *L, const char *s);
    VLUA_IMPORT_FN(void, lua_pushlightuserdata) (lua_State *L, void *p);
    VLUA_IMPORT_FN(void, lua_insert) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void, lua_replace) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void, lua_remove) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void, lua_pushvalue) (lua_State *L, int idx);
    VLUA_IMPORT_FN(int, lua_gettop) (lua_State *L);
    VLUA_IMPORT_FN(int, lua_checkstack) (lua_State *L, int sz);
    VLUA_IMPORT_FN(int, lua_next) (lua_State *L, int idx);
    VLUA_IMPORT_FN(void, lua_call) (lua_State *L, int nargs, int nresults);
    VLUA_IMPORT_FN(int, lua_pcall) (lua_State *L, int nargs, int nresults, int errfunc);
    VLUA_IMPORT_FN(int, luaopen_math) (lua_State *L);
    VLUA_IMPORT_FN(int, luaopen_string) (lua_State *L);
    VLUA_IMPORT_FN(int, luaopen_table) (lua_State *L);
    VLUA_IMPORT_FN(int, luaopen_debug) (lua_State *L);
    VLUA_IMPORT_FN(int, luaopen_utf8) (lua_State *L);
    VLUA_IMPORT_FN(int, luaopen_os) (lua_State *L);
    VLUA_IMPORT_FN(void, luaL_register) (struct lua_State *L, const char *libname, const struct luaL_Reg *l);
    VLUA_IMPORT_FN(int, luaL_callmeta) (struct lua_State *L, int obj, const char *e);
    VLUA_IMPORT_FN(void*, lua_touserdata) (lua_State* L, int idx);

    VLUA_IMPORT_FN(int, luaL_checkoption) (lua_State* L, int narg, const char* def, const char* const lst[]);
    VLUA_IMPORT_FN(int, luaL_optinteger) (lua_State* L, int nArg, int def);
    VLUA_IMPORT_FN(lua_Callbacks*, lua_callbacks) (lua_State* L);

    VLUA_IMPORT_FN(void, luaL_where) (struct lua_State *L, int level);
    VLUA_IMPORT_FN(void, lua_concat) (struct lua_State *L, int n);

    // Custom function
    VLUA_IMPORT_FN(int, lua_nresults) (lua_State* L);
    VLUA_IMPORT_FN(int, lua_getstackgap) (lua_State* L);
    VLUA_IMPORT_FN(void, lua_addtotalbytes) (lua_State *L, int n);
} g_Luau;

void Vluau_registerPreCallHook(lua_PreCallHook f)
{
    // TODO
}

void Vluau_registerPostCallHook(lua_PostCallHook f)
{
    // TODO
}

void Vluau_registerUndumpHook(lua_UndumpHook f)
{
    // TODO
}

static const int VluaTypeLookup[]
{
    0, // LUA_TNIL
    1, // LUA_TBOOLEAN


    2, // LUA_TLIGHTUSERDATA
    3, // LUA_TNUMBER
    0, // LUA_TVECTOR [Unsupported by Lua 5.1. Return nil instead.]

    4, // LUA_TSTRING


    5, // LUA_TTABLE,
    6, // LUA_TFUNCTION,
    7, // LUA_TUSERDATA,
    8, // LUA_TTHREAD,

    // values below this line are used in GCObject tags but may never show up in TValue type tags
    0, // LUA_TPROTO,
    0, // LUA_TUPVAL,
    0, // LUA_TDEADKEY,
};

int (Vluau_type)(lua_State* L, int idx)
{
    int res = g_Luau.lua_type(L, idx);
    if (res >= 0)
        return VluaTypeLookup[res];

    return -1; // None
}

size_t (Vluau_objlen) (struct lua_State *L, int idx)
{
    int res = g_Luau.lua_objlen(L, idx);
    if (res >= 0)
        return static_cast<size_t>(res);

    // TODO: Warning
    return 0;
}

lua_Number (Vluau_tonumber) (struct lua_State *L, int idx)
{
    return g_Luau.lua_tonumberx(L, idx, NULL);
}

void  (Vluau_gettable) (lua_State *L, int idx)
{
    g_Luau.lua_gettable(L, idx);
}

void *(Vluau_newuserdata) (lua_State *L, size_t sz)
{
    return g_Luau.lua_newuserdatatagged(L, sz, 0); // lua_newuserdata macro
}

void  (Vluau_rawget) (lua_State *L, int idx)
{
    g_Luau.lua_rawget(L, idx);
}

void  (Vluau_getfield) (lua_State *L, int idx, const char *k)
{
    g_Luau.lua_getfield(L, idx, k);
}

void  (Vluau_rawgeti) (lua_State *L, int idx, int n)
{
    g_Luau.lua_rawgeti(L, idx, n);
}

void  (Vluau_pushcclosure) (lua_State *L, lua_CFunction fn, int n)
{
    g_Luau.lua_pushcclosurek(L, fn, "", n, NULL);
}

// MTA Specific functions.
// ChrML: Added function to get the main state from a lua state that is a coroutine
lua_State* (Vluau_getmainstate) (lua_State* L)
{
    if (L)
    {
        lua_State* Result = g_Luau.lua_mainthread(L);
        if (Result)
            L = Result;
    }
    return L;
}

void *Vluau_getmtasaowner(lua_State* L)
{
    if (L)
        return g_Luau.lua_getthreaddata(L);

    return nullptr;
}

int   (Vluau_error) (lua_State *L)
{
    g_Luau.lua_error(L);
    return 0;  /* to avoid warnings */
}

static const int VLuaGCLookup[] = {
    0, // LUA_GCSTOP,
    1, // LUA_GCRESTART
    2, // LUA_GCCOLLECT
    3, // LUA_GCCOUNT
    4, // LUA_GCCOUNTB
    6, // LUA_GCSTEP,

    -1, // Not supported by LuaU

    8, // LUA_GCSETSTEPMUL
};

int (Vluau_gc) (lua_State *L, int what, int data)
{
    assert(what != 6); // Not supported by LuaU directly
    return g_Luau.lua_gc(L, VLuaGCLookup[what], data);
}

static void VluaDebugCtor(Vlua_Debug *ar)
{
    // Unused by MTA
    ar->event = 0; // TODO
    ar->name = "";	/* (n) */        
    ar->namewhat = ""; // TODO	/* (n) `global', `local', `field', `method' */
    ar->what = "";	/* (S) `Lua', `C', `main', `tail' */
    ar->nups = 0;		/* (u) number of upvalues */
    ar->lastlinedefined = 0;	/* (S) */

    ar->source = "";	/* (S) */
    ar->currentline = 0;	/* (l) */        
    ar->linedefined = 0;	/* (S) */    
    std::memset(ar->short_src, 0, VLUA_IDSIZE); /* (S) */
}

static void VluaDebugSet(Vlua_Debug *ar, lua_Debug& ldebug)
{
    // Unused by MTA
    ar->event = 0; // TODO
    ar->name = ldebug.name;	/* (n) */        
    ar->namewhat = ""; // TODO	/* (n) `global', `local', `field', `method' */
    ar->what = ldebug.what;	/* (S) `Lua', `C', `main', `tail' */
    ar->nups = ldebug.nupvals;		/* (u) number of upvalues */
    ar->lastlinedefined = ldebug.linedefined;	/* (S) */

    ar->source = ldebug.source;	/* (S) */
    ar->currentline = ldebug.currentline;	/* (l) */        
    ar->linedefined = ldebug.linedefined;	/* (S) */    
    if (ldebug.short_src)
        std::memcpy(ar->short_src, ldebug.short_src, std::min(std::strlen(ldebug.short_src) + 1, static_cast<size_t>(VLUA_IDSIZE))); /* (S) */
    ar->short_src[VLUA_IDSIZE - 1] = 0; // Null terminator
}

int Vluau_getstack (lua_State *L, int level, Vlua_Debug *ar)
{
    lua_Debug ldebug;
    if (g_Luau.lua_getinfo(L, level, "n", &ldebug))
    {
        ar->i_ci = level;
        return 1;
    }

    return 0;
}

int Vluau_getinfo (lua_State *L, const char *what, Vlua_Debug *ar)
{
    int level;
    const bool bFn = what[0] == '>';
    if (bFn)
    {
        level = 0;
        ++what; // Skip this char        
    }
    else
        level = ar->i_ci;

    char buffer[8];

    // Remap what symbols
    size_t i = 0;
    for (; *what; what++)
    {
        switch (*what)
        {
        case 'f':
        {
            // pushes onto the stack the function that is running at the given level
            // Not supported
            break;
        }
        case 'l':
        {
            // fills in the field currentline
            buffer[i++] = 'l';
            break;
        }
        case 'n':
        {
            // fills in the fields name and namewhat
            buffer[i++] = 'n';
            break;
        }
        case 'r':
        {
            // fills in the fields ftransfer and ntransfer
            // Not supported
            break;
        }
        case 'S':
        {
            // fills in the fields source, short_src, linedefined, lastlinedefined, and what
            buffer[i++] = 's';
            break;
        }
        case 't':
        {
            // fills in the field istailcall
            // Not supported
            break;
        }
        case 'u':
        {
            // fills in the fields nups, nparams, and isvararg
            buffer[i++] = 'u';
            break;
        }
        case 'L':
        {
            // pushes onto the stack a table whose indices are the lines on the function with some associated code
            // Not supported
            break;
        }
        default:
            break;
        }
    }

    VluaDebugCtor(ar);

    lua_Debug ldebug;
    if (g_Luau.lua_getinfo(L, level, buffer, &ldebug))
    {
        VluaDebugSet(ar, ldebug);

        if (bFn)
        {
            // lua_getinfo pops the function from the top of the stack
            g_Luau.lua_settop(L, -(1)-1); // lua_pop macro
        }

        return 1;
    }

    if (bFn)
    {
        // lua_getinfo pops the function from the top of the stack
        g_Luau.lua_settop(L, -(1)-1); // lua_pop macro
    }

    return 0;
}

int Vluau_sethook (lua_State *L, Vlua_Hook func, int mask, int count)
{
    // TODO: MTA uses it with only LUA_MASKCOUNT
    return 0;
}

static int Vluau_collectgarbage (lua_State *L) 
{
    static const char *const opts[] = {
        "stop", "restart", "collect",
        "count", "step", "setpause", "setstepmul", NULL
    };
    static const int optsnum[] = {
        LUA_GCSTOP, LUA_GCRESTART, LUA_GCCOLLECT,
        LUA_GCCOUNT, LUA_GCSTEP, 
        LUA_GCCOLLECT, // Pausing is unsupported in Luau. Collect instead.
        LUA_GCSETSTEPMUL
    };
    const int o = g_Luau.luaL_checkoption(L, 1, "collect", opts);
    const int ex = g_Luau.luaL_optinteger(L, 2, 0);

    const int res = g_Luau.lua_gc(L, optsnum[o], ex);
    switch (optsnum[o]) 
    {
    case LUA_GCCOUNT: 
    {
        int b = g_Luau.lua_gc(L, LUA_GCCOUNTB, 0);
        g_Luau.lua_pushnumber(L, res + ((lua_Number)b/1024));
        return 1;
        
    }
    case LUA_GCSTEP: 
    {
        g_Luau.lua_pushboolean(L, res);
        return 1;
    }
    default: 
    {
        g_Luau.lua_pushnumber(L, res);
        return 1;
    }
    }
}

int (Vluauopen_base) (lua_State *L)
{   
    // In Luau corounines are within a separate library
    if (!g_Luau.luaopen_coroutine(L))
        return 0;

    if (!g_Luau.luaopen_base(L))
        return 0;

    // Add missing functions
    g_Luau.lua_pushcclosurek(L, Vluau_collectgarbage, "collectgarbage", 0, NULL);
    g_Luau.lua_setfield(L, LUA_GLOBALSINDEX, "collectgarbage");

    return 1;
}

static void* l_alloc(void* ud, void* ptr, size_t osize, size_t nsize)
{
    (void)ud;
    (void)osize;
    if (nsize == 0)
    {
        // Remap Luau ptr to the actuall alloc address
        if (ptr)
            ptr = static_cast<size_t*>(ptr) - 1;

        free(ptr);
        return NULL;
    }
    else
    {
        // Remap Luau ptr to the actuall alloc address
        if (ptr)
            ptr = static_cast<size_t*>(ptr) - 1;
        
        // Make sure that we have 4 extra bytes in front of lua_State(and not only)
        ptr = realloc(ptr, nsize + sizeof(size_t));

        // Remap the actuall address to Luau ptr
        if (ptr)
            ptr = static_cast<size_t*>(ptr) + 1;

        return ptr;
    }
}

void Vluau_userthread (lua_State* LP, lua_State* L)
{
    if (LP)
        *((size_t*)LP - 0x1) = *((size_t*)L - 0x1);
}

lua_State *(VluauL_newstate) (void *mtasaowner)
{
    // We are using a custom allocator here to ensure that we always have 4 extra bytes in front of lua_States
    // It is waste of memory and must be replaced with a Luau internal solution(like default Lua do)
    lua_State* L = g_Luau.lua_newstate(l_alloc, NULL);
    if (L)
    {
        *((size_t*)L - 0x1) = VLUAU_MAGIC_VALUE;

        g_Luau.lua_setthreaddata(L, mtasaowner);

        lua_Callbacks* cb = g_Luau.lua_callbacks(L);
        cb->userthread = &Vluau_userthread;
    }

    return L;
}

int (VluauL_ref) (struct lua_State *L, int t)
{
    // If the object at the top of the stack is nil, luaL_ref returns the constant LUA_REFNIL
    if (t != LUA_REGISTRYINDEX || g_Luau.lua_type(L, -1) == LUA_TNIL)
        return LUA_NOREF;

    // lua_ref in Luau doesn't change the stack in any way, it simply pins the object and returns the reference.
    const int ref = g_Luau.lua_ref(L, -1);
    g_Luau.lua_settop(L, -(1)-1); // lua_pop macro
    return ref;
}

void (VluauL_unref) (struct lua_State *L, int t, int ref)
{
    if (t == LUA_REGISTRYINDEX)
        g_Luau.lua_unref(L, ref);
}

int (VluauL_loadbuffer) (struct lua_State *L, const char *buff, size_t sz, const char *name)
{
    size_t bytecodeSize = 0;
    char* bytecode = g_Luau.luau_compile(buff, sz, NULL, &bytecodeSize);
    if (!bytecode)
        return 4;

    if (!name)
        name = "";

    int result = g_Luau.luau_load(L, name, bytecode, bytecodeSize, 0);
    free(bytecode);

    return result != 0 ? 3 : 0;
}

int (VluauL_error) (struct lua_State *L, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    g_Luau.luaL_errorL(L, fmt, argp);
    va_end(argp);

    return 0;
}

#if defined(LUA_USE_APICHECK)
int VluauX_is_apicheck_enabled()
{
#ifndef NDEBUG
    return 1;
#else
    return 0;
#endif
}
#endif

Vlua_StateFns VLuau_Functions = {};

#ifdef VLUA_WIN
static HMODULE VLuau_hmodule = nullptr;
#else
static void* VLuau_hmodule = nullptr;
#endif

static_assert(sizeof(void*) == sizeof(void(*)()), "Invalid pointer size");
#ifdef VLUA_WIN
static_assert(sizeof(void*) == sizeof(HMODULE), "Invalid handle size");
#endif

bool VLuau_init()
{
    assert(VLuau_hmodule == nullptr);

    const static char* Vluau_libname = VLUA_PLATFORM VLUA_LIB_PREFIX "luau" VLUA_LIB_SUFFIX VLUA_LIB_EXTENSION;

#ifdef VLUA_WIN
    VLuau_hmodule = LoadLibrary(Vluau_libname);
#else
    VLuau_hmodule = dlopen(Vluau_libname, RTLD_LAZY);
#endif

    if (VLuau_hmodule == nullptr)
    {
        return false;
    }

    const static size_t ptrBlockSize = sizeof(void*) * 2;
    const size_t fnsNum = sizeof(g_Luau) / ptrBlockSize;

    for (size_t i = 0; i < fnsNum; ++i)
    {
        const char* fnName = reinterpret_cast<const char**>(&g_Luau)[i*2 + 0];
#ifdef VLUA_WIN
        FARPROC& pFn = reinterpret_cast<FARPROC*>(&g_Luau)[i*2 + 1];
        pFn = GetProcAddress(VLuau_hmodule, fnName);
#else
        void*& pFn = reinterpret_cast<void**>(&g_Luau)[i*2 + 1];
        pFn = dlsym(VLuau_hmodule, fnName);
#endif
        assert(pFn != nullptr);  
    }

    // Remap Luau functions to the MTA-compatible interface
    // Some of the functions are wrapped to sustain the default behaviour
    VLuau_Functions = {
        g_Luau.lua_close,

        // MTA hooks
        &Vluau_registerPreCallHook,
        &Vluau_registerPostCallHook,
        &Vluau_registerUndumpHook,

        g_Luau.lua_isnumber,
        &Vluau_type, // Wrapper: different lua types
        g_Luau.lua_toboolean,
        g_Luau.lua_tolstring,
        &Vluau_objlen, // Wrapper: return type differs
        g_Luau.lua_touserdata,
        g_Luau.lua_topointer,
        &Vluau_tonumber, // Wrapper: Luau macros
        g_Luau.lua_typename,
        g_Luau.lua_setfield,
        g_Luau.lua_settable,
        g_Luau.lua_rawseti,
        g_Luau.lua_rawset,
        g_Luau.lua_setmetatable,
        &Vluau_gettable, // Wrapper: return type differs
        &Vluau_newuserdata, // Wrapper: Luau macros
        &Vluau_rawget, // Wrapper: return type differs
        g_Luau.lua_createtable,
        &Vluau_getfield, // Wrapper: return type differs
        &Vluau_rawgeti, // Wrapper: return type differs
        g_Luau.lua_pushnil,
        g_Luau.lua_pushnumber,
        g_Luau.lua_pushinteger,
        g_Luau.lua_pushboolean,
        g_Luau.lua_pushlstring,
        g_Luau.lua_pushstring,
        g_Luau.lua_pushlightuserdata,
        &Vluau_pushcclosure, // Wrapper: 4 args in Luau
        g_Luau.lua_insert,
        &Vluau_getmainstate, // Wrapper: Custom MTA function
        &Vluau_getmtasaowner, // Wrapper: Custom MTA function
        g_Luau.lua_replace,
        g_Luau.lua_remove,
        g_Luau.lua_pushvalue,
        g_Luau.lua_gettop,
        g_Luau.lua_settop,
        g_Luau.lua_checkstack,
        g_Luau.lua_getstackgap, // Wrapper: Custom MTA function
        g_Luau.lua_next,
        g_Luau.lua_call,
        g_Luau.lua_pcall,
        g_Luau.lua_nresults, // Wrapper: Custom MTA function
        &Vluau_error, // Wrapper: return type differs
        &Vluau_gc, // Wrapper: different lua types
        &Vluau_getstack, // Wrapper: different behaviour
        &Vluau_getinfo, // Wrapper: different behaviour
        &Vluau_sethook, // Wrapper: not supported bu Luau
        g_Luau.lua_addtotalbytes, // Wrapper: Custom MTA function

        // Libraries
        &Vluauopen_base, // Wrapper: In Luau coroutines are part of separate lib
        g_Luau.luaopen_math,
        g_Luau.luaopen_string,
        g_Luau.luaopen_table,
        g_Luau.luaopen_debug,
        g_Luau.luaopen_utf8,
        g_Luau.luaopen_os,

        // Aux libraries
        g_Luau.luaL_register,
        &VluauL_newstate, // Wrapper: Custom MTA argument
        &VluauL_ref, // Wrapper: Different semantics
        &VluauL_unref, // Wrapper: Different semantics

        &VluauL_loadbuffer, // Wrapper: different behaviour
        &VluauL_error, // Wrapper: Luau macros
        g_Luau.luaL_callmeta,

        g_Luau.luaL_where,
        g_Luau.lua_concat,

    #if defined(LUA_USE_APICHECK)
        &VluauX_is_apicheck_enabled // Wrapper: Custom function
    #endif
    };

    return true;
}

void VLuau_close()
{
    assert(VLuau_hmodule != nullptr);
#ifdef VLUA_WIN  
    FreeLibrary(VLuau_hmodule);
#else
    dlclose(VLuau_hmodule);
#endif
}