/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CModelAtomic.h
 *  PURPOSE:     Atomic model class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CModelVehicle;

#pragma once

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
    eVehicleType          eVehicleType = eVehicleType::CAR;
    uint8_t               cAttributes = 0;
    bool                  bHasDoors = true;
    const CHandlingEntry* pHandling = nullptr;
    const char*           strVehicleName = "NoName";
    eVehicleVariationType eVehicleVariationType = eVehicleVariationType::DEFAULT;
    CVehicleColor         vehicleColors;
};

class CModelVehicle : public CModelBase
{
public:
    // CModelVehicle(){};
    CModelVehicle(uint32_t uiModelID, const SModelVehicleDefs* SModelVehicleDefs);
    ~CModelVehicle();

    virtual CModelVehicle* Clone(uint32_t uiModelID);

    CHandlingEntry*        GetVehicleHandling() { return m_pVehicleHandling; };
    const CHandlingEntry*  GetOriginalHandling() { return m_modelDef->pHandling; };
    void                   SetVehicleHandling(CHandlingEntry* pEntry) { m_pVehicleHandling = pEntry; };
    void                   SetVehicleHandlingChanged(bool bState) { m_bVehicleHandlingChanged = bState; };
    bool                   HasVehicleHandlingChanged() { return m_bVehicleHandlingChanged; };

    bool                   HasDamageModel();
    bool                   IsTrailer() { return m_modelDef->eVehicleType == eVehicleType::TRAILER; };
    const char*            GetVehicleName() { return m_modelDef->strVehicleName; };
    eVehicleType           GetVehicleType() { return m_modelDef->eVehicleType; }
    uint8_t                GetVariantsCount() { return m_modelDef->uiVariantsCount; };
    uint8_t                GetAttributes() { return m_modelDef->cAttributes; };
    uint8_t                GetPassengesCount() { return m_modelDef->uiMaxPassengers; };
    eVehicleVariationType  GetVariationType() { return m_modelDef->eVehicleVariationType; };
    
    eModelInfoType GetType() { return eModelInfoType::VEHICLE; };
    void           Unload();

private:
    const SModelVehicleDefs*    m_modelDef;
    CHandlingEntry*       m_pVehicleHandling;
    bool                  m_bVehicleHandlingChanged = false;
};
