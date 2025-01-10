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
    static std::variant<CClientTeam*, bool> GetTeamFromName(const std::string name) noexcept;
    static std::string GetTeamName(CClientTeam* team);
    static CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t> GetTeamColor(CClientTeam* team) noexcept;
    static bool GetTeamFriendlyFire(CClientTeam* team) noexcept;
    static std::vector<CClientPlayer*> GetPlayersInTeam(CClientTeam* team);
    static std::uint32_t CountPlayersInTeam(CClientTeam* team) noexcept;
};
