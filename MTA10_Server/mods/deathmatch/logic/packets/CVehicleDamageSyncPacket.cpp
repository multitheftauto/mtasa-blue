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

bool CVehicleDamageSyncPacket::Read ( NetServerBitStreamInterface& BitStream )
{
    return ( BitStream.Read ( m_Vehicle ) &&
             BitStream.Read ( (char*) m_ucDoorStates, MAX_DOORS ) &&
             BitStream.Read ( (char*) m_ucWheelStates, MAX_WHEELS ) &&
             BitStream.Read ( (char*) m_ucPanelStates, MAX_PANELS ) &&
             BitStream.Read ( (char*) m_ucLightStates, MAX_LIGHTS ) );
}


bool CVehicleDamageSyncPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    BitStream.Write ( m_Vehicle );
    BitStream.Write ( (char*) m_ucDoorStates, MAX_DOORS );
    BitStream.Write ( (char*) m_ucWheelStates, MAX_WHEELS );
    BitStream.Write ( (char*) m_ucPanelStates, MAX_PANELS );
    BitStream.Write ( (char*) m_ucLightStates, MAX_LIGHTS );

    return true;
}
