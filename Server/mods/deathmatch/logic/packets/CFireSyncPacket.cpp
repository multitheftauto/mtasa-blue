/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CFireSyncPacket.cpp
 *  PURPOSE:     Fire synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CFireSyncPacket.h"
#include "CPlayer.h"

CFireSyncPacket::CFireSyncPacket()
{
    // Default size
    m_fSize = 1.8f;
}

CFireSyncPacket::CFireSyncPacket(const CVector& vecPosition, float fSize)
{
    m_vecPosition = vecPosition;
    m_fSize = fSize;
}

bool CFireSyncPacket::Read(NetBitStreamInterface& BitStream)
{
    return (BitStream.Read(m_vecPosition.fX) && BitStream.Read(m_vecPosition.fY) && BitStream.Read(m_vecPosition.fZ) && BitStream.Read(m_fSize));
}

bool CFireSyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    // Write the source player and latency if any. Otherwize 0
    if (m_pSourceElement)
    {
        ElementID ID = m_pSourceElement->GetID();
        BitStream.Write(ID);

        unsigned short usLatency = static_cast<CPlayer*>(m_pSourceElement)->GetPing();
        BitStream.WriteCompressed(usLatency);
    }
    else
    {
        BitStream.Write(static_cast<ElementID>(INVALID_ELEMENT_ID));
        BitStream.WriteCompressed(static_cast<unsigned short>(0));
    }

    // Write position and size
    BitStream.Write(m_vecPosition.fX);
    BitStream.Write(m_vecPosition.fY);
    BitStream.Write(m_vecPosition.fZ);
    BitStream.Write(m_fSize);

    return true;
}
