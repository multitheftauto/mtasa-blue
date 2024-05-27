/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CServerInfoSyncPacket.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CServerInfoSyncPacket.h"
#include "CStaticFunctionDefinitions.h"

bool CServerInfoSyncPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    // Flag is set
    if (!m_ActualInfo)
        return false;

    BitStream.Write(m_ActualInfo);

    // Check the flags one by one & write in order
    if (maxPlayers)
    {
        BitStream.Write(CStaticFunctionDefinitions::GetMaxPlayers());
    }

    return true;
}
