/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelManager.cpp
 *  PURPOSE:     Model manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CModelManager.h"

CModelManager::CModelManager()
{
    m_vModels.resize(MAX_GAME_MODELS);
}

CModelManager::~CModelManager()
{
    for (auto* pModel : m_vModels)
    {
        if (pModel)
        {
            delete pModel;
        }
    }
}

void CModelManager::RegisterModel(CModelBase* pModelHandler)
{
    uint16_t iModelID = pModelHandler->GetModelID();

    if (m_vModels[iModelID])
        assert("Model currently allocated");

    m_vModels[iModelID] = pModelHandler;
}

bool CModelManager::AllocateModelFromParent(uint32_t uiNewModelID, uint32_t uiParentModel)
{
    if (uiNewModelID >= MAX_GAME_MODELS || uiParentModel >= MAX_GAME_MODELS)
        return false;

    CModelBase* pParentModel = m_vModels[uiParentModel];

    if (!pParentModel)
        return false;

    if (m_vModels[uiNewModelID])
        return false;

    CModelBase* pNewModel = pParentModel->Clone(uiNewModelID);
    RegisterModel(pNewModel);

    // Save in simple allocated list
    m_vSimpleAllocatedModels.push_back(pNewModel);
}

bool CModelManager::UnloadCustomModel(uint32 uiModelID)
{
    CModelBase* pModel = m_vModels[uiModelID];

    if (!pModel)
        return false;

    if (!pModel->IsCustom())
        return false;

    pModel->Unload();

    delete pModel;

    m_vSimpleAllocatedModels.remove(pModel);
    m_vModels[uiModelID] = nullptr;

    return true;
}

CModelVehicle* CModelManager::GetVehicleModel(uint32_t iModelID)
{
    CModelBase* pModel = m_vModels[iModelID];
    if (pModel && pModel->GetType() == eModelInfoType::VEHICLE)
        return dynamic_cast<CModelVehicle*>(pModel);

    return nullptr;
}
