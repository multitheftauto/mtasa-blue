/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.BodyClothes.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::GetBodyPartName ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned char ucID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );

        char szBuffer [256];
        if ( CStaticFunctionDefinitions::GetBodyPartName ( ucID, szBuffer ) )
        {
            lua_pushstring ( luaVM, szBuffer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBodyPartName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetClothesByTypeIndex ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        unsigned char ucIndex = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        char szTexture [ 128 ], szModel [ 128 ];
        if ( CStaticFunctionDefinitions::GetClothesByTypeIndex ( ucType, ucIndex, szTexture, szModel ) )
        {
            lua_pushstring ( luaVM, szTexture );
            lua_pushstring ( luaVM, szModel );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getClothesByTypeIndex" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTypeIndexFromClothes ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szTexture = lua_tostring ( luaVM, 1 );
        const char* szModel = NULL;
        if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
            szModel = lua_tostring ( luaVM, 2 );

        unsigned char ucType, ucIndex;
        if ( CStaticFunctionDefinitions::GetTypeIndexFromClothes ( const_cast < char* > ( szTexture ), const_cast < char* > ( szModel ), ucType, ucIndex ) )
        {
            lua_pushnumber ( luaVM, ucType );
            lua_pushnumber ( luaVM, ucIndex );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getTypeIndexFromClothes" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetClothesTypeName ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );

        char szName [ 40 ];
        if ( CStaticFunctionDefinitions::GetClothesTypeName ( ucType, szName ) )
        {
            lua_pushstring ( luaVM, szName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getClothesTypeName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}



