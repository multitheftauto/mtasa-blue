/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CBulletsyncPacket.h
 *  PURPOSE:     Bullet synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef __CBULLETSYNCPACKET_H
#define __CBULLETSYNCPACKET_H

#pragma once

#include "CPacket.h"
#include "BulletSyncData.h"

class CBulletsyncPacket final : public CPacket
{
public:
    CBulletsyncPacket() = default;
    explicit CBulletsyncPacket(class CPlayer* player);

    bool          HasSimHandler() const noexcept override { return true; }
    ePacketID     GetPacketID() const noexcept override { return PACKET_ID_PLAYER_BULLETSYNC; }
    unsigned long GetFlags() const noexcept override { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; }

    bool Read(NetBitStreamInterface& stream) override;
    bool Write(NetBitStreamInterface& stream) const override;

    BulletSyncData m_data;
};

#endif  // __CBULLETSYNCPACKET_H
