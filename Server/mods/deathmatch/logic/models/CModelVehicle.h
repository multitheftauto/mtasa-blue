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

class CModelVehicle : public CModelBase
{
public:
    // CModelVehicle(){};
    CModelVehicle(uint32_t uiModelID, const CHandlingEntry* pHandling, eVehicleType eType, uint8_t cVariantsCount, uint8_t cAttributes, uint8_t cPassengesCount);
    ~CModelVehicle();

    virtual CModelVehicle* Clone(uint32_t uiModelID);
    CHandlingEntry*        GetVehicleHandling() { return m_pVehicleHandling; };
    const CHandlingEntry*  GetOriginalHandling() { return m_pOriginalVehicleHandling; };
    void                   SetVehicleHandling(CHandlingEntry* pEntry) { m_pVehicleHandling = pEntry; };
    void                   SetVehicleHandlingChanged(bool bState) { m_bVehicleHandlingChanged = bState; };
    bool                   HasVehicleHandlingChanged() { return m_bVehicleHandlingChanged; };

    eVehicleType GetVehicleType() { return m_eVehicleType; };
    uint8_t GetVariantsCount() { return m_cVariantsCount; };
    uint8_t GetAttributes() { return m_cAttributes; };
    uint8_t GetPassengesCount() { return m_cPassengesCount; };
    
    eModelInfoType GetType() { return eModelInfoType::VEHICLE; };
    void           Unload();

private:
    CHandlingEntry*       m_pVehicleHandling;
    const CHandlingEntry* m_pOriginalVehicleHandling;
    bool                  m_bVehicleHandlingChanged = false;
    eVehicleType          m_eVehicleType;
    uint8_t               m_cVariantsCount;
    uint8_t               m_cAttributes;
    uint8_t               m_cPassengesCount;
};
