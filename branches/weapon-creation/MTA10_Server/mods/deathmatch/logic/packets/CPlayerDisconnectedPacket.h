/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerDisconnectedPacket.h
*  PURPOSE:     Player disconnected packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERDISCONNECTEDPACKET_H
#define __PACKETS_CPLAYERDISCONNECTEDPACKET_H

#include "../packets/CPacket.h"

class CPlayerDisconnectedPacket : public CPacket
{
public:
    inline                  CPlayerDisconnectedPacket   ( void )                    {}
    inline                  CPlayerDisconnectedPacket   ( const char* szReason )    { m_strReason = szReason; }

    inline ePacketID        GetPacketID                 ( void ) const              { return PACKET_ID_SERVER_DISCONNECTED; };
    inline unsigned long    GetFlags                    ( void ) const              { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    inline bool             Write                       ( NetBitStreamInterface& BitStream ) const    { BitStream.Write ( *m_strReason, m_strReason.length () ); return true; }

    inline const char*      GetReason                   ( void )                    { return m_strReason; }
    inline void             SetReason                   ( const char* szReason )    { m_strReason = szReason; }

private:
    SString                 m_strReason;
};

#endif
