/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CEntityRemoveTreePacket.cpp
 *  PURPOSE:     Entity tree remove packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEntityRemoveTreePacket.h"
#include "CElement.h"

bool CEntityRemoveTreePacket::Write(NetBitStreamInterface& bitStream) const
{
    if (m_rootElements.empty())
        return false;

    bitStream.WriteCompressed(static_cast<unsigned short>(m_rootElements.size()));

    for (const auto* element : m_rootElements)
    {
        bitStream.Write(element->GetID());
    }

    return true;
}
