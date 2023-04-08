/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelManager.h
 *  PURPOSE:     Model manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

class CModelManager;

#pragma once

#include "StdInc.h"
#include "CModelBase.h"
#include "CModelAtomic.h"
#include "CModelVehicle.h"
#include "CModelPed.h"

#define MAX_GAME_MODELS 20000

class CModelManager
{
public:
    CModelManager();
    ~CModelManager();

    // Register generic GTA:SA model info
    void RegisterModel(CModelBase* pModelHandler);
    bool AllocateModelFromParent(uint32_t uiNewModelID, uint32_t uiParentModel);
    bool UnloadCustomModel(uint32 uiModelID);

    std::vector<CModelBase*>& GetModels() { return m_vModels; };

    CModelVehicle* GetVehicleModel(uint32_t iModelID);

    std::list<CModelBase*> GetSimpleAllocatedModels() { return m_vSimpleAllocatedModels; };

private:
    // modelID - CModelBase
    std::vector<CModelBase*> m_vModels;
    std::list<CModelBase*>   m_vSimpleAllocatedModels;
};
