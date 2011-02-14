/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionRef.cpp
*  PURPOSE:     Lua function reference
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#pragma warning( disable : 4355 )   // warning C4355: 'this' : used in base member initializer list

// Custom Lua stack argument->reference function
CLuaFunctionRef luaM_toref ( lua_State *luaVM, int iArgument )
{
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
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
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
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
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
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
        MapRemove ( pLuaMain->m_FunctionTagMap, iFunction );
    }
}


#if 0
typedef CIntrusiveListExt < CLuaFunctionRef, &CLuaFunctionRef::m_FromRootNode > CFromRootListType2;
static CFromRootListType2 g_LuaFunctionRefList;
/*
typedef google::dense_hash_map < unsigned int, CFromRootListType > t_mapEntitiesFromRoot;
static t_mapEntitiesFromRoot    ms_mapEntitiesFromRoot;
static bool                     ms_bEntitiesFromRootInitialized = false;

void StartupEntitiesFromRoot ()
{
    if ( !ms_bEntitiesFromRootInitialized )
    {
        ms_mapEntitiesFromRoot.set_deleted_key ( (unsigned int)0x00000000 );
        ms_mapEntitiesFromRoot.set_empty_key ( (unsigned int)0xFFFFFFFF );
        ms_bEntitiesFromRootInitialized = true;
    }
}
*/

void CLuaFunctionRef::NotifyDestroyedLuaVM ( lua_State *luaVM )
{
    for ( CFromRootListType2::iterator iter = g_LuaFunctionRefList.begin (); iter != g_LuaFunctionRefList.end (); ++iter )
    {
        CLuaFunctionRef* pLuaFunctionRef = *iter;
        if ( pLuaFunctionRef->m_luaVM == luaVM )
            //pLuaFunctionRef->m_luaVM = NULL;
            pLuaFunctionRef->m_luaVM = pLuaFunctionRef->m_luaVM;
    }
}
#endif


//
// CLuaFunctionRef implementation
//

CLuaFunctionRef::CLuaFunctionRef ( void )
    : m_FromRootNode ( this )
{
//    g_LuaFunctionRefList.push_front ( this );
    m_luaVM = NULL;
    m_iFunction = LUA_REFNIL;
    m_pFuncPtr = NULL;
}

CLuaFunctionRef::CLuaFunctionRef ( lua_State *luaVM, int iFunction, const void* pFuncPtr )
    : m_FromRootNode ( this )
{
//    g_LuaFunctionRefList.push_front ( this );
    m_luaVM = luaVM;
    m_iFunction = iFunction;
    m_pFuncPtr = pFuncPtr;
}

CLuaFunctionRef::CLuaFunctionRef ( const CLuaFunctionRef& other )
    : m_FromRootNode ( this )
{
//    g_LuaFunctionRefList.push_front ( this );
    m_luaVM = other.m_luaVM;
    m_iFunction = other.m_iFunction;
    m_pFuncPtr = other.m_pFuncPtr;
    luaM_inc_use ( m_luaVM, m_iFunction, m_pFuncPtr );
}

CLuaFunctionRef::~CLuaFunctionRef ( void )
{
    if ( !m_FromRootNode.m_pPrev )
        m_FromRootNode.m_pPrev = m_FromRootNode.m_pPrev;
    luaM_dec_use ( m_luaVM, m_iFunction, m_pFuncPtr );
//    g_LuaFunctionRefList.remove ( this );
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
