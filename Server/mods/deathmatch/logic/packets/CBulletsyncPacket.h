/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "CCommon.h"

class CBulletsyncPacket final : public CPacket
{
public:
    CBulletsyncPacket() = default;
    explicit CBulletsyncPacket(class CPlayer* player);

    bool          HasSimHandler() const { return true; }
    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_BULLETSYNC; }
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; }

    bool Read(NetBitStreamInterface& stream);
    bool Write(NetBitStreamInterface& stream) const;

    eWeaponType  m_weapon{};
    CVector      m_start{};
    CVector      m_end{};
    std::uint8_t m_order{};
    float        m_damage{};
    std::uint8_t m_zone{};
    ElementID    m_damaged{INVALID_ELEMENT_ID};
};
