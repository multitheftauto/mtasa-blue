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
    unsigned char ucID = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ucID );

    if ( !argStream.HasErrors ( ) )
    {
        SString strBuffer;
        if ( CStaticFunctionDefinitions::GetBodyPartName ( ucID, strBuffer ) )
        {
            lua_pushstring ( luaVM, strBuffer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetClothesByTypeIndex ( lua_State* luaVM )
{
    unsigned char ucType = 0;
    unsigned char ucIndex = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ucType );
    argStream.ReadNumber ( ucIndex );

    if ( !argStream.HasErrors ( ) )
    {
        SString strTexture, strModel;
        if ( CStaticFunctionDefinitions::GetClothesByTypeIndex ( ucType, ucIndex, strTexture, strModel ) )
        {
            lua_pushstring ( luaVM, strTexture );
            lua_pushstring ( luaVM, strModel );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTypeIndexFromClothes ( lua_State* luaVM )
{
    SString strTexture = "", strModel = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strTexture );
    argStream.ReadString ( strModel, "" );

    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucType, ucIndex;
        if ( CStaticFunctionDefinitions::GetTypeIndexFromClothes ( strTexture, strModel == "" ? NULL : strModel, ucType, ucIndex ) )
        {
            lua_pushnumber ( luaVM, ucType );
            lua_pushnumber ( luaVM, ucIndex );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetClothesTypeName ( lua_State* luaVM )
{
    unsigned char ucType = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ucType );

    if ( !argStream.HasErrors ( ) )
    {
        SString strName;
        if ( CStaticFunctionDefinitions::GetClothesTypeName ( ucType, strName ) )
        {
            lua_pushstring ( luaVM, strName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



