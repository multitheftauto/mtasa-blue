/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerChangeNickPacket.h
*  PURPOSE:     Player nickname change packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERCHANGENICKPACKET_H
#define __PACKETS_CPLAYERCHANGENICKPACKET_H

#include "CPacket.h"
#include "../../Config.h"

class CPlayerChangeNickPacket : public CPacket
{
public:
    explicit                CPlayerChangeNickPacket     ( const char* szNewNick );

    inline ePacketID        GetPacketID                 ( void ) const          { return PACKET_ID_PLAYER_CHANGE_NICK; };
    inline unsigned long    GetFlags                    ( void ) const          { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

    inline const char*      GetNewNick                  ( void )                    { return m_szNewNick; };
    inline void             SetNewNick                  ( const char* szNewNick )   { strncpy ( m_szNewNick, szNewNick, MAX_NICK_LENGTH ); };

private:
    char                    m_szNewNick [MAX_NICK_LENGTH + 1];
};

#endif
