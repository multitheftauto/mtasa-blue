/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientModelManager.h
 *  PURPOSE:     Model manager class
 *
 *****************************************************************************/

class CModelManager;

#pragma once

class CModelManager
{
public:
    CModelManager::CModelManager() { }
    ~CModelManager(void) { }

    bool RequestModel(ushort modelID, ushort parentID, eModelType modelType);

    bool IsValidModel(ushort modelID);
    ushort GetModelParent(ushort modelID);
    eModelType GetModelType(ushort modelID);

private:
    std::map<ushort, ushort> m_ModelsParent;
    std::map<ushort, eModelType> m_ModelsType;
    unsigned int m_modelCount = 0;
};
