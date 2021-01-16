/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaGenericDefs.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaGenericDefs.h"

void CLuaGenericDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"setTransferBoxVisible", ArgumentParser<CStaticFunctionDefinitions::SetClientTransferBoxVisible>},
        {"isTransferBoxVisible", ArgumentParser<IsTransferBoxVisible>},
    };

    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

bool CLuaGenericDefs::IsTransferBoxVisible()
{
    return g_pGame->IsClientTransferBoxVisible();
}
