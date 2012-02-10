/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPedStartSyncPacket.h
*  PURPOSE:     Ped start synchronization packet class
*  DEVELOPERS:  lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPEDSTARTSYNCPACKET_H
#define __PACKETS_CPEDSTARTSYNCPACKET_H

#include "CPacket.h"
class CPed;

class CPedStartSyncPacket : public CPacket
{
public:
    inline                  CPedStartSyncPacket                     ( CPed * pPed )                             { m_pPed = pPed; };

    inline ePacketID                GetPacketID                     ( void ) const                              { return PACKET_ID_PED_STARTSYNC; };
    inline unsigned long            GetFlags                        ( void ) const                              { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                                   ( NetBitStreamInterface& BitStream ) const;

private:
    CPed *                  m_pPed;
};

#endif
