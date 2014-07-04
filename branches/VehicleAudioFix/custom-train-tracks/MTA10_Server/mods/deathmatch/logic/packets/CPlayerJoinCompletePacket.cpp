/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerJoinCompletePacket.cpp
*  PURPOSE:     Player join completion packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerJoinCompletePacket::CPlayerJoinCompletePacket ( void )
{
    m_PlayerID = INVALID_ELEMENT_ID;
    m_ucNumberOfPlayers = 0;
    m_RootElementID = INVALID_ELEMENT_ID;
    m_ucHTTPDownloadType = HTTP_DOWNLOAD_DISABLED;
    m_usHTTPDownloadPort = 0;
    m_iHTTPMaxConnectionsPerClient = 4;
    m_iEnableClientChecks = 0;
    m_bVoiceEnabled = true;
    m_ucSampleRate = 1;
    m_ucQuality = 4;
    m_uiBitrate = 0;
}


CPlayerJoinCompletePacket::CPlayerJoinCompletePacket ( ElementID PlayerID, unsigned char ucNumberOfPlayers, ElementID RootElementID, eHTTPDownloadType ucHTTPDownloadType, unsigned short usHTTPDownloadPort, const char* szHTTPDownloadURL, int iHTTPMaxConnectionsPerClient, int iEnableClientChecks, bool bVoiceEnabled, unsigned char ucSampleRate, unsigned char ucVoiceQuality, unsigned int uiBitrate )
{
    m_PlayerID = PlayerID;
    m_ucNumberOfPlayers = ucNumberOfPlayers;
    m_RootElementID = RootElementID;
    m_ucHTTPDownloadType = ucHTTPDownloadType;
    m_iHTTPMaxConnectionsPerClient = iHTTPMaxConnectionsPerClient;
    m_iEnableClientChecks = iEnableClientChecks;
    m_bVoiceEnabled = bVoiceEnabled;
    m_ucSampleRate = ucSampleRate;
    m_ucQuality = ucVoiceQuality;
    m_uiBitrate = uiBitrate;

    switch ( m_ucHTTPDownloadType )
    {
    case HTTP_DOWNLOAD_ENABLED_PORT:
        m_usHTTPDownloadPort = usHTTPDownloadPort;
        break;
    case HTTP_DOWNLOAD_ENABLED_URL:
        m_usHTTPDownloadPort = usHTTPDownloadPort;
        m_strHTTPDownloadURL.AssignLeft ( szHTTPDownloadURL, MAX_HTTP_DOWNLOAD_URL );
        break;
    default:
        break;
    }
}


bool CPlayerJoinCompletePacket::Write ( NetBitStreamInterface& BitStream ) const
{
    BitStream.Write ( m_PlayerID );
    BitStream.Write ( m_ucNumberOfPlayers );
    BitStream.Write ( m_RootElementID );

    // Transmit server requirement for the client to check settings
    BitStream.Write ( m_iEnableClientChecks );

    // Transmit whether or not the Voice is enabled
    BitStream.WriteBit ( m_bVoiceEnabled );

    // Transmit the sample rate for voice
    SIntegerSync < unsigned char, 2 > sampleRate ( m_ucSampleRate );
    BitStream.Write ( &sampleRate );

    // Transmit the quality for voice
    SIntegerSync < unsigned char, 4 > voiceQuality ( m_ucQuality );
    BitStream.Write ( &voiceQuality );

    // Transmit the max bitrate for voice
    BitStream.WriteCompressed ( m_uiBitrate );

    // Tellclient about maybe throttling back http client requests
    BitStream.Write ( m_iHTTPMaxConnectionsPerClient );

    BitStream.Write ( static_cast < unsigned char > ( m_ucHTTPDownloadType ) );

    switch ( m_ucHTTPDownloadType )
    {
    case HTTP_DOWNLOAD_ENABLED_PORT:
        {
            BitStream.Write ( m_usHTTPDownloadPort );
        }

        break;
    case HTTP_DOWNLOAD_ENABLED_URL:
        {
            // Internal http server port
            if ( BitStream.Version() >= 0x48 )
                BitStream.Write( m_usHTTPDownloadPort );

            // External http server URL
            BitStream.WriteString ( m_strHTTPDownloadURL );
        }

        break;
    default:
        break;
    }

    return true;
}
