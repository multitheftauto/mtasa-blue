/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CSyncSettingsPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "CCommon.h"

class CSyncSettingsPacket final : public CPacket
{
public:
    CSyncSettingsPacket(){};
    CSyncSettingsPacket(const std::set<eWeaponType>& weaponTypesUsingBulletSync, uchar ucVehExtrapolateEnabled, short sVehExtrapolateBaseMs,
                        short sVehExtrapolatePercent, short sVehExtrapolateMaxMs, uchar ucUseAltPulseOrder, uchar ucAllowFastSprintFix,
                        uchar ucAllowDrivebyAnimationFix, uchar ucAllowShotgunDamageFix);

    ePacketID     GetPacketID() const { return PACKET_ID_SYNC_SETTINGS; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    std::set<eWeaponType> m_weaponTypesUsingBulletSync;
    uchar                 m_ucVehExtrapolateEnabled;
    short                 m_sVehExtrapolateBaseMs;
    short                 m_sVehExtrapolatePercent;
    short                 m_sVehExtrapolateMaxMs;
    uchar                 m_ucUseAltPulseOrder;
    uchar                 m_ucAllowFastSprintFix;
    uchar                 m_ucAllowDrivebyAnimationFix;
    uchar                 m_ucAllowShotgunDamageFix;
};
