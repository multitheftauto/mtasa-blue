/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/tracking/CTCPManager.cpp
*  PURPOSE:     Management of TCP socket clients and servers
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CTCPManager.h"

template<> CTCPManager * CSingleton< CTCPManager >::m_pSingleton = NULL;

CTCPManager::CTCPManager ( )
{
    // Init
    m_bInitialized = false;
    m_szLastError [0] = 0;

    // NULL the array of socket pointers
    memset ( &m_pSocket[0], NULL, sizeof ( m_pSocket ) );
}

CTCPManager::~CTCPManager ( )
{
    if ( m_bInitialized )
    {
        #ifdef WIN32
            // Clean up winsock
            WSACleanup ();
        #endif
    }

    // Clean up the array of socket pointers
    for ( unsigned int i = 0; i < 255; i++ )
    {
        SAFE_RELEASE ( m_pSocket[i] )
    }
}

void CTCPManager::HandleEvent ( unsigned int uiID, LPARAM lType )
{
    // Call the CTCP socket's event handler that's associated with the uiID argument
    if ( m_pSocket[uiID] != NULL )
    {
        // We know it's a derived class for sure.. so static_cast it without any error checking
        CTCPClientSocketImpl* pSocket = static_cast < CTCPClientSocketImpl* > ( m_pSocket[uiID] );
        pSocket->FireEvent ( lType );
    }
}

CTCPClientSocket* CTCPManager::CreateClient ( void )
{
    for ( unsigned int i = 0; i < 255; i++ )
    {
        // Check for an empty socket pointer slot
        if ( m_pSocket[i] == NULL )
        {
            // Create the socket and initialize it
            CTCPClientSocketImpl* pSocket = new CTCPClientSocketImpl;
            CCore::GetSingleton().GetConsole()->Printf("Async socket #%u was created.\n", i);
            if ( !pSocket->Initialize ( i ) )
            {
                // Copy the error details, delete it and return NULL
                strcpy ( m_szLastError, pSocket->GetLastError () );
                SAFE_RELEASE ( pSocket )
                return NULL;
            }
            m_pSocket[i] = pSocket;
            return pSocket;
        }
    }
    return NULL;
}


CTCPServerSocket* CTCPManager::CreateServer ( void )
{
    return 0;
}


bool CTCPManager::DestroyClient ( CTCPClientSocket* pClient )
{
    for ( unsigned int i = 0; i < 255; i++ )
    {
        // Check if we have a match on the socket pointer
        if ( m_pSocket[i] != NULL )
        {
            if ( m_pSocket[i] == pClient ) {
                CCore::GetSingleton().GetConsole()->Printf("Async socket #%u was destroyed.\n", i);
                SAFE_RELEASE( m_pSocket[i] )
                return true;
            }
        }
    }
    return false;
}


bool CTCPManager::DestroyServer ( CTCPServerSocket* pServer )
{
    /* ACHTUNG: do something */
    return true;
}


const char* CTCPManager::GetLastError ( void )
{
    return m_szLastError;
}


bool CTCPManager::Initialize ( void )
{
    if ( !m_bInitialized )
    {
        #ifdef WIN32
            // Initialize winsock
            WSADATA Data;
            if ( WSAStartup ( MAKEWORD ( 2, 0 ), &Data ) != 0 )
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


void CTCPManager::Reset ( void )
{

}
