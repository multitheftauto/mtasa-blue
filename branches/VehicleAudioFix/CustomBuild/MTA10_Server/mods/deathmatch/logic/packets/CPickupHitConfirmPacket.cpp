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

bool CPickupHitConfirmPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // unsigned short   (2)     - pickup id
    // bool                     - hide it?

    // Got a pickup to send?
    if ( m_pPickup )
    {
        // Write the pickup id and visibily state
        BitStream.WriteCompressed ( m_pPickup->GetID () );

        // WRite the flags
        BitStream.WriteBit ( m_pPickup->IsVisible () );
        BitStream.WriteBit ( m_bPlaySound );

        return true;
    }

    return false;
}
