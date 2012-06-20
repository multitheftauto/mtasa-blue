/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPedStopSyncPacket.h
*  PURPOSE:     Ped stop synchronization packet class
*  DEVELOPERS:  lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPEDSTOPSYNCPACKET_H
#define __PACKETS_CPEDSTOPSYNCPACKET_H

#include "CPacket.h"

class CPedStopSyncPacket : public CPacket
{
public:
    inline                  CPedStopSyncPacket                      ( ElementID ID )                                    { m_ID = ID; };

    inline ePacketID                GetPacketID                     ( void ) const                                      { return PACKET_ID_PED_STOPSYNC; };
    inline unsigned long            GetFlags                        ( void ) const                                      { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    inline bool             Write                                   ( NetBitStreamInterface& BitStream ) const          { BitStream.Write ( m_ID ); return true; };

private:
    ElementID               m_ID;
};

#endif
