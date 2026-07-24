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
#include "CGame.h"
#include "CMainConfig.h"

bool CVoiceDataPacket::Read(NetBitStreamInterface& BitStream)
{
    CPlayer* pPlayer = GetSourcePlayer();
    if (!pPlayer)
        return false;

    unsigned short voiceBufferLength{};
    if (!BitStream.Read(voiceBufferLength) || voiceBufferLength == 0 || voiceBufferLength > MAX_VOICE_BUFFER_SIZE)
        return false;

    const auto*         mainConfig = g_pGame->GetConfig();
    const long long     now = GetTickCount64_();
    const bool          newInterval = pPlayer->GetLastVoiceDataTime() == 0 || now - pPlayer->GetLastVoiceDataTime() >= mainConfig->GetVoicePacketsInterval();
    const unsigned char packetsInInterval = newInterval ? 0 : pPlayer->GetVoiceDataPacketsInInterval();

    if (packetsInInterval >= mainConfig->GetMaxVoicePacketsPerInterval())
        return false;

    std::vector<unsigned char> voiceBuffer(voiceBufferLength);
    if (!BitStream.Read(reinterpret_cast<char*>(voiceBuffer.data()), voiceBuffer.size()))
        return false;

    m_voiceBuffer = std::move(voiceBuffer);
    if (newInterval)
    {
        pPlayer->SetLastVoiceDataTime(now);
        pPlayer->SetVoiceDataPacketsInInterval(1);
    }
    else
        pPlayer->IncrementVoiceDataPacketsInInterval();
    return true;
}

bool CVoiceDataPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (!m_pSourceElement || m_voiceBuffer.empty())
        return false;

    const auto voiceBuffer = reinterpret_cast<const char*>(m_voiceBuffer.data());
    const auto voiceBufferLength = static_cast<uint16_t>(m_voiceBuffer.size());

    BitStream.Write(m_pSourceElement->GetID());
    BitStream.Write(voiceBufferLength);
    BitStream.Write(voiceBuffer, voiceBufferLength);

    return true;
}
