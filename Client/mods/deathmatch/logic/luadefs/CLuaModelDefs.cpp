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
        {"getModelLowLOD", ArgumentParser<GetModelLowLOD>},
        {"getModelHighLOD", ArgumentParser<GetModelHighLOD>},
        {"setModelLOD", ArgumentParser<SetModelLOD>},
        {"resetModelLODByHigh", ArgumentParser<ResetModelLODByHigh>},
        {"resetModelLODByLow", ArgumentParser<ResetModelLODByLow>},
        {"resetAllModelLOD", ArgumentParser<ResetAllModelLOD>},
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

std::variant<bool, std::uint32_t> CLuaModelDefs::GetModelLowLOD(std::uint32_t hLODModel) noexcept
{
    return CLodModels::GetModelLowLOD(hLODModel);
}

std::variant<bool, std::uint32_t> CLuaModelDefs::GetModelHighLOD(std::uint32_t lLODModel) noexcept
{
    return CLodModels::GetModelHighLOD(lLODModel);
}

bool CLuaModelDefs::SetModelLOD(std::string modelPurpose, std::uint32_t hLODModel, std::uint32_t lLODModel)
{
    if (!(modelPurpose == "object" || modelPurpose == "building"))
        throw std::invalid_argument("Invalid model purpose passed, the only options for LOD are 'object' or 'building'");

    if (!IsValidModel(modelPurpose, hLODModel))
        throw std::invalid_argument("Invalid High-LOD model ID passed");

    if (!IsValidModel(modelPurpose, lLODModel))
        throw std::invalid_argument("Invalid Low-LOD model ID passed");

    return CLodModels::SetModelLOD(hLODModel, lLODModel);
}

bool CLuaModelDefs::ResetModelLODByHigh(std::uint32_t hLODModel) noexcept
{
    return CLodModels::ResetModelLODByHigh(hLODModel);
}

bool CLuaModelDefs::ResetModelLODByLow(std::uint32_t hLODModel) noexcept
{
    return CLodModels::ResetModelLODByLow(hLODModel);
}

void CLuaModelDefs::ResetAllModelLOD() noexcept
{
    CLodModels::ResetAllModelLOD();
}
