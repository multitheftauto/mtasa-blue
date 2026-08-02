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
    ushort usResourceNetId;
    if (!BitStream.Read(usResourceNetId))
        return false;

    if (BitStream.Can(eBitStreamVersion::OnPlayerResourceStartGeneration))
    {
        if (!BitStream.Read(m_uiStartGeneration))
            return false;

        m_bHasStartGeneration = true;
    }
    else
    {
        m_uiStartGeneration = 0;
        m_bHasStartGeneration = false;
    }

    m_pResource = g_pGame->GetResourceManager()->GetResourceFromNetID(usResourceNetId);
    return true;
}
