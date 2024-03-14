/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicleUpgrades.h
 *  PURPOSE:     GTA vehicle upgrade definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CVehicleUpgrades;

#pragma once

#define VEHICLE_UPGRADE_SLOTS 17

#include "CVehicle.h"

enum eVehicleUpgrade
{
    VEHICLEUPGRADE_NITRO_5X = 1008,
    VEHICLEUPGRADE_NITRO_2X = 1009,
    VEHICLEUPGRADE_NITRO_10X = 1010,
    VEHICLEUPGRADE_HYDRAULICS = 1087,
};

typedef SFixedArray<std::uint16_t, VEHICLE_UPGRADE_SLOTS> SSlotStates;

class CVehicleUpgrades
{
public:
    CVehicleUpgrades(CVehicle* pVehicle);
    CVehicleUpgrades(CVehicle* pVehicle, CVehicleUpgrades* pUpgrades);

    bool               IsUpgradeCompatible(std::uint16_t usUpgrade);
    static bool        GetSlotFromUpgrade(std::uint16_t usUpgrade, std::uint8_t& ucSlot);
    void               SetSlotState(std::uint8_t ucSlot, std::uint16_t usUpgrade);
    std::uint16_t     GetSlotState(std::uint8_t ucSlot);
    bool               AddUpgrade(std::uint16_t usUpgrade);
    void               AddAllUpgrades();
    bool               HasUpgrade(std::uint16_t usUpgrade);
    const SSlotStates& GetSlotStates() { return m_SlotStates; }
    std::uint8_t      Count();

    static const char* GetSlotName(std::uint8_t ucSlot);
    static bool        IsValidUpgrade(std::uint16_t usUpgrade);

protected:
    SSlotStates m_SlotStates;
    CVehicle*   m_pVehicle;
};
