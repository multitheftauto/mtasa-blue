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
#include "CCustomWeapon.h"
#include "net/SyncStructures.h"

class CCustomWeaponBulletSyncPacket final : public CPacket
{
public:
    CCustomWeaponBulletSyncPacket() = default;
    explicit CCustomWeaponBulletSyncPacket(class CPlayer* player);

    ePacketID     GetPacketID() const { return PACKET_ID_WEAPON_BULLETSYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; };

    bool Read(NetBitStreamInterface& stream);
    bool Write(NetBitStreamInterface& stream) const;

    CPlayer*       GetWeaponOwner() { return m_weapon != NULL ? m_weapon->GetOwner() : NULL; };
    CCustomWeapon* GetWeapon() { return m_weapon; };
    CVector        GetStart() { return m_start.data.vecPosition; };
    CVector        GetEnd() { return m_end.data.vecPosition; };

    CCustomWeapon* m_weapon{};
    ElementID      m_weaponID{INVALID_ELEMENT_ID};
    SPositionSync  m_start{};
    SPositionSync  m_end{};
};
