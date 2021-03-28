/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaClientDefs.cpp
 *  PURPOSE:     Lua client definitions class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaClientDefs.h"
#include "lua/CLuaFunctionParser.h"

void CLuaClientDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"setTransferBoxVisible", ArgumentParser<SetTransferBoxVisible>},
        {"isTransferBoxVisible", ArgumentParser<IsTransferBoxVisible>},
        {"isTransferBoxAlwaysVisible", ArgumentParser<IsTransferBoxAlwaysVisible>},

        {"clearDebug", ArgumentParser<ClearDebug>},
        {"clearConsoleHistory", ArgumentParser<ClearConsoleHistory>}
    };

    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

bool CLuaClientDefs::SetTransferBoxVisible(bool visible)
{
    return g_pClientGame->GetTransferBox()->SetClientVisibility(visible);
}

bool CLuaClientDefs::IsTransferBoxVisible()
{
    return g_pClientGame->GetTransferBox()->IsVisible();
}

bool CLuaClientDefs::IsTransferBoxAlwaysVisible()
{
    return g_pClientGame->GetTransferBox()->IsAlwaysVisible();
}

bool CLuaClientDefs::ClearDebug()
{
    g_pCore->DebugClear();
    return true;
}

bool CLuaClientDefs::ClearConsoleHistory()
{
    g_pCore->GetConsole()->Clear();
    return true;
}
