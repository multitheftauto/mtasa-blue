/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerStatsPacket.cpp
 *  PURPOSE:     Player statistics packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerStatsPacket.h"
#include "CElement.h"

bool CPlayerStatsPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    if (!m_pSourceElement)
        return false;

    // Write the source elements's ID
    BitStream.Write(m_pSourceElement->GetID());

    // Write stat count
    BitStream.WriteCompressed(static_cast<std::uint16_t>(m_map.size()));
    for (auto&& [statID, value] : m_map)
    {
        BitStream.Write(statID);
        BitStream.Write(value);
    }

    return true;
}

void CPlayerStatsPacket::Add(const std::uint16_t usID, const float fValue) noexcept
{
    if (auto iter = m_map.find(usID); iter != m_map.end())
    {
        if (fValue == 0.0f)
            m_map.erase(iter);            // Erase stat
        else
            iter->second = fValue;            // Update value
    }
    else            // Not in map
        m_map.emplace(usID, fValue);
}
