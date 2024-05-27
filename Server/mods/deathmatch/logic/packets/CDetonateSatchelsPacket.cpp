/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CDetonateSatchelsPacket.cpp
 *  PURPOSE:     Satchel detonation packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDetonateSatchelsPacket.h"
#include "CPlayer.h"

CDetonateSatchelsPacket::CDetonateSatchelsPacket() noexcept
{
}

bool CDetonateSatchelsPacket::Read(NetBitStreamInterface& BitStream) noexcept
{
    return true;
}

bool CDetonateSatchelsPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    // Write the source player and latency if any.
    if (!m_pSourceElement)
        return false;

    BitStream.Write(m_pSourceElement->GetID());

    std::uint16_t usLatency = static_cast<CPlayer*>(m_pSourceElement)->GetPing();
    BitStream.WriteCompressed(usLatency);

    return true;
}
