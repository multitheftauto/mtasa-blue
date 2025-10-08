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

CVoiceDataPacket::CVoiceDataPacket()
{
    m_voiceBuffer.reserve(2048);
}

bool CVoiceDataPacket::Read(NetBitStreamInterface& BitStream)
{
    unsigned short voiceBufferLength{};

    if (BitStream.Read(voiceBufferLength) && voiceBufferLength <= m_voiceBuffer.capacity())
    {
        m_voiceBuffer.resize(voiceBufferLength);
        return BitStream.Read(reinterpret_cast<char*>(m_voiceBuffer.data()), m_voiceBuffer.size());
    }

    return false;
}

bool CVoiceDataPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (!m_voiceBuffer.empty())
    {
        const auto voiceBuffer = reinterpret_cast<const char*>(m_voiceBuffer.data());
        const auto voiceBufferLength = static_cast<uint16_t>(m_voiceBuffer.size());

        // Write the source player
        BitStream.Write(m_pSourceElement->GetID());
        // Write the length as an unsigned short and then write the string
        BitStream.Write(voiceBufferLength);
        BitStream.Write(voiceBuffer, voiceBufferLength);
        return true;
    }

    return false;
}

void CVoiceDataPacket::SetVoiceData(const unsigned char* voiceBuffer, unsigned short voiceBufferLength)
{
    m_voiceBuffer.clear();

    if (!voiceBuffer || !voiceBufferLength || voiceBufferLength > m_voiceBuffer.capacity())
        return;

    m_voiceBuffer.resize(voiceBufferLength);
    std::copy_n(voiceBuffer, voiceBufferLength, m_voiceBuffer.data());
}
