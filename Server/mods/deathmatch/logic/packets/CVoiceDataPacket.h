/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVoiceDataPacket.h
 *  PURPOSE:     Voice data packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CVoiceDataPacket final : public CPacket
{
public:
    CVoiceDataPacket(CPlayer* pPlayer, const unsigned char* pbSrcBuffer, unsigned short usLength);
    CVoiceDataPacket();
    ~CVoiceDataPacket();

    ePacketID               GetPacketID() const { return PACKET_ID_VOICE_DATA; }
    unsigned long           GetFlags() const { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; };
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_VOICE; }

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    void SetData(const unsigned char* pbSrcBuffer, unsigned short usLength);

    unsigned short       GetDataLength() const;
    const unsigned char* GetData() const;

private:
    void AllocateBuffer(unsigned short usBufferSize);
    void DeallocateBuffer();

    unsigned char* m_pBuffer;
    unsigned short m_usDataBufferSize;
    unsigned short m_usActualDataLength;
};
