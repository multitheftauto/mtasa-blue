/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelVehicle.cpp
 *  PURPOSE:     Vehicle model info class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "StdInc.h"
#include "CModelVehicle.h"
#include "CGame.h"
#include "CVehicleManager.h"

CModelVehicle::CModelVehicle(uint32_t uiModelID, const SModelVehicleDefs& sModelDefs) : CModelBase(eModelInfoType::VEHICLE, uiModelID)
{
    m_modelDef = sModelDefs;
    m_pVehicleHandling = new CHandlingEntry();
    m_pVehicleHandling->ApplyHandlingData((const CHandlingEntry*)&sModelDefs.handling);
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

void CModelVehicle::GetRandomVariation(unsigned char& ucVariant, unsigned char& ucVariant2)
{
    RandomizeRandomSeed();
    ucVariant = 255;
    ucVariant2 = 255;
    // Valid model?
    char cVariants = GetVariantsCount();
    if (cVariants != 255)
    {
        switch (m_modelDef.eVariationType)
        {
            case eVehicleVariationType::CADDY:
                // 255, 0, 1, 2
                ucVariant = (rand() % 4) - 1;

                // 3, 4, 5
                ucVariant2 = (rand() % 3);
                ucVariant2 += 3;
                return;
            case eVehicleVariationType::SLAMVAN:
                // Slamvan has steering wheel "extras" we want one of those so default cannot be an option.
                ucVariant = (rand() % (cVariants + 1));
                return;
            case eVehicleVariationType::NRG:
                // e.g. 581 ( BF400 )
                // first 3 properties are Exhaust
                // last 2 are fairings.

                // 255, 0, 1, 2
                ucVariant = (rand() % 4) - 1;

                // 3, 4
                ucVariant2 = (rand() % 2);
                ucVariant2 += 3;
                return;
            default:
                // e.g. ( rand () % ( 5 + 2 ) ) - 1
                // Can generate 6 then minus 1 = 5
                // Can generate 0 then minus 1 = -1 (255) (default model with nothing)
                ucVariant = (rand() % (cVariants + 2)) - 1;
                return;
        }
    }
}

void CModelVehicle::Unload()
{
    const auto vehicles = g_pGame->GetVehicleManager()->GetVehicles();
    for (CVehicle* pVehicle : vehicles)
    {
        if (pVehicle->GetModel() == m_uiModelID)
        {
            pVehicle->SetModel(m_uiParentID);

            CLuaArguments Arguments;
            Arguments.PushNumber(m_uiModelID);
            Arguments.PushNumber(m_uiParentID);
            pVehicle->CallEvent("onElementModelChange", Arguments);
        }
    }
}
