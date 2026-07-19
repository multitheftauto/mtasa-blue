/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVoiceDataPacket.cpp
 *  PURPOSE:     Voice data packet class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVoiceDataPacket.h"
#include "CPlayer.h"

bool CVoiceDataPacket::Read(NetBitStreamInterface& BitStream)
{
    if (!m_pSourceElement)
        return false;

    auto player = GetSourcePlayer();

    auto now = GetTickCount64_(), last = player->GetLastSendVoiceData();
    if (now - last < 100)
        return false;

    player->SetLastSendVoiceData(now);

    unsigned short voiceBufferLength{};
    if (!BitStream.Read(voiceBufferLength))
        return false;
    
    if (voiceBufferLength > MAX_VOICE_BUFFER)
        return false;

    m_voiceBuffer.resize(voiceBufferLength);
    return BitStream.Read(reinterpret_cast<char*>(m_voiceBuffer.data()), m_voiceBuffer.size());
}

bool CVoiceDataPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (m_voiceBuffer.empty())
        return false;

    const auto voiceBuffer = reinterpret_cast<const char*>(m_voiceBuffer.data());
    const auto voiceBufferLength = static_cast<uint16_t>(m_voiceBuffer.size());

    // Write the source player
    BitStream.Write(m_pSourceElement->GetID());

    // Write the length as an unsigned short and then write the string
    BitStream.Write(voiceBufferLength);
    BitStream.Write(voiceBuffer, voiceBufferLength);

    return true;
}
