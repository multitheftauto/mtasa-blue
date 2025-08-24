/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPickupHitConfirmPacket.h
 *  PURPOSE:     Pickup hit confirmation packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPickupHitConfirmPacket final : public CPacket
{
public:
    explicit CPickupHitConfirmPacket(class CPickup* pPickup, bool bPlaySound)
    {
        m_pPickup = pPickup;
        m_bPlaySound = bPlaySound;
    };

    ePacketID     GetPacketID() const { return PACKET_ID_PICKUP_HIT_CONFIRM; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    class CPickup* m_pPickup;
    bool           m_bPlaySound;
};
