/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CUpdateInfoPacket.cpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CUpdateInfoPacket.h"

CUpdateInfoPacket::CUpdateInfoPacket() noexcept
{
}

CUpdateInfoPacket::CUpdateInfoPacket(const SString& strType, const SString& strData) noexcept
{
    m_strType = strType;
    m_strData = strData;
}

bool CUpdateInfoPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    BitStream.Write((std::uint16_t)m_strType.length());
    BitStream.Write(m_strType.c_str(), m_strType.length());

    BitStream.Write((std::uint16_t)m_strData.length());
    BitStream.Write(m_strData.c_str(), m_strData.length());

    return true;
}
