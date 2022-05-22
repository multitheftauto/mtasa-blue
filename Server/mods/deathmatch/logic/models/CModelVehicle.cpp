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

CModelVehicle::CModelVehicle(uint32_t uiModelID, const SModelVehicleDefs* sModelDefs)
    : CModelBase(uiModelID)
{
    m_modelDef = sModelDefs;
    m_pVehicleHandling = new CHandlingEntry();
    m_pVehicleHandling->ApplyHandlingData(sModelDefs->pHandling);
}

CModelVehicle::~CModelVehicle()
{
}

CModelVehicle* CModelVehicle::Clone(uint32_t uiModelID)
{
    CModelVehicle* pNewModel = new CModelVehicle(uiModelID, m_modelDef);

    pNewModel->SetParentModel(m_uiModelID);

    return pNewModel;
}

bool CModelVehicle::HasDamageModel()
{
    switch (GetVehicleType())
    {
        case eVehicleType::TRAILER:
        case eVehicleType::MONSTERTRUCK:
        case eVehicleType::QUADBIKE:
        case eVehicleType::HELI:
        case eVehicleType::PLANE:
        case eVehicleType::CAR:
            return true;
        default:
            return false;
    }
}

void CModelVehicle::Unload()
{
}
