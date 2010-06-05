/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CLanBroadcast.cpp
*  PURPOSE:     LAN server broadcasting class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CLanBroadcast::CLanBroadcast ( unsigned short usServerPort )
{
    // Open the socket on the UDP broadcast port
    m_Socket = socket ( AF_INET, SOCK_DGRAM, 0 );

    m_SockAddr.sin_family       = AF_INET;         
    m_SockAddr.sin_port         = htons ( SERVER_LIST_BROADCAST_PORT );    
    m_SockAddr.sin_addr.s_addr  = INADDR_ANY; 
 
    setsockopt ( m_Socket, SOL_SOCKET, SO_REUSEADDR, "1", sizeof ( "1" ) );

    // Bind the socket
    if ( bind ( m_Socket, ( sockaddr* )&m_SockAddr, sizeof ( m_SockAddr ) ) != 0 )
    {
        closesocket ( m_Socket );
        return;
    }

    // Set it to non blocking, so we dont have to wait for a packet
    #ifdef WIN32
    unsigned long ulNonBlock = 1;
    ioctlsocket ( m_Socket, FIONBIO, &ulNonBlock );
    #else
    fcntl(m_Socket, F_SETFL, O_NONBLOCK); 
    #endif

    // Set up the query messages
    std::stringstream ss;
    m_strClientMessage = std::string ( SERVER_LIST_CLIENT_BROADCAST_STR ) + " " + std::string ( MTA_DM_ASE_VERSION );
    ss << std::string ( SERVER_LIST_SERVER_BROADCAST_STR ) << " " << ( usServerPort );
    m_strServerMessage = ss.str();
}


CLanBroadcast::~CLanBroadcast ( void )
{
    closesocket ( m_Socket );
}


void CLanBroadcast::DoPulse ( void )
{
    sockaddr_in SockAddr;
#ifndef WIN32
	socklen_t nLen = sizeof ( sockaddr );
#else
    int nLen = sizeof ( sockaddr );
#endif

    char szBuffer[32];
	int iBuffer = 0;

	iBuffer = recvfrom ( m_Socket, szBuffer, 31, 0, (sockaddr*)&SockAddr, &nLen );
	if ( iBuffer > 0 )
	{
        // Compare the client's query against the query message we stored
        // to ensure that we repond only to queries with the same version
		if ( m_strClientMessage.compare ( szBuffer ) == 0 )
		{
			/*int sent =*/ sendto ( m_Socket,
					m_strServerMessage.c_str (),
					m_strServerMessage.length () + 1,
					0,
					(sockaddr*)&SockAddr,
					nLen );
		}
	}
}
