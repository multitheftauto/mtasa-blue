/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerDisconnectedPacket.cpp
 *  PURPOSE:     Player disconnected packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerDisconnectedPacket.h"

CPlayerDisconnectedPacket::CPlayerDisconnectedPacket(const char* szReason) noexcept
{
    m_eType = CPlayerDisconnectedPacket::CUSTOM;
    m_strReason = szReason;
    m_Duration = 0;
}

CPlayerDisconnectedPacket::CPlayerDisconnectedPacket(ePlayerDisconnectType eType, const char* szReason) noexcept
{
    m_eType = eType;
    m_strReason = szReason;
    m_Duration = 0;
}

CPlayerDisconnectedPacket::CPlayerDisconnectedPacket(ePlayerDisconnectType eType, time_t BanDuration, const char* szReason) noexcept
{
    m_eType = eType;
    m_strReason = szReason;
    m_Duration = BanDuration;
}

bool CPlayerDisconnectedPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    BitStream.WriteBits(&m_eType, 5);

    if (m_eType == BAN || m_eType == BANNED_SERIAL || m_eType == BANNED_IP)
    {
        SString strDuration = SString("%llu", (long long)m_Duration);
        BitStream.WriteString(strDuration);
    }

    if (!m_strReason.empty())
        BitStream.WriteString(m_strReason);

    return true;
}
