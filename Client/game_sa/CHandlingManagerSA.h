/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHandlingManagerSA.h
 *  PURPOSE:     Header file for vehicle handling manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CHandlingManager.h>
#include "CHandlingEntrySA.h"
#include "CFlyingHandlingEntrySA.h"
#include "CBoatHandlingEntrySA.h"
#include "CBikeHandlingEntrySA.h"

class CHandlingManagerSA : public CHandlingManager
{
public:
    CHandlingManagerSA();
    ~CHandlingManagerSA();

    CHandlingEntry*       CreateHandlingData();
    CFlyingHandlingEntry* CreateFlyingHandlingData();
    CBoatHandlingEntry*   CreateBoatHandlingData();
    CBikeHandlingEntry*   CreateBikeHandlingData();

    const CHandlingEntry*       GetOriginalHandlingData(eVehicleTypes eModel);
    const CFlyingHandlingEntry* GetOriginalFlyingHandlingData(eVehicleTypes eModel);
    const CBoatHandlingEntry*   GetOriginalBoatHandlingData(eVehicleTypes eModel);
    const CBikeHandlingEntry*   GetOriginalBikeHandlingData(eVehicleTypes eModel);

    eHandlingTypes GetHandlingID(eVehicleTypes eModel);

    eHandlingProperty GetPropertyEnumFromName(std::string strName);

    void CheckSuspensionChanges(CHandlingEntry* pEntry);
    void RemoveChangedVehicle();

private:
    void InitializeDefaultHandlings();

    static DWORD m_dwStore_LoadHandlingCfg;

    // Original handling data unaffected by handling.cfg changes
    static tHandlingDataSA   m_OriginalHandlingData[HT_MAX];
    static CHandlingEntrySA* m_pOriginalEntries[HT_MAX];

    static tFlyingHandlingDataSA   m_OriginalFlyingHandlingData[24];
    static CFlyingHandlingEntrySA* m_pOriginalFlyingEntries[24];

    static tBoatHandlingDataSA   m_OriginalBoatHandlingData[12];
    static CBoatHandlingEntrySA* m_pOriginalBoatEntries[12];

    static tBikeHandlingDataSA   m_OriginalBikeHandlingData[14];
    static CBikeHandlingEntrySA* m_pOriginalBikeEntries[14];

    std::map<std::string, eHandlingProperty> m_HandlingNames;
    int                                      iChangedVehicles;
};
