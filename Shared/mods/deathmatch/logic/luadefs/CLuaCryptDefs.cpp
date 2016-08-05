/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/mods/logic/luadefs/CLuaCryptDefs.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaCryptDefs::LoadFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "md5", Md5 );
    CLuaCFunctions::AddFunction ( "sha256", Sha256 );
    CLuaCFunctions::AddFunction ( "hash", Hash );
    CLuaCFunctions::AddFunction ( "teaEncode", TeaEncode );
    CLuaCFunctions::AddFunction ( "teaDecode", TeaDecode );
    CLuaCFunctions::AddFunction ( "base64Encode", Base64encode );
    CLuaCFunctions::AddFunction ( "base64Decode", Base64decode );
}

int CLuaCryptDefs::Md5 ( lua_State* luaVM )
{
    SString strMd5 = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strMd5 );

    if ( !argStream.HasErrors () )
    {
        MD5 md5bytes;
        char szResult[33];
        CMD5Hasher hasher;
        hasher.Calculate ( strMd5, strMd5.length (), md5bytes );
        hasher.ConvertToHex ( md5bytes, szResult );
        lua_pushstring ( luaVM, szResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaCryptDefs::Sha256 ( lua_State* luaVM )
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

int CLuaCryptDefs::Hash ( lua_State* luaVM )
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

int CLuaCryptDefs::TeaEncode ( lua_State* luaVM )
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

int CLuaCryptDefs::TeaDecode ( lua_State* luaVM )
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

int CLuaCryptDefs::Base64encode ( lua_State* luaVM )
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

int CLuaCryptDefs::Base64decode ( lua_State* luaVM )
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
