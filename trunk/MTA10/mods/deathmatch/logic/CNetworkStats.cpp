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
    m_uiLastBytesSent = stats.runningTotal[NS_ACTUAL_BYTES_SENT];
    m_uiLastBytesReceived = stats.runningTotal[NS_ACTUAL_BYTES_RECEIVED];
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
    g_pCore->GetGraphics ()->DrawRectangle ( 
                                        0.75f * fResWidth, 0.30f * fResHeight,
                                        0.25f * fResWidth, 0.33f * fResHeight,
                                        0x78000000 );

    // Grab the bytes sent/recv and datarate in the proper unit
    SString strBytesSent = GetDataUnit ( stats.runningTotal [ NS_ACTUAL_BYTES_SENT ] );
    SString strBytesRecv = GetDataUnit ( stats.runningTotal [ NS_ACTUAL_BYTES_RECEIVED ] );
    SString strRecvRate  = GetDataUnit ( floor(m_fByteReceiveRate + 0.5f) );
    SString strSendRate  = GetDataUnit ( floor(m_fByteSendRate + 0.5f) );

    unsigned int uiNumMessagesInSendBuffer = 0;
    for ( int i = 0; i < PACKET_PRIORITY_COUNT; ++i )
        uiNumMessagesInSendBuffer += stats.messageInSendBuffer[i];

    // Populate a string to print
    SString strBuffer (
                "Ping: %u ms\n"
                "Messages in send buffer: %u\n"
                "Messages in resend buffer: %u\n"
                "Packets recv: %u\n"
                "Packets sent: %u\n"
                "Overall packet loss: %.2f%%\n"
                "Current packet loss: %.2f%%\n"
                "Bytes recv: %s\n"
                "Bytes sent: %s\n"
                "Datarate: %s/s / %s/s\n"
                "Packet rate: %u / %u\n"
                "Compression ratio: %.3f%%\n"
                "Decompression ratio: %.3f%%\n"
                "BPS limit by CC: %llu\n"
                "BPS limit by OB: %llu\n",

                g_pNet->GetPing (),

                uiNumMessagesInSendBuffer,
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
                
                stats.compressionRatio * 100.0f,
                stats.decompressionRatio * 100.0f,
                
                stats.isLimitedByCongestionControl ? stats.BPSLimitByCongestionControl : 0ULL,
                stats.isLimitedByOutgoingBandwidthLimit ? stats.BPSLimitByOutgoingBandwidthLimit : 0ULL
                );

    // Print it
    m_pDisplayManager->DrawText2D ( strBuffer, CVector ( 0.76f, 0.31f, 0 ), 1.0f, 0xFFFFFFFF );
}


void CNetworkStats::Update ( void )
{
    NetStatistics stats;
    g_pNet->GetNetworkStatistics ( &stats );

    float delay = CClientTime::GetTime () - m_ulLastUpdateTime;
    m_ulLastUpdateTime = CClientTime::GetTime ();

    m_fPacketSendRate = ( stats.packetsSent - m_uiLastPacketsSent ) * 1000 / delay;
    m_fPacketReceiveRate = ( stats.packetsReceived - m_uiLastPacketsReceived ) * 1000 / delay;
    m_fByteSendRate = ( stats.runningTotal[NS_ACTUAL_BYTES_SENT] - m_uiLastBytesSent ) * 1000 / delay;
    m_fByteReceiveRate = ( stats.runningTotal[NS_ACTUAL_BYTES_RECEIVED] - m_uiLastBytesReceived ) * 1000 / delay;

    m_uiLastPacketsSent = stats.packetsSent;
    m_uiLastPacketsReceived = stats.packetsReceived;
    m_uiLastBytesSent = stats.runningTotal[NS_ACTUAL_BYTES_SENT];
    m_uiLastBytesReceived = stats.runningTotal[NS_ACTUAL_BYTES_RECEIVED];
}
