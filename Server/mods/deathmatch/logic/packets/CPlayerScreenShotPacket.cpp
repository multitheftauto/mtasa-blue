/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerScreenShotPacket.cpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerScreenShotPacket.h"
#include "Enums.h"
#include "CGame.h"
#include "CResourceManager.h"

bool CPlayerScreenShotPacket::Read(NetBitStreamInterface& BitStream)
{
    m_pResource = NULL;

    CPlayer* pPlayer = GetSourcePlayer();
    if (!pPlayer)
        return false;

    bool bHasGrabTime = false;
    uint uiServerGrabTime;

    // Read status
    BitStream.Read(m_ucStatus);

    if (m_ucStatus != EPlayerScreenShotResult::SUCCESS)
    {
        // minimized, disabled or error
        bHasGrabTime = true;
        BitStream.Read(uiServerGrabTime);
        if (BitStream.Version() >= 0x053)
        {
            ushort usResourceNetId;
            BitStream.Read(usResourceNetId);
            m_pResource = g_pGame->GetResourceManager()->GetResourceFromNetID(usResourceNetId);
        }
        else
        {
            SString strResourceName;
            BitStream.ReadString(strResourceName);
            m_pResource = g_pGame->GetResourceManager()->GetResource(strResourceName);
        }

        if (!BitStream.ReadString(m_strTag))
            return false;

        if (BitStream.Version() >= 0x53)
            BitStream.ReadString(m_strError);
    }
    else if (m_ucStatus == EPlayerScreenShotResult::SUCCESS)
    {
        // Read info
        BitStream.Read(m_usScreenShotId);
        BitStream.Read(m_usPartNumber);

        // Read data
        ushort usNumBytes = 0;
        if (!BitStream.Read(usNumBytes))
            return false;

        m_buffer.SetSize(usNumBytes);
        if (!BitStream.Read(m_buffer.GetData(), m_buffer.GetSize()))
            return false;

        // Read more info if first part
        if (m_usPartNumber == 0)
        {
            bHasGrabTime = true;
            BitStream.Read(uiServerGrabTime);
            BitStream.Read(m_uiTotalBytes);
            BitStream.Read(m_usTotalParts);
            if (BitStream.Version() >= 0x053)
            {
                ushort usResourceNetId;
                BitStream.Read(usResourceNetId);
                m_pResource = g_pGame->GetResourceManager()->GetResourceFromNetID(usResourceNetId);
            }
            else
            {
                SString strResourceName;
                BitStream.ReadString(strResourceName);
                m_pResource = g_pGame->GetResourceManager()->GetResource(strResourceName);
            }
            if (!BitStream.ReadString(m_strTag))
                return false;
        }
    }

    // Fixup grab time
    if (bHasGrabTime)
    {
        uiServerGrabTime += pPlayer->GetPing() / 2;
        uint uiTimeSinceGrab = GetTickCount32() - uiServerGrabTime;
        m_llServerGrabTime = GetTickCount64_() - uiTimeSinceGrab;
    }

    return true;
}
