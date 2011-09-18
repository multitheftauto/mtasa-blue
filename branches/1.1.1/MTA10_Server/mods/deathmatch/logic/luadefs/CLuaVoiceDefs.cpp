/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaVoiceDefs.h
*  PURPOSE:     Lua Voice function definitions class
*  DEVELOPERS:  Talidan <>
*               Philip Farquharson <philip@philipfarquharson.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaVoiceDefs::LoadFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "isVoiceEnabled", CLuaVoiceDefs::IsVoiceEnabled );
    CLuaCFunctions::AddFunction ( "setPlayerVoiceBroadcastTo", CLuaVoiceDefs::SetPlayerVoiceBroadcastTo );
    CLuaCFunctions::AddFunction ( "setPlayerVoiceIgnoreFrom", CLuaVoiceDefs::setPlayerVoiceIgnoreFrom );
}


int CLuaVoiceDefs::IsVoiceEnabled ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pGame->GetConfig()->IsVoiceEnabled() );
    return 1;
}

int CLuaVoiceDefs::SetPlayerVoiceBroadcastTo ( lua_State* luaVM )
{
    // Arg 1, player
    CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
    if ( !pPlayer )
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerVoiceBroadcastTo", "player", 1 );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    // Arg 2, element OR table OR nil
    CElement* pElement = lua_toelement ( luaVM, 2 );
    if ( pElement )
        pPlayer->SetVoiceBroadcastTo ( pElement );
    else if ( lua_type ( luaVM, 2 ) == LUA_TTABLE )
    {
        std::list < CElement* > lstElements;
        for (lua_pushnil(luaVM); lua_next(luaVM, 2); lua_pop(luaVM, 1))
        {
            CElement* pListedElement = lua_toelement ( luaVM, -1 );
            if ( pListedElement )
                lstElements.push_back ( pListedElement );
        }
        pPlayer->SetVoiceBroadcastTo ( lstElements );
    }
    else if ( lua_type ( luaVM, 2 ) == LUA_TNIL )
        pPlayer->SetVoiceBroadcastTo ( NULL ); //Muted
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerVoiceBroadcastTo", "broadcastTo", 2 );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    lua_pushboolean ( luaVM, true );
    return 1;
}


int CLuaVoiceDefs::setPlayerVoiceIgnoreFrom ( lua_State* luaVM )
{
    // Arg 1, player
    CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
    if ( !pPlayer )
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerVoiceIgnoreFrom", "player", 1 );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    // Arg 2, element OR table OR nil
    CElement* pElement = lua_toelement ( luaVM, 2 );
    if ( pElement )
        pPlayer->SetVoiceIgnoredElement ( pElement );
    else if ( lua_type ( luaVM, 2 ) == LUA_TTABLE )
    {
        std::list < CElement* > lstElements;
        for (lua_pushnil(luaVM); lua_next(luaVM, 2); lua_pop(luaVM, 1))
        {
            CElement* pListedElement = lua_toelement ( luaVM, -1 );
            if ( pListedElement )
                lstElements.push_back ( pListedElement );
        }
        pPlayer->SetVoiceIgnoredList ( lstElements );
    }
    else if ( lua_type ( luaVM, 2 ) == LUA_TNIL )
        pPlayer->SetVoiceIgnoredElement ( NULL ); // Remove all ignored
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerVoiceIgnoreFrom", "ignoreFrom", 2 );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    lua_pushboolean ( luaVM, true );
    return 1;
}