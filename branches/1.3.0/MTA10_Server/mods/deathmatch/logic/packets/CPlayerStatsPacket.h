/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerStatsPacket.h
*  PURPOSE:     Player statistics packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPLAYERSTATSPACKET_H
#define __CPLAYERSTATSPACKET_H

#include "CPacket.h"
#include <CVector.h>
#include "../CPlayerStats.h"

struct sPlayerStat
{
    unsigned short id;
    float value;
};

class CPlayerStatsPacket : public CPacket
{
public:
                                ~CPlayerStatsPacket         ( void );

    inline ePacketID            GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_STATS; };
    inline unsigned long        GetFlags                    ( void ) const                  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                        Write                       ( NetBitStreamInterface& BitStream ) const;

    void                        Add                         ( unsigned short usID, float fValue );
    void                        Remove                      ( unsigned short usID, float fValue );
    void                        Clear                       ( void );

    int                         GetSize                     ( void )                        { return m_List.size ( ); }

private:
    map < unsigned short, sPlayerStat* >     m_List;
};

#endif
