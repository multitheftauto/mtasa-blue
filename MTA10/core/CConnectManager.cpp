/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CConnectManager.cpp
*  PURPOSE:		Manager for connecting to servers
*  DEVELOPERS:	Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <net/net_packetenums.h>
using namespace std;

static CConnectManager* g_pConnectManager = NULL;
extern CCore* g_pCore;

CConnectManager::CConnectManager ( void )
{
    g_pConnectManager = this;

    m_szHost = NULL;
    m_szNick = NULL;
    m_szPassword = NULL;
    m_usPort = 0;
    m_bIsConnecting = false;
    m_tConnectStarted = 0;

    m_pOnCancelClick = new GUI_CALLBACK ( &CConnectManager::Event_OnCancelClick, this );

	// Set default MTU size
	m_usMTUSize = NET_MTU_DSL;
}


CConnectManager::~CConnectManager ( void )
{
    if ( m_szHost )
    {
        delete [] m_szHost;
        m_szHost = NULL;
    }
    if ( m_szNick )
    {
        delete [] m_szNick;
        m_szNick = NULL;
    }
    if ( m_szPassword )
    {
        delete [] m_szPassword;
        m_szPassword = NULL;
    }
    if ( m_pOnCancelClick )
    {
        delete m_pOnCancelClick;
        m_pOnCancelClick = NULL;
    }

    g_pConnectManager = NULL;
}


bool CConnectManager::Connect ( const char* szHost, unsigned short usPort, const char* szNick, const char* szPassword )
{
    assert ( szHost );
    assert ( szNick );
    assert ( szPassword );

    // Are we already connecting?
    CNet* pNet = CCore::GetSingleton ().GetNetwork ();
    if ( m_bIsConnecting || pNet->IsConnected () )
    {
        return false;
    }    

	// Is the nick valid?
	if ( !CheckNickProvided ( (char*) szNick ) )
	{
		char szBuffer [256];
		_snprintf ( szBuffer, 255, "Connecting failed. Invalid nick provided!" );
        CCore::GetSingleton ().ShowMessageBox ( "Error", szBuffer, MB_BUTTON_OK | MB_ICON_ERROR );
		return false;
	}

    // Save the nick too
    CVARS_SET ( "nick", std::string ( szNick ) );

    // Reset the network
    pNet->Reset ();

    // Set our packet handler
    pNet->RegisterPacketHandler ( CConnectManager::StaticProcessPacket, true );

    // Set our MTU size to the default
    pNet->SetMTUSize ( m_usMTUSize );

    // Try to start a network to connect
    char szBuffer [256];
    szBuffer [255] = 0;
    if ( !pNet->StartNetwork ( szHost, usPort ) )
    {
        _snprintf ( szBuffer, 255, "Connecting to %s at port %u failed!", szHost, usPort );
        CCore::GetSingleton ().ShowMessageBox ( "Error", szBuffer, MB_BUTTON_OK | MB_ICON_ERROR );
        return false;
    }

    // Eventually free previous host and nick
    if ( m_szHost )
    {
        delete [] m_szHost;
        m_szHost = NULL;
    }
    if ( m_szNick )
    {
        delete [] m_szNick;
        m_szNick = NULL;
    }
    if ( m_szPassword )
    {
        delete [] m_szPassword;
        m_szPassword = NULL;
    }

    // Allocate a new host and nick buffer and store the strings in them
    m_szHost = new char [ strlen ( szHost ) + 1 ];
    strcpy ( m_szHost, szHost );

    m_szNick = new char [ strlen ( szNick ) + 1 ];
    strcpy ( m_szNick, szNick );

    m_szPassword = new char [ strlen ( szPassword ) + 1 ];
    strcpy ( m_szPassword, szPassword );

    // Store the port and that we're connecting
    m_usPort = usPort;
    m_bIsConnecting = true;
    m_tConnectStarted = time ( NULL );

    // Display the status box
	_snprintf ( szBuffer, 255, "Connecting to %s:%u ...", szHost, usPort );
    CCore::GetSingleton ().ShowMessageBox ( "Connecting", szBuffer, MB_BUTTON_CANCEL | MB_ICON_INFO, m_pOnCancelClick );

    return true;
}


bool CConnectManager::Event_OnCancelClick ( CGUIElement * pElement )
{
	// The user clicked cancel, so abort connecting
	Abort ();
    // Remove the message box next frame
    g_pCore->RemoveMessageBox ( true );

	return true;
}


bool CConnectManager::Abort ( void )
{
    // Stop the attempt
    CNet* pNet = CCore::GetSingleton ().GetNetwork ();
    pNet->StopNetwork ();
    pNet->Reset ();

    // Reset our variables
    if ( m_szHost )
    {
        delete [] m_szHost;
        m_szHost = NULL;
    }

    if ( m_szNick )
    {
        delete [] m_szNick;
        m_szNick = NULL;
    }

    if ( m_szPassword )
    {
        delete [] m_szPassword;
        m_szPassword = NULL;
    }

    m_usPort = 0;
    m_bIsConnecting = false;
    m_tConnectStarted = 0;

    // Success
    return true;
}


void CConnectManager::DoPulse ( void )
{
    // Are we connecting?
    if ( m_bIsConnecting )
    {
        // Time to timeout the connection?
        if ( time ( NULL ) >= m_tConnectStarted + 8 )
        {
            // Show a message that the connection timed out and abort
            CCore::GetSingleton ().ShowMessageBox ( "Error", "Connection timed out", MB_BUTTON_OK | MB_ICON_ERROR );
            Abort ();
        }
        else
        {
            // Some error?
            unsigned char ucError = CCore::GetSingleton ().GetNetwork ()->GetConnectionError ();
            if ( ucError != 0 )
            {
                char szError [NET_DISCONNECT_REASON_SIZE] = {0};
                switch ( ucError )
                {
		            case ID_RSA_PUBLIC_KEY_MISMATCH:
                        _snprintf ( szError, NET_DISCONNECT_REASON_SIZE - 1, "Disconnected: unknown protocol error" );  // encryption key mismatch
			            break;
		            case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			            _snprintf ( szError, NET_DISCONNECT_REASON_SIZE - 1, "Disconnected: disconnected remotely" );
			            break;
		            case ID_REMOTE_CONNECTION_LOST:
			            _snprintf ( szError, NET_DISCONNECT_REASON_SIZE - 1, "Disconnected: connection lost remotely" );
			            break;
		            case ID_CONNECTION_BANNED:
			            _snprintf ( szError, NET_DISCONNECT_REASON_SIZE - 1, "Disconnected: you are banned from this server" );
			            break;
		            case ID_NO_FREE_INCOMING_CONNECTIONS:
			            _snprintf ( szError, NET_DISCONNECT_REASON_SIZE - 1, "Disconnected: server is full" );
			            break;
		            case ID_DISCONNECTION_NOTIFICATION:
			            _snprintf ( szError, NET_DISCONNECT_REASON_SIZE - 1, "Disconnected: disconnected" );
			            break;
		            case ID_CONNECTION_LOST:
			            _snprintf ( szError, NET_DISCONNECT_REASON_SIZE - 1, "Disconnected: connection lost" );
			            break;
		            case ID_INVALID_PASSWORD:
			            _snprintf ( szError, NET_DISCONNECT_REASON_SIZE - 1, "Disconnected: invalid password" );
			            break;
                    default:
			            _snprintf ( szError, NET_DISCONNECT_REASON_SIZE - 1, "Disconnected: connection refused" );
			            break;
                }

                // Display an error, reset the error status and exit
                CCore::GetSingleton ().ShowMessageBox ( "Error", szError, MB_BUTTON_OK | MB_ICON_ERROR );
                CCore::GetSingleton ().GetNetwork ()->SetConnectionError ( 0 );
				CCore::GetSingleton ().GetNetwork ()->SetImmediateError ( 0 );
                Abort ();
            }
        }

        // Pulse the network interface
        CCore::GetSingleton ().GetNetwork ()->DoPulse ();
    }
}


bool CConnectManager::StaticProcessPacket ( unsigned char ucPacketID, NetBitStreamInterface& BitStream, unsigned long ulTimeStamp )
{
    // We're working on connecting?
    if ( g_pConnectManager->m_bIsConnecting )
    {
        // The packet we're expecting?
        if ( ucPacketID == PACKET_ID_CORE_CONNECTED )
        {
            // Read out the mod to load
            char szModName [256];
            memset ( szModName, 0, 256 );
            if ( BitStream.Read ( szModName, BitStream.GetNumberOfBytesUsed () ) )
            {
                // Populate the arguments to pass it (-c host port nick)
                char szArguments [256];
                szArguments [255] = 0;
                _snprintf ( szArguments, 255, "%s %s", g_pConnectManager->m_szNick, g_pConnectManager->m_szPassword );

                // Hide the messagebox we're currently showing
                CCore::GetSingleton ().RemoveMessageBox ();

                // If we connected from quick-connect, get rid of it
                CQuickConnect * pQuickConnect = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuickConnectWindow ();
                if ( pQuickConnect->IsVisible () )
                    pQuickConnect->SetVisible ( false );

                // Save the connection details into the config
                CVARS_SET ( "host",     std::string ( g_pConnectManager->m_szHost ) );
                CVARS_SET ( "port",     g_pConnectManager->m_usPort );
                CVARS_SET ( "password", std::string ( g_pConnectManager->m_szPassword ) );


                // Save the connection details into the recently played servers list
                char szName [ 32 ];
                _snprintf ( szName, 32, "%s:%u", g_pConnectManager->m_szHost, g_pConnectManager->m_usPort );
//                CCore::GetSingleton ().GetConfig ()->GetRecentlyPlayedServers ().AddServer ( szName, g_pConnectManager->m_szHost, g_pConnectManager->m_usPort, true );

                // Kevuwk: Forced the config to save here so that the IP/Port isn't lost on crash
                CCore::GetSingleton ().SaveConfig ();

                // Reset our variables
                if ( g_pConnectManager->m_szHost )
                {
                    delete [] g_pConnectManager->m_szHost;
                    g_pConnectManager->m_szHost = NULL;
                }

                if ( g_pConnectManager->m_szNick )
                {
                    delete [] g_pConnectManager->m_szNick;
                    g_pConnectManager->m_szNick = NULL;
                }

                if ( g_pConnectManager->m_szPassword )
                {
                    delete [] g_pConnectManager->m_szPassword;
                    g_pConnectManager->m_szPassword = NULL;
                }

                g_pConnectManager->m_usPort = 0;
                g_pConnectManager->m_bIsConnecting = false;
                g_pConnectManager->m_tConnectStarted = 0;

                // Load the mod
                if ( !CModManager::GetSingleton ().Load ( szModName, szArguments ) )
                {
                    // Failed loading the mod
                    _snprintf ( szArguments, 255, "No such mod installed (%s)", szModName );
                    CCore::GetSingleton ().ShowMessageBox ( "Error", szArguments, MB_BUTTON_OK | MB_ICON_ERROR );
					g_pConnectManager->Abort ();
                }
            }
            else
            {
                // Show failed message and abort the attempt
                CCore::GetSingleton ().ShowMessageBox ( "Error", "Bad server response (2)", MB_BUTTON_OK | MB_ICON_ERROR );
                g_pConnectManager->Abort ();
            }
        }
        else
        {
            // Not the player joined packet?
            if ( ucPacketID != PACKET_ID_SERVER_JOIN && ucPacketID != PACKET_ID_SERVER_JOIN_DATA )
            {
                // Show failed message and abort the attempt
                CCore::GetSingleton ().ShowMessageBox ( "Error", "Bad server response (1)", MB_BUTTON_OK | MB_ICON_ERROR );
                g_pConnectManager->Abort ();
            }
        }

        return true;
    }

    return false;
}


bool CConnectManager::CheckNickProvided ( char* szNick )
{
    if ( stricmp ( szNick, "admin" ) == 0 )
        return false;
    if ( stricmp ( szNick, "console" ) == 0 )
        return false;
    if ( stricmp ( szNick, "server" ) == 0 )
        return false;
    return true;
}
