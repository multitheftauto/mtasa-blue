/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "packets/CBulletsyncPacket.h"

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

    struct
    {
        eWeaponType          weapon{eWeaponType::WEAPONTYPE_UNARMED};
        SPositionSync        start{};
        SPositionSync        end{};
        SFloatAsBitsSync<16> damage{0.0f, CBulletsyncPacket::MAX_DAMAGE, true, false};
        std::uint8_t         zone{};
        ElementID            damaged{INVALID_ELEMENT_ID};
    } m_cache;
};
