/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionRef.h
*  PURPOSE:     Lua function reference
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAFUNCTIONREF_H
#define __CLUAFUNCTIONREF_H

#define LUA_REFNIL      (-1)

class CLuaFunctionRef
{
public:
    CLuaFunctionRef  ( void );
    CLuaFunctionRef  ( lua_State *luaVM, int iFunction, const void* pFuncPtr );
    CLuaFunctionRef  ( const CLuaFunctionRef& other );
    ~CLuaFunctionRef ( void );
    CLuaFunctionRef& operator=( const CLuaFunctionRef& other );
    operator int() const;
    bool operator==( const CLuaFunctionRef& other ) const;
    bool operator!=( const CLuaFunctionRef& other ) const;

    lua_State *     m_luaVM;
    int             m_iFunction;
    const void*     m_pFuncPtr;
    CIntrusiveListNode < CLuaFunctionRef >        m_FromRootNode;     // Our node entry in the 'GlobalLuaFunctionRefList'
};


#endif