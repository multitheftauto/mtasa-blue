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
    inline                  CPlayerDisconnectedPacket   ( void )                    { m_szReason [0] = 0; m_szReason [sizeof ( m_szReason ) - 1] = 0; };
    inline                  CPlayerDisconnectedPacket   ( const char* szReason )    { strncpy ( m_szReason, szReason, sizeof ( m_szReason ) ); m_szReason [sizeof ( m_szReason ) - 1] = 0; };

    inline ePacketID        GetPacketID                 ( void ) const              { return PACKET_ID_SERVER_DISCONNECTED; };
    inline unsigned long    GetFlags                    ( void ) const              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    inline bool             Write                       ( NetBitStreamInterface& BitStream ) const    { BitStream.Write ( const_cast < char* > ( m_szReason ), strlen ( m_szReason ) ); return true; };

    inline const char*      GetReason                   ( void )                    { return m_szReason; };
    inline void             SetReason                   ( const char* szReason )    { strncpy ( m_szReason, szReason, sizeof ( m_szReason ) ); };

private:
    char                    m_szReason [256];
};

#endif
