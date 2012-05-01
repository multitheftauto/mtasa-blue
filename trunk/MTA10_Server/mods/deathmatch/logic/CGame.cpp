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
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "../utils/COpenPortsTester.h"
#include "net/SimHeaders.h"

#define MAX_BULLETSYNC_DISTANCE 400.0f
#define MAX_EXPLOSION_SYNC_DISTANCE 400.0f
#define MAX_PROJECTILE_SYNC_DISTANCE 400.0f

#define RUN_CHILDREN CChildListType::const_iterator iter=pElement->IterBegin();for(;iter!=pElement->IterEnd();iter++)

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
    : m_FloodProtect( 4, 30000, 30000 )     // Max of 4 connections per 30 seconds, then 30 second ignore
{
    // Set our global pointer
    g_pGame = this;

    m_bServerFullyUp = false;

    // Initialize random number generator and time
    RandomizeRandomSeed ();
    InitializeTime ();

    m_bBeingDeleted = false;
    m_pUnoccupiedVehicleSync = NULL;
    m_pConsole = NULL;
    m_pMapManager = NULL;
    m_pHandlingManager = NULL;
    m_pLuaManager = NULL;
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
    m_pDatabaseManager = NULL;
    m_pLuaCallbackManager = NULL;
    m_pRegistryManager = NULL;
    m_pRegistry = NULL;
    m_pAccountManager = NULL;
    m_pPedManager = NULL;
    m_pResourceManager = NULL;
    m_pLatentTransferManager = NULL;
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
    m_pWeaponStatsManager = NULL;
    m_pBuildingRemovalManager = NULL;
#ifdef WITH_OBJECT_SYNC
    m_pObjectSync = NULL;
#endif

    m_bInteriorSoundsEnabled = true;
    m_bOverrideRainLevel = false;
    m_bOverrideSunSize = false;
    m_bOverrideSunColor = false;
    m_bOverrideWindVelocity = false;
    m_bOverrideFarClip = false;
    m_bOverrideFogDistance = false;

    m_pASE = NULL;
    ResetMapInfo();
    m_usFPS = 0;
    m_usFrames = 0;
    m_ulLastFPSTime = 0;
    m_szCurrentFileName = NULL;
    m_pConsoleClient = NULL;
    m_bIsFinished = false;

    //Setup game glitch defaults ( false = disabled )
    m_Glitches [ GLITCH_QUICKRELOAD ] = false;
    m_Glitches [ GLITCH_FASTFIRE ] = false;
    m_Glitches [ GLITCH_FASTMOVE ] = false;
    m_Glitches [ GLITCH_CROUCHBUG ] = false;
    m_Glitches [ GLITCH_CLOSEDAMAGE ] = false;

    //Glitch names (for Lua interface)
    m_GlitchNames["quickreload"] = GLITCH_QUICKRELOAD;
    m_GlitchNames["fastfire"] = GLITCH_FASTFIRE;
    m_GlitchNames["fastmove"] = GLITCH_FASTMOVE;
    m_GlitchNames["crouchbug"] = GLITCH_CROUCHBUG;
    m_GlitchNames["highcloserangedamage"] = GLITCH_CLOSEDAMAGE;

    m_bCloudsEnabled = true;

    m_llLastAnnouceTime = 0;
    m_pOpenPortsTester = NULL;

    m_bTrafficLightsLocked = false;
    m_ucTrafficLightState = 0;
    m_ulLastTrafficUpdate = 0;

    m_bOcclusionsEnabled = true;

    memset( &m_bGarageStates[0], 0, sizeof( m_bGarageStates ) );

    // init our mutex
    pthread_mutex_init(&mutexhttp, NULL);
}

void CGame::ResetMapInfo ( void )
{
    // Add variables to get reset in resetMapInfo here
    m_fGravity = 0.008f;
    m_fGameSpeed = 1.0f;
    m_fJetpackMaxHeight = 100;
    m_fAircraftMaxHeight = 800;

    if ( m_pWaterManager )
    {
        m_pWaterManager->ResetWorldWaterLevel ();
        m_pWaterManager->SetGlobalWaveHeight ( 0.0f );
    }

    m_ucSkyGradientTR = 0, m_ucSkyGradientTG = 0, m_ucSkyGradientTB = 0;
    m_ucSkyGradientBR = 0, m_ucSkyGradientBG = 0, m_ucSkyGradientBB = 0;
    m_bHasSkyGradient = false;
    m_HeatHazeSettings = SHeatHazeSettings ();
    m_bHasHeatHaze = false;
    m_bCloudsEnabled = true;

    m_bTrafficLightsLocked = false;
    m_ucTrafficLightState = 0;
    m_ulLastTrafficUpdate = 0;

    g_pGame->SetHasWaterColor ( false );
    g_pGame->SetInteriorSoundsEnabled ( true );
    g_pGame->SetHasFarClipDistance ( false );
    g_pGame->SetHasFogDistance ( false );
    g_pGame->SetHasRainLevel ( false );
    g_pGame->SetHasSunColor ( false );
    g_pGame->SetHasSunSize ( false );
    g_pGame->SetHasWindVelocity ( false );
}

CGame::~CGame ( void )
{
    m_bBeingDeleted = true;

    // Remove our console control handler
    #ifdef WIN32
    SetConsoleCtrlHandler ( ConsoleEventHandler, FALSE );
    #endif

    // Eventually stop our game
    CSimControl::EnableSimSystem ( false );
    Stop ();

    // Stop the web server
    if ( m_pHTTPD )
        m_pHTTPD->StopHTTPD ();

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
#ifdef WITH_OBJECT_SYNC
    SAFE_DELETE ( m_pObjectSync );
#endif
    SAFE_DELETE ( m_pConsole );
    SAFE_DELETE ( m_pMapManager );
    SAFE_DELETE ( m_pLuaManager );
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
    if ( m_pRegistryManager )
        m_pRegistryManager->CloseRegistry ( m_pRegistry );
    m_pRegistry = NULL;
    SAFE_DELETE ( m_pAccountManager );
    SAFE_DELETE ( m_pRegistryManager );
    SAFE_DELETE ( m_pDatabaseManager );
    SAFE_DELETE ( m_pLuaCallbackManager );
    SAFE_DELETE ( m_pRegisteredCommands );
    SAFE_DELETE ( m_pPedManager );
    SAFE_DELETE ( m_pLatentTransferManager );
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
    SAFE_DELETE ( m_pWeaponStatsManager );
    SAFE_DELETE ( m_pBuildingRemovalManager );
    SAFE_DELETE ( m_pOpenPortsTester );
    CSimControl::Shutdown ();

    // Clear our global pointer
    g_pGame = NULL;
}


void CGame::GetTag ( char *szInfoTag, int iInfoTag )
{
    // Construct the info tag
    SString strInfoTag ( "%c[%c%c%c] MTA: San Andreas %c:%c: %d/%d players %c:%c: %u resources",
               132, 135, szProgress[ucProgress], 132,
               130, 130, m_pPlayerManager->Count (), m_pMainConfig->GetMaxPlayers (),
               130, 130, m_pResourceManager->GetResourceLoadedCount () );

    if ( !GetConfig ()->GetThreadNetEnabled () )
    {
        strInfoTag += SString ( " %c:%c: %u fps",
               130, 130, g_pGame->GetServerFPS () );
    }
    else
    {
        strInfoTag += SString ( " %c:%c: %u fps (%u)",
               130, 130, g_pGame->GetSyncFPS (), g_pGame->GetServerFPS () );
    }

    STRNCPY ( szInfoTag, *strInfoTag, iInfoTag );
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

    UpdateModuleTickCount64 ();

    // Calculate FPS
    unsigned long ulCurrentTime = GetTickCount32 ();
    unsigned long ulDiff = ulCurrentTime - m_ulLastFPSTime;

    // Calculate the server-side fps
    if ( ulDiff >= 1000 )
    {
        m_usFPS = m_usFrames;
        m_usFrames = 0;
        m_ulLastFPSTime = ulCurrentTime;
    }
    m_usFrames++;

    // Update the progress rotator
    uchar ucDelta = (uchar)ulCurrentTime - ucProgressSkip;
    ushort usReqDelta = 80 - ( 100 - Min < ushort > ( 100, m_usFPS ) ) / 5;
    
    if ( ucDelta > usReqDelta ) {
        // Clamp ucProgress between 0 and 3
        ucProgress = ( ucProgress + 1 ) & 3;
        ucProgressSkip = (uchar)ulCurrentTime;
    }

    // Handle critical things
    CSimControl::DoPulse ();
    CNetBufferWatchDog::DoPulse ();

    CLOCK_SET_SECTION( "CGame::DoPulse" );
    CLOCK_CALL1( g_pNetServer->GetHTTPDownloadManager()->ProcessQueuedFiles(); );

    CLOCK_CALL1( m_pPlayerManager->DoPulse (); );

    // Pulse the net interface
    CLOCK_CALL1( g_pNetServer->DoPulse (); );

    if ( m_pLanBroadcast )
    {
        CLOCK_CALL1( m_pLanBroadcast->DoPulse(); );
    }

    // Pulse our stuff
    CLOCK_CALL1( m_pMapManager->DoPulse (); );
    CLOCK_CALL1( m_pUnoccupiedVehicleSync->DoPulse (); );
    CLOCK_CALL1( m_pPedSync->DoPulse (); );
#ifdef WITH_OBJECT_SYNC
    CLOCK_CALL1( m_pObjectSync->DoPulse (); );
#endif
    CLOCK_CALL1( m_pBanManager->DoPulse (); );
    CLOCK_CALL1( m_pAccountManager->DoPulse (); );
    CLOCK_CALL1( m_pRegistryManager->DoPulse (); );
    CLOCK_CALL1( m_pACLManager->DoPulse (); );
    
    // Handle the traffic light sync
    if (m_bTrafficLightsLocked == false)
    {
        CLOCK_CALL1( ProcessTrafficLights (ulCurrentTime); );
    }

    // Pulse ASE
    if ( m_pASE )
    {
        CLOCK_CALL1( m_pASE->DoPulse (); );
    }

    // Pulse the scripting system
    if ( m_pLuaManager )
    {
        CLOCK_CALL1( m_pLuaManager->DoPulse (); );
    }

    CLOCK_CALL1( m_pDatabaseManager->DoPulse (); );

    // Process our resource stop/restart queue
    CLOCK_CALL1( m_pResourceManager->ProcessQueue (); );

    // Delete all items requested
    CLOCK_CALL1( m_ElementDeleter.DoDeleteAll (); );

    CLOCK_CALL1( CPerfStatManager::GetSingleton ()->DoPulse (); );

    PulseMasterServerAnnounce ();

    if ( m_pOpenPortsTester )
        m_pOpenPortsTester->Poll ();

    CLOCK_CALL1( m_lightsyncManager.DoPulse (); );

    CLOCK_CALL1( m_pLatentTransferManager->DoPulse (); );

    // Unlock the critical section again
    Unlock();
}


bool CGame::Start ( int iArgumentCount, char* szArguments [] )
{
    // Init
    m_pASE = NULL;
    IsMainThread ();

    // Startup the getElementsByType from root optimizations
    CElement::StartupEntitiesFromRoot ();

    CSimControl::Startup ();
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
    m_pBanManager = new CBanManager;
    m_pTeamManager = new CTeamManager;
    m_pPedManager = new CPedManager;
    m_pWaterManager = new CWaterManager;
    m_pScriptDebugging = new CScriptDebugging ( m_pLuaManager );
    m_pMapManager = new CMapManager ( m_pBlipManager, m_pObjectManager, m_pPickupManager, m_pPlayerManager, m_pRadarAreaManager, m_pMarkerManager, m_pVehicleManager, m_pTeamManager, m_pPedManager, m_pColManager, m_pWaterManager, m_pClock, m_pLuaManager, m_pGroups, &m_Events, m_pScriptDebugging, &m_ElementDeleter );
    m_pACLManager = new CAccessControlListManager;

    m_pRegisteredCommands = new CRegisteredCommands ( m_pACLManager );
    m_pLuaManager = new CLuaManager ( m_pObjectManager, m_pPlayerManager, m_pVehicleManager, m_pBlipManager, m_pRadarAreaManager, m_pRegisteredCommands, m_pMapManager, &m_Events );
    m_pConsole = new CConsole ( m_pBlipManager, m_pMapManager, m_pPlayerManager, m_pRegisteredCommands, m_pVehicleManager, m_pLuaManager, &m_WhoWas, m_pBanManager, m_pACLManager );
    m_pMainConfig = new CMainConfig ( m_pConsole, m_pLuaManager );
    m_pRPCFunctions = new CRPCFunctions;

    m_pWeaponStatsManager = new CWeaponStatManager();

    m_pBuildingRemovalManager = new CBuildingRemovalManager;


    // Parse the commandline
    if ( !m_CommandLineParser.Parse ( iArgumentCount, szArguments ) )
    {
        return false;
    }

    // Grab the path to the main config
    SString strBuffer;
    const char* szMainConfig;
    if ( m_CommandLineParser.GetMainConfig ( szMainConfig ) )
    {
        strBuffer = g_pServerInterface->GetModManager ()->GetAbsolutePath ( szMainConfig );
    }
    else
    {
        strBuffer = g_pServerInterface->GetModManager ()->GetAbsolutePath ( "mtaserver.conf" );
    }
    m_pMainConfig->SetFileName ( strBuffer );

    m_pResourceDownloader = new CResourceDownloader();

    // Load the main config base
    if ( !m_pMainConfig->Load () )
        return false;

    // Let the main config handle selecting settings from the command line where appropriate
    m_pMainConfig->SetCommandLineParser ( &m_CommandLineParser );

    // Do basic backup
    HandleBackup ();

    // Read some settings
    m_pACLManager->SetFileName ( m_pMainConfig->GetAccessControlListFile ().c_str () );
    const SString strServerIP = m_pMainConfig->GetServerIP ();
    unsigned short usServerPort = m_pMainConfig->GetServerPort ();
    unsigned int uiMaxPlayers = m_pMainConfig->GetMaxPlayers ();

    // Create the account manager
    strBuffer = g_pServerInterface->GetModManager ()->GetAbsolutePath ( "internal.db" );
    m_pDatabaseManager = NewDatabaseManager ();
    m_pLuaCallbackManager = new CLuaCallbackManager ();
    m_pRegistryManager = new CRegistryManager ();
    m_pAccountManager = new CAccountManager ( NULL, strBuffer );

    // Create and start the HTTP server
    m_pHTTPD = new CHTTPD;
    m_pLatentTransferManager = new CLatentTransferManager ();

    // Enable it if required
    if ( m_pMainConfig->IsHTTPEnabled () )
    {
        if ( !m_pHTTPD->StartHTTPD ( strServerIP, m_pMainConfig->GetHTTPPort () ) )
        {
            CLogger::ErrorPrintf ( "Could not start HTTP server on interface '%s' and port '%u'!\n", strServerIP.c_str (), m_pMainConfig->GetHTTPPort () );
            return false;
        }
    }

    // Setup server id
    if ( !g_pNetServer->InitServerId ( m_pMainConfig->GetIdFile () ) )
    {
        CLogger::ErrorPrintf ( "Could not read or create server-id keys file at '%s'\n", *m_pMainConfig->GetIdFile () );
        return false;
    }

    // Eventually set the logfiles
    bool bLogFile = CLogger::SetLogFile ( m_pMainConfig->GetLogFile ().c_str () );
    CLogger::SetAuthFile ( m_pMainConfig->GetAuthFile ().c_str () );

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

    // Prepare our voice string
    SString strVoice = "Disabled";
    if ( m_pMainConfig->IsVoiceEnabled() )
        switch (m_pMainConfig->GetVoiceSampleRate())
        {
            case 0:
                strVoice = SString("Quality [%i];  Sample Rate: [8000kHz]",m_pMainConfig->GetVoiceQuality() ); break;
            case 1:
                strVoice = SString("Quality [%i];  Sample Rate: [16000kHz]",m_pMainConfig->GetVoiceQuality() ); break;
            case 2:
                strVoice = SString("Quality [%i];  Sample Rate: [32000kHz]",m_pMainConfig->GetVoiceQuality() ); break;
            default:
                break;
        }
    if ( m_pMainConfig->GetVoiceBitrate() )
        strVoice += SString(";  Bitrate: [%ibps]",  m_pMainConfig->GetVoiceBitrate() );

    // Make bandwidth reduction string 
    SString strBandwidthSaving = m_pMainConfig->GetSetting ( "bandwidth_reduction" );
    strBandwidthSaving = strBandwidthSaving.Left ( 1 ).ToUpper () + strBandwidthSaving.SubStr ( 1 );
    if ( g_pBandwidthSettings->bLightSyncEnabled )
        strBandwidthSaving += SString ( " with lightweight sync rate of %dms", m_pMainConfig->GetLightSyncRate () );

    // Show the server header
    CLogger::LogPrintfNoStamp ( "==================================================================\n" \
                                "= Multi Theft Auto: San Andreas v%s\n" \
                                "==================================================================\n" \
                                "= Server name      : %s\n" \
                                "= Server IP address: %s\n" \
                                "= Server port      : %u\n" \
                                "= \n" \
                                "= Log file         : %s\n" \
                                "= Maximum players  : %u\n" \
                                "= HTTP port        : %u\n" \
                                "= Voice Chat       : %s\n" \
                                "= Bandwidth saving : %s\n" \
                                "==================================================================\n",

                                MTA_DM_BUILDTAG_SHORT,
                                m_pMainConfig->GetServerName ().c_str (),
                                strServerIP.c_str (),
                                usServerPort,
                                pszLogFileName,
                                uiMaxPlayers,
                                m_pMainConfig->IsHTTPEnabled () ? m_pMainConfig->GetHTTPPort () : 0,
                                strVoice.c_str(),
                                *strBandwidthSaving );

    if ( !bLogFile )
        CLogger::ErrorPrintf ( "Unable to save logfile to '%s'\n", m_pMainConfig->GetLogFile ().c_str () );

    // Check accounts database and print message if there is a problem
    m_pAccountManager->IntegrityCheck ();

    // Setup resource-cache directory
    {
        SString strResourceCachePath ( "%s/resource-cache", g_pServerInterface->GetServerModPath () );
        SString strResourceCacheUnzippedPath ( "%s/unzipped", strResourceCachePath.c_str () );
        SString strResourceCacheHttpClientFilesPath ( "%s/http-client-files", strResourceCachePath.c_str () );
        SString strResourceCacheHttpClientFilesProtectedPath ( "%s/http-client-files-protected", strResourceCachePath.c_str () );

        // Make sure the resource-cache directories exists
        MakeSureDirExists ( ( strResourceCacheUnzippedPath + "/" ).c_str () );
        MakeSureDirExists ( ( strResourceCacheHttpClientFilesPath + "/" ).c_str () );
        MakeSureDirExists ( ( strResourceCacheHttpClientFilesProtectedPath + "/" ).c_str () );

        // Rename old resourcecache dir to show that it is no longer used
        SString strOldResourceCachePath ( "%s/resourcecache", g_pServerInterface->GetServerModPath () );
        SString strOldResourceCachePath2 ( strResourceCachePath + "/_old_resourcecache.delete-me" );
        #ifdef WIN32
            MoveFile ( strOldResourceCachePath, strOldResourceCachePath2 );
        #else
            std::rename ( strOldResourceCachePath, strOldResourceCachePath2 );
        #endif

        // Create cache readme
        SString strReadmeFilename ( "%s/DO_NOT_MODIFY_Readme.txt", strResourceCachePath.c_str () );
        FILE *fh = fopen ( strReadmeFilename, "w" );
        if ( fh )
        {
            fprintf ( fh, "---------------------------------------------------------------------------\n" );
            fprintf ( fh, "The content of this directory is automatically generated by the server.\n\n" );
            fprintf ( fh, "Do not modify or delete anything in here while the server is running.\n\n" );
            fprintf ( fh, "When the server is not running, you can do what you want, including clearing\n" );
            fprintf ( fh, "out all the cached files by deleting the resource-cache directory.\n" );
            fprintf ( fh, "(It will get recreated when the server is next started)\n" );
            fprintf ( fh, "---------------------------------------------------------------------------\n\n" );
            fprintf ( fh, "The 'http-client-files' directory always contains the correct client files\n" );
            fprintf ( fh, "for hosting on a web server.\n" );
            fprintf ( fh, "* If the web server is on the same machine, you can simply link the appropriate\n" );
            fprintf ( fh, "  web server directory to 'http-client-files'.\n" );
            fprintf ( fh, "* If the web server is on a separate machine, ensure it has access to\n" );
            fprintf ( fh, "  'http-client-files' via a network path, or maintain a remote copy using\n" );
            fprintf ( fh, "  synchronization software.\n" );
            fprintf ( fh, "---------------------------------------------------------------------------\n\n" );
            fclose ( fh );
        }
    }

    // Load the ACL's
    if ( !m_pACLManager->Load () )
        return false;

    m_pRemoteCalls = new CRemoteCalls();
    m_pRegistry = m_pRegistryManager->OpenRegistry ( "" );

    m_pResourceManager = new CResourceManager;
    m_pSettings = new CSettings ( m_pResourceManager );
    m_pResourceManager->Refresh();
    m_pUnoccupiedVehicleSync = new CUnoccupiedVehicleSync ( m_pPlayerManager, m_pVehicleManager );
    m_pPedSync = new CPedSync ( m_pPlayerManager, m_pPedManager );
#ifdef WITH_OBJECT_SYNC
    m_pObjectSync = new CObjectSync ( m_pPlayerManager, m_pObjectManager );
#endif
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

    // Set our console control handler
    #ifdef WIN32
    SetConsoleCtrlHandler ( ConsoleEventHandler, TRUE );
    #endif

    // Add our builtin events
    AddBuiltInEvents ();

    // Load the vehicle colors before the main config
    strBuffer = g_pServerInterface->GetModManager ()->GetAbsolutePath ( "vehiclecolors.conf" );
    if ( !m_pVehicleManager->GetColorManager ()->Load ( strBuffer ) )
    {
        // Try to generate a new one and load it again
        if ( m_pVehicleManager->GetColorManager ()->Generate ( strBuffer ) )
        {
            if ( !m_pVehicleManager->GetColorManager ()->Load ( strBuffer ) )
            {
                CLogger::ErrorPrintf ( "%s", "Loading 'vehiclecolors.conf' failed\n " );
            }
        }
        else
        {
            CLogger::ErrorPrintf ( "%s", "Generating a new 'vehiclecolors.conf' failed\n " );
        }
    }

    // Load the registry
    strBuffer = g_pServerInterface->GetModManager ()->GetAbsolutePath ( "registry.db" );
    m_pRegistry->Load ( strBuffer );
    // Check accounts database and print a message if there is a problem
    m_pRegistry->IntegrityCheck ();

    // Load the accounts
    strBuffer = g_pServerInterface->GetModManager ()->GetAbsolutePath ( "accounts.xml" );
    m_pAccountManager->SetFileName ( strBuffer );
    m_pAccountManager->SmartLoad ();

    // Register our packethandler
    g_pNetServer->RegisterPacketHandler ( CGame::StaticProcessPacket );

    // Set encryption level
    g_pNetServer->SetEncryptionEnabled ( m_pMainConfig->GetNetworkEncryptionEnabled () );
    if ( !m_pMainConfig->GetNetworkEncryptionEnabled ()  )
        CLogger::LogPrintf ( "Network encryption disabled\n" );

    // Try to start the network
    if ( !g_pNetServer->StartNetwork ( strServerIP, usServerPort, uiMaxPlayers ) )
    {
        CLogger::ErrorPrintf ( "Could not bind the server on interface '%s' and port '%u'!\n", strServerIP.c_str (), usServerPort );
        return false;
    }

    // Load the banlist
    m_pBanManager->LoadBanList ();

    // If the server is passworded
    if ( m_pMainConfig->HasPassword () )
    {
        // Check it for validity
        const char* szPassword = m_pMainConfig->GetPassword ().c_str ();
        if ( m_pMainConfig->IsValidPassword ( szPassword ) )
        {
            // Store the server password
            CLogger::LogPrintf ( "Server password set to '%s'\n", szPassword );
        }
        else
        {
            CLogger::LogPrint ( "Invalid password in config, no password is used\n" );
        }
    }

    // If ASE is enabled
    if ( m_pMainConfig->GetASEEnabled () || !m_pMainConfig->GetDontBroadcastLan() )
    {
        m_pASE = new ASE ( m_pMainConfig, m_pPlayerManager, static_cast < int > ( usServerPort ), strServerIP, !m_pMainConfig->GetDontBroadcastLan() && !m_pMainConfig->GetASEEnabled () );

        if ( m_pMainConfig->GetSerialVerificationEnabled () )
            m_pASE->SetRuleValue ( "SerialVerification", "yes" );

        if ( m_pMainConfig->GetASEEnabled () )
        {
            PulseMasterServerAnnounce ();
        }

        if ( !m_pMainConfig->GetDontBroadcastLan() )
        {
            m_pLanBroadcast = m_pASE->InitLan();
        }
    }

    // Now load the rest of the config
    if ( !m_pMainConfig->LoadExtended () )
        return false;

    // Is the script debug log enabled?
    if ( m_pMainConfig->GetScriptDebugLogEnabled () )
    {
        if ( !m_pScriptDebugging->SetLogfile ( m_pMainConfig->GetScriptDebugLogFile ().c_str (), m_pMainConfig->GetScriptDebugLogLevel () ) )
        {
            CLogger::LogPrint ( "WARNING: Unable to open the given script debug logfile\n" );
        }
    }


    // Run startup commands
    strBuffer = g_pServerInterface->GetModManager ()->GetAbsolutePath ( "autoexec.conf"  );
    CCommandFile Autoexec ( strBuffer, *m_pConsole, *m_pConsoleClient );
    if ( Autoexec.IsValid () )
    {
        CLogger::LogPrintf ( "autoexec.conf file found! Executing...\n" );
        Autoexec.Run ();
    }

    // Done
    // If you're ever going to change this message, update the "server ready" determination
    // inside CServer.cpp in deathmatch mod aswell.
    CLogger::LogPrint ( "Server started and is ready to accept connections!\n" );

    // Create port tester
    m_pOpenPortsTester = new COpenPortsTester ();

    // Add help hint
    CLogger::LogPrint ( "Type 'help' for a list of commands.\n" );

    m_bServerFullyUp = true;
    return true;
}


void CGame::Stop ( void )
{
    m_bServerFullyUp = false;

    // Tell the log
    CLogger::LogPrint ( "Server stopped!\n" );

    // Stop our network
    g_pNetServer->StopNetwork ();

    // Unregister our packethandler
    g_pNetServer->RegisterPacketHandler ( NULL );

}


void CGame::StartOpenPortsTest ( void )
{
    if ( m_pOpenPortsTester )
        m_pOpenPortsTester->Start ();
}


// Remind master server once every 24 hrs
void CGame::PulseMasterServerAnnounce ( void )
{
    if ( m_llLastAnnouceTime == 0 || GetTickCount64_ () - m_llLastAnnouceTime > 24 * 60 * 60 * 1000 )
    {
        bool bFirstPass = m_llLastAnnouceTime == 0;
        m_llLastAnnouceTime = GetTickCount64_ ();

        // If ASE is enabled
        if ( m_pMainConfig->GetASEEnabled () )
        {
            const SString strServerIP = m_pMainConfig->GetServerIP ();
            unsigned short usServerPort = m_pMainConfig->GetServerPort ();
            unsigned short usHTTPPort = m_pMainConfig->GetHTTPPort ();

            SString strVersion ( "%d.%d.%d-%d.%05d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE, MTASA_VERSION_BUILD );

            struct {
                SString strDesc;
                SString strURL;
                bool bRepeat;
            } masterServerList[] = {
                                     { "Querying game-monitor.com master server... ", SString ( QUERY_URL_GAME_MONITOR, usServerPort + 123 ), false },
                                     { "Querying backup master server... ", SString ( "http://master.mtasa.com/ase/add.php?g=%u&a=%u&h=%u&v=%s", usServerPort, usServerPort + 123, usHTTPPort, *strVersion ), true },
                                   };

            for ( uint i = 0 ; i < NUMELMS( masterServerList ) ; i++ )
            {
                const SString& strDesc = masterServerList[i].strDesc;
                const SString& strURL = masterServerList[i].strURL;
                bool bRepeat = masterServerList[i].bRepeat;

                // Don't repeat request for some servers
                if ( !bFirstPass && !bRepeat )
                    continue;

                // Only log on first pass
                if ( bFirstPass )
                    CLogger::LogPrint ( strDesc );

                // Send request
                CTCPImpl * pTCP = new CTCPImpl ();
                pTCP->Initialize ();
                CHTTPRequest * request = new CHTTPRequest ( strURL );
                request->SetLocalIP ( strServerIP );
                CHTTPResponse * response = request->Send ( pTCP );

                // Only log on first pass
                if ( bFirstPass )
                {
                    if ( !response )
                        CLogger::LogPrintfNoStamp ( "failed! (Not available)\n" );
                    else if ( response->GetErrorCode () != 200 )
                    {
                        if ( response->GetErrorCode () == 500 && strDesc.ContainsI ( "game-monitor" ) )
                            CLogger::LogPrintfNoStamp ( "unavailable!\n" );
                        else
                            CLogger::LogPrintfNoStamp ( "failed! (%u: %s)\n", response->GetErrorCode (), response->GetErrorDescription () );
                    }
                    else
                        CLogger::LogPrintfNoStamp ( "success!\n");
                }

                if ( response )
                    delete response;
                delete pTCP;
                delete request;
            }
        }
    }
}


bool CGame::StaticProcessPacket ( unsigned char ucPacketID, const NetServerPlayerID& Socket, NetBitStreamInterface* pBitStream, SNetExtraInfo* pNetExtraInfo )
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
        g_pGame->m_pRPCFunctions->ProcessPacket ( Socket, *pBitStream );
        return true;
    }

    // Is it this?
    if ( ucPacketID == PACKET_ID_LATENT_TRANSFER )
    {
        g_pGame->GetLatentTransferManager ()->OnReceive ( Socket, pBitStream );
        return true;
    }

    // Translate the packet
    CPacket* pPacket = g_pGame->m_pPacketTranslator->Translate ( Socket, static_cast < ePacketID > ( ucPacketID ), *pBitStream, pNetExtraInfo );
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

        case PACKET_ID_PED_WASTED:
        {
            Packet_PedWasted ( static_cast < CPedWastedPacket& > ( Packet ) );
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

        case PACKET_ID_PLAYER_BULLETSYNC:
        {
            Packet_Bulletsync ( static_cast < CBulletsyncPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_DETONATE_SATCHELS:
        {
            Packet_DetonateSatchels ( static_cast < CDetonateSatchelsPacket& > ( Packet ) );
            return true;
        }


        case PACKET_ID_DESTROY_SATCHELS:
        {
            Packet_DestroySatchels ( static_cast < CDestroySatchelsPacket& > ( Packet ) );
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

        case PACKET_ID_VOICE_END:
        {
            Packet_Voice_End ( static_cast < CVoiceEndPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_CAMERA_SYNC:
        {
            Packet_CameraSync ( static_cast < CCameraSyncPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_PLAYER_TRANSGRESSION:
        {
            Packet_PlayerTransgression ( static_cast < CPlayerTransgressionPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_PLAYER_DIAGNOSTIC:
        {
            Packet_PlayerDiagnostic ( static_cast < CPlayerDiagnosticPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_PLAYER_MODINFO:
        {
            Packet_PlayerModInfo ( static_cast < CPlayerModInfoPacket& > ( Packet ) );
            return true;
        }

        case PACKET_ID_PLAYER_SCREENSHOT:
        {
            Packet_PlayerScreenShot ( static_cast < CPlayerScreenShotPacket& > ( Packet ) );
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
#ifdef WITH_OBJECT_SYNC
    else if ( m_pObjectSync->ProcessPacket ( Packet ) )
    {
        return true;
    }
#endif
    return false;
}


void CGame::JoinPlayer ( CPlayer& Player )
{
    CTimeUsMarker < 20 > marker;
    marker.Set ( "Start" );

    // Set the root element as his parent
    // NOTE: Make sure he doesn't get any entities sent to him because we're sending him soon
    Player.SetDoNotSendEntities ( true );
    Player.SetParentObject ( m_pMapManager->GetRootElement () );
    Player.SetDoNotSendEntities ( false );

    marker.Set ( "SetParentObject" );

    // Let him join
    Player.Send ( CPlayerJoinCompletePacket ( Player.GetID (),
                                              m_pPlayerManager->Count (),
                                              m_pMapManager->GetRootElement ()->GetID (),
                                              m_pMainConfig->GetHTTPDownloadType (),
                                              m_pMainConfig->GetHTTPPort (),
                                              m_pMainConfig->GetHTTPDownloadURL ().c_str (),
                                              m_pMainConfig->GetHTTPMaxConnectionsPerClient (),
                                              m_pMainConfig->GetEnableClientChecks (),
                                              m_pMainConfig->IsVoiceEnabled(),
                                              m_pMainConfig->GetVoiceSampleRate(),
                                              m_pMainConfig->GetVoiceQuality(),
                                              m_pMainConfig->GetVoiceBitrate() ) );

    marker.Set ( "CPlayerJoinCompletePacket" );

    // Add debug info if wanted
    if ( CPerfStatDebugInfo::GetSingleton ()->IsActive ( "PlayerInGameNotice" ) )
        CPerfStatDebugInfo::GetSingleton ()->AddLine ( "PlayerInGameNotice", marker.GetString () );
}


void CGame::InitialDataStream ( CPlayer& Player )
{
    CTimeUsMarker < 20 > marker;
    marker.Set ( "Start" );

    // He's joined now
    Player.SetStatus ( STATUS_JOINED );

    // Console
    CLogger::LogPrintf ( "JOIN: %s joined the game (IP: %s)\n", Player.GetNick (), Player.GetSourceIP () );

    // Tell the other players about him
    CPlayerListPacket PlayerNotice;
    PlayerNotice.AddPlayer ( &Player );
    PlayerNotice.SetShowInChat ( true );
    m_pPlayerManager->BroadcastOnlyJoined ( PlayerNotice, &Player );

    marker.Set ( "PlayerNotice" );

    // Tell the map manager
    m_pMapManager->OnPlayerJoin ( Player ); // This sends the elements that are needed before the resources start

    marker.Set ( "SendMapElements" );

    // Write all players connected right now to a playerlist packet except the one we got the ingame notice from
    CPlayerListPacket PlayerList;
    // Entity add packet might as well be generated
    CEntityAddPacket EntityPacket;
    PlayerList.SetShowInChat ( false );
    list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
    {
        CPlayer * pPlayer = *iter;
        if ( &Player != *iter && (*iter)->IsJoined () && !(*iter)->IsBeingDeleted () )
        {
            PlayerList.AddPlayer ( *iter );
        }
        if ( pPlayer != &Player )
        {
            EntityPacket.Add ( pPlayer );
        }
    }

    // Send it to the player we got this ingame notice from
    Player.Send ( PlayerList );

    marker.Set ( "PlayerList" );

    Player.Send ( EntityPacket );

    marker.Set ( "SendPlayerElements" );

    // Tell him about the blips (needs to be done after all the entities it can be attached to)
    m_pMapManager->SendBlips ( Player );

    marker.Set ( "SendBlips" );

    // Send him the current info of the current players ( stats, clothes, etc )
    iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
    {
        if ( &Player != *iter && (*iter)->IsJoined () && (*iter)->IsSpawned () )
        {
            CPlayerStatsPacket PlayerStats = *(*iter)->GetPlayerStatsPacket ( );
            PlayerStats.SetSourceElement ( *iter );
            if ( PlayerStats.GetSize() > 0 )
                Player.Send ( PlayerStats );

            CPlayerClothesPacket PlayerClothes;
            PlayerClothes.SetSourceElement ( *iter );
            PlayerClothes.Add ( (*iter)->GetClothes () );
            if ( PlayerClothes.Count () > 0 )
                Player.Send ( PlayerClothes );
        }
    }

    marker.Set ( "PlayerStats" );

    // Tell the resource manager
    m_pResourceManager->OnPlayerJoin ( Player );

    marker.Set ( "ResourceStartPacket" );

    // Tell our scripts the player has joined
    CLuaArguments Arguments;
    Player.CallEvent ( "onPlayerJoin", Arguments );

    marker.Set ( "onPlayerJoin" );

    // If auto-login is enabled, try and log him in
    if ( m_pAccountManager->IsAutoLoginEnabled () )
    {
        std::string strIP = Player.GetSourceIP ();
        std::string strPlayerSerial = Player.GetSerial();
        CAccount* pAccount = m_pAccountManager->Get ( Player.GetNick (), strIP.c_str () );
        if ( pAccount )
        {
            if ( !pAccount->GetClient ( ) )
            {
                m_pAccountManager->LogIn ( &Player, &Player, pAccount, true );
            }
            else
            {
                CLogger::AuthPrintf ( "LOGIN: %s failed to login in as '%s' (IP: %s  Serial: %s) due to the account already being in use.\n", Player.GetNick (), pAccount->GetName ().c_str (), strIP.c_str (), strPlayerSerial.c_str () );
                Player.SendEcho ( "auto-login: You could not login because your account is already in use." );
            }
        }
    }

    marker.Set ( "AutoLogin" );

    // Register them on the lightweight sync manager.
    m_lightsyncManager.RegisterPlayer ( &Player );

    // Add debug info if wanted
    if ( CPerfStatDebugInfo::GetSingleton ()->IsActive ( "InitialDataStream" ) )
        CPerfStatDebugInfo::GetSingleton ()->AddLine ( "InitialDataStream", marker.GetString () );
}

void CGame::QuitPlayer ( CPlayer& Player, CClient::eQuitReasons Reason, bool bSayInConsole, const char* szKickReason, const char* szResponsiblePlayer )
{
    // Grab quit reaason
    const char* szReason = "Unknown";
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
        if ((Reason == CClient::QUIT_BAN || Reason == CClient::QUIT_KICK))
        {
            if ( szKickReason && szKickReason [ 0 ] )
                Arguments.PushString ( szKickReason );
            else
                Arguments.PushBoolean ( false );
            //Arguments.PushUserData ( strResponsiblePlayer != "Console" ? m_pPlayerManager->Get ( strResponsiblePlayer, true ) : static_cast < CConsoleClient* > (m_pConsoleClient) );
            if ( strcmp ( szResponsiblePlayer, "Console" ) == 0 )
                Arguments.PushElement ( m_pConsoleClient );
            else
                Arguments.PushElement ( m_pPlayerManager->Get ( szResponsiblePlayer, true ) );
        }
        else
        {
            Arguments.PushBoolean ( false );
            Arguments.PushBoolean ( false );
        }

        Player.CallEvent ( "onPlayerQuit", Arguments );

        // Tell the map manager
        m_pMapManager->OnPlayerQuit ( Player );

        // Tell all the players except the parting one (we don't tell them if he hadn't joined because the players don't know about him)
        CPlayerQuitPacket Packet;
        Packet.SetPlayer ( Player.GetID () );
        Packet.SetQuitReason ( static_cast < unsigned char > ( Reason ) );
        m_pPlayerManager->BroadcastOnlyJoined ( Packet, &Player );
    }

    // Tell net module connection version info will no longer be required
    g_pNetServer->ClearClientBitStreamVersion ( Player.GetSocket () );

    // Remove from any latent transfers
    GetLatentTransferManager ()->RemoveRemote ( Player.GetSocket () );

    // Delete it, don't unlink yet, we could be inside the player-manager's iteration
    m_ElementDeleter.Delete ( &Player, false );

    // Unregister them from the lightweight sync manager
    m_lightsyncManager.UnregisterPlayer ( &Player );
}


void CGame::AddBuiltInEvents ( void )
{
    // Resource events
    m_Events.AddEvent ( "onResourcePreStart", "resource", NULL, false );
    m_Events.AddEvent ( "onResourceStart", "resource", NULL, false );
    m_Events.AddEvent ( "onResourceStop", "resource", NULL, false );

    // Blip events

    // Marker events
    m_Events.AddEvent ( "onMarkerHit", "player, matchingDimension", NULL, false );
    m_Events.AddEvent ( "onMarkerLeave", "player, matchingDimension", NULL, false );

    // Voice events
    m_Events.AddEvent ( "onPlayerVoiceStart", "", NULL, false );
    m_Events.AddEvent ( "onPlayerVoiceStop", "", NULL, false );


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
    m_Events.AddEvent ( "onPlayerPrivateMessage", "text, player", NULL, false );
    m_Events.AddEvent ( "onPlayerStealthKill", "target", NULL, false );
    m_Events.AddEvent ( "onPlayerMute", "", NULL, false );
    m_Events.AddEvent ( "onPlayerUnmute", "", NULL, false );
    m_Events.AddEvent ( "onPlayerCommand", "command", NULL, false );
    m_Events.AddEvent ( "onPlayerModInfo", "type, ids, names", NULL, false );
    m_Events.AddEvent ( "onPlayerScreenShot", "resource, status, file_data, timestamp, tag", NULL, false );

    // Ped events
    m_Events.AddEvent ( "onPedWasted", "ammo, killer, weapon, bodypart", NULL, false );
    m_Events.AddEvent ( "onPedWeaponSwitch", "previous, current", NULL, false );

    // Element events
    m_Events.AddEvent ( "onElementColShapeHit", "colshape, matchingDimension", NULL, false );
    m_Events.AddEvent ( "onElementColShapeLeave", "colshape, matchingDimension", NULL, false );
    m_Events.AddEvent ( "onElementClicked", "button, state, clicker, posX, posY, posZ", NULL, false );
    m_Events.AddEvent ( "onElementDataChange", "key, oldValue", NULL, false );
    m_Events.AddEvent ( "onElementDestroy", "", NULL, false );
    m_Events.AddEvent ( "onElementStartSync", "newSyncer", NULL, false );
    m_Events.AddEvent ( "onElementStopSync", "oldSyncer", NULL, false );
    m_Events.AddEvent ( "onElementModelChange", "oldModel, newModel", NULL, false );

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

    // Debug events
    m_Events.AddEvent ( "onDebugMessage", "message, level, file, line", NULL, false );

    // Ban events
    m_Events.AddEvent ( "onBan", "ip", NULL, false );
    m_Events.AddEvent ( "onUnban", "ip", NULL, false );

    // Account events
    m_Events.AddEvent ( "onAccountDataChange", "account, key, value", NULL, false );

    // Other events
    m_Events.AddEvent ( "onSettingChange", "setting, oldValue, newValue", NULL, false );
    m_Events.AddEvent ( "onChatMessage", "message, element", NULL, false );
}

void CGame::ProcessTrafficLights ( unsigned long ulCurrentTime )
{
    unsigned long ulDiff = static_cast < unsigned long > ( (ulCurrentTime - m_ulLastTrafficUpdate)*m_fGameSpeed );
    unsigned char ucNewState = 0xFF;

    if ( ulDiff >= 1000 )
    {
        if ( ( m_ucTrafficLightState == 0 || m_ucTrafficLightState == 3 ) && ulDiff >= 8000 ) // green
        {
            ucNewState = m_ucTrafficLightState + 1;
        }
        else if ( ( m_ucTrafficLightState == 1 || m_ucTrafficLightState == 4 ) && ulDiff >= 3000 ) // orange
        {
            ucNewState = ( m_ucTrafficLightState == 4 ) ? 0 : 2;
        }
        else if ( m_ucTrafficLightState == 2 && ulDiff >= 2000 ) // red
        {
            ucNewState = 3;
        }

        if ( ucNewState != 0xFF )
        {
            CStaticFunctionDefinitions::SetTrafficLightState (ucNewState);
            m_ulLastTrafficUpdate = GetTickCount32 ();
        }
    }
}


void CGame::Packet_PlayerJoin ( const NetServerPlayerID& Source )
{
    // Reply with the mod this server is running
    NetBitStreamInterface* pBitStream = g_pNetServer->AllocateNetServerBitStream ( 0 );
    if ( pBitStream )
    {
        // Write the mod name to the bitstream
        pBitStream->Write ( static_cast < unsigned short > ( MTA_DM_BITSTREAM_VERSION ) );
        pBitStream->WriteString ( "deathmatch" );

        // Send and destroy the bitstream
        g_pNetServer->SendPacket ( PACKET_ID_MOD_NAME, Source, pBitStream, false, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED );
        g_pNetServer->DeallocateNetServerBitStream ( pBitStream );
    }
}


void CGame::Packet_PlayerJoinData ( CPlayerJoinDataPacket& Packet )
{
    // Grab the nick
    const char* szNick = Packet.GetNick ();
    if ( szNick && szNick [0] != 0 )
    {
        // Is the server passworded?
        bool bPasswordIsValid = true;
        if ( m_pMainConfig->HasPassword () )
        {
            // Grab the password hash from the packet
            const MD5& PacketHash = Packet.GetPassword ();

            // Hash our password
            const std::string& strPassword = m_pMainConfig->GetPassword ();
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
            // Set the bitstream version number for this connection
            pPlayer->SetBitStreamVersion ( Packet.GetBitStreamVersion () );
            g_pNetServer->SetClientBitStreamVersion ( Packet.GetSourceSocket (), Packet.GetBitStreamVersion () );

            // Get the serial number from the packet source
            NetServerPlayerID p = Packet.GetSourceSocket ();
            SString strSerial;
            SString strPlayerVersion;
            {
                SFixedString < 32 > strSerialTemp;
                SFixedString < 32 > strPlayerVersionTemp;
                g_pNetServer->GetClientSerialAndVersion ( p, strSerialTemp, strPlayerVersionTemp );
                strSerial = SStringX ( strSerialTemp );
                strPlayerVersion = SStringX ( strPlayerVersionTemp );
            }

            SString strIP = pPlayer->GetSourceIP ();
            SString strIPAndSerial( "IP: %s  Serial: %s  Version: %s", strIP.c_str (), strSerial.c_str (), strPlayerVersion.c_str () );
            if ( !CheckNickProvided ( szNick ) ) // check the nick is valid
            {
                // Tell the console
                CLogger::LogPrintf ( "CONNECT: %s failed to connect (Invalid Nick) (%s)\n", szNick, strIPAndSerial.c_str () );

                // Tell the player the problem
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
                    // Same person? Quit the first and let this one join
                    if ( strcmp ( pPlayer->GetSourceIP (), pTempPlayer->GetSourceIP () ) == 0 )
                    {
                        // Two players could have the same IP, so see if the old player appears inactive before quitting them
                        if ( pTempPlayer->UhOhNetworkTrouble () )
                        {
                            pTempPlayer->Send ( CPlayerDisconnectedPacket ( SString ( "Supplanted by %s from %s", szNick, pPlayer->GetSourceIP () ) ) );
                            QuitPlayer ( *pTempPlayer, CClient::QUIT_QUIT );
                            pTempPlayer = NULL;
                        }
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
                            if ( !m_pMainConfig->GetJoinFloodProtectionEnabled () || !m_FloodProtect.AddConnect ( SString ( "%x", Packet.GetSourceIP() ) ) )
                            {
                                // Set the nick and the game version
                                pPlayer->SetNick ( szNick );
                                pPlayer->SetGameVersion ( Packet.GetGameVersion () );
                                pPlayer->SetMTAVersion ( Packet.GetMTAVersion () );
                                pPlayer->SetSerialUser ( Packet.GetSerialUser () );
                                pPlayer->SetSerial ( strSerial );
                                pPlayer->SetPlayerVersion ( strPlayerVersion );

                                // Check if client must update
                                if ( GetConfig ()->IsBelowMinimumClient ( pPlayer->GetPlayerVersion () ) )
                                {
                                    // Tell the console
                                    CLogger::LogPrintf ( "CONNECT: %s failed to connect (Client version is below minimum) (%s)\n", szNick, strIPAndSerial.c_str () );

                                    // Tell the player
                                    pPlayer->Send ( CUpdateInfoPacket ( "Mandatory", GetConfig ()->GetMinimumClientVersion () ) );
                                    DisconnectPlayer ( this, *pPlayer, "" );
                                    return;
                                }

                                // Check if client should optionally update
                                if ( Packet.IsOptionalUpdateInfoRequired () && GetConfig ()->IsBelowRecommendedClient ( pPlayer->GetPlayerVersion () ) )
                                {
                                    // Tell the console
                                    CLogger::LogPrintf ( "CONNECT: %s advised to update (Client version is below recommended) (%s)\n", szNick, strIPAndSerial.c_str () );

                                    // Tell the player
                                    pPlayer->Send ( CUpdateInfoPacket ( "Optional", GetConfig ()->GetRecommendedClientVersion () ) );
                                    DisconnectPlayer ( this, *pPlayer, "" );
                                    return;
                                }

                                // Check the serial for validity
                                if ( CBan* pBan = m_pBanManager->GetBanFromSerial ( pPlayer->GetSerial ().c_str () ) )
                                {
                                    // Make a message including the ban duration
                                    SString strBanMessage = "Serial is banned";
                                    SString strDurationDesc = pBan->GetDurationDesc ();
                                    if ( strDurationDesc.length () )
                                        strBanMessage += " (" + strDurationDesc + ")";

                                    // Tell the console
                                    CLogger::LogPrintf ( "CONNECT: %s failed to connect (%s) (%s)\n", szNick, strBanMessage.c_str (), strIPAndSerial.c_str () );

                                    // Make a message for the player
                                    strBanMessage = std::string ( "Disconnected: " ) + strBanMessage;

                                    // Tell the player he's banned
                                    DisconnectPlayer ( this, *pPlayer, strBanMessage );
                                    return;
                                }

                                // Check the ip for banness
                                if ( CBan* pBan = m_pBanManager->GetBanFromIP ( strIP ) )
                                {
                                    // Make a message including the ban duration
                                    SString strBanMessage;// = "Serial is banned";
                                    SString strDurationDesc = pBan->GetDurationDesc ();
                                    if ( strDurationDesc.length () )
                                        strBanMessage += " (" + strDurationDesc + ")";

                                    // Tell the console
                                    CLogger::LogPrintf ( "CONNECT: %s failed to connect (IP is banned%s) (%s)\n", szNick, strBanMessage.c_str (), strIPAndSerial.c_str () );

                                    // Make a message for the player
                                    strBanMessage = std::string ( "Disconnected: You are banned " ) + strBanMessage;

                                    // Tell the player he's banned
                                    DisconnectPlayer ( this, *pPlayer, strBanMessage );
                                    return;
                                }

                                if ( !pPlayer->GetSerialUser ().empty() &&
                                     m_pBanManager->IsAccountBanned ( pPlayer->GetSerialUser ().c_str () ) )
                                {
                                    // Tell the console
                                    CLogger::LogPrintf ( "CONNECT: %s failed to connect (Account is banned) (%s)\n", szNick, strIPAndSerial.c_str () );

                                    // Tell the player he's banned
                                    DisconnectPlayer ( this, *pPlayer, "Disconnected: Account is banned" );
                                    return;
                                }

                            #if MTASA_VERSION_TYPE > VERSION_TYPE_UNSTABLE
                                if ( Packet.GetPlayerVersion ().length () > 0 &&
                                     Packet.GetPlayerVersion () != pPlayer->GetPlayerVersion () )
                                {
                                    // Tell the console
                                    CLogger::LogPrintf ( "CONNECT: %s failed to connect (Version mismatch) (%s)\n", szNick, strIPAndSerial.c_str () );

                                    // Tell the player
                                    DisconnectPlayer ( this, *pPlayer, "Disconnected: Version mismatch" );
                                    return;
                                }
                            #endif

                                // Add him to the whowas list
                                m_WhoWas.Add ( szNick, Packet.GetSourceIP (), pPlayer->GetSerial (), pPlayer->GetPlayerVersion () );

                                // Verify the player's serial if necessary
                                if ( m_pMainConfig->GetSerialVerificationEnabled () )
                                    m_SerialManager.Verify ( pPlayer, PlayerCompleteConnect );
                                else
                                    PlayerCompleteConnect ( pPlayer, true, NULL );
                            }
                            else
                            {
                                // Tell the console
                                CLogger::LogPrintf ( "CONNECT: %s failed to connect (Join flood) (%s)\n", szNick, strIPAndSerial.c_str () );

                                // Tell the player the problem
                                DisconnectPlayer ( this, *pPlayer, "Disconnected: Join flood. Please wait a minute, then reconnect." );
                            }
                        }
                        else
                        {
                            // Tell the console
                            CLogger::LogPrintf ( "CONNECT: %s failed to connect (Wrong password) (%s)\n", szNick, strIPAndSerial.c_str () );

                            // Tell the player the password was wrong
                            DisconnectPlayer ( this, *pPlayer, "Disconnected: Incorrect password" );
                        }
                    }
                    else
                    {
                        // Tell the console
                        CLogger::LogPrintf ( "CONNECT: %s failed to connect (Bad version) (%s)\n", szNick, strIPAndSerial.c_str () );

                        // Tell the player the problem
                        SString strMessage;
                        ushort usClientNetVersion = Packet.GetNetVersion ();
                        ushort usServerNetVersion = MTA_DM_NETCODE_VERSION;
                        ushort usClientBranchId = usClientNetVersion >> 12;
                        ushort usServerBranchId = usServerNetVersion >> 12;

                        if ( usClientBranchId != usServerBranchId )
                        {
                            strMessage = SString ( "Disconnected: Server from different branch (client: %X, server: %X)\n", usClientBranchId, usServerBranchId );
                        }
                        else
                        if ( MTASA_VERSION_BUILD == 0 )
                        {
                            strMessage = SString ( "Disconnected: Bad version (client: %X, server: %X)\n", usClientNetVersion, usServerNetVersion );
                        }
                        else
                        {
                            if ( usClientNetVersion < usServerNetVersion )
                                strMessage = SString ( "Disconnected: Server is running a newer build (%d)\n", MTASA_VERSION_BUILD );
                            else
                                strMessage = SString ( "Disconnected: Server is running an older build (%d)\n", MTASA_VERSION_BUILD );
                        }
                        DisconnectPlayer ( this, *pPlayer, strMessage );
                    }
                }
                else
                {
                    // Tell the console
                    CLogger::LogPrintf ( "CONNECT: %s failed to connect (Nick already in use) (%s)\n", szNick, strIPAndSerial.c_str () );

                    // Tell the player the problem
                    DisconnectPlayer ( this, *pPlayer, "Disconnected: Nick already in use" );
                }
            }
            else
            {
                // Tell the console
                CLogger::LogPrintf ( "CONNECT: %s failed to connect (Invalid nickname)\n", pPlayer->GetSourceIP () );

                // Tell the player the problem
                DisconnectPlayer ( this, *pPlayer, "Disconnected: Invalid nickname" );
            }
        }
    }
}

void CGame::Packet_PedWasted ( CPedWastedPacket& Packet )
{
    CElement * pElement = ( Packet.m_PedID != INVALID_ELEMENT_ID ) ? CElementIDs::GetElement ( Packet.m_PedID ) : NULL;
    CPed* pPed = static_cast < CPed* > ( pElement );
    if ( pPed && !pPed->IsDead() )
    {
        pPed->SetIsDead ( true );
        pPed->SetPosition ( Packet.m_vecPosition );
        // Remove him from any occupied vehicle
        pPed->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
        CVehicle* pVehicle = pPed->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            pVehicle->SetOccupant ( NULL, pPed->GetOccupiedVehicleSeat () );
            pPed->SetOccupiedVehicle ( NULL, 0 );
        }

        CElement * pKiller = ( Packet.m_Killer != INVALID_ELEMENT_ID ) ? CElementIDs::GetElement ( Packet.m_Killer ) : NULL;

        // Create a new packet to send to everyone
        CPedWastedPacket ReturnWastedPacket ( pPed, pKiller, Packet.m_ucKillerWeapon, Packet.m_ucBodyPart, false, Packet.m_AnimGroup, Packet.m_AnimID );
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
        pPed->CallEvent ( "onPedWasted", Arguments );

        // Reset the weapons list, because a ped loses his weapons on death
        for (unsigned int slot = 0; slot < WEAPON_SLOTS; ++slot)
        {
            pPed->SetWeaponType(0, slot);
            pPed->SetWeaponAmmoInClip(0, slot);
            pPed->SetWeaponTotalAmmo(0, slot);
        }
    }
}
void CGame::Packet_PlayerWasted ( CPlayerWastedPacket& Packet )
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if ( pPlayer ) {
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
        m_pPlayerManager->BroadcastOnlyJoined ( ReturnWastedPacket, pPlayer );

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

        // Reset the weapons list, because a player loses his weapons on death
        for (unsigned int slot = 0; slot < WEAPON_SLOTS; ++slot)
        {
            pPlayer->SetWeaponType(0, slot);
            pPlayer->SetWeaponAmmoInClip(0, slot);
            pPlayer->SetWeaponTotalAmmo(0, slot);
        }
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


// Relay this (pure sync) packet to all the other players using distance rules
void CGame::RelayPlayerPuresync ( CPacket& Packet )
{
    UpdateModuleTickCount64 ();

    // Make a list of players to send this packet to
    std::vector < CPlayer* > sendList;
    std::vector < CPlayer* > simSendList;
    bool bUseSimSendList = CSimControl::IsSimSystemEnabled ();

    CPlayer* pPlayer = Packet.GetSourcePlayer ();

    //
    // Process far sync (only if light sync is not active)
    //
    if ( pPlayer->IsTimeForPuresyncFar () )
    {
        long long llTickCountNow = GetModuleTickCount64 ();

        // Use this players far list
        std::map < CPlayer*, SPuresyncNearInfo >& farList = pPlayer->GetPuresyncFarPlayerList ();

        // For each far player
        for ( std::map < CPlayer*, SPuresyncNearInfo > ::iterator it = farList.begin (); it != farList.end (); ++it )
        {
            CPlayer* pSendPlayer = it->first;
            SPuresyncNearInfo& nearInfo = it->second;

            nearInfo.llLastUpdateTime = llTickCountNow;
            sendList.push_back ( pSendPlayer );
        }
    }

    //
    // Process near sync
    //
    {
        // Insert into other players puresync near list if appropriate
        pPlayer->UpdateOthersPuresyncNearList ();

        // Use this players puresync near list for sending packets
        std::map < CPlayer*, SPuresyncNearInfo >& nearList = pPlayer->GetPuresyncNearPlayerList ();

        // Array for holding players that need moving to the puresync far list
        std::vector < CPlayer* > moveToFarListList;

        // For each puresync near player
        for ( std::map < CPlayer*, SPuresyncNearInfo > ::iterator it = nearList.begin (); it != nearList.end (); ++it )
        {
            CPlayer* pSendPlayer = it->first;
            SPuresyncNearInfo& nearInfo = it->second;
            if ( --nearInfo.iCount < 1 )
            {
                // Remove player from puresync near list (Has to be not near for 5 calls to get removed (The delay ensures timely updates of players moving far away))
                moveToFarListList.push_back ( pSendPlayer );
            }
            else
            {
                bool bTimeForSync = pSendPlayer->IsTimeToReceivePuresyncNearFrom ( pPlayer, nearInfo );
                if ( !bUseSimSendList )
                {
                    // Standard sending
                    if ( bTimeForSync )
                        sendList.push_back ( pSendPlayer );
                }
                else
                {
                    //
                    // Sim sync relays pure sync packets to the other player when he is in zone 0 (as seen from this player)
                    // Enabling/disabling sim puresync will only take effect for the next pure sync packet, so:
                    //                           prevzone nowZone
                    //      Moving into zone 0:     1+       0      Do sync here, sim pure sync on
                    //      In zone 0:              0        0      No sync here, sim pure sync on
                    //      Moving from zone 0:     0        1+     No sync here, sim pure sync off
                    //      Not in zone0:           1+       1+     Do sync here  sim pure sync off
                    //
                    bool bSyncHere = ( nearInfo.iPrevZone > 0 );
                    bool bSimSync = ( nearInfo.iZone == 0 );
        
                    if ( bSyncHere && bTimeForSync )
                        sendList.push_back ( pSendPlayer );
    
                    if ( bSimSync )
                        simSendList.push_back ( pSendPlayer );
                }
            }
        }

        // Do pending near->far list moves
        for ( std::vector < CPlayer* > ::const_iterator iter = moveToFarListList.begin (); iter != moveToFarListList.end (); ++iter )
        {
            pPlayer->MovePlayerToPuresyncFarList ( *iter );
        }
    }

    // Relay packet
    if ( !sendList.empty () )
        for ( int i = 0 ; i < g_pBandwidthSettings->iTestSendMultiplier ; i++ )
            CPlayerManager::Broadcast ( Packet, sendList );

    // Update sim data
    CSimControl::UpdatePuresyncSimPlayer ( pPlayer, simSendList );
}

void CGame::Packet_PlayerPuresync ( CPlayerPuresyncPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        pPlayer->NotifyReceivedSync ();
        pPlayer->IncrementPuresync ();

        // Ignore this packet if he should be in a vehicle
        if ( !pPlayer->GetOccupiedVehicle () )
        {
            // Send a returnsync packet to the player that sent it
            // Only every 4 packets.
            if ( ( pPlayer->GetPuresyncCount () % 4 ) == 0 )
                pPlayer->Send ( CReturnSyncPacket ( pPlayer ) );

            // Relay to other players
            RelayPlayerPuresync ( Packet );

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
                for ( unsigned int i = 0; i < MAX_DOORS; ++i )
                {
                    if ( Packet.m_damage.data.bDoorStatesChanged [ i ] )
                        pVehicle->m_ucDoorStates [ i ] = Packet.m_damage.data.ucDoorStates [ i ];
                }
                for ( unsigned int i = 0; i < MAX_WHEELS; ++i )
                {
                    if ( Packet.m_damage.data.bWheelStatesChanged [ i ] )
                        pVehicle->m_ucWheelStates [ i ] = Packet.m_damage.data.ucWheelStates [ i ];
                }
                for ( unsigned int i = 0; i < MAX_PANELS; ++i )
                {
                    if ( Packet.m_damage.data.bPanelStatesChanged [ i ] )
                        pVehicle->m_ucPanelStates [ i ] = Packet.m_damage.data.ucPanelStates [ i ];
                }
                for ( unsigned int i = 0; i < MAX_LIGHTS; ++i )
                {
                    if ( Packet.m_damage.data.bLightStatesChanged [ i ] )
                        pVehicle->m_ucLightStates [ i ] = Packet.m_damage.data.ucLightStates [ i ];
                }

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
        pPlayer->NotifyReceivedSync ();

        // Grab the vehicle
        CVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            // Send a returnsync packet to the player that sent it
            pPlayer->Send ( CReturnSyncPacket ( pPlayer ) );

            // Relay to other players
            RelayPlayerPuresync ( Packet );

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
        // Relay to other players
        RelayKeysync ( Packet );
    }
}


// Relay this (key sync) packet to other players using distance rules
void CGame::RelayKeysync ( CPacket& Packet )
{
    // Make a list of players to send this packet to
    std::vector < CPlayer* > sendList;
    std::vector < CPlayer* > simSendList;
    bool bUseSimSendList = CSimControl::IsSimSystemEnabled ();

    CPlayer* pPlayer = Packet.GetSourcePlayer ();

    //
    // Process near sync
    //
    {
        // Update list of players who need the packet
        pPlayer->UpdateKeysyncNearList ();

        // Use this players keysync near list for sending packets
        std::map < CPlayer*, SKeysyncNearInfo >& nearList = pPlayer->GetKeysyncNearPlayerList ();

        // For each keysync near player
        for ( std::map < CPlayer*, SKeysyncNearInfo > ::iterator it = nearList.begin (); it != nearList.end (); ++it )
        {
            CPlayer* pSendPlayer = it->first;
            if ( !bUseSimSendList )
            {
                // Standard sending
                sendList.push_back ( pSendPlayer );
            }
            else
            {
                SKeysyncNearInfo& nearInfo = it->second;

                //
                // Sim sync relays key sync packets to the other player when he is near
                // Enabling/disabling sim keysync will only take effect for the next key sync packet, so:
                //                           prevNear nowNear
                //      Moving into range:     false   true     Do sync here, sim keysync on
                //      In range:              true    true     No sync here, sim keysync on
                //
                if ( !nearInfo.bPrevIsNear )
                {
                    sendList.push_back ( pSendPlayer );
                    nearInfo.bPrevIsNear = true;
                }

                simSendList.push_back ( pSendPlayer );
            }
        }
    }

    // Relay packet
    if ( !sendList.empty () )
        CPlayerManager::Broadcast ( Packet, sendList );

    // Update sim data
    CSimControl::UpdateKeysyncSimPlayer ( pPlayer, simSendList );
}


void CGame::Packet_Bulletsync ( CBulletsyncPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Make a list of players to send this packet to
        std::vector < CPlayer* > sendList;

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

                // Is this players camera close enough to warrant bulletsync?
                if ( IsPointNearPoint3D ( vecSourcePosition, vecCameraPosition, MAX_BULLETSYNC_DISTANCE ) )
                {
                    // Send the packet to him
                    sendList.push_back ( pSendPlayer );
                }
            }
        }

        CPlayerManager::Broadcast ( Packet, sendList );
    }
}


void CGame::Packet_LuaEvent ( CLuaEventPacket& Packet )
{
    // Grab the source player, even name, element id and the arguments passed
    CPlayer* pCaller = Packet.GetSourcePlayer ();
    const char* szName = Packet.GetName ();
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

            // Ignore if the wrong length
            if ( strlen ( szName ) > MAX_CUSTOMDATA_NAME_LENGTH )
            {
                CLogger::ErrorPrintf( "Received oversized custom data name from %s (%s)", Packet.GetSourcePlayer ()->GetNick (), *SStringX ( szName ).Left ( MAX_CUSTOMDATA_NAME_LENGTH + 1 ) );
                return;
            }
            pElement->SetCustomData ( szName, Value, NULL, true, pSourcePlayer );

            // Tell our clients to update their data. Send to everyone but the one we got this packet from.
            unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szName ) );
            CBitStream BitStream;
            BitStream.pBitStream->WriteCompressed ( usNameLength );
            BitStream.pBitStream->Write ( szName, usNameLength );
            Value.WriteToBitStream ( *BitStream.pBitStream );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_DATA, *BitStream.pBitStream ), pSourcePlayer );
        }
    }
}


void CGame::Packet_DetonateSatchels ( CDetonateSatchelsPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Tell everyone to blow up this guy's satchels
        m_pPlayerManager->BroadcastOnlyJoined ( Packet );
        //Take away their detonator
        CStaticFunctionDefinitions::TakeWeapon( pPlayer, 40 );
    }
}


void CGame::Packet_DestroySatchels ( CDestroySatchelsPacket& Packet )
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Tell everyone to destroy up this player's satchels
        m_pPlayerManager->BroadcastOnlyJoined ( Packet );
        //Take away their detonator
        CStaticFunctionDefinitions::TakeWeapon( pPlayer, 40 );
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
                            case 12: // EXP_TYPE_TINY - RC Vehicles
                            {
                                CVehicle * pVehicle = static_cast < CVehicle * > ( pOrigin );
                                // Is this vehicle not already blown?
                                if ( pVehicle->GetBlowTime () == 0 )
                                {
                                    pVehicle->SetBlowTime ( GetTime () );

                                    // Call the onVehicleExplode event
                                    CLuaArguments Arguments;
                                    pVehicle->CallEvent ( "onVehicleExplode", Arguments );
                                    //Update our engine State
                                    pVehicle->SetEngineOn( false );
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
            // Make a list of players to send this packet to
            std::vector < CPlayer* > sendList;

            // Loop through all the players
            std::list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
            for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
            {
                CPlayer* pSendPlayer = *iter;

                // We tell the reporter to create the explosion too
                // Grab this player's camera position
                CVector vecCameraPosition;
                pSendPlayer->GetCamera ()->GetPosition ( vecCameraPosition );

                // Is this players camera close enough to send?
                if ( IsPointNearPoint3D ( vecPosition, vecCameraPosition, MAX_EXPLOSION_SYNC_DISTANCE ) )
                {
                    // Send the packet to him
                    sendList.push_back ( pSendPlayer );
                }
            }

            CPlayerManager::Broadcast ( Packet, sendList );
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
            if ( pOriginSource )
                vecPosition += pOriginSource->GetPosition ();
        }

        // Make a list of players to send this packet to
        std::vector < CPlayer* > sendList;

        // Loop through all the players
        std::list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
        for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
        {
            CPlayer* pSendPlayer = *iter;

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
                    sendList.push_back ( pSendPlayer );
                }
            }
        }
        CPlayerManager::Broadcast ( Packet, sendList );
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
                                        // Cazomino05: changed to check if the vehicle is in water not player
                                        if ( (pPlayer->IsInWater() || Packet.GetOnWater()) && ( usVehicleModel == VT_SKIMMER ||
                                                                        usVehicleModel == VT_SEASPAR ||
                                                                        usVehicleModel == VT_LEVIATHN ||
                                                                        usVehicleModel == VT_VORTEX ) )
                                        {
                                            fCutoffDistance = 10.0f;
                                            bWarpIn = true;
                                        }
                                        if (usVehicleModel == VT_RCBARON) { // warp in for rc baron.
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
                                            unsigned char ucDoor = Packet.GetDoor();
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
                                                    Arguments.PushNumber ( ucDoor );        // Door
                                                    if ( pVehicle->CallEvent ( "onVehicleStartEnter", Arguments ) )
                                                    {
                                                        // HACK?: check the player's vehicle-action is still the same (not warped in?)
                                                        if ( pPlayer->GetVehicleAction () == CPlayer::VEHICLEACTION_ENTERING )
                                                        {
                                                            // Force the player as the syncer of the vehicle to which they are entering
                                                            m_pUnoccupiedVehicleSync->OverrideSyncer ( pVehicle, pPlayer );

                                                            if ( bWarpIn )
                                                            {
                                                                if ( pOccupant == pPlayer )
                                                                    pVehicle->SetOccupant ( NULL, 0 );

                                                                CStaticFunctionDefinitions::WarpPedIntoVehicle ( pPlayer, pVehicle, 0 );
                                                            }
                                                            else
                                                            {
                                                                // Tell everyone he can start entering the vehicle from his current position
                                                                CVehicleInOutPacket Reply ( ID, 0, VEHICLE_REQUEST_IN_CONFIRMED, ucDoor );
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
                                                        EnterArguments.PushNumber ( ucDoor );       // Door
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
                                                                        CVehicleInOutPacket Reply ( ID, 0, VEHICLE_REQUEST_JACK_CONFIRMED, ucDoor );
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
                                                    Arguments.PushNumber ( ucDoor );            // Door
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
                                                                CVehicleInOutPacket Reply ( ID, ucSeat, VEHICLE_REQUEST_IN_CONFIRMED, ucDoor );
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

                                    //Update our engine State
                                    pVehicle->SetEngineOn( true );

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
                                    unsigned char ucDoor = Packet.GetDoor ();
                                    float fDoorAngle = Packet.GetDoorAngle ();

                                    // Mark that he's in no vehicle
                                    pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
                                    pPlayer->SetOccupiedVehicle ( NULL, 0 );
                                    pVehicle->SetOccupant ( NULL, ucOccupiedSeat );

                                    // Update the door angle.
                                    pVehicle->SetDoorOpenRatio ( ucDoor + 2, fDoorAngle );

                                    // Tell everyone he's in (they should warp him in)
                                    CVehicleInOutPacket Reply ( ID, ucOccupiedSeat, VEHICLE_NOTIFY_IN_ABORT_RETURN, ucDoor );
                                    Reply.SetSourceElement ( pPlayer );
                                    Reply.SetDoorAngle ( fDoorAngle );
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
                                    Arguments.PushNumber ( Packet.GetDoor() );  // door being used
                                    if ( pVehicle->CallEvent ( "onVehicleStartExit", Arguments ) &&
                                        pPlayer->GetOccupiedVehicle () == pVehicle )
                                    {
                                        // Mark him as exiting the vehicle
                                        pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_EXITING );

                                        // Tell everyone he can start exiting the vehicle
                                        CVehicleInOutPacket Reply ( ID, ucOccupiedSeat, VEHICLE_REQUEST_OUT_CONFIRMED, Packet.GetDoor() );
                                        Reply.SetSourceElement ( pPlayer );
                                        m_pPlayerManager->BroadcastOnlyJoined ( Reply );
                                    }
                                    else
                                    {
                                        // Tell him he can't exit (script denied it or he was
                                        // already warped out)
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
                                pPlayer->SetVehicleAction( CPlayer::VEHICLEACTION_NONE );
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
                                unsigned char ucDoor = Packet.GetDoor ();
                                float fAngle = Packet.GetDoorAngle ();
                                CPed* pJacked = pVehicle->GetOccupant ( 0 );

                                // Mark that the jacker is in no vehicle
                                pPlayer->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
                                pPlayer->SetOccupiedVehicle ( NULL, 0 );
                                pPlayer->SetJackingVehicle ( NULL );
                                pVehicle->SetJackingPlayer ( NULL );

                                // Set the door angle.
                                pVehicle->SetDoorOpenRatio ( ucDoor, fAngle );

                                // Tell everyone he aborted
                                CVehicleInOutPacket Reply ( ID, 0, VEHICLE_NOTIFY_IN_ABORT_RETURN, ucDoor );
                                Reply.SetSourceElement ( pPlayer );
                                Reply.SetDoorAngle ( fAngle );
                                m_pPlayerManager->BroadcastOnlyJoined ( Reply );

                                // The jacked is still inside?
                                if ( pJacked )
                                {
                                    // Did he already start jacking him? (we're trusting the client here!)
                                    if ( Packet.GetStartedJacking () == 1 )
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
                                    pJacked->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
                                }

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
    unsigned short usDataLength = 0;

    if ( m_pMainConfig->IsVoiceEnabled() ) // Shouldn't really be receiving voice packets at all if voice is disabled
    {
        usDataLength = Packet.GetDataLength();

        if ( usDataLength > 0 )
        {
            CPlayer* pPlayer = Packet.GetSourcePlayer();

            if ( pPlayer )
            {
                if ( pPlayer->IsVoiceMuted() ) // Shouldn't be receiving voice packets, player should be muted client side
                    return;

                // Is it the start of the voice stream?
                if ( pPlayer->GetVoiceState () == VOICESTATE_IDLE )
                {
                    // Trigger the related event
                    CLuaArguments Arguments;
                    bool bEventTriggered = pPlayer->CallEvent( "onPlayerVoiceStart", Arguments, pPlayer);

                    if ( !bEventTriggered ) // Was the event cancelled?
                    {
                        return;
                    }

                    // Our voice state has changed
                    pPlayer->SetVoiceState ( VOICESTATE_TRANSMITTING );
                }

                if ( pPlayer->GetVoiceState () == VOICESTATE_TRANSMITTING ) // If we reach here, and we're still in idle state, then the event was cancelled
                {
                    const unsigned char* pBuffer = Packet.GetData();
                    CVoiceDataPacket VoicePacket(pPlayer, pBuffer, usDataLength);

                    // Make list of players to send the voice packet to
                    std::set < CPlayer* > playerSendMap;

                    list < CElement* > ::const_iterator iter = pPlayer->IterBroadcastListBegin() ;
                    for ( ; iter != pPlayer->IterBroadcastListEnd(); iter++ )
                    {
                        CElement* pBroadcastElement = *iter;
                        if ( IS_TEAM( pBroadcastElement ) )
                        {
                            // Add team members
                            CTeam* pTeam = static_cast < CTeam* > ( pBroadcastElement );
                            list < CPlayer* > ::const_iterator iter = pTeam->PlayersBegin ();
                            for ( ; iter != pTeam->PlayersEnd (); iter++ )
                                playerSendMap.insert ( *iter );
                        }
                        else if ( IS_PLAYER( pBroadcastElement ) )
                        {
                            // Add a player
                            playerSendMap.insert ( static_cast < CPlayer* > ( pBroadcastElement ) );
                        }
                        else
                        {
                            // Add element decendants
                            std::vector < CPlayer* > descendantList;
                            pBroadcastElement->GetDescendantsByType ( descendantList, false, CElement::PLAYER );
                            for ( std::vector < CPlayer* >::const_iterator iter = descendantList.begin() ; iter != descendantList.end() ; ++iter )
                            {
                                playerSendMap.insert ( *iter );
                            }
                        }
                    }

                    // Filter out ourselves and ignored
                    for ( std::set < CPlayer* >::iterator iter = playerSendMap.begin () ; iter != playerSendMap.end () ; )
                    {
                        if ( *iter == pPlayer || (*iter)->IsPlayerIgnoringElement ( pPlayer ) )
                            playerSendMap.erase ( iter++ );
                        else
                            ++iter;
                    }

                    // Send to all players in the send list
                    CPlayerManager::Broadcast ( VoicePacket, playerSendMap );
                }
            }
        }
    }
}

void CGame::Packet_Voice_End ( CVoiceEndPacket& Packet )
{
    if ( m_pMainConfig->IsVoiceEnabled() ) // Shouldn't really be receiving voice packets at all if voice is disabled
    {
        CPlayer* pPlayer = Packet.GetSourcePlayer();

        if ( pPlayer )
        {
            CLuaArguments Arguments;
            pPlayer->CallEvent( "onPlayerVoiceStop", Arguments, pPlayer);

            // Reset our voice state
            pPlayer->SetVoiceState ( VOICESTATE_IDLE );

            CVoiceEndPacket EndPacket(pPlayer);

            // Make list of players to send the voice packet to
            std::set < CPlayer* > playerSendMap;

            list < CElement* > ::const_iterator iter = pPlayer->IterBroadcastListBegin() ;
            for ( ; iter != pPlayer->IterBroadcastListEnd(); iter++ )
            {
                CElement* pBroadcastElement = *iter;
                if ( IS_TEAM( pBroadcastElement ) )
                {
                    // Add team members
                    CTeam* pTeam = static_cast < CTeam* > ( pBroadcastElement );
                    list < CPlayer* > ::const_iterator iter = pTeam->PlayersBegin ();
                    for ( ; iter != pTeam->PlayersEnd (); iter++ )
                        playerSendMap.insert ( *iter );
                }
                else if ( IS_PLAYER( pBroadcastElement ) )
                {
                    // Add a player
                    playerSendMap.insert ( static_cast < CPlayer* > ( pBroadcastElement ) );
                }
                else
                {
                    // Add element decendants
                    std::vector < CPlayer* > descendantList;
                    pBroadcastElement->GetDescendantsByType ( descendantList, false, CElement::PLAYER );
                    for ( std::vector < CPlayer* >::const_iterator iter = descendantList.begin() ; iter != descendantList.end() ; ++iter )
                    {
                        playerSendMap.insert ( *iter );
                    }
                }
            }

            // Filter out ourselves and ignored
            for ( std::set < CPlayer* >::iterator iter = playerSendMap.begin () ; iter != playerSendMap.end () ; )
            {
                if ( *iter == pPlayer || (*iter)->IsPlayerIgnoringElement ( pPlayer ) )
                    playerSendMap.erase ( iter++ );
                else
                    ++iter;
            }

            // Send to all players in the send list
            CPlayerManager::Broadcast ( EndPacket, playerSendMap );

        }
    }
}



void CGame::Packet_CameraSync ( CCameraSyncPacket & Packet )
{
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        pPlayer->NotifyReceivedSync ();

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


void CGame::Packet_PlayerTransgression ( CPlayerTransgressionPacket & Packet )
{
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // If ac# not disabled on this server, do a kick
        if ( !g_pGame->GetConfig ()->IsDisableAC ( SString ( "%d", Packet.m_uiLevel ) ) )
        {
            CStaticFunctionDefinitions::KickPlayer ( pPlayer, NULL, Packet.m_strMessage );
        }
    }
}


void CGame::Packet_PlayerDiagnostic ( CPlayerDiagnosticPacket & Packet )
{
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // If diagnosticis enabled on this server, log it
        if ( g_pGame->GetConfig ()->IsEnableDiagnostic ( SString ( "%d", Packet.m_uiLevel ) ) )
        {
            SString strMessageCombo ( "DIAGNOSTIC: %s #%d %s\n", pPlayer->GetNick (), Packet.m_uiLevel, Packet.m_strMessage.c_str () );
            CLogger::LogPrint ( strMessageCombo );
        }
    }
}


void CGame::Packet_PlayerScreenShot ( CPlayerScreenShotPacket & Packet )
{
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        if ( Packet.m_ucStatus != 1 )
        {
            // disabled or minimized
            CResource* pResource = g_pGame->GetResourceManager ()->GetResource ( Packet.m_strResourceName );
            if ( pResource )
            {
                CLuaArguments Arguments;
                Arguments.PushResource ( pResource );
                Arguments.PushString ( Packet.m_ucStatus == 2 ? "minimized" : "disabled" );
                Arguments.PushBoolean ( false );
                Arguments.PushNumber ( static_cast < double > ( Packet.m_llServerGrabTime ) );
                Arguments.PushString ( Packet.m_strTag );
                pPlayer->CallEvent ( "onPlayerScreenShot", Arguments );
            }
        }
        else
        if ( Packet.m_ucStatus == 1 )
        {
            // Get in-progress info
            SScreenShotInfo& info = pPlayer->GetScreenShotInfo ();

            // Validate
            if ( !info.bInProgress
                    || info.usNextPartNumber != Packet.m_usPartNumber
                    || info.usScreenShotId != Packet.m_usScreenShotId
                    )
            {
                info.bInProgress = false;
                info.buffer.Clear ();

                // Check if new start
                if ( Packet.m_usPartNumber == 0 )
                {
                    info.bInProgress = true;
                    info.usNextPartNumber = 0;
                    info.usScreenShotId = Packet.m_usScreenShotId;

                    info.llTimeStamp = Packet.m_llServerGrabTime;
                    info.uiTotalBytes = Packet.m_uiTotalBytes;
                    info.usTotalParts = Packet.m_usTotalParts;
                    info.strResourceName = Packet.m_strResourceName;
                    info.strTag = Packet.m_strTag;
                }
            }

            // Add data if valid
            if ( info.bInProgress )
            {
                info.buffer += Packet.m_buffer;
                info.usNextPartNumber++;

                // Finished?
                if ( info.usNextPartNumber == info.usTotalParts )
                {
                    CResource* pResource = g_pGame->GetResourceManager ()->GetResource ( info.strResourceName );
                    if ( pResource && info.uiTotalBytes == info.buffer.GetSize () )
                    {
                        CLuaArguments Arguments;
                        Arguments.PushResource ( pResource );
                        Arguments.PushString ( "ok" );
                        Arguments.PushString ( std::string ( info.buffer.GetData (), info.buffer.GetSize () ) );
                        Arguments.PushNumber ( static_cast < double > ( info.llTimeStamp ) );
                        Arguments.PushString ( info.strTag );
                        pPlayer->CallEvent ( "onPlayerScreenShot", Arguments );
                    }

                    info.bInProgress = false;
                    info.buffer.Clear ();
                }
            }
        }
    }
}

void CGame::Packet_PlayerModInfo ( CPlayerModInfoPacket & Packet )
{
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Make itemList table
        CLuaArguments resultItemList;
        for ( std::vector < SModInfoItem >::iterator iter = Packet.m_ModInfoItemList.begin(); iter != Packet.m_ModInfoItemList.end(); ++iter )
        {
            const SModInfoItem& in = *iter;

            // Make item table
            CLuaArguments resultItem;

            resultItem.PushString ( "id" );
            resultItem.PushNumber ( in.usId );

            resultItem.PushString ( "name" );
            resultItem.PushString ( in.strName );

            resultItem.PushString ( "hash" );
            resultItem.PushNumber ( in.uiHash );

            if ( in.bHasSize )
            {
                resultItem.PushString ( "sizeX" );
                resultItem.PushNumber ( in.vecSize.fX - fmod ( (double)in.vecSize.fX, 0.01 ) );

                resultItem.PushString ( "sizeY" );
                resultItem.PushNumber ( in.vecSize.fY - fmod ( (double)in.vecSize.fY, 0.01 ) );

                resultItem.PushString ( "sizeZ" );
                resultItem.PushNumber ( in.vecSize.fZ - fmod ( (double)in.vecSize.fZ, 0.01 ) );
            }

            resultItemList.PushNumber ( resultItemList.Count () / 2 + 1 );
            resultItemList.PushTable ( &resultItem );
        }

        CLuaArguments Arguments;
        Arguments.PushString ( Packet.m_strInfoType );
        Arguments.PushTable ( &resultItemList );
        pPlayer->CallEvent ( "onPlayerModInfo", Arguments );
    }
}


void CGame::PlayerCompleteConnect ( CPlayer* pPlayer, bool bSuccess, const char* szError )
{
    SString strIPAndSerial( "IP: %s  Serial: %s  Version: %s", pPlayer->GetSourceIP (), pPlayer->GetSerial ().c_str (), pPlayer->GetPlayerVersion ().c_str () );
    if ( bSuccess )
    {
        // Call the onPlayerConnect event. If it returns false, disconnect the player
        CLuaArguments Arguments;
        Arguments.PushString ( pPlayer->GetNick () );
        Arguments.PushString ( pPlayer->GetSourceIP () );
        Arguments.PushString ( pPlayer->GetSerialUser ().c_str() );
        Arguments.PushString ( pPlayer->GetSerial ().c_str() );
        Arguments.PushNumber ( pPlayer->GetMTAVersion () );
        Arguments.PushString ( pPlayer->GetPlayerVersion () );
        if ( !g_pGame->GetMapManager()->GetRootElement()->CallEvent ( "onPlayerConnect", Arguments ) )
        {
            // event cancelled, disconnect the player
            CLogger::LogPrintf ( "CONNECT: %s failed to connect. (onPlayerConnect event cancelled) (%s)\n", pPlayer->GetNick(), strIPAndSerial.c_str () );
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
        CLogger::LogPrintf ( "CONNECT: %s connected (%s)\n", pPlayer->GetNick(), strIPAndSerial.c_str () );

        // Send him the join details
        pPlayer->Send ( CPlayerConnectCompletePacket () );

        // The player is spawned when he's connected, just the Camera is not faded in/not targetting
        pPlayer->SetSpawned ( true );
    }
    else
    {
        CLogger::LogPrintf ( "CONNECT: %s failed to connect (Invalid serial) (%s)\n", pPlayer->GetNick(), strIPAndSerial.c_str () );
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

void CGame::SetGlitchEnabled ( const std::string& strGlitch, bool bEnabled )
{
    eGlitchType cGlitch = m_GlitchNames[strGlitch];
    assert ( cGlitch >= 0 && cGlitch < NUM_GLITCHES );
    m_Glitches[cGlitch] = bEnabled;
}

bool CGame::IsGlitchEnabled ( const std::string& strGlitch )
{
    eGlitchType cGlitch = m_GlitchNames[strGlitch];
    assert ( cGlitch >= 0 && cGlitch < NUM_GLITCHES );
    return m_Glitches[cGlitch] ? true : false;
}
bool CGame::IsGlitchEnabled ( eGlitchType cGlitch )
{
    assert ( cGlitch >= 0 && cGlitch < NUM_GLITCHES );
    return m_Glitches[cGlitch] || false;
}

void CGame::SetCloudsEnabled ( bool bEnabled )
{
    m_bCloudsEnabled = bEnabled;
}
bool CGame::GetCloudsEnabled ( void )
{
    return m_bCloudsEnabled;
}


//
// Handle basic backup of databases and config files
//
void CGame::HandleBackup ( void )
{
    // Get backup vars
    SString strBackupPath = PathConform ( m_pMainConfig->GetBackupPath () ).TrimEnd ( PATH_SEPERATOR );
    int iBackupInterval = m_pMainConfig->GetBackupInterval ();
    uint uiBackupAmount = m_pMainConfig->GetBackupAmount ();
    if ( iBackupInterval == 0 || uiBackupAmount == 0 )
        return;

    // Check if brand new installation
    CModManager* pModManager = g_pServerInterface->GetModManager ();
    if ( !DirectoryExists ( m_pMainConfig->GetSystemDatabasesPath () ) &&
         !FileExists ( m_pMainConfig->GetLogFile () ) &&
         !FileExists ( pModManager->GetAbsolutePath ( "internal.db" ) ) )
    {
        return;
    }

    // Determine date now
    time_t secondsNow = time ( NULL );

    // Determine last backup date
    std::vector < SString > fileList = FindFiles ( strBackupPath + "/", true, false );

    // Check each file name is a valid backup name
    for ( uint f = 0 ; f < fileList.size () ; f++ )
    {
        SString strName = fileList[f];
        const SString strCheck = "0000-00-00.zip";
        for ( uint i = 0 ; i < Min ( strCheck.length (), strName.length () ) ; i++ )
            if ( !isdigit( (uchar)strName[i] ) || !isdigit( (uchar)strCheck[i] ) )
                if ( strName[i] != strCheck[i] )
                {
                    ListRemoveIndex ( fileList, f-- );
                    break;
                }
    }

    // Alpha sort
    std::sort ( fileList.rbegin (), fileList.rend () );

    // Check date of last backup
    if ( !fileList.empty () )
    {
        SString strNewest = fileList.front ();
        tm timeinfo;
        memset ( &timeinfo, 0, sizeof ( timeinfo ) );
        timeinfo.tm_year    = atoi ( strNewest.SubStr ( 0, 4 ) ) - 1900;
        timeinfo.tm_mon     = atoi ( strNewest.SubStr ( 5, 2 ) ) - 1;
        timeinfo.tm_mday    = atoi ( strNewest.SubStr ( 8, 2 ) );

        time_t timeSinceBackup = secondsNow - mktime ( &timeinfo );
        if ( timeSinceBackup < iBackupInterval * 86400L )
            return;     // No backup required
    }

    // Make target file name
    tm* tmp = gmtime ( &secondsNow );
    char outstr[200] = { 0 };
    strftime ( outstr, sizeof ( outstr ), "%Y-%m-%d", tmp );
    SString strDateNow = outstr;
    SString strBackupZip = PathJoin ( strBackupPath, strDateNow + ".zip" );
    SString strTempZip = PathJoin ( strBackupPath, strDateNow + "_temp.zip" );

    if ( FileExists ( strBackupZip ) )
        return;     // Can't do backup as target file already exists

    MkDir ( strBackupPath );

    // Delete previous temp zip if any
    FileDelete ( strTempZip );

    CZipMaker zipMaker ( strTempZip );
    if ( !zipMaker.IsValid () )
        return;     // Can't do backup as can't create target zip

    CLogger::LogPrintfNoStamp ( "Please wait...\n" );

    // Backup config files
    zipMaker.InsertFile ( pModManager->GetAbsolutePath ( "mtaserver.conf" ),        PathJoin ( "config", "mtaserver.conf" ) );
    zipMaker.InsertFile ( m_pMainConfig->GetAccessControlListFile (),               PathJoin ( "config", "acl.xml" ) );
    zipMaker.InsertFile ( pModManager->GetAbsolutePath ( FILENAME_BANLIST ),        PathJoin ( "config", "banlist.xml" ) );
    zipMaker.InsertFile ( pModManager->GetAbsolutePath ( "editor.conf" ),           PathJoin ( "config", "editor.conf" ) );
    zipMaker.InsertFile ( pModManager->GetAbsolutePath ( "editor_acl.xml" ),        PathJoin ( "config", "editor_acl.xml" ) );
    zipMaker.InsertFile ( pModManager->GetAbsolutePath ( "local.conf" ),            PathJoin ( "config", "local.conf" ) );
    zipMaker.InsertFile ( m_pMainConfig->GetIdFile (),                              PathJoin ( "config", "server-id.keys" ) );
    zipMaker.InsertFile ( pModManager->GetAbsolutePath ( FILENAME_SETTINGS ),       PathJoin ( "config", "settings.xml" ) );
    zipMaker.InsertFile ( pModManager->GetAbsolutePath ( "vehiclecolors.conf" ),    PathJoin ( "config", "vehiclecolors.conf" ) );

    // Backup database files
    zipMaker.InsertDirectoryTree ( m_pMainConfig->GetGlobalDatabasesPath (),        PathJoin ( "databases", "global" ) );
    zipMaker.InsertDirectoryTree ( m_pMainConfig->GetSystemDatabasesPath (),        PathJoin ( "databases", "system" ) );
    zipMaker.InsertFile ( pModManager->GetAbsolutePath ( "internal.db" ),           PathJoin ( "databases", "other", "internal.db" ) );
    zipMaker.InsertFile ( pModManager->GetAbsolutePath ( "registry.db" ),           PathJoin ( "databases", "other", "registry.db" ) );

    zipMaker.Close ();

    // Rename temp file to final name
    FileRename ( strTempZip, strBackupZip );

    // Remove backups over min required
    while ( fileList.size () >= uiBackupAmount )
    {
        SString strOldest = fileList.back ();
        FileDelete ( PathJoin ( strBackupPath, strOldest ) );
        fileList.pop_back ();
    }
}


//
// Toggle latent send mode
//
void CGame::EnableLatentSends ( bool bEnabled, int iBandwidth, CLuaMain* pLuaMain )
{
    m_bLatentSendsEnabled = bEnabled && iBandwidth;
    m_iLatentSendsBandwidth = iBandwidth;
    m_pLatentSendsLuaMain = pLuaMain;
}


//
// Maybe route though LatentTransferManager
//
bool CGame::SendPacket ( unsigned char ucPacketID, const NetServerPlayerID& playerID, NetBitStreamInterface* pBitStream, bool bBroadcast, NetServerPacketPriority packetPriority, NetServerPacketReliability packetReliability, ePacketOrdering packetOrdering )
{
    if ( !m_bLatentSendsEnabled )
        return g_pNetServer->SendPacket ( ucPacketID, playerID, pBitStream, bBroadcast, packetPriority, packetReliability, packetOrdering );
    else
        GetLatentTransferManager ()->AddSend ( playerID, ucPacketID, pBitStream, m_iLatentSendsBandwidth, m_pLatentSendsLuaMain );
    return true;
}

