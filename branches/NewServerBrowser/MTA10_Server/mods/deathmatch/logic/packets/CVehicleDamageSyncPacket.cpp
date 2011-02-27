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
     return ( BitStream.ReadCompressed ( m_Vehicle ) &&
              BitStream.Read ( &m_damage ) );
}


bool CVehicleDamageSyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    BitStream.WriteCompressed ( m_Vehicle );
    BitStream.Write ( &m_damage );

    return true;
}
