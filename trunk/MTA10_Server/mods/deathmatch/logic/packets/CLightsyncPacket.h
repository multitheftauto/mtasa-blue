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

#define LIGHTSYNC_HEALTH_THRESHOLD          2.0f
#define LIGHTSYNC_VEHICLE_HEALTH_THRESHOLD  50.0f

class CLightsyncPacket : public CPacket
{
public:
                                CLightsyncPacket            ( CPlayer* pPlayer );

    inline ePacketID            GetPacketID                 ( void ) const                  { return PACKET_ID_LIGHTSYNC; };
    inline unsigned long        GetFlags                    ( void ) const                  { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; };

    bool                        Read                        ( NetBitStreamInterface& BitStream );
    bool                        Write                       ( NetBitStreamInterface& BitStream ) const;

private:
    bool    m_bSyncHealth;
    bool    m_bSyncPosition;
    bool    m_bSyncVehicleHealth;
};

