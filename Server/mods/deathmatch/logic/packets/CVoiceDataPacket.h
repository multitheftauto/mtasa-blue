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
    CVoiceDataPacket() noexcept;
    CVoiceDataPacket(CPlayer* pPlayer, const std::uint8_t* pbSrcBuffer, std::uint16_t usLength) noexcept;
    ~CVoiceDataPacket() noexcept;

    ePacketID               GetPacketID() const noexcept { return PACKET_ID_VOICE_DATA; }
    std::uint32_t           GetFlags() const noexcept { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_VOICE; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    void SetData(const std::uint8_t* pbSrcBuffer, std::uint16_t usLength) noexcept;

    std::uint16_t       GetDataLength() const noexcept;
    const std::uint8_t* GetData() const noexcept;

private:
    void AllocateBuffer(std::uint16_t usBufferSize) noexcept;
    void DeallocateBuffer() noexcept;

    std::uint8_t* m_pBuffer;
    std::uint16_t m_usDataBufferSize;
    std::uint16_t m_usActualDataLength;
};
