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

class CVehicleUpgrades;

#ifndef __CVEHICLEUPGRADES_H
#define __CVEHICLEUPGRADES_H

#define VEHICLE_UPGRADE_SLOTS 17

enum eVehicleUpgrade
{
    VEHICLEUPGRADE_NITRO_5X = 1008,
    VEHICLEUPGRADE_NITRO_2X = 1009,
    VEHICLEUPGRADE_NITRO_10X = 1010,
    VEHICLEUPGRADE_HYDRAULICS = 1087,
};

class CClientVehicle;

class CVehicleUpgrades
{
public:
							CVehicleUpgrades			( CClientVehicle* pVehicle );

    static bool             IsUpgrade                   ( unsigned short usModel );
	bool					IsUpgradeCompatible			( unsigned short usUpgrade );
	static bool				GetSlotFromUpgrade			( unsigned short usUpgrade, unsigned char& ucSlot );
	
	bool					AddUpgrade					( unsigned short usUpgrade );
    void                    AddAllUpgrades              ( void );
    void                    ForceAddUpgrade             ( unsigned short usUpgrade );
	bool					HasUpgrade					( unsigned short usUpgrade );
	bool					RemoveUpgrade				( unsigned short usUpgrade );
	unsigned short			GetSlotState				( unsigned char ucSlot );
    inline unsigned short*	GetSlotStates				( void )		{ return m_SlotStates; }
    static const char*		GetSlotName					( unsigned char ucSlot );

    void                    ReAddAll                    ( void );
    void                    RemoveAll                   ( bool bRipFromVehicle );

protected:
    CClientVehicle*			m_pVehicle;    
	unsigned short			m_SlotStates [ VEHICLE_UPGRADE_SLOTS ];
};

#endif