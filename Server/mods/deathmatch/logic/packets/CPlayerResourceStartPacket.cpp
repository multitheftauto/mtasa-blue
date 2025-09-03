/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerResourceStartPacket.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerResourceStartPacket.h"
#include "CGame.h"
#include "CResourceManager.h"

bool CPlayerResourceStartPacket::Read(NetBitStreamInterface& BitStream)
{
    ushort usResourceNetId{};

    if (!BitStream.Read(usResourceNetId) || !BitStream.Read(m_startCounter))
        return false;

    m_pResource = g_pGame->GetResourceManager()->GetResourceFromNetID(usResourceNetId);
    return true;
}
