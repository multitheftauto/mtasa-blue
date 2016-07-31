/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleDamageSyncPacket.cpp
*  PURPOSE:     Vehicle damage synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CVehicleDamageSyncPacket::CVehicleDamageSyncPacket ()
: m_damage ( true, true, true, true, true )
{
}

bool CVehicleDamageSyncPacket::Read ( NetBitStreamInterface& BitStream )
{
     return ( BitStream.Read ( m_Vehicle ) &&
              BitStream.Read ( &m_damage ) );
}


bool CVehicleDamageSyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    BitStream.Write ( m_Vehicle );
    BitStream.Write ( &m_damage );

    return true;
}

void CVehicleDamageSyncPacket::SetFromVehicle ( CVehicle* pVehicle )
{
    m_Vehicle = pVehicle->GetID();
    m_damage.data.ucDoorStates = pVehicle->m_ucDoorStates;
    m_damage.data.ucWheelStates = pVehicle->m_ucWheelStates;
    m_damage.data.ucPanelStates = pVehicle->m_ucPanelStates;
    m_damage.data.ucLightStates = pVehicle->m_ucLightStates;
    for ( unsigned int i = 0; i < MAX_DOORS; ++i )
        m_damage.data.bDoorStatesChanged [ i ] = true;
    for ( unsigned int i = 0; i < MAX_WHEELS; ++i )
        m_damage.data.bWheelStatesChanged [ i ] = true;
    for ( unsigned int i = 0; i < MAX_PANELS; ++i )
        m_damage.data.bPanelStatesChanged [ i ] = true;
    for ( unsigned int i = 0; i < MAX_LIGHTS; ++i )
        m_damage.data.bLightStatesChanged [ i ] = true;
}

