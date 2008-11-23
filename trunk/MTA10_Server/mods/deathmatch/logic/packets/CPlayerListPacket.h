/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerListPacket.h
*  PURPOSE:     Player list packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERLISTPACKET_H
#define __PACKETS_CPLAYERLISTPACKET_H

#include "CPacket.h"
#include <list>

using namespace std;

class CPlayerListPacket : public CPacket
{
public:
    inline ePacketID            GetPacketID                 ( void ) const              { return PACKET_ID_PLAYER_LIST; };
    inline unsigned long        GetFlags                    ( void ) const              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                        Write                       ( NetServerBitStreamInterface& BitStream ) const;

    inline void                 AddPlayer                   ( CPlayer* pPlayer )        { m_List.push_back ( pPlayer ); };
    inline void                 RemovePlayer                ( CPlayer* pPlayer )        { if ( !m_List.empty() ) m_List.remove ( pPlayer ); };
    inline void                 RemoveAllPlayers            ( void )                    { m_List.clear (); };

    inline bool                 GetShowInChat               ( void )                    { return m_bShowInChat; };
    inline void                 SetShowInChat               ( bool bShowInChat )        { m_bShowInChat = bShowInChat; };

private:
    list < CPlayer* >           m_List;
    bool                        m_bShowInChat;
    
};

#endif
