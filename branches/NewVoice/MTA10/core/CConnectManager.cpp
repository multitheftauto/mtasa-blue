/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CConnectManager.cpp
*  PURPOSE:     Manager for connecting to servers
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/packetenums.h"
using namespace std;

static CConnectManager* g_pConnectManager = NULL;
extern CCore* g_pCore;

CConnectManager::CConnectManager ( void )
{
    g_pConnectManager = this;

    m_usPort = 0;
    m_bReconnect = false;
    m_bIsDetectingVersion = false;
    m_bIsConnecting = false;
    m_tConnectStarted = 0;

    m_pOnCancelClick = new GUI_CALLBACK ( &CConnectManager::Event_OnCancelClick, this );

    m_pServerItem = NULL;
}


CConnectManager::~CConnectManager ( void )
{
    if ( m_pOnCancelClick )
    {
        delete m_pOnCancelClick;
        m_pOnCancelClick = NULL;
    }

    SAFE_DELETE ( m_pServerItem );
    g_pConnectManager = NULL;
}


bool CConnectManager::Connect ( const char* szHost, unsigned short usPort, const char* szNick, const char* szPassword )
{
    assert ( szHost );
    assert ( szNick );
    assert ( szPassword );

    // Hide the server queue
    CServerInfo::GetSingletonPtr()->Hide( );

    // Save the browser state
    CServerBrowser::GetSingletonPtr()->SaveOptions( );

    // Are we already connecting?
    CNet* pNet = CCore::GetSingleton ().GetNetwork ();
    if ( m_bIsConnecting || pNet->IsConnected () )
    {
        CModManager::GetSingleton ().Unload ();
    }    

    // Is the nick valid?
    if ( !CheckNickProvided ( (char*) szNick ) )
    {
        SString strBuffer = "Connecting failed. Invalid nick provided!";
        CCore::GetSingleton ().ShowMessageBox ( "Error", strBuffer, MB_BUTTON_OK | MB_ICON_ERROR );
        return false;
    }

    // Save the nick too
    CVARS_SET ( "nick", std::string ( szNick ) );

    // Reset the network
    pNet->Reset ();
    assert ( pNet->GetServerBitStreamVersion () == 0 );

    // Save input
    m_strHost = szHost;
    m_strNick = szNick;
    m_strPassword = szPassword;
    m_usPort = usPort;

    m_strLastHost = m_strHost;
    m_usLastPort = m_usPort;
    m_strLastPassword = m_strPassword;

    // Parse host into a server item
    in_addr Address;
    if ( !CServerListItem::Parse ( m_strHost.c_str(), Address ) )
    {
        SString strBuffer = "Connecting failed. Invalid host provided!";
        CCore::GetSingleton ().ShowMessageBox ( "Error", strBuffer, MB_BUTTON_OK | MB_ICON_ERROR );
        return false;
    }

    // Set our packet handler
    pNet->RegisterPacketHandler ( CConnectManager::StaticProcessPacket, true );

    // Try to start a network to connect
    if ( !pNet->StartNetwork ( m_strHost.c_str (), m_usPort ) )
    {
        SString strBuffer ( "Connecting to %s at port %u failed!", m_strHost.c_str (), m_usPort );
        CCore::GetSingleton ().ShowMessageBox ( "Error", strBuffer, MB_BUTTON_OK | MB_ICON_ERROR );
        return false;
    }

    m_bIsConnecting = true;
    m_tConnectStarted = time ( NULL );

    // Start server version detection
    SAFE_DELETE ( m_pServerItem );
    m_pServerItem = new CServerListItem ( Address, m_usPort + SERVER_LIST_QUERY_PORT_OFFSET );
    m_pServerItem->m_iTimeoutLength = 2000;
    m_bIsDetectingVersion = true;

    // Display the status box
    SString strBuffer ( "Connecting to %s:%u ...", m_strHost.c_str(), m_usPort );
    CCore::GetSingleton ().ShowMessageBox ( "CONNECTING", strBuffer, MB_BUTTON_CANCEL | MB_ICON_INFO, m_pOnCancelClick );
    return true;
}


bool CConnectManager::Reconnect ( const char* szHost, unsigned short usPort, const char* szPassword )
{
    // Allocate a new host and nick buffer and store the strings in them
    if ( szHost )
    {
        m_strHost = szHost;
    }
    if ( szPassword )
    {
        m_strPassword = szPassword;
    }

    if ( usPort )
    {
        m_usPort = usPort;
    }

    m_bReconnect = true;

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
    m_strHost = "";
    m_strNick = "";
    m_strPassword = "";

    m_usPort = 0;
    m_bIsConnecting = false;
    m_bIsDetectingVersion = false;
    m_tConnectStarted = 0;
    SAFE_DELETE ( m_pServerItem );

    // Success
    return true;
}


void CConnectManager::DoPulse ( void )
{
    // Are we connecting?
    if ( m_bIsConnecting )
    {
        // Are we also getting the server version?
        if ( m_bIsDetectingVersion )
        {
            m_pServerItem->Pulse ( true );
            // Got some sort of result?
            if ( m_pServerItem->bSkipped || m_pServerItem->bScanned )
            {
                m_bIsDetectingVersion = false;
                // Is different version?
                if ( m_pServerItem->bScanned && m_pServerItem->strVersion != MTA_DM_ASE_VERSION )
                {
                    // Version mis-match. See about launching compatible .exe
                    GetVersionUpdater ()->InitiateSidegradeLaunch ( m_pServerItem->strVersion, m_strHost.c_str(), m_usPort, m_strNick.c_str (), m_strPassword.c_str() );
                    Abort ();
                    return;
                }
            }
        }

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
                SString strError;
                switch ( ucError )
                {
                    case ID_RSA_PUBLIC_KEY_MISMATCH:
                        strError = "Disconnected: unknown protocol error";  // encryption key mismatch
                        break;
                    case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                        strError = "Disconnected: disconnected remotely";
                        break;
                    case ID_REMOTE_CONNECTION_LOST:
                        strError = "Disconnected: connection lost remotely";
                        break;
                    case ID_CONNECTION_BANNED:
                        strError = "Disconnected: you are banned from this server";
                        break;
                    case ID_NO_FREE_INCOMING_CONNECTIONS:
                        CServerInfo::GetSingletonPtr()->Show ( eWindowTypes::SERVER_INFO_QUEUE, m_strHost.c_str(), m_usPort, m_strPassword.c_str() );
                        break;
                    case ID_DISCONNECTION_NOTIFICATION:
                        strError = "Disconnected: disconnected";
                        break;
                    case ID_CONNECTION_LOST:
                        strError = "Disconnected: connection lost";
                        break;
                    case ID_INVALID_PASSWORD:
                        CServerInfo::GetSingletonPtr()->Show ( eWindowTypes::SERVER_INFO_PASSWORD, m_strHost.c_str(), m_usPort, m_strPassword.c_str() );
                        break;
                    default:
                        strError = "Disconnected: connection refused";
                        break;
                }

                // Display an error, reset the error status and exit

                // Only display the error if we set one
                if ( strError.length() > 0 )
                    CCore::GetSingleton ().ShowMessageBox ( "Error", strError, MB_BUTTON_OK | MB_ICON_ERROR );
                else // Otherwise, remove the message box and hide quick connect
                {
                    CCore::GetSingleton ().RemoveMessageBox( false );
                    CCore::GetSingleton ().HideQuickConnect ();
                }

                CCore::GetSingleton ().GetNetwork ()->SetConnectionError ( 0 );
                CCore::GetSingleton ().GetNetwork ()->SetImmediateError ( 0 );
                Abort ();
            }
        }

        // Pulse the network interface
        CCore::GetSingleton ().GetNetwork ()->DoPulse ();
    }
    else if ( m_bReconnect )
    {
        std::string strNick;
        CVARS_GET ( "nick", strNick );
        Connect ( m_strHost.c_str(), m_usPort, strNick.c_str(), m_strPassword.c_str() );
        m_bReconnect = false;
    }
}


bool CConnectManager::StaticProcessPacket ( unsigned char ucPacketID, NetBitStreamInterface& BitStream )
{
    // We're working on connecting?
    if ( g_pConnectManager->m_bIsConnecting )
    {
        // The packet we're expecting?
        if ( ucPacketID == PACKET_ID_MOD_NAME )
        {
            // Read packet data
            unsigned short usServerBitStreamVersion = 0x01;
            BitStream.Read ( usServerBitStreamVersion );

            SString strModName;
            BitStream.ReadString ( strModName );

            // Process packet data
            CCore::GetSingleton ().GetNetwork ()->SetServerBitStreamVersion ( usServerBitStreamVersion );

            if ( strModName != "" )
            {
                // Populate the arguments to pass it (-c host port nick)
                SString strArguments ( "%s %s", g_pConnectManager->m_strNick.c_str(), g_pConnectManager->m_strPassword.c_str() );

                // Hide the messagebox we're currently showing
                CCore::GetSingleton ().RemoveMessageBox ();

                // If we connected from quick-connect, get rid of it
                CQuickConnect * pQuickConnect = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuickConnectWindow ();
                if ( pQuickConnect->IsVisible () )
                    pQuickConnect->SetVisible ( false );

                // Save the connection details into the config
                CVARS_SET ( "host",     g_pConnectManager->m_strHost );
                CVARS_SET ( "port",     g_pConnectManager->m_usPort );
                CVARS_SET ( "password", g_pConnectManager->m_strPassword );

                //Conver the Address to an unsigned long
                unsigned long ulAddr = inet_addr( g_pConnectManager->m_strHost.c_str() );

                //Create an instance of the in_addr structure to store the address
                in_addr Address;
                //Set the address to the unsigned long we just created
                Address.S_un.S_addr = ulAddr;
                //Set the current server info and Add the ASE Offset to the Query port)
                CCore::GetSingleton().SetCurrentServer ( Address, g_pConnectManager->m_usPort + 123 );

                // Kevuwk: Forced the config to save here so that the IP/Port isn't lost on crash
                CCore::GetSingleton ().SaveConfig ();

                // Reset our variables
                g_pConnectManager->m_strNick = "";
                g_pConnectManager->m_strHost = "";
                g_pConnectManager->m_strPassword = "";

                g_pConnectManager->m_usPort = 0;
                g_pConnectManager->m_bIsConnecting = false;
                g_pConnectManager->m_bIsDetectingVersion = false;
                g_pConnectManager->m_tConnectStarted = 0;

                // Load the mod
                if ( !CModManager::GetSingleton ().Load ( strModName, strArguments ) )
                {
                    // Failed loading the mod
                    strArguments.Format ( "No such mod installed (%s)", strModName.c_str() );
                    CCore::GetSingleton ().ShowMessageBox ( "Error", strArguments, MB_BUTTON_OK | MB_ICON_ERROR );
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


bool CConnectManager::CheckNickProvided ( const char* szNick )
{
    if ( stricmp ( szNick, "admin" ) == 0 )
        return false;
    if ( stricmp ( szNick, "console" ) == 0 )
        return false;
    if ( stricmp ( szNick, "server" ) == 0 )
        return false;
    return true;
}
