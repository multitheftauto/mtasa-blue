/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CProjectileRestPositionPacket.cpp
 *  PURPOSE:     Projectile final resting position packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProjectileRestPositionPacket.h"

CProjectileRestPositionPacket::CProjectileRestPositionPacket()
{
    m_ucWeaponType = 0;
    m_AttachedToID = INVALID_ELEMENT_ID;
}

bool CProjectileRestPositionPacket::Read(NetBitStreamInterface& BitStream)
{
    if (!BitStream.Read(m_ucWeaponType))
        return false;

    if (!BitStream.Read(m_vecOrigin.fX) || !BitStream.Read(m_vecOrigin.fY) || !BitStream.Read(m_vecOrigin.fZ))
        return false;

    if (!BitStream.Read(m_vecRestPosition.fX) || !BitStream.Read(m_vecRestPosition.fY) || !BitStream.Read(m_vecRestPosition.fZ))
        return false;

    bool bHasAttachedTo;
    if (!BitStream.ReadBit(bHasAttachedTo))
        return false;

    m_AttachedToID = INVALID_ELEMENT_ID;
    if (bHasAttachedTo)
    {
        if (!BitStream.Read(m_AttachedToID))
            return false;

        if (!BitStream.Read(m_vecAttachOffsetPosition.fX) || !BitStream.Read(m_vecAttachOffsetPosition.fY) || !BitStream.Read(m_vecAttachOffsetPosition.fZ))
            return false;

        if (!BitStream.Read(m_vecAttachOffsetRotation.fX) || !BitStream.Read(m_vecAttachOffsetRotation.fY) || !BitStream.Read(m_vecAttachOffsetRotation.fZ))
            return false;
    }

    return true;
}

bool CProjectileRestPositionPacket::Write(NetBitStreamInterface& BitStream) const
{
    BitStream.Write(m_ucWeaponType);

    BitStream.Write(m_vecOrigin.fX);
    BitStream.Write(m_vecOrigin.fY);
    BitStream.Write(m_vecOrigin.fZ);

    BitStream.Write(m_vecRestPosition.fX);
    BitStream.Write(m_vecRestPosition.fY);
    BitStream.Write(m_vecRestPosition.fZ);

    if (m_AttachedToID != INVALID_ELEMENT_ID)
    {
        BitStream.WriteBit(true);
        BitStream.Write(m_AttachedToID);

        BitStream.Write(m_vecAttachOffsetPosition.fX);
        BitStream.Write(m_vecAttachOffsetPosition.fY);
        BitStream.Write(m_vecAttachOffsetPosition.fZ);

        BitStream.Write(m_vecAttachOffsetRotation.fX);
        BitStream.Write(m_vecAttachOffsetRotation.fY);
        BitStream.Write(m_vecAttachOffsetRotation.fZ);
    }
    else
        BitStream.WriteBit(false);

    return true;
}
