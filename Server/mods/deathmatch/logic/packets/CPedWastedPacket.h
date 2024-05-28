/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPedWastedPacket.h
 *  PURPOSE:     Ped wasted state packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>
#include <SharedUtil.IntTypes.h>

class CPed;

class CPedWastedPacket final : public CPacket
{
public:
    CPedWastedPacket() noexcept;
    CPedWastedPacket(CPed* pPed, CElement* pKiller,
        std::uint8_t ucKillerWeapon, std::uint8_t ucBodyPart, bool bStealth,
        AssocGroupId animGroup = 0, AnimationId animID = 15) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PED_WASTED; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    ElementID     m_PedID;
    ElementID     m_Killer;
    std::uint8_t  m_ucKillerWeapon;
    std::uint8_t  m_ucBodyPart;
    CVector       m_vecPosition;
    std::uint16_t m_usAmmo;
    bool          m_bStealth;
    std::uint8_t  m_ucTimeContext;
    std::uint32_t m_AnimGroup;
    std::uint32_t m_AnimID;
};
