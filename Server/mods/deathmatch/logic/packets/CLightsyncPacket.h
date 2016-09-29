/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CLightsyncPacket.h
*  PURPOSE:     Lightweight synchronization packet class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CCommon.h"
#include "CPacket.h"
#include "CPlayer.h"
#include "Common.h"
#include <vector>

class CLightsyncPacket : public CPacket
{
public:
                                CLightsyncPacket            () {}

    inline ePacketID                GetPacketID                 ( void ) const                  { return PACKET_ID_LIGHTSYNC; };
    inline unsigned long            GetFlags                    ( void ) const                  { return PACKET_LOW_PRIORITY; };

    void                        AddPlayer                   ( CPlayer* pPlayer ) { m_players.push_back(pPlayer); }
    unsigned int                Count                       () const { return m_players.size(); }
    void                        Reset                       () { m_players.clear(); }

    bool                        Read                        ( NetBitStreamInterface& BitStream );
    bool                        Write                       ( NetBitStreamInterface& BitStream ) const;

private:
    std::vector < CPlayer* >    m_players;
};

