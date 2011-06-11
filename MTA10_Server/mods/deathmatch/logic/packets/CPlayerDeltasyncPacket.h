/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerDeltasyncPacket.h
*  PURPOSE:     Player delta synchronization packet class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CPacket.h"
#include "CPlayer.h"

class CPlayerDeltasyncPacket : public CPacket
{
public:
    inline                  CPlayerDeltasyncPacket          ( void )                        {};
    inline explicit         CPlayerDeltasyncPacket          ( CPlayer* pPlayer )            { m_pSourceElement = pPlayer; };

    inline ePacketID        GetPacketID                     ( void ) const                  { return PACKET_ID_PLAYER_DELTASYNC; };
    inline unsigned long    GetFlags                        ( void ) const                  { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; };

    bool                    Read                            ( NetBitStreamInterface& BitStream );
    bool                    Write                           ( NetBitStreamInterface& BitStream ) const;

    void                    PrepareToSendDeltaSync          ();

private:
    SPlayerDeltaSyncData    m_delta;
};

