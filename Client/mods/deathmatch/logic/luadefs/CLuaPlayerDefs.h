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
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Player get
    LUA_DECLARE(GetLocalPlayer);
    LUA_DECLARE(GetPlayerName);
    LUA_DECLARE(GetPlayerFromName);
    LUA_DECLARE(GetPlayerNametagText);
    LUA_DECLARE(GetPlayerNametagColor);
    LUA_DECLARE(IsPlayerNametagShowing);
    LUA_DECLARE(GetPlayerPing);
    LUA_DECLARE(GetPlayerTeam);
    LUA_DECLARE(GetPlayerMoney);
    LUA_DECLARE(GetPlayerWantedLevel);
    static std::uint8_t GetPlayerScriptDebugLevel() noexcept;
    static bool         IsPlayerCrosshairVisible();
    static std::variant<float, bool, std::string, CLuaMultiReturn<float, float>, CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>> GetPlayerHudComponentProperty(eHudComponent component, eHudComponentProperty property);

    // Player set
    LUA_DECLARE(ShowPlayerHudComponent);
    LUA_DECLARE(IsPlayerHudComponentVisible);
    LUA_DECLARE(SetPlayerMoney);
    LUA_DECLARE(GivePlayerMoney);
    LUA_DECLARE(TakePlayerMoney);
    LUA_DECLARE(SetPlayerNametagText);
    LUA_DECLARE(SetPlayerNametagColor);
    LUA_DECLARE(SetPlayerNametagShowing);
    static bool SetPlayerHudComponentProperty(eHudComponent component, eHudComponentProperty property, std::variant<CVector2D, float, bool, std::string> value);
    static bool ResetPlayerHudComponentProperty(eHudComponent component, eHudComponentProperty property) noexcept;

    // Community funcs
    LUA_DECLARE(GetPlayerUserName);
    LUA_DECLARE(GetPlayerSerial);

    // Map funcs
    LUA_DECLARE(ForcePlayerMap);
    LUA_DECLARE(IsPlayerMapForced);
    LUA_DECLARE(IsPlayerMapVisible);
    LUA_DECLARE(GetPlayerMapBoundingBox);
    static unsigned char GetPlayerMapOpacity();
};
