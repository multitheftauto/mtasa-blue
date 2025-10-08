/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerStatsPacket.cpp
 *  PURPOSE:     Player statistics packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerStatsPacket.h"
#include "CElement.h"

bool CPlayerStatsPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (!m_pSourceElement)
        return false;

    // Write the source elements's ID
    BitStream.Write(m_pSourceElement->GetID());

    BitStream.WriteCompressed(static_cast<unsigned short>(m_map.size()));            // Write stat count
    for (auto&& [statID, value] : m_map)
    {
        BitStream.Write(statID);
        BitStream.Write(value);
    }

    return true;
}

void CPlayerStatsPacket::Add(unsigned short usID, float fValue)
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
