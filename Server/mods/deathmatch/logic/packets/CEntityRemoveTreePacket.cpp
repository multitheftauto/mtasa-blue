/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEntityRemoveTreePacket.h"
#include "CElement.h"
#include <cstdint>

bool CEntityRemoveTreePacket::Write(NetBitStreamInterface& bitStream) const
{
    if (m_rootElements.empty())
        return false;

    bitStream.WriteCompressed(static_cast<std::uint16_t>(m_rootElements.size()));

    for (const auto* element : m_rootElements)
    {
        bitStream.Write(element->GetID());
    }

    return true;
}
