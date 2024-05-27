/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.3
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CResourceClientScriptsPacket.cpp
 *  PURPOSE:     Resource client-side scripts packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CResourceClientScriptsPacket.h"

CResourceClientScriptsPacket::CResourceClientScriptsPacket(CResource* pResource) noexcept
    : m_pResource(pResource) {}

void CResourceClientScriptsPacket::AddItem(CResourceClientScriptItem* pItem) noexcept
{
    m_vecItems.push_back(pItem);
}

bool CResourceClientScriptsPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    if (m_vecItems.empty())
        return false;

    BitStream.Write(m_pResource->GetNetID());

    auto usItemCount = static_cast<std::uint16_t>(m_vecItems.size());
    BitStream.Write(usItemCount);

    for (const auto& pItem : m_vecItems)
    {
        if (BitStream.Version() >= 0x50)
            BitStream.WriteString(ConformResourcePath(pItem->GetFullName()));

        const auto& data = pItem->GetSourceCode();
        std::uint32_t len = data.length();
        BitStream.Write(len);
        BitStream.Write(data.c_str(), len);
    }

    return true;
}
