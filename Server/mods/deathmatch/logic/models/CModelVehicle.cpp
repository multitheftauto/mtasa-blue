/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelVehicle.cpp
 *  PURPOSE:     Vehicle model info class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "StdInc.h"

CModelVehicle::CModelVehicle(uint32_t uiModelID, const CHandlingEntry* pHandling, eVehicleType eType, uint8_t cVariantsCount, uint8_t cAttributes,
                             uint8_t cPassengesCount)
    : CModelBase(uiModelID)
{
    m_pOriginalVehicleHandling = pHandling;
    m_pVehicleHandling = new CHandlingEntry();
    m_pVehicleHandling->ApplyHandlingData(pHandling);
    m_eVehicleType = eType;
    m_cVariantsCount = cVariantsCount;
    m_cAttributes = cAttributes;
    m_cPassengesCount = cPassengesCount;
}

CModelVehicle::~CModelVehicle()
{
}

CModelVehicle* CModelVehicle::Clone(uint32_t uiModelID)
{
    CModelVehicle* pNewModel = new CModelVehicle(uiModelID, m_pOriginalVehicleHandling, m_eVehicleType, m_cVariantsCount, m_cAttributes, m_cPassengesCount);

    pNewModel->SetParentModel(m_uiModelID);

    return pNewModel;
}

void CModelVehicle::Unload()
{
}
