/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/tracking/CTCPClientSocketImpl.cpp
*  PURPOSE:     TCP client socket wrapper
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CTCPClientSocketImpl.h"

CTCPClientSocketImpl::CTCPClientSocketImpl ( void )
{
    m_uiID = 0;

    m_iRefCount = 1;

    // Init
    m_usPort = 0;
    m_bIsResolvingHost = false;
    m_bIsConnected = false;
    m_pAsyncHostResolving = NULL;
    m_Socket = 0;
    m_szLastError [0] = 0;

    // Events
    m_pEventRead = NULL;
    m_pEventWrite = NULL;
    m_pEventConnect = NULL;
    m_pEventClose = NULL;
    m_pClass = NULL;
}


CTCPClientSocketImpl::~CTCPClientSocketImpl ( void )
{
    assert ( m_iRefCount == 0 );

    // Free our socket
    if ( m_Socket )
    {
        closesocket ( m_Socket );
        m_Socket = 0;
    }

    // Cancel active host resolving
    if ( m_bIsResolvingHost )
    {
        WSACancelAsyncRequest ( m_pAsyncHostResolving );
        m_bIsResolvingHost = false;
    }
}

void CTCPClientSocketImpl::AddRef ( void )
{
    m_iRefCount++;
}

void CTCPClientSocketImpl::Release ( void )
{
    if ( !--m_iRefCount )
        delete this;
}

bool CTCPClientSocketImpl::Connect ( const char* szHost, unsigned short usPort )
{
    // Save the port
    m_usPort = usPort;

    // If we're already connected, disconnect
    if ( m_bIsConnected )
    {
        Disconnect ();
    }

    // If we have active host resolving, cancel it
    if ( m_bIsResolvingHost )
    {
        WSACancelAsyncRequest ( m_pAsyncHostResolving );
    }

    // Got a socket?
    if ( !m_Socket )
    {
        // No socket
        strcpy ( m_szLastError, "No socket initialized" );
        return false;
    }

    // Start async resolving it
    m_pAsyncHostResolving = WSAAsyncGetHostByName ( CCore::GetSingleton ().GetHookedWindow (), WM_ASYNCTRAP + m_uiID + 256, szHost, m_pHostInfo, MAXGETHOSTSTRUCT );
    if ( !m_pAsyncHostResolving )
    {
        // Failed
        strcpy ( m_szLastError, "Unable to start resolving" );
        return false;
    }
    m_bIsResolvingHost = true;
    return true;
}


void CTCPClientSocketImpl::ConnectContinue ( void )
{
    // Create a sockaddr_in structure and set the data
    sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons ( m_usPort );
    SockAddr.sin_addr = *( ( in_addr* ) *reinterpret_cast < hostent* > ( m_pHostInfo )->h_addr_list );

    // Try to connect
    int status = connect ( m_Socket, ( sockaddr* )( &SockAddr ), sizeof ( SockAddr ) );
    if ( status == SOCKET_ERROR ) {
        status = WSAGetLastError ();
    }

    // Mark us as connected
    // ACHTUNG: m_bIsConnected should be set by OnConnect!
    //m_bIsConnected = true;
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
    closesocket ( m_Socket );

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


bool CTCPClientSocketImpl::Initialize ( unsigned int uiID )
{
    // Save the ID
    m_uiID = uiID;

    // Create a socket
    m_Socket = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( !m_Socket ) 
    {
        strcpy ( m_szLastError, "Creating socket failed" );
        return false;
    }

    // So, make it asynchronous and enable some useful window messages
    if ( SOCKET_ERROR == WSAAsyncSelect ( m_Socket, CCore::GetSingleton ().GetHookedWindow (), WM_ASYNCTRAP + uiID, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE ) )
    {
        strcpy ( m_szLastError, "WSAAsyncSelect failed" );
        return false;
    }

    return true;
}

void CTCPClientSocketImpl::FireEvent ( bool bIsResolveEvent, uint uiResolveId, LPARAM lType )
{
    // Check this is the correct thingmy #1
    if ( bIsResolveEvent != m_bIsResolvingHost )
        return;

    // Are we resolving host? If so, we got this event from WSAAsyncGetHostByName
    if ( m_bIsResolvingHost )
    {
        // Check this is the correct thingmy #2
        if ( uiResolveId != (uint)m_pAsyncHostResolving )
            return;

        m_bIsResolvingHost = false;

        // Get error code
        unsigned short usError = WSAGETASYNCERROR ( lType );
        // Succeeded? Continue
        if ( !usError )
        {
            ConnectContinue ();
        }
        else
        {
            strcpy ( m_szLastError, "Unable to resolve" );
        }
        return;
    }

    // Check event type
    switch ( WSAGETSELECTEVENT ( lType ) )
    {
        case FD_READ:
            if ( m_pEventRead )
                m_pEventRead ( this, m_pClass );
            break;
        case FD_WRITE:
            if ( m_pEventWrite )
                m_pEventWrite ( this, m_pClass );
            break;
        case FD_CONNECT:    // Client only
            if ( m_pEventConnect )
                m_pEventConnect ( this, m_pClass );
            break;
        case FD_CLOSE:
            if ( m_pEventClose )
                m_pEventClose ( this, m_pClass );
            break;
        /*
        case FD_ACCEPT:     // Server only
            break;
        */
    }
}

void CTCPClientSocketImpl::SetEventClass ( void* pClass )
{
    m_pClass = pClass;
}

void CTCPClientSocketImpl::SetEventRead ( PFNEVENT pEvent )
{
    m_pEventRead = pEvent;
}

void CTCPClientSocketImpl::SetEventWrite ( PFNEVENT pEvent )
{
    m_pEventWrite = pEvent;
}

void CTCPClientSocketImpl::SetEventConnect ( PFNEVENT pEvent )
{
    m_pEventConnect = pEvent;
}

void CTCPClientSocketImpl::SetEventClose ( PFNEVENT pEvent )
{
    m_pEventClose = pEvent;
}

