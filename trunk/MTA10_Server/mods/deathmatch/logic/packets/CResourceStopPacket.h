/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CResourceStopPacket.h
*  PURPOSE:     Resource stop packet handler class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CRESOURCESTOPPACKET_H
#define __PACKETS_CRESOURCESTOPPACKET_H

#include "../packets/CPacket.h"

class CResourceStopPacket : public CPacket
{
public:
                            CResourceStopPacket         ( unsigned short usID );
                            ~CResourceStopPacket        ( void );

    inline ePacketID        GetPacketID                 ( void ) const      { return PACKET_ID_RESOURCE_STOP; };
    inline unsigned long    GetFlags                    ( void ) const      { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                       ( NetServerBitStreamInterface& BitStream ) const;

private:
    unsigned short          m_usID;
};

#endif
