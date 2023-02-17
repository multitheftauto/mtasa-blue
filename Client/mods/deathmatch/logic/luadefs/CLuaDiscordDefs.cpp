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

void CLuaDiscordDefs::LoadFunctions()
{
    // Backwards compatibility functions
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"discordSetDetails", ArgumentParser<SetDetails>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaDiscordDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "setDetails", "discordSetDetails");

    // lua_classfunction(luaVM, "getDocumentXML", "svgGetDocumentXML");
    // lua_classfunction(luaVM, "setDocumentXML", "svgSetDocumentXML");
    // lua_classfunction(luaVM, "getSize", "svgGetSize");
    // lua_classfunction(luaVM, "setSize", "svgSetSize");

    lua_registerclass(luaVM, "DiscordRPC");
}

bool CLuaDiscordDefs::SetDetails(std::string strDetails)
{
    int detailsLength = strDetails.length();

    if (detailsLength < 1 || detailsLength > 64)
        throw std::invalid_argument("Details length must be greater than 0, or less than/equal to 64");

    auto discord = g_pCore->GetDiscord();

    if (!discord || !discord->SetPresenceDetails(strDetails.c_str()))
        return false;

    discord->UpdatePresence();
    return true;
}
