/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDeathmatchVehicle.h
 *  PURPOSE:     Header for deathmatch vehicle class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientVehicle.h"

class CDeathmatchVehicle final : public CClientVehicle
{
    DECLARE_CLASS(CDeathmatchVehicle, CClientVehicle)
public:
    CDeathmatchVehicle(CClientManager* pManager, class CUnoccupiedVehicleSync* pUnoccupiedVehicleSync, ElementID ID, unsigned short usVehicleModel,
                       unsigned char ucVariant, unsigned char ucVariant2);
    ~CDeathmatchVehicle();

    bool IsSyncing() { return m_bIsSyncing; };
    void SetIsSyncing(bool bIsSyncing);

    bool SyncDamageModel();
    void ResetDamageModelSync();

private:
    class CUnoccupiedVehicleSync* m_pUnoccupiedVehicleSync;
    bool                          m_bIsSyncing;

    SFixedArray<unsigned char, MAX_DOORS>  m_ucLastDoorStates;
    SFixedArray<unsigned char, MAX_WHEELS> m_ucLastWheelStates;
    SFixedArray<unsigned char, MAX_PANELS> m_ucLastPanelStates;
    SFixedArray<unsigned char, MAX_LIGHTS> m_ucLastLightStates;
};
