/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CDestroySatchelsPacket.cpp
 *  PURPOSE:     Satchel destruction packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDestroySatchelsPacket.h"
#include "CElement.h"

CDestroySatchelsPacket::CDestroySatchelsPacket() noexcept {}

bool CDestroySatchelsPacket::Read(NetBitStreamInterface& BitStream) noexcept
{
    return true;
}

bool CDestroySatchelsPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    // Write the source player.
    if (!m_pSourceElement)
        return false;

    BitStream.Write(m_pSourceElement->GetID());
    return true;
}
