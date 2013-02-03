/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CTCPClientSocketImpl.cpp
*  PURPOSE:     TCP client socket class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTCPClientSocketImpl::CTCPClientSocketImpl ( void )
{
    // Init
    m_bIsConnected = false;
    m_Socket = 0;
    m_szLastError [0] = 0;
}


CTCPClientSocketImpl::~CTCPClientSocketImpl ( void )
{
    // Free our socket
    if ( m_Socket )
    {
        CloseSocket ( m_Socket );
        m_Socket = 0;
    }
}


bool CTCPClientSocketImpl::Connect ( const char* szHost, unsigned short usPort, const char* szLocalIP )
{
    // If we're already connected, disconnect
    if ( m_bIsConnected )
    {
        Disconnect ();
    }

    // Got a socket?
    if ( !m_Socket )
    {
        // No socket
        strcpy ( m_szLastError, "No socket initialized" );
        return false;
    }

    // Resolve it
    hostent* pHostInfo = gethostbyname ( szHost );
    if ( !pHostInfo )
    {
        strcpy ( m_szLastError, "Unable to resolve" );
        return false;
    }

    // If a local IP has been specified, ensure it is used for sending
    if ( szLocalIP && szLocalIP[0] )
    {
        sockaddr_in m_SockAddr;
        m_SockAddr.sin_family = AF_INET;
        m_SockAddr.sin_addr.s_addr = inet_addr( szLocalIP );
        m_SockAddr.sin_port = 0;   

        // Bind the socket
		if ( ::bind ( m_Socket, ( sockaddr* ) &m_SockAddr, sizeof ( m_SockAddr ) ) == -1 )
        {
            strcpy ( m_szLastError, "Unable to bind socket" );
            return false;
        }
    }

    // Create a sockaddr_in structure and set the data
    sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons ( usPort );
    SockAddr.sin_addr = *( ( in_addr* ) *pHostInfo->h_addr_list );

    // Try to connect
    if ( connect ( m_Socket, ( sockaddr* )( &SockAddr ), sizeof ( SockAddr ) ) != 0 ) 
    {
        strcpy ( m_szLastError, "Failed to establish connection with server" );
        return false;
    }

    // Mark us as connected and return success
    m_bIsConnected = true;
    return true;
}


bool CTCPClientSocketImpl::Disconnect ( void )
{
    // Got a valid socket?
    if ( !m_Socket )
    {
        strcpy ( m_szLastError, "No socket initialized" );
        return false;
    }

    // Not connected?
    if ( !m_bIsConnected )
    {
        strcpy ( m_szLastError, "Not connected" );
        return false;
    }

    // Couldn't figure out how to gracefully close a connection and prepare the socket for reuse,
    // so I had to do this hacky approach
    //  Close the socket
    CloseSocket ( m_Socket );

    //  Recreate it
    m_Socket = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( ! m_Socket ) 
    {
        strcpy ( m_szLastError, "Recreating socket failed" );
        return false;
    }

    return true;
}


bool CTCPClientSocketImpl::IsConnected ( void )
{
    return m_bIsConnected;
}


const char* CTCPClientSocketImpl::GetLastError ( void )
{
    return m_szLastError;
}


int CTCPClientSocketImpl::ReadBuffer ( void* pOutput, int iSize )
{
    // Try to read something
    int iReturn = recv ( m_Socket, (char*)pOutput, iSize, 0 );
    if ( !iReturn )
    {
        // TODO: Store somewhere
#ifdef WIN32
        WSAGetLastError ();
#endif
        return -1;
    }

    // Return number of bytes actually read
    m_szLastError [0] = 0;
    return iReturn;
}


int CTCPClientSocketImpl::WriteBuffer ( const void* pInput, int iSize )
{
    // Try to write the stuff
    int iReturn = send ( m_Socket, (const char*)pInput, iSize, 0 );
    if ( !iReturn )
    {
        // TODO: Store somewhere
#ifdef WIN32
        WSAGetLastError ();
#endif
        return -1;
    }

    // Return number of bytes actually written
    m_szLastError [0] = 0;
    return iReturn;
}


bool CTCPClientSocketImpl::Initialize ( void )
{
    // Create a socket
    m_Socket = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( !m_Socket ) 
    {
        strcpy ( m_szLastError, "Creating socket failed" );
        return false;
    }

    return true;
}
