/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CObjectStopSyncPacket.h
*  PURPOSE:     Object stop sync packet class
*  DEVELOPERS:  Stanislav Izmalkov <izstas@live.ru>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_COBJECTSTOPSYNCPACKET_H
#define __PACKETS_COBJECTSTOPSYNCPACKET_H

#include "CPacket.h"

class CObjectStopSyncPacket : public CPacket
{
public:
    inline                  CObjectStopSyncPacket                   ( CObject* pObject )                                { m_pObject = pObject; };

    inline ePacketID                GetPacketID                     ( void ) const                                      { return PACKET_ID_OBJECT_STOPSYNC; };
    inline unsigned long            GetFlags                        ( void ) const                                      { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    inline bool             Write                                   ( NetBitStreamInterface& BitStream ) const          { BitStream.Write ( m_pObject->GetID () ); return true; };

private:
    CObject*                m_pObject;
};

#endif
