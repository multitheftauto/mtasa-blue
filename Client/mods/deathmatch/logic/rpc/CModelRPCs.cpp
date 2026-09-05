/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/rpc/CModelRPCs.cpp
 *  PURPOSE:     Server model RPC handlers implementation
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CModelRPCs.h"
#include "CRPCFunctions.h"
#include "CClientModelManager.h"
#include <CServerModelDefinition.h>

void CModelRPCs::LoadFunctions()
{
    AddHandler(ALLOCATE_SERVER_MODEL, AllocateServerModel, "AllocateServerModel");
    AddHandler(FREE_SERVER_MODEL, FreeServerModel, "FreeServerModel");
}

void CModelRPCs::AllocateServerModel(NetBitStreamInterface& bitStream)
{
    SServerModelDefinition definition;
    std::uint8_t           type = 0;

    if (bitStream.Read(definition.logicalModelId) && bitStream.Read(definition.parentModelId) && bitStream.Read(type) && bitStream.ReadString(definition.name))
    {
        definition.type = static_cast<eServerModelType>(type);
        if (g_pClientGame && g_pClientGame->GetManager() && g_pClientGame->GetManager()->GetModelManager())
        {
            g_pClientGame->GetManager()->GetModelManager()->AllocateServerModel(definition);
        }
    }
}

void CModelRPCs::FreeServerModel(NetBitStreamInterface& bitStream)
{
    std::uint16_t logicalModelId = 0;
    if (bitStream.Read(logicalModelId))
    {
        if (g_pClientGame && g_pClientGame->GetManager() && g_pClientGame->GetManager()->GetModelManager())
        {
            g_pClientGame->GetManager()->GetModelManager()->FreeServerModel(logicalModelId);
        }
    }
}
