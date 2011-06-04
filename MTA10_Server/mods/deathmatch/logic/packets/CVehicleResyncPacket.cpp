/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleResyncPacket.cpp
*  PURPOSE:     Vehicle re-synchronization packet class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

bool CVehicleResyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Only the server sends these.
    return false;
}

bool CVehicleResyncPacket::Write ( NetBitStreamInterface& BitStream ) const 
{
    return true;
}
