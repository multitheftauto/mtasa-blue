/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerTransgressionPacket.h
*  PURPOSE:
*  DEVELOPERS: 
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERTRANSGRESSIONPACKET_H
#define __PACKETS_CPLAYERTRANSGRESSIONPACKET_H


class CPlayerTransgressionPacket : public CPacket
{
public:

    inline ePacketID                        GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_TRANSGRESSION; };
    unsigned long                           GetFlags                    ( void ) const                  { return PACKET_SEQUENCED; };

    bool                                    Read                        ( NetBitStreamInterface& BitStream );

    uint                                    m_uiLevel;
    SString                                 m_strMessage;
};


#endif
