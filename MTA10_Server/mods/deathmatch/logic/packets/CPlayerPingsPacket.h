/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerPingsPacket.h
*  PURPOSE:     Player pinging packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERPINGSPACKET_H
#define __PACKETS_CPLAYERPINGSPACKET_H

#include "CPacket.h"
#include <list>

class CPlayer;

class CPlayerPingsPacket : public CPacket
{
public:
    inline ePacketID                GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_PINGS; };
    inline unsigned long            GetFlags                    ( void ) const                  { return 0; };

    bool                            Write                       ( NetServerBitStreamInterface& BitStream ) const;

    inline void                     AddPlayer                   ( CPlayer* pPlayer )            { m_List.push_back ( pPlayer ); };
    inline void                     RemovePlayer                ( CPlayer* pPlayer )            { if ( !m_List.empty() ) m_List.remove ( pPlayer ); };
    inline void                     RemoveAllPlayers            ( void )                        { m_List.clear (); };

private:
    std::list < CPlayer* >          m_List;
};

#endif
