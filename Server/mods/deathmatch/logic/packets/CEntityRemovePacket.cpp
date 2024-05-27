/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CEntityRemovePacket.cpp
 *  PURPOSE:     Entity remove packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEntityRemovePacket.h"
#include "CElement.h"

bool CEntityRemovePacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    // Write each entity type then id to it
    for (const auto& pElem : m_List)
    {
        BitStream.Write(pElem->GetID());
    }

    return m_List.size() > 0;
}
