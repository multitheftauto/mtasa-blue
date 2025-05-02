/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerJoinCompletePacket.cpp
 *  PURPOSE:     Player join completion packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerJoinCompletePacket.h"
#include "CGame.h"
#include "CMainConfig.h"
#include <net/SyncStructures.h>

CPlayerJoinCompletePacket::CPlayerJoinCompletePacket()
{
    m_PlayerID = INVALID_ELEMENT_ID;
    m_RootElementID = INVALID_ELEMENT_ID;
    m_ucHTTPDownloadType = HTTP_DOWNLOAD_DISABLED;
    m_usHTTPDownloadPort = 0;
    m_iHTTPMaxConnectionsPerClient = 4;
    m_iEnableClientChecks = 0;
    m_bVoiceEnabled = true;
    m_ucSampleRate = 1;
    m_ucQuality = 4;
    m_uiBitrate = 0;
    m_szServerName = "";
}

CPlayerJoinCompletePacket::CPlayerJoinCompletePacket(ElementID PlayerID, ElementID RootElementID, eHTTPDownloadType ucHTTPDownloadType,
                                                     unsigned short usHTTPDownloadPort, const char* szHTTPDownloadURL, int iHTTPMaxConnectionsPerClient,
                                                     int iEnableClientChecks, bool bVoiceEnabled, unsigned char ucSampleRate, unsigned char ucVoiceQuality,
                                                     unsigned int uiBitrate, const char* szServerName)
{
    m_PlayerID = PlayerID;
    m_RootElementID = RootElementID;
    m_ucHTTPDownloadType = ucHTTPDownloadType;
    m_iHTTPMaxConnectionsPerClient = iHTTPMaxConnectionsPerClient;
    m_iEnableClientChecks = iEnableClientChecks;
    m_bVoiceEnabled = bVoiceEnabled;
    m_ucSampleRate = ucSampleRate;
    m_ucQuality = ucVoiceQuality;
    m_uiBitrate = uiBitrate;
    m_szServerName = szServerName;

    switch (m_ucHTTPDownloadType)
    {
        case HTTP_DOWNLOAD_ENABLED_PORT:
            m_usHTTPDownloadPort = usHTTPDownloadPort;
            break;
        case HTTP_DOWNLOAD_ENABLED_URL:
            m_usHTTPDownloadPort = usHTTPDownloadPort;
            m_strHTTPDownloadURL.AssignLeft(szHTTPDownloadURL, MAX_HTTP_DOWNLOAD_URL);
            break;
        default:
            break;
    }
}

bool CPlayerJoinCompletePacket::Write(NetBitStreamInterface& BitStream) const
{
    BitStream.Write(m_PlayerID);

    // For protocol backwards compatibility: write a non-zero single byte value.
    // This used to hold the number of players, it was never used on the client side,
    // and it caused protocol error 14 whenever the value wrapped back to zero (uint32_t -> uint8_t).
    uint8_t numPlayers = 1;
    BitStream.Write(numPlayers);

    BitStream.Write(m_RootElementID);

    // Transmit server requirement for the client to check settings
    BitStream.Write(m_iEnableClientChecks);

    // Transmit whether or not the Voice is enabled
    BitStream.WriteBit(m_bVoiceEnabled);

    // Transmit the sample rate for voice
    SIntegerSync<unsigned char, 2> sampleRate(m_ucSampleRate);
    BitStream.Write(&sampleRate);

    // Transmit the quality for voice
    SIntegerSync<unsigned char, 4> voiceQuality(m_ucQuality);
    BitStream.Write(&voiceQuality);

    // Transmit the max bitrate for voice
    BitStream.WriteCompressed(m_uiBitrate);

    // fakelag command enabled
    if (BitStream.Can(eBitStreamVersion::FakeLagCommand))
    {
        BitStream.WriteBit(g_pGame->GetConfig()->IsFakeLagCommandEnabled());
    }

    // Tellclient about maybe throttling back http client requests
    BitStream.Write(m_iHTTPMaxConnectionsPerClient);

    BitStream.Write(static_cast<unsigned char>(m_ucHTTPDownloadType));

    switch (m_ucHTTPDownloadType)
    {
        case HTTP_DOWNLOAD_ENABLED_PORT:
        {
            BitStream.Write(m_usHTTPDownloadPort);
        }

        break;
        case HTTP_DOWNLOAD_ENABLED_URL:
        {
            // Internal http server port
            BitStream.Write(m_usHTTPDownloadPort);

            // External http server URL
            BitStream.WriteString(m_strHTTPDownloadURL);
        }

        break;
        default:
            break;
    }

    if (BitStream.Can(eBitStreamVersion::CPlayerJoinCompletePacket_ServerName))
        BitStream.WriteString(m_szServerName);

    return true;
}
