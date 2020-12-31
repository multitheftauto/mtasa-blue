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

    m_ModelsParent[modelID] = parentID;
    m_ModelsType[modelID] = modelType;
    
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
