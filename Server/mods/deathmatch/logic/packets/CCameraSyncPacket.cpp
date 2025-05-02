/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CCameraSyncPacket.cpp
 *  PURPOSE:     Camera synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCameraSyncPacket.h"
#include "CPlayerCamera.h"
#include "CPlayer.h"
#include <net/SyncStructures.h>

bool CCameraSyncPacket::Read(NetBitStreamInterface& BitStream)
{
    if (BitStream.Version() >= 0x5E)
    {
        CPlayer* pPlayer = GetSourcePlayer();
        if (!pPlayer)
            return false;

        // Check the time context
        uchar ucTimeContext = 0;
        BitStream.Read(ucTimeContext);
        if (!pPlayer->GetCamera()->CanUpdateSync(ucTimeContext))
        {
            return false;
        }
    }

    if (!BitStream.ReadBit(m_bFixed))
        return false;

    if (m_bFixed)
    {
        SPositionSync position(false);
        if (!BitStream.Read(&position))
            return false;
        m_vecPosition = position.data.vecPosition;

        SPositionSync lookAt(false);
        if (!BitStream.Read(&lookAt))
            return false;
        m_vecLookAt = lookAt.data.vecPosition;
        return true;
    }
    else
    {
        return BitStream.Read(m_TargetID);
    }
}
