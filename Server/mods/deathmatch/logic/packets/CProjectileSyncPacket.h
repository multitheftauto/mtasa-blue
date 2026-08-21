/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CProjectileSyncPacket.h
 *  PURPOSE:     Projectile synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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

    // Only ever set on a satchel resync (CGame::Packet_ProjectileRestPosition) when it was stuck to m_OriginID -
    // GTA's own native attach offset on that entity (e.g. the hood, not its centre), so the receiving client can
    // re-attach it at the exact same spot instead of the entity's origin (https://github.com/multitheftauto/mtasa-blue/issues/369, #368)
    bool    m_bHasAttachOffset;
    CVector m_vecAttachOffsetPosition;
    CVector m_vecAttachOffsetRotation;
};
