/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CSyncSettingsPacket.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "CCommon.h"

class CSyncSettingsPacket final : public CPacket
{
public:
    CSyncSettingsPacket() noexcept {}
    CSyncSettingsPacket(const std::set<eWeaponType>& weaponTypesUsingBulletSync,
        std::uint8_t ucVehExtrapolateEnabled, std::int16_t sVehExtrapolateBaseMs,
        std::int16_t sVehExtrapolatePercent, std::int16_t sVehExtrapolateMaxMs,
        std::uint8_t ucUseAltPulseOrder, std::uint8_t ucAllowFastSprintFix,
        std::uint8_t ucAllowDrivebyAnimationFix, std::uint8_t ucAllowShotgunDamageFix
    ) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_SYNC_SETTINGS; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    std::set<eWeaponType> m_weaponTypesUsingBulletSync;
    std::uint8_t          m_ucVehExtrapolateEnabled{0};
    std::int16_t          m_sVehExtrapolateBaseMs{0};
    std::int16_t          m_sVehExtrapolatePercent{0};
    std::int16_t          m_sVehExtrapolateMaxMs{0};
    std::uint8_t          m_ucUseAltPulseOrder{0};
    std::uint8_t          m_ucAllowFastSprintFix{0};
    std::uint8_t          m_ucAllowDrivebyAnimationFix{0};
    std::uint8_t          m_ucAllowShotgunDamageFix{0};
};
