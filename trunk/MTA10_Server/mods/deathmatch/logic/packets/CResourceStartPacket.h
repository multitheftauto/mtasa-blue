/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CResourceStartPacket.h
*  PURPOSE:     Resource start packet class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CRESOURCESTARTPACKET_H
#define __PACKETS_CRESOURCESTARTPACKET_H

#include <list>
#include "../packets/CPacket.h"
#include "../CResource.h"

class CResourceStartPacket : public CPacket
{
public:
                            CResourceStartPacket        ( const char* szResourceName,
                                                          class CResource* pResource );

    inline ePacketID        GetPacketID                 ( void ) const      { return PACKET_ID_RESOURCE_START; };
    inline unsigned long    GetFlags                    ( void ) const      { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                       ( NetServerBitStreamInterface& BitStream ) const;

private:
    std::string             m_strResourceName;
    CResource*              m_pResource;
};

#endif
