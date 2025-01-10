/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaTeamDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"getTeamFromName", ArgumentParserWarn<false, GetTeamFromName>},
        {"getTeamName", ArgumentParserWarn<false, GetTeamName>},
        {"getTeamColor", ArgumentParserWarn<false, GetTeamColor>},
        {"getTeamFriendlyFire", ArgumentParserWarn<false, GetTeamFriendlyFire>},
        {"getPlayersInTeam", ArgumentParserWarn<false, GetPlayersInTeam>},
        {"countPlayersInTeam", ArgumentParserWarn<false, CountPlayersInTeam>}
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaTeamDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "getFromName", "getTeamFromName");
    lua_classfunction(luaVM, "countPlayers", "countPlayersInTeam");
    lua_classfunction(luaVM, "getFriendlyFire", "getTeamFriendlyFire");
    lua_classfunction(luaVM, "getName", "getTeamName");
    lua_classfunction(luaVM, "getColor", "getTeamColor");
    lua_classfunction(luaVM, "getPlayers", "getPlayersInTeam");

    lua_classvariable(luaVM, "playerCount", nullptr, "countPlayersInTeam");
    lua_classvariable(luaVM, "friendlyFire", nullptr, "getTeamFriendlyFire");
    lua_classvariable(luaVM, "players", nullptr, "getPlayersInTeam");
    lua_classvariable(luaVM, "name", nullptr, "getTeamName");

    lua_registerclass(luaVM, "Team", "Element");
}

std::variant<CClientTeam*, bool> CLuaTeamDefs::GetTeamFromName(const std::string name) noexcept
{
    CClientTeam* team = m_pTeamManager->GetTeam(name.c_str());

    if (!team)
        return false;

    return team;
}

std::string CLuaTeamDefs::GetTeamName(CClientTeam* team)
{
    return std::string(team->GetTeamName());
}

CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t> CLuaTeamDefs::GetTeamColor(CClientTeam* team) noexcept
{
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;

    team->GetColor(red, green, blue);

    return {red, green, blue};
}

bool CLuaTeamDefs::GetTeamFriendlyFire(CClientTeam* team) noexcept
{
    return team->GetFriendlyFire();
}

std::vector<CClientPlayer*> CLuaTeamDefs::GetPlayersInTeam(CClientTeam* team)
{
    return team->GetPlayers();
}

std::uint32_t CLuaTeamDefs::CountPlayersInTeam(CClientTeam* team) noexcept
{
    return team->CountPlayers();
}
