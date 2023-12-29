/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CHealthPacket.cpp
 *  PURPOSE:     health packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CHealthPacket.h"
#include "CElement.h"

CHealthPacket::CHealthPacket()
{
}

bool CHealthPacket::Read(NetBitStreamInterface& BitStream)
{
    if (!BitStream.Read(m_ElementID))
        return false;
    if (!BitStream.Read(m_fNewHealth))
        return false;

    return true;
}

bool CHealthPacket::Write(NetBitStreamInterface& BitStream) const
{
    // Write the source element's ID, old health, and new health.
    if (m_pSourceElement)
    {
        BitStream.Write(m_pSourceElement->GetID());
        BitStream.Write(m_fNewHealth);
    }
    else
        return false;

    return true;
}
