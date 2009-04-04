/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPortScan.cpp
*  PURPOSE:     Scanning local ports to make sure they are
*               accessible from the internet
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#include <iphlpapi.h>

CPortScan::CPortScan ( unsigned short usPort, unsigned char ucType, CBPORTSCAN pCallBack )
{
    m_ucType = ucType;
    m_usPort = usPort;

    m_pCallBack = pCallBack;

    m_Socket = NULL;

    m_hThread = NULL;

    if ( Init () )
    {
        m_hThread = CreateThread ( NULL, 0, Scan, this, 0, NULL );
    }
}


CPortScan::~CPortScan ( void )
{
    if ( m_hThread )
        CloseHandle ( m_hThread );
    if ( m_Socket )
        sockclose ( m_Socket );
}


bool CPortScan::Init ( void )
{
    m_SockAddr.sin_family = AF_INET;         
    m_SockAddr.sin_port = htons ( m_usPort );    
    m_SockAddr.sin_addr.s_addr = INADDR_ANY; 

    // Setup the recievenig server
    m_Socket = socket ( AF_INET, ( m_ucType == PROTOCOL_TCP ) ? SOCK_STREAM : SOCK_DGRAM, 0 );

    if ( bind ( m_Socket, ( sockaddr* ) &m_SockAddr, sizeof ( m_SockAddr ) ) != 0 )
    {
        sockclose ( m_Socket );
        m_Socket = NULL;
        return false;
    }

    unsigned long ulNonBlock = 1;
    ioctlsocket ( m_Socket, FIONBIO, &ulNonBlock );

    if ( m_ucType == PROTOCOL_TCP )
    {
        if ( listen ( m_Socket, 1 ) != 0 )
        {
            sockclose ( m_Socket );
            m_Socket = NULL;
            return false;
        }
    }
    return true;
}


DWORD WINAPI CPortScan::Scan ( LPVOID pObject )
{
    CPortScan* pScan = reinterpret_cast < CPortScan* > ( pObject );
    unsigned short usPort = pScan->GetPort();
    unsigned short usSent = 0;

    int Socket;

    // Sock addr
    sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons ( usPort );

    // Get adapters to cycle through our IPs
	ULONG BufferLength = 0;
	BYTE* pBuffer = 0;
	if ( ERROR_BUFFER_OVERFLOW == GetAdaptersInfo( 0, &BufferLength ) )
	{
		pBuffer = new BYTE[ BufferLength ];
	}

	PIP_ADAPTER_INFO pAdapterInfo =
		  reinterpret_cast<PIP_ADAPTER_INFO>(pBuffer);
	GetAdaptersInfo( pAdapterInfo, &BufferLength );

	while( pAdapterInfo )
    {
        IP_ADDR_STRING* pAddrList = &pAdapterInfo->IpAddressList;
		while ( pAddrList )
		{
            SockAddr.sin_addr.s_addr = inet_addr ( pAddrList->IpAddress.String );

            if ( pScan->GetType() == PROTOCOL_TCP )
            {
                Socket = socket ( AF_INET, SOCK_STREAM, 0 );

                // Set it to non-blocking cause we start pulsing later
                unsigned long ulNonBlock = 1;
                ioctlsocket ( Socket, FIONBIO, &ulNonBlock );

                connect ( Socket, ( sockaddr* )( &SockAddr ), sizeof ( SockAddr ) );
                //sockclose ( Socket );
                //Socket = NULL;

                usSent++;
            }
            else
            {
                Socket = socket ( AF_INET, SOCK_DGRAM, 0 );
                sendto ( Socket, PORTSCAN_STR, PORTSCAN_LEN+1, 0, ( sockaddr * ) &SockAddr, sizeof ( SockAddr ) );
                sockclose ( Socket );
                Socket = NULL;

                // Cause unlike TCP, bastard UDP doesn't need a connection to
                // send packets, we will log how many packets we sent and recieved
                usSent++;
            }

            pAddrList = pAddrList->Next;
        }

        pAdapterInfo = pAdapterInfo->Next;
    }

    // Start the recieving pulser
    unsigned long ulTimer = GetTickCount() + 3000;
    while ( usSent > 0 )
    {
        if ( GetTickCount() > ulTimer )
        {
            pScan->GetCallBack()( usPort, false, pScan );
            break;
        }
        pScan->DoPulse ( usSent );
        Sleep ( 500 );
    }

    return 0;
}


void CPortScan::DoPulse ( unsigned short& usSent )
{
    sockaddr_in SockAddr;
    socklen_t iLen = sizeof ( SockAddr );

    if ( m_ucType == PROTOCOL_TCP )
    {
        int Socket = accept ( m_Socket, ( sockaddr* )&SockAddr, &iLen );
        if ( Socket != -1 )
        {
            sockclose ( Socket );
            usSent--;
        }
    }
    else
    {
	    char szBuffer[PORTSCAN_LEN];
	    unsigned int uiBuffer = 0;

	    timeval tm;
        tm.tv_sec = 0;
	    tm.tv_usec = 0;

        fd_set readfds;
	    FD_ZERO(&readfds);
	    FD_SET(m_Socket, &readfds);

        if ( select ( 1, &readfds, NULL, NULL, &tm ) > 0 )
        {
		    uiBuffer = recvfrom ( m_Socket, szBuffer, PORTSCAN_LEN, 0, (sockaddr*)&SockAddr, &iLen );
            if ( uiBuffer > 0 )
		    {
                if ( strncmp ( szBuffer, PORTSCAN_STR, PORTSCAN_LEN ) == 0 )
                {
                    usSent--;
                }
		    }
        }
    }

    // Succeed
    if ( usSent <= 0 )
    {
        if ( m_pCallBack )
            m_pCallBack ( m_usPort, true, this );
    }
}