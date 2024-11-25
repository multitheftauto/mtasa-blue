/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaModelDefs.cpp
 *  PURPOSE:     Lua model definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>
#include "CLodModels.h"
#include "CClientObjectManager.h"
#include "CClientBuildingManager.h"

void CLuaModelDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Util func
        {"IsValidModel", ArgumentParser<IsValidModel>},

        // LOD funcs
        {"getLowLODModel", ArgumentParser<GetLowLODModel>},
        {"getHighLODModel", ArgumentParser<GetHighLODModel>},
        {"setLowLODModel", ArgumentParser<SetLowLODModel>},
        {"resetLowLODModel", ArgumentParser<ResetLowLODModel>},
        {"resetLowLODModels", ArgumentParser<ResetLowLODModels>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}


bool CLuaModelDefs::IsValidModel(std::string modelPurpose, std::uint32_t id)
{
    if (modelPurpose == "object")
        return CClientObjectManager::IsValidModel(id);
    else if (modelPurpose == "building")
        return CClientBuildingManager::IsValidModel(id);

    throw std::invalid_argument("Invalid model purpose passed, expected 'object' or 'building'");
}

std::variant<bool, std::uint32_t> CLuaModelDefs::GetLowLODModel(std::uint32_t hLODModel) noexcept
{
    return CLodModels::GetLowLODModel(hLODModel);
}

std::variant<bool, std::uint32_t> CLuaModelDefs::GetHighLODModel(std::uint32_t lLODModel) noexcept
{
    return CLodModels::GetHighLODModel(lLODModel);
}

bool CLuaModelDefs::SetLowLODModel(std::string modelPurpose, std::uint32_t hLODModel, std::uint32_t lLODModel)
{
    if (!IsValidModel(modelPurpose, hLODModel))
        throw std::invalid_argument("Invalid model ID passed for High-LOD argument");

    if (!IsValidModel(modelPurpose, lLODModel))
        throw std::invalid_argument("Invalid model ID passed for Low-LOD argument");

    return CLodModels::SetLowLODModel(hLODModel, lLODModel);
}

bool CLuaModelDefs::ResetLowLODModel(std::uint32_t hLODModel) noexcept
{
    return CLodModels::ResetLowLODModel(hLODModel);
}

void CLuaModelDefs::ResetLowLODModels() noexcept
{
    CLodModels::ResetLowLODModels();
}
