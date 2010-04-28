/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerQuitPacket.h
*  PURPOSE:     Player quit packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERQUITPACKET_H
#define __PACKETS_CPLAYERQUITPACKET_H

#include "CPacket.h"

class CPlayerQuitPacket : public CPacket
{
public:
                            CPlayerQuitPacket           ( void );

    inline ePacketID        GetPacketID                 ( void ) const                                      { return static_cast < ePacketID > ( PACKET_ID_PLAYER_QUIT ); };
    inline unsigned long    GetFlags                    ( void ) const                                      { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    inline bool             Read                        ( NetBitStreamInterface& BitStream )                { return true; };
    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

    inline ElementID        GetPlayer                   ( void )                                            { return m_PlayerID; };
    inline void             SetPlayer                   ( ElementID PlayerID )                              { m_PlayerID = PlayerID; };

    inline unsigned char    GetQuitReason               ( void )                                            { return m_ucQuitReason; }
    inline void             SetQuitReason               ( unsigned char ucQuitReason )                      { m_ucQuitReason = ucQuitReason; }

private:
    ElementID               m_PlayerID;
    unsigned char           m_ucQuitReason;
};

#endif
