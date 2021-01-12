/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientModelManager.cpp
 *  PURPOSE:     Model manager class
 *
 *****************************************************************************/

#include "StdInc.h"

bool CModelManager::RequestModel(ushort modelID, ushort parentID, eModelType modelType)
{
    if (IsValidModel(modelID))
        return false;

    if (HasModelsByType(modelType))
        m_Models[modelType][modelID] = parentID;
    else
        m_Models[modelType] = { { modelID, parentID } };

    m_ModelsParent[modelID] = parentID;
    m_ModelsType[modelID] = modelType;

    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CServerCustomModelsPacket(modelType, modelID, parentID));
    
    return true;
}

bool CModelManager::IsValidModel(ushort modelID)
{
    return m_ModelsParent.find(modelID) != m_ModelsParent.end();
}

ushort CModelManager::GetModelParent(ushort modelID)
{
    return m_ModelsParent[modelID];
}

eModelType CModelManager::GetModelType(ushort modelID)
{
    return m_ModelsType[modelID];
}

bool CModelManager::HasModelsByType(eModelType modelType)
{
    return m_Models.find(modelType) != m_Models.end();
}

std::map<unsigned short, unsigned short> CModelManager::GetModels(eModelType modelType)
{
    return m_Models[modelType];
}
