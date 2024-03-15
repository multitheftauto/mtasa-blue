/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CVehicleUpgrades.h
 *  PURPOSE:     Vehicle upgrades class header
 *
 *****************************************************************************/

#pragma once

#define VEHICLE_UPGRADE_SLOTS 17

class CClientVehicle;

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
    CVehicleUpgrades(CClientVehicle* pVehicle);

    static bool IsUpgrade(std::uint16_t usModel);
    bool        IsUpgradeCompatible(std::uint16_t usUpgrade);
    static bool GetSlotFromUpgrade(std::uint16_t usUpgrade, std::uint8_t& ucSlot);

    bool               AddUpgrade(std::uint16_t usUpgrade, bool bAddedLocally);
    void               AddAllUpgrades();
    void               ForceAddUpgrade(std::uint16_t usUpgrade);
    bool               HasUpgrade(std::uint16_t usUpgrade);
    bool               RemoveUpgrade(std::uint16_t usUpgrade);
    std::uint16_t     GetSlotState(std::uint8_t ucSlot);
    const SSlotStates& GetSlotStates() { return m_SlotStates; }
    static const char* GetSlotName(std::uint8_t ucSlot);

    void ReAddAll();
    void RemoveAll(bool bRipFromVehicle);

    void RestreamVehicleUpgrades(std::uint16_t usModel);

protected:
    SSlotStates     m_SlotStates;
    CClientVehicle* m_pVehicle;
    ushort          m_usLastLocalAddNitroType;
    CElapsedTime    m_lastLocalAddNitroTimer;
};
