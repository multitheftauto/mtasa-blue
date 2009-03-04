/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CGame.cpp
*  PURPOSE:     Server game class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define MAX_KEYSYNC_DISTANCE 400.0f
#define MAX_EXPLOSION_SYNC_DISTANCE 400.0f
#define MAX_PROJECTILE_SYNC_DISTANCE 400.0f

CGame* g_pGame = NULL;

char szProgress[4] = {'-','\\','|','/'};
unsigned char ucProgress = 0;
unsigned char ucProgressSkip = 0;

pthread_mutex_t mutexhttp;

#ifdef WIN32
BOOL WINAPI ConsoleEventHandler ( DWORD dwCtrlType )
{
    if ( dwCtrlType == CTRL_CLOSE_EVENT )
    {
        if ( g_pGame )
        {
            g_pGame->SetIsFinished ( true );
            return TRUE;
        }
    }

    return FALSE;
}
#endif


CGame::CGame ( void )
{
#ifdef WIN32
    InitializeCriticalSection(&m_cs);
#endif
    // Set our global pointer
    g_pGame = this;

    // Initialize random number generator and time
    srand ( time ( NULL ) );
    InitializeTime ();

	m_bBeingDeleted = false;
    m_pUnoccupiedVehicleSync = NULL;
    m_pConsole = NULL;
    m_pMapManager = NULL;
    m_pHandlingManager = NULL;
    m_pLuaManager = NULL;
    m_pMapFiles = NULL;
    m_pPacketTranslator = NULL;
    m_pMarkerManager = NULL;
    m_pRadarAreaManager = NULL;
    m_pPlayerManager = NULL;
    m_pVehicleManager = NULL;
    m_pPickupManager = NULL;
    m_pObjectManager = NULL;
    m_pColManager = NULL;
    m_pBlipManager = NULL;
    m_pClock = NULL;
    m_pScriptDebugging = NULL;
    m_pBanManager = NULL;
    m_pTeamManager = NULL;
    m_pMainConfig = NULL;
    m_pRegistry = NULL;
    m_pAccountManager = NULL;
    m_pPedManager = NULL;
    m_pResourceManager = NULL;
    m_pHTTPD = NULL;
    m_pACLManager = NULL;
    m_pRegisteredCommands = NULL;
    m_pZoneNames = NULL;
    m_pGroups = NULL;
    m_pSettings = NULL;
    m_pRemoteCalls = NULL;
    m_pResourceDownloader = NULL;
    m_pRPCFunctions = NULL;
	m_pLanBroadcast = NULL;
    m_pPedSync = NULL;
    m_pWaterManager = NULL;

#ifdef MTA_VOICE
    m_pVoiceServer = NULL;
#endif

    m_pASE = NULL;
    m_fGravity = 0.008f; // default
    m_fGameSpeed = 1.0f; // default
    m_fWaveHeight = 0.0f; // default - weather controlled
    m_ucSkyGradientTR = 0, m_ucSkyGradientTG = 0, m_ucSkyGradientTB = 0;
    m_ucSkyGradientBR = 0, m_ucSkyGradientBG = 0, m_ucSkyGradientBB = 0;
    m_bHasSkyGradient = false;
	m_usFPS = 0;
	m_usFrames = 0;
	m_ulLastFPSTime = 0;
    m_ulNextPingTime = 0;
    m_LastPingPlayer = INVALID_ELEMENT_ID;
    m_szCurrentFileName = NULL;
    m_pConsoleClient = NULL;
    m_bIsFinished = false;     

    memset( m_bGarageStates, 0, sizeof(bool) * MAX_GARAGES );

    // init our mutex
    pthread_mutex_init(&mutexhttp, NULL);
}


CGame::~CGame ( void )
{
	m_bBeingDeleted = true;
#ifdef WIN32
    DeleteCriticalSection(&m_cs);
#endif

    // Remove our console control handler
    #ifdef WIN32
    SetConsoleCtrlHandler ( ConsoleEventHandler, FALSE );
    #endif

    // Eventually stop our game
    Stop ();

     // Destroy our stuff
    if ( m_pResourceManager )
    {
        delete m_pResourceManager;
        m_pResourceManager = NULL;
    }

    // Delete everything we have undeleted
    m_ElementDeleter.DoDeleteAll ();
    SAFE_DELETE ( m_pUnoccupiedVehicleSync );
    SAFE_DELETE ( m_pPedSync );
    SAFE_DELETE ( m_pConsole );
    SAFE_DELETE ( m_pMapManager );
    SAFE_DELETE ( m_pLuaManager );
    SAFE_DELETE ( m_pMapFiles );
    SAFE_DELETE ( m_pPacketTranslator );
    SAFE_DELETE ( m_pMarkerManager );
    SAFE_DELETE ( m_pRadarAreaManager );
    SAFE_DELETE ( m_pPlayerManager );
    SAFE_DELETE ( m_pVehicleManager );
    SAFE_DELETE ( m_pHandlingManager );
    SAFE_DELETE ( m_pPickupManager );
    SAFE_DELETE ( m_pObjectManager );
    SAFE_DELETE ( m_pColManager );
    SAFE_DELETE ( m_pBlipManager );
    SAFE_DELETE ( m_pClock );
    SAFE_DELETE ( m_pScriptDebugging );
    SAFE_DELETE ( m_pBanManager );
    SAFE_DELETE ( m_pTeamManager );
    SAFE_DELETE ( m_pMainConfig );
    SAFE_DELETE ( m_pRegistry );
    SAFE_DELETE ( m_pAccountManager );
    SAFE_DELETE ( m_pRegisteredCommands );
    SAFE_DELETE ( m_pPedManager );
    SAFE_DELETE ( m_pHTTPD );
    SAFE_DELETE ( m_pACLManager );
    SAFE_DELETE ( m_pGroups );
    SAFE_DELETE ( m_pZoneNames );
    SAFE_DELETE ( m_pASE );
    SAFE_DELETE ( m_pSettings );
    SAFE_DELETE ( m_pRemoteCalls );
    SAFE_DELETE ( m_pResourceDownloader );
    SAFE_DELETE ( m_pRPCFunctions );
    SAFE_DELETE ( m_pWaterManager );

    // Clear our global pointer
    g_pGame = NULL;
}


void CGame::GetTag ( char *szInfoTag, int iInfoTag )
{
	// Construct the info tag
    unsigned int uiMaxPlayers;
    if ( !m_CommandLineParser.GetMaxPlayers ( uiMaxPlayers ) )
        uiMaxPlayers = m_pMainConfig->GetMaxPlayers ();
	_snprintf ( szInfoTag, iInfoTag, "%c[%c%c%c] MTA:SA Deathmatch R1 %c:%c: %d/%d players %c:%c: %u resources %c:%c: %u fps",
			   132, 135, szProgress[ucProgress], 132,
		       130, 130, m_pPlayerManager->Count (), uiMaxPlayers,
               130, 130, m_pResourceManager->GetResourceLoadedCount (),
			   130, 130, m_usFPS );
	if (iInfoTag > 0)
		szInfoTag[iInfoTag-1] = '\0';
}


void CGame::HandleInput ( char* szCommand )
{
    // Lock the critical section so http server won't interrupt in the middle of our pulse
    Lock ();
    // Handle the input
	m_pConsole->HandleInput ( szCommand, m_pConsoleClient, m_pConsoleClient );

    // Unlock the critical section again
    Unlock();
}


void CGame::DoPulse ( void )
{
    // Lock the critical section so http server won't interrupt in the middle of our pulse
    Lock ();
    // Calculate FPS
	unsigned long ulCurrentTime = GetTickCount ();
	unsigned long ulDiff = ulCurrentTime - m_ulLastFPSTime;

	// Update the progress rotator
	if ( ucProgressSkip == 5 ) {
		ucProgressSkip = 0;

		// Clamp ucProgress between 0 and 3
		ucProgress = ( ucProgress + 1 ) & 3;
	}
	ucProgressSkip++;

	// Calculate the server-side fps
	if ( ulDiff >= 1000 )
	{
		m_usFPS = m_usFrames;
		m_usFrames = 0;
		m_ulLastFPSTime = ulCurrentTime;
	}
	m_usFrames++;

 
#ifdef MTA_VOICE
    // Do voice processing
    if ( m_pVoiceServer ) 
    {
        list < CPlayer * >::const_iterator vplayer = m_pPlayerManager->IterBegin ( );
        unsigned char Data [ 1024 ];
        unsigned short usLen = 0;
        CVoiceDataPacket Packet;
        
        for (; vplayer != m_pPlayerManager->IterEnd ( ); vplayer++ )
        {
            usLen = m_pVoiceServer->GetMixedBufForClient ( (*vplayer)->GetID ( ), Data, 1024 );
            if ( usLen > 0 )
            {
                Packet.SetData ( Data, usLen );
                
                (*vplayer)->Send ( Packet );
            }
            memset ( Data, 0, sizeof ( Data ) );
        }

        m_pVoiceServer->FreePerFrameBuffers ( );    
    }
#endif

    //_asm int 3
    g_pNetServer->GetHTTPDownloadManager()->ProcessQueuedFiles();
    
    // TODO: Low Priorityy: No need to do this every frame. Could be done every minute or so.
    // Remove any players that have been connected for very long (90 sec) but hasn't reached the verifying step
    list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
    {
        if ( (*iter)->GetStatus () == STATUS_CONNECTED && GetTime () > (*iter)->GetTimeConnected () + 90000 )
        {
            // Tell the console he timed out due during connect
            char szIP [64];
            CLogger::LogPrintf ( "INFO: %s (%s) timed out during connect\n", (*iter)->GetNick (), (*iter)->GetSourceIP ( szIP ) );

            // Remove him
            delete *iter;
            break;
        }
    }

    m_pPlayerManager->DoPulse ();

    // Pulse the net interface
    g_pNetServer->DoPulse ();

	if ( m_pLanBroadcast )
		m_pLanBroadcast->DoPulse();

    // Pulse our stuff
    m_pMapManager->DoPulse ();
    m_pUnoccupiedVehicleSync->DoPulse ();
    m_pPedSync->DoPulse ();
    m_pBanManager->DoPulse ();
    m_pAccountManager->DoPulse ();

    // Eventually send pings...
    SendPings ();

    // Pulse ASE
    if ( m_pASE )
    {
        m_pASE->DoPulse ();
    }

    // Pulse the scripting system
    if ( m_pLuaManager )
    {
        m_pLuaManager->DoPulse ();
    }

    // Process our resource stop/restart queue
    m_pResourceManager->ProcessQueue ();

    // Delete all items requested
    m_ElementDeleter.DoDeleteAll ();

    // Unlock the critical section again
    Unlock();
}


bool CGame::Start ( int iArgumentCount, char* szArguments [] )
{
    // Init
    m_pASE = NULL;

#ifdef MTA_VOICE
    m_pVoiceServer = NULL;
#endif

	m_pGroups = new CGroups;
    m_pClock = new CClock;
    m_pBlipManager = new CBlipManager;
    m_pColManager = new CColManager;
    m_pObjectManager = new CObjectManager;
    m_pPickupManager = new CPickupManager ( m_pColManager );
    m_pPlayerManager = new CPlayerManager;
    m_pRadarAreaManager = new CRadarAreaManager;
    m_pMarkerManager = new CMarkerManager ( m_pColManager );
    m_pHandlingManager = new CHandlingManager;
    m_pVehicleManager = new CVehicleManager;
    m_pPacketTranslator = new CPacketTranslator ( m_pPlayerManager );
    m_pMapFiles = new CMapFiles;
    m_pBanManager = new CBanManager;
    m_pTeamManager = new CTeamManager;
    m_pPedManager = new CPedManager;
    m_pScriptDebugging = new CScriptDebugging ( m_pLuaManager );
    m_pMapManager = new CMapManager ( m_pBlipManager, m_pObjectManager, m_pPickupManager, m_pPlayerManager, m_pRadarAreaManager, m_pMarkerManager, m_pVehicleManager, m_pTeamManager, m_pPedManager, m_pColManager, m_pMapFiles, m_pClock, m_pLuaManager, m_pGroups, &m_Events, m_pScriptDebugging, &m_ElementDeleter );
    m_pACLManager = new CAccessControlListManager;

    m_pRegisteredCommands = new CRegisteredCommands ( m_pACLManager );
    m_pLuaManager = new CLuaManager ( m_pObjectManager, m_pPlayerManager, m_pVehicleManager, m_pBlipManager, m_pRadarAreaManager, m_pRegisteredCommands, m_pMapManager, &m_Events );
    m_pConsole = new CConsole ( m_pBlipManager, m_pMapManager, m_pPlayerManager, m_pRegisteredCommands, m_pVehicleManager, m_pLuaManager, &m_WhoWas, m_pMapFiles, m_pBanManager, m_pACLManager );
    m_pMainConfig = new CMainConfig ( m_pConsole, m_pLuaManager );
    m_pRPCFunctions = new CRPCFunctions;
    m_pWaterManager = new CWaterManager;

    // Parse the commandline
    if ( !m_CommandLineParser.Parse ( iArgumentCount, szArguments ) )
    {
        return false;
    }

    // Grab the path to the main config
    char szBuffer [MAX_PATH];
    const char* szMainConfig;
    if ( m_CommandLineParser.GetMainConfig ( szMainConfig ) )
    {
        g_pServerInterface->GetModManager ()->GetAbsolutePath ( szMainConfig, szBuffer, MAX_PATH );
    }
    else
    {
        g_pServerInterface->GetModManager ()->GetAbsolutePath ( "mtaserver.conf", szBuffer, MAX_PATH );
    }
    
    m_pResourceDownloader = new CResourceDownloader();

    // Load the main config base
    if ( !m_pMainConfig->Load ( szBuffer ) )
        return false;

    // Grab the IP to put the server at
    const char* szServerIP;
    if ( !m_CommandLineParser.GetIP ( szServerIP ) )
        szServerIP = m_pMainConfig->GetServerIP ().c_str ();

    if ( szServerIP && szServerIP [0] == '\0' )
        szServerIP = NULL;
    
    unsigned short usServerPort;
    if ( !m_CommandLineParser.GetPort ( usServerPort ) )
        usServerPort = m_pMainConfig->GetServerPort ();

    unsigned int uiMaxPlayers;
    if ( !m_CommandLineParser.GetMaxPlayers ( uiMaxPlayers ) )
        uiMaxPlayers = m_pMainConfig->GetMaxPlayers ();

    // Create the account manager
    m_pAccountManager = new CAccountManager ( NULL );

	// Create and start the HTTP server
    m_pHTTPD = new CHTTPD;

    // Enable it if -nohttp is not specified in the commandline
    if ( m_CommandLineParser.IsHTTPEnabled () )
    {
	    m_pHTTPD->StartHTTPD ( szServerIP, m_pMainConfig->GetHTTPPort () );
    }

    // Eventually set a logfile
	bool bLogFile = true;
    if ( m_pMainConfig->GetLogFileEnabled () && !m_pMainConfig->GetLogFile ().empty () )
    {
        // Try to set the logfile
        if ( !CLogger::SetLogFile ( m_pMainConfig->GetLogFile ().c_str () ) )
			bLogFile = false;
    }

    // Trim the logfile name for the output
    char szLogFileNameOutput [MAX_PATH];
    char* pszLogFileName = szLogFileNameOutput;
    strncpy ( szLogFileNameOutput, m_pMainConfig->GetLogFile ().c_str (), MAX_PATH );
    size_t sizeLogFileName = strlen ( szLogFileNameOutput );
    if ( sizeLogFileName > 45 )
    {
        pszLogFileName += ( sizeLogFileName - 45 );
        pszLogFileName [0] = '.';
        pszLogFileName [1] = '.';
    }

    // Show the server header
	CLogger::LogPrintfNoStamp ( "===========================================================\n" \
								"= Multi Theft Auto: San Andreas Deathmatch v%s\n" \
								"===========================================================\n" \
								"= Server name		: %s\n" \
								"= Server IP address	: %s\n" \
								"= Server port		: %u\n" \
								"= \n" \
								"= Log file		: %s\n" \
								"= Maximum players	: %u\n" \
								"= MTU packet size	: %u\n" \
								"===========================================================\n",

                                MTA_VERSION,
								m_pMainConfig->GetServerName ().c_str (),
								szServerIP,
								usServerPort,
								pszLogFileName,
								uiMaxPlayers,
								m_pMainConfig->GetMTUSize () );

	if ( !bLogFile )
		CLogger::ErrorPrintf ( "Unable to save logfile to '%s'\n", m_pMainConfig->GetLogFile ().c_str () );

	m_pRemoteCalls = new CRemoteCalls();
    m_pRegistry = new CRegistry ("");

	m_pResourceManager = new CResourceManager;
    m_pSettings = new CSettings ( m_pResourceManager );
    m_pResourceManager->Refresh();
    m_pUnoccupiedVehicleSync = new CUnoccupiedVehicleSync ( m_pPlayerManager, m_pVehicleManager );
    m_pPedSync = new CPedSync ( m_pPlayerManager, m_pPedManager );
    // Must be created before all clients    
    m_pConsoleClient = new CConsoleClient ( m_pConsole );
    


    m_pZoneNames = new CZoneNames;

    CStaticFunctionDefinitions ( this );
    CLuaFunctionDefinitions::SetBlipManager ( m_pBlipManager );
    CLuaFunctionDefinitions::SetLuaManager ( m_pLuaManager );
    CLuaFunctionDefinitions::SetMarkerManager ( m_pMarkerManager );
    CLuaFunctionDefinitions::SetObjectManager ( m_pObjectManager );
    CLuaFunctionDefinitions::SetPickupManager ( m_pPickupManager );
    CLuaFunctionDefinitions::SetPlayerManager ( m_pPlayerManager );
    CLuaFunctionDefinitions::SetRadarAreaManager ( m_pRadarAreaManager );
    CLuaFunctionDefinitions::SetTeamManager ( m_pTeamManager );
    CLuaFunctionDefinitions::SetAccountManager ( m_pAccountManager );
    CLuaFunctionDefinitions::SetRegisteredCommands ( m_pRegisteredCommands );
    CLuaFunctionDefinitions::SetRootElement ( m_pMapManager->GetRootElement () );
    CLuaFunctionDefinitions::SetScriptDebugging ( m_pScriptDebugging );
    CLuaFunctionDefinitions::SetVehicleManager ( m_pVehicleManager );
    CLuaFunctionDefinitions::SetColManager ( m_pColManager );
    CLuaFunctionDefinitions::SetResourceManager ( m_pResourceManager );
    CLuaFunctionDefinitions::SetACL ( m_pACLManager );

    // Initialize the lua function definition dependancies
    CLuaDefs::Initialize ( m_pMapManager->GetRootElement (),
                           &m_ElementDeleter,
                           m_pBlipManager,
                           m_pHandlingManager,
                           m_pLuaManager,
                           m_pMarkerManager,
                           m_pObjectManager,
                           m_pPickupManager,
                           m_pPlayerManager,
                           m_pRadarAreaManager,
                           m_pRegisteredCommands,
                           m_pScriptDebugging,
                           m_pVehicleManager,
                           m_pTeamManager,
                           m_pAccountManager,
                           m_pColManager,
                           m_pResourceManager,
                           m_pACLManager,
                           m_pMainConfig );
                           
    m_pPlayerManager->SetScriptDebugging ( m_pScriptDebugging );

    m_ulNextPingTime = 0;
    m_LastPingPlayer = INVALID_ELEMENT_ID;

    // Set our console control handler
    #ifdef WIN32
    SetConsoleCtrlHandler ( ConsoleEventHandler, TRUE );
    #endif

    // Add our builtin events
    AddBuiltInEvents ();

	// Load the vehicle colors before the main config
    g_pServerInterface->GetModManager ()->GetAbsolutePath ( "vehiclecolors.conf", szBuffer, 512 );
    if ( !m_pVehicleManager->GetColorManager ()->Load ( szBuffer ) )
    {
        // Try to generate a new one and load it again
        if ( m_pVehicleManager->GetColorManager ()->Generate ( szBuffer ) )
        {
            if ( !m_pVehicleManager->GetColorManager ()->Load ( szBuffer ) )
            {
                CLogger::ErrorPrintf ( "%s", "Loading 'vehiclecolors.conf' failed\n " );
            }
        }
        else
        {
            CLogger::ErrorPrintf ( "%s", "Generating a new 'vehiclecolors.conf' failed\n " );
        }
    }

    // Load the ACL's
    if ( !m_pACLManager->Load ( const_cast < char* > ( m_pMainConfig->GetAccessControlListFile ().c_str () ) ) )
        return false;

    // Load the registry
    g_pServerInterface->GetModManager ()->GetAbsolutePath ( "registry.db", szBuffer, MAX_PATH );
    m_pRegistry->Load ( szBuffer );

	// Now load the rest of the config
	if ( !m_pMainConfig->LoadExtended () )
		return false;

    // Load the accounts
    g_pServerInterface->GetModManager ()->GetAbsolutePath ( "accounts.xml", szBuffer, MAX_PATH );
    m_pAccountManager->SetFileName ( szBuffer );
    m_pAccountManager->Load ();

    // Register our packethandler
    g_pNetServer->RegisterPacketHandler ( CGame::StaticProcessPacket, TRUE );

	// Grab the MTU size
	unsigned int uiMTUSize = m_pMainConfig->GetMTUSize ();

    // Try to start the network
    if ( !g_pNetServer->StartNetwork ( szServerIP, usServerPort, uiMTUSize, uiMaxPlayers ) )
    {
		CLogger::ErrorPrintf ( "Could not bind the server on interface '%s' and port '%u'!\n", szServerIP, usServerPort );
        return false;
    }

	if ( !m_pMainConfig->GetDontBroadcastLan() )
	{
		m_pLanBroadcast = g_pServerInterface->GetLanBroadcast();
		if ( m_pLanBroadcast )
		{
			// Try to start lan broadcaster
			if ( !m_pLanBroadcast->Initialize ( usServerPort ) )
				CLogger::LogPrintf ( "Could not bind LAN broadcast on port '%u'\n", m_pLanBroadcast->GetPort() );
		}
	}

    // Load the banlist
    m_pBanManager->LoadBanList ();

    // If the server is passworded
	if ( m_pMainConfig->HasPassword () )
	{
        // Check it for validity
        const char* szPassword = m_pMainConfig->GetPassword ().c_str ();
        unsigned int uiUnsupportedIndex;
        if ( m_pMainConfig->IsValidPassword ( szPassword, uiUnsupportedIndex ) )
        {
            // Store the server password
		    CLogger::LogPrintf ( "Server password set to '%s'\n", szPassword );

            // If remote admin is enabled
            if ( m_pMainConfig->GetAdminServerEnabled () )
            {
			    CLogger::LogPrint ( "Password can be changed from Remote Admin\n" );
		    }
        }
        else
        {
            CLogger::LogPrint ( "Invalid password in config, no password is used\n" );
        }
	}

    if ( m_pMainConfig->GetAdminServerEnabled () )
    {
        unsigned int uiPort = m_pMainConfig->GetAdminPort ();
    }

    // If ASE is enabled
    if ( m_pMainConfig->GetASEEnabled () )
    {
		m_pASE = new ASE ( m_pMainConfig, m_pPlayerManager, static_cast < int > ( usServerPort ), szServerIP );

        if ( m_pMainConfig->GetSerialVerificationEnabled () )
            m_pASE->SetRuleValue ( "SerialVerification", "yes" );
		// Query Wojjie's game-monitor.com
        CTCPImpl * pTCP = new CTCPImpl;
        pTCP->Initialize ();

		char szURL[256] = { '\0' };
        CLogger::LogPrint ( "Querying game-monitor.com master server... " );
		sprintf ( szURL, QUERY_URL_GAME_MONITOR, usServerPort + 123);

		CHTTPRequest * request = new CHTTPRequest ( szURL );
        CHTTPResponse * response = request->Send ( pTCP );
        if ( !response )
			CLogger::LogPrintfNoStamp ( "failed! (Not available)\n" );
		else if ( response->GetErrorCode () != 200 )
			CLogger::LogPrintfNoStamp ( "failed! (%u: %s)\n", response->GetErrorCode (), response->GetErrorDescription () );
		else
			CLogger::LogPrintfNoStamp ( "success!\n");

		delete pTCP;
		delete request;
    }

    // Is the script debug log enabled?
    if ( m_pMainConfig->GetScriptDebugLogEnabled () )
    {
        if ( !m_pScriptDebugging->SetLogfile ( m_pMainConfig->GetScriptDebugLogFile ().c_str (), m_pMainConfig->GetScriptDebugLogLevel () ) )
        {
            CLogger::LogPrint ( "WARNING: Unable to open the given script debug logfile\n" );
        }
    }

	// Set the autopatcher directory
	g_pServerInterface->GetModManager ()->GetAbsolutePath ( "", szBuffer, MAX_PATH );
	g_pNetServer->SetAutoPatcherDirectory ( szBuffer );

    
#ifdef MTA_VOICE
    // Grab our voice server instance
    m_pVoiceServer = GetVoiceServerInterface ( );
    if ( m_pVoiceServer )
    {
        if ( m_pVoiceServer->Initialize ( ) == false )
        {
            CLogger::LogPrint ( "WARNING: Error initializing voice interface.  Voice won't be enabled this session.\n" );
        }

        m_pVoiceServer->Enable ( );

        CLogger::LogPrint( "Server voice module loaded.\n" );
    }
    else
    {
        CLogger::LogPrint ( "WARNING: Error trying to retrieve voice server interface.\n" );
    }
#endif

    // Run startup commands
    g_pServerInterface->GetModManager ()->GetAbsolutePath ( "autoexec.conf", szBuffer, MAX_PATH );
    CCommandFile Autoexec ( szBuffer, *m_pConsole, *m_pConsoleClient );
    if ( Autoexec.IsValid () )
    {
        CLogger::LogPrintf ( "autoexec.conf file found! Executing...\n" );
        Autoexec.Run ();
    }

    // Done
    // If you're ever going to change this message, update the "server ready" determination
    // inside CServer.cpp in deathmatch mod aswell.
    CLogger::LogPrint ( "Server started and is ready to accept connections!\n" );

    return true;
}


void CGame::Stop ( void )
{
    // Tell the log
    CLogger::LogPrint ( "Server stopped!\n" );

    // Stop our network
    g_pNetServer->StopNetwork ();

    // Unregister our packethandler
    g_pNetServer->RegisterPacketHandler ( NULL, TRUE );

#ifdef MTA_VOICE
    // Stop voice.
    if ( m_pVoiceServer )
    {
        m_pVoiceServer->Shutdown();
    }
#endif
}


bool CGame::StaticProcessPacket ( unsigned char ucPacketID, NetServerPlayerID& Socket, NetServerBitStreamInterface& BitStream )
{
    // Is it a join packet? Pass it to the handler immediately
    if ( ucPacketID == PACKET_ID_PLAYER_JOIN )
    {
        g_pGame->Packet_PlayerJoin ( Socket );
        return true;
    }

    // Is it an rpc call?
    if ( ucPacketID == PACKET_ID_RPC )
    {
        g_pGame->m_pRPCFunctions->ProcessPacket ( Socket, BitStream );
        return true;
    }

    // Translate the packet
    CPacket* pPacket = g_pGame->m_pPacketTranslator->Translate ( Socket, static_cast < ePacketID > ( ucPacketID ), BitStream );
    if ( pPacket )
    {
        // Handle it
        bool bHandled = g_pGame->ProcessPacket ( *pPacket );

        // Destroy the packet and return whether it could handle it or not
        delete pPacket;
        return bHandled;
    }

    // Failed to handle it
    return false;
}


bool CGame::ProcessPacket ( CPacket& Packet )
{
    // Can we handle it?
    ePacketID PacketID = Packet.GetPacketID ();
    switch ( PacketID )
    {
        case PACKET_ID_PLAYER_JOINDATA:
        {
            Packet_PlayerJoinData ( static_cast < CPlayerJoinDataPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_PLAYER_WASTED:
        {
            Packet_PlayerWasted ( static_cast < CPlayerWastedPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_PLAYER_QUIT:
        {
            Packet_PlayerQuit ( static_cast < CPlayerQuitPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_PLAYER_TIMEOUT:
        {
            Packet_PlayerTimeout ( static_cast < CPlayerTimeoutPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_PLAYER_PURESYNC:
        {
            Packet_PlayerPuresync ( static_cast < CPlayerPuresyncPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_PLAYER_VEHICLE_PURESYNC:
        {
            Packet_VehiclePuresync ( static_cast < CVehiclePuresyncPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_PLAYER_KEYSYNC:
        {
            Packet_Keysync ( static_cast < CKeysyncPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_DETONATE_SATCHELS:
        {
            Packet_DetonateSatchels ( static_cast < CDetonateSatchelsPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_EXPLOSION:
            Packet_ExplosionSync ( static_cast < CExplosionSyncPacket& > ( Packet ) );
            return true;

        case PACKET_ID_PROJECTILE:
            Packet_ProjectileSync ( static_cast < CProjectileSyncPacket& > ( Packet ) );
            return true;

        case PACKET_ID_COMMAND:
        {
            Packet_Command ( static_cast < CCommandPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_VEHICLE_DAMAGE_SYNC:
        {
            Packet_VehicleDamageSync ( static_cast < CVehicleDamageSyncPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_VEHICLE_INOUT:
        {
            Packet_Vehicle_InOut ( static_cast < CVehicleInOutPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_LUA_EVENT:
        {
            Packet_LuaEvent ( static_cast < CLuaEventPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_CUSTOM_DATA:
        {
            Packet_CustomData ( static_cast < CCustomDataPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_VOICE_DATA:
        {
            Packet_Voice_Data ( static_cast < CVoiceDataPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_CAMERA_SYNC:
        {
            Packet_CameraSync ( static_cast < CCameraSyncPacket& > ( Packet ) );
            return true;
        }

		default:
			break;
    }

    // Let the unoccupied vehicle sync manager handle it
    if ( m_pUnoccupiedVehicleSync->ProcessPacket ( Packet ) )
    {
        return true;
    }
    else if ( m_pPedSync->ProcessPacket ( Packet ) )
    {
        return true;
    }

    return false;
}


void CGame::JoinPlayer ( CPlayer& Player )
{
    // He's joined now
    Player.SetStatus ( STATUS_JOINED );

    // Console
    char szIP [64];
    Player.GetSourceIP ( szIP );
    CLogger::LogPrintf ( "JOIN: %s joined the game (IP: %s)\n", Player.GetNick (), szIP );

    // Set the root element as his parent
    // NOTE: Make sure he doesn't get any entities sent to him because we're sending him soon
    Player.SetDoNotSendEntities ( true );
    Player.SetParentObject ( m_pMapManager->GetRootElement () );
    Player.SetDoNotSendEntities ( false );

    // Let him join
    Player.Send ( CPlayerJoinCompletePacket ( Player.GetID (),
                                              m_pPlayerManager->Count (),
                                              m_pMapManager->GetRootElement ()->GetID (),
                                              m_pMainConfig->GetHTTPDownloadType (),
                                              m_pMainConfig->GetHTTPPort (),
                                              m_pMainConfig->GetHTTPDownloadURL ().c_str () ) );

    // Tell the other players about him
    CPlayerListPacket PlayerNotice;
    PlayerNotice.AddPlayer ( &Player );
    PlayerNotice.SetShowInChat ( true );
    m_pPlayerManager->BroadcastOnlyJoined ( PlayerNotice, &Player );

    // Add this player to the voice list.
#ifdef MTA_VOICE
    if ( m_pVoiceServer )
    {
        m_pVoiceServer->AddClient ( Player.GetID ( ) );
    }
#endif

    // Tell the map manager
    m_pMapManager->OnPlayerJoin ( Player );	// This sends the elements that are needed before the resources start

	// Tell the resource manager
    m_pResourceManager->OnPlayerJoin ( Player );

    // Write all players connected right now to a playerlist packet except the one we got the ingame notice from
    CPlayerListPacket PlayerList;
    PlayerList.SetShowInChat ( false );
    list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
    {
        if ( &Player != *iter && (*iter)->IsJoined () && !(*iter)->IsBeingDeleted () )
        {
            PlayerList.AddPlayer ( *iter ); 
        }
    }

    // Send it to the player we got this ingame notice from
    Player.Send ( PlayerList );

    // Send him element data of all the other players
    CEntityAddPacket EntityPacket;
    iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd () ; iter++ )
    {
        CPlayer * pPlayer = *iter;
        // He doesn't need to know about himself
        if ( pPlayer != &Player )
        {
            EntityPacket.Add ( pPlayer );
        }
    }
    Player.Send ( EntityPacket );

    // Tell him about the blips (needs to be done after all the entities it can be attached to)
    m_pMapManager->SendBlips ( Player );

    // Send him the current info of the current players ( stats, clothes, etc )
	iter = m_pPlayerManager->IterBegin ();
	for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
	{
		if ( &Player != *iter && (*iter)->IsJoined () && (*iter)->IsSpawned () )
		{
			float* usStats = (*iter)->GetPlayerStats ();
			unsigned short count = 0;
			CPlayerStatsPacket PlayerStats;
            PlayerStats.SetSourceElement ( *iter );
			for ( unsigned short us = 0 ; us < NUM_PLAYER_STATS ; us++ )
			{
                if ( usStats [ us ] != 0 )
				{
					PlayerStats.Add ( us, usStats [ us ] );
					count++;
				}
			}
			if ( count > 0 )
				Player.Send ( PlayerStats );

            count = 0;
            CPlayerClothesPacket PlayerClothes;
            PlayerClothes.SetSourceElement ( *iter );
            PlayerClothes.Add ( (*iter)->GetClothes () );
            if ( PlayerClothes.Count () > 0 )
                Player.Send ( PlayerClothes );            
		}
	}

    // Tell our scripts the player has joined
    CLuaArguments Arguments;
    Player.CallEvent ( "onPlayerJoin", Arguments );

    // If auto-login is enabled, try and log him in
    if ( m_pAccountManager->IsAutoLoginEnabled () )
    {
        char szIP [ 25 ];
        Player.GetSourceIP ( szIP );
        CAccount* pAccount = m_pAccountManager->Get ( Player.GetNick (), szIP );
        if ( pAccount )
        {
            m_pAccountManager->LogIn ( &Player, &Player, pAccount, true );
        }
    }
}


void CGame::QuitPlayer ( CPlayer& Player, CClient::eQuitReasons Reason, bool bSayInConsole )
{
    // Grab quit reaason
    char* szReason = "Unknown";
    switch ( Reason )
    {
        case CClient::QUIT_QUIT: szReason = "Quit"; break;
        case CClient::QUIT_KICK: szReason = "Kicked"; break;
        case CClient::QUIT_BAN: szReason = "Banned"; break;
        case CClient::QUIT_CONNECTION_DESYNC: szReason = "Bad Connection"; break;
        case CClient::QUIT_TIMEOUT: szReason = "Timed out"; break;
    }

    // Remove the player from the serial manager
    if ( m_pMainConfig->GetSerialVerificationEnabled () )
        m_SerialManager.Remove ( &Player );

    // Output
    const char* szNick = Player.GetNick ();
    if ( bSayInConsole && szNick && szNick [0] )
    {
        CLogger::LogPrintf ( "QUIT: %s left the game [%s]\n", szNick, szReason );
    }

    // If he had joined
    if ( Player.GetStatus () == STATUS_JOINED )
    {
        // Tell our scripts the player has quit, but only if the scripts got told he joined
        CLuaArguments Arguments;
        Arguments.PushString ( szReason );
        Player.CallEvent ( "onPlayerQuit", Arguments );

        // Tell the map manager
        m_pMapManager->OnPlayerQuit ( Player );

        // Tell all the players except the parting one (we don't tell them if he hadn't joined because the players don't know about him)
        CPlayerQuitPacket Packet;
        Packet.SetPlayer ( Player.GetID () );
        Packet.SetQuitReason ( static_cast < unsigned char > ( Reason ) );
        m_pPlayerManager->BroadcastOnlyJoined ( Packet, &Player );

        #ifdef MTA_VOICE
            // Remove from voice system.
            if ( m_pVoiceServer )
            {
                m_pVoiceServer->DeleteClient ( Player.GetID ( ) );
            }
        #endif
    }

    // Delete it, don't unlink yet, we could be inside the player-manager's iteration
    m_ElementDeleter.Delete ( &Player, false );
}


void CGame::AddBuiltInEvents ( void )
{
	// Resource events
	m_Events.AddEvent ( "onResourceStart", "resource", NULL, false );
	m_Events.AddEvent ( "onResourceStop", "resource", NULL, false );

    // Blip events

    // Marker events
    m_Events.AddEvent ( "onMarkerHit", "player, matchingDimension", NULL, false );
	m_Events.AddEvent ( "onMarkerLeave", "player, matchingDimension", NULL, false );

    // Object events

    // Pickup events
    m_Events.AddEvent ( "onPickupHit", "player, matchingDimension", NULL, false );
    m_Events.AddEvent ( "onPickupUse", "player", NULL, false );
    m_Events.AddEvent ( "onPickupSpawn", "", NULL, false );

    // Player events
	m_Events.AddEvent ( "onPlayerConnect", "player", NULL, false );
    m_Events.AddEvent ( "onPlayerChat", "text", NULL, false );
    m_Events.AddEvent ( "onPlayerDamage", "attacker, weapon, bodypart, loss", NULL, false );
    m_Events.AddEvent ( "onPlayerVehicleEnter", "vehicle, seat, jacked", NULL, false );
    m_Events.AddEvent ( "onPlayerVehicleExit", "vehicle, reason, jacker", NULL, false );
    m_Events.AddEvent ( "onPlayerJoin", "", NULL, false );
    m_Events.AddEvent ( "onPlayerQuit", "reason", NULL, false );
    m_Events.AddEvent ( "onPlayerSpawn", "spawnpoint, team", NULL, false );
    m_Events.AddEvent ( "onPlayerTarget", "target", NULL, false );
    m_Events.AddEvent ( "onPlayerWasted", "ammo, killer, weapon, bodypart", NULL, false );
    m_Events.AddEvent ( "onPlayerWeaponSwitch", "previous, current", NULL, false );
    m_Events.AddEvent ( "onPlayerMarkerHit", "marker, matchingDimension", NULL, false );
	m_Events.AddEvent ( "onPlayerMarkerLeave", "marker, matchingDimension", NULL, false );
    m_Events.AddEvent ( "onPlayerPickupHit", "pickup, matchingDimension", NULL, false );
    m_Events.AddEvent ( "onPlayerPickupUse", "pickup", NULL, false );
    m_Events.AddEvent ( "onPlayerClick", "button, state, element, posX, posY, posZ", NULL, false );
    m_Events.AddEvent ( "onPlayerContact", "previous, current", NULL, false );
    m_Events.AddEvent ( "onPlayerBan", "ban", NULL, false );
    m_Events.AddEvent ( "onPlayerLogin", "guest_account, account, auto-login", NULL, false );
    m_Events.AddEvent ( "onPlayerLogout", "account, guest_account", NULL, false );
	m_Events.AddEvent ( "onPlayerChangeNick", "oldnick, newnick", NULL, false );

    // Element events
    m_Events.AddEvent ( "onElementColShapeHit", "colshape, matchingDimension", NULL, false );
    m_Events.AddEvent ( "onElementColShapeLeave", "colshape, matchingDimension", NULL, false );
    m_Events.AddEvent ( "onElementClicked", "button, state, clicker, posX, posY, posZ", NULL, false );
    m_Events.AddEvent ( "onElementDataChange", "key, oldValue", NULL, false );
	m_Events.AddEvent ( "onElementDestroy", "", NULL, false );

    // Radar area events

    // Shape events
    m_Events.AddEvent ( "onColShapeHit", "entity, matchingDimension", NULL, false );
    m_Events.AddEvent ( "onColShapeLeave", "entity, matchingDimension", NULL, false );

    // Vehicle events
    m_Events.AddEvent ( "onVehicleDamage", "loss", NULL, false );
    m_Events.AddEvent ( "onVehicleRespawn", "exploded", NULL, false );
    m_Events.AddEvent ( "onTrailerAttach", "towedBy", NULL, false );
    m_Events.AddEvent ( "onTrailerDetach", "towedBy", NULL, false );
    m_Events.AddEvent ( "onVehicleStartEnter", "player, seat, jacked", NULL, false );
    m_Events.AddEvent ( "onVehicleStartExit", "player, seat, jacker", NULL, false );
    m_Events.AddEvent ( "onVehicleEnter", "player, seat, jacked", NULL, false );
    m_Events.AddEvent ( "onVehicleExit", "player, seat, jacker", NULL, false );
    m_Events.AddEvent ( "onVehicleExplode", "", NULL, false );

    // Console events
    m_Events.AddEvent ( "onConsole", "text", NULL, false );

	// Ban events
	m_Events.AddEvent ( "onBan", "ip", NULL, false );
	m_Events.AddEvent ( "onUnban", "ip", NULL, false );
}


void CGame::SendPings ( void )
{
    #define SEND_PING_CYCLE_RATE 3000

    // Time to send pings?
    unsigned int uiPlayerCount = m_pPlayerManager->Count ();
    unsigned long ulCurrentTime = GetTime ();
    if ( uiPlayerCount > 0 && ulCurrentTime >= m_ulNextPingTime )
    {
        // Grab the next player id
        CPlayer* pSendPlayer = m_pPlayerManager->GetAfter ( m_LastPingPlayer );
        if ( pSendPlayer )
        {
            // Set this player as the last player we sent the pings to
            m_LastPingPlayer = pSendPlayer->GetID ();

            // Make sure he's joined
            if ( pSendPlayer->IsJoined () )
            {
                // Create the ping packet and add all players to it
                CPlayerPingsPacket PingsPacket;
                CPlayer* pPlayer;
                list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
                for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
                {
                    // Add him if he's joined
                    pPlayer = *iter;
                    if ( pPlayer->IsJoined () )
                    {
                        PingsPacket.AddPlayer ( pPlayer );
                    }
                }

                // Send the packet
                pSendPlayer->Send ( PingsPacket );
            }
        }

        // Calculate when to send the next ping
        m_ulNextPingTime = ulCurrentTime + SEND_PING_CYCLE_RATE / uiPlayerCount;
    }
}


void CGame::Packet_PlayerJoin ( NetServerPlayerID& Source )
{
    // Reply with the mod this server is running
    NetServerBitStreamInterface* pBitStream = g_pNetServer->AllocateNetServerBitStream ();
    if ( pBitStream )
    {
        // Write the mod name to the bitstream
        pBitStream->Write ( const_cast < char* > ( "deathmatch" ), 10 );

        // Send and destroy the bitstream
        g_pNetServer->SendPacket ( PACKET_ID_MOD_NAME, Source, pBitStream );
        g_pNetServer->DeallocateNetServerBitStream ( pBitStream );
    }
}


void CGame::Packet_PlayerJoinData ( CPlayerJoinDataPacket& Packet )
{
	// Grab the nick
    const char* szNick = Packet.GetNick ();
    char szNickTemp [32];
    if ( szNick && szNick [0] != 0 )
    {
        // Valid?
        strncpy ( szNickTemp, szNick, sizeof ( szNickTemp ) );
        szNickTemp [ 31 ] = 0;
        szNick = SanityCheckNick ( szNickTemp );

        // Is the server passworded?
        bool bPasswordIsValid = true;
        if ( m_pMainConfig->HasPassword () )
        {
            // Grab the password hash from the packet
            const MD5& PacketHash = Packet.GetPassword ();

            // Hash our password
            std::string strPassword = m_pMainConfig->GetPassword ();
            MD5 ConfigHash;
            CMD5Hasher Hasher;
            if ( !strPassword.empty () && Hasher.Calculate ( strPassword.c_str (), strPassword.length (), ConfigHash ) )
            {
                // Compare the hashes
                if ( memcmp ( &ConfigHash, &PacketHash, sizeof ( MD5 ) ) != 0 )
                {
                    bPasswordIsValid = false;
                }
            }
        }

        // Add the player
        CPlayer* pPlayer = m_pPlayerManager->Create ( Packet.GetSourceSocket () );
        if ( pPlayer )
        {
            if ( !CheckNickProvided ( szNickTemp ) ) // check the nick is valid
            {
                // Tell the console
                char szIP [22];
                CLogger::LogPrintf ( "CONNECT: %s failed to connect (IP: %s - Invalid Nick)\n", szNick, pPlayer->GetSourceIP ( szIP ) );

                // Tell the player the password was wrong
                DisconnectPlayer ( this, *pPlayer, "Disconnected: Invalid Nick" );
                return;
            }

            // Check the size of the nick
            size_t sizeNick = strlen ( szNick );
            if ( sizeNick >= MIN_NICK_LENGTH && sizeNick <= MAX_NICK_LENGTH )
            {
                // Someone here with the same name?
                CPlayer* pTempPlayer = m_pPlayerManager->Get ( szNick );
                if ( pTempPlayer )
                {
                    char szIP [ 22 ], szTempIP [ 22 ];
                    pPlayer->GetSourceIP ( szIP );
                    pTempPlayer->GetSourceIP ( szTempIP );
                    // Same person? Quit the first and let this one join
                    if ( strcmp ( szIP, szTempIP ) == 0 )
                    {
                        QuitPlayer ( *pTempPlayer, CClient::QUIT_QUIT );
                        pTempPlayer = NULL;
                    }
                }
                if ( pTempPlayer == NULL )
                {
                    // Correct version?
                    if ( Packet.GetNetVersion () == MTA_DM_NETCODE_VERSION )
                    {
                        // If the password is valid
                        if ( bPasswordIsValid )
                        {
                            // If he's not join flooding
                            if ( !m_pMainConfig->GetJoinFloodProtectionEnabled () || !m_ConnectHistory.IsFlooding ( Packet.GetSourceIP() ) )
                            {
								// Set the nick and the game version
								pPlayer->SetNick ( szNick );
								pPlayer->SetGameVersion ( Packet.GetGameVersion () );
								pPlayer->SetMTAVersion ( Packet.GetMTAVersion () );
								pPlayer->SetSerialUser ( Packet.GetSerialUser () );

                                // Get the serial number from the packet source
                                NetServerPlayerID p = Packet.GetSourceSocket ();
                                std::string strSerial;
                                p.GetSerial ( strSerial );
								pPlayer->SetSerial ( strSerial );

								// Check the serial for validity
                                if ( !pPlayer->GetSerial ().empty() &&
                                     m_pBanManager->IsSerialBanned ( pPlayer->GetSerial ().c_str () ) )
								{
									// Tell the console
									CLogger::LogPrintf ( "CONNECT: %s failed to connect (Serial is banned)\n", szNick );

									// Tell the player he's banned
									DisconnectPlayer ( this, *pPlayer, "Disconnected: Serial is banned" );
									return;
								}

                                if ( !pPlayer->GetSerialUser ().empty() &&
                                     m_pBanManager->IsAccountBanned ( pPlayer->GetSerialUser ().c_str () ) )
								{
									// Tell the console
									CLogger::LogPrintf ( "CONNECT: %s failed to connect (Account is banned)\n", szNick );

									// Tell the player he's banned
									DisconnectPlayer ( this, *pPlayer, "Disconnected: Account is banned" );
									return;
								}

								// Add him to the whowas list
								m_WhoWas.Add ( szNick, Packet.GetSourceIP (), Packet.GetSourcePort () );

                                // Verify the player's serial if necessary
                                if ( m_pMainConfig->GetSerialVerificationEnabled () )
                                    m_SerialManager.Verify ( pPlayer, PlayerCompleteConnect );
                                else
								    PlayerCompleteConnect ( pPlayer, true, NULL );
                            }
                            else
                            {
                                // Tell the console
                                char szIP [22];
                                CLogger::LogPrintf ( "CONNECT: %s failed to connect (IP: %s - Join flood)\n", szNick, pPlayer->GetSourceIP ( szIP ) );

                                // Tell the player the password was wrong
                                DisconnectPlayer ( this, *pPlayer, "Disconnected: Join flood. Please wait a minute, then reconnect." );
                            }
                        }
                        else
                        {
                            // Tell the console
                            char szIP [22];
                            CLogger::LogPrintf ( "CONNECT: %s failed to connect (IP: %s - Wrong password)\n", szNick, pPlayer->GetSourceIP ( szIP ) );

                            // Tell the player the password was wrong
                            DisconnectPlayer ( this, *pPlayer, "Disconnected: Incorrect password" );
                        }
                    }
                    else
                    {
                        // Tell the console
                        char szIP [22];
                        CLogger::LogPrintf ( "CONNECT: %s failed to connect (IP: %s - Bad version)\n", szNick, pPlayer->GetSourceIP ( szIP ) );

                        // Tell the player that the version was wrong
                        char szReturn [128];
                        _snprintf ( szReturn, 128, "Disconnected: Bad version (client: %X, server: %X)\n", Packet.GetNetVersion (), MTA_DM_NETCODE_VERSION );
                        szReturn [127] = '\0';
                        DisconnectPlayer ( this, *pPlayer, szReturn );
                    }
                }
                else
                {
                    // Tell the console
                    char szIP [22];
                    CLogger::LogPrintf ( "CONNECT: %s failed to connect (IP: %s - Nick already in use)\n", szNick, pPlayer->GetSourceIP ( szIP ) );

                    // Tell the player the password was wrong
                    DisconnectPlayer ( this, *pPlayer, "Disconnected: Nick already in use" );
                }
            }
            else
            {
                // Tell the console
                char szIP [22];
                CLogger::LogPrintf ( "CONNECT: %s failed to connect (Invalid nickname)\n", pPlayer->GetSourceIP ( szIP ) );

                // Tell the player the password was wrong
                DisconnectPlayer ( this, *pPlayer, "Disconnected: Invalid nickname" );
            }
        }
    }
}


void CGame::Packet_PlayerWasted ( CPlayerWastedPacket& Packet )
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () && !pPlayer->IsDead () )
    {       
        // Update the player class
        pPlayer->SetSpawned ( false );
        pPlayer->SetIsDead ( true );
        pPlayer->SetPosition ( Packet.m_vecPosition );

        // Remove him from any occupied vehicle
        pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
        CVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            pVehicle->SetOccupant ( NULL, pPlayer->GetOccupiedVehicleSeat () );
            pPlayer->SetOccupiedVehicle ( NULL, 0 );
        }

        CElement * pKiller = ( Packet.m_Killer != INVALID_ELEMENT_ID ) ? CElementIDs::GetElement ( Packet.m_Killer ) : NULL;

        // Create a new packet to send to everyone
        CPlayerWastedPacket ReturnWastedPacket ( pPlayer, pKiller, Packet.m_ucKillerWeapon, Packet.m_ucBodyPart, false, Packet.m_AnimGroup, Packet.m_AnimID );
        m_pPlayerManager->BroadcastOnlyJoined ( ReturnWastedPacket );

        // Tell our scripts the player has died
        CLuaArguments Arguments;
        Arguments.PushNumber ( Packet.m_usAmmo );
        if ( pKiller ) Arguments.PushElement ( pKiller );
        else Arguments.PushBoolean ( false );
        if ( Packet.m_ucKillerWeapon != 0xFF ) Arguments.PushNumber ( Packet.m_ucKillerWeapon );
        else Arguments.PushBoolean ( false );
        if ( Packet.m_ucBodyPart != 0xFF ) Arguments.PushNumber ( Packet.m_ucBodyPart );
        else Arguments.PushBoolean ( false );
        Arguments.PushBoolean ( false ); 
        pPlayer->CallEvent ( "onPlayerWasted", Arguments );
    }
}


void CGame::Packet_PlayerQuit ( CPlayerQuitPacket& Packet )
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer )
    {
        // Quit him
        QuitPlayer ( *pPlayer );
    }
}


void CGame::Packet_PlayerTimeout ( CPlayerTimeoutPacket& Packet )
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer )
    {
        // Quit him
        QuitPlayer ( *pPlayer, CClient::QUIT_TIMEOUT );
    }
}


void CGame::Packet_PlayerPuresync ( CPlayerPuresyncPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Ignore this packet if he should be in a vehicle
        if ( !pPlayer->GetOccupiedVehicle () )
        {
            // Send a returnsync packet to the player that sent it
            pPlayer->Send ( CReturnSyncPacket ( pPlayer ) );

            // Grab current time
            unsigned long ulTimeNow = GetTime ();

            // Loop through all our players
            CPlayer* pSendPlayer;
            list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
            for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
            {
                // Not the local player?
                pSendPlayer = *iter;
                if ( pSendPlayer != pPlayer )
                {
                    // Should we send it to this player? This returns false if the
                    // distance between the players is so great that the sync is not
                    // neccessary that often.
                    if ( pSendPlayer->IsTimeToSendSyncFrom ( *pPlayer, ulTimeNow ) )
                    {
                        // Send it.
                        pSendPlayer->Send ( Packet );
                    }
                }
            }

            // Run colpoint checks
            m_pColManager->DoHitDetection ( pPlayer->GetLastPosition (), pPlayer->GetPosition (), 0.0f, pPlayer );  
        }
    }
}


void CGame::Packet_Command ( CCommandPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Tell the console
        m_pConsole->HandleInput ( Packet.GetCommand (), pPlayer, pPlayer );
    }
}


void CGame::Packet_VehicleDamageSync ( CVehicleDamageSyncPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Grab the vehicle
        CElement* pVehicleElement = CElementIDs::GetElement ( Packet.m_Vehicle );
        if ( pVehicleElement && IS_VEHICLE ( pVehicleElement ) )
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pVehicleElement );

            // Is this guy the driver or syncer?
            if ( pVehicle->GetSyncer () == pPlayer || pVehicle->GetOccupant ( 0 ) == pPlayer )
            {
                // Set the new damage model
                memcpy ( pVehicle->m_ucDoorStates, Packet.m_ucDoorStates, MAX_DOORS );
                memcpy ( pVehicle->m_ucWheelStates, Packet.m_ucWheelStates, MAX_WHEELS );
                memcpy ( pVehicle->m_ucPanelStates, Packet.m_ucPanelStates, MAX_PANELS );
                memcpy ( pVehicle->m_ucLightStates, Packet.m_ucLightStates, MAX_LIGHTS );

                // Broadcast the packet to everyone
                m_pPlayerManager->BroadcastOnlyJoined ( Packet, pPlayer );
            }
        }
    }
}


void CGame::Packet_VehiclePuresync ( CVehiclePuresyncPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Grab the vehicle
        CVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            // Send a returnsync packet to the player that sent it
            pPlayer->Send ( CReturnSyncPacket ( pPlayer ) );

            // Grab current time
            unsigned long ulTimeNow = GetTime ();

            // Loop through all our players
            CPlayer* pSendPlayer;
            list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
            for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
            {
                // Not the local player?
                pSendPlayer = *iter;
                if ( pSendPlayer != pPlayer )
                {
                    // Should we send it to this player? This returns false if the
                    // distance between the players is so great that the sync is not
                    // neccessary that often.
                    if ( pSendPlayer->IsTimeToSendSyncFrom ( *pPlayer, ulTimeNow ) )
                    {
                        // Send it.
                        pSendPlayer->Send ( Packet );
                    }
                }
            }

            // Run colpoint checks
            m_pColManager->DoHitDetection ( pPlayer->GetLastPosition (), pPlayer->GetPosition (), 0.0f, pPlayer );
            m_pColManager->DoHitDetection ( pVehicle->GetLastPosition (), pVehicle->GetPosition (), 0.0f, pVehicle );  
        }
    }
}


void CGame::Packet_Keysync ( CKeysyncPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Grab the position of the source player
        const CVector& vecSourcePosition = pPlayer->GetPosition ();

        // Loop through all the players
        CPlayer* pSendPlayer;
        std::list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
        for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
        {
            pSendPlayer = *iter;

            // Not the player we got the packet from?
            if ( pSendPlayer != pPlayer )
            {
                // Grab this player's camera position
                CVector vecCameraPosition;
                pSendPlayer->GetCamera ()->GetPosition ( vecCameraPosition );

                // Is this players camera close enough to warrant keysync?
                if ( IsPointNearPoint3D ( vecSourcePosition, vecCameraPosition, MAX_KEYSYNC_DISTANCE ) )
                {
                    // Send the packet to him
                    pSendPlayer->Send ( Packet );
                }
            }
        }
    }
}


void CGame::Packet_LuaEvent ( CLuaEventPacket& Packet )
{
    // Grab the source player, even name, element id and the arguments passed
	CPlayer* pCaller = Packet.GetSourcePlayer ();
    char* szName = Packet.GetName ();
    ElementID ElementID = Packet.GetElementID ();
    CLuaArguments Arguments = Packet.GetArguments ();

    // Grab the element
    CElement* pElement = CElementIDs::GetElement ( ElementID );
    if ( pElement )
    {
        // Make sure the event exists and that it allows clientside triggering
        SEvent* pEvent = m_Events.Get ( szName );
        if ( pEvent )
        {
            if ( pEvent->bAllowRemoteTrigger )
            {
                pElement->CallEvent ( szName, Arguments, pCaller );
            }
            else
                m_pScriptDebugging->LogError ( NULL, "Client triggered serverside event %s, but event is not marked as remotly triggerable", szName );
        }
        else
            m_pScriptDebugging->LogError ( NULL, "Client triggered serverside event %s, but event is not added serverside", szName );
    }
}

void CGame::Packet_CustomData ( CCustomDataPacket& Packet )
{
    // Got a valid source?
    CPlayer* pSourcePlayer = Packet.GetSourcePlayer ();
    if ( pSourcePlayer )
    {
        // Grab the element
        ElementID ID = Packet.GetElementID ();
        CElement* pElement = CElementIDs::GetElement ( ID );
        if ( pElement )
        {
            // Change the data
            const char* szName = Packet.GetName ();
            CLuaArgument& Value = Packet.GetValue ();
            pElement->SetCustomData ( szName, Value, NULL );

            // Tell our clients to update their data. Send to everyone but the one we got this packet from.
            unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szName ) );
            CBitStream BitStream;
            BitStream.pBitStream->Write ( ID );
            BitStream.pBitStream->Write ( usNameLength );
            BitStream.pBitStream->Write ( szName, usNameLength );
            Value.WriteToBitStream ( *BitStream.pBitStream );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ELEMENT_DATA, *BitStream.pBitStream ), pSourcePlayer );
        }
    }
}


void CGame::Packet_DetonateSatchels ( CDetonateSatchelsPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        if ( pPlayer->IsSpawned () && !pPlayer->IsDead () )
        {
            // Tell everyone to blow up this guy's satchels
            m_pPlayerManager->BroadcastOnlyJoined ( Packet );
        }
    }
}


void CGame::Packet_ExplosionSync ( CExplosionSyncPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        bool bBroadcast = true;
        ElementID OriginID = Packet.m_OriginID;
        unsigned char ucType = Packet.m_ucType;
        CVector vecPosition = Packet.m_vecPosition;
        if ( OriginID != INVALID_ELEMENT_ID )
        {
            CElement * pOrigin = CElementIDs::GetElement ( OriginID );
            // Do we have an origin source?
            if ( pOrigin )
            {
                // Is the source of the explosion a vehicle?
                switch ( pOrigin->GetType () )
                {
                    case CElement::PLAYER:
                    {    
                        // Correct our position vector
                        CVehicle * pVehicle = static_cast < CPlayer * > ( pOrigin )->GetOccupiedVehicle ();
                        if ( pVehicle )
                        {
                            // Use the vehicle's position?
                            vecPosition += pVehicle->GetPosition ();
                        }
                        else
                        {
                            // Use the player's position
                            vecPosition += pOrigin->GetPosition ();
                        }
                        break;
                    }
                    case CElement::VEHICLE:
                    {
                        // Correct our position vector
                        vecPosition += pOrigin->GetPosition ();

                        // Has the vehicle blown up?
                        switch ( ucType )
                        {
                            case 4: // EXP_TYPE_CAR
                            case 5: // EXP_TYPE_CAR_QUICK
                            case 6: // EXP_TYPE_BOAT
                            case 7: // EXP_TYPE_HELI
                            {
                                CVehicle * pVehicle = static_cast < CVehicle * > ( pOrigin );
                                // Is this vehicle not already blown?
                                if ( pVehicle->GetBlowTime () == 0 )
                                {
                                    pVehicle->SetBlowTime ( GetTime () );

                                    // Call the onVehicleExplode event
                                    CLuaArguments Arguments;
                                    pVehicle->CallEvent ( "onVehicleExplode", Arguments );
                                }
                                else
                                {
                                    bBroadcast = false;
                                }
                            }
                        }
                        break;
                    }
                    default: break;
                }
            }
        }

        if ( bBroadcast )
        {
            // Loop through all the players
            CPlayer* pSendPlayer;
            std::list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
            for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
            {
                pSendPlayer = *iter;

                /* We now tell the reporter to create the explosion too!
                // Not the player we got the packet from?
                if ( pSendPlayer != pPlayer )
                */
                {
                    // Grab this player's camera position
                    CVector vecCameraPosition;
                    pSendPlayer->GetCamera ()->GetPosition ( vecCameraPosition );

                    // Is this players camera close enough to send?
                    if ( IsPointNearPoint3D ( vecPosition, vecCameraPosition, MAX_EXPLOSION_SYNC_DISTANCE ) )
                    {
                        // Send the packet to him
                        pSendPlayer->Send ( Packet );
                    }
                }
            }
        }
    }
}


void CGame::Packet_ProjectileSync ( CProjectileSyncPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        CVector vecPosition = Packet.m_vecOrigin;
        if ( Packet.m_OriginID != INVALID_ELEMENT_ID )
        {
            CElement * pOriginSource = CElementIDs::GetElement ( Packet.m_OriginID );
            vecPosition += pOriginSource->GetPosition ();
        }

        // Loop through all the players
        CPlayer* pSendPlayer;
        std::list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
        for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
        {
            pSendPlayer = *iter;

            // Not the player we got the packet from?
            if ( pSendPlayer != pPlayer )
            {
                // Grab this player's camera position
                CVector vecCameraPosition;
                pSendPlayer->GetCamera ()->GetPosition ( vecCameraPosition );

                // Is this players camera close enough to send?
                if ( IsPointNearPoint3D ( vecPosition, vecCameraPosition, MAX_PROJECTILE_SYNC_DISTANCE ) )
                {
                    // Send the packet to him
                    pSendPlayer->Send ( Packet );
                }
            }
        }
    }
}


void CGame::Packet_Vehicle_InOut ( CVehicleInOutPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer )
    {
        // Joined?
        if ( pPlayer->IsJoined () )
        {
            // Grab the vehicle with the chosen ID
            ElementID ID = Packet.GetID ();
            unsigned char ucAction = Packet.GetAction ();

            // Spawned?
            if ( pPlayer->IsSpawned () )
            {      
                CElement* pVehicleElement = CElementIDs::GetElement ( ID );
                if ( pVehicleElement && IS_VEHICLE ( pVehicleElement ) )
                {
                    CVehicle* pVehicle = static_cast < CVehicle* > ( pVehicleElement );

                    // Handle it depending on the action
                    switch ( ucAction )
                    {
                        // Vehicle get in request?
                        case VEHICLE_REQUEST_IN:
                        {
                            bool bFailed = true;
                            enum eFailReasons
                            {
                                FAIL_INVALID = 0,
                                FAIL_SCRIPT,
                                FAIL_SCRIPT_2,
                                FAIL_JACKED_ACTION,
                                FAIL_SEAT,
                                FAIL_DISTANCE,
                                FAIL_IN_VEHICLE,
                                FAIL_ACTION,
                                FAIL_TRAILER,
                            } failReason = FAIL_INVALID;

                            // Is this vehicle enterable? (not a trailer)
                            unsigned short usVehicleModel = pVehicle->GetModel ();
                            if ( !CVehicleManager::IsTrailer ( usVehicleModel ) )
                            {
                                // He musn't already be doing something
                                if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_NONE )
                                {
                                    // He musn't already be in a vehicle
                                    if ( !pPlayer->GetOccupiedVehicle () )
                                    {  
                                        float fCutoffDistance = 50.0f;
                                        bool bWarpIn = false;
                                        // Jax: is he in water and trying to get in a floating vehicle
                                        if ( pPlayer->IsInWater () && ( usVehicleModel == VT_SKIMMER ||
                                                                        usVehicleModel == VT_SEASPAR ||
                                                                        usVehicleModel == VT_LEVIATHN ||
                                                                        usVehicleModel == VT_VORTEX ) )
                                        {
                                            fCutoffDistance = 10.0f;
                                            bWarpIn = true;
                                        }

                                        CPed* pOccupant = pVehicle->GetOccupant ( 0 );
                                        // If he's going to be jacking this vehicle, lets make sure he's very close to it
                                        if ( pOccupant )
                                            fCutoffDistance = 10.0f;

                                        // Is he close enough to the vehicle?
                                        if ( IsPointNearPoint3D ( pPlayer->GetPosition (), pVehicle->GetPosition (), fCutoffDistance ) )
                                        {
                                            unsigned char ucSeat = Packet.GetSeat ();
                                            // Going for driver?
                                            if ( ucSeat == 0 )
                                            {
                                                // Does it already have an occupant/occupying?                                                
                                                if ( !pOccupant )
                                                {
                                                    // Mark him as entering the vehicle
                                                    pPlayer->SetOccupiedVehicle ( pVehicle, 0 );
                                                    pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_ENTERING );                                        

                                                    // Call the entering vehicle event
                                                    CLuaArguments Arguments;
                                                    Arguments.PushElement ( pPlayer );     // player
                                                    Arguments.PushNumber ( 0 );             // seat
                                                    Arguments.PushBoolean ( false );        // jacked
                                                    if ( pVehicle->CallEvent ( "onVehicleStartEnter", Arguments ) )   
                                                    {
                                                        // HACK?: check the player's vehicle-action is still the same (not warped in?)
                                                        if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_ENTERING )
                                                        {
                                                            // Force the player as the syncer of the vehicle to which they are entering
                                                            m_pUnoccupiedVehicleSync->OverrideSyncer ( pVehicle, pPlayer );

                                                            if ( bWarpIn )
                                                            {
                                                                CStaticFunctionDefinitions::WarpPedIntoVehicle ( pPlayer, pVehicle, 0 );
                                                            }
                                                            else
                                                            {
                                                                // Tell everyone he can start entering the vehicle from his current position
                                                                CVehicleInOutPacket Reply ( ID, 0, VEHICLE_REQUEST_IN_CONFIRMED );
                                                                Reply.SetSourceElement ( pPlayer );
                                                                m_pPlayerManager->BroadcastOnlyJoined ( Reply );                                                                
                                                            }
                                                            bFailed = false;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        pPlayer->SetOccupiedVehicle ( NULL, 0 );
                                                        pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
                                                        pVehicle->SetOccupant ( NULL, 0 );
                                                        failReason = FAIL_SCRIPT;
                                                    }
                                                }
                                                else
                                                {
                                                    // TODO: support jacking peds (not simple!)
                                                    // Is the jacked person a player and stationary in the car (ie not getting in or out)
                                                    if ( IS_PLAYER ( pOccupant ) && pOccupant->GetVehicleAction () == CPlayer::VEHICLEACTION_NONE )
                                                    {
                                                        // He's now jacking the car and the occupant is getting jacked
                                                        pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_JACKING );
                                                        pPlayer->SetJackingVehicle ( pVehicle );
                                                        pVehicle->SetJackingPlayer ( pPlayer );
                                                        pOccupant->SetVehicleAction ( CPlayer::VEHICLEACTION_JACKED );
                                                                                                               
                                                        // Call the entering vehicle event
                                                        CLuaArguments EnterArguments;
                                                        EnterArguments.PushElement ( pPlayer );     // player
                                                        EnterArguments.PushNumber ( 0 );             // seat
                                                        EnterArguments.PushElement ( pOccupant );   // jacked
                                                        if ( pVehicle->CallEvent ( "onVehicleStartEnter", EnterArguments ) )
                                                        {
                                                            // HACK?: check the player's vehicle-action is still the same (not warped in?)
                                                            if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_JACKING )
                                                            { 
                                                                // Call the exiting vehicle event
                                                                CLuaArguments ExitArguments;
                                                                ExitArguments.PushElement ( pOccupant );       // player
                                                                ExitArguments.PushNumber ( ucSeat );            // seat
                                                                ExitArguments.PushElement ( pPlayer );         // jacker
                                                                if ( pVehicle->CallEvent ( "onVehicleStartExit", ExitArguments ) )
                                                                {
                                                                    // HACK?: check the player's vehicle-action is still the same (not warped out?)
                                                                    if ( pOccupant->GetVehicleAction () == CPlayer::VEHICLEACTION_JACKED )
                                                                    { 
                                                                        /* Jax: we don't need to worry about a syncer if we already have and will have a driver 
                                                                        // Force the player as the syncer of the vehicle to which they are entering
                                                                        m_pUnoccupiedVehicleSync->OverrideSyncer ( pVehicle, pPlayer );
                                                                        */

                                                                        // Broadcast a jack message (tells him he can get in, but he must jack it)
                                                                        CVehicleInOutPacket Reply ( ID, 0, VEHICLE_REQUEST_JACK_CONFIRMED );
                                                                        Reply.SetSourceElement ( pPlayer );
                                                                        m_pPlayerManager->BroadcastOnlyJoined ( Reply );

                                                                        bFailed = false;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else
                                                        {
                                                            pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
                                                            pOccupant->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
                                                            failReason = FAIL_SCRIPT_2;
                                                        }                                                            
                                                    }
                                                    else
                                                        failReason = FAIL_JACKED_ACTION;
                                                }
                                            }
                                            else
                                            {
                                                CPed* pCurrentOccupant = pVehicle->GetOccupant ( ucSeat );
                                                if ( pCurrentOccupant || ucSeat > pVehicle->GetMaxPassengers () )
                                                {                                                    
                                                    // Grab a free passenger spot in the vehicle
                                                    ucSeat = pVehicle->GetFreePassengerSeat ();
                                                }
                                                if ( ucSeat <= 8 )
                                                {
                                                    // Mark him as entering the vehicle
                                                    pPlayer->SetOccupiedVehicle ( pVehicle, ucSeat );
                                                    pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_ENTERING );

                                                    // Call the entering vehicle event
                                                    CLuaArguments Arguments;
                                                    Arguments.PushElement ( pPlayer );         // player
                                                    Arguments.PushNumber ( ucSeat );            // seat
                                                    Arguments.PushBoolean ( false );            // jacked
                                                    if ( pVehicle->CallEvent ( "onVehicleStartEnter", Arguments ) )
                                                    {
                                                        // HACK?: check the player's vehicle-action is still the same (not warped in?)
                                                        if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_ENTERING )
                                                        {                        
                                                            if ( bWarpIn )
                                                            {
                                                                CStaticFunctionDefinitions::WarpPedIntoVehicle ( pPlayer, pVehicle, ucSeat );
                                                            }
                                                            else
                                                            {
                                                                // Tell everyone he can start entering the vehicle from his current position
                                                                CVehicleInOutPacket Reply ( ID, ucSeat, VEHICLE_REQUEST_IN_CONFIRMED );
                                                                Reply.SetSourceElement ( pPlayer );
                                                                m_pPlayerManager->BroadcastOnlyJoined ( Reply );

                                                                bFailed = false;
                                                            }
                                                        }
                                                    }
                                                    else
                                                    {
                                                        pPlayer->SetOccupiedVehicle ( NULL, 0 );
                                                        pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
                                                        pVehicle->SetOccupant ( NULL, ucSeat );
                                                        failReason = FAIL_SCRIPT;
                                                    }
                                                }
                                                else
                                                    failReason = FAIL_SEAT;
                                            }
                                        }
                                        else
                                            failReason = FAIL_DISTANCE;
                                    }
                                    else
                                        failReason = FAIL_IN_VEHICLE;
                                }
                                else
                                    failReason = FAIL_ACTION;
                            }
                            else
                                failReason = FAIL_TRAILER;

                            if ( bFailed )
                            {
                                // He can't get in
                                CVehicleInOutPacket Reply ( ID, 0, VEHICLE_ATTEMPT_FAILED );
                                Reply.SetSourceElement ( pPlayer );
                                Reply.SetFailReason ( ( unsigned char ) failReason );
                                // Was he too far away from the vehicle?
                                if ( failReason == FAIL_DISTANCE )
                                {
                                    // Correct the vehicles position
                                    Reply.SetCorrectVector ( pVehicle->GetPosition () );
                                }
                                pPlayer->Send ( Reply );
                            }

                            break;
                        }

                        // Vehicle in notification?
                        case VEHICLE_NOTIFY_IN:
                        {
                            // Is he entering?
                            if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_ENTERING )
                            {
                                // Is he the occupant? (he must unless the client has fucked up)
                                unsigned char ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat ();
                                if ( pPlayer == pVehicle->GetOccupant ( ucOccupiedSeat ) )
                                {
                                    // Mark him as successfully entered
                                    pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );

                                    // The vehicle is no longer idle
                                    pVehicle->SetIdleTime ( 0 );

                                    // Tell everyone he's in (they should warp him in)
                                    CVehicleInOutPacket Reply ( ID, ucOccupiedSeat, VEHICLE_NOTIFY_IN_RETURN );
                                    Reply.SetSourceElement ( pPlayer );
                                    m_pPlayerManager->BroadcastOnlyJoined ( Reply );

                                    // Call the player->vehicle event
                                    CLuaArguments Arguments;
                                    Arguments.PushElement ( pVehicle );        // vehice
                                    Arguments.PushNumber ( ucOccupiedSeat );    // seat
                                    Arguments.PushBoolean ( false );            // jacked
                                    pPlayer->CallEvent ( "onPlayerVehicleEnter", Arguments );

                                    // Call the vehicle->player event
                                    CLuaArguments Arguments2;
                                    Arguments2.PushElement ( pPlayer );        // player
                                    Arguments2.PushNumber ( ucOccupiedSeat );   // seat
                                    Arguments2.PushBoolean ( false );           // jacked
                                    pVehicle->CallEvent ( "onVehicleEnter", Arguments2 );
                                }
                            }

                            break;
                        }

                        // Vehicle in aborted notification?
                        case VEHICLE_NOTIFY_IN_ABORT:
                        {
                            // Is he entering?
                            if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_ENTERING )
                            {
                                // Is he the occupant? (he must unless the client has fucked up)
                                unsigned char ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat ();
                                if ( pPlayer == pVehicle->GetOccupant ( ucOccupiedSeat ) )
                                {
                                    // Mark that he's in no vehicle
                                    pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
                                    pPlayer->SetOccupiedVehicle ( NULL, 0 );
                                    pVehicle->SetOccupant ( NULL, ucOccupiedSeat );

                                    // Tell everyone he's in (they should warp him in)
                                    CVehicleInOutPacket Reply ( ID, ucOccupiedSeat, VEHICLE_NOTIFY_IN_ABORT_RETURN );
                                    Reply.SetSourceElement ( pPlayer );
                                    m_pPlayerManager->BroadcastOnlyJoined ( Reply );
                                }
                            }

                            break;
                        }

                        // Vehicle get out request?
                        case VEHICLE_REQUEST_OUT:
                        {
                            // Is he getting jacked?
                            if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_NONE )
                            {
                                // Does it have an occupant and is the occupant the requesting player?
                                unsigned ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat ();
                                if ( pPlayer == pVehicle->GetOccupant ( ucOccupiedSeat ) )
                                {
                                    // Call the exiting vehicle event
                                    CLuaArguments Arguments;
                                    Arguments.PushElement ( pPlayer );         // player
                                    Arguments.PushNumber ( ucOccupiedSeat );    // seat
                                    Arguments.PushBoolean ( false );            // jacked
                                    if ( pVehicle->CallEvent ( "onVehicleStartExit", Arguments ) )
                                    {
                                        // Mark him as exiting the vehicle
                                        pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_EXITING );

                                        // Tell everyone he can start exiting the vehicle
                                        CVehicleInOutPacket Reply ( ID, ucOccupiedSeat, VEHICLE_REQUEST_OUT_CONFIRMED );
                                        Reply.SetSourceElement ( pPlayer );
                                        m_pPlayerManager->BroadcastOnlyJoined ( Reply );
                                    }
                                    else
                                    {
                                        // Tell him he can't exit
                                        CVehicleInOutPacket Reply ( ID, 0, VEHICLE_ATTEMPT_FAILED );
                                        Reply.SetSourceElement ( pPlayer );
                                        pPlayer->Send ( Reply );
                                    }
                                }
                                else
                                {
                                    // Tell him he can't exit
                                    CVehicleInOutPacket Reply ( ID, 0, VEHICLE_ATTEMPT_FAILED );
                                    Reply.SetSourceElement ( pPlayer );
                                    pPlayer->Send ( Reply );
                                }
                            }
                            else
                            {
                                // Tell him he can't exit
                                CVehicleInOutPacket Reply ( ID, 0, VEHICLE_ATTEMPT_FAILED );
                                Reply.SetSourceElement ( pPlayer );
                                pPlayer->Send ( Reply );
                            }

                            break;
                        }

                        // Vehicle out notify?
                        case VEHICLE_NOTIFY_OUT:
                        {
                            // Is he already getting out?
                            if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_EXITING )
                            {
                                // Does it have an occupant and is the occupant the requesting player?
                                unsigned char ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat ();
                                if ( pPlayer == pVehicle->GetOccupant ( ucOccupiedSeat ) )
                                {
                                    // Mark the player/vehicle as empty
                                    pVehicle->SetOccupant ( NULL, ucOccupiedSeat );
                                    pPlayer->SetOccupiedVehicle ( NULL, 0 );
                                    pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );

                                    // Force the player that just left the vehicle as the syncer
                                    m_pUnoccupiedVehicleSync->OverrideSyncer ( pVehicle, pPlayer );

                                    // If it's empty, set the idle time
                                    if ( pVehicle->GetFirstOccupant () == NULL )
                                    {
                                        pVehicle->SetIdleTime ( GetTime () );
                                    }

                                    // Tell everyone he can start exiting the vehicle
                                    CVehicleInOutPacket Reply ( ID, ucOccupiedSeat, VEHICLE_NOTIFY_OUT_RETURN );
                                    Reply.SetSourceElement ( pPlayer );
                                    m_pPlayerManager->BroadcastOnlyJoined ( Reply );

                                    // Call the player->vehicle event
                                    CLuaArguments Arguments;
                                    Arguments.PushElement ( pVehicle );        // vehicle
                                    Arguments.PushNumber ( ucOccupiedSeat );    // seat
                                    Arguments.PushBoolean ( false );            // jacker
                                    pPlayer->CallEvent ( "onPlayerVehicleExit", Arguments );

                                    // Call the vehicle->player event
                                    CLuaArguments Arguments2;
                                    Arguments2.PushElement ( pPlayer );         // player
                                    Arguments2.PushNumber ( ucOccupiedSeat );    // seat
                                    Arguments2.PushBoolean ( false );            // jacker
                                    pVehicle->CallEvent ( "onVehicleExit", Arguments2 );
                                }
                            }

                            break;
                        }

                        // Vehicle out aborted notification?
                        case VEHICLE_NOTIFY_OUT_ABORT:
                        {
                            // Is he entering?
                            if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_EXITING )
                            {
                                // Is he the occupant?
                                unsigned char ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat ();
                                if ( pPlayer == pVehicle->GetOccupant ( ucOccupiedSeat ) )
                                {
                                    // Mark that he's in no vehicle
                                    pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );

                                    // Tell everyone he's in (they should warp him in)
                                    CVehicleInOutPacket Reply ( ID, ucOccupiedSeat, VEHICLE_NOTIFY_OUT_ABORT_RETURN );
                                    Reply.SetSourceElement ( pPlayer );
                                    m_pPlayerManager->BroadcastOnlyJoined ( Reply );
                                }
                            }

                            break;
                        }

                        case VEHICLE_NOTIFY_FELL_OFF:
                        {
                            // Check that the player is in the given vehicle
                            unsigned char ucOccupiedSeat = pPlayer->GetOccupiedVehicleSeat ();
                            if ( pVehicle->GetOccupant ( ucOccupiedSeat ) == pPlayer )
                            {
                                // Remove him from the vehicle
                                pPlayer->SetOccupiedVehicle ( NULL, 0 );
                                pVehicle->SetOccupant ( NULL, ucOccupiedSeat );

                                // Force the player that just left the vehicle as the syncer
                                m_pUnoccupiedVehicleSync->OverrideSyncer ( pVehicle, pPlayer );

                                // If it's empty, set the idle time
                                if ( pVehicle->GetFirstOccupant () == NULL )
                                {
                                    pVehicle->SetIdleTime ( GetTime () );
                                }

                                // Tell the other players about it
                                CVehicleInOutPacket Reply ( ID, ucOccupiedSeat, VEHICLE_NOTIFY_FELL_OFF_RETURN );
                                Reply.SetSourceElement ( pPlayer );
                                m_pPlayerManager->BroadcastOnlyJoined ( Reply );

                                // Call the player->vehicle event
                                CLuaArguments Arguments;
                                Arguments.PushElement ( pVehicle );        // vehicle
                                Arguments.PushNumber ( ucOccupiedSeat );    // seat
                                Arguments.PushBoolean ( false );            // jacker
                                pPlayer->CallEvent ( "onPlayerVehicleExit", Arguments );

                                // Call the vehicle->player event
                                CLuaArguments Arguments2;
                                Arguments2.PushElement ( pPlayer );        // player
                                Arguments2.PushNumber ( ucOccupiedSeat );    // seat
                                Arguments2.PushBoolean ( false );            // jacker
                                pVehicle->CallEvent ( "onVehicleExit", Arguments2 );
                            }

                            break;
                        }

                        case VEHICLE_NOTIFY_JACK: // Finished jacking him
                        {
                            // Is the sender jacking?
                            if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_JACKING )
                            {
                                // Grab the jacked player
                                CPed* pJacked = pVehicle->GetOccupant ( 0 );
                                if ( pJacked )
                                {
                                    // TODO! CHECK THE CAR ID
                                    // Throw the jacked player out
                                    pJacked->SetOccupiedVehicle ( NULL, 0 );
                                    pJacked->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );

                                    // Put the jacking player into it
                                    pPlayer->SetOccupiedVehicle ( pVehicle, 0 );
                                    pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
                                    pPlayer->SetJackingVehicle ( NULL );
                                    pVehicle->SetJackingPlayer ( NULL );

                                    // Tell everyone about it
                                    ElementID PlayerID = pPlayer->GetID ();
                                    ElementID JackedID = pJacked->GetID ();
                                    CVehicleInOutPacket Reply ( ID, 0, VEHICLE_NOTIFY_JACK_RETURN, PlayerID, JackedID );
                                    Reply.SetSourceElement ( pPlayer );
                                    m_pPlayerManager->BroadcastOnlyJoined ( Reply );

                                    // Execute the player->vehicle script function for the jacked player
                                    CLuaArguments ArgumentsExit;
                                    ArgumentsExit.PushElement ( pVehicle );        // vehicle
                                    ArgumentsExit.PushNumber ( 0 );                 // seat
                                    ArgumentsExit.PushElement ( pPlayer );         // jacker
                                    pJacked->CallEvent ( "onPlayerVehicleExit", ArgumentsExit );

                                    // Execute the vehicle->vehicle script function for the jacked player
                                    CLuaArguments ArgumentsExit2;
                                    ArgumentsExit2.PushElement ( pJacked );         // player
                                    ArgumentsExit2.PushNumber ( 0 );                 // seat
                                    ArgumentsExit2.PushElement ( pPlayer );         // jacker
                                    pVehicle->CallEvent ( "onVehicleExit", ArgumentsExit2 );


                                    // Execute the player->vehicle script function
                                    CLuaArguments ArgumentsEnter;
                                    ArgumentsEnter.PushElement ( pVehicle );        // vehice
                                    ArgumentsEnter.PushNumber ( 0 );                 // seat
                                    ArgumentsEnter.PushElement ( pJacked );         // jacked
                                    pPlayer->CallEvent ( "onPlayerVehicleEnter", ArgumentsEnter );

                                    // Execute the vehicle->player script function
                                    CLuaArguments ArgumentsEnter2;
                                    ArgumentsEnter2.PushElement ( pPlayer );         // player
                                    ArgumentsEnter2.PushNumber ( 0 );                 // seat
                                    ArgumentsEnter2.PushElement ( pJacked );         // jacked
                                    pVehicle->CallEvent ( "onVehicleEnter", ArgumentsEnter2 );
                                }
                                else
                                {
                                    // Put the jacking player into it
                                    pPlayer->SetOccupiedVehicle ( pVehicle, 0 );
                                    pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
                                    pPlayer->SetJackingVehicle ( NULL );
                                    pVehicle->SetJackingPlayer ( NULL );

                                    // Tell everyone about it
                                    CVehicleInOutPacket Reply ( ID, 0, VEHICLE_NOTIFY_IN_RETURN );
                                    Reply.SetSourceElement ( pPlayer );
                                    m_pPlayerManager->BroadcastOnlyJoined ( Reply );

                                    // Execute the player->vehicle script function
                                    CLuaArguments Arguments;
                                    Arguments.PushElement ( pVehicle );        // vehice
                                    Arguments.PushNumber ( 0 );                 // seat
                                    Arguments.PushBoolean ( false );            // jacked
                                    pPlayer->CallEvent ( "onPlayerVehicleEnter", Arguments );

                                    // Execute the vehicle->player script function
                                    CLuaArguments Arguments2;
                                    Arguments2.PushElement ( pPlayer );         // player
                                    Arguments2.PushNumber ( 0 );                 // seat
                                    Arguments2.PushBoolean ( false );            // jacked
                                    pVehicle->CallEvent ( "onVehicleEnter", Arguments2 );
                                }
                            }
                            
                            break;
                        }

                        case VEHICLE_NOTIFY_JACK_ABORT:
                        {
                            // Is the sender jacking?
                            if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_JACKING )
                            {
                                CPed* pJacked = pVehicle->GetOccupant ( 0 );

                                // Mark that the jacker is in no vehicle
                                pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
                                pPlayer->SetOccupiedVehicle ( NULL, 0 );
                                pPlayer->SetJackingVehicle ( NULL );
                                pVehicle->SetJackingPlayer ( NULL );

                                // Tell everyone he aborted
                                CVehicleInOutPacket Reply ( ID, 0, VEHICLE_NOTIFY_IN_ABORT_RETURN );
                                Reply.SetSourceElement ( pPlayer );
                                m_pPlayerManager->BroadcastOnlyJoined ( Reply );

                                // Did he already start jacking him? (we're trusting the client here!)
                                if ( Packet.GetStartedJacking () == 1 )
                                {
                                    // The jacked is still inside?
                                    if ( pJacked )
                                    {
                                        // Get the jacked out
                                        pJacked->SetOccupiedVehicle ( NULL, 0 );

                                        // No driver in this vehicle
                                        pVehicle->SetOccupant ( NULL, 0 );

                                        // Tell everyone to get the jacked person out 
                                        CVehicleInOutPacket JackedReply ( ID, 0, VEHICLE_NOTIFY_OUT_RETURN );
                                        JackedReply.SetSourceElement ( pJacked );
                                        m_pPlayerManager->BroadcastOnlyJoined ( JackedReply );
                                    }
                                }
                                pJacked->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );

                                // If it's empty, set the idle time
                                if ( pVehicle->GetFirstOccupant () == NULL )
                                {
                                    pVehicle->SetIdleTime ( GetTime () );
                                }
                            }

                            break;
                        }

                        default:
                        {
                            DisconnectConnectionDesync ( this, *pPlayer, 2 );
                            return;
                        }
                    }
                }
				else
				{
                    CVehicleInOutPacket Reply ( ID, 0, VEHICLE_ATTEMPT_FAILED );
                    Reply.SetSourceElement ( pPlayer );
                    pPlayer->Send ( Reply );
				}
            }
			else
			{
                CVehicleInOutPacket Reply ( ID, 0, VEHICLE_ATTEMPT_FAILED );
                Reply.SetSourceElement ( pPlayer );
                pPlayer->Send ( Reply );
			}
        }
        else
        {
            DisconnectConnectionDesync ( this, *pPlayer, 5 );
            return;
        }
    }
}


void CGame::Packet_VehicleTrailer ( CVehicleTrailerPacket& Packet )
{
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Spawned?
        if ( pPlayer->IsSpawned () )
        {
            // Grab the vehicle with the chosen ID
            ElementID ID = Packet.GetVehicle ();
            ElementID TrailerID = Packet.GetAttachedVehicle ();
            bool bAttached = Packet.GetAttached ();

            CElement* pVehicleElement = CElementIDs::GetElement ( ID );
            if ( pVehicleElement && IS_VEHICLE ( pVehicleElement ) )
            {
                CVehicle* pVehicle = static_cast < CVehicle* > ( pVehicleElement );

                pVehicleElement = CElementIDs::GetElement ( TrailerID );
                if ( pVehicleElement && IS_VEHICLE ( pVehicleElement ) )
                {
                    CVehicle* pTrailer = static_cast < CVehicle* > ( pVehicleElement );

                    // If we're attaching
                    if ( bAttached )
                    {
                        // Do we already have a trailer?
                        CVehicle* pPresentTrailer = pVehicle->GetTowedVehicle ();
                        if ( pPresentTrailer )
                        {
                            pPresentTrailer->SetTowedByVehicle ( NULL );
                            pVehicle->SetTowedVehicle ( NULL );

                            // Detach this one
                            CVehicleTrailerPacket DetachPacket ( pVehicle, pPresentTrailer, false );
                            DetachPacket.SetSourceElement ( pPlayer );
                            m_pPlayerManager->BroadcastOnlyJoined ( DetachPacket );
                        }

                        // Is our trailer already attached to something?
                        CVehicle* pPresentVehicle = pTrailer->GetTowedByVehicle ();
                        if ( pPresentVehicle )
                        {
                            pTrailer->SetTowedByVehicle ( NULL );
                            pPresentVehicle->SetTowedVehicle ( NULL );

                            // Detach from this one
                            CVehicleTrailerPacket DetachPacket ( pPresentVehicle, pTrailer, false );
                            DetachPacket.SetSourceElement ( pPlayer );
                            m_pPlayerManager->BroadcastOnlyJoined ( DetachPacket );
                        }

                        // Attach them
                        pVehicle->SetTowedVehicle ( pTrailer );
                        pTrailer->SetTowedByVehicle ( pVehicle );

                        // Make sure the un-occupied syncer of the trailer is this driver
                        m_pUnoccupiedVehicleSync->OverrideSyncer ( pTrailer, pPlayer );

                        // Broadcast this packet to everyone else
                        m_pPlayerManager->BroadcastOnlyJoined ( Packet, pPlayer );

                        // Execute the attach trailer script function
                        CLuaArguments Arguments;
                        Arguments.PushElement ( pVehicle );
                        bool bContinue = pTrailer->CallEvent ( "onTrailerAttach", Arguments );

                        if ( !bContinue )
                        {
                            // Detach them
                            CVehicleTrailerPacket DetachPacket ( pVehicle, pTrailer, false );
                            DetachPacket.SetSourceElement ( pPlayer );
                            m_pPlayerManager->BroadcastOnlyJoined ( DetachPacket );
                        }
                    }
                    else // If we're detaching
                    {
                        // Make sure they're attached
                        if ( pVehicle->GetTowedVehicle () == pTrailer &&
                                pTrailer->GetTowedByVehicle () == pVehicle )
                        {
                            // Detach them
                            pVehicle->SetTowedVehicle ( NULL );
                            pTrailer->SetTowedByVehicle ( NULL );

                            // Tell everyone else to detach them
                            m_pPlayerManager->BroadcastOnlyJoined ( Packet, pPlayer );

                            // Execute the detach trailer script function
                            CLuaArguments Arguments;
                            Arguments.PushElement ( pVehicle );
                            pTrailer->CallEvent ( "onTrailerDetach", Arguments );
                        }
                    }
                }
            }
        }
    }
}


void CGame::Packet_Voice_Data ( CVoiceDataPacket& Packet )
{ 
#ifdef MTA_VOICE
    unsigned short usDataLength = 0;
    char szNick [ 256 ];

    if ( m_pVoiceServer )
    {
        usDataLength = Packet.GetDataLength ( );

        if ( usDataLength > 0 )
        {
            CPlayer* pPlayer = Packet.GetSourcePlayer ();
            if ( pPlayer )
            {
                pPlayer->GetNick ( szNick, 256 );
				szNick[255] = '\0';

                // Pass the packet to the voice server.
                if (!m_pVoiceServer->DecodeAndQueuePacket ( pPlayer->GetID(), 
                                                    Packet.GetData ( ), 
                                                    usDataLength ))
                {
                    CLogger::LogPrint("Couldn't decode vox packet");
                }
            }
        }
    }
#endif
}


void CGame::Packet_CameraSync ( CCameraSyncPacket & Packet )
{
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // This might need to be time-contexted
        CPlayerCamera * pCamera = pPlayer->GetCamera ();

        if ( Packet.m_bFixed )
        {    
            pCamera->SetMode ( CAMERAMODE_FIXED );
            pCamera->SetPosition ( Packet.m_vecPosition );
            pCamera->SetLookAt ( Packet.m_vecLookAt );
        }
        else
        {        
            CElement * pTarget = CElementIDs::GetElement ( Packet.m_TargetID );
            pCamera->SetMode ( CAMERAMODE_PLAYER );
            pCamera->SetTarget ( pTarget );
        }
    }
}


void CGame::PlayerCompleteConnect ( CPlayer* pPlayer, bool bSuccess, char* szError )
{
	if ( bSuccess )
	{
		// Call the onPlayerConnect event. If it returns false, disconnect the player
		CLuaArguments Arguments;
		char szIP [22];
		Arguments.PushString ( pPlayer->GetNick () );
		Arguments.PushString ( pPlayer->GetSourceIP ( szIP ) );
		Arguments.PushString ( pPlayer->GetSerialUser ().c_str() );
		Arguments.PushString ( pPlayer->GetSerial ().c_str() );
		if ( !g_pGame->GetMapManager()->GetRootElement()->CallEvent ( "onPlayerConnect", Arguments ) )
		{
			// event cancelled, disconnect the player
			CLogger::LogPrintf ( "CONNECT: %s failed to connect.\n", pPlayer->GetNick() );
			const char* szError = g_pGame->GetEvents()->GetLastError ();
			if ( szError )
			{
				DisconnectPlayer ( g_pGame, *pPlayer, szError );
				return;
			}
			DisconnectPlayer ( g_pGame, *pPlayer, "Disconnected: server refused the connection" );
			return;
		}

		// Tell the console
		//char szIP [22];
		CLogger::LogPrintf ( "CONNECT: %s connected (IP: %s)\n", pPlayer->GetNick(), pPlayer->GetSourceIP ( szIP ) );

		// Send him the join details
		pPlayer->Send ( CPlayerConnectCompletePacket () );

        // The player is spawned when he's connected, just the Camera is not faded in/not targetting
        pPlayer->SetSpawned ( true );
	}
	else
	{
		CLogger::LogPrintf ( "CONNECT: %s failed to connect (Invalid serial)\n", pPlayer->GetNick() );
		if ( szError && strlen ( szError ) > 0 )
			DisconnectPlayer ( g_pGame, *pPlayer, szError );
		else
			DisconnectPlayer ( g_pGame, *pPlayer, "Disconnected: Serial verification failed" );
		return;
	}
}


void CGame::Lock ( void )
{
    pthread_mutex_lock ( &mutexhttp );
}

void CGame::Unlock ( void )
{
    pthread_mutex_unlock ( &mutexhttp );
}
