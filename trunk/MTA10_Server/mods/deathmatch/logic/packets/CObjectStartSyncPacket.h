/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CObjectStartSyncPacket.h
*  PURPOSE:     Header for object start sync packet class
*  DEVELOPERS:  Stanislav Izmalkov <izstas@live.ru>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_COBJECTSTARTSYNCPACKET_H
#define __PACKETS_COBJECTSTARTSYNCPACKET_H

#include "CPacket.h"

class CObjectStartSyncPacket : public CPacket
{
public:
    inline                  CObjectStartSyncPacket                  ( CObject* pObject )                        { m_pObject = pObject; };

    inline ePacketID                GetPacketID                     ( void ) const                  { return PACKET_ID_OBJECT_STARTSYNC; };
    inline NetServerPacketOrdering  GetPacketOrdering               ( void ) const                  { return PACKET_ORDERING_OTHERSYNC; }
    inline unsigned long            GetFlags                        ( void ) const                  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                                   ( NetBitStreamInterface& BitStream ) const;

private:
    CObject*                m_pObject;
};

#endif
