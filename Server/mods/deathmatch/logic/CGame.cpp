/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CGame.cpp
 *  PURPOSE:     Server game class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "../utils/COpenPortsTester.h"
#include "../utils/CMasterServerAnnouncer.h"
#include "../utils/CHqComms.h"
#include "../utils/CFunctionUseLogger.h"
#include "net/SimHeaders.h"
#include <signal.h>

#define MAX_BULLETSYNC_DISTANCE      400.0f
#define MAX_EXPLOSION_SYNC_DISTANCE  400.0f
#define MAX_PROJECTILE_SYNC_DISTANCE 400.0f

#define RELEASE_MIN_CLIENT_VERSION            "1.4.0-0.00000"
#define BULLET_SYNC_MIN_CLIENT_VERSION        "1.3.0-9.04311"
#define VEH_EXTRAPOLATION_MIN_CLIENT_VERSION  "1.3.0-9.04460"
#define ALT_PULSE_ORDER_MIN_CLIENT_VERSION    "1.3.1-9.04913"
#define HIT_ANIM_CLIENT_VERSION               "1.3.2"
#define SNIPER_BULLET_SYNC_MIN_CLIENT_VERSION "1.3.5-9.06054"
#define SPRINT_FIX_MIN_CLIENT_VERSION         "1.3.5-9.06277"
#define DRIVEBY_HITBOX_FIX_MIN_CLIENT_VERSION "1.4.0-5.06399"
#define SHOTGUN_DAMAGE_FIX_MIN_CLIENT_VERSION "1.5.1"

// This include needs to see the defines
#include "packet_process/all.hpp"

CGame* g_pGame = NULL;

char          szProgress[4] = {'-', '\\', '|', '/'};
unsigned char ucProgress = 0;
unsigned char ucProgressSkip = 0;

pthread_mutex_t mutexhttp;

#ifdef WIN32
BOOL WINAPI ConsoleEventHandler(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_CLOSE_EVENT)
    {
        // Close button pressed or task ended in task manager
        if (g_pGame)
        {
            // Warning message if server started
            if (g_pGame->IsServerFullyUp())
            {
                printf("\n** TERMINATING SERVER WITHOUT SAVING **\n");
                printf("\nUse Ctrl-C next time!\n");
                Sleep(3000);
            }
        }
        // Don't call g_pGame->SetIsFinished() as Windows could terminate the process mid-shutdown
        return TRUE;
    }
    else if (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_BREAK_EVENT)
    {
        if (g_pGame)
        {
            // If we have nothing in the input buffer, let's close the server, otherwise just reset input
            if (!g_pServerInterface->ResetInput())
            {
                // Graceful close on Ctrl-C or Ctrl-Break
                g_pGame->SetIsFinished(true);
            }
            return TRUE;
        }
    }
    return FALSE;
}
#else
void sighandler(int sig)
{
    if (sig == SIGTERM || sig == SIGINT)
    {
        // If we received a Ctrl-C, let's try resetting input buffer first, otherwise close the server
        if (g_pGame && (sig != SIGINT || (sig == SIGINT && !g_pServerInterface->ResetInput())))
        {
            // Graceful close on Ctrl-C or 'kill'
            g_pGame->SetIsFinished(true);
        }
    }
}
#endif

CGame::CGame() : m_FloodProtect(4, 30000, 30000)            // Max of 4 connections per 30 seconds, then 30 second ignore
{
    // Set our global pointer
    g_pGame = this;

    m_bServerFullyUp = false;

    // Initialize random number generator and time
    RandomizeRandomSeed();

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
    m_pRPCFunctions = NULL;
    m_pLanBroadcast = NULL;
    m_pPedSync = NULL;
    m_pWaterManager = NULL;
    m_pWeaponStatsManager = NULL;
    m_pBuildingRemovalManager = NULL;
    m_pCustomWeaponManager = NULL;
    m_pFunctionUseLogger = NULL;
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
    m_bOverrideMoonSize = false;

    m_pASE = NULL;
    ResetMapInfo();
    m_usFPS = 0;
    m_usFrames = 0;
    m_llLastFPSTime = 0;
    m_szCurrentFileName = NULL;
    m_pConsoleClient = NULL;
    m_bIsFinished = false;

    // Setup game glitch defaults ( false = disabled )
    m_Glitches[GLITCH_QUICKRELOAD] = false;
    m_Glitches[GLITCH_FASTFIRE] = false;
    m_Glitches[GLITCH_FASTMOVE] = false;
    m_Glitches[GLITCH_CROUCHBUG] = false;
    m_Glitches[GLITCH_CLOSEDAMAGE] = false;
    m_Glitches[GLITCH_HITANIM] = false;
    m_Glitches[GLITCH_FASTSPRINT] = false;
    m_Glitches[GLITCH_BADDRIVEBYHITBOX] = false;
    m_Glitches[GLITCH_QUICKSTAND] = false;
    m_Glitches[GLITCH_KICKOUTOFVEHICLE_ONMODELREPLACE] = false;
    for (int i = 0; i < WEAPONTYPE_LAST_WEAPONTYPE; i++)
        m_JetpackWeapons[i] = false;

    m_JetpackWeapons[WEAPONTYPE_MICRO_UZI] = true;
    m_JetpackWeapons[WEAPONTYPE_TEC9] = true;
    m_JetpackWeapons[WEAPONTYPE_PISTOL] = true;
    // Glitch names (for Lua interface)
    m_GlitchNames["quickreload"] = GLITCH_QUICKRELOAD;
    m_GlitchNames["fastfire"] = GLITCH_FASTFIRE;
    m_GlitchNames["fastmove"] = GLITCH_FASTMOVE;
    m_GlitchNames["crouchbug"] = GLITCH_CROUCHBUG;
    m_GlitchNames["highcloserangedamage"] = GLITCH_CLOSEDAMAGE;
    m_GlitchNames["hitanim"] = GLITCH_HITANIM;
    m_GlitchNames["fastsprint"] = GLITCH_FASTSPRINT;
    m_GlitchNames["baddrivebyhitbox"] = GLITCH_BADDRIVEBYHITBOX;
    m_GlitchNames["quickstand"] = GLITCH_QUICKSTAND;
    m_GlitchNames["kickoutofvehicle_onmodelreplace"] = GLITCH_KICKOUTOFVEHICLE_ONMODELREPLACE;

    m_bCloudsEnabled = true;

    m_pOpenPortsTester = NULL;

    m_bTrafficLightsLocked = false;
    m_ucTrafficLightState = 0;
    m_llLastTrafficUpdate = 0;

    m_bOcclusionsEnabled = true;

    memset(&m_bGarageStates[0], 0, sizeof(m_bGarageStates));

    // init our mutex
    pthread_mutex_init(&mutexhttp, NULL);
}

void CGame::ResetMapInfo()
{
    // Add variables to get reset in resetMapInfo here
    m_fGravity = 0.008f;
    m_fGameSpeed = 1.0f;
    m_fJetpackMaxHeight = 100;
    m_fAircraftMaxHeight = 800;
    m_fAircraftMaxVelocity = 1.5f;

    if (m_pWaterManager)
    {
        m_pWaterManager->ResetWorldWaterLevel();
        m_pWaterManager->SetGlobalWaveHeight(0.0f);
    }

    m_ucSkyGradientTR = 0, m_ucSkyGradientTG = 0, m_ucSkyGradientTB = 0;
    m_ucSkyGradientBR = 0, m_ucSkyGradientBG = 0, m_ucSkyGradientBB = 0;
    m_bHasSkyGradient = false;
    m_HeatHazeSettings = SHeatHazeSettings();
    m_bHasHeatHaze = false;
    m_bCloudsEnabled = true;

    m_bTrafficLightsLocked = false;
    m_ucTrafficLightState = 0;
    m_llLastTrafficUpdate = 0;

    g_pGame->SetHasWaterColor(false);
    g_pGame->SetInteriorSoundsEnabled(true);
    g_pGame->SetHasFarClipDistance(false);
    g_pGame->SetHasFogDistance(false);
    g_pGame->SetHasRainLevel(false);
    g_pGame->SetHasSunColor(false);
    g_pGame->SetHasSunSize(false);
    g_pGame->SetHasWindVelocity(false);
    g_pGame->SetHasMoonSize(false);
}

CGame::~CGame()
{
    m_bBeingDeleted = true;

    // Stop the web server first to avoid threading issues
    if (m_pHTTPD)
        m_pHTTPD->StopHTTPD();

    // Stop the performance stats modules
    if (CPerfStatManager::GetSingleton() != NULL)
        CPerfStatManager::GetSingleton()->Stop();

    // Stop and flush sim packet handling
    CSimControl::EnableSimSystem(false);

    // Disconnect all players
    std::list<CPlayer*>::const_iterator iter = m_pPlayerManager->IterBegin();
    for (; iter != m_pPlayerManager->IterEnd(); iter++)
        DisconnectPlayer(this, **iter, CPlayerDisconnectedPacket::SHUTDOWN);

    // Stop networking
    Stop();

    // Stop async task scheduler
    SAFE_DELETE(m_pAsyncTaskScheduler);

    // Destroy our stuff
    SAFE_DELETE(m_pResourceManager);

    // Delete everything we have undeleted
    m_ElementDeleter.DoDeleteAll();
    SAFE_DELETE(m_pUnoccupiedVehicleSync);
    SAFE_DELETE(m_pPedSync);
#ifdef WITH_OBJECT_SYNC
    SAFE_DELETE(m_pObjectSync);
#endif
    SAFE_DELETE(m_pConsole);
    SAFE_DELETE(m_pLuaManager);
    SAFE_DELETE(m_pMapManager);
    SAFE_DELETE(m_pRemoteCalls);
    SAFE_DELETE(m_pPacketTranslator);
    SAFE_DELETE(m_pMarkerManager);
    SAFE_DELETE(m_pRadarAreaManager);
    SAFE_DELETE(m_pPlayerManager);
    SAFE_DELETE(m_pVehicleManager);
    SAFE_DELETE(m_pHandlingManager);
    SAFE_DELETE(m_pPickupManager);
    SAFE_DELETE(m_pObjectManager);
    SAFE_DELETE(m_pColManager);
    SAFE_DELETE(m_pBlipManager);
    SAFE_DELETE(m_pClock);
    SAFE_DELETE(m_pScriptDebugging);
    SAFE_DELETE(m_pBanManager);
    SAFE_DELETE(m_pTeamManager);
    SAFE_DELETE(m_pMainConfig);
    if (m_pRegistryManager)
        m_pRegistryManager->CloseRegistry(m_pRegistry);
    m_pRegistry = NULL;
    SAFE_DELETE(m_pConsoleClient);
    SAFE_DELETE(m_pAccountManager);
    SAFE_DELETE(m_pRegistryManager);
    SAFE_DELETE(m_pDatabaseManager);
    SAFE_DELETE(m_pLuaCallbackManager);
    SAFE_DELETE(m_pRegisteredCommands);
    SAFE_DELETE(m_pPedManager);
    SAFE_DELETE(m_pLatentTransferManager);
    SAFE_DELETE(m_pDebugHookManager);
    SAFE_DELETE(m_pHTTPD);
    SAFE_DELETE(m_pACLManager);
    SAFE_DELETE(m_pGroups);
    SAFE_DELETE(m_pZoneNames);
    SAFE_DELETE(m_pASE);
    SAFE_DELETE(m_pSettings);
    SAFE_DELETE(m_pRPCFunctions);
    SAFE_DELETE(m_pWaterManager);
    SAFE_DELETE(m_pWeaponStatsManager);
    SAFE_DELETE(m_pBuildingRemovalManager);
    SAFE_DELETE(m_pCustomWeaponManager);
    SAFE_DELETE(m_pFunctionUseLogger);
    SAFE_DELETE(m_pOpenPortsTester);
    SAFE_DELETE(m_pMasterServerAnnouncer);
    SAFE_DELETE(m_pASE);
    SAFE_RELEASE(m_pHqComms);
    CSimControl::Shutdown();

    // Clear our global pointer
    g_pGame = NULL;

// Remove our console control handler
#ifdef WIN32
    SetConsoleCtrlHandler(ConsoleEventHandler, FALSE);
#else
    signal(SIGTERM, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);
#endif
}

void CGame::GetTag(char* szInfoTag, int iInfoTag)
{
    // Construct the info tag
    SString strInfoTag("%c[%c%c%c] MTA: San Andreas %c:%c: %d/%d players %c:%c: %u resources", 132, 135, szProgress[ucProgress], 132, 130, 130,
                       m_pPlayerManager->Count(), m_pMainConfig->GetMaxPlayers(), 130, 130, m_pResourceManager->GetResourceLoadedCount());

    if (!GetConfig()->GetThreadNetEnabled())
    {
        strInfoTag += SString(" %c:%c: %u fps", 130, 130, g_pGame->GetServerFPS());
    }
    else
    {
        strInfoTag += SString(" %c:%c: %u fps (%u)", 130, 130, g_pGame->GetSyncFPS(), g_pGame->GetServerFPS());
    }

    STRNCPY(szInfoTag, *strInfoTag, iInfoTag);
}

void CGame::HandleInput(char* szCommand)
{
    // Lock the critical section so http server won't interrupt in the middle of our pulse
    Lock();
    // Handle the input
    m_pConsole->HandleInput(szCommand, m_pConsoleClient, m_pConsoleClient);

    // Unlock the critical section again
    Unlock();
}

void CGame::DoPulse()
{
    // Lock the critical section so http server won't interrupt in the middle of our pulse
    Lock();

    UpdateModuleTickCount64();

    // Calculate FPS
    long long llCurrentTime = SharedUtil::GetModuleTickCount64();
    long long ulDiff = llCurrentTime - m_llLastFPSTime;

    // Calculate the server-side fps
    if (ulDiff >= 1000)
    {
        m_usFPS = m_usFrames;
        m_usFrames = 0;
        m_llLastFPSTime = llCurrentTime;
    }
    m_usFrames++;

    // Update the progress rotator
    uchar  ucDelta = (uchar)llCurrentTime - ucProgressSkip;
    ushort usReqDelta = 80 - (100 - std::min<ushort>(100, m_usFPS)) / 5;

    if (ucDelta > usReqDelta)
    {
        // Clamp ucProgress between 0 and 3
        ucProgress = (ucProgress + 1) & 3;
        ucProgressSkip = (uchar)llCurrentTime;
    }

    // Handle critical things
    CSimControl::DoPulse();
    CNetBufferWatchDog::DoPulse();

    CLOCK_SET_SECTION("CGame::DoPulse");
    CLOCK1("HTTPDownloadManager");
    GetRemoteCalls()->ProcessQueuedFiles();
    g_pNetServer->GetHTTPDownloadManager(EDownloadMode::ASE)->ProcessQueuedFiles();
    UNCLOCK1("HTTPDownloadManager");

    CLOCK_CALL1(m_pPlayerManager->DoPulse(););

    // Pulse the net interface
    CLOCK_CALL1(g_pNetServer->DoPulse(););

    if (m_pLanBroadcast)
    {
        CLOCK_CALL1(m_pLanBroadcast->DoPulse(););
    }

    // Pulse our stuff
    CLOCK_CALL1(m_pMapManager->DoPulse(););
    CLOCK_CALL1(m_pUnoccupiedVehicleSync->DoPulse(););
    CLOCK_CALL1(m_pPedSync->DoPulse(););
#ifdef WITH_OBJECT_SYNC
    CLOCK_CALL1(m_pObjectSync->DoPulse(););
#endif
    CLOCK_CALL1(m_pBanManager->DoPulse(););
    CLOCK_CALL1(m_pAccountManager->DoPulse(););
    CLOCK_CALL1(m_pRegistryManager->DoPulse(););
    CLOCK_CALL1(m_pACLManager->DoPulse(););

    // Handle the traffic light sync
    if (m_bTrafficLightsLocked == false)
    {
        CLOCK_CALL1(ProcessTrafficLights(llCurrentTime););
    }

    // Pulse ASE
    if (m_pASE)
    {
        CLOCK_CALL1(m_pASE->DoPulse(););
    }

    // Pulse the scripting system
    if (m_pLuaManager)
    {
        CLOCK_CALL1(m_pLuaManager->DoPulse(););
    }

    CLOCK_CALL1(m_pDatabaseManager->DoPulse(););

    // Process our resource stop/restart queue
    CLOCK_CALL1(m_pResourceManager->ProcessQueue(););

    // Delete all items requested
    CLOCK_CALL1(m_ElementDeleter.DoDeleteAll(););

    CLOCK_CALL1(CPerfStatManager::GetSingleton()->DoPulse(););

    if (m_pMasterServerAnnouncer)
        m_pMasterServerAnnouncer->Pulse();

    if (m_pHqComms)
        m_pHqComms->Pulse();

    CLOCK_CALL1(m_pFunctionUseLogger->Pulse(););
    CLOCK_CALL1(m_lightsyncManager.DoPulse(););

    CLOCK_CALL1(m_pLatentTransferManager->DoPulse(););

    CLOCK_CALL1(m_pAsyncTaskScheduler->CollectResults());

    CLOCK_CALL1(m_pMapManager->GetWeather()->DoPulse(););

    PrintLogOutputFromNetModule();
    m_pScriptDebugging->UpdateLogOutput();

    // Unlock the critical section again
    Unlock();
}

bool CGame::Start(int iArgumentCount, char* szArguments[])
{
    // Init
    m_pASE = NULL;
    IsMainThread();

    // Startup the getElementsByType from root optimizations
    CElement::StartupEntitiesFromRoot();

    CSimControl::Startup();
    m_pGroups = new CGroups;
    m_pClock = new CClock;
    m_pBlipManager = new CBlipManager;
    m_pColManager = new CColManager;
    m_pObjectManager = new CObjectManager;
    m_pPickupManager = new CPickupManager(m_pColManager);
    m_pPlayerManager = new CPlayerManager;
    m_pRadarAreaManager = new CRadarAreaManager;
    m_pMarkerManager = new CMarkerManager(m_pColManager);
    m_pHandlingManager = new CHandlingManager;
    m_pVehicleManager = new CVehicleManager;
    m_pPacketTranslator = new CPacketTranslator(m_pPlayerManager);
    m_pBanManager = new CBanManager;
    m_pTeamManager = new CTeamManager;
    m_pPedManager = new CPedManager;
    m_pWaterManager = new CWaterManager;
    m_pScriptDebugging = new CScriptDebugging();
    m_pMapManager = new CMapManager(m_pBlipManager, m_pObjectManager, m_pPickupManager, m_pPlayerManager, m_pRadarAreaManager, m_pMarkerManager,
                                    m_pVehicleManager, m_pTeamManager, m_pPedManager, m_pColManager, m_pWaterManager, m_pClock, m_pGroups,
                                    &m_Events, m_pScriptDebugging, &m_ElementDeleter);
    m_pACLManager = new CAccessControlListManager;
    m_pHqComms = new CHqComms;

    m_pRegisteredCommands = new CRegisteredCommands(m_pACLManager);
    m_pLuaManager = new CLuaManager(m_pObjectManager, m_pPlayerManager, m_pVehicleManager, m_pBlipManager, m_pRadarAreaManager, m_pRegisteredCommands,
                                    m_pMapManager, &m_Events);
    m_pConsole = new CConsole(m_pBlipManager, m_pMapManager, m_pPlayerManager, m_pRegisteredCommands, m_pVehicleManager,
                              m_pBanManager, m_pACLManager);
    m_pMainConfig = new CMainConfig(m_pConsole);
    m_pRPCFunctions = new CRPCFunctions;

    m_pWeaponStatsManager = new CWeaponStatManager();

    m_pBuildingRemovalManager = new CBuildingRemovalManager;

    m_pCustomWeaponManager = new CCustomWeaponManager();

    m_pTrainTrackManager = std::make_shared<CTrainTrackManager>();

    // Parse the commandline
    if (!m_CommandLineParser.Parse(iArgumentCount, szArguments))
    {
        return false;
    }

    // Check pcre has been built correctly
    int iPcreConfigUtf8 = 0;
    pcre_config(PCRE_CONFIG_UTF8, &iPcreConfigUtf8);
    if (iPcreConfigUtf8 == 0)
    {
        CLogger::ErrorPrintf("PCRE built without UTF8 support\n");
        return false;
    }

    // Check json has precision mod - #8853 (toJSON passes wrong floats)
    json_object* pJsonObject = json_object_new_double(5.12345678901234);
    SString      strJsonResult = json_object_to_json_string_ext(pJsonObject, JSON_C_TO_STRING_PLAIN);
    json_object_put(pJsonObject);
    if (strJsonResult != "5.12345678901234")
    {
        CLogger::ErrorPrintf("JSON built without precision modification\n");
    }

    // Grab the path to the main config
    SString     strBuffer;
    const char* szMainConfig;
    if (m_CommandLineParser.GetMainConfig(szMainConfig))
    {
        strBuffer = g_pServerInterface->GetModManager()->GetAbsolutePath(szMainConfig);
    }
    else
    {
        strBuffer = g_pServerInterface->GetModManager()->GetAbsolutePath("mtaserver.conf");
        m_bUsingMtaServerConf = true;
    }
    m_pMainConfig->SetFileName(strBuffer);

    // Load the main config base
    if (!m_pMainConfig->Load())
        return false;

    // Let the main config handle selecting settings from the command line where appropriate
    m_pMainConfig->SetCommandLineParser(&m_CommandLineParser);

    // Do basic backup
    HandleBackup();

    // Encrypt crash dumps for uploading
    HandleCrashDumpEncryption();

// Check Windows server is using correctly compiled Lua dll
#ifndef MTA_DEBUG
    #ifdef WIN32
    HMODULE hModule = LoadLibrary("lua5.1.dll");
    // Release server should not have this function
    PVOID pFunc = static_cast<PVOID>(GetProcAddress(hModule, "luaX_is_apicheck_enabled"));
    FreeLibrary(hModule);
    if (pFunc)
    {
        CLogger::ErrorPrintf("Problem with Lua dll\n");
        return false;
    }
    #endif
#endif

    // Read some settings
    m_pACLManager->SetFileName(m_pMainConfig->GetAccessControlListFile().c_str());
    const SString  strServerIP = m_pMainConfig->GetServerIP();
    const SString  strServerIPList = m_pMainConfig->GetServerIPList();
    unsigned short usServerPort = m_pMainConfig->GetServerPort();
    unsigned int   uiMaxPlayers = m_pMainConfig->GetMaxPlayers();

    // Start async task scheduler
    m_pAsyncTaskScheduler = new SharedUtil::CAsyncTaskScheduler(2);

    // Create the account manager
    strBuffer = g_pServerInterface->GetModManager()->GetAbsolutePath("internal.db");
    m_pDatabaseManager = NewDatabaseManager();
    m_pDebugHookManager = new CDebugHookManager();
    m_pLuaCallbackManager = new CLuaCallbackManager();
    m_pRegistryManager = new CRegistryManager();
    m_pAccountManager = new CAccountManager(strBuffer);

    // Create and start the HTTP server
    m_pHTTPD = new CHTTPD;
    m_pLatentTransferManager = new CLatentTransferManager();

    // Enable it if required
    if (m_pMainConfig->IsHTTPEnabled())
    {
        // Slight hack for internal HTTPD: Listen on all IPs if multiple IPs declared
        SString strUseIP = (strServerIP == strServerIPList) ? strServerIP : "";
        if (!m_pHTTPD->StartHTTPD(strUseIP, m_pMainConfig->GetHTTPPort()))
        {
            CLogger::ErrorPrintf("Could not start HTTP server on interface '%s' and port '%u'!\n", strUseIP.c_str(), m_pMainConfig->GetHTTPPort());
            return false;
        }
    }

    m_pFunctionUseLogger = new CFunctionUseLogger(m_pMainConfig->GetLoadstringLogFilename());

    // Setup server id
    if (!g_pNetServer->InitServerId(m_pMainConfig->GetIdFile()))
    {
        CLogger::ErrorPrintf("Could not read or create server-id keys file at '%s'\n", *m_pMainConfig->GetIdFile());
        return false;
    }

    // Eventually set the logfiles
    bool bLogFile = CLogger::SetLogFile(m_pMainConfig->GetLogFile().c_str());
    CLogger::SetAuthFile(m_pMainConfig->GetAuthFile().c_str());

    // Trim the logfile name for the output
    char  szLogFileNameOutput[MAX_PATH];
    char* pszLogFileName = szLogFileNameOutput;
    strncpy(szLogFileNameOutput, m_pMainConfig->GetLogFile().c_str(), MAX_PATH);
    size_t sizeLogFileName = strlen(szLogFileNameOutput);
    if (sizeLogFileName > 45)
    {
        pszLogFileName += (sizeLogFileName - 45);
        pszLogFileName[0] = '.';
        pszLogFileName[1] = '.';
    }

    // Prepare our voice string
    SString strVoice = "Disabled";
    if (m_pMainConfig->IsVoiceEnabled())
        switch (m_pMainConfig->GetVoiceSampleRate())
        {
            case 0:
                strVoice = SString("Quality [%i];  Sample Rate: [8000Hz]", m_pMainConfig->GetVoiceQuality());
                break;
            case 1:
                strVoice = SString("Quality [%i];  Sample Rate: [16000Hz]", m_pMainConfig->GetVoiceQuality());
                break;
            case 2:
                strVoice = SString("Quality [%i];  Sample Rate: [32000Hz]", m_pMainConfig->GetVoiceQuality());
                break;
            default:
                break;
        }
    if (m_pMainConfig->GetVoiceBitrate())
        strVoice += SString(";  Bitrate: [%ibps]", m_pMainConfig->GetVoiceBitrate());

    // Make bandwidth reduction string
    SString strBandwidthSaving = m_pMainConfig->GetSetting("bandwidth_reduction");
    strBandwidthSaving = strBandwidthSaving.Left(1).ToUpper() + strBandwidthSaving.SubStr(1);
    if (g_pBandwidthSettings->bLightSyncEnabled)
        strBandwidthSaving += SString(" with lightweight sync rate of %dms", g_TickRateSettings.iLightSync);

    // Show the server header
    CLogger::LogPrintfNoStamp(
        "==================================================================\n"
        "= Multi Theft Auto: San Andreas v%s\n"
        "==================================================================\n"
        "= Server name      : %s\n"
        "= Server IP address: %s\n"
        "= Server port      : %u\n"
        "= \n"
        "= Log file         : %s\n"
        "= Maximum players  : %u\n"
        "= HTTP port        : %u\n"
        "= Voice Chat       : %s\n"
        "= Bandwidth saving : %s\n"
        "==================================================================\n",

        MTA_DM_BUILDTAG_SHORT
#ifdef ANY_x64
        " [64 bit]"
#endif
        ,
        m_pMainConfig->GetServerName().c_str(), strServerIPList.empty() ? "auto" : strServerIPList.c_str(), usServerPort, pszLogFileName, uiMaxPlayers,
        m_pMainConfig->IsHTTPEnabled() ? m_pMainConfig->GetHTTPPort() : 0, strVoice.c_str(), *strBandwidthSaving);

    if (!bLogFile)
        CLogger::ErrorPrintf("Unable to save logfile to '%s'\n", m_pMainConfig->GetLogFile().c_str());

    // Show startup messages from net module
    PrintLogOutputFromNetModule();

    // Show some warnings if applicable
    if (m_pMainConfig->IsFakeLagCommandEnabled())
    {
        CLogger::LogPrintf("WARNING: ase disabled due to fakelag command\n");
    }

    if (m_pMainConfig->GetAseInternetListenEnabled())
    {
        // Check if IP is one of the most common private IP addresses
        in_addr serverIp;
        serverIp.s_addr = inet_addr(strServerIP);
        uchar a = ((uchar*)&serverIp.s_addr)[0];
        uchar b = ((uchar*)&serverIp.s_addr)[1];
        if (a == 10 || a == 127 || (a == 169 && b == 254) || (a == 192 && b == 168))
        {
            CLogger::LogPrintf("WARNING: Private IP '%s' with ase enabled! Use: <serverip>auto</serverip>\n", *strServerIP);
        }
    }

    // Check accounts database and print message if there is a problem
    if (!m_pAccountManager->IntegrityCheck())
        return false;

    // Setup resource-cache directory
    {
        SString strResourceCachePath("%s/resource-cache", g_pServerInterface->GetServerModPath());
        SString strResourceCacheUnzippedPath("%s/unzipped", strResourceCachePath.c_str());
        SString strResourceCacheHttpClientFilesPath("%s/http-client-files", strResourceCachePath.c_str());
        SString strResourceCacheHttpClientFilesNoClientCachePath("%s/http-client-files-no-client-cache", strResourceCachePath.c_str());

        // Make sure the resource-cache directories exists
        MakeSureDirExists((strResourceCacheUnzippedPath + "/").c_str());
        MakeSureDirExists((strResourceCacheHttpClientFilesPath + "/").c_str());
        MakeSureDirExists((strResourceCacheHttpClientFilesNoClientCachePath + "/").c_str());

        // Rename old dirs to show that they are no longer used
        FileRename(PathJoin(g_pServerInterface->GetServerModPath(), "resourcecache"), strResourceCachePath + "/_old_resourcecache.delete-me");
        FileRename(strResourceCachePath + "/http-client-files-protected", strResourceCachePath + "/_old_http-client-files-protected.delete-me");

        // Create cache readme
        SString strReadmeFilename("%s/DO_NOT_MODIFY_Readme.txt", strResourceCachePath.c_str());
        FILE*   fh = File::Fopen(strReadmeFilename, "w");
        if (fh)
        {
            fprintf(fh, "---------------------------------------------------------------------------\n");
            fprintf(fh, "The content of this directory is automatically generated by the server.\n\n");
            fprintf(fh, "Do not modify or delete anything in here while the server is running.\n\n");
            fprintf(fh, "When the server is not running, you can do what you want, including clearing\n");
            fprintf(fh, "out all the cached files by deleting the resource-cache directory.\n");
            fprintf(fh, "(It will get recreated when the server is next started)\n");
            fprintf(fh, "---------------------------------------------------------------------------\n\n");
            fprintf(fh, "The 'http-client-files' directory always contains the correct client files\n");
            fprintf(fh, "for hosting on a web server.\n");
            fprintf(fh, "* If the web server is on the same machine, you can simply link the appropriate\n");
            fprintf(fh, "  web server directory to 'http-client-files'.\n");
            fprintf(fh, "* If the web server is on a separate machine, ensure it has access to\n");
            fprintf(fh, "  'http-client-files' via a network path, or maintain a remote copy using\n");
            fprintf(fh, "  synchronization software.\n");
            fprintf(fh, "---------------------------------------------------------------------------\n\n");
            fclose(fh);
        }
    }

    // Load the ACL's
    if (!m_pACLManager->Load())
        return false;

    m_pRemoteCalls = new CRemoteCalls();
    m_pRegistry = m_pRegistryManager->OpenRegistry("");

    m_pResourceManager = new CResourceManager;
    m_pSettings = new CSettings(m_pResourceManager);
    if (!m_pResourceManager->Refresh())
        return false;            // Load cancelled
    m_pUnoccupiedVehicleSync = new CUnoccupiedVehicleSync(m_pPlayerManager, m_pVehicleManager);
    m_pPedSync = new CPedSync(m_pPlayerManager, m_pPedManager);
#ifdef WITH_OBJECT_SYNC
    m_pObjectSync = new CObjectSync(m_pPlayerManager, m_pObjectManager);
#endif
    // Must be created before all clients
    m_pConsoleClient = new CConsoleClient(m_pConsole);

    m_pZoneNames = new CZoneNames;

    CStaticFunctionDefinitions(this);
    CLuaFunctionDefs::Initialize(m_pLuaManager, this);
    CLuaDefs::Initialize(this);

    m_pPlayerManager->SetScriptDebugging(m_pScriptDebugging);

// Set our console control handler
#ifdef WIN32
    SetConsoleCtrlHandler(ConsoleEventHandler, TRUE);
// Hide the close box
// DeleteMenu ( GetSystemMenu ( GetConsoleWindow(), FALSE ), SC_CLOSE, MF_BYCOMMAND );
#else
    signal(SIGTERM, &sighandler);
    signal(SIGINT, &sighandler);
    signal(SIGPIPE, SIG_IGN);
#endif

    // Add our builtin events
    AddBuiltInEvents();

    // Load the vehicle colors before the main config
    strBuffer = g_pServerInterface->GetModManager()->GetAbsolutePath("vehiclecolors.conf");
    if (!m_pVehicleManager->GetColorManager()->Load(strBuffer))
    {
        // Try to generate a new one and load it again
        if (m_pVehicleManager->GetColorManager()->Generate(strBuffer))
        {
            if (!m_pVehicleManager->GetColorManager()->Load(strBuffer))
            {
                CLogger::ErrorPrintf("%s", "Loading 'vehiclecolors.conf' failed\n ");
            }
        }
        else
        {
            CLogger::ErrorPrintf("%s", "Generating a new 'vehiclecolors.conf' failed\n ");
        }
    }

    // Load the registry
    strBuffer = g_pServerInterface->GetModManager()->GetAbsolutePath("registry.db");
    m_pRegistry->Load(strBuffer);
    // Check accounts database and print a message if there is a problem
    m_pRegistry->IntegrityCheck();

    // Load the accounts
    m_pAccountManager->Load();

    // Register our packethandler
    g_pNetServer->RegisterPacketHandler(CGame::StaticProcessPacket);

    // Try to start the network
    if (!g_pNetServer->StartNetwork(strServerIPList, usServerPort, uiMaxPlayers, m_pMainConfig->GetServerName().c_str()))
    {
        CLogger::ErrorPrintf("Could not bind the server on interface '%s' and port '%u'!\n", strServerIPList.c_str(), usServerPort);
        return false;
    }

    // Load the banlist
    m_pBanManager->LoadBanList();

    // If the server is passworded
    if (m_pMainConfig->HasPassword())
    {
        // Check it for validity
        const char* szPassword = m_pMainConfig->GetPassword().c_str();
        if (m_pMainConfig->IsValidPassword(szPassword))
        {
            // Store the server password
            CLogger::LogPrintf("Server password set to '%s'\n", szPassword);
        }
        else
        {
            CLogger::LogPrint("Invalid password in config, no password is used\n");
        }
    }

    // If ASE is enabled
    m_pASE = new ASE(m_pMainConfig, m_pPlayerManager, static_cast<int>(usServerPort), strServerIPList);
    if (m_pMainConfig->GetSerialVerificationEnabled())
        m_pASE->SetRuleValue("SerialVerification", "yes");
    ApplyAseSetting();
    m_pMasterServerAnnouncer = new CMasterServerAnnouncer();
    m_pMasterServerAnnouncer->Pulse();

    // Now load the rest of the config
    if (!m_pMainConfig->LoadExtended())
        return false;            // Fail or cancelled

    // Is the script debug log enabled?
    if (m_pMainConfig->GetScriptDebugLogEnabled())
    {
        if (!m_pScriptDebugging->SetLogfile(m_pMainConfig->GetScriptDebugLogFile().c_str(), m_pMainConfig->GetScriptDebugLogLevel()))
        {
            CLogger::LogPrint("WARNING: Unable to open the given script debug logfile\n");
        }
    }

    // Run startup commands
    strBuffer = g_pServerInterface->GetModManager()->GetAbsolutePath("autoexec.conf");
    CCommandFile Autoexec(strBuffer, *m_pConsole, *m_pConsoleClient);
    if (Autoexec.IsValid())
    {
        CLogger::LogPrintf("autoexec.conf file found! Executing...\n");
        Autoexec.Run();
    }

    // Flush any pending master server announce messages
    g_pNetServer->GetHTTPDownloadManager(EDownloadMode::ASE)->ProcessQueuedFiles();

    // Warnings only if not editor or local server
    if (IsUsingMtaServerConf())
    {
        // Authorized serial account protection
        if (m_pMainConfig->GetAuthSerialEnabled())
        {
            CLogger::LogPrintf(
                "Authorized serial account protection is enabled for the ACL group(s): `%s`  See http:"
                "//mtasa.com/authserial\n",
                *SString::Join(",", m_pMainConfig->GetAuthSerialGroupList()));
        }
        else
        {
            CLogger::LogPrint(
                "Authorized serial account protection is DISABLED. See http:"
                "//mtasa.com/authserial\n");
        }

        // Owner email address
        if (m_pMainConfig->GetOwnerEmailAddressList().empty())
        {
            CLogger::LogPrintf("WARNING: <owner_email_address> not set\n");
        }
    }

    // Done
    // If you're ever going to change this message, update the "server ready" determination
    // inside CServer.cpp in deathmatch mod aswell.
    CLogger::LogPrint("Server started and is ready to accept connections!\n");

    // Create port tester
    m_pOpenPortsTester = new COpenPortsTester();

    // Add help hint
    CLogger::LogPrint("To stop the server, type 'shutdown' or press Ctrl-C\n");
    CLogger::LogPrint("Type 'help' for a list of commands.\n");

    m_bServerFullyUp = true;
    return true;
}

void CGame::Stop()
{
    m_bServerFullyUp = false;

    // Tell the log
    CLogger::LogPrint("Server stopped!\n");

    // Stop our network
    g_pNetServer->StopNetwork();

    // Unregister our packethandler
    g_pNetServer->RegisterPacketHandler(NULL);
}

// Handle logging output from the net module
void CGame::PrintLogOutputFromNetModule()
{
    std::vector<SString> lineList;
    SStringX(g_pRealNetServer->GetLogOutput()).Split("\n", lineList);
    for (std::vector<SString>::iterator iter = lineList.begin(); iter != lineList.end(); ++iter)
        if (!iter->empty())
            CLogger::LogPrint(*iter + "\n");
}

void CGame::StartOpenPortsTest()
{
    if (m_pOpenPortsTester)
        m_pOpenPortsTester->Start();
}

bool CGame::StaticProcessPacket(unsigned char ucPacketID, const NetServerPlayerID& Socket, NetBitStreamInterface* pBitStream, SNetExtraInfo* pNetExtraInfo)
{
    // Is it a join packet? Pass it to the handler immediately
    if (ucPacketID == PACKET_ID_PLAYER_JOIN)
    {
        g_pGame->Packet_PlayerJoin(Socket);
        return true;
    }

    // Is it an rpc call?
    if (ucPacketID == PACKET_ID_RPC)
    {
        g_pGame->m_pRPCFunctions->ProcessPacket(Socket, *pBitStream);
        return true;
    }

    // Is it this?
    if (ucPacketID == PACKET_ID_LATENT_TRANSFER)
    {
        g_pGame->GetLatentTransferManager()->OnReceive(Socket, pBitStream);
        return true;
    }

    // Translate the packet
    CPacket* pPacket = g_pGame->m_pPacketTranslator->Translate(Socket, static_cast<ePacketID>(ucPacketID), *pBitStream, pNetExtraInfo);
    if (pPacket)
    {
        // Handle it
        bool bHandled = g_pGame->ProcessPacket(*pPacket);

        // Destroy the packet and return whether it could handle it or not
        delete pPacket;
        return bHandled;
    }

    // Failed to handle it
    return false;
}

bool CGame::ProcessPacket(CPacket& Packet)
{
    // Can we handle it?
    ePacketID PacketID = Packet.GetPacketID();
    switch ((int)PacketID)
    {
        case PACKET_ID_PLAYER_JOINDATA:
        {
            Packet_PlayerJoinData(static_cast<CPlayerJoinDataPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PED_WASTED:
        {
            Packet_PedWasted(static_cast<CPedWastedPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_WASTED:
        {
            Packet_PlayerWasted(static_cast<CPlayerWastedPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_QUIT:
        {
            Packet_PlayerQuit(static_cast<CPlayerQuitPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_TIMEOUT:
        {
            Packet_PlayerTimeout(static_cast<CPlayerTimeoutPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_PURESYNC:
        {
            Packet_PlayerPuresync(static_cast<CPlayerPuresyncPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_VEHICLE_PURESYNC:
        {
            Packet_VehiclePuresync(static_cast<CVehiclePuresyncPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_KEYSYNC:
        {
            Packet_Keysync(static_cast<CKeysyncPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_BULLETSYNC:
        {
            Packet_Bulletsync(static_cast<CBulletsyncPacket&>(Packet));
            return true;
        }

        case PACKET_ID_WEAPON_BULLETSYNC:
        {
            Packet_WeaponBulletsync(static_cast<CCustomWeaponBulletSyncPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PED_TASK:
        {
            Packet_PedTask(static_cast<CPedTaskPacket&>(Packet));
            return true;
        }

        case PACKET_ID_DETONATE_SATCHELS:
        {
            Packet_DetonateSatchels(static_cast<CDetonateSatchelsPacket&>(Packet));
            return true;
        }

        case PACKET_ID_DESTROY_SATCHELS:
        {
            Packet_DestroySatchels(static_cast<CDestroySatchelsPacket&>(Packet));
            return true;
        }

        case PACKET_ID_EXPLOSION:
            Packet_ExplosionSync(static_cast<CExplosionSyncPacket&>(Packet));
            return true;

        case PACKET_ID_PROJECTILE:
            Packet_ProjectileSync(static_cast<CProjectileSyncPacket&>(Packet));
            return true;

        case PACKET_ID_COMMAND:
        {
            Packet_Command(static_cast<CCommandPacket&>(Packet));
            return true;
        }

        case PACKET_ID_VEHICLE_DAMAGE_SYNC:
        {
            Packet_VehicleDamageSync(static_cast<CVehicleDamageSyncPacket&>(Packet));
            return true;
        }

        case PACKET_ID_VEHICLE_INOUT:
        {
            Packet_Vehicle_InOut(static_cast<CVehicleInOutPacket&>(Packet));
            return true;
        }

        case PACKET_ID_LUA_EVENT:
        {
            Packet_LuaEvent(static_cast<CLuaEventPacket&>(Packet));
            return true;
        }

        case PACKET_ID_CUSTOM_DATA:
        {
            Packet_CustomData(static_cast<CCustomDataPacket&>(Packet));
            return true;
        }

        case PACKET_ID_VOICE_DATA:
        {
            Packet_Voice_Data(static_cast<CVoiceDataPacket&>(Packet));
            return true;
        }

        case PACKET_ID_VOICE_END:
        {
            Packet_Voice_End(static_cast<CVoiceEndPacket&>(Packet));
            return true;
        }

        case PACKET_ID_CAMERA_SYNC:
        {
            Packet_CameraSync(static_cast<CCameraSyncPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_TRANSGRESSION:
        {
            Packet_PlayerTransgression(static_cast<CPlayerTransgressionPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_DIAGNOSTIC:
        {
            Packet_PlayerDiagnostic(static_cast<CPlayerDiagnosticPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_MODINFO:
        {
            Packet_PlayerModInfo(static_cast<CPlayerModInfoPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_ACINFO:
        {
            Packet_PlayerACInfo(static_cast<CPlayerACInfoPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_SCREENSHOT:
        {
            Packet_PlayerScreenShot(static_cast<CPlayerScreenShotPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_NO_SOCKET:
        {
            Packet_PlayerNoSocket(static_cast<CPlayerNoSocketPacket&>(Packet));
            return true;
        }

        case PACKET_ID_PLAYER_NETWORK_STATUS:
        {
            Packet_PlayerNetworkStatus(static_cast<CPlayerNetworkStatusPacket&>(Packet));
            return true;
        }

        case PACKET_ID_DISCORD_JOIN:
        {
            Packet_DiscordJoin(static_cast<CDiscordJoinPacket&>(Packet));
            return true;
        }

        default:
            break;
    }

    // Let the unoccupied vehicle sync manager handle it
    if (m_pUnoccupiedVehicleSync->ProcessPacket(Packet))
    {
        return true;
    }
    else if (m_pPedSync->ProcessPacket(Packet))
    {
        return true;
    }
#ifdef WITH_OBJECT_SYNC
    else if (m_pObjectSync->ProcessPacket(Packet))
    {
        return true;
    }
#endif
    return false;
}

void CGame::JoinPlayer(CPlayer& Player)
{
    CTimeUsMarker<20> marker;
    marker.Set("Start");

    // Let him join
    Player.Send(CPlayerJoinCompletePacket(
        Player.GetID(), m_pPlayerManager->Count(), m_pMapManager->GetRootElement()->GetID(), m_pMainConfig->GetHTTPDownloadType(), m_pMainConfig->GetHTTPPort(),
        m_pMainConfig->GetHTTPDownloadURL().c_str(), m_pMainConfig->GetHTTPMaxConnectionsPerClient(), m_pMainConfig->GetEnableClientChecks(),
        m_pMainConfig->IsVoiceEnabled(), m_pMainConfig->GetVoiceSampleRate(), m_pMainConfig->GetVoiceQuality(), m_pMainConfig->GetVoiceBitrate()));

    marker.Set("CPlayerJoinCompletePacket");

    // Sync up server info on entry
    if (Player.CanBitStream(eBitStreamVersion::Discord_InitialImplementation))
        Player.Send(CServerInfoSyncPacket(SERVER_INFO_FLAG_ALL));

    // Add debug info if wanted
    if (CPerfStatDebugInfo::GetSingleton()->IsActive("PlayerInGameNotice"))
        CPerfStatDebugInfo::GetSingleton()->AddLine("PlayerInGameNotice", marker.GetString());
}

void CGame::InitialDataStream(CPlayer& Player)
{
    CTimeUsMarker<20> marker;
    marker.Set("Start");

    // Set the root element as his parent
    // NOTE: Make sure he doesn't get any entities sent to him because we're sending him soon
    Player.SetDoNotSendEntities(true);
    Player.SetParentObject(m_pMapManager->GetRootElement());
    Player.SetDoNotSendEntities(false);

    marker.Set("SetParentObject");

    // He's joined now
    Player.SetJoined();
    m_pPlayerManager->OnPlayerJoin(&Player);

    // Console
    CLogger::LogPrintf("JOIN: %s joined the game (IP: %s)\n", Player.GetNick(), Player.GetSourceIP());

    // Tell him current sync rates
    CStaticFunctionDefinitions::SendSyncIntervals(&Player);

    // Tell him current bullet sync enabled weapons and vehicle extrapolation settings
    SendSyncSettings(&Player);

    // Tell the other players about him
    CPlayerListPacket PlayerNotice;
    PlayerNotice.AddPlayer(&Player);
    PlayerNotice.SetShowInChat(true);
    m_pPlayerManager->BroadcastOnlyJoined(PlayerNotice, &Player);

    marker.Set("PlayerNotice");

    // Tell the map manager
    m_pMapManager->OnPlayerJoin(Player);            // This sends the elements that are needed before the resources start

    marker.Set("SendMapElements");

    // Write all players connected right now to a playerlist packet except the one we got the ingame notice from
    CPlayerListPacket PlayerList;
    // Entity add packet might as well be generated
    CEntityAddPacket EntityPacket;
    PlayerList.SetShowInChat(false);
    list<CPlayer*>::const_iterator iter = m_pPlayerManager->IterBegin();
    for (; iter != m_pPlayerManager->IterEnd(); iter++)
    {
        CPlayer* pPlayer = *iter;
        if (&Player != *iter && (*iter)->IsJoined() && !(*iter)->IsBeingDeleted())
        {
            PlayerList.AddPlayer(*iter);
        }
        if (pPlayer != &Player)
        {
            EntityPacket.Add(pPlayer);
        }
    }

    // Send it to the player we got this ingame notice from
    Player.Send(PlayerList);

    marker.Set("PlayerList");

    Player.Send(EntityPacket);

    marker.Set("SendPlayerElements");

    // Tell him about the blips (needs to be done after all the entities it can be attached to)
    m_pMapManager->SendBlips(Player);

    marker.Set("SendBlips");

    // Send him the current info of the current players ( stats, clothes, etc )
    iter = m_pPlayerManager->IterBegin();
    for (; iter != m_pPlayerManager->IterEnd(); iter++)
    {
        if (&Player != *iter && (*iter)->IsJoined())
        {
            CPlayerStatsPacket PlayerStats = *(*iter)->GetPlayerStatsPacket();
            PlayerStats.SetSourceElement(*iter);
            if (PlayerStats.GetSize() > 0)
                Player.Send(PlayerStats);

            CPlayerClothesPacket PlayerClothes;
            PlayerClothes.SetSourceElement(*iter);
            PlayerClothes.Add((*iter)->GetClothes());
            if (PlayerClothes.Count() > 0)
                Player.Send(PlayerClothes);
        }
    }

    marker.Set("PlayerStats");

    // Send the root element custom data
    m_pMapManager->GetRootElement()->SendAllCustomData(&Player);

    // Tell the resource manager
    m_pResourceManager->OnPlayerJoin(Player);

    marker.Set("ResourceStartPacket");

    // Tell our scripts the player has joined
    CLuaArguments Arguments;
    Player.CallEvent("onPlayerJoin", Arguments);

    marker.Set("onPlayerJoin");

    SString joinSecret = Player.GetDiscordJoinSecret();
    if (joinSecret.length())
    {
        CLuaArguments Arguments;
        Arguments.PushBoolean(true);
        Arguments.PushString(joinSecret);
        Player.CallEvent("onPlayerDiscordJoin", Arguments);
    }

    // Register them on the lightweight sync manager.
    m_lightsyncManager.RegisterPlayer(&Player);

    // Add debug info if wanted
    if (CPerfStatDebugInfo::GetSingleton()->IsActive("InitialDataStream"))
        CPerfStatDebugInfo::GetSingleton()->AddLine("InitialDataStream", marker.GetString());
}

void CGame::QuitPlayer(CPlayer& Player, CClient::eQuitReasons Reason, bool bSayInConsole, const char* szKickReason, const char* szResponsiblePlayer)
{
    if (Player.IsLeavingServer())
        return;

    Player.SetLeavingServer(true);

    // Grab quit reaason
    const char* szReason = "Unknown";
    switch (Reason)
    {
        case CClient::QUIT_QUIT:
            szReason = "Quit";
            break;
        case CClient::QUIT_KICK:
            szReason = "Kicked";
            break;
        case CClient::QUIT_BAN:
            szReason = "Banned";
            break;
        case CClient::QUIT_CONNECTION_DESYNC:
            szReason = "Bad Connection";
            break;
        case CClient::QUIT_TIMEOUT:
            szReason = "Timed out";
            break;
    }

    // Output
    const char* szNick = Player.GetNick();
    if (bSayInConsole && szNick && szNick[0] && !m_bBeingDeleted)
    {
        CLogger::LogPrintf("QUIT: %s left the game [%s]%s\n", szNick, szReason, *Player.GetQuitReasonForLog());
    }

    // If he had joined
    if (Player.IsJoined())
    {
        // Tell our scripts the player has quit, but only if the scripts got told he joined
        CLuaArguments Arguments;
        Arguments.PushString(szReason);
        if ((Reason == CClient::QUIT_BAN || Reason == CClient::QUIT_KICK))
        {
            if (szKickReason && szKickReason[0])
                Arguments.PushString(szKickReason);
            else
                Arguments.PushBoolean(false);
            // Arguments.PushUserData ( strResponsiblePlayer != "Console" ? m_pPlayerManager->Get ( strResponsiblePlayer, true ) : static_cast < CConsoleClient*
            // > (m_pConsoleClient) );
            if (strcmp(szResponsiblePlayer, "Console") == 0)
                Arguments.PushElement(m_pConsoleClient);
            else
                Arguments.PushElement(m_pPlayerManager->Get(szResponsiblePlayer, true));
        }
        else
        {
            Arguments.PushBoolean(false);
            Arguments.PushBoolean(false);
        }

        Player.CallEvent("onPlayerQuit", Arguments);

        // Tell the map manager
        m_pMapManager->OnPlayerQuit(Player);

        if (!m_bBeingDeleted)
        {
            // Tell all the players except the parting one (we don't tell them if he hadn't joined because the players don't know about him)
            CPlayerQuitPacket Packet;
            Packet.SetPlayer(Player.GetID());
            Packet.SetQuitReason(static_cast<unsigned char>(Reason));
            m_pPlayerManager->BroadcastOnlyJoined(Packet, &Player);
        }
    }

    // Tell net module connection version info will no longer be required
    g_pNetServer->ClearClientBitStreamVersion(Player.GetSocket());

    // Remove from any latent transfers
    GetLatentTransferManager()->RemoveRemote(Player.GetSocket());

    // Delete it, don't unlink yet, we could be inside the player-manager's iteration
    m_ElementDeleter.Delete(&Player, false);

    // Unregister them from the lightweight sync manager
    m_lightsyncManager.UnregisterPlayer(&Player);
}

void CGame::AddBuiltInEvents()
{
    // Resource events
    m_Events.AddEvent("onResourcePreStart", "resource", NULL, false);
    m_Events.AddEvent("onResourceStart", "resource", NULL, false);
    m_Events.AddEvent("onResourceStop", "resource, deleted", NULL, false);

    // Blip events

    // Marker events
    m_Events.AddEvent("onMarkerHit", "player, matchingDimension", NULL, false);
    m_Events.AddEvent("onMarkerLeave", "player, matchingDimension", NULL, false);

    // Voice events
    m_Events.AddEvent("onPlayerVoiceStart", "", NULL, false);
    m_Events.AddEvent("onPlayerVoiceStop", "", NULL, false);

    // Object events

    // Pickup events
    m_Events.AddEvent("onPickupHit", "player", NULL, false);
    m_Events.AddEvent("onPickupLeave", "player", NULL, false);
    m_Events.AddEvent("onPickupUse", "player", NULL, false);
    m_Events.AddEvent("onPickupSpawn", "", NULL, false);

    // Player events
    m_Events.AddEvent("onPlayerConnect", "player", NULL, false);
    m_Events.AddEvent("onPlayerChat", "text", NULL, false);
    m_Events.AddEvent("onPlayerDamage", "attacker, weapon, bodypart, loss", NULL, false);
    m_Events.AddEvent("onPlayerVehicleEnter", "vehicle, seat, jacked", NULL, false);
    m_Events.AddEvent("onPlayerVehicleExit", "vehicle, reason, jacker", NULL, false);
    m_Events.AddEvent("onPlayerJoin", "", NULL, false);
    m_Events.AddEvent("onPlayerQuit", "reason", NULL, false);
    m_Events.AddEvent("onPlayerSpawn", "spawnpoint, team", NULL, false);
    m_Events.AddEvent("onPlayerTarget", "target", NULL, false);
    m_Events.AddEvent("onPlayerWasted", "ammo, killer, weapon, bodypart", NULL, false);
    m_Events.AddEvent("onPlayerWeaponSwitch", "previous, current", NULL, false);
    m_Events.AddEvent("onPlayerMarkerHit", "marker, matchingDimension", NULL, false);
    m_Events.AddEvent("onPlayerMarkerLeave", "marker, matchingDimension", NULL, false);
    m_Events.AddEvent("onPlayerPickupHit", "pickup", NULL, false);
    m_Events.AddEvent("onPlayerPickupLeave", "pickup", NULL, false);
    m_Events.AddEvent("onPlayerPickupUse", "pickup", NULL, false);
    m_Events.AddEvent("onPlayerClick", "button, state, element, posX, posY, posZ", NULL, false);
    m_Events.AddEvent("onPlayerContact", "previous, current", NULL, false);
    m_Events.AddEvent("onPlayerBan", "ban", NULL, false);
    m_Events.AddEvent("onPlayerLogin", "guest_account, account, auto-login", NULL, false);
    m_Events.AddEvent("onPlayerLogout", "account, guest_account", NULL, false);
    m_Events.AddEvent("onPlayerChangeNick", "oldnick, newnick, manuallyChanged", NULL, false);
    m_Events.AddEvent("onPlayerPrivateMessage", "text, player", NULL, false);
    m_Events.AddEvent("onPlayerStealthKill", "target", NULL, false);
    m_Events.AddEvent("onPlayerMute", "", NULL, false);
    m_Events.AddEvent("onPlayerUnmute", "", NULL, false);
    m_Events.AddEvent("onPlayerCommand", "command", NULL, false);
    m_Events.AddEvent("onPlayerModInfo", "filename, itemlist", NULL, false);
    m_Events.AddEvent("onPlayerACInfo", "aclist, size, md5, sha256", NULL, false);
    m_Events.AddEvent("onPlayerNetworkStatus", "type, ticks", NULL, false);
    m_Events.AddEvent("onPlayerScreenShot", "resource, status, file_data, timestamp, tag", NULL, false);
    m_Events.AddEvent("onPlayerDiscordJoin", "justConnected, secret", NULL, false);

    // Ped events
    m_Events.AddEvent("onPedWasted", "ammo, killer, weapon, bodypart", NULL, false);
    m_Events.AddEvent("onPedWeaponSwitch", "previous, current", NULL, false);

    // Element events
    m_Events.AddEvent("onElementColShapeHit", "colshape, matchingDimension", NULL, false);
    m_Events.AddEvent("onElementColShapeLeave", "colshape, matchingDimension", NULL, false);
    m_Events.AddEvent("onElementClicked", "button, state, clicker, posX, posY, posZ", NULL, false);
    m_Events.AddEvent("onElementDataChange", "key, oldValue", NULL, false);
    m_Events.AddEvent("onElementDestroy", "", NULL, false);
    m_Events.AddEvent("onElementStartSync", "newSyncer", NULL, false);
    m_Events.AddEvent("onElementStopSync", "oldSyncer", NULL, false);
    m_Events.AddEvent("onElementModelChange", "oldModel, newModel", NULL, false);
    m_Events.AddEvent("onElementDimensionChange", "oldDimension, newDimension", nullptr, false);

    // Radar area events

    // Shape events
    m_Events.AddEvent("onColShapeHit", "entity, matchingDimension", NULL, false);
    m_Events.AddEvent("onColShapeLeave", "entity, matchingDimension", NULL, false);

    // Vehicle events
    m_Events.AddEvent("onVehicleDamage", "loss", NULL, false);
    m_Events.AddEvent("onVehicleRespawn", "exploded", NULL, false);
    m_Events.AddEvent("onTrailerAttach", "towedBy", NULL, false);
    m_Events.AddEvent("onTrailerDetach", "towedBy", NULL, false);
    m_Events.AddEvent("onVehicleStartEnter", "player, seat, jacked", NULL, false);
    m_Events.AddEvent("onVehicleStartExit", "player, seat, jacker", NULL, false);
    m_Events.AddEvent("onVehicleEnter", "player, seat, jacked", NULL, false);
    m_Events.AddEvent("onVehicleExit", "player, seat, jacker", NULL, false);
    m_Events.AddEvent("onVehicleExplode", "", NULL, false);

    // Console events
    m_Events.AddEvent("onConsole", "text", NULL, false);

    // Debug events
    m_Events.AddEvent("onDebugMessage", "message, level, file, line", NULL, false);

    // Ban events
    m_Events.AddEvent("onBan", "ip", NULL, false);
    m_Events.AddEvent("onUnban", "ip", NULL, false);

    // Account events
    m_Events.AddEvent("onAccountDataChange", "account, key, value", NULL, false);

    // Other events
    m_Events.AddEvent("onSettingChange", "setting, oldValue, newValue", NULL, false);
    m_Events.AddEvent("onChatMessage", "message, element", NULL, false);

    // Weapon events
    m_Events.AddEvent("onWeaponFire", "", NULL, false);
    m_Events.AddEvent("onPlayerWeaponFire", "weapon, endX, endY, endZ, hitElement, startX, startY, startZ", NULL, false);
}

void CGame::ProcessTrafficLights(long long llCurrentTime)
{
    long long     ulDiff = static_cast<long long>((llCurrentTime - m_llLastTrafficUpdate) * m_fGameSpeed);
    unsigned char ucNewState = 0xFF;

    if (ulDiff >= 1000)
    {
        if ((m_ucTrafficLightState == 0 || m_ucTrafficLightState == 3) && ulDiff >= 8000)            // green
        {
            ucNewState = m_ucTrafficLightState + 1;
        }
        else if ((m_ucTrafficLightState == 1 || m_ucTrafficLightState == 4) && ulDiff >= 3000)            // orange
        {
            ucNewState = (m_ucTrafficLightState == 4) ? 0 : 2;
        }
        else if (m_ucTrafficLightState == 2 && ulDiff >= 2000)            // red
        {
            ucNewState = 3;
        }

        if (ucNewState != 0xFF)
        {
            CStaticFunctionDefinitions::SetTrafficLightState(ucNewState);
            m_llLastTrafficUpdate = SharedUtil::GetModuleTickCount64();
        }
    }
}

// Relay this (pure sync) packet to all the other players using distance rules
void CGame::RelayPlayerPuresync(CPacket& Packet)
{
    // No need to update tick counter every call
    static uint uiUpdateCounter = 0;
    if ((++uiUpdateCounter & 7) == 0)
        UpdateModuleTickCount64();

    // Make a list of players to send this packet to
    CSendList sendList;
    bool      bUseSimSendList = CSimControl::IsSimSystemEnabled();

    CPlayer* pPlayer = Packet.GetSourcePlayer();

    //
    // Process far sync (only if light sync is not active)
    //
    if (pPlayer->IsTimeForPuresyncFar())
    {
        long long llTickCountNow = GetModuleTickCount64();

        // Use this players far list
        SViewerMapType& farList = pPlayer->GetFarPlayerList();

        // For each far player
        for (SViewerMapType ::iterator it = farList.begin(); it != farList.end(); ++it)
        {
            CPlayer*     pSendPlayer = it->first;
            SViewerInfo& nearInfo = it->second;

            nearInfo.llLastUpdateTime = llTickCountNow;
            sendList.push_back(pSendPlayer);
        }
    }

    //
    // Process near sync
    //
    {
        // Insert into other players puresync near list if appropriate
        pPlayer->MaybeUpdateOthersNearList();

        CLOCK("RelayPlayerPuresync", "ProcessNearList");
        // Use this players puresync near list for sending packets
        SViewerMapType& nearList = pPlayer->GetNearPlayerList();

        // Array for holding players that need moving to the puresync far list
        static std::vector<CPlayer*> moveToFarListList;            // static to help reduce memory allocations
        moveToFarListList.clear();

        // For each puresync near player
        for (SViewerMapType ::iterator it = nearList.begin(); it != nearList.end(); ++it)
        {
            CPlayer*     pSendPlayer = it->first;
            SViewerInfo& nearInfo = it->second;
            dassert(MapContains(pPlayer->m_PureSyncSimSendList, pSendPlayer) == nearInfo.bInPureSyncSimSendList);

            if (--nearInfo.iMoveToFarCountDown < 1)
            {
                // Remove player from puresync near list (Has to be not near for 5 calls to get removed (The delay ensures timely updates of players moving far
                // away))
                if (!pPlayer->ShouldPlayerBeInNearList(pSendPlayer))            // Double check remove is required.
                {
                    moveToFarListList.push_back(pSendPlayer);
                    continue;
                }
                nearInfo.iMoveToFarCountDown = 5;
            }

            {
                bool bTimeForSync = pSendPlayer->IsTimeToReceivePuresyncNearFrom(pPlayer, nearInfo);
                if (!bUseSimSendList)
                {
                    // Standard sending
                    if (bTimeForSync)
                        sendList.push_back(pSendPlayer);
                }
                else
                {
                    //
                    // Sim sync relays pure sync packets to the other player when he is in zone 0 (as seen from this player)
                    // Enabling/disabling sim puresync will only take effect for the next pure sync packet, so:
                    //
                    if (nearInfo.iZone > 0)
                    {
                        ////////////////////////////////////////////////
                        // Don't use sim sync for this player
                        ////////////////////////////////////////////////
                        if (!nearInfo.bInPureSyncSimSendList)
                        {
                            // If not in sim list, do send here
                            if (bTimeForSync)
                                sendList.push_back(pSendPlayer);
                        }
                        else if (nearInfo.bInPureSyncSimSendList)
                        {
                            // If in sim list, sim send has already been done.
                            // So don't send here as well - Just remove from sim list
                            MapRemove(pPlayer->m_PureSyncSimSendList, pSendPlayer);
                            pPlayer->m_bPureSyncSimSendListDirty = true;
                            nearInfo.bInPureSyncSimSendList = false;
                        }
                    }
                    else
                    {
                        ////////////////////////////////////////////////
                        // Use sim sync for this player
                        ////////////////////////////////////////////////
                        if (!nearInfo.bInPureSyncSimSendList)
                        {
                            // If not in sim list yet, do send here
                            if (bTimeForSync)
                                sendList.push_back(pSendPlayer);

                            // and add it to sim list for next time
                            MapInsert(pPlayer->m_PureSyncSimSendList, pSendPlayer);
                            pPlayer->m_bPureSyncSimSendListDirty = true;
                            nearInfo.bInPureSyncSimSendList = true;
                        }
                    }
                }
            }
        }
        UNCLOCK("RelayPlayerPuresync", "ProcessNearList");

        // Do pending near->far list moves
        for (std::vector<CPlayer*>::const_iterator iter = moveToFarListList.begin(); iter != moveToFarListList.end(); ++iter)
        {
            pPlayer->MovePlayerToFarList(*iter);
        }
    }

    // Relay packet
    if (!sendList.empty())
    {
        CLOCK("RelayPlayerPuresync", "Broadcast");
        CPlayerManager::Broadcast(Packet, sendList);
        UNCLOCK("RelayPlayerPuresync", "Broadcast");
    }

    CLOCK("RelayPlayerPuresync", "UpdatePuresyncSimPlayer");
    // Update sim data
    CSimControl::UpdateSimPlayer(pPlayer);
    UNCLOCK("RelayPlayerPuresync", "UpdatePuresyncSimPlayer");
}

// Relay this packet to other nearby players
void CGame::RelayNearbyPacket(CPacket& Packet)
{
    // Make a list of players to send this packet to
    CSendList sendList;
    bool      bUseSimSendList = CSimControl::IsSimSystemEnabled() && Packet.HasSimHandler();

    CPlayer* pPlayer = Packet.GetSourcePlayer();

    //
    // Process near sync
    //
    {
        // Update list of players who need the packet
        pPlayer->MaybeUpdateOthersNearList();

        // Use this player's near list for sending packets
        SViewerMapType& nearList = pPlayer->GetNearPlayerList();

        // For each near player
        for (SViewerMapType ::iterator it = nearList.begin(); it != nearList.end(); ++it)
        {
            CPlayer* pSendPlayer = it->first;
            if (!bUseSimSendList)
            {
                // Standard sending
                sendList.push_back(pSendPlayer);
            }
            else
            {
                const SViewerInfo& nearInfo = it->second;
                dassert(MapContains(pPlayer->m_PureSyncSimSendList, pSendPlayer) == nearInfo.bInPureSyncSimSendList);

                if (!nearInfo.bInPureSyncSimSendList)
                {
                    // If not in sim send list, do send here
                    sendList.push_back(pSendPlayer);
                }
            }
        }
    }

    // Relay packet
    if (!sendList.empty())
        CPlayerManager::Broadcast(Packet, sendList);
}

void CGame::PlayerCompleteConnect(CPlayer* pPlayer)
{
    SString strIPAndSerial("IP: %s  Serial: %s  Version: %s", pPlayer->GetSourceIP(), pPlayer->GetSerial().c_str(), pPlayer->GetPlayerVersion().c_str());
    // Call the onPlayerConnect event. If it returns false, disconnect the player
    CLuaArguments Arguments;
    Arguments.PushString(pPlayer->GetNick());
    Arguments.PushString(pPlayer->GetSourceIP());
    Arguments.PushString(pPlayer->GetSerialUser().c_str());
    Arguments.PushString(pPlayer->GetSerial().c_str());
    Arguments.PushNumber(pPlayer->GetMTAVersion());
    Arguments.PushString(pPlayer->GetPlayerVersion());
    if (!g_pGame->GetMapManager()->GetRootElement()->CallEvent("onPlayerConnect", Arguments))
    {
        // event cancelled, disconnect the player
        CLogger::LogPrintf("CONNECT: %s failed to connect. (onPlayerConnect event cancelled) (%s)\n", pPlayer->GetNick(), strIPAndSerial.c_str());
        const char* szError = g_pGame->GetEvents()->GetLastError();
        if (szError && szError[0])
        {
            DisconnectPlayer(g_pGame, *pPlayer, szError);
            return;
        }
        DisconnectPlayer(g_pGame, *pPlayer, CPlayerDisconnectedPacket::GENERAL_REFUSED);
        return;
    }

    // Tell the console
    CLogger::LogPrintf("CONNECT: %s connected (%s)\n", pPlayer->GetNick(), strIPAndSerial.c_str());

    // Send him the join details
    pPlayer->Send(CPlayerConnectCompletePacket());

    // The player is spawned when he's connected, just the Camera is not faded in/not targetting
    pPlayer->SetSpawned(true);
}

void CGame::Lock()
{
    pthread_mutex_lock(&mutexhttp);
}

void CGame::Unlock()
{
    pthread_mutex_unlock(&mutexhttp);
}

void CGame::SetGlitchEnabled(const std::string& strGlitch, bool bEnabled)
{
    eGlitchType cGlitch = m_GlitchNames[strGlitch];
    assert(cGlitch >= 0 && cGlitch < NUM_GLITCHES);
    m_Glitches[cGlitch] = bEnabled;
    SendSyncSettings();
    CalculateMinClientRequirement();
}

bool CGame::IsGlitchEnabled(const std::string& strGlitch)
{
    eGlitchType cGlitch = m_GlitchNames[strGlitch];
    assert(cGlitch >= 0 && cGlitch < NUM_GLITCHES);
    return m_Glitches[cGlitch] ? true : false;
}
bool CGame::IsGlitchEnabled(eGlitchType cGlitch)
{
    assert(cGlitch >= 0 && cGlitch < NUM_GLITCHES);
    return m_Glitches[cGlitch] || false;
}

void CGame::SetCloudsEnabled(bool bEnabled)
{
    m_bCloudsEnabled = bEnabled;
}
bool CGame::GetCloudsEnabled()
{
    return m_bCloudsEnabled;
}

bool CGame::GetJetpackWeaponEnabled(eWeaponType weaponType)
{
    if (weaponType >= WEAPONTYPE_BRASSKNUCKLE && weaponType < WEAPONTYPE_LAST_WEAPONTYPE)
    {
        return m_JetpackWeapons[weaponType];
    }
    return false;
}

void CGame::SetJetpackWeaponEnabled(eWeaponType weaponType, bool bEnabled)
{
    if (weaponType >= WEAPONTYPE_BRASSKNUCKLE && weaponType < WEAPONTYPE_LAST_WEAPONTYPE)
    {
        m_JetpackWeapons[weaponType] = bEnabled;
    }
}

//
// Handle basic backup of databases and config files
//
void CGame::HandleBackup()
{
    // Get backup vars
    SString strBackupPath = PathConform(m_pMainConfig->GetBackupPath()).TrimEnd(PATH_SEPERATOR);
    int     iBackupInterval = m_pMainConfig->GetBackupInterval();
    uint    uiBackupAmount = m_pMainConfig->GetBackupAmount();
    if (iBackupInterval == 0 || uiBackupAmount == 0)
        return;

    // Check if brand new installation
    CModManager* pModManager = g_pServerInterface->GetModManager();
    if (!DirectoryExists(m_pMainConfig->GetSystemDatabasesPath()) && !FileExists(m_pMainConfig->GetLogFile()) &&
        !FileExists(pModManager->GetAbsolutePath("internal.db")))
    {
        return;
    }

    // Determine date now
    time_t secondsNow = time(NULL);

    // Determine last backup date
    std::vector<SString> fileList = FindFiles(strBackupPath + "/", true, false);

    // Check each file name is a valid backup name
    for (uint f = 0; f < fileList.size(); f++)
    {
        SString       strName = fileList[f];
        const SString strCheck = "0000-00-00.zip";
        for (uint i = 0; i < std::min(strCheck.length(), strName.length()); i++)
            if (!isdigit((uchar)strName[i]) || !isdigit((uchar)strCheck[i]))
                if (strName[i] != strCheck[i])
                {
                    ListRemoveIndex(fileList, f--);
                    break;
                }
    }

    // Alpha sort
    std::sort(fileList.rbegin(), fileList.rend());

    // Check date of last backup
    if (!fileList.empty())
    {
        SString strNewest = fileList.front();
        tm      timeinfo;
        memset(&timeinfo, 0, sizeof(timeinfo));
        timeinfo.tm_year = atoi(strNewest.SubStr(0, 4)) - 1900;
        timeinfo.tm_mon = atoi(strNewest.SubStr(5, 2)) - 1;
        timeinfo.tm_mday = atoi(strNewest.SubStr(8, 2));

        time_t timeSinceBackup = secondsNow - mktime(&timeinfo);
        if (timeSinceBackup < iBackupInterval * 86400L)
            return;            // No backup required
    }

    m_pMainConfig->NotifyDidBackup();

    // Make target file name
    tm*  tmp = gmtime(&secondsNow);
    char outstr[200] = {0};
    strftime(outstr, sizeof(outstr), "%Y-%m-%d", tmp);
    SString strDateNow = outstr;
    SString strBackupZip = PathJoin(strBackupPath, strDateNow + ".zip");
    SString strTempZip = PathJoin(strBackupPath, strDateNow + "_temp.zip");

    if (FileExists(strBackupZip))
        return;            // Can't do backup as target file already exists

    MkDir(strBackupPath);

    // Delete previous temp zip if any
    FileDelete(strTempZip);

    CZipMaker zipMaker(strTempZip);
    if (!zipMaker.IsValid())
        return;            // Can't do backup as can't create target zip

    CLogger::LogPrintfNoStamp("Please wait...\n");

    // Backup config files
    zipMaker.InsertFile(pModManager->GetAbsolutePath("mtaserver.conf"), PathJoin("config", "mtaserver.conf"));
    zipMaker.InsertFile(m_pMainConfig->GetAccessControlListFile(), PathJoin("config", "acl.xml"));
    zipMaker.InsertFile(pModManager->GetAbsolutePath(FILENAME_BANLIST), PathJoin("config", "banlist.xml"));
    zipMaker.InsertFile(pModManager->GetAbsolutePath("editor.conf"), PathJoin("config", "editor.conf"));
    zipMaker.InsertFile(pModManager->GetAbsolutePath("editor_acl.xml"), PathJoin("config", "editor_acl.xml"));
    zipMaker.InsertFile(pModManager->GetAbsolutePath("local.conf"), PathJoin("config", "local.conf"));
    zipMaker.InsertFile(m_pMainConfig->GetIdFile(), PathJoin("config", "server-id.keys"));
    zipMaker.InsertFile(pModManager->GetAbsolutePath(FILENAME_SETTINGS), PathJoin("config", "settings.xml"));
    zipMaker.InsertFile(pModManager->GetAbsolutePath("vehiclecolors.conf"), PathJoin("config", "vehiclecolors.conf"));

    // Backup database files
    zipMaker.InsertDirectoryTree(m_pMainConfig->GetGlobalDatabasesPath(), PathJoin("databases", "global"));
    zipMaker.InsertDirectoryTree(m_pMainConfig->GetSystemDatabasesPath(), PathJoin("databases", "system"));
    zipMaker.InsertFile(pModManager->GetAbsolutePath("internal.db"), PathJoin("databases", "other", "internal.db"));
    zipMaker.InsertFile(pModManager->GetAbsolutePath("registry.db"), PathJoin("databases", "other", "registry.db"));

    zipMaker.Close();

    // Rename temp file to final name
    FileRename(strTempZip, strBackupZip);

    // Remove backups over min required
    while (fileList.size() >= uiBackupAmount)
    {
        SString strOldest = fileList.back();
        FileDelete(PathJoin(strBackupPath, strOldest));
        fileList.pop_back();
    }
}

//
// Toggle latent send mode
//
void CGame::EnableLatentSends(bool bEnabled, int iBandwidth, CLuaMain* pLuaMain, ushort usResourceNetId)
{
    m_bLatentSendsEnabled = bEnabled && iBandwidth;
    m_iLatentSendsBandwidth = iBandwidth;
    m_pLatentSendsLuaMain = pLuaMain;
    m_usLatentSendsResourceNetId = usResourceNetId;
}

//
// Optimization for latent sends
//
void CGame::SendPacketBatchBegin(unsigned char ucPacketId, NetBitStreamInterface* pBitStream)
{
    if (m_bLatentSendsEnabled)
        GetLatentTransferManager()->AddSendBatchBegin(ucPacketId, pBitStream);
}

//
// Maybe route though LatentTransferManager
//
bool CGame::SendPacket(unsigned char ucPacketID, const NetServerPlayerID& playerID, NetBitStreamInterface* pBitStream, bool bBroadcast,
                       NetServerPacketPriority packetPriority, NetServerPacketReliability packetReliability, ePacketOrdering packetOrdering)
{
    if (!m_bLatentSendsEnabled)
    {
        if (ucPacketID == PACKET_ID_LUA_ELEMENT_RPC)
        {
            // Get the RPC number in an ugly way
            pBitStream->ResetReadPointer();
            uchar ucRpcId = 0;
            pBitStream->Read(ucRpcId);
            pBitStream->ResetReadPointer();
            CPerfStatRPCPacketUsage::GetSingleton()->UpdatePacketUsageOut(ucRpcId, pBitStream->GetNumberOfBytesUsed());
        }
        return g_pNetServer->SendPacket(ucPacketID, playerID, pBitStream, bBroadcast, packetPriority, packetReliability, packetOrdering);
    }
    else
        GetLatentTransferManager()->AddSend(playerID, pBitStream->Version(), m_iLatentSendsBandwidth, m_pLatentSendsLuaMain, m_usLatentSendsResourceNetId);
    return true;
}

//
// Optimization for latent sends
//
void CGame::SendPacketBatchEnd()
{
    if (m_bLatentSendsEnabled)
        GetLatentTransferManager()->AddSendBatchEnd();
}

//////////////////////////////////////////////////////////////////
//
// CGame::IsBulletSyncActive
//
// Determine the state of bullet sync
//
//////////////////////////////////////////////////////////////////
bool CGame::IsBulletSyncActive()
{
    bool bConfigSaysEnable = m_pMainConfig->GetBulletSyncEnabled();
#if 0            // No auto bullet sync as there are some problems with it
    bool bGlitchesSayEnable = ( m_Glitches [ GLITCH_FASTFIRE ] || m_Glitches [ GLITCH_CROUCHBUG ] );
#else
    bool bGlitchesSayEnable = false;
#endif
    return bConfigSaysEnable || bGlitchesSayEnable;
}

//////////////////////////////////////////////////////////////////
//
// CGame::SendSyncSettings
//
// Determine and send required state of bullet sync and vehicle extrapolation
// If player is NULL, send to all joined players
//
//////////////////////////////////////////////////////////////////
void CGame::SendSyncSettings(CPlayer* pPlayer)
{
    std::set<eWeaponType> weaponTypesUsingBulletSync;

    if (IsBulletSyncActive())
    {
        // List of weapons to enable bullet sync for. (Minigun causes too many packets)
        eWeaponType weaponList[] = {
            WEAPONTYPE_PISTOL,         WEAPONTYPE_PISTOL_SILENCED, WEAPONTYPE_DESERT_EAGLE, WEAPONTYPE_SHOTGUN, WEAPONTYPE_SAWNOFF_SHOTGUN,
            WEAPONTYPE_SPAS12_SHOTGUN, WEAPONTYPE_MICRO_UZI,       WEAPONTYPE_MP5,          WEAPONTYPE_AK47,    WEAPONTYPE_M4,
            WEAPONTYPE_TEC9,           WEAPONTYPE_COUNTRYRIFLE};

        for (uint i = 0; i < NUMELMS(weaponList); i++)
            MapInsert(weaponTypesUsingBulletSync, weaponList[i]);

        // Add sniper if all clients can handle it
        if (ExtractVersionStringBuildNumber(m_pPlayerManager->GetLowestConnectedPlayerVersion()) >=
            ExtractVersionStringBuildNumber(SNIPER_BULLET_SYNC_MIN_CLIENT_VERSION))
            MapInsert(weaponTypesUsingBulletSync, WEAPONTYPE_SNIPERRIFLE);
    }

    short sVehExtrapolateBaseMs = 5;
    short sVehExtrapolatePercent = m_pMainConfig->GetVehExtrapolatePercent();
    short sVehExtrapolateMaxMs = m_pMainConfig->GetVehExtrapolatePingLimit();
    uchar ucVehExtrapolateEnabled = sVehExtrapolatePercent != 0;
    uchar ucUseAltPulseOrder = m_pMainConfig->GetUseAltPulseOrder() != 0;
    uchar ucAllowFastSprintFix = false;
    uchar ucAllowDrivebyAnimFix = false;
    uchar ucAllowShotgunDamageFix = false;

    // Add sprint fix if all clients can handle it
    if (ExtractVersionStringBuildNumber(m_pPlayerManager->GetLowestConnectedPlayerVersion()) >= ExtractVersionStringBuildNumber(SPRINT_FIX_MIN_CLIENT_VERSION))
        ucAllowFastSprintFix = true;

    // Add driveby animation fix if all clients can handle it
    if (ExtractVersionStringBuildNumber(m_pPlayerManager->GetLowestConnectedPlayerVersion()) >=
        ExtractVersionStringBuildNumber(DRIVEBY_HITBOX_FIX_MIN_CLIENT_VERSION))
        ucAllowDrivebyAnimFix = true;

    // Add shotgun bullet sync damage fix if all clients can handle it
    if (m_pPlayerManager->GetLowestConnectedPlayerVersion() >= SHOTGUN_DAMAGE_FIX_MIN_CLIENT_VERSION)
        ucAllowShotgunDamageFix = true;

    CSyncSettingsPacket packet(weaponTypesUsingBulletSync, ucVehExtrapolateEnabled, sVehExtrapolateBaseMs, sVehExtrapolatePercent, sVehExtrapolateMaxMs,
                               ucUseAltPulseOrder, ucAllowFastSprintFix, ucAllowDrivebyAnimFix, ucAllowShotgunDamageFix);
    if (pPlayer)
        pPlayer->Send(packet);
    else
        m_pPlayerManager->BroadcastOnlyJoined(packet);
}

//////////////////////////////////////////////////////////////////
//
// CGame::IsBelowMinimumClient
//
// Check if supplied version string is below current minimum requirement
//
//////////////////////////////////////////////////////////////////
bool CGame::IsBelowMinimumClient(const CMtaVersion& strVersion)
{
    return strVersion < CalculateMinClientRequirement();
}

//////////////////////////////////////////////////////////////////
//
// CGame::IsBelowRecommendedClient
//
// Check if supplied version string is below recommended
//
//////////////////////////////////////////////////////////////////
bool CGame::IsBelowRecommendedClient(const CMtaVersion& strVersion)
{
    return strVersion < m_pMainConfig->GetRecommendedClientVersion();
}

//////////////////////////////////////////////////////////////////
//
// CGame::CalculateMinClientRequirement
//
// Determine min client version setting to apply for connecting players
//
//////////////////////////////////////////////////////////////////
CMtaVersion CGame::CalculateMinClientRequirement()
{
    if (g_pGame->IsBeingDeleted())
        return "";

    // Calc effective min client version
    CMtaVersion strMinClientRequirementFromConfig = m_pMainConfig->GetMinClientVersion();
    CMtaVersion strMinClientRequirementFromResources = m_pResourceManager->GetMinClientRequirement();

    CMtaVersion strNewMin;

    if (strNewMin < strMinClientRequirementFromConfig)
        strNewMin = strMinClientRequirementFromConfig;

    if (strNewMin < strMinClientRequirementFromResources)
        strNewMin = strMinClientRequirementFromResources;

    if (g_pGame->IsBulletSyncActive())
    {
        if (strNewMin < BULLET_SYNC_MIN_CLIENT_VERSION)
            strNewMin = BULLET_SYNC_MIN_CLIENT_VERSION;
    }
    if (m_pMainConfig->GetVehExtrapolatePercent() > 0)
    {
        if (strNewMin < VEH_EXTRAPOLATION_MIN_CLIENT_VERSION)
            strNewMin = VEH_EXTRAPOLATION_MIN_CLIENT_VERSION;
    }
    if (m_pMainConfig->GetUseAltPulseOrder())
    {
        if (strNewMin < ALT_PULSE_ORDER_MIN_CLIENT_VERSION)
            strNewMin = ALT_PULSE_ORDER_MIN_CLIENT_VERSION;
    }
    if (g_pGame->IsGlitchEnabled(GLITCH_HITANIM))
    {
        if (strNewMin < HIT_ANIM_CLIENT_VERSION)
            strNewMin = HIT_ANIM_CLIENT_VERSION;
    }

    // Log effective min client version
    if (strNewMin != m_strPrevMinClientConnectRequirement)
    {
        m_strPrevMinClientConnectRequirement = strNewMin;
        if (!strNewMin.empty())
            CLogger::LogPrintf(SString("Server minclientversion is now %s\n", *strNewMin));
        else
            CLogger::LogPrintf("Server minclientversion is now cleared\n");
    }

    // Handle settings that change depending on the lowest connected player version
    if (m_strPrevLowestConnectedPlayerVersion != m_pPlayerManager->GetLowestConnectedPlayerVersion())
    {
        m_strPrevLowestConnectedPlayerVersion = m_pPlayerManager->GetLowestConnectedPlayerVersion();
        SendSyncSettings();
    }

    // Do version based kick check as well
    {
        CMtaVersion strKickMin;

        if (g_pGame->IsBulletSyncActive())
        {
            if (strKickMin < BULLET_SYNC_MIN_CLIENT_VERSION)
                strKickMin = BULLET_SYNC_MIN_CLIENT_VERSION;
        }
        if (m_pMainConfig->GetVehExtrapolatePercent() > 0)
        {
            if (strKickMin < VEH_EXTRAPOLATION_MIN_CLIENT_VERSION)
                strKickMin = VEH_EXTRAPOLATION_MIN_CLIENT_VERSION;
        }
        if (m_pMainConfig->GetUseAltPulseOrder())
        {
            if (strKickMin < ALT_PULSE_ORDER_MIN_CLIENT_VERSION)
                strKickMin = ALT_PULSE_ORDER_MIN_CLIENT_VERSION;
        }

        if (strKickMin != m_strPrevMinClientKickRequirement)
        {
            m_strPrevMinClientKickRequirement = strKickMin;

            // Do kicking
            uint uiNumIncompatiblePlayers = 0;
            for (std::list<CPlayer*>::const_iterator iter = g_pGame->GetPlayerManager()->IterBegin(); iter != g_pGame->GetPlayerManager()->IterEnd(); iter++)
            {
                CPlayer* pPlayer = *iter;
                if (strKickMin > pPlayer->GetPlayerVersion())
                {
                    CStaticFunctionDefinitions::RedirectPlayer(pPlayer, "", 0, NULL);
                    uiNumIncompatiblePlayers++;
                }
            }

            if (uiNumIncompatiblePlayers > 0)
                CLogger::LogPrintf(SString("Forced %d player(s) to reconnect so they can update to %s\n", uiNumIncompatiblePlayers, *strKickMin));
        }
    }

    // Also seems a good place to keep this setting synchronized
    g_pBandwidthSettings->NotifyBulletSyncEnabled(g_pGame->IsBulletSyncActive());

#ifndef MTA_DEBUG
    if (strNewMin < RELEASE_MIN_CLIENT_VERSION)
        strNewMin = RELEASE_MIN_CLIENT_VERSION;
#endif

    return strNewMin;
}

//
// Handle encryption of Windows crash dump files
//
void CGame::HandleCrashDumpEncryption()
{
#ifdef WIN32
    SString strDumpDirPath = g_pServerInterface->GetAbsolutePath(SERVER_DUMP_PATH);
    SString strDumpDirPrivatePath = PathJoin(strDumpDirPath, "private");
    SString strDumpDirPublicPath = PathJoin(strDumpDirPath, "public");
    MakeSureDirExists(strDumpDirPrivatePath + "/");
    MakeSureDirExists(strDumpDirPublicPath + "/");

    SString strMessage = "Dump files in this directory are encrypted and copied to 'dumps\\public' during server startup\n\n";
    FileSave(PathJoin(strDumpDirPrivatePath, "README.txt"), strMessage);

    // Copy and encrypt private files to public if they don't already exist
    {
        std::vector<SString> privateList = FindFiles(PathJoin(strDumpDirPrivatePath, "*.dmp"), true, false);
        for (uint i = 0; i < privateList.size(); i++)
        {
            const SString& strPrivateFilename = privateList[i];
            SString        strPublicFilename = ExtractBeforeExtension(strPrivateFilename) + ".rsa." + ExtractExtension(strPrivateFilename);
            SString        strPrivatePathFilename = PathJoin(strDumpDirPrivatePath, strPrivateFilename);
            SString        strPublicPathFilename = PathJoin(strDumpDirPublicPath, strPublicFilename);
            if (!FileExists(strPublicPathFilename))
            {
                g_pRealNetServer->EncryptDumpfile(strPrivatePathFilename, strPublicPathFilename);
            }
        }
    }
#endif
}
