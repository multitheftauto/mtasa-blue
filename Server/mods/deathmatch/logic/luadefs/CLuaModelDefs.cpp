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
#include "CLuaModelDefs.h"
#include <lua/CLuaFunctionParser.h>
#include "CScriptArgReader.h"
#include "CGame.h"
#include "models/CModelManager.h"
#include <packets/CLuaPacket.h>

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

    // Send network event
    CBitStream BitStream;
    BitStream.pBitStream->Write(uiModelID);
    BitStream.pBitStream->Write(uiParentModelID);
    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CLuaPacket(ALLOCATE_MODEL_FROM_PARENT, *BitStream.pBitStream));
    return true;
}

std::vector<uint> CLuaModelDefs::GetAllocatedModels(std::optional<eModelInfoType> eType)
{
    std::vector<uint> o;
    return o;
}
