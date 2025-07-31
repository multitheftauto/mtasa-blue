/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaFunctionRef.h
 *  PURPOSE:     Lua function reference
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define LUA_REFNIL      (-1)
#define VERIFY_FUNCTION(func) ( (func).ToInt () != LUA_REFNIL )
#define IS_REFNIL(func) ( (func).ToInt () == LUA_REFNIL )

struct lua_State;

class CLuaFunctionRef
{
public:
    CLuaFunctionRef();
    CLuaFunctionRef(lua_State* luaVM, int iFunction, const void* pFuncPtr);
    CLuaFunctionRef(const CLuaFunctionRef& other);
    ~CLuaFunctionRef();
    CLuaFunctionRef& operator=(const CLuaFunctionRef& other);
    int              ToInt() const;
    lua_State*       GetLuaVM() const;
    bool             operator==(const CLuaFunctionRef& other) const;
    bool             operator!=(const CLuaFunctionRef& other) const;
    static void      RemoveLuaFunctionRefsForVM(lua_State* luaVM);

protected:
    lua_State*                          m_luaVM;
    int                                 m_iFunction;
    const void*                         m_pFuncPtr;
    CIntrusiveListNode<CLuaFunctionRef> m_ListNode;

    // Global list to track function refs and make sure they can't be used after a VM has closed
    static CIntrusiveList<CLuaFunctionRef> ms_AllRefList;
};
