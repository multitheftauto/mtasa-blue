/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CProjectileSyncPacket.h
 *  PURPOSE:     Projectile synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>

class CProjectileSyncPacket final : public CPacket
{
public:
    CProjectileSyncPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_PROJECTILE; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    unsigned char  m_ucWeaponType;
    ElementID      m_OriginID;
    CVector        m_vecOrigin;
    float          m_fForce;
    bool           m_bHasTarget;
    ElementID      m_TargetID;
    CVector        m_vecTarget;
    CVector        m_vecRotation;
    CVector        m_vecMoveSpeed;
    unsigned short m_usModel;
};
