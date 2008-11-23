/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPickupHitConfirmPacket.cpp
*  PURPOSE:     Pickup hit confirmation packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CPickupHitConfirmPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    // unsigned short   (2)     - pickup id
    // bool                     - hide it?

    // Got a pickup to send?
    if ( m_pPickup )
    {
        // Write the pickup id and visibily state
        BitStream.Write ( m_pPickup->GetID () );

        // Populate the flags
        unsigned char ucFlags = 0;
        ucFlags |= m_pPickup->IsVisible () ? 1:0;
        ucFlags |= m_bPlaySound << 1;

        // Write the flags
        BitStream.Write ( ucFlags );
        return true;
    }

    return false;
}
