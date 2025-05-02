/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicleUpgrades.h
 *  PURPOSE:     GTA vehicle upgrade definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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

typedef SFixedArray<unsigned short, VEHICLE_UPGRADE_SLOTS> SSlotStates;

class CVehicleUpgrades
{
public:
    CVehicleUpgrades(CVehicle* pVehicle);
    CVehicleUpgrades(CVehicle* pVehicle, CVehicleUpgrades* pUpgrades);

    bool               IsUpgradeCompatible(unsigned short usUpgrade);
    static bool        GetSlotFromUpgrade(unsigned short usUpgrade, unsigned char& ucSlot);
    void               SetSlotState(unsigned char ucSlot, unsigned short usUpgrade);
    unsigned short     GetSlotState(unsigned char ucSlot);
    bool               AddUpgrade(unsigned short usUpgrade);
    void               AddAllUpgrades();
    bool               HasUpgrade(unsigned short usUpgrade);
    const SSlotStates& GetSlotStates() { return m_SlotStates; }
    unsigned char      Count();

    static const char* GetSlotName(unsigned char ucSlot);
    static bool        IsValidUpgrade(unsigned short usUpgrade);

protected:
    SSlotStates m_SlotStates;
    CVehicle*   m_pVehicle;
};
