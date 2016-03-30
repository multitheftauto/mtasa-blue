/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaPlayerDefs.h
*  PURPOSE:     Lua player definitions class header
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaPlayerDefs : public CLuaDefs
{
public:
    static void LoadFunctions ( void );
    static void AddClass ( lua_State* luaVM );

    // Player get
    LUA_DECLARE ( GetLocalPlayer );
    LUA_DECLARE ( GetPlayerName );
    LUA_DECLARE ( GetPlayerFromName );
    LUA_DECLARE ( GetPlayerNametagText );
    LUA_DECLARE ( GetPlayerNametagColor );
    LUA_DECLARE ( IsPlayerNametagShowing );
    LUA_DECLARE ( GetPlayerPing );
    LUA_DECLARE ( GetPlayerTeam );
    LUA_DECLARE ( GetPlayerMoney );
    LUA_DECLARE ( GetPlayerWantedLevel );

    // Player set
    LUA_DECLARE ( ShowPlayerHudComponent );
    LUA_DECLARE ( IsPlayerHudComponentVisible );
    LUA_DECLARE ( SetPlayerMoney );
    LUA_DECLARE ( GivePlayerMoney );
    LUA_DECLARE ( TakePlayerMoney );
    LUA_DECLARE ( SetPlayerNametagText );
    LUA_DECLARE ( SetPlayerNametagColor );
    LUA_DECLARE ( SetPlayerNametagShowing );

    // Community funcs
    LUA_DECLARE ( GetPlayerUserName );
    LUA_DECLARE ( GetPlayerSerial );

    // Map funcs
    LUA_DECLARE ( ForcePlayerMap );
    LUA_DECLARE ( IsPlayerMapForced );
    LUA_DECLARE ( IsPlayerMapVisible );
    LUA_DECLARE ( GetPlayerMapBoundingBox );
};