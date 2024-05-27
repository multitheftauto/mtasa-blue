/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerACInfoPacket.cpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerACInfoPacket.h"

bool CPlayerACInfoPacket::Read(NetBitStreamInterface& BitStream) noexcept
{
    // Read type
    std::uint8_t ucNumItems = 0;
    BitStream.Read(ucNumItems);
    for (auto i = 0; i < ucNumItems; i++)
    {
        std::uint8_t ucId;
        if (!BitStream.Read(ucId))
            return false;
        m_IdList.push_back(ucId);
    }

    BitStream.Read(m_uiD3d9Size);
    BitStream.ReadString(m_strD3d9MD5);
    if (!BitStream.ReadString(m_strD3d9SHA256))
        return false;

    return true;
}
