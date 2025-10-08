/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVoiceDataPacket.h
 *  PURPOSE:     Voice data packet class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <vector>

class CVoiceDataPacket final : public CPacket
{
public:
    CVoiceDataPacket();

    ePacketID               GetPacketID() const { return PACKET_ID_VOICE_DATA; }
    unsigned long           GetFlags() const { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; };
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_VOICE; }

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    void SetVoiceData(const unsigned char* voiceBuffer, unsigned short voiceBufferLength);

    bool IsEmpty() const noexcept { return m_voiceBuffer.empty(); }

private:
    std::vector<unsigned char> m_voiceBuffer;
};
