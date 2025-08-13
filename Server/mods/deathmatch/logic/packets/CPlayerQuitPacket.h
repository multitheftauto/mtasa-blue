/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerQuitPacket.h
 *  PURPOSE:     Player quit packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerQuitPacket final : public CPacket
{
public:
    CPlayerQuitPacket();

    ePacketID     GetPacketID() const { return static_cast<ePacketID>(PACKET_ID_PLAYER_QUIT); };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream) { return true; };
    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID GetPlayer() { return m_PlayerID; };
    void      SetPlayer(ElementID PlayerID) { m_PlayerID = PlayerID; };

    unsigned char GetQuitReason() { return m_ucQuitReason; }
    void          SetQuitReason(unsigned char ucQuitReason) { m_ucQuitReason = ucQuitReason; }

private:
    ElementID     m_PlayerID;
    unsigned char m_ucQuitReason;
};
