/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Crypt.cpp
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

int CLuaFunctionDefs::Md5 ( lua_State* luaVM )
{
    SString strSetting;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strSetting );

    if ( !argStream.HasErrors () )
    {
        MD5 md5bytes;
        char szResult[33];
        CMD5Hasher hasher;
        hasher.Calculate ( strSetting, strSetting.length (), md5bytes );
        hasher.ConvertToHex ( md5bytes, szResult );

        lua_pushstring ( luaVM, szResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::Sha256 ( lua_State* luaVM )
{
    //  string sha256 ( string str )
    SString strSourceData;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strSourceData );

    if ( !argStream.HasErrors () )
    {
        SString strResult = GenerateSha256HexString ( strSourceData );
        lua_pushstring ( luaVM, strResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::Hash ( lua_State* luaVM )
{
    //  string hash ( string type, string data )
    EHashFunctionType hashFunction; SString strSourceData;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumString ( hashFunction );
    argStream.ReadString ( strSourceData );

    if ( !argStream.HasErrors () )
    {
        SString strResult = GenerateHashHexString ( hashFunction, strSourceData );
        lua_pushstring ( luaVM, strResult.ToLower () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::TeaEncode ( lua_State* luaVM )
{
    SString str;
    SString key;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( str );
    argStream.ReadString ( key );

    if ( !argStream.HasErrors () )
    {
        SString result;
        SString humanReadableResult;
        SharedUtil::TeaEncode ( str, key, &result );
        humanReadableResult = Base64::encode ( result );
        lua_pushstring ( luaVM, humanReadableResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::TeaDecode ( lua_State* luaVM )
{
    SString str;
    SString key;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( str );
    argStream.ReadString ( key );

    if ( !argStream.HasErrors () )
    {
        SString result;
        Base64::decode ( str, result );
        SharedUtil::TeaDecode ( result, key, &str );
        lua_pushstring ( luaVM, str );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::Base64encode ( lua_State* luaVM )
{
    SString str;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( str );

    if ( !argStream.HasErrors () )
    {
        SString result = Base64::encode ( str );
        lua_pushstring ( luaVM, result );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::Base64decode ( lua_State* luaVM )
{
    SString str;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( str );

    if ( !argStream.HasErrors () )
    {
        SString result;
        Base64::decode ( str, result );
        lua_pushlstring ( luaVM, result, result.length () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
