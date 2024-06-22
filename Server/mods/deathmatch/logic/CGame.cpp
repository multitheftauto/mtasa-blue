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
#include "CGame.h"
#include "CAccessControlListManager.h"
#include "ASE.h"
#include "CPerfStatManager.h"
#include "CSettings.h"
#include "CZoneNames.h"
#include "CRemoteCalls.h"
#include "luadefs/CLuaDefs.h"
#include "CRegistry.h"
#include "CLanBroadcast.h"
#include "CRegisteredCommands.h"
#include "CTickRateSettings.h"
#include "CBuildingRemovalManager.h"
#include "CDebugHookManager.h"
#include "CTrainTrackManager.h"
#include "lua/CLuaCallback.h"
#include "CWeaponStatManager.h"
#include "CPedSync.h"
#include "CHTTPD.h"
#include "CBan.h"
#include "CPlayerCamera.h"
#include "CPacketTranslator.h"
#include "CAccountManager.h"
#include "CWaterManager.h"
#include "CResourceManager.h"
#include "CMapManager.h"
#include "CMarkerManager.h"
#include "CHandlingManager.h"
#include "CScriptDebugging.h"
#include "CBandwidthSettings.h"
#include "CMainConfig.h"
#include "CUnoccupiedVehicleSync.h"
#include "CRegistryManager.h"
#include "CLatentTransferManager.h"
#include "CCommandFile.h"
#include "packets/CVoiceEndPacket.h"
#include "packets/CEntityAddPacket.h"
#include "packets/CUpdateInfoPacket.h"
#include "packets/CPlayerWastedPacket.h"
#include "packets/CElementRPCPacket.h"
#include "packets/CReturnSyncPacket.h"
#include "packets/CPlayerNoSocketPacket.h"
#include "packets/CPlayerConnectCompletePacket.h"
#include "packets/CPlayerJoinCompletePacket.h"
#include "packets/CPlayerResourceStartPacket.h"
#include "packets/CPlayerNetworkStatusPacket.h"
#include "packets/CPlayerListPacket.h"
#include "packets/CPlayerClothesPacket.h"
#include "packets/CServerInfoSyncPacket.h"
#include "../utils/COpenPortsTester.h"
#include "../utils/CMasterServerAnnouncer.h"
#include "../utils/CHqComms.h"
#include "../utils/CFunctionUseLogger.h"
#include "Utils.h"
#include "CStaticFunctionDefinitions.h"
#include "lua/CLuaFunctionParseHelpers.h"
#include "CZipMaker.h"
#include "version.h"
#include "net/SimHeaders.h"
#include <signal.h>

#define MAX_BULLETSYNC_DISTANCE      400.0f
#define MAX_EXPLOSION_SYNC_DISTANCE  400.0f
#define MAX_PROJECTILE_SYNC_DISTANCE 400.0f

#define RELEASE_MIN_CLIENT_VERSION          "1.6.0-0.00000"
#define FIREBALLDESTRUCT_MIN_CLIENT_VERSION "1.6.0-9.22199"

#ifndef WIN32
    #include <limits.h>

    #ifndef MAX_PATH
        #define MAX_PATH PATH_MAX
    #endif
#endif

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

    // Setup world special properties
    m_WorldSpecialProps[WorldSpecialProperty::HOVERCARS] = false;
    m_WorldSpecialProps[WorldSpecialProperty::AIRCARS] = false;
    m_WorldSpecialProps[WorldSpecialProperty::EXTRABUNNY] = false;
    m_WorldSpecialProps[WorldSpecialProperty::EXTRAJUMP] = false;
    m_WorldSpecialProps[WorldSpecialProperty::RANDOMFOLIAGE] = true;
    m_WorldSpecialProps[WorldSpecialProperty::SNIPERMOON] = false;
    m_WorldSpecialProps[WorldSpecialProperty::EXTRAAIRRESISTANCE] = true;
    m_WorldSpecialProps[WorldSpecialProperty::UNDERWORLDWARP] = true;
    m_WorldSpecialProps[WorldSpecialProperty::VEHICLESUNGLARE] = false;
    m_WorldSpecialProps[WorldSpecialProperty::CORONAZTEST] = true;
    m_WorldSpecialProps[WorldSpecialProperty::WATERCREATURES] = true;
    m_WorldSpecialProps[WorldSpecialProperty::BURNFLIPPEDCARS] = true;
    m_WorldSpecialProps[WorldSpecialProperty::FIREBALLDESTRUCT] = true;
    m_WorldSpecialProps[WorldSpecialProperty::EXTENDEDWATERCANNONS] = true;
    m_WorldSpecialProps[WorldSpecialProperty::ROADSIGNSTEXT] = true;

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

    ProcessClientTriggeredEventSpam();

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
    m_pMapManager =
        new CMapManager(m_pBlipManager, m_pObjectManager, m_pPickupManager, m_pPlayerManager, m_pRadarAreaManager, m_pMarkerManager, m_pVehicleManager,
                        m_pTeamManager, m_pPedManager, m_pColManager, m_pWaterManager, m_pClock, m_pGroups, &m_Events, m_pScriptDebugging, &m_ElementDeleter);
    m_pACLManager = new CAccessControlListManager;
    m_pHqComms = new CHqComms;

    m_pRegisteredCommands = new CRegisteredCommands(m_pACLManager);
    m_pLuaManager = new CLuaManager(m_pObjectManager, m_pPlayerManager, m_pVehicleManager, m_pBlipManager, m_pRadarAreaManager, m_pRegisteredCommands,
                                    m_pMapManager, &m_Events);
    m_pConsole = new CConsole(m_pBlipManager, m_pMapManager, m_pPlayerManager, m_pRegisteredCommands, m_pVehicleManager, m_pBanManager, m_pACLManager);
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
#elif defined(ANY_arm)
        " [arm]"
#elif defined(ANY_arm64)
        " [arm64]"
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
        if (!m_pScriptDebugging->SetLogfile(
            m_pMainConfig->GetScriptDebugLogFile().c_str(),
            m_pMainConfig->GetScriptDebugLogLevelMap()
        ))
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

        case PACKET_ID_PLAYER_RESOURCE_START:
        {
            Packet_PlayerResourceStart(static_cast<CPlayerResourceStartPacket&>(Packet));
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
    Player.Send(CPlayerJoinCompletePacket(Player.GetID(), m_pMapManager->GetRootElement()->GetID(), m_pMainConfig->GetHTTPDownloadType(),
                                          m_pMainConfig->GetHTTPPort(), m_pMainConfig->GetHTTPDownloadURL().c_str(),
                                          m_pMainConfig->GetHTTPMaxConnectionsPerClient(), m_pMainConfig->GetEnableClientChecks(),
                                          m_pMainConfig->IsVoiceEnabled(), m_pMainConfig->GetVoiceSampleRate(), m_pMainConfig->GetVoiceQuality(),
                                          m_pMainConfig->GetVoiceBitrate(), m_pMainConfig->GetServerName().c_str()));

    marker.Set("CPlayerJoinCompletePacket");

    // Sync up server info on entry
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

    // Tell client the transfer box visibility
    CStaticFunctionDefinitions::SendClientTransferBoxVisibility(&Player);

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
        CLogger::LogPrintf("QUIT: %s left the game [%s] %s\n", szNick, szReason, *Player.GetQuitReasonForLog());
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
    m_Events.AddEvent("onResourceStateChange", "resource, oldState, newState", nullptr, false);
    m_Events.AddEvent("onResourceLoadStateChange", "resource, oldState, newState", NULL, false);

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
    m_Events.AddEvent("onPlayerChat", "text, messageType", NULL, false);
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
    m_Events.AddEvent("onPlayerResourceStart", "resource", NULL, false);
    m_Events.AddEvent("onPlayerProjectileCreation", "weaponType, posX, posY, posZ, force, target, rotX, rotY, rotZ, velX, velY, velZ", nullptr, false);
    m_Events.AddEvent("onPlayerDetonateSatchels", "", nullptr, false);
    m_Events.AddEvent("onPlayerTriggerEventThreshold", "", nullptr, false);
    m_Events.AddEvent("onPlayerTeamChange", "oldTeam, newTeam", nullptr, false);
    m_Events.AddEvent("onPlayerTriggerInvalidEvent", "eventName, isAdded, isRemote", nullptr, false);

    // Ped events
    m_Events.AddEvent("onPedVehicleEnter", "vehicle, seat, jacked", NULL, false);
    m_Events.AddEvent("onPedVehicleExit", "vehicle, reason, jacker", NULL, false);
    m_Events.AddEvent("onPedWasted", "ammo, killer, weapon, bodypart", NULL, false);
    m_Events.AddEvent("onPedWeaponSwitch", "previous, current", NULL, false);
    m_Events.AddEvent("onPedDamage", "loss", NULL, false);

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
    m_Events.AddEvent("onElementInteriorChange", "oldInterior, newInterior", nullptr, false);

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

    m_Events.AddEvent("onAccountCreate", "account", NULL, false);
    m_Events.AddEvent("onAccountRemove", "account", NULL, false);

    // Other events
    m_Events.AddEvent("onSettingChange", "setting, oldValue, newValue", NULL, false);
    m_Events.AddEvent("onChatMessage", "message, element", NULL, false);
    m_Events.AddEvent("onExplosion", "x, y, z, type, origin", nullptr, false);

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

void CGame::Packet_PlayerJoin(const NetServerPlayerID& Source)
{
    // Reply with the mod this server is running
    NetBitStreamInterface* pBitStream = g_pNetServer->AllocateNetServerBitStream(0);
    if (pBitStream)
    {
        // Write the mod name to the bitstream
        pBitStream->Write(static_cast<unsigned short>(MTA_DM_BITSTREAM_VERSION));
        pBitStream->WriteString("deathmatch");

        // Send and destroy the bitstream
        g_pNetServer->SendPacket(PACKET_ID_MOD_NAME, Source, pBitStream, false, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
        g_pNetServer->DeallocateNetServerBitStream(pBitStream);
    }
}

void CGame::Packet_PlayerJoinData(CPlayerJoinDataPacket& Packet)
{
    // Grab the nick
    const char* szNick = Packet.GetNick();
    if (szNick && szNick[0] != 0)
    {
        // Is the server passworded?
        bool bPasswordIsValid = true;
        if (m_pMainConfig->HasPassword())
        {
            // Grab the password hash from the packet
            const MD5& PacketHash = Packet.GetPassword();

            // Hash our password
            const std::string& strPassword = m_pMainConfig->GetPassword();
            MD5                ConfigHash;
            CMD5Hasher         Hasher;
            if (!strPassword.empty() && Hasher.Calculate(strPassword.c_str(), strPassword.length(), ConfigHash))
            {
                // Compare the hashes
                if (memcmp(&ConfigHash, &PacketHash, sizeof(MD5)) != 0)
                {
                    bPasswordIsValid = false;
                }
            }
        }

        // Add the player
        CPlayer* pPlayer = m_pPlayerManager->Create(Packet.GetSourceSocket());

        if (!pPlayer)
            return;

        if (pPlayer->GetID() == INVALID_ELEMENT_ID)
        {
            // Tell the console
            CLogger::LogPrintf("CONNECT: %s failed to connect (Player element could not be created.)\n", szNick);

            // Tell the player the problem
            return DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::ELEMENT_FAILURE);
        }

        // Set the bitstream version number for this connection
        pPlayer->SetBitStreamVersion(Packet.GetBitStreamVersion());
        g_pNetServer->SetClientBitStreamVersion(Packet.GetSourceSocket(), Packet.GetBitStreamVersion());

        // Get the serial number from the packet source
        NetServerPlayerID p = Packet.GetSourceSocket();
        SString           strSerial;
        SString           strExtra;
        CMtaVersion       strPlayerVersion;
        {
            SFixedString<32> strSerialTemp;
            SFixedString<64> strExtraTemp;
            SFixedString<32> strPlayerVersionTemp;
            g_pNetServer->GetClientSerialAndVersion(p, strSerialTemp, strExtraTemp, strPlayerVersionTemp);
            strSerial = SStringX(strSerialTemp);
            strExtra = SStringX(strExtraTemp);
            strPlayerVersion = SStringX(strPlayerVersionTemp);
        }
#if MTASA_VERSION_TYPE < VERSION_TYPE_UNSTABLE
        if (atoi(ExtractVersionStringBuildNumber(Packet.GetPlayerVersion())) != 0)
        {
            // Use player version from packet if it contains a valid build number
            strPlayerVersion = Packet.GetPlayerVersion();
        }
#endif

        SString strIP = pPlayer->GetSourceIP();
        SString strIPAndSerial("IP: %s  Serial: %s  Version: %s", strIP.c_str(), strSerial.c_str(), strPlayerVersion.c_str());
        if (!CheckNickProvided(szNick))            // check the nick is valid
        {
            // Tell the console
            CLogger::LogPrintf("CONNECT: %s failed to connect (Invalid nickname) (%s)\n", szNick, strIPAndSerial.c_str());

            // Tell the player the problem
            DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::INVALID_NICKNAME);
            return;
        }

        // Check the size of the nick
        size_t sizeNick = strlen(szNick);
        if (sizeNick >= MIN_PLAYER_NICK_LENGTH && sizeNick <= MAX_PLAYER_NICK_LENGTH)
        {
            // Someone here with the same name?
            CPlayer* pTempPlayer = m_pPlayerManager->Get(szNick);
            if (pTempPlayer)
            {
                // Same person? Quit the first and let this one join
                if (strcmp(pPlayer->GetSourceIP(), pTempPlayer->GetSourceIP()) == 0)
                {
                    // Two players could have the same IP, so see if the old player appears inactive before quitting them
                    if (pTempPlayer->UhOhNetworkTrouble())
                    {
                        pTempPlayer->Send(CPlayerDisconnectedPacket(SString("Supplanted by %s from %s", szNick, pPlayer->GetSourceIP())));
                        // Tell the console
                        CLogger::LogPrintf("DISCONNECT: %s Supplanted by (%s)\n", szNick, pTempPlayer->GetNick());
                        QuitPlayer(*pTempPlayer, CClient::QUIT_QUIT);
                        pTempPlayer = NULL;
                    }
                }
            }
            if (pTempPlayer == NULL)
            {
                // Correct version?
                if (Packet.GetNetVersion() == MTA_DM_NETCODE_VERSION)
                {
                    // If the password is valid
                    if (bPasswordIsValid)
                    {
                        // If he's not join flooding
                        if (!m_pMainConfig->GetJoinFloodProtectionEnabled() || !m_FloodProtect.AddConnect(SString("%x", Packet.GetSourceIP())))
                        {
                            // Set the nick and the game version
                            pPlayer->SetNick(szNick);
                            pPlayer->SetGameVersion(Packet.GetGameVersion());
                            pPlayer->SetMTAVersion(Packet.GetMTAVersion());
                            pPlayer->SetSerialUser(Packet.GetSerialUser());
                            pPlayer->SetSerial(strSerial, 0);
                            pPlayer->SetSerial(strExtra, 1);
                            pPlayer->SetPlayerVersion(strPlayerVersion);

                            // Check if client must update
                            if (IsBelowMinimumClient(pPlayer->GetPlayerVersion()) && !pPlayer->ShouldIgnoreMinClientVersionChecks())
                            {
                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s failed to connect (Client version is below minimum) (%s)\n", szNick, strIPAndSerial.c_str());

                                // Tell the player
                                pPlayer->Send(CUpdateInfoPacket("Mandatory", CalculateMinClientRequirement()));
                                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::NO_REASON);
                                return;
                            }

                            // Check if client should optionally update
                            if (Packet.IsOptionalUpdateInfoRequired() && IsBelowRecommendedClient(pPlayer->GetPlayerVersion()) &&
                                !pPlayer->ShouldIgnoreMinClientVersionChecks())
                            {
                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s advised to update (Client version is below recommended) (%s)\n", szNick,
                                                   strIPAndSerial.c_str());

                                // Tell the player
                                pPlayer->Send(CUpdateInfoPacket("Optional", GetConfig()->GetRecommendedClientVersion()));
                                DisconnectPlayer(this, *pPlayer, "");
                                return;
                            }

                            // Check the serial for validity
                            if (CBan* pBan = m_pBanManager->GetBanFromSerial(pPlayer->GetSerial().c_str()))
                            {
                                time_t  Duration = pBan->GetBanTimeRemaining();
                                SString strBanMessage = "Serial is banned";
                                SString strDurationDesc = pBan->GetDurationDesc();
                                if (strDurationDesc.length())
                                    strBanMessage += " (" + strDurationDesc + ")";

                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s failed to connect (%s) (%s)\n", szNick, strBanMessage.c_str(), strIPAndSerial.c_str());

                                // Tell the player he's banned
                                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::BANNED_SERIAL, Duration, pBan->GetReason().c_str());
                                return;
                            }

                            // Check the ip for banness
                            if (CBan* pBan = m_pBanManager->GetBanFromIP(strIP))
                            {
                                time_t  Duration = pBan->GetBanTimeRemaining();
                                SString strBanMessage;            // = "Serial is banned";
                                SString strDurationDesc = pBan->GetDurationDesc();
                                if (strDurationDesc.length())
                                    strBanMessage += " (" + strDurationDesc + ")";

                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s failed to connect (IP is banned%s) (%s)\n", szNick, strBanMessage.c_str(),
                                                   strIPAndSerial.c_str());

                                // Tell the player he's banned
                                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::BANNED_IP, Duration, pBan->GetReason().c_str());
                                return;
                            }

                            if (!pPlayer->GetSerialUser().empty() && m_pBanManager->IsAccountBanned(pPlayer->GetSerialUser().c_str()))
                            {
                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s failed to connect (Account is banned) (%s)\n", szNick, strIPAndSerial.c_str());

                                CBan*   pBan = m_pBanManager->GetBanFromAccount(pPlayer->GetSerialUser().c_str());
                                time_t  Duration = 0;
                                SString strReason;
                                if (pBan)
                                {
                                    strReason = pBan->GetReason();
                                    Duration = pBan->GetBanTimeRemaining();
                                }

                                // Tell the player he's banned
                                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::BANNED_ACCOUNT, Duration, strReason.c_str());
                                return;
                            }

#if MTASA_VERSION_TYPE > VERSION_TYPE_UNSTABLE
                            if (Packet.GetPlayerVersion().length() > 0 && Packet.GetPlayerVersion() != pPlayer->GetPlayerVersion())
                            {
                                // Tell the console
                                CLogger::LogPrintf("CONNECT: %s failed to connect (Version mismatch) (%s)\n", szNick, strIPAndSerial.c_str());

                                // Tell the player
                                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::VERSION_MISMATCH);
                                return;
                            }
#endif

                            PlayerCompleteConnect(pPlayer);
                        }
                        else
                        {
                            // Tell the console
                            CLogger::LogPrintf("CONNECT: %s failed to connect (Join flood) (%s)\n", szNick, strIPAndSerial.c_str());

                            // Tell the player the problem
                            DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::JOIN_FLOOD);
                        }
                    }
                    else
                    {
                        // Tell the console
                        CLogger::LogPrintf("CONNECT: %s failed to connect (Wrong password) (%s)\n", szNick, strIPAndSerial.c_str());

                        // Tell the player the password was wrong
                        DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::INVALID_PASSWORD);
                    }
                }
                else
                {
                    // Tell the console
                    CLogger::LogPrintf("CONNECT: %s failed to connect (Bad version) (%s)\n", szNick, strIPAndSerial.c_str());

                    // Tell the player the problem
                    SString strMessage;
                    ushort  usClientNetVersion = Packet.GetNetVersion();
                    ushort  usServerNetVersion = MTA_DM_NETCODE_VERSION;
                    ushort  usClientBranchId = usClientNetVersion >> 12;
                    ushort  usServerBranchId = usServerNetVersion >> 12;

                    CPlayerDisconnectedPacket::ePlayerDisconnectType eType;

                    if (usClientBranchId != usServerBranchId)
                    {
                        eType = CPlayerDisconnectedPacket::DIFFERENT_BRANCH;
                        strMessage = SString("(client: %X, server: %X)\n", usClientBranchId, usServerBranchId);
                    }
                    else if (MTASA_VERSION_BUILD == 0)
                    {
                        eType = CPlayerDisconnectedPacket::BAD_VERSION;
                        strMessage = SString("(client: %X, server: %X)\n", usClientNetVersion, usServerNetVersion);
                    }
                    else
                    {
                        if (usClientNetVersion < usServerNetVersion)
                        {
                            eType = CPlayerDisconnectedPacket::SERVER_NEWER;
                            strMessage = SString("(%d)\n", MTASA_VERSION_BUILD);
                        }
                        else
                        {
                            eType = CPlayerDisconnectedPacket::SERVER_OLDER;
                            strMessage = SString("(%d)\n", MTASA_VERSION_BUILD);
                        }
                    }
                    DisconnectPlayer(this, *pPlayer, eType, strMessage);
                }
            }
            else
            {
                // Tell the console
                CLogger::LogPrintf("CONNECT: %s failed to connect (Nick already in use) (%s)\n", szNick, strIPAndSerial.c_str());

                // Tell the player the problem
                DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::NICK_CLASH);
            }
        }
        else
        {
            // Tell the console
            CLogger::LogPrintf("CONNECT: %s failed to connect (Invalid nickname)\n", pPlayer->GetSourceIP());

            // Tell the player the problem
            DisconnectPlayer(this, *pPlayer, CPlayerDisconnectedPacket::INVALID_NICKNAME);
        }
    }
}

void CGame::Packet_PedWasted(CPedWastedPacket& Packet)
{
    CPed* pPed = GetElementFromId<CPed>(Packet.m_PedID);
    if (pPed && !pPed->IsDead())
    {
        pPed->SetIsDead(true);
        pPed->SetHealth(0.0f);
        pPed->SetArmor(0.0f);
        pPed->SetPosition(Packet.m_vecPosition);

        // Reset his vehicle action, but only if not jacking
        // If jacking we wait for him to reply with VEHICLE_NOTIFY_JACK_ABORT
        // We don't know if he actually jacked the person at this point, and we need to set the jacked person correctly (fix for #908)
        if (pPed->GetVehicleAction() != CPed::VEHICLEACTION_JACKING)
            pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);

        // Remove him from any occupied vehicle
        CVehicle* pVehicle = pPed->GetOccupiedVehicle();
        if (pVehicle)
        {
            pVehicle->SetOccupant(NULL, pPed->GetOccupiedVehicleSeat());
            pPed->SetOccupiedVehicle(NULL, 0);
        }

        CElement* pKiller = (Packet.m_Killer != INVALID_ELEMENT_ID) ? CElementIDs::GetElement(Packet.m_Killer) : NULL;

        // Create a new packet to send to everyone
        CPedWastedPacket ReturnWastedPacket(pPed, pKiller, Packet.m_ucKillerWeapon, Packet.m_ucBodyPart, false, Packet.m_AnimGroup, Packet.m_AnimID);
        m_pPlayerManager->BroadcastOnlyJoined(ReturnWastedPacket);

        // Tell our scripts the player has died
        CLuaArguments Arguments;
        Arguments.PushNumber(Packet.m_usAmmo);
        if (pKiller)
            Arguments.PushElement(pKiller);
        else
            Arguments.PushBoolean(false);
        if (Packet.m_ucKillerWeapon != 0xFF)
            Arguments.PushNumber(Packet.m_ucKillerWeapon);
        else
            Arguments.PushBoolean(false);
        if (Packet.m_ucBodyPart != 0xFF)
            Arguments.PushNumber(Packet.m_ucBodyPart);
        else
            Arguments.PushBoolean(false);
        Arguments.PushBoolean(false);
        pPed->CallEvent("onPedWasted", Arguments);

        // Reset the weapons list, because a ped loses his weapons on death
        for (unsigned int slot = 0; slot < WEAPON_SLOTS; ++slot)
        {
            pPed->SetWeaponType(0, slot);
            pPed->SetWeaponAmmoInClip(0, slot);
            pPed->SetWeaponTotalAmmo(0, slot);
        }
    }
}
void CGame::Packet_PlayerWasted(CPlayerWastedPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && !pPlayer->IsDead())
    {
        pPlayer->SetSpawned(false);
        pPlayer->SetIsDead(true);
        pPlayer->SetHealth(0.0f);
        pPlayer->SetArmor(0.0f);
        pPlayer->SetPosition(Packet.m_vecPosition);

        // Reset his vehicle action, but only if not jacking
        // If jacking we wait for him to reply with VEHICLE_NOTIFY_JACK_ABORT
        // We don't know if he actually jacked the person at this point, and we need to set the jacked person correctly (fix for #908)
        if (pPlayer->GetVehicleAction() != CPed::VEHICLEACTION_JACKING)
            pPlayer->SetVehicleAction(CPed::VEHICLEACTION_NONE);

        // Remove him from any occupied vehicle
        CVehicle* pVehicle = pPlayer->GetOccupiedVehicle();
        if (pVehicle)
        {
            pVehicle->SetOccupant(NULL, pPlayer->GetOccupiedVehicleSeat());
            pPlayer->SetOccupiedVehicle(NULL, 0);
        }

        CElement* pKiller = (Packet.m_Killer != INVALID_ELEMENT_ID) ? CElementIDs::GetElement(Packet.m_Killer) : NULL;

        // Create a new packet to send to everyone
        CPlayerWastedPacket ReturnWastedPacket(pPlayer, pKiller, Packet.m_ucKillerWeapon, Packet.m_ucBodyPart, false, Packet.m_AnimGroup, Packet.m_AnimID);
        // Caz: send this to the local player to avoid issue #8148 - "Desync when calling spawnPlayer from an event handler remotely triggered from within
        // onClientPlayerWasted"
        m_pPlayerManager->BroadcastOnlyJoined(ReturnWastedPacket);

        // Tell our scripts the player has died
        CLuaArguments Arguments;
        Arguments.PushNumber(Packet.m_usAmmo);
        if (pKiller)
            Arguments.PushElement(pKiller);
        else
            Arguments.PushBoolean(false);
        if (Packet.m_ucKillerWeapon != 0xFF)
            Arguments.PushNumber(Packet.m_ucKillerWeapon);
        else
            Arguments.PushBoolean(false);
        if (Packet.m_ucBodyPart != 0xFF)
            Arguments.PushNumber(Packet.m_ucBodyPart);
        else
            Arguments.PushBoolean(false);
        Arguments.PushBoolean(false);
        pPlayer->CallEvent("onPlayerWasted", Arguments);

        // Reset the weapons list, because a player loses his weapons on death
        for (unsigned int slot = 0; slot < WEAPON_SLOTS; ++slot)
        {
            pPlayer->SetWeaponType(0, slot);
            pPlayer->SetWeaponAmmoInClip(0, slot);
            pPlayer->SetWeaponTotalAmmo(0, slot);
        }
    }
}

void CGame::Packet_PlayerQuit(CPlayerQuitPacket& Packet)
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // Quit him
        QuitPlayer(*pPlayer);
    }
}

void CGame::Packet_PlayerTimeout(CPlayerTimeoutPacket& Packet)
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // Quit him
        QuitPlayer(*pPlayer, CClient::QUIT_TIMEOUT);
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

void CGame::Packet_PlayerPuresync(CPlayerPuresyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        pPlayer->NotifyReceivedSync();
        pPlayer->IncrementPuresync();

        // Ignore this packet if he should be in a vehicle
        if (pPlayer->GetOccupiedVehicle())
        {
            // Allow it if he's exiting
            if (pPlayer->GetVehicleAction() != CPed::VEHICLEACTION_EXITING)
                return;
        }

        // Send a returnsync packet to the player that sent it
        // Only every 4 packets.
        if ((pPlayer->GetPuresyncCount() % 4) == 0)
            pPlayer->Send(CReturnSyncPacket(pPlayer));

        CLOCK("PlayerPuresync", "RelayPlayerPuresync");
        // Relay to other players
        RelayPlayerPuresync(Packet);
        UNCLOCK("PlayerPuresync", "RelayPlayerPuresync");

        CLOCK("PlayerPuresync", "DoHitDetection");
        // Run colpoint checks
        m_pColManager->DoHitDetection(pPlayer->GetPosition(), pPlayer);
        UNCLOCK("PlayerPuresync", "DoHitDetection");
    }
}

void CGame::Packet_Command(CCommandPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Tell the console
        m_pConsole->HandleInput(Packet.GetCommand(), pPlayer, pPlayer);
    }
}

void CGame::Packet_VehicleDamageSync(CVehicleDamageSyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Grab the vehicle
        CElement* pVehicleElement = CElementIDs::GetElement(Packet.m_Vehicle);
        if (pVehicleElement && IS_VEHICLE(pVehicleElement))
        {
            CVehicle* pVehicle = static_cast<CVehicle*>(pVehicleElement);

            // Is this guy the driver or syncer?
            if (pVehicle->GetSyncer() == pPlayer || pVehicle->GetOccupant(0) == pPlayer)
            {
                // Set the new damage model
                for (unsigned int i = 0; i < MAX_DOORS; ++i)
                {
                    if (Packet.m_damage.data.bDoorStatesChanged[i])
                        pVehicle->m_ucDoorStates[i] = Packet.m_damage.data.ucDoorStates[i];
                }
                for (unsigned int i = 0; i < MAX_WHEELS; ++i)
                {
                    if (Packet.m_damage.data.bWheelStatesChanged[i])
                        pVehicle->m_ucWheelStates[i] = Packet.m_damage.data.ucWheelStates[i];
                }
                for (unsigned int i = 0; i < MAX_PANELS; ++i)
                {
                    if (Packet.m_damage.data.bPanelStatesChanged[i])
                        pVehicle->m_ucPanelStates[i] = Packet.m_damage.data.ucPanelStates[i];
                }
                for (unsigned int i = 0; i < MAX_LIGHTS; ++i)
                {
                    if (Packet.m_damage.data.bLightStatesChanged[i])
                        pVehicle->m_ucLightStates[i] = Packet.m_damage.data.ucLightStates[i];
                }

                // Make a list of players to relay this packet to
                CSendList                      sendList;
                list<CPlayer*>::const_iterator iter = m_pPlayerManager->IterBegin();
                for (; iter != m_pPlayerManager->IterEnd(); iter++)
                {
                    CPlayer* pOther = *iter;
                    if (pOther != pPlayer && pOther->IsJoined())
                    {
                        if (pOther->GetDimension() == pPlayer->GetDimension())
                        {
                            // Newer clients only need sync if vehicle has no driver
                            if (pOther->GetBitStreamVersion() < 0x5D || pVehicle->GetOccupant(0) == NULL)
                            {
                                sendList.push_back(pOther);
                            }
                        }
                    }
                }

                CPlayerManager::Broadcast(Packet, sendList);
            }
        }
    }
}

void CGame::Packet_VehiclePuresync(CVehiclePuresyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        pPlayer->NotifyReceivedSync();

        // Grab the vehicle
        CVehicle* pVehicle = pPlayer->GetOccupiedVehicle();
        if (pVehicle)
        {
            // Send a returnsync packet to the player that sent it
            pPlayer->Send(CReturnSyncPacket(pPlayer));

            // Increment counter to spread out damage info sends
            pVehicle->m_uiDamageInfoSendPhase++;

            CLOCK("VehiclePuresync", "RelayPlayerPuresync");
            // Relay to other players
            RelayPlayerPuresync(Packet);
            UNCLOCK("VehiclePuresync", "RelayPlayerPuresync");

            CVehicle* pTrailer = pVehicle->GetTowedVehicle();

            // Run colpoint checks
            CLOCK("VehiclePuresync", "DoHitDetection");
            m_pColManager->DoHitDetection(pPlayer->GetPosition(), pPlayer);
            m_pColManager->DoHitDetection(pVehicle->GetPosition(), pVehicle);
            while (pTrailer)
            {
                m_pColManager->DoHitDetection(pTrailer->GetPosition(), pTrailer);
                pTrailer = pTrailer->GetTowedVehicle();
            }
            UNCLOCK("VehiclePuresync", "DoHitDetection");
        }
    }
}

void CGame::Packet_Keysync(CKeysyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Relay to other players
        RelayNearbyPacket(Packet);
    }
}

void CGame::Packet_Bulletsync(CBulletsyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Early return when the player attempts to fire a weapon they do not have
        if (!pPlayer->HasWeaponType(Packet.m_WeaponType))
            return;

        // Relay to other players
        RelayNearbyPacket(Packet);

        // Call event
        CLuaArguments Arguments;
        Arguments.PushNumber(Packet.m_WeaponType);
        Arguments.PushNumber(Packet.m_vecEnd.fX);
        Arguments.PushNumber(Packet.m_vecEnd.fY);
        Arguments.PushNumber(Packet.m_vecEnd.fZ);

        if (Packet.m_DamagedPlayerID == INVALID_ELEMENT_ID)
        {
            Arguments.PushNil();
        }
        else
        {
            Arguments.PushElement(CElementIDs::GetElement(Packet.m_DamagedPlayerID));
        }

        Arguments.PushNumber(Packet.m_vecStart.fX);
        Arguments.PushNumber(Packet.m_vecStart.fY);
        Arguments.PushNumber(Packet.m_vecStart.fZ);
        pPlayer->CallEvent("onPlayerWeaponFire", Arguments);
    }
}

void CGame::Packet_WeaponBulletsync(CCustomWeaponBulletSyncPacket& Packet)
{
    // Grab the source player
    CPlayer*       pPlayer = Packet.GetSourcePlayer();
    CCustomWeapon* pWeapon = Packet.GetWeapon();
    if (pPlayer && pPlayer->IsJoined() && pPlayer == Packet.GetWeaponOwner())
    {
        // Tell our scripts the player has fired
        CLuaArguments Arguments;
        Arguments.PushElement(pPlayer);

        if (pWeapon->CallEvent("onWeaponFire", Arguments))
        {
            // Relay to other players
            m_pPlayerManager->BroadcastOnlyJoined(Packet, pPlayer);
        }
    }
}

void CGame::Packet_PedTask(CPedTaskPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Relay to other players
        RelayNearbyPacket(Packet);
    }
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

void CGame::Packet_LuaEvent(CLuaEventPacket& Packet)
{
    // Grab the source player, even name, element id and the arguments passed
    CPlayer*       pCaller = Packet.GetSourcePlayer();
    const char*    szName = Packet.GetName();
    ElementID      ElementID = Packet.GetElementID();
    CLuaArguments* pArguments = Packet.GetArguments();

    // Grab the element
    CElement* pElement = CElementIDs::GetElement(ElementID);
    if (pElement)
    {
        // Make sure the event exists and that it allows clientside triggering
        SEvent* pEvent = m_Events.Get(szName);
        if (pEvent)
        {
            if (pEvent->bAllowRemoteTrigger)
            {
                pElement->CallEvent(szName, *pArguments, pCaller);
            }
            else
            {
                CLuaArguments arguments;
                arguments.PushString(szName);
                arguments.PushBoolean(true);
                arguments.PushBoolean(false);
                pCaller->CallEvent("onPlayerTriggerInvalidEvent", arguments);
                m_pScriptDebugging->LogError(NULL, "Client (%s) triggered serverside event %s, but event is not marked as remotely triggerable",
                                             pCaller->GetNick(), szName);
            }

        }
            else
            {
                CLuaArguments arguments;
                arguments.PushString(szName);
                arguments.PushBoolean(false);
                arguments.PushBoolean(false);
                pCaller->CallEvent("onPlayerTriggerInvalidEvent", arguments);
                m_pScriptDebugging->LogError(NULL, "Client (%s) triggered serverside event %s, but event is not added serverside", pCaller->GetNick(), szName);
            }

        RegisterClientTriggeredEventUsage(pCaller);
    }
}

void CGame::Packet_CustomData(CCustomDataPacket& Packet)
{
    // Got a valid source?
    CPlayer* pSourcePlayer = Packet.GetSourcePlayer();
    if (pSourcePlayer)
    {
        // Grab the element
        ElementID ID = Packet.GetElementID();
        CElement* pElement = CElementIDs::GetElement(ID);
        if (pElement)
        {
            // Change the data
            const char*   szName = Packet.GetName();
            CLuaArgument& Value = Packet.GetValue();

            // Ignore if the wrong length
            if (strlen(szName) > MAX_CUSTOMDATA_NAME_LENGTH)
            {
                CLogger::ErrorPrintf("Received oversized custom data name from %s (%s)", Packet.GetSourcePlayer()->GetNick(),
                                     *SStringX(szName).Left(MAX_CUSTOMDATA_NAME_LENGTH + 1));
                return;
            }

            ESyncType lastSyncType = ESyncType::BROADCAST;
            pElement->GetCustomData(szName, false, &lastSyncType);

            if (lastSyncType != ESyncType::LOCAL)
            {
                // Tell our clients to update their data. Send to everyone but the one we got this packet from.
                unsigned short usNameLength = static_cast<unsigned short>(strlen(szName));
                CBitStream     BitStream;
                BitStream.pBitStream->WriteCompressed(usNameLength);
                BitStream.pBitStream->Write(szName, usNameLength);
                Value.WriteToBitStream(*BitStream.pBitStream);
                if (lastSyncType == ESyncType::BROADCAST)
                    m_pPlayerManager->BroadcastOnlyJoined(CElementRPCPacket(pElement, SET_ELEMENT_DATA, *BitStream.pBitStream), pSourcePlayer);
                else
                    m_pPlayerManager->BroadcastOnlySubscribed(CElementRPCPacket(pElement, SET_ELEMENT_DATA, *BitStream.pBitStream), pElement, szName,
                                                              pSourcePlayer);

                CPerfStatEventPacketUsage::GetSingleton()->UpdateElementDataUsageRelayed(szName, m_pPlayerManager->Count(),
                                                                                         BitStream.pBitStream->GetNumberOfBytesUsed());
            }

            pElement->SetCustomData(szName, Value, lastSyncType, pSourcePlayer);
        }
    }
}

void CGame::Packet_DetonateSatchels(CDetonateSatchelsPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Trigger Lua event and see if we are allowed to continue
        CLuaArguments arguments;
        if (!pPlayer->CallEvent("onPlayerDetonateSatchels", arguments))
            return;

        // Tell everyone to blow up this guy's satchels
        m_pPlayerManager->BroadcastOnlyJoined(Packet);
        // Take away their detonator
        CStaticFunctionDefinitions::TakeWeapon(pPlayer, 40);
    }
}

void CGame::Packet_DestroySatchels(CDestroySatchelsPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Tell everyone to destroy up this player's satchels
        m_pPlayerManager->BroadcastOnlyJoined(Packet);
        // Take away their detonator
        CStaticFunctionDefinitions::TakeWeapon(pPlayer, 40);
    }
}

void CGame::Packet_ExplosionSync(CExplosionSyncPacket& Packet)
{
    CPlayer* const clientSource = Packet.GetSourcePlayer();

    if (!clientSource || !clientSource->IsJoined())
        return;

    bool          syncToPlayers = true;
    CVector       explosionPosition = Packet.m_vecPosition;
    CElement*     explosionSource = nullptr;
    unsigned char explosionType = Packet.m_ucType;

    if (ElementID originID = Packet.m_OriginID; originID != INVALID_ELEMENT_ID)
    {
        if (explosionSource = CElementIDs::GetElement(originID); explosionSource != nullptr)
        {
            switch (explosionSource->GetType())
            {
                case CElement::PLAYER:
                {
                    // Shift the relative explosion position to an absolute position in the world.
                    CVehicle* occupiedVehicle = static_cast<CPlayer*>(explosionSource)->GetOccupiedVehicle();

                    if (occupiedVehicle)
                    {
                        explosionPosition += occupiedVehicle->GetPosition();
                        explosionSource = occupiedVehicle;
                    }
                    else
                        explosionPosition += explosionSource->GetPosition();

                    break;
                }
                case CElement::VEHICLE:
                {
                    // Shift the relative explosion position to an absolute position in the world.
                    explosionPosition += explosionSource->GetPosition();

                    // Has the vehicle blown up?
                    switch (explosionType)
                    {
                        case CExplosionSyncPacket::EXPLOSION_CAR:
                        case CExplosionSyncPacket::EXPLOSION_CAR_QUICK:
                        case CExplosionSyncPacket::EXPLOSION_BOAT:
                        case CExplosionSyncPacket::EXPLOSION_HELI:
                        case CExplosionSyncPacket::EXPLOSION_TINY:
                        {
                            CVehicle*        vehicle = static_cast<CVehicle*>(explosionSource);
                            VehicleBlowState previousBlowState = vehicle->GetBlowState();

                            if (previousBlowState != VehicleBlowState::BLOWN)
                            {
                                vehicle->SetBlowState(VehicleBlowState::BLOWN);
                                vehicle->SetEngineOn(false);

                                // NOTE(botder): We only trigger this event if we didn't blow up a vehicle with `blowVehicle`
                                if (previousBlowState == VehicleBlowState::INTACT)
                                {
                                    CLuaArguments arguments;
                                    arguments.PushBoolean(!Packet.m_blowVehicleWithoutExplosion);
                                    vehicle->CallEvent("onVehicleExplode", arguments);
                                }

                                syncToPlayers = vehicle->GetBlowState() == VehicleBlowState::BLOWN && !vehicle->IsBeingDeleted();
                            }
                            else
                            {
                                syncToPlayers = false;
                            }
                        }
                    }

                    break;
                }
                default:
                    break;
            }
        }
    }

    if (!syncToPlayers)
        return;

    if (!explosionSource)
        explosionSource = m_pMapManager->GetRootElement();

    CLuaArguments arguments;
    arguments.PushNumber(explosionPosition.fX);
    arguments.PushNumber(explosionPosition.fY);
    arguments.PushNumber(explosionPosition.fZ);
    arguments.PushNumber(explosionType);
    // TODO: The client uses a nearby player as the origin, if there is none, and we don't want that.
    // arguments.PushElement(explosionSource);
    syncToPlayers = clientSource->CallEvent("onExplosion", arguments);

    if (!syncToPlayers)
        return;

    // Make a list of players to send this packet to (including the explosion reporter).
    CSendList sendList;

    for (auto iter = m_pPlayerManager->IterBegin(); iter != m_pPlayerManager->IterEnd(); ++iter)
    {
        CPlayer* player = *iter;

        CVector cameraPosition;
        player->GetCamera()->GetPosition(cameraPosition);

        // Is this players camera close enough to send?
        if (IsPointNearPoint3D(explosionPosition, cameraPosition, MAX_EXPLOSION_SYNC_DISTANCE))
        {
            sendList.push_back(player);
        }
    }

    if (!sendList.empty())
        CPlayerManager::Broadcast(Packet, sendList);
}

void CGame::Packet_ProjectileSync(CProjectileSyncPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        CVector vecPosition = Packet.m_vecOrigin;
        if (Packet.m_OriginID != INVALID_ELEMENT_ID)
        {
            CElement* pOriginSource = CElementIDs::GetElement(Packet.m_OriginID);
            if (pOriginSource)
                vecPosition += pOriginSource->GetPosition();
        }

        CLuaArguments arguments;
        arguments.PushNumber(Packet.m_ucWeaponType);            // "weaponType"
        arguments.PushNumber(vecPosition.fX);                   // "posX"
        arguments.PushNumber(vecPosition.fY);                   // "posY"
        arguments.PushNumber(vecPosition.fZ);                   // "posZ"
        arguments.PushNumber(Packet.m_fForce);                  // "force"

        CElement* pTarget = nullptr;
        if (Packet.m_bHasTarget && Packet.m_TargetID != INVALID_ELEMENT_ID)
            pTarget = CElementIDs::GetElement(Packet.m_TargetID);

        arguments.PushElement(pTarget);                            // "target"
        arguments.PushNumber(Packet.m_vecRotation.fX);             // "rotX"
        arguments.PushNumber(Packet.m_vecRotation.fY);             // "rotY"
        arguments.PushNumber(Packet.m_vecRotation.fZ);             // "rotZ"
        arguments.PushNumber(Packet.m_vecMoveSpeed.fX);            // "velX"
        arguments.PushNumber(Packet.m_vecMoveSpeed.fY);            // "velY"
        arguments.PushNumber(Packet.m_vecMoveSpeed.fZ);            // "velZ"

        // Trigger Lua event and see if we are allowed to continue
        if (!pPlayer->CallEvent("onPlayerProjectileCreation", arguments))
            return;

        // Make a list of players to send this packet to
        CSendList sendList;

        // Loop through all the players
        std::list<CPlayer*>::const_iterator iter = m_pPlayerManager->IterBegin();
        for (; iter != m_pPlayerManager->IterEnd(); iter++)
        {
            CPlayer* pSendPlayer = *iter;

            // Not the player we got the packet from?
            if (pSendPlayer != pPlayer)
            {
                // Grab this player's camera position
                CVector vecCameraPosition;
                pSendPlayer->GetCamera()->GetPosition(vecCameraPosition);

                // Is this players camera close enough to send?
                if (IsPointNearPoint3D(vecPosition, vecCameraPosition, MAX_PROJECTILE_SYNC_DISTANCE))
                {
                    // Send the packet to him
                    sendList.push_back(pSendPlayer);
                }
            }
        }
        CPlayerManager::Broadcast(Packet, sendList);
    }
}

void CGame::Packet_Vehicle_InOut(CVehicleInOutPacket& Packet)
{
    // Grab the source player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // Joined?
        if (pPlayer->IsJoined())
        {
            // Grab the ped with the chosen ID
            ElementID PedID = Packet.GetPedID();
            CElement* pPedElement = CElementIDs::GetElement(PedID);
            if (pPedElement && IS_PED(pPedElement))
            {
                CPed*     pPed = static_cast<CPed*>(pPedElement);
                bool      bValidPed = false;
                bool      bValidVehicle = false;
                CSendList sendListIncompatiblePlayers;

                // Grab the vehicle with the chosen ID
                ElementID VehicleID = Packet.GetVehicleID();
                CElement* pVehicleElement = CElementIDs::GetElement(VehicleID);
                if (!pVehicleElement || !IS_VEHICLE(pVehicleElement))
                {
                    // Tell the client we failed
                    CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_ATTEMPT_FAILED);
                    pPlayer->Send(Reply);
                    return;
                }
                CVehicle* pVehicle = static_cast<CVehicle*>(pVehicleElement);

                // Grab the action
                unsigned char ucAction = Packet.GetAction();

                if (pPed)
                {
                    if (pPed->IsPlayer())
                    {
                        // Make sure the source player is also the ped
                        CPlayer* pPlayerPed = static_cast<CPlayer*>(pPed);
                        if (pPlayerPed == pPlayer)
                        {
                            bValidPed = true;
                        }
                        switch (ucAction)
                        {
                            // Check if we are finishing a jacking sequence
                            case VEHICLE_NOTIFY_JACK:
                            case VEHICLE_NOTIFY_JACK_ABORT:
                            {
                                // Are we jacking a ped?
                                CPed* pJacked = pVehicle->GetOccupant(0);
                                if (pJacked && !pJacked->IsPlayer())
                                {
                                    // Check that all clients have a compatible bitstream
                                    for (auto iter = m_pPlayerManager->IterBegin(); iter != m_pPlayerManager->IterEnd(); iter++)
                                    {
                                        CPlayer* pSendPlayer = *iter;
                                        if (!pSendPlayer->CanBitStream(eBitStreamVersion::PedEnterExit))
                                        {
                                            if (pSendPlayer->IsJoined())
                                                // Store this player as incompatible for later
                                                // This happens because the player joined during a player jacking a ped
                                                sendListIncompatiblePlayers.push_back(pSendPlayer);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        // Make sure the source player is the syncer of the ped
                        if (pPed->GetSyncer() == pPlayer)
                        {
                            bValidPed = true;

                            // Check that all clients have a compatible bitstream when we have a ped enter/exit
                            for (auto iter = m_pPlayerManager->IterBegin(); iter != m_pPlayerManager->IterEnd(); iter++)
                            {
                                CPlayer* pSendPlayer = *iter;
                                if (!pSendPlayer->CanBitStream(eBitStreamVersion::PedEnterExit))
                                {
                                    switch (ucAction)
                                    {
                                        // Is he requesting to start enter/exit then reject it
                                        case VEHICLE_REQUEST_IN:
                                        case VEHICLE_REQUEST_OUT:
                                        case VEHICLE_NOTIFY_FELL_OFF:
                                        {
                                            bValidPed = false;
                                            break;
                                        }
                                        // Otherwise allow it to move on
                                        default:
                                        {
                                            if (pSendPlayer->IsJoined())
                                                // Store this player as incompatible for later
                                                // This happens because the player joined during a ped enter/exit
                                                sendListIncompatiblePlayers.push_back(pSendPlayer);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Check we have a valid ped
                if (bValidPed)
                {
                    // Handle it depending on the action
                    switch (ucAction)
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

                            // Is he spawned? (Fix for #2335)
                            if (!pPed->IsSpawned())
                            {
                                CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_ATTEMPT_FAILED);
                                pPlayer->Send(Reply);
                                break;
                            }

                            // Is this vehicle enterable? (not a trailer)
                            unsigned short usVehicleModel = pVehicle->GetModel();
                            if (!CVehicleManager::IsTrailer(usVehicleModel))
                            {
                                // He musn't already be doing something
                                if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_NONE)
                                {
                                    // He musn't already be in a vehicle
                                    if (!pPed->GetOccupiedVehicle())
                                    {
                                        float fCutoffDistance = 50.0f;
                                        bool  bWarpIn = false;
                                        // Jax: is he in water and trying to get in a floating vehicle
                                        // Cazomino05: changed to check if the vehicle is in water not player
                                        if ((pPed->IsInWater() || Packet.GetOnWater()) && (usVehicleModel == VT_SKIMMER || usVehicleModel == VT_SEASPAR ||
                                                                                           usVehicleModel == VT_LEVIATHN || usVehicleModel == VT_VORTEX))
                                        {
                                            fCutoffDistance = 10.0f;
                                            bWarpIn = true;
                                        }
                                        if (usVehicleModel == VT_RCBARON)
                                        {            // warp in for rc baron.
                                            fCutoffDistance = 10.0f;
                                            bWarpIn = true;
                                        }

                                        CPed* pOccupant = pVehicle->GetOccupant(0);
                                        // If he's going to be jacking this vehicle, lets make sure he's very close to it
                                        if (pOccupant)
                                            fCutoffDistance = 10.0f;

                                        // Is he close enough to the vehicle?
                                        if (IsPointNearPoint3D(pPed->GetPosition(), pVehicle->GetPosition(), fCutoffDistance))
                                        {
                                            unsigned char ucSeat = Packet.GetSeat();
                                            unsigned char ucDoor = Packet.GetDoor();

                                            // Temp fix: Disable driver seat for train carriages since the whole vehicle sync logic is based on the the
                                            // player on the first seat being the vehicle syncer (Todo)
                                            if (pVehicle->GetVehicleType() == VEHICLE_TRAIN && ucSeat == 0 && pVehicle->GetTowedByVehicle())
                                                ucSeat++;

                                            // Going for driver?
                                            if (ucSeat == 0)
                                            {
                                                // Does it already have an occupant/occupying?
                                                if (!pOccupant)
                                                {
                                                    // Mark him as entering the vehicle
                                                    pPed->SetOccupiedVehicle(pVehicle, 0);
                                                    pPed->SetVehicleAction(CPlayer::VEHICLEACTION_ENTERING);
                                                    pVehicle->m_bOccupantChanged = false;

                                                    // Call the entering vehicle event
                                                    CLuaArguments Arguments;
                                                    Arguments.PushElement(pPed);             // player / ped
                                                    Arguments.PushNumber(0);                 // seat
                                                    Arguments.PushBoolean(false);            // jacked
                                                    Arguments.PushNumber(ucDoor);            // Door
                                                    if (pVehicle->CallEvent("onVehicleStartEnter", Arguments))
                                                    {
                                                        // HACK?: check the ped's vehicle-action is still the same (not warped in?)
                                                        if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_ENTERING)
                                                        {
                                                            if (!m_pUnoccupiedVehicleSync->IsSyncerPersistent())
                                                            {
                                                                // Force the player (or ped syncer) as the syncer of the vehicle to which they are entering
                                                                m_pUnoccupiedVehicleSync->OverrideSyncer(pVehicle, pPlayer);
                                                            }

                                                            if (bWarpIn)
                                                            {
                                                                // Unmark him as entering the vehicle so WarpPedIntoVehicle will work
                                                                if (!pVehicle->m_bOccupantChanged)
                                                                {
                                                                    pPed->SetOccupiedVehicle(NULL, 0);
                                                                    pVehicle->SetOccupant(NULL, 0);
                                                                }

                                                                if (CStaticFunctionDefinitions::WarpPedIntoVehicle(pPed, pVehicle, 0))
                                                                {
                                                                    bFailed = false;
                                                                }
                                                                else
                                                                {
                                                                    // Warp failed
                                                                    pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);
                                                                    failReason = FAIL_SCRIPT;
                                                                }
                                                            }
                                                            else
                                                            {
                                                                // Tell everyone he can start entering the vehicle from his current position
                                                                CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_REQUEST_IN_CONFIRMED, ucDoor);
                                                                m_pPlayerManager->BroadcastOnlyJoined(Reply);
                                                                bFailed = false;
                                                            }
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (!pVehicle->m_bOccupantChanged)
                                                        {
                                                            pPed->SetOccupiedVehicle(NULL, 0);
                                                            pVehicle->SetOccupant(NULL, 0);
                                                        }
                                                        pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);
                                                        failReason = FAIL_SCRIPT;
                                                    }
                                                }
                                                else
                                                {
                                                    bool bValidOccupant = false;
                                                    // Is the jacked ped stationary in the car (ie not getting in or out)
                                                    if (IS_PED(pOccupant) && pOccupant->GetVehicleAction() == CPed::VEHICLEACTION_NONE)
                                                    {
                                                        bValidOccupant = true;
                                                        // Check if we are jacking a ped
                                                        if (!IS_PLAYER(pOccupant))
                                                        {
                                                            // Check that all clients have a compatible bitstream
                                                            for (auto iter = m_pPlayerManager->IterBegin(); iter != m_pPlayerManager->IterEnd(); iter++)
                                                            {
                                                                CPlayer* pSendPlayer = *iter;
                                                                if (!pSendPlayer->CanBitStream(eBitStreamVersion::PedEnterExit))
                                                                {
                                                                    bValidOccupant = false;
                                                                    break;
                                                                }
                                                            }
                                                        }
                                                    }

                                                    if (bValidOccupant)
                                                    {
                                                        // He's now jacking the car and the occupant is getting jacked
                                                        pPed->SetVehicleAction(CPed::VEHICLEACTION_JACKING);
                                                        pPed->SetJackingVehicle(pVehicle);
                                                        pVehicle->SetJackingPed(pPed);
                                                        pOccupant->SetVehicleAction(CPed::VEHICLEACTION_JACKED);

                                                        // Call the entering vehicle event
                                                        CLuaArguments EnterArguments;
                                                        EnterArguments.PushElement(pPed);                 // player / ped
                                                        EnterArguments.PushNumber(0);                     // seat
                                                        EnterArguments.PushElement(pOccupant);            // jacked
                                                        EnterArguments.PushNumber(ucDoor);                // Door
                                                        if (pVehicle->CallEvent("onVehicleStartEnter", EnterArguments))
                                                        {
                                                            // HACK?: check the peds vehicle-action is still the same (not warped in?)
                                                            if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_JACKING)
                                                            {
                                                                // Call the exiting vehicle event
                                                                CLuaArguments ExitArguments;
                                                                ExitArguments.PushElement(pOccupant);            // player / ped
                                                                ExitArguments.PushNumber(ucSeat);                // seat
                                                                ExitArguments.PushElement(pPed);                 // jacker
                                                                if (pVehicle->CallEvent("onVehicleStartExit", ExitArguments))
                                                                {
                                                                    // HACK?: check the player's vehicle-action is still the same (not warped out?)
                                                                    if (pOccupant->GetVehicleAction() == CPed::VEHICLEACTION_JACKED)
                                                                    {
                                                                        /* Jax: we don't need to worry about a syncer if we already have and will have a
                                                                        driver
                                                                        // Force the player as the syncer of the vehicle to which they are entering
                                                                        m_pUnoccupiedVehicleSync->OverrideSyncer ( pVehicle, pPlayer );
                                                                        */

                                                                        // Broadcast a jack message (tells him he can get in, but he must jack it)
                                                                        CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_REQUEST_JACK_CONFIRMED, ucDoor);
                                                                        m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                                                        bFailed = false;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else
                                                        {
                                                            pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);
                                                            pOccupant->SetVehicleAction(CPed::VEHICLEACTION_NONE);
                                                            failReason = FAIL_SCRIPT_2;
                                                        }
                                                    }
                                                    else
                                                        failReason = FAIL_JACKED_ACTION;
                                                }
                                            }
                                            else
                                            {
                                                CPed* pCurrentOccupant = pVehicle->GetOccupant(ucSeat);
                                                if (pCurrentOccupant || ucSeat > pVehicle->GetMaxPassengers())
                                                {
                                                    // Grab a free passenger spot in the vehicle
                                                    ucSeat = pVehicle->GetFreePassengerSeat();
                                                }
                                                if (ucSeat <= 8)
                                                {
                                                    // Mark him as entering the vehicle
                                                    pPed->SetOccupiedVehicle(pVehicle, ucSeat);
                                                    pPed->SetVehicleAction(CPed::VEHICLEACTION_ENTERING);
                                                    pVehicle->m_bOccupantChanged = false;

                                                    // Call the entering vehicle event
                                                    CLuaArguments Arguments;
                                                    Arguments.PushElement(pPed);             // player / ped
                                                    Arguments.PushNumber(ucSeat);            // seat
                                                    Arguments.PushBoolean(false);            // jacked
                                                    Arguments.PushNumber(ucDoor);            // Door
                                                    if (pVehicle->CallEvent("onVehicleStartEnter", Arguments))
                                                    {
                                                        // HACK?: check the player's vehicle-action is still the same (not warped in?)
                                                        if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_ENTERING)
                                                        {
                                                            if (bWarpIn)
                                                            {
                                                                // Unmark him as entering the vehicle so WarpPedIntoVehicle will work
                                                                if (!pVehicle->m_bOccupantChanged)
                                                                {
                                                                    pPlayer->SetOccupiedVehicle(NULL, 0);
                                                                    pVehicle->SetOccupant(NULL, ucSeat);
                                                                }

                                                                if (CStaticFunctionDefinitions::WarpPedIntoVehicle(pPed, pVehicle, ucSeat))
                                                                {
                                                                    bFailed = false;
                                                                }
                                                                else
                                                                {
                                                                    // Warp failed
                                                                    pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);
                                                                    failReason = FAIL_SCRIPT;
                                                                }
                                                            }
                                                            else
                                                            {
                                                                // Tell everyone he can start entering the vehicle from his current position
                                                                CVehicleInOutPacket Reply(PedID, VehicleID, ucSeat, VEHICLE_REQUEST_IN_CONFIRMED, ucDoor);
                                                                m_pPlayerManager->BroadcastOnlyJoined(Reply);
                                                                bFailed = false;
                                                            }
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (!pVehicle->m_bOccupantChanged)
                                                        {
                                                            pPed->SetOccupiedVehicle(NULL, 0);
                                                            pVehicle->SetOccupant(NULL, ucSeat);
                                                        }
                                                        pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);
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

                            if (bFailed)
                            {
                                // He can't get in
                                CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_ATTEMPT_FAILED);
                                Reply.SetFailReason((unsigned char)failReason);
                                // Was he too far away from the vehicle?
                                if (failReason == FAIL_DISTANCE)
                                {
                                    // Correct the vehicles position
                                    Reply.SetCorrectVector(pVehicle->GetPosition());
                                }
                                pPlayer->Send(Reply);
                            }

                            break;
                        }

                        // Vehicle in notification?
                        case VEHICLE_NOTIFY_IN:
                        {
                            // Is he entering?
                            if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_ENTERING)
                            {
                                // Is he the occupant? (he must unless the client has fucked up)
                                unsigned char ucOccupiedSeat = pPed->GetOccupiedVehicleSeat();
                                if (pPed == pVehicle->GetOccupant(ucOccupiedSeat))
                                {
                                    // Mark him as successfully entered
                                    pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);

                                    // Update our engine State
                                    pVehicle->SetEngineOn(true);

                                    // Tell everyone he's in (they should warp him in)
                                    CVehicleInOutPacket Reply(PedID, VehicleID, ucOccupiedSeat, VEHICLE_NOTIFY_IN_RETURN);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                    // Call the player->vehicle event
                                    CLuaArguments Arguments;
                                    Arguments.PushElement(pVehicle);                 // vehicle
                                    Arguments.PushNumber(ucOccupiedSeat);            // seat
                                    Arguments.PushBoolean(false);                    // jacked
                                    if (pPed->IsPlayer())
                                        pPed->CallEvent("onPlayerVehicleEnter", Arguments);
                                    else
                                        pPed->CallEvent("onPedVehicleEnter", Arguments);

                                    // Call the vehicle->player event
                                    CLuaArguments Arguments2;
                                    Arguments2.PushElement(pPed);                     // player / ped
                                    Arguments2.PushNumber(ucOccupiedSeat);            // seat
                                    Arguments2.PushBoolean(false);                    // jacked
                                    pVehicle->CallEvent("onVehicleEnter", Arguments2);
                                }
                            }

                            break;
                        }

                        // Vehicle in aborted notification?
                        case VEHICLE_NOTIFY_IN_ABORT:
                        {
                            // Is he entering?
                            if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_ENTERING)
                            {
                                // Is he the occupant? (he must unless the client has fucked up)
                                unsigned char ucOccupiedSeat = pPed->GetOccupiedVehicleSeat();
                                if (pPed == pVehicle->GetOccupant(ucOccupiedSeat))
                                {
                                    unsigned char ucDoor = Packet.GetDoor();
                                    float         fDoorAngle = Packet.GetDoorAngle();

                                    // Mark that he's in no vehicle
                                    pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);
                                    pPed->SetOccupiedVehicle(NULL, 0);
                                    pVehicle->SetOccupant(NULL, ucOccupiedSeat);

                                    // Update the door angle.
                                    pVehicle->SetDoorOpenRatio(ucDoor + 2, fDoorAngle);

                                    // Tell everyone he's out (they should warp him out)
                                    CVehicleInOutPacket Reply(PedID, VehicleID, ucOccupiedSeat, VEHICLE_NOTIFY_IN_ABORT_RETURN, ucDoor);
                                    Reply.SetDoorAngle(fDoorAngle);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);
                                    if (!sendListIncompatiblePlayers.empty())
                                    {
                                        CBitStream BitStream;
                                        BitStream.pBitStream->Write(pPed->GetSyncTimeContext());
                                        m_pPlayerManager->Broadcast(CElementRPCPacket(pPed, REMOVE_PED_FROM_VEHICLE, *BitStream.pBitStream),
                                                                    sendListIncompatiblePlayers);
                                    }
                                }
                            }

                            break;
                        }

                        // Vehicle get out request?
                        case VEHICLE_REQUEST_OUT:
                        {
                            // Is he getting jacked?
                            if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_NONE)
                            {
                                // Does it have an occupant and is the occupant the requesting ped?
                                unsigned ucOccupiedSeat = pPed->GetOccupiedVehicleSeat();
                                if (pPed == pVehicle->GetOccupant(ucOccupiedSeat))
                                {
                                    // Call the exiting vehicle event
                                    CLuaArguments Arguments;
                                    Arguments.PushElement(pPed);                       // player / ped
                                    Arguments.PushNumber(ucOccupiedSeat);              // seat
                                    Arguments.PushBoolean(false);                      // jacked
                                    Arguments.PushNumber(Packet.GetDoor());            // door being used
                                    if (pVehicle->CallEvent("onVehicleStartExit", Arguments) && pPed->GetOccupiedVehicle() == pVehicle)
                                    {
                                        // Mark him as exiting the vehicle
                                        pPed->SetVehicleAction(CPed::VEHICLEACTION_EXITING);

                                        // Tell everyone he can start exiting the vehicle
                                        CVehicleInOutPacket Reply(PedID, VehicleID, ucOccupiedSeat, VEHICLE_REQUEST_OUT_CONFIRMED, Packet.GetDoor());
                                        m_pPlayerManager->BroadcastOnlyJoined(Reply);
                                    }
                                    else
                                    {
                                        // Tell him he can't exit (script denied it or he was
                                        // already warped out)
                                        CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_ATTEMPT_FAILED);
                                        pPlayer->Send(Reply);
                                    }
                                }
                                else
                                {
                                    // Tell him he can't exit
                                    CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_ATTEMPT_FAILED);
                                    pPlayer->Send(Reply);
                                }
                            }
                            else
                            {
                                // Tell him he can't exit
                                CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_ATTEMPT_FAILED);
                                pPlayer->Send(Reply);
                            }

                            break;
                        }

                        // Vehicle out notify?
                        case VEHICLE_NOTIFY_OUT:
                        {
                            // Is he already getting out?
                            if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_EXITING)
                            {
                                // Does it have an occupant and is the occupant the requesting ped?
                                unsigned char ucOccupiedSeat = pPed->GetOccupiedVehicleSeat();
                                if (pPed == pVehicle->GetOccupant(ucOccupiedSeat))
                                {
                                    // Mark the ped/vehicle as empty
                                    pVehicle->SetOccupant(NULL, ucOccupiedSeat);
                                    pPed->SetOccupiedVehicle(NULL, 0);
                                    pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);

                                    if (!m_pUnoccupiedVehicleSync->IsSyncerPersistent())
                                    {
                                        // Force the player (or ped syncer) that just left the vehicle as the syncer
                                        m_pUnoccupiedVehicleSync->OverrideSyncer(pVehicle, pPlayer);
                                    }

                                    // Tell everyone he can start exiting the vehicle
                                    CVehicleInOutPacket Reply(PedID, VehicleID, ucOccupiedSeat, VEHICLE_NOTIFY_OUT_RETURN);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);
                                    if (!sendListIncompatiblePlayers.empty())
                                    {
                                        // Warp the ped out of the vehicle manually for incompatible players
                                        CBitStream BitStream;
                                        BitStream.pBitStream->Write(pPed->GetSyncTimeContext());
                                        m_pPlayerManager->Broadcast(CElementRPCPacket(pPed, REMOVE_PED_FROM_VEHICLE, *BitStream.pBitStream),
                                                                    sendListIncompatiblePlayers);
                                    }

                                    // Call the ped->vehicle event
                                    CLuaArguments Arguments;
                                    Arguments.PushElement(pVehicle);                 // vehicle
                                    Arguments.PushNumber(ucOccupiedSeat);            // seat
                                    Arguments.PushBoolean(false);                    // jacker
                                    Arguments.PushBoolean(false);                    // forcedByScript
                                    if (pPed->IsPlayer())
                                        pPed->CallEvent("onPlayerVehicleExit", Arguments);
                                    else
                                        pPed->CallEvent("onPedVehicleExit", Arguments);

                                    // Call the vehicle->player event
                                    CLuaArguments Arguments2;
                                    Arguments2.PushElement(pPed);                     // player / ped
                                    Arguments2.PushNumber(ucOccupiedSeat);            // seat
                                    Arguments2.PushBoolean(false);                    // jacker
                                    Arguments2.PushBoolean(false);                    // forcedByScript
                                    pVehicle->CallEvent("onVehicleExit", Arguments2);
                                }
                            }

                            break;
                        }

                        // Vehicle out aborted notification?
                        case VEHICLE_NOTIFY_OUT_ABORT:
                        {
                            // Is he exiting?
                            if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_EXITING)
                            {
                                // Is he the occupant?
                                unsigned char ucOccupiedSeat = pPed->GetOccupiedVehicleSeat();
                                if (pPed == pVehicle->GetOccupant(ucOccupiedSeat))
                                {
                                    // Mark that he's no longer exiting
                                    pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);

                                    // Tell everyone he's in (they should warp him in)
                                    CVehicleInOutPacket Reply(PedID, VehicleID, ucOccupiedSeat, VEHICLE_NOTIFY_OUT_ABORT_RETURN);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);
                                }
                            }

                            break;
                        }

                        case VEHICLE_NOTIFY_FELL_OFF:
                        {
                            // Check that the ped is in the given vehicle
                            unsigned char ucOccupiedSeat = pPed->GetOccupiedVehicleSeat();
                            if (pVehicle->GetOccupant(ucOccupiedSeat) == pPed)
                            {
                                // Remove him from the vehicle
                                pPed->SetOccupiedVehicle(NULL, 0);
                                pVehicle->SetOccupant(NULL, ucOccupiedSeat);

                                if (!m_pUnoccupiedVehicleSync->IsSyncerPersistent())
                                {
                                    // Force the player (or ped syncer) that just left the vehicle as the syncer
                                    m_pUnoccupiedVehicleSync->OverrideSyncer(pVehicle, pPlayer);
                                }

                                pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);
                                // Tell the other players about it
                                CVehicleInOutPacket Reply(PedID, VehicleID, ucOccupiedSeat, VEHICLE_NOTIFY_FELL_OFF_RETURN);
                                m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                // Call the ped->vehicle event
                                CLuaArguments Arguments;
                                Arguments.PushElement(pVehicle);                 // vehicle
                                Arguments.PushNumber(ucOccupiedSeat);            // seat
                                Arguments.PushBoolean(false);                    // jacker
                                Arguments.PushBoolean(false);                    // forcedByScript
                                if (pPed->IsPlayer())
                                {
                                    pPed->CallEvent("onPlayerVehicleExit", Arguments);
                                }
                                else
                                {
                                    pPed->CallEvent("onPedVehicleExit", Arguments);
                                }

                                // Call the vehicle->player event
                                CLuaArguments Arguments2;
                                Arguments2.PushElement(pPed);                     // player / ped
                                Arguments2.PushNumber(ucOccupiedSeat);            // seat
                                Arguments2.PushBoolean(false);                    // jacker
                                Arguments2.PushBoolean(false);                    // forcedByScript
                                pVehicle->CallEvent("onVehicleExit", Arguments2);
                            }

                            break;
                        }

                        case VEHICLE_NOTIFY_JACK:            // Finished jacking him
                        {
                            // Is the ped jacking?
                            if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_JACKING)
                            {
                                // Grab the jacked ped
                                CPed* pJacked = pVehicle->GetOccupant(0);
                                if (pJacked)
                                {
                                    // TODO! CHECK THE CAR ID
                                    // Throw the jacked ped out
                                    pJacked->SetOccupiedVehicle(NULL, 0);
                                    pJacked->SetVehicleAction(CPed::VEHICLEACTION_NONE);

                                    // Put the jacking ped into it
                                    pPed->SetOccupiedVehicle(pVehicle, 0);
                                    pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);
                                    pPed->SetJackingVehicle(NULL);
                                    pVehicle->SetJackingPed(NULL);

                                    // Tell everyone about it
                                    ElementID           JackedID = pJacked->GetID();
                                    CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_NOTIFY_JACK_RETURN, PedID, JackedID);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                    // Execute the ped->vehicle script function for the jacked ped
                                    CLuaArguments ArgumentsExit;
                                    ArgumentsExit.PushElement(pVehicle);            // vehicle
                                    ArgumentsExit.PushNumber(0);                    // seat
                                    ArgumentsExit.PushElement(pPed);                // jacker
                                    ArgumentsExit.PushBoolean(false);               // forcedByScript
                                    if (pJacked->IsPlayer())
                                        pJacked->CallEvent("onPlayerVehicleExit", ArgumentsExit);
                                    else
                                        pJacked->CallEvent("onPedVehicleExit", ArgumentsExit);

                                    // Execute the vehicle->ped script function for the jacked ped
                                    CLuaArguments ArgumentsExit2;
                                    ArgumentsExit2.PushElement(pJacked);            // player / ped
                                    ArgumentsExit2.PushNumber(0);                   // seat
                                    ArgumentsExit2.PushElement(pPed);               // jacker
                                    ArgumentsExit2.PushBoolean(false);              // forcedByScript
                                    pVehicle->CallEvent("onVehicleExit", ArgumentsExit2);

                                    // Execute the ped->vehicle script function
                                    CLuaArguments ArgumentsEnter;
                                    ArgumentsEnter.PushElement(pVehicle);            // vehicle
                                    ArgumentsEnter.PushNumber(0);                    // seat
                                    ArgumentsEnter.PushElement(pJacked);             // jacked
                                    if (pPed->IsPlayer())
                                        pPed->CallEvent("onPlayerVehicleEnter", ArgumentsEnter);
                                    else
                                        pPed->CallEvent("onPedVehicleEnter", ArgumentsEnter);

                                    // Execute the vehicle->ped script function
                                    CLuaArguments ArgumentsEnter2;
                                    ArgumentsEnter2.PushElement(pPed);               // player / ped
                                    ArgumentsEnter2.PushNumber(0);                   // seat
                                    ArgumentsEnter2.PushElement(pJacked);            // jacked
                                    pVehicle->CallEvent("onVehicleEnter", ArgumentsEnter2);
                                }
                                else
                                {
                                    // Put the jacking ped into it
                                    pPed->SetOccupiedVehicle(pVehicle, 0);
                                    pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);
                                    pPed->SetJackingVehicle(NULL);
                                    pVehicle->SetJackingPed(NULL);

                                    // Tell everyone about it
                                    CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_NOTIFY_IN_RETURN);
                                    m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                    // Execute the player->vehicle script function
                                    CLuaArguments Arguments;
                                    Arguments.PushElement(pVehicle);            // vehicle
                                    Arguments.PushNumber(0);                    // seat
                                    Arguments.PushBoolean(false);               // jacked
                                    if (pPed->IsPlayer())
                                        pPed->CallEvent("onPlayerVehicleEnter", Arguments);
                                    else
                                        pPed->CallEvent("onPedVehicleEnter", Arguments);

                                    // Execute the vehicle->player script function
                                    CLuaArguments Arguments2;
                                    Arguments2.PushElement(pPed);             // player / ped
                                    Arguments2.PushNumber(0);                 // seat
                                    Arguments2.PushBoolean(false);            // jacked
                                    pVehicle->CallEvent("onVehicleEnter", Arguments2);
                                }

                                if (!sendListIncompatiblePlayers.empty())
                                {
                                    // Warp the ped into the vehicle manually for incompatible players
                                    CBitStream BitStream;
                                    BitStream.pBitStream->Write(pVehicle->GetID());
                                    BitStream.pBitStream->Write((unsigned char)0);
                                    BitStream.pBitStream->Write(pPed->GetSyncTimeContext());
                                    m_pPlayerManager->Broadcast(CElementRPCPacket(pPed, WARP_PED_INTO_VEHICLE, *BitStream.pBitStream),
                                                                sendListIncompatiblePlayers);
                                }
                            }

                            break;
                        }

                        case VEHICLE_NOTIFY_JACK_ABORT:
                        {
                            // Is the sender jacking?
                            if (pPed->GetVehicleAction() == CPed::VEHICLEACTION_JACKING)
                            {
                                unsigned char ucDoor = Packet.GetDoor();
                                unsigned char ucOccupiedSeat = pPed->GetOccupiedVehicleSeat();
                                float         fAngle = Packet.GetDoorAngle();
                                CPed*         pJacked = pVehicle->GetOccupant(0);

                                // Mark that the jacker is in no vehicle
                                pPed->SetVehicleAction(CPed::VEHICLEACTION_NONE);
                                pPed->SetOccupiedVehicle(NULL, 0);
                                pPed->SetJackingVehicle(NULL);
                                pVehicle->SetJackingPed(NULL);

                                // Set the door angle.
                                pVehicle->SetDoorOpenRatio(ucDoor, fAngle);

                                // Tell everyone he aborted
                                CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_NOTIFY_IN_ABORT_RETURN, ucDoor);
                                Reply.SetDoorAngle(fAngle);
                                m_pPlayerManager->BroadcastOnlyJoined(Reply);

                                // The jacked is still inside?
                                if (pJacked)
                                {
                                    // Did he already start jacking him? (we're trusting the client here!)
                                    if (Packet.GetStartedJacking() == 1)
                                    {
                                        // Get the jacked out
                                        pJacked->SetOccupiedVehicle(NULL, 0);

                                        // No driver in this vehicle
                                        pVehicle->SetOccupant(NULL, 0);

                                        // Tell everyone to get the jacked person out
                                        CVehicleInOutPacket JackedReply(pJacked->GetID(), VehicleID, 0, VEHICLE_NOTIFY_OUT_RETURN);
                                        m_pPlayerManager->BroadcastOnlyJoined(JackedReply);

                                        CLuaArguments Arguments;
                                        Arguments.PushElement(pVehicle);                 // vehicle
                                        Arguments.PushNumber(ucOccupiedSeat);            // seat
                                        Arguments.PushElement(pPed);                     // jacker
                                        Arguments.PushBoolean(false);                    // forcedByScript

                                        if (pJacked->IsPlayer())
                                        {
                                            pJacked->CallEvent("onPlayerVehicleExit", Arguments);
                                        }
                                        else
                                        {
                                            pJacked->CallEvent("onPedVehicleExit", Arguments);
                                        }

                                        CLuaArguments Arguments2;
                                        Arguments2.PushElement(pJacked);                  // jacked
                                        Arguments2.PushNumber(ucOccupiedSeat);            // seat
                                        Arguments2.PushElement(pPed);                     // jacker
                                        Arguments2.PushBoolean(false);                    // forcedByScript

                                        pVehicle->CallEvent("onVehicleExit", Arguments2);

                                        if (!sendListIncompatiblePlayers.empty())
                                        {
                                            // Warp the ped out of the vehicle manually for incompatible players
                                            CBitStream BitStream;
                                            BitStream.pBitStream->Write(pJacked->GetSyncTimeContext());
                                            m_pPlayerManager->Broadcast(CElementRPCPacket(pJacked, REMOVE_PED_FROM_VEHICLE, *BitStream.pBitStream),
                                                                        sendListIncompatiblePlayers);
                                        }
                                    }
                                    pJacked->SetVehicleAction(CPed::VEHICLEACTION_NONE);
                                }
                            }

                            break;
                        }

                        default:
                        {
                            DisconnectConnectionDesync(this, *pPlayer, 2);
                            return;
                        }
                    }

                    return;
                }

                // Tell the client we failed
                CVehicleInOutPacket Reply(PedID, VehicleID, 0, VEHICLE_ATTEMPT_FAILED);
                pPlayer->Send(Reply);
            }
            else
            {
                return;
            }
        }
    }
}

void CGame::Packet_VehicleTrailer(CVehicleTrailerPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Spawned?
        if (pPlayer->IsSpawned())
        {
            // Grab the vehicle with the chosen ID
            ElementID ID = Packet.GetVehicle();
            ElementID TrailerID = Packet.GetAttachedVehicle();
            bool      bAttached = Packet.GetAttached();

            CElement* pVehicleElement = CElementIDs::GetElement(ID);
            if (pVehicleElement && IS_VEHICLE(pVehicleElement))
            {
                CVehicle* pVehicle = static_cast<CVehicle*>(pVehicleElement);

                pVehicleElement = CElementIDs::GetElement(TrailerID);
                if (pVehicleElement && IS_VEHICLE(pVehicleElement))
                {
                    CVehicle* pTrailer = static_cast<CVehicle*>(pVehicleElement);

                    // If we're attaching
                    if (bAttached)
                    {
                        // Do we already have a trailer?
                        CVehicle* pPresentTrailer = pVehicle->GetTowedVehicle();
                        if (pPresentTrailer)
                        {
                            pPresentTrailer->SetTowedByVehicle(NULL);
                            pVehicle->SetTowedVehicle(NULL);

                            // Detach this one
                            CVehicleTrailerPacket DetachPacket(pVehicle, pPresentTrailer, false);
                            DetachPacket.SetSourceElement(pPlayer);
                            m_pPlayerManager->BroadcastOnlyJoined(DetachPacket);
                        }

                        // Is our trailer already attached to something?
                        CVehicle* pPresentVehicle = pTrailer->GetTowedByVehicle();
                        if (pPresentVehicle)
                        {
                            pTrailer->SetTowedByVehicle(NULL);
                            pPresentVehicle->SetTowedVehicle(NULL);

                            // Detach from this one
                            CVehicleTrailerPacket DetachPacket(pPresentVehicle, pTrailer, false);
                            DetachPacket.SetSourceElement(pPlayer);
                            m_pPlayerManager->BroadcastOnlyJoined(DetachPacket);
                        }

                        // Attach them
                        pVehicle->SetTowedVehicle(pTrailer);
                        pTrailer->SetTowedByVehicle(pVehicle);

                        if (m_pUnoccupiedVehicleSync->IsSyncerPersistent())
                        {
                            // Make sure the un-occupied syncer of the trailer is this driver
                            m_pUnoccupiedVehicleSync->OverrideSyncer(pTrailer, pPlayer);
                        }

                        // Broadcast this packet to everyone else
                        m_pPlayerManager->BroadcastOnlyJoined(Packet, pPlayer);

                        // Execute the attach trailer script function
                        CLuaArguments Arguments;
                        Arguments.PushElement(pVehicle);
                        bool bContinue = pTrailer->CallEvent("onTrailerAttach", Arguments);

                        if (!bContinue)
                        {
                            // Detach them
                            CVehicleTrailerPacket DetachPacket(pVehicle, pTrailer, false);
                            DetachPacket.SetSourceElement(pPlayer);
                            m_pPlayerManager->BroadcastOnlyJoined(DetachPacket);
                        }
                    }
                    else            // If we're detaching
                    {
                        // Make sure they're attached
                        if (pVehicle->GetTowedVehicle() == pTrailer && pTrailer->GetTowedByVehicle() == pVehicle)
                        {
                            // Detach them
                            pVehicle->SetTowedVehicle(NULL);
                            pTrailer->SetTowedByVehicle(NULL);

                            // Tell everyone else to detach them
                            m_pPlayerManager->BroadcastOnlyJoined(Packet, pPlayer);

                            // Execute the detach trailer script function
                            CLuaArguments Arguments;
                            Arguments.PushElement(pVehicle);
                            pTrailer->CallEvent("onTrailerDetach", Arguments);
                        }
                    }
                }
            }
        }
    }
}

void CGame::Packet_Voice_Data(CVoiceDataPacket& Packet)
{
    unsigned short usDataLength = 0;

    if (m_pMainConfig->IsVoiceEnabled())            // Shouldn't really be receiving voice packets at all if voice is disabled
    {
        usDataLength = Packet.GetDataLength();

        if (usDataLength > 0)
        {
            CPlayer* pPlayer = Packet.GetSourcePlayer();

            if (pPlayer)
            {
                if (pPlayer->IsVoiceMuted())            // Shouldn't be receiving voice packets, player should be muted client side
                    return;

                // Is it the start of the voice stream?
                if (pPlayer->GetVoiceState() == VOICESTATE_IDLE)
                {
                    // Trigger the related event
                    CLuaArguments Arguments;
                    bool          bEventTriggered = pPlayer->CallEvent("onPlayerVoiceStart", Arguments, pPlayer);

                    if (!bEventTriggered)            // Was the event cancelled?
                    {
                        pPlayer->SetVoiceState(VOICESTATE_TRANSMITTING_IGNORED);
                        return;
                    }

                    // Our voice state has changed
                    pPlayer->SetVoiceState(VOICESTATE_TRANSMITTING);
                }

                if (pPlayer->GetVoiceState() ==
                    VOICESTATE_TRANSMITTING)            // If we reach here, and we're still in idle state, then the event was cancelled
                {
                    const unsigned char* pBuffer = Packet.GetData();
                    CVoiceDataPacket     VoicePacket(pPlayer, pBuffer, usDataLength);

                    // Make list of players to send the voice packet to
                    std::set<CPlayer*> playerSendMap;

                    list<CElement*>::const_iterator iter = pPlayer->IterBroadcastListBegin();
                    for (; iter != pPlayer->IterBroadcastListEnd(); iter++)
                    {
                        CElement* pBroadcastElement = *iter;
                        if (IS_TEAM(pBroadcastElement))
                        {
                            // Add team members
                            CTeam*                         pTeam = static_cast<CTeam*>(pBroadcastElement);
                            list<CPlayer*>::const_iterator iter = pTeam->PlayersBegin();
                            for (; iter != pTeam->PlayersEnd(); iter++)
                                playerSendMap.insert(*iter);
                        }
                        else if (IS_PLAYER(pBroadcastElement))
                        {
                            // Add a player
                            playerSendMap.insert(static_cast<CPlayer*>(pBroadcastElement));
                        }
                        else
                        {
                            // Add element decendants
                            std::vector<CPlayer*> descendantList;
                            pBroadcastElement->GetDescendantsByType(descendantList, CElement::PLAYER);
                            for (std::vector<CPlayer*>::const_iterator iter = descendantList.begin(); iter != descendantList.end(); ++iter)
                            {
                                playerSendMap.insert(*iter);
                            }
                        }
                    }

                    // Filter out ourselves and ignored
                    for (std::set<CPlayer*>::iterator iter = playerSendMap.begin(); iter != playerSendMap.end();)
                    {
                        if (*iter == pPlayer || (*iter)->IsPlayerIgnoringElement(pPlayer))
                            playerSendMap.erase(iter++);
                        else
                            ++iter;
                    }

                    // Send to all players in the send list
                    CPlayerManager::Broadcast(VoicePacket, playerSendMap);
                }
            }
        }
    }
}

void CGame::Packet_Voice_End(CVoiceEndPacket& Packet)
{
    if (m_pMainConfig->IsVoiceEnabled())            // Shouldn't really be receiving voice packets at all if voice is disabled
    {
        CPlayer* pPlayer = Packet.GetSourcePlayer();

        if (pPlayer)
        {
            CLuaArguments Arguments;
            pPlayer->CallEvent("onPlayerVoiceStop", Arguments, pPlayer);

            // Reset our voice state
            pPlayer->SetVoiceState(VOICESTATE_IDLE);

            CVoiceEndPacket EndPacket(pPlayer);

            // Make list of players to send the voice packet to
            std::set<CPlayer*> playerSendMap;

            list<CElement*>::const_iterator iter = pPlayer->IterBroadcastListBegin();
            for (; iter != pPlayer->IterBroadcastListEnd(); iter++)
            {
                CElement* pBroadcastElement = *iter;
                if (IS_TEAM(pBroadcastElement))
                {
                    // Add team members
                    CTeam*                         pTeam = static_cast<CTeam*>(pBroadcastElement);
                    list<CPlayer*>::const_iterator iter = pTeam->PlayersBegin();
                    for (; iter != pTeam->PlayersEnd(); iter++)
                        playerSendMap.insert(*iter);
                }
                else if (IS_PLAYER(pBroadcastElement))
                {
                    // Add a player
                    playerSendMap.insert(static_cast<CPlayer*>(pBroadcastElement));
                }
                else
                {
                    // Add element decendants
                    std::vector<CPlayer*> descendantList;
                    pBroadcastElement->GetDescendantsByType(descendantList, CElement::PLAYER);
                    for (std::vector<CPlayer*>::const_iterator iter = descendantList.begin(); iter != descendantList.end(); ++iter)
                    {
                        playerSendMap.insert(*iter);
                    }
                }
            }

            // Filter out ourselves and ignored
            for (std::set<CPlayer*>::iterator iter = playerSendMap.begin(); iter != playerSendMap.end();)
            {
                if (*iter == pPlayer || (*iter)->IsPlayerIgnoringElement(pPlayer))
                    playerSendMap.erase(iter++);
                else
                    ++iter;
            }

            // Send to all players in the send list
            CPlayerManager::Broadcast(EndPacket, playerSendMap);
        }
    }
}

void CGame::Packet_CameraSync(CCameraSyncPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        pPlayer->NotifyReceivedSync();

        CPlayerCamera* pCamera = pPlayer->GetCamera();

        if (Packet.m_bFixed)
        {
            pCamera->SetMode(CAMERAMODE_FIXED);
            pCamera->SetPosition(Packet.m_vecPosition);
            pCamera->SetLookAt(Packet.m_vecLookAt);
        }
        else
        {
            CElement* pTarget = GetElementFromId<CElement>(Packet.m_TargetID);
            if (!pTarget)
                pTarget = pPlayer;

            pCamera->SetMode(CAMERAMODE_PLAYER);
            pCamera->SetTarget(pTarget);
        }
    }
}

void CGame::Packet_PlayerTransgression(CPlayerTransgressionPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // If ac# not disabled on this server, do a kick
        if (!g_pGame->GetConfig()->IsDisableAC(SString("%d", Packet.m_uiLevel)))
        {
            CStaticFunctionDefinitions::KickPlayer(pPlayer, NULL, Packet.m_strMessage);
        }
    }
}

void CGame::Packet_PlayerDiagnostic(CPlayerDiagnosticPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        if (Packet.m_uiLevel == 236)
        {
            // Handle special info
            std::vector<SString> parts;
            Packet.m_strMessage.Split(",", parts);
            if (parts.size() > 3)
            {
                pPlayer->m_strDetectedAC = parts[0].Replace("|", ",");
                pPlayer->m_uiD3d9Size = atoi(parts[1]);
                pPlayer->m_strD3d9Md5 = parts[2];
                pPlayer->m_strD3d9Sha256 = parts[3];
            }
        }
        else if (Packet.m_uiLevel >= 1000 || g_pGame->GetConfig()->IsEnableDiagnostic(SString("%d", Packet.m_uiLevel)))
        {
            // If diagnosticis enabled on this server, log it
            SString strMessageCombo("DIAGNOSTIC: %s #%d %s\n", pPlayer->GetNick(), Packet.m_uiLevel, Packet.m_strMessage.c_str());
            CLogger::LogPrint(strMessageCombo);
        }
    }
}

void CGame::Packet_PlayerScreenShot(CPlayerScreenShotPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        if (Packet.m_ucStatus != EPlayerScreenShotResult::SUCCESS)
        {
            // disabled, minimized or error
            if (Packet.m_pResource)
            {
                CLuaArguments Arguments;
                Arguments.PushResource(Packet.m_pResource);
                Arguments.PushString(EnumToString((EPlayerScreenShotResultType)Packet.m_ucStatus));
                Arguments.PushBoolean(false);
                Arguments.PushNumber(static_cast<double>(Packet.m_llServerGrabTime));
                Arguments.PushString(Packet.m_strTag);
                Arguments.PushString(Packet.m_strError);
                pPlayer->CallEvent("onPlayerScreenShot", Arguments);
            }
        }
        else if (Packet.m_ucStatus == EPlayerScreenShotResult::SUCCESS)
        {
            // Get in-progress info
            SScreenShotInfo& info = pPlayer->GetScreenShotInfo();

            // Validate
            if (!info.bInProgress || info.usNextPartNumber != Packet.m_usPartNumber || info.usScreenShotId != Packet.m_usScreenShotId)
            {
                info.bInProgress = false;
                info.buffer.Clear();

                // Check if new start
                if (Packet.m_usPartNumber == 0)
                {
                    info.bInProgress = true;
                    info.usNextPartNumber = 0;
                    info.usScreenShotId = Packet.m_usScreenShotId;

                    info.llTimeStamp = Packet.m_llServerGrabTime;
                    info.uiTotalBytes = Packet.m_uiTotalBytes;
                    info.usTotalParts = Packet.m_usTotalParts;
                    info.usResourceNetId = Packet.m_pResource ? Packet.m_pResource->GetNetID() : INVALID_RESOURCE_NET_ID;
                    info.strTag = Packet.m_strTag;
                }
            }

            // Add data if valid
            if (info.bInProgress)
            {
                info.buffer += Packet.m_buffer;
                info.usNextPartNumber++;

                // Finished?
                if (info.usNextPartNumber == info.usTotalParts)
                {
                    CResource* pResource = g_pGame->GetResourceManager()->GetResourceFromNetID(info.usResourceNetId);
                    if (pResource && info.uiTotalBytes == info.buffer.GetSize())
                    {
                        CLuaArguments Arguments;
                        Arguments.PushResource(pResource);
                        Arguments.PushString("ok");
                        Arguments.PushString(std::string(info.buffer.GetData(), info.buffer.GetSize()));
                        Arguments.PushNumber(static_cast<double>(info.llTimeStamp));
                        Arguments.PushString(info.strTag);
                        pPlayer->CallEvent("onPlayerScreenShot", Arguments);
                    }

                    info.bInProgress = false;
                    info.buffer.Clear();
                }
            }
        }
    }
}

void CGame::Packet_PlayerNoSocket(CPlayerNoSocketPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // If we are getting 'no socket' warnings from the network layer, and sync has not been received for ages, assume some sort of problem and quit the
        // player
        if (pPlayer->GetTimeSinceReceivedSync() > 20000)
        {
            CLogger::LogPrintf("INFO: Dead connection detected for %s\n", pPlayer->GetNick());
            pPlayer->Send(CPlayerDisconnectedPacket(CPlayerDisconnectedPacket::KICK, "Worrying message"));
            g_pGame->QuitPlayer(*pPlayer, CClient::QUIT_TIMEOUT);
        }
    }
}

void CGame::Packet_PlayerNetworkStatus(CPlayerNetworkStatusPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        CLuaArguments Arguments;
        Arguments.PushNumber(Packet.m_ucType);             // 0-interruption began  1-interruption end
        Arguments.PushNumber(Packet.m_uiTicks);            // Ticks since interruption start
        pPlayer->CallEvent("onPlayerNetworkStatus", Arguments, NULL);
    }
}

void CGame::Packet_PlayerResourceStart(CPlayerResourceStartPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        CResource* pResource = Packet.GetResource();
        if (pResource)
        {
            CLuaArguments Arguments;
            Arguments.PushResource(pResource);
            pPlayer->CallEvent("onPlayerResourceStart", Arguments, NULL);
        }
    }
}

void CGame::Packet_PlayerModInfo(CPlayerModInfoPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        // Make itemList table
        CLuaArguments resultItemList;
        for (std::vector<SModInfoItem>::iterator iter = Packet.m_ModInfoItemList.begin(); iter != Packet.m_ModInfoItemList.end(); ++iter)
        {
            const SModInfoItem& in = *iter;

            // Make item table
            CLuaArguments resultItem;

            resultItem.PushString("id");
            resultItem.PushNumber(in.usId);

            resultItem.PushString("name");
            resultItem.PushString(in.strName);

            resultItem.PushString("hash");
            resultItem.PushNumber(in.uiHash);

            if (in.bHasSize)
            {
                resultItem.PushString("sizeX");
                resultItem.PushNumber(in.vecSize.fX - fmod((double)in.vecSize.fX, 0.01));

                resultItem.PushString("sizeY");
                resultItem.PushNumber(in.vecSize.fY - fmod((double)in.vecSize.fY, 0.01));

                resultItem.PushString("sizeZ");
                resultItem.PushNumber(in.vecSize.fZ - fmod((double)in.vecSize.fZ, 0.01));

                resultItem.PushString("originalSizeX");
                resultItem.PushNumber(in.vecOriginalSize.fX - fmod((double)in.vecOriginalSize.fX, 0.01));

                resultItem.PushString("originalSizeY");
                resultItem.PushNumber(in.vecOriginalSize.fY - fmod((double)in.vecOriginalSize.fY, 0.01));

                resultItem.PushString("originalSizeZ");
                resultItem.PushNumber(in.vecOriginalSize.fZ - fmod((double)in.vecOriginalSize.fZ, 0.01));
            }

            if (in.bHasHashInfo)
            {
                resultItem.PushString("length");
                resultItem.PushNumber(in.uiShortBytes);

                resultItem.PushString("md5");
                resultItem.PushString(in.strShortMd5);

                resultItem.PushString("sha256");
                resultItem.PushString(in.strShortSha256);

                resultItem.PushString("paddedLength");
                resultItem.PushNumber(in.uiLongBytes);

                resultItem.PushString("paddedMd5");
                resultItem.PushString(in.strLongMd5);

                resultItem.PushString("paddedSha256");
                resultItem.PushString(in.strLongSha256);
            }

            resultItemList.PushNumber(resultItemList.Count() / 2 + 1);
            resultItemList.PushTable(&resultItem);
        }

        CLuaArguments Arguments;
        Arguments.PushString(Packet.m_strInfoType);
        Arguments.PushTable(&resultItemList);
        pPlayer->CallEvent("onPlayerModInfo", Arguments);
    }
}

void CGame::Packet_PlayerACInfo(CPlayerACInfoPacket& Packet)
{
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer)
    {
        CLuaArguments acList;
        for (uint i = 0; i < Packet.m_IdList.size(); i++)
        {
            acList.PushNumber(i + 1);
            acList.PushNumber(Packet.m_IdList[i]);
        }

        CLuaArguments Arguments;
        Arguments.PushTable(&acList);
        Arguments.PushNumber(Packet.m_uiD3d9Size);
        Arguments.PushString(Packet.m_strD3d9MD5);
        Arguments.PushString(Packet.m_strD3d9SHA256);
        pPlayer->CallEvent("onPlayerACInfo", Arguments);
    }
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
            WEAPONTYPE_TEC9,           WEAPONTYPE_COUNTRYRIFLE,    WEAPONTYPE_SNIPERRIFLE};

        for (uint i = 0; i < NUMELMS(weaponList); i++)
            MapInsert(weaponTypesUsingBulletSync, weaponList[i]);
    }

    short sVehExtrapolateBaseMs = 5;
    short sVehExtrapolatePercent = m_pMainConfig->GetVehExtrapolatePercent();
    short sVehExtrapolateMaxMs = m_pMainConfig->GetVehExtrapolatePingLimit();
    uchar ucVehExtrapolateEnabled = sVehExtrapolatePercent != 0;
    uchar ucUseAltPulseOrder = m_pMainConfig->GetUseAltPulseOrder() != 0;
    uchar ucAllowFastSprintFix = true;
    uchar ucAllowDrivebyAnimFix = true;
    uchar ucAllowShotgunDamageFix = true;

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

    if (!g_pGame->IsWorldSpecialPropertyEnabled(WorldSpecialProperty::FIREBALLDESTRUCT))
    {
        if (strNewMin < FIREBALLDESTRUCT_MIN_CLIENT_VERSION)
            strNewMin = FIREBALLDESTRUCT_MIN_CLIENT_VERSION;
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

        if (!g_pGame->IsWorldSpecialPropertyEnabled(WorldSpecialProperty::FIREBALLDESTRUCT))
        {
            if (strKickMin < FIREBALLDESTRUCT_MIN_CLIENT_VERSION)
                strKickMin = FIREBALLDESTRUCT_MIN_CLIENT_VERSION;
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

void CGame::RegisterClientTriggeredEventUsage(CPlayer* pPlayer)
{
    if (!pPlayer || !pPlayer->IsPlayer() || pPlayer->IsBeingDeleted())
        return;

    auto now = GetTickCount64_();

    // If key/player doesn't exist in map, store time of entry
    if (m_mapClientTriggeredEvents.find(pPlayer) == m_mapClientTriggeredEvents.end())
        m_mapClientTriggeredEvents[pPlayer].m_llTicks = now;

    // Only increment if we haven't reached the interval time already
    if (now - m_mapClientTriggeredEvents[pPlayer].m_llTicks <= m_iClientTriggeredEventsIntervalMs)
        m_mapClientTriggeredEvents[pPlayer].m_uiCounter++;
}

void CGame::ProcessClientTriggeredEventSpam()
{
    for (auto it = m_mapClientTriggeredEvents.begin(); it != m_mapClientTriggeredEvents.end();)
    {
        const auto& [player, data] = *it;
        bool remove = false;

        if (player && player->IsPlayer() && !player->IsBeingDeleted())
        {
            if (GetTickCount64_() - data.m_llTicks >= m_iClientTriggeredEventsIntervalMs)
            {
                if (data.m_uiCounter > m_iMaxClientTriggeredEventsPerInterval)
                    player->CallEvent("onPlayerTriggerEventThreshold", {});

                remove = true;
            }
        }
        else
        {
            remove = true;
        }

        if (remove)
            it = m_mapClientTriggeredEvents.erase(it);
        else
            it++;
    }
}
