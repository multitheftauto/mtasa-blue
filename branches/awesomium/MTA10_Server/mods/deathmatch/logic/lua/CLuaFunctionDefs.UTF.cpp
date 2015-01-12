/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.UTF.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::UtfLen ( lua_State* luaVM )
{
    SString strInput;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strInput );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, MbUTF8ToUTF16 ( strInput ).size () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::UtfSeek ( lua_State* luaVM )
{
    SString strInput;
    int iPos;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strInput );
    argStream.ReadNumber ( iPos );

    if ( !argStream.HasErrors () )
    {
        std::wstring strUTF = MbUTF8ToUTF16 ( strInput );
        if ( iPos <= static_cast <int>( strUTF.size () ) && iPos >= 0 )
        {
            strUTF = strUTF.substr ( 0, iPos );
            lua_pushnumber ( luaVM, UTF16ToMbUTF8 ( strUTF ).size () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::UtfSub ( lua_State* luaVM )
{
    SString strInput;
    ptrdiff_t iStart;
    ptrdiff_t iEnd;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strInput );
    argStream.ReadNumber ( iStart );
    argStream.ReadNumber ( iEnd, -1 );

    if ( !argStream.HasErrors () )
    {
        std::wstring strUTF = MbUTF8ToUTF16 ( strInput );
        size_t l = static_cast <int> ( strUTF.size () );

        //posrelat them both
        if ( iStart < 0 ) iStart += (ptrdiff_t) l + 1;
        iStart = ( iStart >= 0 ) ? iStart : 0;

        if ( iEnd < 0 ) iEnd += (ptrdiff_t) l + 1;
        iEnd = ( iEnd >= 0 ) ? iEnd : 0;

        if ( iStart < 1 ) iStart = 1;
        if ( iEnd > ( ptrdiff_t )l ) iEnd = (ptrdiff_t) l;
        if ( iStart <= iEnd )
        {
            strUTF = strUTF.substr ( iStart - 1, iEnd - iStart + 1 );
            lua_pushstring ( luaVM, UTF16ToMbUTF8 ( strUTF ).c_str () );
        }
        else
            lua_pushliteral ( luaVM, "" );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::UtfChar ( lua_State* luaVM )
{
    int iCode;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iCode );

    if ( !argStream.HasErrors () )
    {
        if ( iCode > 65534 || iCode < 32 )
        {
            m_pScriptDebugging->LogBadType ( luaVM );
            lua_pushnil ( luaVM );
            return 1;
        }

        // Generate a null-terminating string for our character
        wchar_t wUNICODE[2] = { iCode, '\0' };

        // Convert our UTF character into an ANSI string
        SString strANSI = UTF16ToMbUTF8 ( wUNICODE );

        lua_pushstring ( luaVM, strANSI.c_str () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::UtfCode ( lua_State* luaVM )
{
    SString strInput;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strInput );

    if ( !argStream.HasErrors () )
    {
        std::string strInput = lua_tostring ( luaVM, 1 );
        std::wstring strUTF = MbUTF8ToUTF16 ( strInput );
        unsigned long ulCode = strUTF.c_str ()[0];

        lua_pushnumber ( luaVM, ulCode );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


