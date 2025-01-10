/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
    static CTeam* CreateTeam(lua_State* lua, std::string name, std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept;

    // Team get funcs
    static CTeam* GetTeamFromName(const std::string name) noexcept;
    static std::string GetTeamName(CTeam* team) noexcept;
    static CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t> GetTeamColor(CTeam* team) noexcept;
    static bool GetTeamFriendlyFire(CTeam* team) noexcept;
    static std::vector<CPlayer*> GetPlayersInTeam(CTeam* team) noexcept;
    static std::uint32_t CountPlayersInTeam(CTeam* team) noexcept;

    // Team set funcs
    static bool SetPlayerTeam(CPlayer* player, CTeam* team) noexcept;
    static bool SetTeamName(CTeam* team, std::string name) noexcept;
    static bool SetTeamColor(CTeam* team, std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept;
    static bool SetTeamFriendlyFire(CTeam* team, bool state) noexcept;
};
