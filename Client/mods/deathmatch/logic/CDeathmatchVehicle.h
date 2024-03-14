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
    CDeathmatchVehicle(CClientManager* pManager, class CUnoccupiedVehicleSync* pUnoccupiedVehicleSync, ElementID ID, std::uint16_t usVehicleModel,
                       std::uint8_t ucVariant, std::uint8_t ucVariant2);
    ~CDeathmatchVehicle();

    bool IsSyncing() { return m_bIsSyncing; };
    void SetIsSyncing(bool bIsSyncing);

    bool SyncDamageModel();
    void ResetDamageModelSync();

private:
    class CUnoccupiedVehicleSync* m_pUnoccupiedVehicleSync;
    bool                          m_bIsSyncing;

    SFixedArray<std::uint8_t, MAX_DOORS>  m_ucLastDoorStates;
    SFixedArray<std::uint8_t, MAX_WHEELS> m_ucLastWheelStates;
    SFixedArray<std::uint8_t, MAX_PANELS> m_ucLastPanelStates;
    SFixedArray<std::uint8_t, MAX_LIGHTS> m_ucLastLightStates;
};
