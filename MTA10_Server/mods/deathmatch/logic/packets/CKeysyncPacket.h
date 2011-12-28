/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CKeysyncPacket.h
*  PURPOSE:     Key controls synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Chris McArthur <>
*               Jax <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CKEYSYNCPACKET_H
#define __PACKETS_CKEYSYNCPACKET_H

#include "../CCommon.h"
#include "CPacket.h"

class CKeysyncPacket : public CPacket
{
public:
    inline                                  CKeysyncPacket              ( void )                        {};
                                            CKeysyncPacket              ( class CPlayer* pPlayer );

    inline ePacketID                        GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_KEYSYNC; };
    inline NetServerPacketOrdering          GetPacketOrdering           ( void ) const                  { return PACKET_ORDERING_OTHERSYNC; }
    unsigned long                           GetFlags                    ( void ) const                  { return PACKET_SEQUENCED; };

    bool                                    Read                        ( NetBitStreamInterface& BitStream );
    bool                                    Write                       ( NetBitStreamInterface& BitStream ) const;

private:
    void                                    ReadVehicleSpecific         ( class CVehicle* pVehicle, NetBitStreamInterface& BitStream );
    void                                    WriteVehicleSpecific        ( class CVehicle* pVehicle, NetBitStreamInterface& BitStream ) const;
};

#endif
