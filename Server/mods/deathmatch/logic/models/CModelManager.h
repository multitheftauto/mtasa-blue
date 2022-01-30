/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelManager.h
 *  PURPOSE:     Model manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CModelManager;

#pragma once

#include "StdInc.h"
#include <vector>

class CModelManager
{
public:
    CModelManager::CModelManager();
    ~CModelManager();

    // Register generic GTA:SA model info
    void RegisterModel(CModelBase* pModelHandler);

    // Create new GTA:SA model info
    CModelBase* CreateModel(uint32_t iModelID, eModelInfoType eType);

    void CleanResourceModels(CResource* pResource);
    void AllocateModelFromParent(uint32_t uiNewModelID, uint32_t uiParentModel);

    CModelVehicle* GetVehicleModel(uint32_t iModelID) { return dynamic_cast<CModelVehicle*>(m_vModels[iModelID]); };

    std::list<CModelBase*> GetSimpleAllocatedModels() { return m_vSimpleAllocatedModels; };

private:
    void RemoveModel(CModelBase* pModel);

private:
    // modelID - CModelBase
    std::vector<CModelBase*> m_vModels;
    std::list<CModelBase*> m_vSimpleAllocatedModels;
};
