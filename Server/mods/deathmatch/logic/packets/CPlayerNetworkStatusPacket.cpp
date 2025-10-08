/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerNetworkStatusPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerNetworkStatusPacket.h"

bool CPlayerNetworkStatusPacket::Read(NetBitStreamInterface& BitStream)
{
    BitStream.Read(m_ucType);
    if (!BitStream.Read(m_uiTicks))
        return false;
    return true;
}
