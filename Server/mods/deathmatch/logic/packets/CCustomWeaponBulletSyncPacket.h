/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CCustomWeaponBulletSyncPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "CCustomWeapon.h"

class CCustomWeaponBulletSyncPacket final : public CPacket
{
public:
    CCustomWeaponBulletSyncPacket(){};
    CCustomWeaponBulletSyncPacket(class CPlayer* pPlayer);

    ePacketID     GetPacketID() const { return PACKET_ID_WEAPON_BULLETSYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    CPlayer*       GetWeaponOwner() { return m_pWeapon != NULL ? m_pWeapon->GetOwner() : NULL; };
    CCustomWeapon* GetWeapon() { return m_pWeapon; };
    CVector        GetStart() { return m_vecStart; };
    CVector        GetEnd() { return m_vecEnd; };

    CCustomWeapon* m_pWeapon;
    CVector        m_vecStart;
    CVector        m_vecEnd;
    uchar          m_ucOrderCounter;
};
