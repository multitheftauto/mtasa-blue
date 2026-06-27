/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CProjectileRestPositionPacket.h
 *  PURPOSE:     Projectile final resting position packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>

// Sent by the owning client once a persistent projectile (currently just satchel charges) has settled, so the
// server can resync late-joining/streaming-in players with the actual resting spot instead of replaying the
// original throw (https://github.com/multitheftauto/mtasa-blue/issues/369, #368).
class CProjectileRestPositionPacket final : public CPacket
{
public:
    CProjectileRestPositionPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_PROJECTILE_REST_POSITION; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    unsigned char m_ucWeaponType;
    CVector       m_vecOrigin;             // Matches the original throw's m_vecOrigin, used to find the tracked entry
    CVector       m_vecRestPosition;
};
