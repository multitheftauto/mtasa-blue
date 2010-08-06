/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionRef.cpp
*  PURPOSE:     Lua function reference
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// Custom Lua stack argument->reference function
CLuaFunctionRef luaM_toref ( lua_State *luaVM, int iArgument )
{
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    assert ( pLuaMain );

    const void* pFuncPtr = lua_topointer ( luaVM, iArgument );

    if ( CRefInfo* pInfo = MapFind ( pLuaMain->m_CallbackTable, pFuncPtr ) )
    {
        // Re-use the lua ref we already have to this function
        pInfo->ulUseCount++;
        return CLuaFunctionRef ( luaVM, pInfo->iFunction, pFuncPtr );
    }
    else
    {
        // Get a lua ref to this function
        lua_settop ( luaVM, iArgument );
        int ref = lua_ref ( luaVM, 1 );

        // Save ref info
        CRefInfo info;
        info.ulUseCount = 1;
        info.iFunction = ref;
        MapSet ( pLuaMain->m_CallbackTable, pFuncPtr, info );

        return CLuaFunctionRef ( luaVM, ref, pFuncPtr );
    }
}


// Increment use count for an existing lua ref
void luaM_inc_use ( lua_State *luaVM, int iFunction, const void* pFuncPtr )
{
    if ( !luaVM )
        return;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( !pLuaMain )
        return;

    CRefInfo* pInfo = MapFind ( pLuaMain->m_CallbackTable, pFuncPtr );
    assert ( pInfo );
    assert ( pInfo->iFunction == iFunction );
    pInfo->ulUseCount++;
}


// Decrement use count for an existing lua ref
void luaM_dec_use ( lua_State *luaVM, int iFunction, const void* pFuncPtr )
{
    if ( !luaVM )
        return;
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( !pLuaMain )
        return;

    CRefInfo* pInfo = MapFind ( pLuaMain->m_CallbackTable, pFuncPtr );
    assert ( pInfo );
    assert ( pInfo->iFunction == iFunction );

    if ( --pInfo->ulUseCount == 0 )
    {
        // Remove on last unuse
        lua_unref ( luaVM, iFunction );
        MapRemove ( pLuaMain->m_CallbackTable, pFuncPtr );
    }
}


//
// CLuaFunctionRef implementation
//

CLuaFunctionRef::CLuaFunctionRef ( void )
{
    m_luaVM = NULL;
    m_iFunction = LUA_REFNIL;
    m_pFuncPtr = NULL;
}

CLuaFunctionRef::CLuaFunctionRef ( lua_State *luaVM, int iFunction, const void* pFuncPtr )
{
    m_luaVM = luaVM;
    m_iFunction = iFunction;
    m_pFuncPtr = pFuncPtr;
}

CLuaFunctionRef::CLuaFunctionRef ( const CLuaFunctionRef& other )
{
    m_luaVM = other.m_luaVM;
    m_iFunction = other.m_iFunction;
    m_pFuncPtr = other.m_pFuncPtr;
    luaM_inc_use ( m_luaVM, m_iFunction, m_pFuncPtr );
}

CLuaFunctionRef::~CLuaFunctionRef ( void )
{
    luaM_dec_use ( m_luaVM, m_iFunction, m_pFuncPtr );
}

CLuaFunctionRef& CLuaFunctionRef::operator=( const CLuaFunctionRef& other )
{
    luaM_dec_use ( m_luaVM, m_iFunction, m_pFuncPtr );

    m_luaVM = other.m_luaVM;
    m_iFunction = other.m_iFunction;
    m_pFuncPtr = other.m_pFuncPtr;
    luaM_inc_use ( m_luaVM, m_iFunction, m_pFuncPtr );
    return *this;
}

CLuaFunctionRef::operator int() const
{
    return m_iFunction;
}

bool CLuaFunctionRef::operator==( const CLuaFunctionRef& other ) const
{
    return m_luaVM == other.m_luaVM
        && m_iFunction == other.m_iFunction;
}

bool CLuaFunctionRef::operator!=( const CLuaFunctionRef& other ) const
{
    return !operator==( other );
}
