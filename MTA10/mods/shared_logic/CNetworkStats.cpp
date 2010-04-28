/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CNetworkStats.css
*  PURPOSE:     Network stats class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#pragma error("Deprecated\n");

CNetworkStats::CNetworkStats ( void )
{
    // Initialize members
    m_ullBytesReceived = 0;
    m_ullBytesSent = 0;
    m_ullRemoteBytesReceived = 0;
    m_ullRemoteBytesSent = 0;
}


CNetworkStats::~CNetworkStats ( void )
{

}


/*
Status packet:
unsigned int (8)      -   Bytes received at senders side
unsigned int (8)      -   Bytes sent at senders side
unsigned     (8)      -   Reserved (0)
*/

void CNetworkStats::ReadStatusPacket ( NetBitStreamInterface& bitStream )
{
    // Copy out the packet content
    bitStream.Read ( m_ullRemoteBytesReceived );
    bitStream.Read ( m_ullRemoteBytesSent );
    
    // Read out reserved size
    char buf [8];
    bitStream.Read ( buf, 8 );
}


void CNetworkStats::WriteStatusPacket ( NetBitStreamInterface& bitStream )
{
    // Write the number of bytes received and sent
    bitStream.Write ( m_ullBytesReceived );
    bitStream.Write ( m_ullBytesSent );

    // Write an empty buffer of 8 chars to fill the reserved
    char buf [8];
    memset ( buf, 0, 8 );
    bitStream.Write ( buf, 8 );
}


float CNetworkStats::GetAveragePacketLoss ( void )
{
    // Return the average packet loss in 0..100%
    return static_cast < float > ( 100 - ( GetBytesReceived () + GetBytesSent () ) / ( m_ullRemoteBytesReceived + m_ullRemoteBytesSent ) * 100 );
}


unsigned int CNetworkStats::GetBytesReceived ( void )
{
    m_ullBytesReceived = g_pNet->GetBitsReceived () / 8;
    return m_ullBytesReceived;
}


unsigned int CNetworkStats::GetBytesSent ( void )
{
    m_ullBytesSent = g_pNet->GetBitsSent () / 8;
    return m_ullBytesSent;
}


unsigned int CNetworkStats::GetRemotelyBytesReceived ( void )
{
    return m_ullRemoteBytesReceived;
}


unsigned int CNetworkStats::GetRemotelyBytesSent ( void )
{
    return m_ullRemoteBytesSent;
}
