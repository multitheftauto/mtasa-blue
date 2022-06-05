/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CModelManager.cpp
 *  PURPOSE:     Model manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CModelManager.h"

CModelManager::CModelManager()
{
    m_vModels.resize(25500);
}

CModelManager::~CModelManager()
{
    for (auto* pModel : m_vModels)
    {
        if (pModel)
        {
            RemoveModel(pModel);
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

void CModelManager::RemoveModel(CModelBase* pModel)
{
    pModel->Unload();

    switch (pModel->GetType())
    {
        case eModelInfoType::ATOMIC:
            delete dynamic_cast<CModelAtomic*>(pModel);
            break;
        case eModelInfoType::VEHICLE:
            delete dynamic_cast<CModelVehicle*>(pModel);
            break;
        default:
            break;
    }
}
