/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaTeamDefs.h"
#include "CLuaGenericDefs.h"
#include "CStaticFunctionDefinitions.h"
#include <lua/CLuaFunctionParser.h>

void CLuaTeamDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Team create/destroy functions
        {"createTeam", ArgumentParserWarn<false, CreateTeam>},

        // Team get funcs
        {"getTeamName", ArgumentParserWarn<false, GetTeamName>},
        {"getTeamFromName", ArgumentParserWarn<false, GetTeamFromName>},
        {"getTeamColor", ArgumentParserWarn<false, GetTeamColor>},
        {"getTeamFriendlyFire", ArgumentParserWarn<false, GetTeamFriendlyFire>},
        {"getPlayersInTeam", ArgumentParserWarn<false, GetPlayersInTeam>},
        {"countPlayersInTeam", ArgumentParserWarn<false, CountPlayersInTeam>},

        // Team set funcs
        {"setPlayerTeam", ArgumentParserWarn<false, SetPlayerTeam>},
        {"setTeamName", ArgumentParserWarn<false, SetTeamName>},
        {"setTeamColor", ArgumentParserWarn<false, SetTeamColor>},
        {"setTeamFriendlyFire", ArgumentParserWarn<false, SetTeamFriendlyFire>}
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaTeamDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createTeam");
    lua_classfunction(luaVM, "getFromName", "getTeamFromName");
    lua_classfunction(luaVM, "countPlayers", "countPlayersInTeam");
    lua_classfunction(luaVM, "getPlayers", "getPlayersInTeam");
    lua_classfunction(luaVM, "outputChat", "outputChatBox", ArgumentParserWarn<false, CLuaGenericDefs::OOP_OutputChatBox>);
     
    lua_classfunction(luaVM, "getFriendlyFire", "getTeamFriendlyFire");
    lua_classfunction(luaVM, "getName", "getTeamName");
    lua_classfunction(luaVM, "getColor", "getTeamColor");

    lua_classfunction(luaVM, "setName", "setTeamName");
    lua_classfunction(luaVM, "setColor", "setTeamColor");
    lua_classfunction(luaVM, "setFriendlyFire", "setTeamFriendlyFire");

    lua_classvariable(luaVM, "playerCount", NULL, "countPlayersInTeam");
    lua_classvariable(luaVM, "friendlyFire", "setTeamFriendlyFire", "getTeamFriendlyFire");
    lua_classvariable(luaVM, "players", NULL, "getPlayersInTeam");
    lua_classvariable(luaVM, "name", "setTeamName", "getTeamName");

    lua_registerclass(luaVM, "Team", "Element");
}

std::variant<CTeam*, bool> CLuaTeamDefs::CreateTeam(lua_State* lua, const std::string name, const std::optional<std::uint8_t> red, const std::optional<std::uint8_t> green, const std::optional<std::uint8_t> blue)
{
    CLuaMain& vm = lua_getownercluamain(lua);
    CResource* resource = vm.GetResource();

    if (!resource)
        return false;

    CTeam* team = CStaticFunctionDefinitions::CreateTeam(resource, name.c_str(), red.value_or(235), green.value_or(221), blue.value_or(178));

    if (!team)
        return false;

    CElementGroup* group = resource->GetElementGroup();

    if (group)
        group->Add(team);

    return team;
}

std::variant<CTeam*, bool> CLuaTeamDefs::GetTeamFromName(const std::string name)
{
    CTeam* team = m_pTeamManager->GetTeam(name.c_str());

    if (!team)
        return false;

    return team;
}

std::string CLuaTeamDefs::GetTeamName(CTeam* team)
{
    return team->GetTeamName();
}

CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t> CLuaTeamDefs::GetTeamColor(CTeam* team) noexcept
{
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;

    team->GetColor(red, green, blue);

    return {red, green, blue};
}

bool CLuaTeamDefs::GetTeamFriendlyFire(CTeam* team) noexcept
{
    return team->GetFriendlyFire();
}

std::vector<CPlayer*> CLuaTeamDefs::GetPlayersInTeam(CTeam* team)
{
    return team->GetPlayers();
}

std::uint32_t CLuaTeamDefs::CountPlayersInTeam(CTeam* team) noexcept
{
    return team->CountPlayers();
}

bool CLuaTeamDefs::SetPlayerTeam(CPlayer* player, std::optional<CTeam*> team) noexcept
{
    return CStaticFunctionDefinitions::SetPlayerTeam(player, team.value_or(nullptr));
}

bool CLuaTeamDefs::SetTeamName(CTeam* team, const std::string name)
{
    return CStaticFunctionDefinitions::SetTeamName(team, name.c_str());
}

bool CLuaTeamDefs::SetTeamColor(CTeam* team, const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue) noexcept
{
    return CStaticFunctionDefinitions::SetTeamColor(team, red, green, blue);
}

bool CLuaTeamDefs::SetTeamFriendlyFire(CTeam* team, const bool state) noexcept
{
    return CStaticFunctionDefinitions::SetTeamFriendlyFire(team, state);
}
