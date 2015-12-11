/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaFileDefs.h
*  PURPOSE:     Lua player definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaPlayerDefs : public CLuaDefs
{
public:
    static void     LoadFunctions ( void );
    static void     AddClass ( lua_State* luaVM );

    // Player get functions
    LUA_DECLARE ( GetPlayerCount );
    LUA_DECLARE ( SetPlayerAmmo );
    LUA_DECLARE ( GetPlayerFromName );
    LUA_DECLARE ( GetPlayerMoney );
    LUA_DECLARE ( GetPlayerPing );
    LUA_DECLARE ( GetRandomPlayer );
    LUA_DECLARE ( IsPlayerMuted );
    LUA_DECLARE ( GetPlayerTeam );
    LUA_DECLARE ( CanPlayerUseFunction );
    LUA_DECLARE ( GetPlayerWantedLevel );
    LUA_DECLARE ( GetAlivePlayers );
    LUA_DECLARE ( GetDeadPlayers );
    LUA_DECLARE ( GetPlayerIdleTime );
    LUA_DECLARE ( IsPlayerScoreboardForced );
    LUA_DECLARE ( IsPlayerMapForced );
    LUA_DECLARE ( GetPlayerNametagText );
    LUA_DECLARE ( GetPlayerNametagColor );
    LUA_DECLARE ( IsPlayerNametagShowing );
    LUA_DECLARE ( GetPlayerSerial );
    LUA_DECLARE ( GetPlayerCommunityID ); // deprecate me
    LUA_DECLARE ( GetPlayerUserName ); // deprecate me
    LUA_DECLARE ( GetPlayerBlurLevel );
    LUA_DECLARE ( GetPlayerName );
    LUA_DECLARE ( GetPlayerIP );
    LUA_DECLARE ( GetPlayerAccount );
    LUA_DECLARE ( GetPlayerVersion );
    LUA_DECLARE ( GetPlayerACInfo );

    // Player set functions
    LUA_DECLARE ( SetPlayerMoney );
    LUA_DECLARE ( GivePlayerMoney );
    LUA_DECLARE ( TakePlayerMoney );
    LUA_DECLARE ( SpawnPlayer );
    LUA_DECLARE ( ShowPlayerHudComponent ); // deprecate me
    LUA_DECLARE ( SetPlayerWantedLevel );
    LUA_DECLARE ( ForcePlayerMap );
    LUA_DECLARE ( SetPlayerNametagText );
    LUA_DECLARE ( SetPlayerNametagColor );
    LUA_DECLARE ( SetPlayerNametagShowing );
    LUA_DECLARE ( SetPlayerMuted );
    LUA_DECLARE ( SetPlayerBlurLevel );
    LUA_DECLARE ( RedirectPlayer );
    LUA_DECLARE ( SetPlayerName );
    LUA_DECLARE ( DetonateSatchels );
    LUA_DECLARE ( TakePlayerScreenShot );

    // All seeing eye
    LUA_DECLARE ( GetPlayerAnnounceValue );
    LUA_DECLARE ( SetPlayerAnnounceValue );
    LUA_DECLARE ( ResendPlayerModInfo );
    LUA_DECLARE ( ResendPlayerACInfo );

    // Key bind funcs
    LUA_DECLARE ( BindKey );
    LUA_DECLARE ( UnbindKey );
    LUA_DECLARE ( IsKeyBound );
    LUA_DECLARE ( GetFunctionsBoundToKey );
    LUA_DECLARE ( GetKeyBoundToFunction );
    LUA_DECLARE ( GetControlState );
    LUA_DECLARE ( IsControlEnabled );

    LUA_DECLARE ( SetControlState );
    LUA_DECLARE ( ToggleControl );
    LUA_DECLARE ( ToggleAllControls );

    // Audio funcs
    LUA_DECLARE ( PlaySoundFrontEnd );
    LUA_DECLARE ( PlayMissionAudio );
    LUA_DECLARE ( PreloadMissionAudio );

    // Cursor funcs
    LUA_DECLARE ( IsCursorShowing );
    LUA_DECLARE ( ShowCursor );

    // Chat funcs
    LUA_DECLARE ( ShowChat );

    // Admin funcs
    LUA_DECLARE ( KickPlayer );
    LUA_DECLARE ( BanPlayer );
};