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

class CPlayerListPacket : public CPacket
{
public:
    ePacketID     GetPacketID(void) const { return PACKET_ID_PLAYER_LIST; };
    unsigned long GetFlags(void) const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

    void AddPlayer(CPlayer* pPlayer) { m_List.push_back(pPlayer); };
    void RemovePlayer(CPlayer* pPlayer) { m_List.remove(pPlayer); };
    void RemoveAllPlayers(void) { m_List.clear(); };

    bool GetShowInChat(void) { return m_bShowInChat; };
    void SetShowInChat(bool bShowInChat) { m_bShowInChat = bShowInChat; };

private:
    std::list<CPlayer*> m_List;
    bool                m_bShowInChat;
};
