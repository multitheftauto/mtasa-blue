/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CDamageCancelEventPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CCommon.h>

class CDamageCancelEventPacket final : public CPacket
{
public:
    CDamageCancelEventPacket() noexcept {}

    ePacketID               GetPacketID() const noexcept { return PACKET_ID_CANCEL_DAMAGE_EVENT; }
    unsigned long           GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_DEFAULT; }

    bool Read(NetBitStreamInterface& bitStream) noexcept;

    ElementID   GetDamagedEntityID() const noexcept { return m_damagedEntityID; }
    ElementID   GetAtackerEntityID() const noexcept { return m_atackerEntityID; }
    eWeaponType GetWeaponType() const noexcept { return m_weaponType; }
    float       GetDamage() const noexcept { return m_damage; }
    std::string GetResourceName() const noexcept { return m_resourceName; }

private:
    std::string m_resourceName{};
    ElementID   m_damagedEntityID;
    ElementID   m_atackerEntityID{INVALID_ELEMENT_ID};
    float       m_damage;
    eWeaponType m_weaponType;
};
