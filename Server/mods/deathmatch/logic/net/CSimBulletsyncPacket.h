/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "packets/BulletSyncData.h"

class CSimBulletsyncPacket : public CSimPacket
{
public:
    CSimBulletsyncPacket() = default;
    explicit CSimBulletsyncPacket(ElementID id);

    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_BULLETSYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; };

    bool Read(NetBitStreamInterface& stream);
    bool Write(NetBitStreamInterface& stream) const;

    const ElementID m_id{};
    BulletSyncData  m_cache;
};
