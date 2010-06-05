/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Voice.cpp
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

#ifdef MTA_VOICE

int CLuaFunctionDefs::SetVoiceInputEnabled ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TBOOLEAN )
    {
        bool bEnabled = lua_toboolean ( luaVM, 1 ) ? true : false;
        bool bResult = CStaticFunctionDefinitions::SetVoiceInputEnabled(bEnabled); 
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVoiceMuteAllEnabled ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TBOOLEAN )
    {
        bool bEnabled = lua_toboolean ( luaVM, 1 ) ? true : false;
        bool bResult = CStaticFunctionDefinitions::SetVoiceMuteAllEnabled ( bEnabled );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

#endif

