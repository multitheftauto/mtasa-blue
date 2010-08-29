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
}


CPlayerJoinCompletePacket::CPlayerJoinCompletePacket ( ElementID PlayerID, unsigned char ucNumberOfPlayers, ElementID RootElementID, eHTTPDownloadType ucHTTPDownloadType, unsigned short usHTTPDownloadPort, const char* szHTTPDownloadURL, int iHTTPMaxConnectionsPerClient, int iEnableClientChecks )
{
    m_PlayerID = PlayerID;
    m_ucNumberOfPlayers = ucNumberOfPlayers;
    m_RootElementID = RootElementID;
    m_ucHTTPDownloadType = ucHTTPDownloadType;
    m_iHTTPMaxConnectionsPerClient = iHTTPMaxConnectionsPerClient;
    m_iEnableClientChecks = iEnableClientChecks;

    switch ( m_ucHTTPDownloadType )
    {
    case HTTP_DOWNLOAD_ENABLED_PORT:
        m_usHTTPDownloadPort = usHTTPDownloadPort;
        break;
    case HTTP_DOWNLOAD_ENABLED_URL:
        m_usHTTPDownloadPort = usHTTPDownloadPort;

        strncpy ( m_szHTTPDownloadURL, szHTTPDownloadURL, MAX_HTTP_DOWNLOAD_URL );
        m_szHTTPDownloadURL [MAX_HTTP_DOWNLOAD_URL] = 0;
        break;
    default:
        break;
    }
}


bool CPlayerJoinCompletePacket::Write ( NetBitStreamInterface& BitStream ) const
{
    BitStream.WriteCompressed ( m_PlayerID );
    BitStream.Write ( m_ucNumberOfPlayers );
    BitStream.WriteCompressed ( m_RootElementID );

    // Transmit server requirement for the client to check settings
    if ( BitStream.Version () >= 0x05 )
        BitStream.Write ( m_iEnableClientChecks );

    // Tell aware clients about maybe throttling back http client requests (doesn't work too well)
    if ( BitStream.Version () >= 0x04 )
        BitStream.Write ( Clamp ( 2, m_iHTTPMaxConnectionsPerClient, 32 ) );

    // Tell aware clients about maybe throttling back http client requests (works better)
    if ( BitStream.Version () >= 0x10 )
        BitStream.Write ( m_iHTTPMaxConnectionsPerClient );

    // Tell unaware clients to use the builtin web server if http flood protection is hinted
    unsigned char ucHTTPDownloadType = ( m_iHTTPMaxConnectionsPerClient < 4 && BitStream.Version () < 0x04 && m_ucHTTPDownloadType != HTTP_DOWNLOAD_DISABLED ) ? HTTP_DOWNLOAD_ENABLED_PORT : m_ucHTTPDownloadType;

    BitStream.Write ( static_cast < unsigned char > ( ucHTTPDownloadType ) );

    switch ( ucHTTPDownloadType )
    {
    case HTTP_DOWNLOAD_ENABLED_PORT:
        {
            BitStream.Write ( m_usHTTPDownloadPort );
        }

        break;
    case HTTP_DOWNLOAD_ENABLED_URL:
        {
            size_t sizeHTTPDownloadURL = strlen ( m_szHTTPDownloadURL );

            BitStream.Write ( static_cast < unsigned short > ( sizeHTTPDownloadURL ) );
            if ( sizeHTTPDownloadURL > 0 )
            {
                BitStream.Write ( const_cast < char* > ( m_szHTTPDownloadURL ), sizeHTTPDownloadURL );
            }
        }

        break;
    default:
        break;
    }

    return true;
}
