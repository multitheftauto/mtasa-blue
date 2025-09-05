/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <vector>
#include <cstddef>

class CEntityRemoveTreePacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const noexcept override { return PACKET_ID_ENTITY_REMOVE_TREE; }
    unsigned long GetFlags() const noexcept override { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& bitStream) const override;

    void AddRootElement(CElement* element) { m_rootElements.push_back(element); }
    void Clear() noexcept { m_rootElements.clear(); }
    bool IsEmpty() const noexcept { return m_rootElements.empty(); }
    std::size_t GetRootElementCount() const noexcept { return m_rootElements.size(); }

private:
    std::vector<CElement*> m_rootElements;
};
