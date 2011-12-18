/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/sockets/CTCPServerSocketImpl.cpp
*  PURPOSE:     TCP server socket class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CTCPServerSocketImpl.h"

/************************************
* (o<)< || IJs' short guide to      *
* //\   || writing POSIX compatible *
* V_/_  || network code             *
*************************************
*( 1. Keep all your socket related types lowercase. SOCKADDR_IN becomes sockaddr_in and compiles fine across platforms.
*( 2. Keep your types as basic as possible. No fancy SOCKET stuff. Use unsigned int instead, it's exactly the same.
*( 3. Enclose all the Winsock init/cleanup/error code in WIN32 ifdefs. There is no need for init/cleanup code in POSIX.
*( 4. Have a look at Utils.h in the race mod for all the necessary defines & cross-platform function defines.
************************************/


CTCPServerSocketImpl::CTCPServerSocketImpl ( CTCPServerSocketImplManager*  pServerManager )
{
    m_Socket = 0;
    m_bIsListening = false;
    m_bIsConnected = false;
    m_szLastError [0] = 0;
    m_uiListenPort = 0;
    m_pServerManager = pServerManager;
}


CTCPServerSocketImpl::~CTCPServerSocketImpl ( void )
{
    // Free our socket
    if ( m_Socket )
    {
        CloseSocket ( m_Socket );
        m_Socket = 0;
    }
}


bool CTCPServerSocketImpl::Listen ( unsigned int uiListenPort )
{
    // If we're already connected, we can't listen for connections
    if ( m_bIsConnected )
    {
        strcpy ( m_szLastError, "Socket in use, unable to listen" );
        return false;
    }

    // Got a socket?
    if ( !m_Socket )
    {
        // No socket
        strcpy ( m_szLastError, "No socket initialized" );
        return false;
    }

    // Set out port
    m_uiListenPort = uiListenPort;

    // Set our name stuff
    m_SockAddr.sin_family = AF_INET;
    m_SockAddr.sin_addr.s_addr = htonl ( INADDR_ANY );
    m_SockAddr.sin_port = htons ( m_uiListenPort );   

    // Bind the socket
    if ( bind ( m_Socket, ( sockaddr* ) &m_SockAddr, sizeof ( m_SockAddr ) ) == -1 )
    {
        strcpy ( m_szLastError, "Unable to bind socket" );
        CloseSocket ( m_Socket );
        m_Socket = 0;
        return false;
    }

    // Listen
    if ( listen ( m_Socket, 1 ) == -1 )
    {
        strcpy ( m_szLastError, "Unable to listen" );
        CloseSocket ( m_Socket );
        m_Socket = 0;
        return false;
    }

    // Set us to listening
    m_bIsListening = true;
    return true;
}


CTCPServerSocket* CTCPServerSocketImpl::Accept ( void )
{
    // Got a socket?
    if ( !m_Socket )
    {
        // No socket
        strcpy ( m_szLastError, "No socket initialized" );
        return NULL;
    }

    CTCPServerSocketImpl* pAcceptSocket = new CTCPServerSocketImpl ( m_pServerManager );
    socklen_t iLength = sizeof ( *pAcceptSocket->GetSocketAddress () );
    int Socket = accept ( m_Socket, reinterpret_cast < sockaddr* > ( pAcceptSocket->GetSocketAddress () ), &iLength );

    if ( Socket != -1 )
    {
        pAcceptSocket->SetSocket ( Socket );
        pAcceptSocket->SetConnected ( true );
        m_pServerManager->AddServer ( pAcceptSocket );
        return pAcceptSocket;
    }

    if ( pAcceptSocket )
        delete pAcceptSocket;
    return NULL;
}


bool CTCPServerSocketImpl::IsListening ( void )
{
    return m_bIsListening;
}


bool CTCPServerSocketImpl::IsConnected ( void )
{
    return m_bIsConnected;
}


void CTCPServerSocketImpl::SetConnected ( bool bConnected )
{
    m_bIsConnected = bConnected;
}


sockaddr_in* CTCPServerSocketImpl::GetSocketAddress ( void )
{
    return &m_SockAddr;
}


unsigned int CTCPServerSocketImpl::GetSocket ( void )
{
    return m_Socket;
}


void CTCPServerSocketImpl::SetSocket ( int Socket )
{
    m_Socket = Socket;
}


const char* CTCPServerSocketImpl::GetLastError ( void )
{
    return m_szLastError;
}


int CTCPServerSocketImpl::ReadBuffer ( void* pOutput, int iSize )
{
    // Try to read something
    int iReturn = recv ( m_Socket, static_cast < char* > ( pOutput ), iSize, 0 );

    if ( iReturn == -1 )
    {
    #ifdef WIN32
        int iError = WSAGetLastError ();

        if ( iError != WSAEWOULDBLOCK )
        {
            return 0;
        }
    #else
        // Need unix code here
    #endif
    }

    // Return number of bytes actually read
    m_szLastError [0] = 0;
    return iReturn;
}


int CTCPServerSocketImpl::WriteBuffer ( const void* pInput, int iSize )
{
    return send ( m_Socket, static_cast < const char* > ( pInput ), iSize, 0 );
}


bool CTCPServerSocketImpl::Initialize ( void )
{
    // Create a socket
    m_Socket = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( !m_Socket ) 
    {
        strcpy ( m_szLastError, "Creating socket failed" );
        return false;
    }

    // Set the socket to non blocking
    #ifdef WIN32
        unsigned long ulNonBlock = 1;
        if ( ioctlsocket ( m_Socket, FIONBIO, &ulNonBlock ) == SOCKET_ERROR )
        {
            strcpy ( m_szLastError, "Could not set socket to non-blocking" );
            return false;
        }
    #else
        int flags = 0;
        flags = fcntl ( m_Socket, F_GETFL, 0 );
        fcntl ( m_Socket, F_SETFL, flags | O_NONBLOCK );
    #endif

    return true;
}

