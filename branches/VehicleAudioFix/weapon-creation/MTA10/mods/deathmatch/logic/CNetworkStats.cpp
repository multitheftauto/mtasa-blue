/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CNetworkStats.cpp
*  PURPOSE:     Network stats logger
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CNetworkStats::CNetworkStats ( CClientDisplayManager* pDisplayManager )
{
    m_pDisplayManager = pDisplayManager;
    Reset ();
}

void CNetworkStats::Reset ( void )
{
    m_ulLastUpdateTime = CClientTime::GetTime ();

    NetStatistics stats;
    g_pNet->GetNetworkStatistics ( &stats );

    m_fPacketSendRate = 0;
    m_fPacketReceiveRate = 0;
    m_fByteSendRate = 0;
    m_fByteReceiveRate = 0;

    m_uiLastPacketsSent = stats.packetsSent;
    m_uiLastPacketsReceived = stats.packetsReceived;
    m_ullLastBytesSent = stats.bytesSent;
    m_ullLastBytesReceived = stats.bytesReceived;
}

void CNetworkStats::Draw ( void )
{
    // Time to update?
    if ( CClientTime::GetTime () >= m_ulLastUpdateTime + 1000 )
    {
        Update();
    }

    NetStatistics stats;
    g_pNet->GetNetworkStatistics ( &stats );

    // Draw the background
    float fResWidth = static_cast < float > ( g_pCore->GetGraphics ()->GetViewportWidth () );
    float fResHeight = static_cast < float > ( g_pCore->GetGraphics ()->GetViewportHeight () );

    // Grab the bytes sent/recv and datarate in the proper unit
    SString strBytesSent = GetDataUnit ( stats.bytesSent );
    SString strBytesRecv = GetDataUnit ( stats.bytesReceived );
    SString strRecvRate  = GetDataUnit ( Round ( m_fByteReceiveRate ) );
    SString strSendRate  = GetDataUnit ( Round ( m_fByteSendRate ) );

    SString strBuffer;

    // Select a string to print
    bool bHoldingCtrl = ( GetAsyncKeyState ( VK_CONTROL ) & 0x8000 ) != 0;
    if ( !bHoldingCtrl )
    {
        strBuffer = SString (
                "Ping: %u ms\n"
                "Messages in send buffer: %u\n"
                "Messages in resend buffer: %u\n"
                "Packets recv: %u\n"
                "Packets sent: %u\n"
                "Overall packet loss: %.2f%%\n"
                "Current packet loss: %.2f%%\n"
                "Bytes recv: %s\n"
                "Bytes sent: %s\n"
                "Datarate in/out: %s/s / %s/s\n"
                "Packet rate in/out: %u / %u\n"
                "BPS limit by CC: %llu\n"
                "BPS limit by OB: %llu\n"
                "Encryption: %s\n"
                "Client: %s\n"
                "Server: %s\n",
                g_pNet->GetPing (),
                stats.messagesInSendBuffer,
                stats.messagesInResendBuffer,
                stats.packetsReceived,
                stats.packetsSent,              
                stats.packetlossTotal,
                stats.packetlossLastSecond,
                strBytesRecv.c_str (),
                strBytesSent.c_str (),
                strRecvRate.c_str (),
                strSendRate.c_str (),
                (unsigned int)floor(m_fPacketReceiveRate + 0.5f),
                (unsigned int)floor(m_fPacketSendRate + 0.5f),                   
                stats.isLimitedByCongestionControl ? 1ULL : 0ULL,
                stats.isLimitedByOutgoingBandwidthLimit ? 1ULL : 0ULL,
                stats.encryptionStatus ? stats.encryptionStatus == 1 ? "On" : "Unknown" : "Off",
                *CStaticFunctionDefinitions::GetVersionSortable(),
                *g_pClientGame->GetServerVersionSortable ()
                );
    }
    else
    {
        NetRawStatistics& r = stats.raw;
        strBuffer += SString ( "messageSendBuffer %d,%d,%d,%d\n", r.messageSendBuffer[0], r.messageSendBuffer[1], r.messageSendBuffer[2], r.messageSendBuffer[3] );
        strBuffer += SString ( "messagesSent %d,%d,%d,%d\n", r.messagesSent[0], r.messagesSent[1], r.messagesSent[2], r.messagesSent[3] );
        strBuffer += SString ( "messageDataBitsSent %lld,%lld,%lld,%lld\n", r.messageDataBitsSent[0], r.messageDataBitsSent[1], r.messageDataBitsSent[2], r.messageDataBitsSent[3] );
        strBuffer += SString ( "messageTotalBitsSent %lld,%lld,%lld,%lld\n", r.messageTotalBitsSent[0], r.messageTotalBitsSent[1], r.messageTotalBitsSent[2], r.messageTotalBitsSent[3] );
        strBuffer += SString ( "packetsContainingOnlyAcknowlegements %d\n", r.packetsContainingOnlyAcknowlegements );
        strBuffer += SString ( "acknowlegementsSent %d\n", r.acknowlegementsSent );
        strBuffer += SString ( "acknowlegementsPending %d\n", r.acknowlegementsPending );
        strBuffer += SString ( "acknowlegementBitsSent %lld\n", r.acknowlegementBitsSent );
        strBuffer += SString ( "packetsContainingOnlyAcksAndResends %d\n", r.packetsContainingOnlyAcknowlegementsAndResends );
        strBuffer += SString ( "messageResends %d\n", r.messageResends );
        strBuffer += SString ( "messageDataBitsResent %lld\n", r.messageDataBitsResent );
        strBuffer += SString ( "messagesTotalBitsResent %lld\n", r.messagesTotalBitsResent );
        strBuffer += SString ( "messagesOnResendQueue %d\n", r.messagesOnResendQueue );
        strBuffer += SString ( "numberOfUnsplitMessages %d\n", r.numberOfUnsplitMessages );
        strBuffer += SString ( "numberOfSplitMessages %d\n", r.numberOfSplitMessages );
        strBuffer += SString ( "totalSplits %d\n", r.totalSplits );
        strBuffer += SString ( "packetsSent %d\n", r.packetsSent );
        strBuffer += SString ( "encryptionBitsSent %lld\n", r.encryptionBitsSent );
        strBuffer += SString ( "totalBitsSent %lld\n", r.totalBitsSent );
        strBuffer += SString ( "sequencedMessagesOutOfOrder %d\n", r.sequencedMessagesOutOfOrder );
        strBuffer += SString ( "sequencedMessagesInOrder %d\n", r.sequencedMessagesInOrder );
        strBuffer += SString ( "orderedMessagesOutOfOrder %d\n", r.orderedMessagesOutOfOrder );
        strBuffer += SString ( "orderedMessagesInOrder %d\n", r.orderedMessagesInOrder );
        strBuffer += SString ( "packetsReceived %d\n", r.packetsReceived );
        strBuffer += SString ( "packetsWithBadCRCReceived %d\n", r.packetsWithBadCRCReceived );
        strBuffer += SString ( "bitsReceived %lld\n", r.bitsReceived );
        strBuffer += SString ( "bitsWithBadCRCReceived %lld\n", r.bitsWithBadCRCReceived );
        strBuffer += SString ( "acknowlegementsReceived %d\n", r.acknowlegementsReceived );
        strBuffer += SString ( "duplicateAcknowlegementsReceived %d\n", r.duplicateAcknowlegementsReceived );
        strBuffer += SString ( "messagesReceived %d\n", r.messagesReceived );
        strBuffer += SString ( "invalidMessagesReceived %d\n", r.invalidMessagesReceived );
        strBuffer += SString ( "duplicateMessagesReceived %d\n", r.duplicateMessagesReceived );
        strBuffer += SString ( "messagesWaitingForReassembly %d\n", r.messagesWaitingForReassembly );
        strBuffer += SString ( "internalOutputQueueSize %d\n", r.internalOutputQueueSize );
        strBuffer += SString ( "bitsPerSecond %0.0f\n", r.bitsPerSecond );
        strBuffer += SString ( "connectionStartTime %lld\n", r.connectionStartTime );
        strBuffer += SString ( "bandwidthExceeded %d\n", r.bandwidthExceeded );
    }

    int iNumLines = 0;
    for ( int i = strBuffer.length () - 1 ; i >= 0 ; i-- )
        if ( strBuffer[i] == '\n' )
            iNumLines++;

    float fBackWidth = 310;
    float fBackHeight = (float)( iNumLines * 15 );
    float fX = fResWidth - fBackWidth;
    float fY = 0.40f * fResHeight - iNumLines * 4;

    g_pCore->GetGraphics ()->DrawRectQueued ( 
                                        fX - 10, fY - 10,
                                        fBackWidth + 10, fBackHeight + 20,
                                        0x78000000, true );

    g_pCore->GetGraphics ()->DrawTextQueued ( fX, fY,
                                              fX, fY,
                                              0xFFFFFFFF,
                                              strBuffer,
                                              1,
                                              1,
                                              DT_NOCLIP,
                                              NULL,
                                              true );
}


void CNetworkStats::Update ( void )
{
    NetStatistics stats;
    g_pNet->GetNetworkStatistics ( &stats );

    float delay = static_cast < float > ( CClientTime::GetTime () - m_ulLastUpdateTime );
    m_ulLastUpdateTime = CClientTime::GetTime ();

    m_fPacketSendRate = ( stats.packetsSent - m_uiLastPacketsSent ) * 1000 / delay;
    m_fPacketReceiveRate = ( stats.packetsReceived - m_uiLastPacketsReceived ) * 1000 / delay;
    m_fByteSendRate = ( stats.bytesSent - m_ullLastBytesSent ) * 1000 / delay;
    m_fByteReceiveRate = ( stats.bytesReceived - m_ullLastBytesReceived ) * 1000 / delay;

    m_uiLastPacketsSent = stats.packetsSent;
    m_uiLastPacketsReceived = stats.packetsReceived;
    m_ullLastBytesSent = stats.bytesSent;
    m_ullLastBytesReceived = stats.bytesReceived;
}
