/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/luadefs/CLuaEngineDefs.cpp
 *  PURPOSE:     Lua engine function definitions implementation
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaEngineDefs.h"
#include "lua/CLuaCFunctions.h"
#include "CGame.h"
#include "CResource.h"
#include "lua/CLuaMain.h"
#include "lua/CLuaManager.h"
#include "models/CModelManager.h"

void CLuaEngineDefs::LoadFunctions()
{
    constexpr std::pair<const char*, lua_CFunction> functions[] = {
        {"engineRequestModel", ArgumentParser<RequestModel>},
        {"engineFreeModel", ArgumentParser<FreeModel>},
        {"engineGetModelParent", ArgumentParser<GetModelParent>},
        {"engineGetModelType", ArgumentParser<GetModelType>},
        {"engineGetModelAllocatingResource", ArgumentParser<GetModelAllocatingResource>},
        {"engineGetModelsByType", ArgumentParser<GetModelsByType>},
        {"engineIsModelCustom", ArgumentParser<IsModelCustom>},
    };

    for (const auto& [name, func] : functions)
    {
        CLuaCFunctions::AddFunction(name, func);
    }
}

std::variant<std::uint32_t, bool> CLuaEngineDefs::RequestModel(lua_State* luaVM, std::string modelTypeStr, std::optional<std::uint32_t> parentModelIdOpt,
                                                               std::optional<std::uint32_t> requestedIdOpt)
{
    eModelType    modelType = eModelType::UNKNOWN;
    std::uint32_t defaultParent = 0;

    if (modelTypeStr == "vehicle")
    {
        modelType = eModelType::VEHICLE;
        defaultParent = 400;
    }
    else if (modelTypeStr == "ped")
    {
        modelType = eModelType::PED;
        defaultParent = 0;
    }
    else if (modelTypeStr == "object")
    {
        modelType = eModelType::OBJECT;
        defaultParent = 1337;
    }
    else
    {
        return false;
    }

    std::uint32_t parentModelId = parentModelIdOpt.value_or(defaultParent);
    std::uint32_t requestedId = requestedIdOpt.value_or(0);

    CLuaMain*  luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    CResource* resource = luaMain ? luaMain->GetResource() : nullptr;

    if (g_pGame && g_pGame->GetModelManager())
    {
        auto customModel = g_pGame->GetModelManager()->RequestModel(resource, modelType, parentModelId, requestedId);
        if (customModel)
            return customModel->GetModelId();
    }

    return false;
}

bool CLuaEngineDefs::FreeModel(lua_State* luaVM, std::uint32_t modelId)
{
    CLuaMain*  luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    CResource* resource = luaMain ? luaMain->GetResource() : nullptr;

    if (g_pGame && g_pGame->GetModelManager())
    {
        return g_pGame->GetModelManager()->FreeModel(modelId, resource);
    }

    return false;
}

std::variant<std::uint32_t, bool> CLuaEngineDefs::GetModelParent(std::uint32_t modelId)
{
    if (g_pGame && g_pGame->GetModelManager())
    {
        auto model = g_pGame->GetModelManager()->FindModel(modelId);
        if (model)
            return model->GetParentModelId();
    }

    return false;
}

std::variant<std::string, bool> CLuaEngineDefs::GetModelType(std::uint32_t modelId)
{
    if (g_pGame && g_pGame->GetModelManager())
    {
        auto model = g_pGame->GetModelManager()->FindModel(modelId);
        if (model)
        {
            switch (model->GetModelType())
            {
                case eModelType::VEHICLE:
                    return std::string("vehicle");
                case eModelType::PED:
                    return std::string("ped");
                case eModelType::OBJECT:
                    return std::string("object");
                default:
                    break;
            }
        }
    }

    return false;
}

std::variant<CResource*, bool> CLuaEngineDefs::GetModelAllocatingResource(std::uint32_t modelId)
{
    if (g_pGame && g_pGame->GetModelManager())
    {
        auto model = g_pGame->GetModelManager()->FindModel(modelId);
        if (model && model->IsCustom() && model->GetResource())
        {
            return model->GetResource();
        }
    }

    return false;
}

std::vector<std::uint32_t> CLuaEngineDefs::GetModelsByType(std::string modelTypeStr, std::optional<std::uint32_t> minModelIdOpt)
{
    eModelType modelType = eModelType::UNKNOWN;
    if (modelTypeStr == "vehicle")
        modelType = eModelType::VEHICLE;
    else if (modelTypeStr == "ped")
        modelType = eModelType::PED;
    else if (modelTypeStr == "object")
        modelType = eModelType::OBJECT;
    else
        return {};

    std::uint32_t minModelId = minModelIdOpt.value_or(0);

    if (g_pGame && g_pGame->GetModelManager())
    {
        return g_pGame->GetModelManager()->GetModelsByType(modelType, minModelId);
    }

    return {};
}

bool CLuaEngineDefs::IsModelCustom(std::uint32_t modelId)
{
    if (g_pGame && g_pGame->GetModelManager())
    {
        auto model = g_pGame->GetModelManager()->FindModel(modelId);
        if (model)
            return model->IsCustom();
    }

    return false;
}
