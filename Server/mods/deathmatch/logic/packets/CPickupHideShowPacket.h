/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPickupHideShowPacket.h
 *  PURPOSE:     Pickup hide/show packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <vector>

class CPickupHideShowPacket final : public CPacket
{
public:
    CPickupHideShowPacket(bool bShow) noexcept { m_bShow = bShow; }

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PICKUP_HIDESHOW; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    bool GetShow() const noexcept { return m_bShow; }
    void SetShow(bool bShow) noexcept { m_bShow = bShow; }

    void         Add(class CPickup* pPickup) noexcept { m_List.push_back(pPickup); }
    void         Clear() noexcept { m_List.clear(); }
    std::size_t  Count() const noexcept { return m_List.size(); }

private:
    bool                        m_bShow;
    std::vector<class CPickup*> m_List;
};
