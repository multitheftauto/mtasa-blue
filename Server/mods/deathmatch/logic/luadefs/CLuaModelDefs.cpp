/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaModelDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaModelDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"allocateModel", ArgumentParserWarn<false, AllocateModel>},
        {"getAllocatedModels", ArgumentParserWarn<false, GetAllocatedModels>},
        {"allocateModelFromParent", ArgumentParserWarn<false, AllocateModelFromParent>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

uint CLuaModelDefs::AllocateModel(eModelInfoType eModel, std::optional<uint> uiParentModel, std::optional<uint> uiModelID)
{
    return -1;
}

boolean CLuaModelDefs::AllocateModelFromParent(uint32_t uiModelID, uint32_t uiParentModelID)
{
    g_pGame->GetModelManager()->AllocateModelFromParent(uiModelID, uiParentModelID);
    return true;
}

std::vector<uint> CLuaModelDefs::GetAllocatedModels(std::optional<eModelInfoType> eType)
{
    std::vector<uint> o;
    return o;
}
