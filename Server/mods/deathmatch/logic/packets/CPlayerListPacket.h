/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerListPacket.h
 *  PURPOSE:     Player list packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <list>

class CPlayerListPacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_LIST; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    void AddPlayer(CPlayer* pPlayer) noexcept { m_List.push_back(pPlayer); }
    void RemovePlayer(CPlayer* pPlayer) noexcept { m_List.remove(pPlayer); }
    void RemoveAllPlayers() noexcept { m_List.clear(); }

    bool GetShowInChat() const noexcept { return m_bShowInChat; }
    void SetShowInChat(bool bShowInChat) noexcept { m_bShowInChat = bShowInChat; }

private:
    std::list<CPlayer*> m_List;
    bool                m_bShowInChat;
};
