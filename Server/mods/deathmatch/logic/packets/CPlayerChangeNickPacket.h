/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerChangeNickPacket.h
 *  PURPOSE:     Player nickname change packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerChangeNickPacket final : public CPacket
{
public:
    explicit CPlayerChangeNickPacket(const char* szNewNick);

    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_CHANGE_NICK; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

    const char* GetNewNick() { return m_strNewNick; }
    void        SetNewNick(const char* szNewNick) { m_strNewNick.AssignLeft(szNewNick, MAX_PLAYER_NICK_LENGTH); }

private:
    SString m_strNewNick;
};
