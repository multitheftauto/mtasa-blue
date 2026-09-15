/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerACInfoPacket.cpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerACInfoPacket.h"

namespace
{
    constexpr uchar MAX_PLAYER_ACINFO_ID_COUNT = 64;
}

bool CPlayerACInfoPacket::Read(NetBitStreamInterface& BitStream)
{
    // Read type
    uchar ucNumItems = 0;
    if (!BitStream.Read(ucNumItems) || ucNumItems > MAX_PLAYER_ACINFO_ID_COUNT)
        return false;

    m_IdList.clear();
    bool bSeenIds[64] = {};
    for (uint i = 0; i < ucNumItems; i++)
    {
        uchar ucId;
        if (!BitStream.Read(ucId) || ucId > 63 || bSeenIds[ucId])
            return false;

        bSeenIds[ucId] = true;
        m_IdList.push_back(ucId);
    }

    if (!BitStream.Read(m_uiD3d9Size) || !BitStream.ReadString(m_strD3d9MD5) || !BitStream.ReadString(m_strD3d9SHA256))
        return false;

    return true;
}
