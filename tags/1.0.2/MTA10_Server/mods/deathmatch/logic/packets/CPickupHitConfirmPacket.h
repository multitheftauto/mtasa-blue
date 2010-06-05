/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPickupHitConfirmPacket.h
*  PURPOSE:     Pickup hit confirmation packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPICKUPHITCONFIRMPACKET_H
#define __PACKETS_CPICKUPHITCONFIRMPACKET_H

#include "CPacket.h"

class CPickupHitConfirmPacket : public CPacket
{
public:
    inline explicit                 CPickupHitConfirmPacket     ( class CPickup* pPickup, bool bPlaySound )                  { m_pPickup = pPickup; m_bPlaySound = bPlaySound; };

    inline ePacketID                GetPacketID                 ( void ) const                              { return PACKET_ID_PICKUP_HIT_CONFIRM; };
    inline unsigned long            GetFlags                    ( void ) const                              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                            Write                       ( NetBitStreamInterface& BitStream ) const;

private:
    class CPickup*                  m_pPickup;
    bool                            m_bPlaySound;
};

#endif
