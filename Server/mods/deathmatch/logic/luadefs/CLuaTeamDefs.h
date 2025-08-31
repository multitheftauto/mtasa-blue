/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <lua/CLuaMultiReturn.h>

class CLuaTeamDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

private:
    // Team create/destroy functions
    static std::variant<CTeam*, bool> CreateTeam(lua_State* lua, const std::string name, const std::optional<std::uint8_t> red, const std::optional<std::uint8_t> green, const std::optional<std::uint8_t> blue);

    // Team get funcs
    static std::variant<CTeam*, bool> GetTeamFromName(const std::string name);
    static std::string GetTeamName(CTeam* team);
    static CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t> GetTeamColor(CTeam* team);
    static bool GetTeamFriendlyFire(CTeam* team);
    static std::vector<CPlayer*> GetPlayersInTeam(CTeam* team);
    static std::uint32_t CountPlayersInTeam(CTeam* team);

    // Team set funcs
    static bool SetPlayerTeam(CPlayer* player, std::optional<CTeam*> team);
    static bool SetTeamName(CTeam* team, const std::string name);
    static bool SetTeamColor(CTeam* team, const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue);
    static bool SetTeamFriendlyFire(CTeam* team, const bool state);
};
