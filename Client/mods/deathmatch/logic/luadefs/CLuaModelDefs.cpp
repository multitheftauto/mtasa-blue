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
        {"isValidModel", ArgumentParser<IsValidModel>},

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
    else if (modelPurpose == "weapon")
        return CClientPedManager::IsValidWeaponModel(id);
    else if (modelPurpose == "upgrade")
        return CVehicleUpgrades::IsUpgrade(id);
    else if (modelPurpose == "building")
        return CClientBuildingManager::IsValidModel(id);
    else if (modelPurpose == "vehicle")
        return CClientVehicleManager::IsValidModel(id);
    else if (modelPurpose == "ped" || modelPurpose == "player")
        return CClientPlayerManager::IsValidModel(id);

    throw std::invalid_argument("Invalid model purpose passed, expected [object, weapon, upgrade, building, vehicle, ped, player]");
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
    if (!(modelPurpose == "object" || modelPurpose == "building"))
        throw std::invalid_argument("Invalid model purpose passed, the only options for LOD are 'object' or 'building'");

    if (!IsValidModel(modelPurpose, hLODModel))
        throw std::invalid_argument("Invalid High-LOD model ID passed");

    if (!IsValidModel(modelPurpose, lLODModel))
        throw std::invalid_argument("Invalid Low-LOD model ID passed");

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
