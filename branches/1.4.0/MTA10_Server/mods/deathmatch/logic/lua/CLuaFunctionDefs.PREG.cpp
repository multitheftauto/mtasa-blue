/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.PREG.cpp
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


int CLuaFunctionDefs::PregFind ( lua_State* luaVM )
{
    //  bool pregFind ( string base, string pattern, uint flags/string = 0 )
    SString strBase, strPattern;
    pcrecpp::RE_Options pOptions;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strBase );
    argStream.ReadString ( strPattern );
    ReadPregFlags ( argStream, pOptions );

    if ( !argStream.HasErrors () )
    {
        pcrecpp::RE pPattern ( strPattern, pOptions );

        if ( pPattern.PartialMatch ( strBase ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::PregReplace ( lua_State* luaVM )
{
    //  string pregReplace ( string base, string pattern, string replace, uint flags/string = 0 )
    SString strBase, strPattern, strReplace;
    pcrecpp::RE_Options pOptions;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strBase );
    argStream.ReadString ( strPattern );
    argStream.ReadString ( strReplace );
    ReadPregFlags ( argStream, pOptions );

    if ( !argStream.HasErrors () )
    {
        pcrecpp::RE pPattern ( strPattern, pOptions );

        string strNew = strBase;
        if ( pPattern.GlobalReplace ( strReplace, &strNew ) )
        {
            lua_pushstring ( luaVM, strNew.c_str () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::PregMatch ( lua_State* luaVM )
{
    //  table pregMatch ( string base, string pattern, uint flags/string = 0, int maxResults = 100000 )
    SString strBase, strPattern;
    pcrecpp::RE_Options pOptions;
    int iMaxResults;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strBase );
    argStream.ReadString ( strPattern );
    ReadPregFlags ( argStream, pOptions );
    argStream.ReadNumber ( iMaxResults, 100000 );

    if ( !argStream.HasErrors () )
    {
        lua_newtable ( luaVM );

        pcrecpp::RE pPattern ( strPattern, pOptions );

        pcrecpp::StringPiece strInput ( strBase );

        string strGet; int i = 1;
        while ( pPattern.FindAndConsume ( &strInput, &strGet ) && i <= iMaxResults )
        {
            lua_pushnumber ( luaVM, i );
            lua_pushstring ( luaVM, strGet.c_str () );
            lua_settable ( luaVM, -3 );
            i++;
        }

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}