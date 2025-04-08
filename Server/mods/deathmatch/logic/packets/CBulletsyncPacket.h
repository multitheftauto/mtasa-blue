/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CBulletsyncPacket.h
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
    CBulletsyncPacket(){};
    CBulletsyncPacket(class CPlayer* pPlayer);

    bool          HasSimHandler() const { return true; }
    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_BULLETSYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    eWeaponType m_WeaponType;
    CVector     m_vecStart;
    CVector     m_vecEnd;
    uchar       m_ucOrderCounter;
    float       m_fDamage;
    uchar       m_ucHitZone;
    ElementID   m_DamagedPlayerID;
};
