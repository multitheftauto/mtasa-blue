/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelVehicle.h
 *  PURPOSE:     Vehicle model class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

class CModelVehicle;

#pragma once
#include "CModelBase.h"
#include <CVehicle.h>
#include "CVehicleColors.h"

enum class eVehicleVariationType
{
    DEFAULT,
    NRG,
    SLAMVAN,
    CADDY,
};

struct SModelVehicleDefs
{
    uint8_t               uiMaxPassengers = 0;
    uint8_t               uiVariantsCount = 0;
    eVehicleType          eVehicleModelType = eVehicleType::CAR;
    uint8_t               cAttributes = 0;
    bool                  bHasDoors = true;
    CHandlingEntry        handling;
    const char*           strVehicleName = "NoName";
    eVehicleVariationType eVariationType = eVehicleVariationType::DEFAULT;
    CVehicleColors        vehicleColors;
};

class CModelVehicle : public CModelBase
{
public:
    CModelVehicle(uint32_t uiModelID, const SModelVehicleDefs& SModelVehicleDefs);
    virtual ~CModelVehicle();

    virtual CModelVehicle* Clone(uint32_t uiModelID);

    CHandlingEntry*       GetVehicleHandling() { return m_pVehicleHandling; };
    const CHandlingEntry* GetOriginalHandling() { return &m_modelDef.handling; };
    void                  SetVehicleDefaultHandling(CHandlingEntry& pEntry) { m_modelDef.handling = pEntry; }
    void                  SetVehicleHandling(CHandlingEntry* pEntry) { m_pVehicleHandling = pEntry; };
    void                  SetVehicleHandlingChanged(bool bState) { m_bVehicleHandlingChanged = bState; };
    bool                  HasVehicleHandlingChanged() { return m_bVehicleHandlingChanged; };

    void SetVehicleDafaultColors(CVehicleColors colors) { m_modelDef.vehicleColors = colors; };

    bool                  HasDamageModel();
    bool                  HasDoors() { return m_modelDef.bHasDoors; };
    bool                  IsTrailer() { return m_modelDef.eVehicleModelType == eVehicleType::TRAILER; };
    const char*           GetVehicleName() { return m_modelDef.strVehicleName; };
    eVehicleType          GetVehicleType() { return m_modelDef.eVehicleModelType; }
    uint8_t               GetVariantsCount() { return m_modelDef.uiVariantsCount; };
    uint8_t               GetAttributes() { return m_modelDef.cAttributes; };
    uint8_t               GetPassengesCount() { return m_modelDef.uiMaxPassengers; };
    eVehicleVariationType GetVariationType() { return m_modelDef.eVariationType; };

    CVehicleColor GetRandomColor() { return m_modelDef.vehicleColors.GetRandomColor(); }
    void          AddColor(const CVehicleColor& color) { return m_modelDef.vehicleColors.AddColor(color); };

    void GetRandomVariation(unsigned char& ucVariant, unsigned char& ucVariant2);
    void Unload();

private:
    SModelVehicleDefs m_modelDef;
    CHandlingEntry*   m_pVehicleHandling;
    bool              m_bVehicleHandlingChanged = false;
};
