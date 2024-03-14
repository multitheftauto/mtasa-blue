/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVoiceDataPacket.h
 *  PURPOSE:     Voice data packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CVoiceDataPacket final : public CPacket
{
public:
    CVoiceDataPacket(CPlayer* pPlayer, const std::uint8_t* pbSrcBuffer, std::uint16_t usLength);
    CVoiceDataPacket();
    ~CVoiceDataPacket();

    ePacketID               GetPacketID() const { return PACKET_ID_VOICE_DATA; }
    unsigned long           GetFlags() const { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; };
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_VOICE; }

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    void SetData(const std::uint8_t* pbSrcBuffer, std::uint16_t usLength);

    std::uint16_t       GetDataLength() const;
    const std::uint8_t* GetData() const;

private:
    void AllocateBuffer(std::uint16_t usBufferSize);
    void DeallocateBuffer();

    std::uint8_t* m_pBuffer;
    std::uint16_t m_usDataBufferSize;
    std::uint16_t m_usActualDataLength;
};
