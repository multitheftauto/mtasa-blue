/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVoiceDataPacket.h
 *  PURPOSE:     Voice end packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CVoiceEndPacket final : public CPacket
{
public:
    CVoiceEndPacket(class CPlayer* pPlayer = nullptr) noexcept;
    ~CVoiceEndPacket() noexcept;

    ePacketID               GetPacketID() const noexcept { return PACKET_ID_VOICE_END; }
    std::uint32_t           GetFlags() const noexcept { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_VOICE; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    ElementID GetPlayer() const noexcept;
    void      SetPlayer(ElementID PlayerID) noexcept;

private:
    ElementID m_PlayerID;
};
