/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/sockets/CTCPImpl.cpp
*  PURPOSE:     TCP client/server manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oli <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CTCPImpl.h"

#ifdef WIN32
    #include <windows.h>
    #include <winsock.h>
#endif

#include <stdio.h>
#include <string.h>

CTCPImpl::CTCPImpl ( void )
{
    // Init
    m_pServerManager = new CTCPServerSocketImplManager;
    m_bInitialized = false;
    m_szLastError [0] = 0;
}


CTCPImpl::~CTCPImpl ( void )
{
    delete m_pServerManager;

    if ( m_bInitialized )
    {
        #ifdef WIN32
            // Clean up winsock
            WSACleanup ();
        #endif
    }
}


CTCPClientSocket* CTCPImpl::CreateClient ( void )
{
    // Create the socket and initialize it
    CTCPClientSocketImpl* pSocket = new CTCPClientSocketImpl;
    if ( !pSocket->Initialize () )
    {
        // Copy the error details, delete it and return NULL
        strcpy ( m_szLastError, pSocket->GetLastError () );
        delete pSocket;
        return 0;
    }

    // Return the socket we created
    return pSocket;
}


CTCPServerSocket* CTCPImpl::CreateServer ( void )
{
    // Create the socket and initialize it
    CTCPServerSocketImpl* pSocket = new CTCPServerSocketImpl ( m_pServerManager );
    if ( !pSocket->Initialize () )
    {
        // Copy the error details, delete it and return NULL
        strcpy ( m_szLastError, pSocket->GetLastError () );
        delete pSocket;
        return 0;
    }

    m_pServerManager->AddServer ( pSocket );

    // Return the socket we created
    return pSocket;
}


void CTCPImpl::DestroyClient ( CTCPClientSocket* pClient )
{
    delete pClient;
}


void CTCPImpl::DestroyServer ( CTCPServerSocket* pServer )
{
    m_pServerManager->RemoveServer ( reinterpret_cast < CTCPServerSocketImpl* > ( pServer ) );
}


bool CTCPImpl::Initialize ( void )
{
    if ( !m_bInitialized )
    {
        #ifdef WIN32
            // Initialize winsock
            WSADATA Data;
            if ( WSAStartup ( MAKEWORD ( 1, 1 ), &Data ) != 0 )
            {
                strcpy ( m_szLastError, "Unable to initialize Winsock" );
                return false;
            }

            // Confirm that winsock 1.1 is supported
            if ( LOBYTE ( Data.wVersion ) != 1 || HIBYTE ( Data.wVersion ) != 1 )
            {
                strcpy ( m_szLastError, "Winsock 1.1 is unsupported" );
                return false;
            }

            // Success
            m_bInitialized = true;
            return true;
        #else
            // Fill in for linux
			// -> Initialization code for sockets doesn't exist on *nix
        #endif
    }

    return true;
}


void CTCPImpl::Reset ( void )
{

}
