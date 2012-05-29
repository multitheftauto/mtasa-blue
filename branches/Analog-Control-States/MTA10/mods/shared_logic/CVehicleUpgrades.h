/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CVehicleUpgrades.h
*  PURPOSE:     Vehicle upgrades class header
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CVEHICLEUPGRADES_H
#define __CVEHICLEUPGRADES_H

#define VEHICLE_UPGRADE_SLOTS 17

class CClientVehicle;

enum eVehicleUpgrade
{
    VEHICLEUPGRADE_NITRO_5X = 1008,
    VEHICLEUPGRADE_NITRO_2X = 1009,
    VEHICLEUPGRADE_NITRO_10X = 1010,
    VEHICLEUPGRADE_HYDRAULICS = 1087,
};

typedef SFixedArray < unsigned short, VEHICLE_UPGRADE_SLOTS > SSlotStates;

class CVehicleUpgrades
{
public:
                            CVehicleUpgrades            ( CClientVehicle* pVehicle );

    static bool             IsUpgrade                   ( unsigned short usModel );
    bool                    IsUpgradeCompatible         ( unsigned short usUpgrade );
    static bool             GetSlotFromUpgrade          ( unsigned short usUpgrade, unsigned char& ucSlot );
    
    bool                    AddUpgrade                  ( unsigned short usUpgrade );
    void                    AddAllUpgrades              ( void );
    void                    ForceAddUpgrade             ( unsigned short usUpgrade );
    bool                    HasUpgrade                  ( unsigned short usUpgrade );
    bool                    RemoveUpgrade               ( unsigned short usUpgrade );
    unsigned short          GetSlotState                ( unsigned char ucSlot );
    const SSlotStates&      GetSlotStates               ( void )        { return m_SlotStates; }
    static const char*      GetSlotName                 ( unsigned char ucSlot );

    void                    ReAddAll                    ( void );
    void                    RemoveAll                   ( bool bRipFromVehicle );

protected:
    SSlotStates             m_SlotStates;
    CClientVehicle*         m_pVehicle;
};

#endif
