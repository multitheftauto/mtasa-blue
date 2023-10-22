/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaModelDefs.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaModelDefs.h"
#include <lua/CLuaFunctionParser.h>
#include "CScriptArgReader.h"
#include "CGame.h"
#include "models/CModelManager.h"
#include <packets/CLuaPacket.h>

void CLuaModelDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"getAllocatedModels", ArgumentParser<GetAllocatedModels>},
        {"getFreeModels", ArgumentParser<GetFreeModels>},
        {"allocateModelFromParent", ArgumentParser<AllocateModelFromParent>},
        {"unloadModel", ArgumentParser<UnloadModel>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

bool CLuaModelDefs::AllocateModelFromParent(lua_State* const luaVM, uint32_t uiModelID, uint32_t uiParentModelID)
{
    // Get the virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return false;

    // Get the resource
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return false;

    // Allocate model
    const bool success = g_pGame->GetModelManager()->AllocateModelFromParent(uiModelID, uiParentModelID);
    if (!success)
        return false;

    // Save model id for resource
    pResource->GetAllocatedModels().push_back(uiModelID);

    // Send network event
    CBitStream BitStream;
    BitStream.pBitStream->Write(uiModelID);
    BitStream.pBitStream->Write(uiParentModelID);
    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CLuaPacket(ALLOCATE_MODEL_FROM_PARENT, *BitStream.pBitStream));

    return true;
}

bool CLuaModelDefs::UnloadModel(lua_State* const luaVM, uint32_t uiModelID)
{
    // Get the virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return false;

    // Get the resource
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return false;

    // Unload model
    const bool success = g_pGame->GetModelManager()->UnloadCustomModel(uiModelID);
    if (!success)
        return false;

    // Remove model from resource
    // TODO: Model can be allocated by another resource
    // This check is unnecessary now
    pResource->GetAllocatedModels().remove(uiModelID);

    // Send network event
    CBitStream BitStream;
    BitStream.pBitStream->Write(uiModelID);
    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CLuaPacket(UNLOAD_MODEL, *BitStream.pBitStream));

    return true;
}

std::list<uint32_t> CLuaModelDefs::GetAllocatedModels(std::optional<eModelInfoType> eType)
{
    std::list<uint32_t> output;

    const auto models = g_pGame->GetModelManager()->GetModels();

    if (eType.has_value())
    {
        for (CModelBase* pModel : models)
        {
            if (pModel && pModel->GetType() == eType.value())
                output.push_back(pModel->GetModelID());
        }
    }
    else
    {
        for (CModelBase* pModel : models)
        {
            if (pModel)
                output.push_back(pModel->GetModelID());
        }
    }

    return output;
}

std::list<uint32_t> CLuaModelDefs::GetFreeModels()
{
    std::list<uint32_t> output;

    const auto models = g_pGame->GetModelManager()->GetModels();
    for (uint32_t uiModelID = 0; uiModelID < MAX_GAME_MODELS; uiModelID++)
    {
        if (!models[uiModelID])
            output.push_back(uiModelID);
    }

    return output;
}
