/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientGame.cpp
 *  PURPOSE:     Client game manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <net/SyncStructures.h>
#include <game/C3DMarkers.h>
#include <game/CAnimBlendAssocGroup.h>
#include <game/CAnimBlendAssociation.h>
#include <game/CAnimBlendHierarchy.h>
#include <game/CAnimManager.h>
#include <game/CColPoint.h>
#include <game/CEventDamage.h>
#include <game/CExplosionManager.h>
#include <game/CFire.h>
#include <game/CGarage.h>
#include <game/CGarages.h>
#include <game/CPedIntelligence.h>
#include <game/CPlayerInfo.h>
#include <game/CSettings.h>
#include <game/CStreaming.h>
#include <game/CTaskManager.h>
#include <game/CWanted.h>
#include <game/CWeapon.h>
#include <game/CWeaponStatManager.h>
#include <game/CWeather.h>
#include <game/Task.h>
#include <game/CBuildingRemoval.h>
#include "game/CClock.h"
#include <windowsx.h>
#include "CServerInfo.h"

SString StringZeroPadout(const SString& strInput, uint uiPadoutSize)
{
    SString strResult = strInput;
    while (strResult.length() < uiPadoutSize)
        strResult += '\0';
    return strResult;
}

using SharedUtil::CalcMTASAPath;
using std::list;
using std::vector;

// Hide the "conversion from 'unsigned long' to 'DWORD*' of greater size" warning
#pragma warning(disable : 4312)

// Used within this file by the packet handler to grab the this pointer of CClientGame
extern CClientGame* g_pClientGame;
extern int          g_iDamageEventLimit;
float               g_fApplyDamageLastAmount;
uchar               g_ucApplyDamageLastHitZone;
CClientPed*         g_pApplyDamageLastDamagedPed;
bool                g_bBulletFireVectorsValid;
CVector             g_vecBulletFireStartPosition;
CVector             g_vecBulletFireEndPosition;

#define DEFAULT_GRAVITY              0.008f
#define DEFAULT_GAME_SPEED           1.0f
#define DEFAULT_JETPACK_MAXHEIGHT    100
#define DEFAULT_AIRCRAFT_MAXHEIGHT   800
#define DEFAULT_AIRCRAFT_MAXVELOCITY 1.5f
#define DEFAULT_MINUTE_DURATION      1000
#define DOUBLECLICK_TIMEOUT          330
#define DOUBLECLICK_MOVE_THRESHOLD   10.0f

static constexpr long long TIME_DISCORD_UPDATE_RATE = 15000;

CClientGame::CClientGame(bool bLocalPlay) : m_ServerInfo(new CServerInfo())
{
    // Init the global var with ourself
    g_pClientGame = this;

    // Packet handler
    m_pPacketHandler = new CPacketHandler();

    // Init
    m_bLocalPlay = bLocalPlay;
    m_bErrorStartingLocal = false;
    m_iLocalConnectAttempts = 0;
    m_Status = CClientGame::STATUS_CONNECTING;
    m_ulVerifyTimeStart = 0;
    m_ulLastClickTick = 0;
    m_pLocalPlayer = NULL;
    m_LocalID = INVALID_ELEMENT_ID;
    m_bShowNametags = true;
    m_bWaitingForLocalConnect = false;
    m_bShowRadar = false;
    m_bGameLoaded = false;
    m_bTriggeredIngameAndConnected = false;
    m_bGracefulDisconnect = false;
    m_pTargetedEntity = NULL;
    m_TargetedPlayerID = INVALID_ELEMENT_ID;
    m_pDamageEntity = NULL;
    m_DamagerID = INVALID_ELEMENT_ID;
    m_ucDamageBodyPiece = 0xFF;
    m_ucDamageWeapon = 0xFF;
    m_ulDamageTime = 0;
    m_bDamageSent = true;
    m_bShowNetstat = false;
    m_bShowFPS = false;
    m_bHudAreaNameDisabled = false;
    m_fGameSpeed = 1.0f;
    m_lMoney = 0;
    m_dwWanted = 0;
    m_timeLastDiscordStateUpdate = 0;
    m_lastWeaponSlot = WEAPONSLOT_MAX;            // last stored weapon slot, for weapon slot syncing to server (sets to invalid value)
    ResetAmmoInClip();

    m_bFocused = g_pCore->IsFocused();

    m_bCursorEventsEnabled = false;
    m_bInitiallyFadedOut = true;

    m_bIsPlayingBack = false;
    m_bFirstPlaybackFrame = false;

    // Setup game glitch defaults ( false = disabled ).  Remember to update these serverside if you alter them!
    m_Glitches[GLITCH_QUICKRELOAD] = false;
    g_pMultiplayer->DisableQuickReload(true);
    m_Glitches[GLITCH_FASTFIRE] = false;
    m_Glitches[GLITCH_FASTMOVE] = false;
    m_Glitches[GLITCH_CROUCHBUG] = false;
    m_Glitches[GLITCH_CLOSEDAMAGE] = false;
    g_pMultiplayer->DisableCloseRangeDamage(true);
    m_Glitches[GLITCH_HITANIM] = false;
    m_Glitches[GLITCH_FASTSPRINT] = false;
    m_Glitches[GLITCH_BADDRIVEBYHITBOX] = false;
    m_Glitches[GLITCH_QUICKSTAND] = false;
    m_Glitches[GLITCH_KICKOUTOFVEHICLE_ONMODELREPLACE] = false;
    g_pMultiplayer->DisableBadDrivebyHitboxes(true);

    // Remove Night & Thermal vision view (if enabled).
    g_pMultiplayer->SetNightVisionEnabled(false, true);
    g_pMultiplayer->SetThermalVisionEnabled(false, true);

    m_bCloudsEnabled = true;

    m_bBirdsEnabled = true;

    m_bWasMinimized = false;

    // Grab the mod path
    m_strModRoot = g_pCore->GetModInstallRoot("deathmatch");

    // Figure out which directory to use for the client resource file cache
    SetFileCacheRoot();

    // Override CGUI's global events
    g_pCore->GetGUI()->SetKeyDownHandler(INPUT_MOD, GUI_CALLBACK_KEY(&CClientGame::OnKeyDown, this));
    g_pCore->GetGUI()->SetMouseClickHandler(INPUT_MOD, GUI_CALLBACK_MOUSE(&CClientGame::OnMouseClick, this));
    g_pCore->GetGUI()->SetMouseDoubleClickHandler(INPUT_MOD, GUI_CALLBACK_MOUSE(&CClientGame::OnMouseDoubleClick, this));
    g_pCore->GetGUI()->SetMouseButtonDownHandler(INPUT_MOD, GUI_CALLBACK_MOUSE(&CClientGame::OnMouseButtonDown, this));
    g_pCore->GetGUI()->SetMouseButtonUpHandler(INPUT_MOD, GUI_CALLBACK_MOUSE(&CClientGame::OnMouseButtonUp, this));
    g_pCore->GetGUI()->SetMouseMoveHandler(INPUT_MOD, GUI_CALLBACK_MOUSE(&CClientGame::OnMouseMove, this));
    g_pCore->GetGUI()->SetMouseEnterHandler(INPUT_MOD, GUI_CALLBACK_MOUSE(&CClientGame::OnMouseEnter, this));
    g_pCore->GetGUI()->SetMouseLeaveHandler(INPUT_MOD, GUI_CALLBACK_MOUSE(&CClientGame::OnMouseLeave, this));
    g_pCore->GetGUI()->SetMouseWheelHandler(INPUT_MOD, GUI_CALLBACK_MOUSE(&CClientGame::OnMouseWheel, this));
    g_pCore->GetGUI()->SetMovedHandler(INPUT_MOD, GUI_CALLBACK(&CClientGame::OnMove, this));
    g_pCore->GetGUI()->SetSizedHandler(INPUT_MOD, GUI_CALLBACK(&CClientGame::OnSize, this));
    g_pCore->GetGUI()->SetFocusGainedHandler(INPUT_MOD, GUI_CALLBACK_FOCUS(&CClientGame::OnFocusGain, this));
    g_pCore->GetGUI()->SetFocusLostHandler(INPUT_MOD, GUI_CALLBACK_FOCUS(&CClientGame::OnFocusLoss, this));
    g_pCore->GetGUI()->SelectInputHandlers(INPUT_MOD);

    // Startup "entities from root" optimization for getElementsByType
    CClientEntity::StartupEntitiesFromRoot();

    // Startup game entity tracking manager
    m_pModelCacheManager = NewClientModelCacheManager();

    // Initialize our root entity with an invalid id, we dont know the true id until map-start
    m_pRootEntity = new CClientDummy(NULL, INVALID_ELEMENT_ID, "root");
    m_pRootEntity->MakeSystemEntity();

    m_pDebugHookManager = new CDebugHookManager();

    // Movings objects manager
    m_pMovingObjectsManager = new CMovingObjectsManager();

    // Create the manager and grab the most important pointers
    m_pManager = new CClientManager;
    m_pCamera = m_pManager->GetCamera();
    m_pMarkerManager = m_pManager->GetMarkerManager();
    m_pObjectManager = m_pManager->GetObjectManager();
    m_pPickupManager = m_pManager->GetPickupManager();
    m_pPlayerManager = m_pManager->GetPlayerManager();
    m_pRadarAreaManager = m_pManager->GetRadarAreaManager();
    m_pDisplayManager = m_pManager->GetDisplayManager();
    m_pVehicleManager = m_pManager->GetVehicleManager();
    m_pRadarMarkerManager = m_pManager->GetRadarMarkerManager();
    m_pPathManager = m_pManager->GetPathManager();
    m_pTeamManager = m_pManager->GetTeamManager();
    m_pPedManager = m_pManager->GetPedManager();
    m_pGUIManager = m_pManager->GetGUIManager();
    m_pResourceManager = m_pManager->GetResourceManager();
    m_pProjectileManager = m_pManager->GetProjectileManager();
    m_pLocalServer = NULL;

    m_pLatentTransferManager = new CLatentTransferManager();
    m_pZoneNames = new CZoneNames;
    m_pScriptKeyBinds = new CScriptKeyBinds;
    m_pRemoteCalls = new CRemoteCalls();
    m_pResourceFileDownloadManager = new CResourceFileDownloadManager();

    // Create our net API
    m_pNetAPI = new CNetAPI(m_pManager);
    m_pNetworkStats = new CNetworkStats(m_pDisplayManager);
    m_pSyncDebug = new CSyncDebug(m_pManager);

    // Create our blended weather class
    m_pBlendedWeather = new CBlendedWeather;

    // Create our RPC class
    m_pRPCFunctions = new CRPCFunctions(this);

    // Our management classes
    m_pUnoccupiedVehicleSync = new CUnoccupiedVehicleSync(m_pVehicleManager);
    m_pPedSync = new CPedSync(m_pPedManager);
#ifdef WITH_OBJECT_SYNC
    m_pObjectSync = new CObjectSync(m_pObjectManager);
#endif
    m_pNametags = new CNametags(m_pManager);
    m_pRadarMap = new CRadarMap(m_pManager);

    // Set the screenshot path
    /* This is now done in CCore, to maintain a global screenshot path
    SString strScreenShotPath = SString::Printf ( "%s\\screenshots", m_szModRoot );
    g_pCore->SetScreenShotPath ( strScreenShotPath );
    */

    // Create the transfer boxes (GUI)
    m_pTransferBox = new CTransferBox(TransferBoxType::RESOURCE_DOWNLOAD);
    m_pBigPacketTransferBox = new CTransferBox(TransferBoxType::MAP_DOWNLOAD);

    // Store the time we started on
    if (bLocalPlay)
        m_ulTimeStart = 0;
    else
        m_ulTimeStart = CClientTime::GetTime();

    // MTA Voice
    m_pVoiceRecorder = new CVoiceRecorder();

    // Singular file download manager
    m_pSingularFileDownloadManager = new CSingularFileDownloadManager();

    // 3D model renderer
    m_pModelRenderer = std::make_unique<CModelRenderer>();

    // Register the message and the net packet handler
    g_pMultiplayer->SetPreWeaponFireHandler(CClientGame::PreWeaponFire);
    g_pMultiplayer->SetPostWeaponFireHandler(CClientGame::PostWeaponFire);
    g_pMultiplayer->SetBulletImpactHandler(CClientGame::BulletImpact);
    g_pMultiplayer->SetBulletFireHandler(CClientGame::BulletFire);
    g_pMultiplayer->SetExplosionHandler(CClientExplosionManager::Hook_StaticExplosionCreation);
    g_pMultiplayer->SetBreakTowLinkHandler(CClientGame::StaticBreakTowLinkHandler);
    g_pMultiplayer->SetDrawRadarAreasHandler(CClientGame::StaticDrawRadarAreasHandler);
    g_pMultiplayer->SetDamageHandler(CClientGame::StaticDamageHandler);
    g_pMultiplayer->SetDeathHandler(CClientGame::StaticDeathHandler);
    g_pMultiplayer->SetFireHandler(CClientGame::StaticFireHandler);
    g_pMultiplayer->SetProjectileStopHandler(CClientProjectileManager::Hook_StaticProjectileAllow);
    g_pMultiplayer->SetProjectileHandler(CClientProjectileManager::Hook_StaticProjectileCreation);
    g_pMultiplayer->SetRender3DStuffHandler(CClientGame::StaticRender3DStuffHandler);
    g_pMultiplayer->SetPreRenderSkyHandler(CClientGame::StaticPreRenderSkyHandler);
    g_pMultiplayer->SetRenderHeliLightHandler(CClientGame::StaticRenderHeliLightHandler);
    g_pMultiplayer->SetRenderEverythingBarRoadsHandler(CClientGame::StaticRenderEverythingBarRoadsHandler);
    g_pMultiplayer->SetChokingHandler(CClientGame::StaticChokingHandler);
    g_pMultiplayer->SetPreWorldProcessHandler(CClientGame::StaticPreWorldProcessHandler);
    g_pMultiplayer->SetPostWorldProcessHandler(CClientGame::StaticPostWorldProcessHandler);
    g_pMultiplayer->SetPostWorldProcessPedsAfterPreRenderHandler(CClientGame::StaticPostWorldProcessPedsAfterPreRenderHandler);
    g_pMultiplayer->SetPreFxRenderHandler(CClientGame::StaticPreFxRenderHandler);
    g_pMultiplayer->SetPostColorFilterRenderHandler(CClientGame::StaticPostColorFilterRenderHandler);
    g_pMultiplayer->SetPreHudRenderHandler(CClientGame::StaticPreHudRenderHandler);
    g_pMultiplayer->DisableCallsToCAnimBlendNode(false);
    g_pMultiplayer->SetCAnimBlendAssocDestructorHandler(CClientGame::StaticCAnimBlendAssocDestructorHandler);
    g_pMultiplayer->SetAddAnimationHandler(CClientGame::StaticAddAnimationHandler);
    g_pMultiplayer->SetAddAnimationAndSyncHandler(CClientGame::StaticAddAnimationAndSyncHandler);
    g_pMultiplayer->SetAssocGroupCopyAnimationHandler(CClientGame::StaticAssocGroupCopyAnimationHandler);
    g_pMultiplayer->SetBlendAnimationHierarchyHandler(CClientGame::StaticBlendAnimationHierarchyHandler);
    g_pMultiplayer->SetProcessCollisionHandler(CClientGame::StaticProcessCollisionHandler);
    g_pMultiplayer->SetVehicleCollisionHandler(CClientGame::StaticVehicleCollisionHandler);
    g_pMultiplayer->SetVehicleDamageHandler(CClientGame::StaticVehicleDamageHandler);
    g_pMultiplayer->SetHeliKillHandler(CClientGame::StaticHeliKillHandler);
    g_pMultiplayer->SetObjectDamageHandler(CClientGame::StaticObjectDamageHandler);
    g_pMultiplayer->SetObjectBreakHandler(CClientGame::StaticObjectBreakHandler);
    g_pMultiplayer->SetWaterCannonHitHandler(CClientGame::StaticWaterCannonHandler);
    g_pMultiplayer->SetVehicleFellThroughMapHandler(CClientGame::StaticVehicleFellThroughMapHandler);
    g_pMultiplayer->SetGameObjectDestructHandler(CClientGame::StaticGameObjectDestructHandler);
    g_pMultiplayer->SetGameVehicleDestructHandler(CClientGame::StaticGameVehicleDestructHandler);
    g_pMultiplayer->SetGamePlayerDestructHandler(CClientGame::StaticGamePlayerDestructHandler);
    g_pMultiplayer->SetGameProjectileDestructHandler(CClientGame::StaticGameProjectileDestructHandler);
    g_pMultiplayer->SetGameModelRemoveHandler(CClientGame::StaticGameModelRemoveHandler);
    g_pMultiplayer->SetGameRunNamedAnimDestructorHandler(CClientGame::StaticGameRunNamedAnimDestructorHandler);
    g_pMultiplayer->SetGameEntityRenderHandler(CClientGame::StaticGameEntityRenderHandler);
    g_pMultiplayer->SetFxSystemDestructionHandler(CClientGame::StaticFxSystemDestructionHandler);
    g_pMultiplayer->SetDrivebyAnimationHandler(CClientGame::StaticDrivebyAnimationHandler);
    g_pMultiplayer->SetPedStepHandler(CClientGame::StaticPedStepHandler);
    g_pMultiplayer->SetVehicleWeaponHitHandler(CClientGame::StaticVehicleWeaponHitHandler);
    g_pMultiplayer->SetAudioZoneRadioSwitchHandler(CClientGame::StaticAudioZoneRadioSwitchHandler);
    g_pGame->SetPreWeaponFireHandler(CClientGame::PreWeaponFire);
    g_pGame->SetPostWeaponFireHandler(CClientGame::PostWeaponFire);
    g_pGame->SetTaskSimpleBeHitHandler(CClientGame::StaticTaskSimpleBeHitHandler);
    g_pGame->GetAudioEngine()->SetWorldSoundHandler(CClientGame::StaticWorldSoundHandler);
    g_pCore->SetMessageProcessor(CClientGame::StaticProcessMessage);
    g_pCore->GetKeyBinds()->SetKeyStrokeHandler(CClientGame::StaticKeyStrokeHandler);
    g_pCore->GetKeyBinds()->SetCharacterKeyHandler(CClientGame::StaticCharacterKeyHandler);
    g_pNet->RegisterPacketHandler(CClientGame::StaticProcessPacket);

    m_pLuaManager = new CLuaManager(this);
    m_pScriptDebugging = new CScriptDebugging(m_pLuaManager);
    m_pScriptDebugging->SetLogfile(CalcMTASAPath("mta\\logs\\clientscript.log"), 3);

    CStaticFunctionDefinitions(m_pLuaManager, &m_Events, g_pCore, g_pGame, this, m_pManager);
    CLuaFunctionDefs::Initialize(m_pLuaManager, m_pScriptDebugging, this);
    CLuaDefs::Initialize(this, m_pLuaManager, m_pScriptDebugging);

    // Start async task scheduler
    m_pAsyncTaskScheduler = new SharedUtil::CAsyncTaskScheduler(2);

    // Disable the enter/exit vehicle key button (we want to handle this button ourselves)
    g_pMultiplayer->DisableEnterExitVehicleKey(true);

    // Disable GTA's pickup processing as we want to confirm the hits with the server
    m_pPickupManager->SetPickupProcessingDisabled(true);

    // Key-bind for fire-key (for handling satchels and stealth-kills)
    g_pCore->GetKeyBinds()->AddControlFunction("fire", CClientGame::StaticUpdateFireKey, true);

    // Init big packet progress vars
    m_bReceivingBigPacket = false;
    m_ulBigPacketSize = 0;
    m_ulBigPacketBytesReceivedBase = 0;

    m_bBeingDeleted = false;

#if defined(MTA_DEBUG) || defined(MTA_BETA)
    m_bShowSyncingInfo = false;
#endif

#ifdef MTA_DEBUG
    m_pShowPlayer = m_pShowPlayerTasks = NULL;
    m_bMimicLag = false;
    m_ulLastMimicLag = 0;
    m_bDoPaintballs = false;
    m_bShowInterpolation = false;
#endif

    // Add our lua events
    AddBuiltInEvents();

    // Load some stuff from the core config
    float fScale;
    g_pCore->GetCVars()->Get("text_scale", fScale);
    CClientTextDisplay::SetGlobalScale(fScale);

    // Reset async loading script settings to default
    g_pGame->SetAsyncLoadingFromScript(true, false);

    // Reset test mode script settings to default
    g_pCore->GetGraphics()->GetRenderItemManager()->SetTestMode(DX_TEST_MODE_NONE);

    // Setup builtin Lua events
    SetupGlobalLuaEvents();

    // Setup default states for Rich Presence
    g_vehicleTypePrefixes = {
        _("Flying a UFO around"),      _("Cruising around"),            _("Riding the waves of"),
        _("Riding the train in"),      _("Flying around"),              _("Flying around"),
        _("Riding around"),            _("Monster truckin' around"),    _("Quaddin' around"),
        _("Bunny hopping around"),     _("Doing weird stuff in")
    };

    g_playerTaskStates = {
        {TASK_COMPLEX_JUMP, {true, _("Climbing around in"), TASK_SIMPLE_CLIMB}},
        {TASK_SIMPLE_GANG_DRIVEBY, {true, _("Doing a drive-by in")}},
        {TASK_SIMPLE_DRIVEBY_SHOOT, {true, _("Doing a drive-by in")}},
        {TASK_SIMPLE_DIE, {false, _("Blub blub..."), TASK_SIMPLE_DROWN}},
        {TASK_SIMPLE_DIE, {false, _("Breathing water"), TASK_SIMPLE_DROWN}},
        {TASK_SIMPLE_DIE, {true, _("Drowning in"), TASK_SIMPLE_DROWN}},
        {TASK_SIMPLE_PLAYER_ON_FOOT, {true, _("Ducking for cover in"), {}, TASK_SIMPLE_DUCK, TASK_SECONDARY_DUCK}},
        {TASK_SIMPLE_PLAYER_ON_FOOT, {true, _("Fighting in"), {}, TASK_SIMPLE_FIGHT, TASK_SECONDARY_ATTACK}},
        {TASK_SIMPLE_PLAYER_ON_FOOT, {true, _("Throwing fists in"), {}, TASK_SIMPLE_FIGHT, TASK_SECONDARY_ATTACK}},
        {TASK_SIMPLE_PLAYER_ON_FOOT, {true, _("Blastin' fools in"), {}, TASK_SIMPLE_USE_GUN, TASK_SECONDARY_ATTACK}},
        {TASK_SIMPLE_PLAYER_ON_FOOT, {true, _("Shooting up"), {}, TASK_SIMPLE_USE_GUN, TASK_SECONDARY_ATTACK}},
        {TASK_SIMPLE_JETPACK, {true, _("Jetpacking in")}},
        {TASK_SIMPLE_PLAYER_ON_FOOT, {true, _("Literally on fire in"), {}, TASK_SIMPLE_PLAYER_ON_FIRE, TASK_SECONDARY_PARTIAL_ANIM}},
        {TASK_SIMPLE_PLAYER_ON_FOOT, {true, _("Burning up in"), {}, TASK_SIMPLE_PLAYER_ON_FIRE, TASK_SECONDARY_PARTIAL_ANIM}},
        {TASK_COMPLEX_IN_WATER, {true, _("Swimming in"), TASK_SIMPLE_SWIM}},
        {TASK_COMPLEX_IN_WATER, {true, _("Floating around in"), TASK_SIMPLE_SWIM}},
        {TASK_COMPLEX_IN_WATER, {false, _("Being chased by a shark"), TASK_SIMPLE_SWIM}},
        {TASK_SIMPLE_CHOKING, {true, _("Choking to death in")}},
    };
}

CClientGame::~CClientGame()
{
    m_bBeingDeleted = true;
    // Stop all explosions. Unfortunately this doesn't fix the crash
    // if a vehicle is destroyed while it explodes.
    g_pGame->GetExplosionManager()->RemoveAllExplosions();

    // Reset custom streaming memory size [possibly] set by the server...
    g_pCore->SetCustomStreamingMemory(0);

    // ...and restore the buffer size too
    g_pGame->GetStreaming()->SetStreamingBufferSize(g_pClientGame->GetManager()->GetIMGManager()->GetLargestFileSizeBlocks());

    // Reset camera shaking
    g_pGame->GetCamera()->SetShakeForce(0.0f);

    // Stop playing the continuous sounds
    // if the game was loaded. This is done by
    // playing these special IDS.
    if (m_bGameLoaded)
    {
        g_pGame->GetAudioEngine()->PlayFrontEndSound(35);
        g_pGame->GetAudioEngine()->PlayFrontEndSound(48);
    }

    // Reset the GUI input mode
    g_pCore->GetGUI()->SetGUIInputMode(INPUTMODE_NO_BINDS_ON_EDIT);

    // Reset CGUI's global events
    g_pCore->GetGUI()->ClearInputHandlers(INPUT_MOD);

// Destroy mimics
#ifdef MTA_DEBUG
    list<CClientPlayer*>::const_iterator iterMimics = m_Mimics.begin();
    for (; iterMimics != m_Mimics.end(); iterMimics++)
    {
        CClientPlayer*  pPlayer = *iterMimics;
        CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle();
        if (pVehicle)
            delete pVehicle;

        delete pPlayer;
    }
#endif

    // Hide the transfer box incase it is showing
    m_pTransferBox->Hide();
    m_pBigPacketTransferBox->Hide();

    // Stop async task scheduler
    SAFE_DELETE(m_pAsyncTaskScheduler);

    SAFE_DELETE(m_pVoiceRecorder);

    // Singular file download manager
    SAFE_DELETE(m_pSingularFileDownloadManager);

    // NULL the message/net stuff
    g_pMultiplayer->SetPreContextSwitchHandler(NULL);
    g_pMultiplayer->SetPostContextSwitchHandler(NULL);
    g_pMultiplayer->SetPreWeaponFireHandler(NULL);
    g_pMultiplayer->SetPostWeaponFireHandler(NULL);
    g_pMultiplayer->SetBulletImpactHandler(NULL);
    g_pMultiplayer->SetBulletFireHandler(NULL);
    g_pMultiplayer->SetExplosionHandler(NULL);
    g_pMultiplayer->SetBreakTowLinkHandler(NULL);
    g_pMultiplayer->SetDrawRadarAreasHandler(NULL);
    g_pMultiplayer->SetDamageHandler(NULL);
    g_pMultiplayer->SetFireHandler(NULL);
    g_pMultiplayer->SetProjectileStopHandler(NULL);
    g_pMultiplayer->SetProjectileHandler(NULL);
    g_pMultiplayer->SetProcessCamHandler(nullptr);
    g_pMultiplayer->SetRender3DStuffHandler(NULL);
    g_pMultiplayer->SetPreRenderSkyHandler(NULL);
    g_pMultiplayer->SetRenderHeliLightHandler(nullptr);
    g_pMultiplayer->SetRenderEverythingBarRoadsHandler(nullptr);
    g_pMultiplayer->SetChokingHandler(NULL);
    g_pMultiplayer->SetPreWorldProcessHandler(NULL);
    g_pMultiplayer->SetPostWorldProcessHandler(NULL);
    g_pMultiplayer->SetPostWorldProcessPedsAfterPreRenderHandler(nullptr);
    g_pMultiplayer->SetPreFxRenderHandler(NULL);
    g_pMultiplayer->SetPostColorFilterRenderHandler(nullptr);
    g_pMultiplayer->SetPreHudRenderHandler(NULL);
    g_pMultiplayer->DisableCallsToCAnimBlendNode(true);
    g_pMultiplayer->SetCAnimBlendAssocDestructorHandler(NULL);
    g_pMultiplayer->SetAddAnimationHandler(NULL);
    g_pMultiplayer->SetAddAnimationAndSyncHandler(NULL);
    g_pMultiplayer->SetAssocGroupCopyAnimationHandler(NULL);
    g_pMultiplayer->SetBlendAnimationHierarchyHandler(NULL);
    g_pMultiplayer->SetProcessCollisionHandler(NULL);
    g_pMultiplayer->SetVehicleCollisionHandler(NULL);
    g_pMultiplayer->SetVehicleDamageHandler(NULL);
    g_pMultiplayer->SetHeliKillHandler(NULL);
    g_pMultiplayer->SetObjectDamageHandler(NULL);
    g_pMultiplayer->SetObjectBreakHandler(NULL);
    g_pMultiplayer->SetWaterCannonHitHandler(NULL);
    g_pMultiplayer->SetGameObjectDestructHandler(NULL);
    g_pMultiplayer->SetGameVehicleDestructHandler(NULL);
    g_pMultiplayer->SetGamePlayerDestructHandler(NULL);
    g_pMultiplayer->SetGameProjectileDestructHandler(NULL);
    g_pMultiplayer->SetGameModelRemoveHandler(NULL);
    g_pMultiplayer->SetGameRunNamedAnimDestructorHandler(nullptr);
    g_pMultiplayer->SetGameEntityRenderHandler(NULL);
    g_pMultiplayer->SetDrivebyAnimationHandler(nullptr);
    g_pMultiplayer->SetPedStepHandler(nullptr);
    g_pMultiplayer->SetVehicleWeaponHitHandler(nullptr);
    g_pMultiplayer->SetAudioZoneRadioSwitchHandler(nullptr);
    g_pGame->SetPreWeaponFireHandler(NULL);
    g_pGame->SetPostWeaponFireHandler(NULL);
    g_pGame->SetTaskSimpleBeHitHandler(NULL);
    g_pGame->GetAudioEngine()->SetWorldSoundHandler(NULL);
    g_pCore->SetMessageProcessor(NULL);
    g_pCore->GetKeyBinds()->SetKeyStrokeHandler(NULL);
    g_pCore->GetKeyBinds()->SetCharacterKeyHandler(NULL);
    g_pNet->StopNetwork();
    g_pNet->RegisterPacketHandler(NULL);
    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds();
    pKeyBinds->RemoveAllFunctions(CClientGame::StaticProcessClientKeyBind);
    pKeyBinds->RemoveAllControlFunctions(CClientGame::StaticProcessClientControlBind);
    pKeyBinds->RemoveAllFunctions(CClientGame::StaticProcessServerKeyBind);
    pKeyBinds->RemoveAllControlFunctions(CClientGame::StaticProcessServerControlBind);
    pKeyBinds->RemoveAllControlFunctions(CClientGame::StaticUpdateFireKey);
    pKeyBinds->SetAllControlsEnabled(true, true, true);
    g_pCore->ForceCursorVisible(false);
    SetCursorEventsEnabled(false);

    // Reset discord stuff
    const auto discord = g_pCore->GetDiscord();
    if (discord && discord->IsDiscordRPCEnabled())
    {
        discord->ResetDiscordData();
        discord->SetPresenceState(_("Main menu"), false);
        discord->UpdatePresence();
    }

    // Destroy our stuff
    SAFE_DELETE(m_pManager);            // Will trigger onClientResourceStop
    SAFE_DELETE(m_pNametags);
    SAFE_DELETE(m_pSyncDebug);
    SAFE_DELETE(m_pNetworkStats);
    SAFE_DELETE(m_pNetAPI);
    SAFE_DELETE(m_pRPCFunctions);
    SAFE_DELETE(m_pUnoccupiedVehicleSync);
    SAFE_DELETE(m_pPedSync);
#ifdef WITH_OBJECT_SYNC
    SAFE_DELETE(m_pObjectSync);
#endif
    SAFE_DELETE(m_pBlendedWeather);
    SAFE_DELETE(m_pMovingObjectsManager);
    SAFE_DELETE(m_pRadarMap);
    SAFE_DELETE(m_pRemoteCalls);
    SAFE_DELETE(m_pLuaManager);
    SAFE_DELETE(m_pLatentTransferManager);
    SAFE_DELETE(m_pResourceFileDownloadManager);

    SAFE_DELETE(m_pRootEntity);

    SAFE_DELETE(m_pModelCacheManager);
    // SAFE_DELETE(m_pGameEntityXRefManager);
    SAFE_DELETE(m_pZoneNames);
    SAFE_DELETE(m_pScriptKeyBinds);

    // Delete the scriptdebugger
    SAFE_DELETE(m_pScriptDebugging);

    // Delete the transfer boxes
    SAFE_DELETE(m_pTransferBox);
    SAFE_DELETE(m_pBigPacketTransferBox);

    SAFE_DELETE(m_pLocalServer);
    SAFE_DELETE(m_pDebugHookManager);

    // Packet handler
    SAFE_DELETE(m_pPacketHandler);

    // Delete PerfStatManager
    delete CClientPerfStatManager::GetSingleton();

    // NULL the global CClientGame var
    g_pClientGame = NULL;
    m_bBeingDeleted = false;

    CStaticFunctionDefinitions::ResetAllSurfaceInfo();
}

/*
bool CClientGame::StartGame ( void ) // for an offline game (e.g. editor)
{
    m_Status = STATUS_OFFLINE;
    g_pCore->SetOfflineMod ( true ); // hide chatbox etc
    g_pCore->SetConnected ( true ); // not sure, but its required :)
    g_pCore->HideMainMenu (); // duh

    // If the game isn't started, start it
    if ( g_pGame->GetSystemState () == 7 )
    {
        g_pGame->StartGame ();
    }
    return true;
}
*/

#include <crtdbg.h>
// #define _CRTDBG_CHECK_EVERY_16_DF   0x00100000  /* check heap every 16 heap ops */
// #define _CRTDBG_CHECK_EVERY_128_DF  0x00800000  /* check heap every 128 heap ops */
// #define _CRTDBG_CHECK_EVERY_1024_DF 0x04000000  /* check heap every 1024 heap ops */

void CClientGame::EnablePacketRecorder(const char* szFilename)
{
    m_pManager->GetPacketRecorder()->StartRecord(szFilename, true);
}

void CClientGame::StartPlayback()
{
    //    strcpy ( m_szNick, "Playback" );

    m_bIsPlayingBack = true;
    m_bFirstPlaybackFrame = true;
    m_pManager->GetPacketRecorder()->SetPacketHandler(CClientGame::StaticProcessPacket);

    if (!m_pManager->IsGameLoaded())
    {
        g_pGame->StartGame();
    }
}

bool CClientGame::StartGame(const char* szNick, const char* szPassword, eServerType Type)
{
    m_ServerType = Type;
    // int dbg = _CrtSetDbgFlag ( _CRTDBG_REPORT_FLAG );
    // dbg |= _CRTDBG_ALLOC_MEM_DF;
    // dbg |= _CRTDBG_CHECK_ALWAYS_DF;
    // dbg |= _CRTDBG_DELAY_FREE_MEM_DF;
    // dbg |= _CRTDBG_LEAK_CHECK_DF;
    //_CrtSetDbgFlag(dbg);

    // Verify that the nickname is valid
    if (!IsNickValid(szNick))
    {
        g_pCore->ShowMessageBox(_("Error") + _E("CD01"), _("Invalid nickname! Please go to Settings and set a new one!"), MB_BUTTON_OK | MB_ICON_ERROR);
        g_pCore->GetModManager()->RequestUnload();
        return false;
    }

    // Store our nickname
    m_strLocalNick.AssignLeft(szNick, MAX_PLAYER_NICK_LENGTH);

    // Are we connected?
    if (g_pNet->IsConnected() || m_bIsPlayingBack)
    {
        // Hide the console when connecting..
        if (g_pCore->GetConsole()->IsVisible())
            g_pCore->GetConsole()->SetVisible(false);

        // Display the status box
        g_pCore->ShowMessageBox(_("CONNECTING"), _("Entering the game ..."), MB_ICON_INFO);

        // Send the initial data to the server
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        if (pBitStream)
        {
            // Hash the password if neccessary
            MD5 Password;
            memset(Password.data, 0, sizeof(MD5));
            if (szPassword)
            {
                // Is it long enough?
                size_t sizePassword = strlen(szPassword);
                if (sizePassword > 0)
                {
                    // Hash the password and put it in the struct
                    CMD5Hasher Hasher;
                    Hasher.Calculate(szPassword, sizePassword, Password);
                }
            }

            // Append version information
            pBitStream->Write(static_cast<unsigned short>(MTA_DM_NETCODE_VERSION));
            pBitStream->Write(static_cast<unsigned short>(MTA_DM_VERSION));
            pBitStream->Write(static_cast<unsigned short>(MTA_DM_BITSTREAM_VERSION));

            SString strPlayerVersion("%d.%d.%d-%d.%05d.%d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE,
                                     MTASA_VERSION_BUILD, g_pNet->GetNetRev());
            pBitStream->WriteString(strPlayerVersion);

            pBitStream->WriteBit(g_pCore->IsOptionalUpdateInfoRequired(g_pNet->GetConnectedServer(true)));

            pBitStream->Write(static_cast<unsigned char>(g_pGame->GetGameVersion()));

            // Append user details
            SString strTemp = StringZeroPadout(m_strLocalNick, MAX_PLAYER_NICK_LENGTH);
            pBitStream->Write(strTemp.c_str(), MAX_PLAYER_NICK_LENGTH);
            pBitStream->Write(reinterpret_cast<const char*>(Password.data), sizeof(MD5));

            // Append community information (Removed)
            std::string strUser;
            pBitStream->Write(strUser.c_str(), MAX_SERIAL_LENGTH);

            // Send an empty string if server still has old Discord implementation (#2499)
            if (g_pNet->CanServerBitStream(eBitStreamVersion::Discord_InitialImplementation) && !g_pNet->CanServerBitStream(eBitStreamVersion::Discord_Cleanup))
            {
                pBitStream->WriteString<uchar>("");
            }

            // Send the packet as joindata
            g_pNet->SendPacket(PACKET_ID_PLAYER_JOINDATA, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
            g_pNet->DeallocateNetBitStream(pBitStream);

            return true;
        }
    }
    else
    {
        g_pCore->ShowMessageBox(_("Error") + _E("CD02"), _("Not connected; please use Quick Connect or the 'connect' command to connect to a server."),
                                MB_BUTTON_OK | MB_ICON_ERROR);
        g_pCore->GetModManager()->RequestUnload();
    }

    return false;
}

void CClientGame::SetupLocalGame(eServerType Type)
{
    SString strConfig = (Type == SERVER_TYPE_EDITOR) ? "editor.conf" : "local.conf";
    m_bWaitingForLocalConnect = true;
    if (!m_pLocalServer)
        m_pLocalServer = new CLocalServer(strConfig);
}

bool CClientGame::StartLocalGame(eServerType Type, const char* szPassword)
{
    // Verify that the nickname is valid
    std::string strNick;
    g_pCore->GetCVars()->Get("nick", strNick);

    if (!IsNickValid(strNick.c_str()))
    {
        g_pCore->ShowMessageBox(_("Error") + _E("CD03"), _("Invalid nickname! Please go to Settings and set a new one!"), MB_BUTTON_OK | MB_ICON_ERROR);
        g_pCore->GetModManager()->RequestUnload();
        return false;
    }

    m_bWaitingForLocalConnect = false;
    m_ServerType = Type;
    SString strTemp = (Type == SERVER_TYPE_EDITOR) ? "editor.conf" : "local.conf";

    SAFE_DELETE(m_pLocalServer);

    // Store our nickname
    m_strLocalNick.AssignLeft(strNick.c_str(), MAX_PLAYER_NICK_LENGTH);

    // Got a server?
    if (m_bLocalPlay)
    {
        // Start the server locally
        if (!m_Server.Start(strTemp))
        {
            m_bWaitingForLocalConnect = true;
            m_bErrorStartingLocal = true;
            g_pCore->ShowMessageBox(_("Error") + _E("CD60"), _("Could not start the local server. See console for details."), MB_BUTTON_OK | MB_ICON_ERROR);
            g_pCore->GetModManager()->RequestUnload();
            return false;
        }

        if (szPassword)
            m_Server.SetPassword(szPassword);

        // Display the status box<<<<<
        m_OnCancelLocalGameClick = GUI_CALLBACK(&CClientGame::OnCancelLocalGameClick, this);
        g_pCore->ShowMessageBox(_("Local Server"), _("Starting local server ..."), MB_BUTTON_CANCEL | MB_ICON_INFO, &m_OnCancelLocalGameClick);
    }
    else
    {
        g_pCore->GetModManager()->RequestUnload();
        return false;
    }

    // We're waiting for connection
    m_bWaitingForLocalConnect = true;
    return true;
}

bool CClientGame::OnCancelLocalGameClick(CGUIElement* pElement)
{
    if (m_bLocalPlay && m_bWaitingForLocalConnect)
    {
        g_pCore->RemoveMessageBox();
        g_pCore->GetModManager()->RequestUnload();
        return true;
    }
    return false;
}

void CClientGame::DoPulsePreFrame()
{
    if (m_Status == CClientGame::STATUS_JOINED)
    {
        if (m_pVoiceRecorder && m_pVoiceRecorder->IsEnabled())
        {
            m_pVoiceRecorder->DoPulse();
        }
    }
}

void CClientGame::DoPulsePreHUDRender(bool bDidUnminimize, bool bDidRecreateRenderTargets)
{
    // Allow scripted dxSetRenderTarget for old scripts
    g_pCore->GetGraphics()->GetRenderItemManager()->EnableSetRenderTargetOldVer(true);

    // If appropriate, call onClientRestore
    if (bDidUnminimize)
    {
        CLuaArguments Arguments;
        Arguments.PushBoolean(bDidRecreateRenderTargets);
        m_pRootEntity->CallEvent("onClientRestore", Arguments, false);
        m_bWasMinimized = false;

        // Reverse any mute on minimize effects
        g_pGame->GetAudioEngine()->SetEffectsMasterVolume(g_pGame->GetSettings()->GetSFXVolume());
        g_pGame->GetAudioEngine()->SetMusicMasterVolume(g_pGame->GetSettings()->GetRadioVolume());
        m_pManager->GetSoundManager()->SetMinimizeMuted(false);
    }

    // Call onClientHUDRender LUA event
    CLuaArguments Arguments;
    m_pRootEntity->CallEvent("onClientHUDRender", Arguments, false);

    // Disallow scripted dxSetRenderTarget for old scripts
    g_pCore->GetGraphics()->GetRenderItemManager()->EnableSetRenderTargetOldVer(false);

    // Restore in case script forgets
    g_pCore->GetGraphics()->GetRenderItemManager()->RestoreDefaultRenderTarget();

    DebugElementRender();
}

void CClientGame::DoPulsePostFrame()
{
    TIMING_CHECKPOINT("+CClientGame::DoPulsePostFrame");
#ifdef DEBUG_KEYSTATES
    // Get the controller state
    CControllerState cs;
    g_pGame->GetPad()->GetCurrentControllerState(&cs);

    SString strBuffer;
    strBuffer = SString::Printf(
        "LeftShoulder1: %u\n"
        "LeftShoulder2: %u\n"
        "RightShoulder1: %u\n"
        "RightShoulder2: %u\n"
        "DPadUp: %u\n"
        "DPadDown: %u\n"
        "DPadLeft: %u\n"
        "DPadRight: %u\n"
        "Start: %u\n"
        "Select: %u\n"
        "ButtonSquare: %u\n"
        "ButtonTriangle: %u\n"
        "ButtonCross: %u\n"
        "ButtonCircle: %u\n"
        "ShockButtonL: %u\n"
        "ShockButtonR: %u\n"
        "PedWalk: %u\n",
        cs.LeftShoulder1, cs.LeftShoulder2, cs.RightShoulder1, cs.RightShoulder2, cs.DPadUp, cs.DPadDown, cs.DPadLeft, cs.DPadRight, cs.Start, cs.Select,
        cs.ButtonSquare, cs.ButtonTriangle, cs.ButtonCross, cs.ButtonCircle, cs.ShockButtonL, cs.ShockButtonR, cs.m_bPedWalk);

    g_pCore->GetGraphics()->DrawTextTTF(300, 10, 1280, 800, 0xFFFFFFFF, strBuffer, 1.0f, 0);

    strBuffer = SString::Printf(
        "VehicleMouseLook: %u\n"
        "LeftStickX: %u\n"
        "LeftStickY: %u\n"
        "RightStickX: %u\n"
        "RightStickY: %u",
        cs.m_bVehicleMouseLook, cs.LeftStickX, cs.LeftStickY, cs.RightStickX, cs.RightStickY);

    g_pCore->GetGraphics()->DrawTextTTF(300, 320, 1280, 800, 0xFFFFFFFF, strBuffer, 1.0f, 0);
#endif

    UpdateModuleTickCount64();

    if (m_pManager->IsGameLoaded())
    {
        // Pulse the nametags before anything that changes player positions, we'll be 1 frame behind, but so is the camera
        // If nametags are enabled, pulse the nametag manager
        if (m_bShowNametags)
        {
            m_pNametags->DoPulse();
        }

        // Sync debug
        m_pSyncDebug->OnPulse();

        // Also eventually draw FPS
        if (m_bShowFPS)
        {
            DrawFPS();
        }

        CGraphicsInterface* pGraphics = g_pCore->GetGraphics();
        unsigned int        uiHeight = pGraphics->GetViewportHeight();
        unsigned int        uiWidth = pGraphics->GetViewportWidth();

        // Draw a little star in the corner if async is on
        if (g_pGame->IsASyncLoadingEnabled(true))
        {
            unsigned int uiPosY = g_pGame->IsASyncLoadingEnabled() ? uiHeight - 7 : uiHeight - 12;
            pGraphics->DrawString(uiWidth - 5, uiPosY, 0x80ffffff, 1, "*");
        }

        // Draw notice text if dx test mode is enabled
        if (g_pCore->GetGraphics()->GetRenderItemManager()->GetTestMode())
        {
            unsigned int uiPosY = uiHeight - 30;
            pGraphics->DrawString(uiWidth - 155, uiPosY, 0x40ffffff, 1, "dx test mode enabled");
        }

        // Draw notice text if diagnostic mode enabled
        EDiagnosticDebugType diagnosticDebug = g_pCore->GetDiagnosticDebug();
        if (diagnosticDebug == EDiagnosticDebug::LOG_TIMING_0000)
        {
            unsigned int uiPosY = uiHeight - 30;
            pGraphics->DrawString(uiWidth - 185, uiPosY, 0xffffff00, 1, "Debug setting: #0000 Log timing");
        }

        // Draw network trouble message if required
        if (m_pNetAPI->IsNetworkTrouble())
        {
            int iPosX = uiWidth / 2;                    // Half way across
            int iPosY = uiHeight * 45 / 100;            // 45/100 down
            g_pCore->GetGraphics()->DrawString(iPosX, iPosY, iPosX, iPosY, COLOR_ARGB(255, 255, 0, 0), "*** NETWORK TROUBLE ***", 2.0f, 2.0f,
                                               DT_NOCLIP | DT_CENTER);
        }

        // Adjust the streaming memory size cvar [if needed]
        if (!g_pCore->IsUsingCustomStreamingMemorySize())
        {
            unsigned int uiStreamingMemoryPrev;
            g_pCore->GetCVars()->Get("streaming_memory", uiStreamingMemoryPrev);
            uint uiStreamingMemory = SharedUtil::Clamp(g_pCore->GetMinStreamingMemory(), uiStreamingMemoryPrev, g_pCore->GetMaxStreamingMemory());
            if (uiStreamingMemory != uiStreamingMemoryPrev)
                g_pCore->GetCVars()->Set("streaming_memory", uiStreamingMemory);
        }

        const auto streamingMemorySizeBytes = g_pCore->GetStreamingMemory();
        if (g_pMultiplayer->GetLimits()->GetStreamingMemory() != streamingMemorySizeBytes)
        {
            g_pMultiplayer->GetLimits()->SetStreamingMemory(streamingMemorySizeBytes);
        }

// If we're in debug mode and are supposed to show task data, do it
#ifdef MTA_DEBUG
        if (m_pShowPlayerTasks)
        {
            DrawTasks(m_pShowPlayerTasks);
        }

        if (m_pShowPlayer)
        {
            DrawPlayerDetails(m_pShowPlayer);
        }

        std::vector<CClientPlayer*>::const_iterator iter = m_pPlayerManager->IterBegin();
        for (; iter != m_pPlayerManager->IterEnd(); ++iter)
        {
            CClientPlayer* pPlayer = *iter;
            if (pPlayer->IsStreamedIn() && pPlayer->IsShowingWepdata())
                DrawWeaponsyncData(pPlayer);
        }
#endif

#if defined(MTA_DEBUG) || defined(MTA_BETA)
        if (m_bShowSyncingInfo)
        {
            // Draw the header boxz
            CVector vecPosition = CVector(0.05f, 0.32f, 0);
            m_pDisplayManager->DrawText2D("Syncing vehicles:", vecPosition, 1.0f, 0xFFFFFFFF);

            // Print each vehicle we're syncing
            CDeathmatchVehicle*                       pVehicle;
            list<CDeathmatchVehicle*>::const_iterator iter = m_pUnoccupiedVehicleSync->IterBegin();
            for (; iter != m_pUnoccupiedVehicleSync->IterEnd(); iter++)
            {
                vecPosition.fY += 0.03f;
                pVehicle = *iter;

                SString strBuffer("ID: %u (%s)", pVehicle->GetID(), pVehicle->GetNamePointer());

                m_pDisplayManager->DrawText2D(strBuffer, vecPosition, 1.0f, 0xFFFFFFFF);
            }
        }
#endif
        // Heli Clear time
        if (m_LastClearTime.Get() > HeliKill_List_Clear_Rate)
        {
            // Clear our list now
            m_HeliCollisionsMap.clear();
            m_LastClearTime.Reset();
        }

        // Check if we need to update the Discord Rich Presence state
        if (const long long ticks = GetTickCount64_(); ticks > m_timeLastDiscordStateUpdate + TIME_DISCORD_UPDATE_RATE)
        {
            const auto discord = g_pCore->GetDiscord();

            if (discord && discord->IsDiscordRPCEnabled() && discord->IsDiscordCustomDetailsDisallowed())
            {
                if (auto pLocalPlayer = g_pClientGame->GetLocalPlayer())
                {
                    CVector position;
                    SString zoneName;

                    pLocalPlayer->GetPosition(position);
                    CStaticFunctionDefinitions::GetZoneName(position, zoneName, true);

                    if (zoneName == "Unknown")
                    {
                        zoneName = _("Area 51");
                    }

                    auto taskManager = pLocalPlayer->GetTaskManager();
                    auto task = taskManager->GetActiveTask();                    
                    auto pVehicle = pLocalPlayer->GetOccupiedVehicle();
                    bool useZoneName = true;

                    const eClientVehicleType vehicleType = (pVehicle) ? CClientVehicleManager::GetVehicleType(pVehicle->GetModel()) : CLIENTVEHICLE_NONE;
                    std::string discordState = (pVehicle) ? g_vehicleTypePrefixes.at(vehicleType).c_str() : _("Walking around ");

                    if (task && task->IsValid())
                    {
                        const auto taskSub = task->GetSubTask();
                        const auto taskType = task->GetTaskType();

                        // Check for states which match our primary task
                        std::vector<STaskState> taskStates;
                        for (const auto& [task, state] : g_playerTaskStates)
                        {
                            if (task == taskType)
                                taskStates.push_back(state);
                        }

                        // Check for non-matching sub/secondary tasks and remove them
                        for (auto it = taskStates.begin(); it != taskStates.end(); )
                        {
                            const STaskState& taskState = (*it);

                            const auto taskSecondary =
                                (!taskState.eSecondaryType.has_value()) ? nullptr : taskManager->GetTaskSecondary(taskState.eSecondaryType.value());
                            bool useState = (!taskState.eSubTask.has_value() && !taskState.eSecondaryTask.has_value());

                            if (!useState)
                            {
                                if (taskSub != nullptr && taskState.eSubTask.has_value() && taskState.eSubTask.value() == taskSub->GetTaskType())
                                    useState = true;
                                else if (taskSecondary != nullptr && taskState.eSecondaryTask.has_value() &&
                                         taskState.eSecondaryTask.value() == taskSecondary->GetTaskType())
                                    useState = true;
                            }

                            if (!useState)
                                it = taskStates.erase(it);
                            else
                                ++it;
                        }

                        // Choose a random task state (if we have any)
                        const int stateCount = taskStates.size();
                        if (stateCount > 0)
                        {
                            std::srand(GetTickCount64_());
                            const int  index = (std::rand() % stateCount);
                            const auto& taskState = taskStates[index];

                            discordState = taskState.strState;
                            useZoneName = taskState.bUseZone;
                        }                                       

                        if (useZoneName)
                        {
                            discordState.append(" " + zoneName);
                        }

                        discord->SetPresenceState(discordState.c_str(), false);
                    }
                }
                else
                {
                    discord->SetPresenceState(_("In-game"), false);
                }

                discord->SetPresencePartySize(m_pPlayerManager->Count(), g_pClientGame->GetServerInfo()->GetMaxPlayers(), false);
                m_timeLastDiscordStateUpdate = ticks;
            }
        }

        CClientPerfStatManager::GetSingleton()->DoPulse();
    }

    m_pRadarMap->DoRender();
    m_pManager->DoRender();
    DoPulses();

    // If we're supposed to show netstat, draw them infront of everything else
    if (m_bShowNetstat)
    {
        m_pNetworkStats->Draw();
    }
}

void CClientGame::DoPulses()
{
    TIMING_CHECKPOINT("-CClientGame::DoPulsePostFrame");

    g_pCore->ApplyFrameRateLimit();

    TIMING_CHECKPOINT("+CClientGame::DoPulses");

    m_BuiltCollisionMapThisFrame = false;

    if (m_bIsPlayingBack && m_bFirstPlaybackFrame && m_pManager->IsGameLoaded())
    {
        g_pCore->GetConsole()->Printf("First playback frame, starting");
        m_pManager->GetPacketRecorder()->StartPlayback("log.rec", false);
        m_bFirstPlaybackFrame = false;
    }

    // Output stuff from our server eventually
    m_Server.Pulse();

    if (m_pManager->IsGameLoaded() && m_Status == CClientGame::STATUS_JOINED && GetTickCount64_() - m_llLastTransgressionTime > 60000)
    {
        uint    uiLevel = 0;
        uint    uiInform = 0;
        SString strMessage;

        // Is the player a cheater?
        if (!m_pManager->GetAntiCheat().PerformChecks())
        {
            uiLevel = 1;
            uiInform = 2;
        }
        else
        {
            strMessage = g_pNet->GetNextBuffer();
            if (strMessage.length())
            {
                uiLevel = atoi(strMessage.SplitLeft(":", &strMessage));
                uiInform = atoi(strMessage.SplitLeft(":", &strMessage));
            }
        }

        // Send message to the server
        if (uiLevel)
        {
            SString strPrefix = (uiInform == 2) ? "AC" : (uiInform == 1) ? "VF" : "SD";
            SString strMessageCombo = SString("%s #%d %s", *strPrefix, uiLevel, strMessage.c_str()).TrimEnd(" ");
            m_llLastTransgressionTime = GetTickCount64_();
            AddReportLog(3100, strMessageCombo + SString(" (%d)", uiInform));

            if (uiInform > 0)
            {
                // The server will use the whole message as supplied here
                NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
                pBitStream->Write(uiLevel);
                pBitStream->WriteString(strMessageCombo);
                g_pNet->SendPacket(PACKET_ID_PLAYER_TRANSGRESSION, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
                g_pNet->DeallocateNetBitStream(pBitStream);
            }
            else
            {
                // Otherwise, disconnect here
                AddReportLog(7105, SString("Core - Kicked (%s)", *strMessageCombo));
                g_pCore->ShowMessageBox(_("Error") + _E("CD05"), SString(_("You were kicked from the game ( %s )"), *strMessageCombo),
                                        MB_BUTTON_OK | MB_ICON_ERROR);
                g_pCore->GetModManager()->RequestUnload();
                return;
            }
        }
    }

    // Send diagnostic info
    if (m_pManager->IsGameLoaded() && m_Status == CClientGame::STATUS_JOINED)
    {
        // Retrieve data
        SString strMessage = g_pNet->GetDiagnosticStatus();

        // Send to the server if changed
        if (strMessage != m_strLastDiagnosticStatus)
        {
            m_strLastDiagnosticStatus = strMessage;
            NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
            pBitStream->WriteString(strMessage);
            g_pNet->SendPacket(PACKET_ID_PLAYER_DIAGNOSTIC, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
            g_pNet->DeallocateNetBitStream(pBitStream);
        }
    }

    // Pulse the network interface

    // Extrapolation test - Change the pulse order to reduce latency (Has side effects for peds)
    DoPulses2(false);

    m_pUnoccupiedVehicleSync->DoPulse();
    m_pPedSync->DoPulse();
#ifdef WITH_OBJECT_SYNC
    m_pObjectSync->DoPulse();
#endif
    m_pLatentTransferManager->DoPulse();
    m_pLuaManager->DoPulse();
    m_pScriptDebugging->UpdateLogOutput();

    GetModelCacheManager()->DoPulse();

#ifdef MTA_DEBUG
    UpdateMimics();
#endif

    // Grab the current time
    unsigned long ulCurrentTime = CClientTime::GetTime();

    // Waiting for a connect?
    if (m_bWaitingForLocalConnect)
    {
        // Connected?
        if (g_pNet->IsConnected())
        {
            // No longer waiting for connect
            m_bWaitingForLocalConnect = false;

            // Assume local server has the same bitstream version
            g_pNet->SetServerBitStreamVersion(static_cast<unsigned short>(MTA_DM_BITSTREAM_VERSION));

            // Run the game normally.
            StartGame(m_strLocalNick, m_Server.GetPassword().c_str(), m_ServerType);
        }
        else
        {
            // Going to try connecting? Do this when the internal server has booted
            // and we haven't started the connecting.
            if (m_Server.IsReady() && m_iLocalConnectAttempts == 0)
            {
                g_pCore->ShowMessageBox(_("Local Server"), _("Connecting to local server..."), MB_ICON_INFO);

                // Connect
                if (g_pNet->StartNetwork("localhost", 22010))
                {
                    // We're waiting for connection
                    m_iLocalConnectAttempts = 1;
                    m_ulTimeStart = CClientTime::GetTime();
                }
                else
                {
                    g_pCore->ShowNetErrorMessageBox(_("Error") + _E("CD06"), _("Error connecting to server."));
                    g_pCore->GetModManager()->RequestUnload();
                    return;
                }
            }

            // Timeout?
            if (m_ulTimeStart != 0 && CClientTime::GetTime() >= m_ulTimeStart + 5000)
            {
                // Show timeout message and disconnect
                g_pCore->ShowNetErrorMessageBox(_("Error") + _E("CD07"), _("Connecting to local server timed out. See console for details."));
                g_pCore->GetModManager()->RequestUnload();
                return;
            }
        }
    }

    // If the game is loaded ...
    if (m_pManager->IsGameLoaded())
    {
        // Pulse the blended weather manager
        m_pBlendedWeather->DoPulse();

        // If we weren't ingame last frame; call the on ingame event
        if (!m_bGameLoaded)
        {
            // Fix for gta not being focused sometimes
            SetActiveWindow(g_pCore->GetHookedWindow());
            SetFocus(g_pCore->GetHookedWindow());

            m_bGameLoaded = true;
            Event_OnIngame();
        }

        // Check if the player is hitting the enter vehicle button
        DoVehicleInKeyCheck();

        // Pulse some stuff
        m_pMovingObjectsManager->DoPulse();

        // Get rid of our deleted elements
        m_ElementDeleter.DoDeleteAll();
        m_pLuaManager->ProcessPendingDeleteList();

        // Get rid of deleted GUI elements
        g_pCore->GetGUI()->CleanDeadPool();

        // Allow scripted dxSetRenderTarget for old scripts
        g_pCore->GetGraphics()->GetRenderItemManager()->EnableSetRenderTargetOldVer(true);

        // Call onClientRender LUA event
        CLuaArguments Arguments;
        m_pRootEntity->CallEvent("onClientRender", Arguments, false);

        // Disallow scripted dxSetRenderTarget for old scripts
        g_pCore->GetGraphics()->GetRenderItemManager()->EnableSetRenderTargetOldVer(false);

        // Restore in case script forgets
        g_pCore->GetGraphics()->GetRenderItemManager()->RestoreDefaultRenderTarget();

        // Ensure replaced/restored textures for models in the GTA map are correct
        g_pGame->FlushPendingRestreamIPL();

        // Respawn objects in respawn pool
        m_ObjectRespawner.DoRespawnAll();
    }

    // Are we connecting?
    if (m_Status == CClientGame::STATUS_CONNECTING)
    {
        if (m_bErrorStartingLocal)
        {
            g_pCore->GetModManager()->RequestUnload();
            return;
        }

        // Timed out?
        if (!m_bWaitingForLocalConnect && ulCurrentTime >= m_ulTimeStart + NET_CONNECT_TIMEOUT)
        {
            g_pCore->ShowNetErrorMessageBox(_("Error") + _E("CD08"), _("Connection timed out"), "connect-timed-out", true);
            g_pCore->GetModManager()->RequestUnload();
            return;
        }
    }
    else if (m_Status == CClientGame::STATUS_JOINED)
    {
        // Pulse DownloadFiles if we're transferring stuff
        GetResourceFileDownloadManager()->DoPulse();
        DownloadSingularResourceFiles();
        GetRemoteCalls()->ProcessQueuedFiles();
    }

    // Not waiting for local connect?
    if (!m_bWaitingForLocalConnect)
    {
        // Trigger the ingame and connected event
        if (!m_bTriggeredIngameAndConnected && m_pManager->IsGameLoaded() && g_pCore->IsConnected())
        {
            m_bTriggeredIngameAndConnected = true;
            Event_OnIngameAndConnected();

            // Initialize the game
            g_pCore->GetGame()->Initialize();

            // Save default streamer buffer size in IMG manager
            m_pManager->GetIMGManager()->InitDefaultBufferSize();
        }

        unsigned char ucError = g_pNet->GetConnectionError();

        // Lost connection?
        if (!g_pNet->IsConnected() && !m_bGracefulDisconnect && !m_bIsPlayingBack)
        {
            // See if we can figure out what specifically it was
            if (ucError == 0)
            {
                g_pCore->ShowNetErrorMessageBox(_("Error") + _E("CD09"), _("Connection with the server was lost"));
                g_pCore->GetModManager()->RequestUnload();
                return;
            }
            else
            {
                SString strError;
                SString strErrorCode;
                switch (ucError)
                {
                    case RID_RSA_PUBLIC_KEY_MISMATCH:
                        strError = _("Disconnected: unknown protocol error");
                        strErrorCode = _E("CD10");            // encryption key mismatch
                        break;
                    case RID_REMOTE_DISCONNECTION_NOTIFICATION:
                        strError = _("Disconnected: disconnected remotely");
                        strErrorCode = _E("CD11");
                        break;
                    case RID_REMOTE_CONNECTION_LOST:
                        strError = _("Disconnected: connection lost remotely");
                        strErrorCode = _E("CD12");
                        break;
                    case RID_CONNECTION_BANNED:
                        strError = _("Disconnected: you are banned from this server");
                        strErrorCode = _E("CD13");
                        break;
                    case RID_NO_FREE_INCOMING_CONNECTIONS:
                        strError = _("Disconnected: the server is currently full");
                        strErrorCode = _E("CD14");
                        break;
                    case RID_DISCONNECTION_NOTIFICATION:
                        strError = _("Disconnected: disconnected from the server");
                        strErrorCode = _E("CD15");
                        break;
                    case RID_CONNECTION_LOST:
                        strError = _("Disconnected: connection to the server was lost");
                        strErrorCode = _E("CD16");
                        break;
                    case RID_INVALID_PASSWORD:
                        strError = _("Disconnected: invalid password specified");
                        strErrorCode = _E("CD17");
                        break;
                    default:
                        strError = _("Disconnected: connection was refused");
                        strErrorCode = _E("CD18");
                        break;
                }

                // Display an error, reset the error status and exit
                g_pCore->ShowNetErrorMessageBox(_("Error") + strErrorCode, strError);
                g_pNet->SetConnectionError(0);
                g_pCore->GetModManager()->RequestUnload();
            }
        }

        // If we're in the verificating status
        if (m_Status == CClientGame::STATUS_JOINING)
        {
            // Time out the verification if it takes too long
            if (m_ulVerifyTimeStart != 0 && ulCurrentTime >= m_ulVerifyTimeStart + CLIENT_VERIFICATION_TIMEOUT)
            {
                g_pCore->ShowNetErrorMessageBox(_("Error") + _E("CD19"), _("MTA Client verification failed!"));
                g_pCore->GetModManager()->RequestUnload();
            }
        }
    }

    // Check for radar input
    m_pRadarMap->DoPulse();
    g_pCore->GetGraphics()->SetAspectRatioAdjustmentSuspended(m_pRadarMap->IsRadarShowing());

    // Got a local player?
    if (m_pLocalPlayer)
    {
        // Network updates
        m_pLocalPlayer->UpdateVehicleInOut();
        UpdatePlayerTarget();
        UpdatePlayerWeapons();
        UpdateTrailers();            // Test: Does it always work without this check?
        UpdateStunts();
        // Clear last damager if more than 2 seconds old
        if (CClientTime::GetTime() - m_ulDamageTime > 2000)
        {
            m_DamagerID = INVALID_ELEMENT_ID;
            m_ucDamageWeapon = 0xFF;
            m_ucDamageBodyPiece = 0xFF;
        }
        DoWastedCheck(m_DamagerID, m_ucDamageWeapon, m_ucDamageBodyPiece);
    }

    // Game hacks, restore certain variables
    // game-speed changes after spawning
    g_pGame->SetGameSpeed(m_fGameSpeed);
    // money changes on death/getting into taxis
    g_pGame->GetPlayerInfo()->SetPlayerMoney(m_lMoney);
    // wanted to stop it changing on skin change etc
    if (m_pLocalPlayer)
    {
        if (m_dwWanted != g_pGame->GetPlayerInfo()->GetWanted()->GetWantedLevel())
            g_pGame->GetPlayerInfo()->GetWanted()->SetWantedLevelNoFlash(m_dwWanted);
    }
    // stop players dying from starvation
    g_pGame->GetPlayerInfo()->SetLastTimeEaten(0);
    // reset weapon logs (for preventing quickreload)

    // Update streaming
    m_pManager->UpdateStreamers();

    // Send screen shot data
    ProcessDelayedSendList();

    // Collect async task scheduler results
    m_pAsyncTaskScheduler->CollectResults();

    TIMING_CHECKPOINT("-CClientGame::DoPulses");
}

// Extrapolation test
void CClientGame::DoPulses2(bool bCalledFromIdle)
{
    bool bIsUsingAlternatePulseOrder = IsUsingAlternatePulseOrder(!bCalledFromIdle);

    // Figure out which pulses to do
    bool bDoStandardPulses;
    bool bDoVehicleManagerPulse;

    if (!bIsUsingAlternatePulseOrder)
    {
        // With std pulse order, do pulses when not called from idle
        bDoStandardPulses = !bCalledFromIdle;
        bDoVehicleManagerPulse = !bCalledFromIdle;
    }
    else
    {
        // With alt pulse order, do pulses when called from idle
        bDoStandardPulses = bCalledFromIdle;
        bDoVehicleManagerPulse = bCalledFromIdle;

        // Except when watching a remote synced vehicle
        if (CClientVehicle* pTargetVehicle = DynamicCast<CClientVehicle>(m_pCamera->GetTargetEntity()))
            if (pTargetVehicle->GetControllingPlayer() != m_pPlayerManager->GetLocalPlayer())
                bDoVehicleManagerPulse = !bDoVehicleManagerPulse;
    }

    if (bDoStandardPulses)
    {
        // Change to high precision so arguments in element data and events can
        // be rounded to look more like what is expected
        ChangeFloatPrecision(true);

        // Pulse the network interface
        TIMING_CHECKPOINT("+NetPulse");
        g_pNet->DoPulse();
        TIMING_CHECKPOINT("-NetPulse");

        // Change precision back, and check we are in low precision mode 4 sure
        ChangeFloatPrecision(false);
        assert(!IsHighFloatPrecision());
    }

    m_pManager->DoPulse(bDoStandardPulses, bDoVehicleManagerPulse);

    if (bDoStandardPulses)
    {
        m_pNetAPI->DoPulse();
    }
}

void CClientGame::HandleException(CExceptionInformation* pExceptionInformation)
{
}

void CClientGame::HandleRadioNext(CControlFunctionBind*)
{
    if (g_pClientGame)
    {
        CClientPlayer* pPlayer = g_pClientGame->m_pPlayerManager->GetLocalPlayer();
        if (pPlayer)
        {
            pPlayer->NextRadioChannel();
        }
    }
}

void CClientGame::HandleRadioPrevious(CControlFunctionBind*)
{
    if (g_pClientGame)
    {
        CClientPlayer* pPlayer = g_pClientGame->m_pPlayerManager->GetLocalPlayer();
        if (pPlayer)
        {
            pPlayer->PreviousRadioChannel();
        }
    }
}
bool CClientGame::IsNametagValid(const char* szNick)
{
    // Grab the size of the nametag. Check that it's not to long or short
    size_t sizeNick = MbUTF8ToUTF16(szNick).size();
    if (sizeNick < MIN_PLAYER_NAMETAG_LENGTH || sizeNick > MAX_PLAYER_NAMETAG_LENGTH)
    {
        return false;
    }

    // Check that each character is valid (Anything above 32)
    unsigned char ucTemp;
    for (size_t i = 0; i < sizeNick; i++)
    {
        ucTemp = szNick[i];
        if (ucTemp < 32)
        {
            return false;
        }
    }

    // nametag is valid, return true
    return true;
}

bool CClientGame::IsNickValid(const char* szNick)
{
    // Grab the size of the nick. Check that it's within the player
    size_t sizeNick = strlen(szNick);
    if (sizeNick < MIN_PLAYER_NICK_LENGTH || sizeNick > MAX_PLAYER_NICK_LENGTH)
    {
        return false;
    }

    // Check that each character is valid (visible characters exluding space)
    unsigned char ucTemp;
    for (size_t i = 0; i < sizeNick; i++)
    {
        ucTemp = szNick[i];
        if (ucTemp < 33 || ucTemp > 126)
        {
            return false;
        }
    }

    // Nickname is valid, return true
    return true;
}

void CClientGame::ShowNetstat(int iCmd)
{
    bool bShow = (iCmd == 1) ? true : (iCmd == 0) ? false : !m_bShowNetstat;

    if (bShow && !m_bShowNetstat)
    {
        m_pNetworkStats->Reset();
    }
    m_bShowNetstat = bShow;
}

#ifdef MTA_WEPSYNCDBG
void CClientGame::ShowWepdata(const char* szNick)
{
    CClientPlayer* pPlayer = m_pPlayerManager->Get(szNick);
    if (pPlayer)
    {
        pPlayer->SetShowingWepdata(!pPlayer->IsShowingWepdata());
    }
}
#endif

#ifdef MTA_DEBUG

void CClientGame::ShowWepdata(const char* szNick)
{
    CClientPlayer* pPlayer = m_pPlayerManager->Get(szNick);
    if (pPlayer)
    {
        pPlayer->SetShowingWepdata(!pPlayer->IsShowingWepdata());
    }
}

void CClientGame::ShowPlayer(const char* szNick)
{
    m_pShowPlayer = m_pPlayerManager->Get(szNick);
}

void CClientGame::ShowTasks(const char* szNick)
{
    m_pShowPlayerTasks = m_pPlayerManager->Get(szNick);
}

void CClientGame::SetMimic(unsigned int uiMimicCount)
{
    // Check if we're within the max mimics boundary
    if (uiMimicCount > MAX_MIMICS)
        return;

    // Create neccessary players
    while (m_Mimics.size() < uiMimicCount)
    {
        CClientPlayer* pPlayer = new CClientPlayer(m_pManager, static_cast<ElementID>(MAX_NET_PLAYERS_REAL + (int)m_Mimics.size()));
        pPlayer->SetNick("Mimic");
        m_Mimics.push_back(pPlayer);
    }

    // Destroy neccessary players
    while (m_Mimics.size() > uiMimicCount)
    {
        CClientPlayer*  pPlayer = m_Mimics.back();
        CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle();
        if (pVehicle)
            delete pVehicle;

        delete pPlayer;
        m_Mimics.pop_back();
    }
}

#endif

void CClientGame::DoVehicleInKeyCheck()
{
    // Grab the controller state
    CControllerState cs;
    g_pGame->GetPad()->GetCurrentControllerState(&cs);
    static bool bButtonTriangleWasDown = false;
    if (cs.ButtonTriangle)
    {
        if (!bButtonTriangleWasDown)
        {
            bButtonTriangleWasDown = true;

            // Process the hit
            // If we are already in a vehicle
            CClientVehicle* pVehicle = m_pLocalPlayer->GetOccupiedVehicle();
            if (pVehicle)
            {
                // Exit
                m_pLocalPlayer->ExitVehicle();
            }
            else
            {
                // Enter
                // Are we holding the aim_weapon key?
                SBindableGTAControl* pBind = g_pCore->GetKeyBinds()->GetBindableFromControl("aim_weapon");
                if (pBind && pBind->bState)
                {
                    // Stop because the player is probably doing special attack
                    return;
                }
                m_pLocalPlayer->EnterVehicle(nullptr, false);
            }
        }
    }
    else
    {
        bButtonTriangleWasDown = false;
    }
}

void CClientGame::UpdatePlayerTarget()
{
    CControllerState ControllerState;
    m_pLocalPlayer->GetControllerState(ControllerState);
    CVector vecOrigin, vecTarget;
    m_pLocalPlayer->GetShotData(&vecOrigin, &vecTarget);

    // Ignore the local player so we don't get hit
    m_pLocalPlayer->WorldIgnore(true);

    // Run a process line of sight and look for an entity we target
    CEntity*   pColEntity = NULL;
    CColPoint* pColPoint = NULL;
    g_pGame->GetWorld()->ProcessLineOfSight(&vecOrigin, &vecTarget, &pColPoint, &pColEntity);

    // Unignore the local player again
    m_pLocalPlayer->WorldIgnore(false);

    // Kill the colpoint or we get a severe memoryleak
    if (pColPoint)
        pColPoint->Destroy();

    if (pColEntity != m_pTargetedGameEntity)
    {
        m_pTargetedGameEntity = pColEntity;

        if (pColEntity)
        {
            CPools* pPools = g_pGame->GetPools();
            m_pTargetedEntity = pPools->GetClientEntity((DWORD*)pColEntity->GetInterface());
        }
        else
            m_pTargetedEntity = NULL;

        // Store the last targeted player's id
        if (m_pTargetedEntity && m_pTargetedEntity->GetType() == CCLIENTPLAYER)
        {
            m_TargetedPlayerID = m_pTargetedEntity->GetID();
        }
        else
            m_TargetedPlayerID = INVALID_ELEMENT_ID;

        // Send the target
        ElementID TargetID = INVALID_ELEMENT_ID;
        if (m_pTargetedEntity && !m_pTargetedEntity->IsLocalEntity())
        {
            TargetID = m_pTargetedEntity->GetID();
        }

        CBitStream bitStream;
        bitStream.pBitStream->Write(TargetID);
        m_pNetAPI->RPC(PLAYER_TARGET, bitStream.pBitStream);

        // Call our onClientPlayerTarget event
        CLuaArguments Arguments;
        if (m_pTargetedEntity)
            Arguments.PushElement(m_pTargetedEntity);
        else
            Arguments.PushBoolean(false);
        m_pLocalPlayer->CallEvent("onClientPlayerTarget", Arguments, true);
    }
}

void CClientGame::UpdatePlayerWeapons()
{
    // Check whether we changed weapon slots
    eWeaponSlot currentSlot = m_pLocalPlayer->GetCurrentWeaponSlot();
    if (currentSlot != m_lastWeaponSlot)
    {
        CLuaArguments Arguments;
        Arguments.PushNumber(m_lastWeaponSlot);
        Arguments.PushNumber(currentSlot);
        bool bCancelled = !m_pLocalPlayer->CallEvent("onClientPlayerWeaponSwitch", Arguments, true);

        if (bCancelled)
        {
            // Save the current ammo in clip
            unsigned short usAmmoInClip = 0;
            CWeapon*       pWeapon = m_pLocalPlayer->GetWeapon(m_lastWeaponSlot);
            if (pWeapon)
                usAmmoInClip = static_cast<unsigned short>(pWeapon->GetAmmoInClip());

            // Force it back to the old slot
            m_pLocalPlayer->SetCurrentWeaponSlot(m_lastWeaponSlot);

            // Restore the ammo in clip that there was in that slot
            if (usAmmoInClip > 0)
                m_pLocalPlayer->GetWeapon()->SetAmmoInClip(usAmmoInClip);
        }
        else
        {
            CBitStream             bitStream;
            CWeapon*               pWeapon = m_pLocalPlayer->GetWeapon();
            NetBitStreamInterface& BitStream = *(bitStream.pBitStream);
            SWeaponSlotSync        slot;

            // Always send bit in case server is not in sync
            if ((BitStream.Version() >= 0x44 && m_lastWeaponSlot == WEAPONSLOT_TYPE_THROWN) || BitStream.Version() >= 0x4D)
            {
                CWeapon* pLastWeapon = m_pLocalPlayer->GetWeapon(m_lastWeaponSlot);
                if (pLastWeapon && pLastWeapon->GetAmmoTotal() == 0 &&
                    (m_lastWeaponSlot == WEAPONSLOT_TYPE_THROWN ||
                     (BitStream.Version() >= 0x5A && (m_lastWeaponSlot == WEAPONSLOT_TYPE_HEAVY || m_lastWeaponSlot == WEAPONSLOT_TYPE_SPECIAL))))
                    BitStream.WriteBit(true);
                else
                    BitStream.WriteBit(false);
            }

            if (pWeapon)
            {
                /* Send a packet to the server with info about the new weapon,
                   so the server stays in sync reliably */
                unsigned int uiSlot = static_cast<unsigned int>(pWeapon->GetSlot());
                slot.data.uiSlot = uiSlot;
                BitStream.Write(&slot);

                if (CWeaponNames::DoesSlotHaveAmmo(uiSlot))
                {
                    SWeaponAmmoSync ammo(pWeapon->GetType(), true, true);
                    ammo.data.usAmmoInClip = static_cast<unsigned short>(pWeapon->GetAmmoInClip());
                    ammo.data.usTotalAmmo = static_cast<unsigned short>(pWeapon->GetAmmoTotal());
                    BitStream.Write(&ammo);
                }
            }
            else
            {
                slot.data.uiSlot = 0;
                BitStream.Write(&slot);
            }

            m_pNetAPI->RPC(PLAYER_WEAPON, bitStream.pBitStream);
            m_lastWeaponSlot = m_pLocalPlayer->GetCurrentWeaponSlot();
        }
    }
}

void CClientGame::UpdateTrailers()
{
    // This function is here to re-attach trailers if they fall off

    unsigned long ulCurrentTime = GetTickCount32();

    CClientVehicle *                        pVehicle = NULL, *pTrailer = NULL;
    CVehicle *                              pGameVehicle = NULL, *pGameTrailer = NULL;
    unsigned long                           ulIllegalTowBreakTime;
    vector<CClientVehicle*>::const_iterator iterVehicles = m_pVehicleManager->StreamedBegin();
    for (; iterVehicles != m_pVehicleManager->StreamedEnd(); iterVehicles++)
    {
        pVehicle = *iterVehicles;
        ulIllegalTowBreakTime = pVehicle->GetIllegalTowBreakTime();

        // Do we have an illegal break?
        if (ulIllegalTowBreakTime != 0)
        {
            // Has it been atleast 1 second since the break
            if (ulCurrentTime > (ulIllegalTowBreakTime + 1000))
            {
                // Try to re-attach them
                CClientVehicle* pTowedBy = pVehicle->GetTowedByVehicle();
                if (pTowedBy)
                {
                    // Little hack to keep illegaly detached trailers close to their tower
                    CVector vecPosition;
                    pVehicle->GetPosition(vecPosition);
                    pVehicle->SetPosition(vecPosition);

                    pGameVehicle = pTowedBy->GetGameVehicle();
                    pGameTrailer = pVehicle->GetGameVehicle();
                    if (pGameVehicle && pGameTrailer)
                    {
                        // pGameTrailer->SetTowLink ( pGameVehicle );
                        CVector vecRotation;
                        pTowedBy->GetRotationRadians(vecRotation);
                        pVehicle->SetRotationRadians(vecRotation);
                        pTowedBy->InternalSetTowLink(pVehicle);
                    }
                }

                // Reset the break time, even if we couldnt re-attach it
                pVehicle->SetIllegalTowBreakTime(0);
            }
        }
    }
}

void CClientGame::UpdateFireKey()
{
    if (m_pLocalPlayer)
    {
        SBindableGTAControl* pControl = g_pCore->GetKeyBinds()->GetBindableFromControl("fire");

        // Is our 'fire' control enabled?
        if (pControl->bEnabled)
        {
            // ** Satchel charge detonation **
            {
                // Do we have a detonator in our hand?
                if (m_pLocalPlayer->GetCurrentWeaponSlot() == WEAPONSLOT_TYPE_DETONATOR)
                {
                    // Planted any satchels?
                    if (m_pLocalPlayer->CountProjectiles(WEAPONTYPE_REMOTE_SATCHEL_CHARGE) > 0)
                    {
                        // Change the state back to false so this press doesn't do anything else
                        pControl->bState = false;

                        // Tell the server we want to detonate our satchels
                        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
                        if (pBitStream)
                        {
                            g_pNet->SendPacket(PACKET_ID_DETONATE_SATCHELS, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
                            g_pNet->DeallocateNetBitStream(pBitStream);
                        }
                    }
                    // Remove the detonator if we dont have any satchels planted
                    else
                    {
                        m_pLocalPlayer->RemoveWeapon(WEAPONTYPE_DETONATOR);
                    }
                }
            }

            // Has our control state been cut short?
            if (!pControl->bState)
                return;

            // ** Stealth kill **
            {
                if (m_pLocalPlayer->IsStealthAiming())
                {
                    // Do we have a target ped?
                    CClientPed* pTargetPed = m_pLocalPlayer->GetTargetedPed();
                    if (pTargetPed)
                    {
                        // Do we have a target player?
                        if (IS_PLAYER(pTargetPed))
                        {
                            CClientPlayer* pTargetPlayer = static_cast<CClientPlayer*>(pTargetPed);

                            // Is the targetted player on a team
                            CClientTeam* pTeam = pTargetPlayer->GetTeam();
                            if (pTeam)
                            {
                                // Is this friendly-fire?
                                if (pTargetPlayer->IsOnMyTeam(m_pLocalPlayer) && !pTeam->GetFriendlyFire())
                                {
                                    // Change the state back to false so this press doesn't do anything else
                                    pControl->bState = false;
                                    return;
                                }
                            }
                        }
                        CPlayerPed* pGameTarget = static_cast<CClientPed*>(pTargetPed)->GetGamePlayer();
                        if (pGameTarget)
                        {
                            // Would GTA let us stealth kill now?
                            if (m_pLocalPlayer->GetGamePlayer()->GetPedIntelligence()->TestForStealthKill(pGameTarget, false))
                            {
                                // Grab our local position
                                CVector vecLocalPosition;
                                m_pLocalPlayer->GetPosition(vecLocalPosition);

                                // Grab the target's position
                                CVector vecTargetPosition;
                                pTargetPed->GetPosition(vecTargetPosition);

                                // Work out an angle between the players, and set this as we initiate our knife kill
                                float fAngle = AngleBetweenPoints2D(vecLocalPosition, vecTargetPosition);
                                m_pLocalPlayer->SetCurrentRotation(fAngle);

                                // Change the state back to false so this press doesn't do anything else
                                pControl->bState = false;
                                CLuaArguments Arguments;
                                Arguments.PushElement(pTargetPed);
                                if (m_pLocalPlayer->CallEvent("onClientPlayerStealthKill", Arguments, false))
                                {
                                    if (pTargetPed->IsLocalEntity())
                                    {
                                        CStaticFunctionDefinitions::KillPed(*pTargetPed, m_pLocalPlayer, 4 /*WEAPONTYPE_KNIFE*/, 9 /*BODYPART_HEAD*/, true);
                                        return;
                                    }

                                    // Lets request a stealth kill
                                    CBitStream bitStream;
                                    bitStream.pBitStream->Write(pTargetPed->GetID());
                                    m_pNetAPI->RPC(REQUEST_STEALTH_KILL, bitStream.pBitStream);
                                }
                                else
                                {
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void CClientGame::UpdateStunts()
{
    // * Two wheeler *
    static unsigned long ulLastCarTwoWheelCounter = 0;
    static float         fLastCarTwoWheelDist = 0.0f;
    unsigned long        ulTemp = g_pGame->GetPlayerInfo()->GetCarTwoWheelCounter();
    // Did we start a stunt?
    if (ulLastCarTwoWheelCounter == 0 && ulTemp != 0)
    {
        // Call our stunt event
        CLuaArguments Arguments;
        Arguments.PushString("2wheeler");
        m_pLocalPlayer->CallEvent("onClientPlayerStuntStart", Arguments, true);
    }
    // Did we finish a stunt?
    else if (ulLastCarTwoWheelCounter != 0 && ulTemp == 0)
    {
        // Call our stunt event
        CLuaArguments Arguments;
        Arguments.PushString("2wheeler");
        Arguments.PushNumber(ulLastCarTwoWheelCounter);
        Arguments.PushNumber(fLastCarTwoWheelDist);
        m_pLocalPlayer->CallEvent("onClientPlayerStuntFinish", Arguments, true);
    }
    ulLastCarTwoWheelCounter = ulTemp;
    fLastCarTwoWheelDist = g_pGame->GetPlayerInfo()->GetCarTwoWheelDist();

    // * Wheelie *
    static unsigned long ulLastBikeRearWheelCounter = 0;
    static float         fLastBikeRearWheelDist = 0.0f;
    ulTemp = g_pGame->GetPlayerInfo()->GetBikeRearWheelCounter();
    // Did we start a stunt?
    if (ulLastBikeRearWheelCounter == 0 && ulTemp != 0)
    {
        // Call our stunt event
        CLuaArguments Arguments;
        Arguments.PushString("wheelie");
        m_pLocalPlayer->CallEvent("onClientPlayerStuntStart", Arguments, true);
    }
    // Did we finish a stunt?
    else if (ulLastBikeRearWheelCounter != 0 && ulTemp == 0)
    {
        // Call our stunt event
        CLuaArguments Arguments;
        Arguments.PushString("wheelie");
        Arguments.PushNumber(ulLastBikeRearWheelCounter);
        Arguments.PushNumber(fLastBikeRearWheelDist);
        m_pLocalPlayer->CallEvent("onClientPlayerStuntFinish", Arguments, true);
    }
    ulLastBikeRearWheelCounter = ulTemp;
    fLastBikeRearWheelDist = g_pGame->GetPlayerInfo()->GetBikeRearWheelDist();

    // * Stoppie *
    static unsigned long ulLastBikeFrontWheelCounter = 0;
    static float         fLastBikeFrontWheelDist = 0.0f;
    ulTemp = g_pGame->GetPlayerInfo()->GetBikeFrontWheelCounter();
    // Did we start a stunt?
    if (ulLastBikeFrontWheelCounter == 0 && ulTemp != 0)
    {
        // Call our stunt event
        CLuaArguments Arguments;
        Arguments.PushString("stoppie");
        m_pLocalPlayer->CallEvent("onClientPlayerStuntStart", Arguments, true);
    }
    // Did we finish a stunt?
    else if (ulLastBikeFrontWheelCounter != 0 && ulTemp == 0)
    {
        // Call our stunt event
        CLuaArguments Arguments;
        Arguments.PushString("stoppie");
        Arguments.PushNumber(ulLastBikeFrontWheelCounter);
        Arguments.PushNumber(fLastBikeFrontWheelDist);
        m_pLocalPlayer->CallEvent("onClientPlayerStuntFinish", Arguments, true);
    }
    ulLastBikeFrontWheelCounter = ulTemp;
    fLastBikeFrontWheelDist = g_pGame->GetPlayerInfo()->GetBikeFrontWheelDist();
}

void CClientGame::StaticUpdateFireKey(CControlFunctionBind* pBind)
{
    g_pClientGame->UpdateFireKey();
}

void CClientGame::ChangeVehicleWeapon(bool bNext)
{
    if (m_pLocalPlayer && m_pLocalPlayer->GetRealOccupiedVehicle())
    {
        eWeaponSlot currentWeaponSlot = m_pLocalPlayer->GetCurrentWeaponSlot();
        eWeaponSlot weaponSlot = currentWeaponSlot;
        CWeapon*    pWeapon = NULL;
        while (!pWeapon || pWeapon->GetType() == WEAPONTYPE_UNARMED)
        {
            if (bNext)
            {
                if (weaponSlot == WEAPONSLOT_TYPE_DETONATOR)
                {
                    weaponSlot = WEAPONSLOT_TYPE_UNARMED;
                    break;
                }

                weaponSlot = (eWeaponSlot)(weaponSlot + 1);

                if (weaponSlot == currentWeaponSlot)
                    break;

                pWeapon = m_pLocalPlayer->GetWeapon(weaponSlot);
            }
            else
            {
                if (weaponSlot == WEAPONSLOT_TYPE_UNARMED)
                {
                    if (weaponSlot != currentWeaponSlot)
                        break;
                    weaponSlot = WEAPONSLOT_TYPE_DETONATOR;
                }

                weaponSlot = (eWeaponSlot)(weaponSlot - 1);

                if (weaponSlot == currentWeaponSlot)
                    break;

                pWeapon = m_pLocalPlayer->GetWeapon(weaponSlot);
            }
        }
        if (pWeapon || weaponSlot == WEAPONSLOT_TYPE_UNARMED)
        {
            m_pLocalPlayer->SetCurrentWeaponSlot(weaponSlot);
        }
    }
}

void CClientGame::SetAllDimensions(unsigned short usDimension)
{
    m_pManager->GetMarkerStreamer()->SetDimension(usDimension);
    m_pManager->GetObjectStreamer()->SetDimension(usDimension);
    m_pManager->GetObjectLodStreamer()->SetDimension(usDimension);
    m_pManager->GetPickupStreamer()->SetDimension(usDimension);
    m_pManager->GetPlayerStreamer()->SetDimension(usDimension);
    m_pManager->GetRadarAreaManager()->SetDimension(usDimension);
    m_pManager->GetVehicleStreamer()->SetDimension(usDimension);
    m_pManager->GetRadarMarkerManager()->SetDimension(usDimension);
    m_pManager->GetSoundManager()->SetDimension(usDimension);
    m_pManager->GetPointLightsManager()->SetDimension(usDimension);
    m_pManager->GetWaterManager()->SetDimension(usDimension);
    m_pNametags->SetDimension(usDimension);
    m_pCamera->SetDimension(usDimension);
}

void CClientGame::SetAllInteriors(unsigned char ucInterior)
{
    m_pNametags->m_ucInterior = ucInterior;
}

bool CClientGame::StaticKeyStrokeHandler(const SString& strKey, bool bState, bool bIsConsoleInputKey)
{
    return g_pClientGame->KeyStrokeHandler(strKey, bState, bIsConsoleInputKey);
}

bool CClientGame::KeyStrokeHandler(const SString& strKey, bool bState, bool bIsConsoleInputKey)
{
    // Do we have a root yet?
    if (m_pRootEntity)
    {
        // Ignore keydown/up pair if main menu is displayed, or console input will use the character
        bool bIgnore = false;
        if (bState)
        {
            auto pFocusedBrowser = g_pCore->IsWebCoreLoaded() ? g_pCore->GetWebCore()->GetFocusedWebView() : nullptr;
            bool isMouseKey = strKey.substr(0, 5) == "mouse";

            if (g_pCore->IsMenuVisible() || (g_pCore->GetConsole()->IsInputActive() && bIsConsoleInputKey) ||
                (pFocusedBrowser && !pFocusedBrowser->IsLocal() && !isMouseKey))

                bIgnore = true;            // Ignore this keydown and the matching keyup
            else
                MapInsert(m_AllowKeyUpMap, strKey);            // Use this keydown and the matching keyup
        }
        else
        {
            if (!MapContains(m_AllowKeyUpMap, strKey))
                bIgnore = true;            // Ignore this keyup
            else
                MapRemove(m_AllowKeyUpMap, strKey);            // Use this keyup
        }

        if (!bIgnore)
        {
            bool bAllow = true;
            // Call our key-stroke event
            CLuaArguments Arguments;
            Arguments.PushString(strKey);
            Arguments.PushBoolean(bState);
            bAllow = m_pRootEntity->CallEvent("onClientKey", Arguments, false);
            if (bState == true)
            {
                if (bAllow == false && strKey == "escape")
                {
                    if (m_bLastKeyWasEscapeCancelled)
                    {
                        // Escape cannot be skipped twice
                        bAllow = true;
                        m_bLastKeyWasEscapeCancelled = false;
                    }
                    else
                        m_bLastKeyWasEscapeCancelled = true;
                }
                else
                    m_bLastKeyWasEscapeCancelled = false;
            }
            return bAllow;
        }
    }
    m_bLastKeyWasEscapeCancelled = false;
    return true;
}

bool CClientGame::StaticCharacterKeyHandler(WPARAM wChar)
{
    return g_pClientGame->CharacterKeyHandler(wChar);
}

bool CClientGame::CharacterKeyHandler(WPARAM wChar)
{
    // Do we have a root yet?
    if (m_pRootEntity && g_pCore->IsMenuVisible() == false && g_pCore->GetConsole()->IsInputActive() == false)
    {
        // Cancel event if remote browser is focused
        auto pFocusedBrowser = g_pCore->IsWebCoreLoaded() ? g_pCore->GetWebCore()->GetFocusedWebView() : nullptr;
        if (pFocusedBrowser && !pFocusedBrowser->IsLocal())
            return false;

        // Safe character?
        if (wChar >= 32)
        {
            // Generate a null-terminating string for our character
            wchar_t wUNICODE[2] = {static_cast<wchar_t>(wChar), '\0'};

            // Convert our UTF character into an ANSI string
            SString strANSI = UTF16ToMbUTF8(wUNICODE);

            // Call our character event
            CLuaArguments Arguments;
            Arguments.PushString(strANSI);
            m_pRootEntity->CallEvent("onClientCharacter", Arguments, false);
        }
    }

    return false;
}

void CClientGame::StaticProcessClientKeyBind(CKeyFunctionBind* pBind)
{
    g_pClientGame->ProcessClientKeyBind(pBind);
}

void CClientGame::ProcessClientKeyBind(CKeyFunctionBind* pBind)
{
    m_pScriptKeyBinds->ProcessKey(pBind->boundKey->szKey, pBind->triggerState, SCRIPT_KEY_BIND_FUNCTION);
}

void CClientGame::StaticProcessClientControlBind(CControlFunctionBind* pBind)
{
    g_pClientGame->ProcessClientControlBind(pBind);
}

void CClientGame::ProcessClientControlBind(CControlFunctionBind* pBind)
{
    m_pScriptKeyBinds->ProcessKey(pBind->control->szControl, pBind->triggerState, SCRIPT_KEY_BIND_CONTROL_FUNCTION);
}

void CClientGame::StaticProcessServerKeyBind(CKeyFunctionBind* pBind)
{
    g_pClientGame->ProcessServerKeyBind(pBind);
}

void CClientGame::ProcessServerKeyBind(CKeyFunctionBind* pBind)
{
    const char*   szName = pBind->boundKey->szKey;
    unsigned char ucNameLength = (unsigned char)strlen(szName);
    CBitStream    bitStream;
    bitStream.pBitStream->WriteBit(false);
    bitStream.pBitStream->WriteBit(pBind->triggerState);
    bitStream.pBitStream->Write(szName, ucNameLength);
    m_pNetAPI->RPC(KEY_BIND, bitStream.pBitStream);
}

void CClientGame::StaticProcessServerControlBind(CControlFunctionBind* pBind)
{
    g_pClientGame->ProcessServerControlBind(pBind);
}

void CClientGame::ProcessServerControlBind(CControlFunctionBind* pBind)
{
    const char*   szName = pBind->control->szControl;
    unsigned char ucNameLength = (unsigned char)strlen(szName);
    CBitStream    bitStream;
    bitStream.pBitStream->WriteBit(true);
    bitStream.pBitStream->WriteBit(pBind->triggerState);
    bitStream.pBitStream->Write(szName, ucNameLength);
    m_pNetAPI->RPC(KEY_BIND, bitStream.pBitStream);
}

bool CClientGame::ProcessMessageForCursorEvents(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool bCursorForcedVisible = g_pCore->IsCursorForcedVisible();
    bool bMenuVisible = g_pCore->IsMenuVisible();
    bool bConsoleVisible = g_pCore->GetConsole()->IsVisible();

    if (bCursorForcedVisible)
    {
        if (!bMenuVisible && !bConsoleVisible)
        {
            if (m_bCursorEventsEnabled)
            {
                unsigned char ucButtonHit = 0xFF;
                switch (uMsg)
                {
                    case WM_LBUTTONDOWN:
                        ucButtonHit = 0;
                        break;
                    case WM_LBUTTONUP:
                        ucButtonHit = 1;
                        break;
                    case WM_MBUTTONDOWN:
                        ucButtonHit = 2;
                        break;
                    case WM_MBUTTONUP:
                        ucButtonHit = 3;
                        break;
                    case WM_RBUTTONDOWN:
                        ucButtonHit = 4;
                        break;
                    case WM_RBUTTONUP:
                        ucButtonHit = 5;
                        break;
                }
                if (ucButtonHit != 0xFF)
                {
                    int iX = GET_X_LPARAM(lParam);
                    int iY = GET_Y_LPARAM(lParam);

                    CVector2D vecResolution = g_pCore->GetGUI()->GetResolution();

                    /*
                    // (IJs) why are these relative? it doesn't make sense
                    CVector2D vecCursorPosition ( ( ( float ) iX ) / vecResolution.fX,
                                                  ( ( float ) iY ) / vecResolution.fY );
                    */

                    CVector2D vecCursorPosition((float)iX, (float)iY);

                    CVector vecOrigin, vecTarget, vecScreen((float)iX, (float)iY, 300.0f);
                    g_pCore->GetGraphics()->CalcWorldCoors(&vecScreen, &vecTarget);

                    // Grab the camera position
                    CCamera* pCamera = g_pGame->GetCamera();
                    CCam*    pCam = pCamera->GetCam(pCamera->GetActiveCam());
                    CMatrix  matCamera;
                    pCamera->GetMatrix(&matCamera);
                    vecOrigin = matCamera.vPos;

                    CColPoint* pColPoint = NULL;
                    CEntity*   pGameEntity = NULL;

                    // Grab the collision point/entity
                    bool bCollision = g_pGame->GetWorld()->ProcessLineOfSight(&vecOrigin, &vecTarget, &pColPoint, &pGameEntity);

                    CVector        vecCollision;
                    ElementID      CollisionEntityID = INVALID_ELEMENT_ID;
                    CClientEntity* pCollisionEntity = NULL;
                    if (bCollision && pColPoint)
                    {
                        vecCollision = pColPoint->GetPosition();
                        if (pGameEntity)
                        {
                            CPools*        pPools = g_pGame->GetPools();
                            CClientEntity* pEntity = pPools->GetClientEntity((DWORD*)pGameEntity->GetInterface());
                            if (pEntity)
                            {
                                pCollisionEntity = pEntity;
                                if (!pEntity->IsLocalEntity())
                                    CollisionEntityID = pEntity->GetID();
                            }
                        }
                    }
                    else
                    {
                        vecCollision = vecTarget;
                    }

                    // Destroy the colpoint so we don't get a leak
                    if (pColPoint)
                    {
                        pColPoint->Destroy();
                    }

                    const char* szButton = NULL;
                    const char* szState = NULL;
                    switch (ucButtonHit)
                    {
                        case 0:
                            szButton = "left";
                            szState = "down";
                            break;
                        case 1:
                            szButton = "left";
                            szState = "up";
                            break;
                        case 2:
                            szButton = "middle";
                            szState = "down";
                            break;
                        case 3:
                            szButton = "middle";
                            szState = "up";
                            break;
                        case 4:
                            szButton = "right";
                            szState = "down";
                            break;
                        case 5:
                            szButton = "right";
                            szState = "up";
                            break;
                    }
                    if (szButton && szState)
                    {
                        if (std::isnan(vecCollision.fX))
                            vecCollision.fX = 0;
                        if (std::isnan(vecCollision.fY))
                            vecCollision.fY = 0;
                        if (std::isnan(vecCollision.fZ))
                            vecCollision.fZ = 0;

                        // Call the event for the client
                        CLuaArguments Arguments;
                        Arguments.PushString(szButton);
                        Arguments.PushString(szState);
                        Arguments.PushNumber(vecCursorPosition.fX);
                        Arguments.PushNumber(vecCursorPosition.fY);
                        Arguments.PushNumber(vecCollision.fX);
                        Arguments.PushNumber(vecCollision.fY);
                        Arguments.PushNumber(vecCollision.fZ);
                        if (pCollisionEntity)
                            Arguments.PushElement(pCollisionEntity);
                        else
                            Arguments.PushBoolean(false);
                        m_pRootEntity->CallEvent("onClientClick", Arguments, false);

                        // Send the button, cursor position, 3d position and the entity collided with
                        CBitStream bitStream;

                        SMouseButtonSync button;
                        button.data.ucButton = ucButtonHit;
                        bitStream.pBitStream->Write(&button);

                        bitStream.pBitStream->WriteCompressed(static_cast<unsigned short>(vecCursorPosition.fX));
                        bitStream.pBitStream->WriteCompressed(static_cast<unsigned short>(vecCursorPosition.fY));

                        SPositionSync position(false);
                        position.data.vecPosition = vecCollision;
                        bitStream.pBitStream->Write(&position);

                        if (CollisionEntityID != INVALID_ELEMENT_ID)
                        {
                            bitStream.pBitStream->WriteBit(true);
                            bitStream.pBitStream->Write(CollisionEntityID);
                        }
                        else
                            bitStream.pBitStream->WriteBit(false);

                        m_pNetAPI->RPC(CURSOR_EVENT, bitStream.pBitStream);

                        if (strcmp(szState, "down") == 0)
                        {
                            CVector2D vecDelta = m_vecLastCursorPosition - vecCursorPosition;

                            if ((GetTickCount32() - m_ulLastClickTick) < DOUBLECLICK_TIMEOUT && vecDelta.Length() <= DOUBLECLICK_MOVE_THRESHOLD)
                            {
                                // Call the event for the client
                                CLuaArguments DoubleClickArguments;
                                DoubleClickArguments.PushString(szButton);
                                DoubleClickArguments.PushNumber(vecCursorPosition.fX);
                                DoubleClickArguments.PushNumber(vecCursorPosition.fY);
                                DoubleClickArguments.PushNumber(vecCollision.fX);
                                DoubleClickArguments.PushNumber(vecCollision.fY);
                                DoubleClickArguments.PushNumber(vecCollision.fZ);
                                if (pCollisionEntity)
                                    DoubleClickArguments.PushElement(pCollisionEntity);
                                else
                                    DoubleClickArguments.PushBoolean(false);
                                m_pRootEntity->CallEvent("onClientDoubleClick", DoubleClickArguments, false);
                            }

                            m_ulLastClickTick = GetTickCount32();
                            m_vecLastCursorPosition = vecCursorPosition;
                        }

                        return true;
                    }
                }
            }
        }
    }
    switch (uMsg)
    {
        case WM_MOUSEMOVE:
        {
            int        iX = GET_X_LPARAM(lParam), iY = GET_Y_LPARAM(lParam);
            static int iPreviousX = 0, iPreviousY = 0;
            if (iX != iPreviousX || iY != iPreviousY)
            {
                iPreviousX = iX, iPreviousY = iY;

                CVector2D vecResolution = g_pCore->GetGUI()->GetResolution();
                CVector2D vecCursorPosition(((float)iX) / vecResolution.fX, ((float)iY) / vecResolution.fY);

                CVector vecTarget, vecScreen((float)iX, (float)iY, 300.0f);
                g_pCore->GetGraphics()->CalcWorldCoors(&vecScreen, &vecTarget);

                // Call the onClientCursorMove event
                CLuaArguments Arguments;
                Arguments.PushNumber((double)vecCursorPosition.fX);
                Arguments.PushNumber((double)vecCursorPosition.fY);
                Arguments.PushNumber((double)iX);
                Arguments.PushNumber((double)iY);
                Arguments.PushNumber((double)vecTarget.fX);
                Arguments.PushNumber((double)vecTarget.fY);
                Arguments.PushNumber((double)vecTarget.fZ);
                m_pRootEntity->CallEvent("onClientCursorMove", Arguments, false);
            }
            break;
        }
    }
    return false;
}

CClientPlayer* CClientGame::GetClosestRemotePlayer(const CVector& vecPosition, float fMaxDistance)
{
    CClientPlayer*                         pClosest = NULL;
    float                                  fDistance = 0.0f, fTemp;
    CVector                                vecTemp;
    CClientPlayer*                         pPlayer;
    vector<CClientPlayer*>::const_iterator iter = m_pPlayerManager->IterBegin();
    for (; iter != m_pPlayerManager->IterEnd(); ++iter)
    {
        pPlayer = *iter;
        if (!pPlayer->IsLocalPlayer() && !pPlayer->IsDeadOnNetwork() && pPlayer->GetHealth() > 0)
        {
            // Ensure remote player is alive and sending position updates
            ulong ulTimeSinceLastPuresync = CClientTime::GetTime() - pPlayer->GetLastPuresyncTime();
            if (ulTimeSinceLastPuresync < static_cast<ulong>(g_TickRateSettings.iPureSync) * 2)
            {
                pPlayer->GetPosition(vecTemp);
                fTemp = DistanceBetweenPoints3D(vecPosition, vecTemp);
                if (fTemp < fMaxDistance)
                {
                    if (!pClosest || fTemp < fDistance)
                    {
                        pClosest = pPlayer;
                        fDistance = fTemp;
                    }
                }
            }
        }
    }
    return pClosest;
}

void CClientGame::SetGameSpeed(float fSpeed)
{
    g_pGame->SetGameSpeed(fSpeed);
    m_fGameSpeed = fSpeed;
}

void CClientGame::SetMinuteDuration(unsigned long ulDelay)
{
    g_pGame->SetMinuteDuration(ulDelay);
    m_ulMinuteDuration = ulDelay;
}

void CClientGame::SetMoney(long lMoney, bool bInstant)
{
    g_pGame->GetPlayerInfo()->SetPlayerMoney(lMoney, bInstant);
    m_lMoney = lMoney;
}

void CClientGame::SetWanted(DWORD dwWanted)
{
    g_pGame->GetPlayerInfo()->GetWanted()->SetWantedLevel(dwWanted);
    m_dwWanted = dwWanted;
}

void CClientGame::AddBuiltInEvents()
{
    // Resource events
    m_Events.AddEvent("onClientResourceStart", "resource", NULL, false);
    m_Events.AddEvent("onClientResourceStop", "resource", NULL, false);

    // Element events
    m_Events.AddEvent("onClientElementDataChange", "name", NULL, false);
    m_Events.AddEvent("onClientElementStreamIn", "", NULL, false);
    m_Events.AddEvent("onClientElementStreamOut", "", NULL, false);
    m_Events.AddEvent("onClientElementDestroy", "", NULL, false);
    m_Events.AddEvent("onClientElementModelChange", "oldModel, newModel", nullptr, false);
    m_Events.AddEvent("onClientElementDimensionChange", "oldDimension, newDimension", nullptr, false);
    m_Events.AddEvent("onClientElementInteriorChange", "oldInterior, newInterior", nullptr, false);

    // Player events
    m_Events.AddEvent("onClientPlayerJoin", "", NULL, false);
    m_Events.AddEvent("onClientPlayerQuit", "reason", NULL, false);
    m_Events.AddEvent("onClientPlayerTarget", "target", NULL, false);
    m_Events.AddEvent("onClientPlayerSpawn", "team", NULL, false);
    m_Events.AddEvent("onClientPlayerChangeNick", "oldNick", NULL, false);
    m_Events.AddEvent("onClientPlayerVehicleEnter", "vehicle, seat", NULL, false);
    m_Events.AddEvent("onClientPlayerVehicleExit", "vehicle, seat", NULL, false);
    m_Events.AddEvent("onClientPlayerTask", "priority, slot, name", NULL, false);
    m_Events.AddEvent("onClientPlayerWeaponSwitch", "previous, current", NULL, false);
    m_Events.AddEvent("onClientPlayerStuntStart", "type", NULL, false);
    m_Events.AddEvent("onClientPlayerStuntFinish", "type, time, distance", NULL, false);
    m_Events.AddEvent("onClientPlayerRadioSwitch", "", NULL, false);
    m_Events.AddEvent("onClientPlayerDamage", "attacker, weapon, bodypart", NULL, false);
    m_Events.AddEvent("onClientPlayerWeaponFire", "weapon, ammo, ammoInClip, hitX, hitY, hitZ, hitElement", NULL, false);
    m_Events.AddEvent("onClientPlayerWasted", "ammo, killer, weapon, bodypart, isStealth, animGroup, animID", nullptr, false);
    m_Events.AddEvent("onClientPlayerChoke", "", NULL, false);
    m_Events.AddEvent("onClientPlayerVoiceStart", "", NULL, false);
    m_Events.AddEvent("onClientPlayerVoiceStop", "", NULL, false);
    m_Events.AddEvent("onClientPlayerVoicePause", "reason", NULL, false);
    m_Events.AddEvent("onClientPlayerVoiceResumed", "reason", NULL, false);
    m_Events.AddEvent("onClientPlayerStealthKill", "target", NULL, false);
    m_Events.AddEvent("onClientPlayerHitByWaterCannon", "vehicle", NULL, false);
    m_Events.AddEvent("onClientPlayerHeliKilled", "heli", NULL, false);
    m_Events.AddEvent("onClientPlayerPickupHit", "pickup, matchingDimension", NULL, false);
    m_Events.AddEvent("onClientPlayerPickupLeave", "pickup, matchingDimension", NULL, false);
    m_Events.AddEvent("onClientPlayerNetworkStatus", "type, ticks", NULL, false);

    // Ped events
    m_Events.AddEvent("onClientPedDamage", "attacker, weapon, bodypart", NULL, false);
    m_Events.AddEvent("onClientPedVehicleEnter", "vehicle, seat", NULL, false);
    m_Events.AddEvent("onClientPedVehicleExit", "vehicle, seat", NULL, false);
    m_Events.AddEvent("onClientPedWeaponFire", "weapon, ammo, ammoInClip, hitX, hitY, hitZ, hitElement", NULL, false);
    m_Events.AddEvent("onClientPedWasted", "", NULL, false);
    m_Events.AddEvent("onClientPedChoke", "", NULL, false);
    m_Events.AddEvent("onClientPedHeliKilled", "heli", NULL, false);
    m_Events.AddEvent("onClientPedHitByWaterCannon", "vehicle", NULL, false);
    m_Events.AddEvent("onClientPedStep", "foot", nullptr, false);

    // Vehicle events
    m_Events.AddEvent("onClientVehicleRespawn", "", NULL, false);
    m_Events.AddEvent("onClientVehicleEnter", "player, seat", NULL, false);
    m_Events.AddEvent("onClientVehicleExit", "player, seat", NULL, false);
    m_Events.AddEvent("onClientVehicleStartEnter", "player, seat", NULL, false);
    m_Events.AddEvent("onClientVehicleStartExit", "player, seat", NULL, false);
    m_Events.AddEvent("onClientTrailerAttach", "towedBy", NULL, false);
    m_Events.AddEvent("onClientTrailerDetach", "towedBy", NULL, false);
    m_Events.AddEvent("onClientVehicleExplode", "", NULL, false);
    m_Events.AddEvent("onClientVehicleCollision", "collidedelement, damageImpulseMag, bodypart, x, y, z, velX, velY, velZ", NULL, false);
    m_Events.AddEvent("onClientVehicleDamage", "attacker, weapon, loss, x, y, z, tyre", NULL, false);
    m_Events.AddEvent("onClientVehicleNitroStateChange", "activated", NULL, false);
    m_Events.AddEvent("onClientVehicleWeaponHit", "weaponType, hitElement, hitX, hitY, hitZ, model, materialID", nullptr, false);

    // GUI events
    m_Events.AddEvent("onClientGUIClick", "button, state, absoluteX, absoluteY", NULL, false);
    m_Events.AddEvent("onClientGUIDoubleClick", "button, state, absoluteX, absoluteY", NULL, false);
    m_Events.AddEvent("onClientGUIMouseDown", "button, absoluteX, absoluteY", NULL, false);
    m_Events.AddEvent("onClientGUIMouseUp", "button, absoluteX, absoluteY", NULL, false);
    m_Events.AddEvent("onClientGUIScroll", "element", NULL, false);
    m_Events.AddEvent("onClientGUIChanged", "element", NULL, false);
    m_Events.AddEvent("onClientGUIAccepted", "element", NULL, false);
    // m_Events.AddEvent ( "onClientGUIClose", "element", NULL, false );
    // m_Events.AddEvent ( "onClientGUIKeyDown", "element", NULL, false );
    m_Events.AddEvent("onClientGUITabSwitched", "element", NULL, false);
    m_Events.AddEvent("onClientGUIComboBoxAccepted", "element", NULL, false);

    // Input events
    m_Events.AddEvent("onClientDoubleClick", "button, screenX, screenY, worldX, worldY, worldZ, element", NULL, false);
    m_Events.AddEvent("onClientMouseMove", "screenX, screenY", NULL, false);
    m_Events.AddEvent("onClientMouseEnter", "screenX, screenY", NULL, false);
    m_Events.AddEvent("onClientMouseLeave", "screenX, screenY", NULL, false);
    m_Events.AddEvent("onClientMouseWheel", "", NULL, false);
    m_Events.AddEvent("onClientGUIMove", "", NULL, false);
    m_Events.AddEvent("onClientGUISize", "", NULL, false);
    m_Events.AddEvent("onClientGUIFocus", "", NULL, false);
    m_Events.AddEvent("onClientGUIBlur", "", NULL, false);
    m_Events.AddEvent("onClientKey", "key, state", NULL, false);
    m_Events.AddEvent("onClientCharacter", "character", NULL, false);
    m_Events.AddEvent("onClientPaste", "clipboardText", NULL, false);

    // Console events
    m_Events.AddEvent("onClientConsole", "text", NULL, false);
    m_Events.AddEvent("onClientCoreCommand", "command", NULL, false);

    // Chat events
    m_Events.AddEvent("onClientChatMessage", "text, r, g, b, messageType", NULL, false);

    // Debug events
    m_Events.AddEvent("onClientDebugMessage", "message, level, file, line", NULL, false);

    // Game events
    m_Events.AddEvent("onClientPreRender", "", NULL, false);
    m_Events.AddEvent("onClientPedsProcessed", "", NULL, false);
    m_Events.AddEvent("onClientHUDRender", "", NULL, false);
    m_Events.AddEvent("onClientRender", "", NULL, false);
    m_Events.AddEvent("onClientMinimize", "", NULL, false);
    m_Events.AddEvent("onClientRestore", "", NULL, false);
    m_Events.AddEvent("onClientMTAFocusChange", "focused", NULL, false);

    // Cursor events
    m_Events.AddEvent("onClientClick", "button, state, screenX, screenY, worldX, worldY, worldZ, gui_clicked", NULL, false);
    m_Events.AddEvent("onClientCursorMove", "relativeX, relativeX, absoluteX, absoluteY, worldX, worldY, worldZ", NULL, false);

    // Marker events
    m_Events.AddEvent("onClientMarkerHit", "entity, matchingDimension", NULL, false);
    m_Events.AddEvent("onClientMarkerLeave", "entity, matchingDimension", NULL, false);

    // Marker events
    m_Events.AddEvent("onClientPickupHit", "entity, matchingDimension", NULL, false);
    m_Events.AddEvent("onClientPickupLeave", "entity, matchingDimension", NULL, false);

    // Col-shape events
    m_Events.AddEvent("onClientColShapeHit", "entity, matchingDimension", NULL, false);
    m_Events.AddEvent("onClientColShapeLeave", "entity, matchingDimension", NULL, false);
    m_Events.AddEvent("onClientElementColShapeHit", "colShape, matchingDimension", NULL, false);
    m_Events.AddEvent("onClientElementColShapeLeave", "colShape, matchingDimension", NULL, false);

    // Explosion events
    m_Events.AddEvent("onClientExplosion", "x, y, z, type", NULL, false);

    // Projectile events
    m_Events.AddEvent("onClientProjectileCreation", "creator", NULL, false);

    // Sound events
    m_Events.AddEvent("onClientSoundStream", "success, length, streamName, error", NULL, false);
    m_Events.AddEvent("onClientSoundFinishedDownload", "length", NULL, false);
    m_Events.AddEvent("onClientSoundChangedMeta", "streamTitle", NULL, false);
    m_Events.AddEvent("onClientSoundStarted", "reason", NULL, false);
    m_Events.AddEvent("onClientSoundStopped", "reason", NULL, false);
    m_Events.AddEvent("onClientSoundBeat", "time", NULL, false);

    // Object events
    m_Events.AddEvent("onClientObjectDamage", "loss, attacker", NULL, false);
    m_Events.AddEvent("onClientObjectBreak", "attacker", NULL, false);
    m_Events.AddEvent("onClientObjectMoveStart", "", NULL, false);
    m_Events.AddEvent("onClientObjectMoveStop", "", NULL, false);

    // Web events
    m_Events.AddEvent("onClientBrowserWhitelistChange", "newPages", NULL, false);
    m_Events.AddEvent("onClientBrowserCreated", "", NULL, false);
    m_Events.AddEvent("onClientBrowserLoadingStart", "url, mainframe", NULL, false);
    m_Events.AddEvent("onClientBrowserDocumentReady", "url", NULL, false);
    m_Events.AddEvent("onClientBrowserLoadingFailed", "url, errorcode, errordescription", NULL, false);
    m_Events.AddEvent("onClientBrowserNavigate", "url, isblocked", NULL, false);
    m_Events.AddEvent("onClientBrowserPopup", "targeturl, openerurl, ispopup", NULL, false);
    m_Events.AddEvent("onClientBrowserCursorChange", "cursor", NULL, false);
    m_Events.AddEvent("onClientBrowserTooltip", "text", NULL, false);
    m_Events.AddEvent("onClientBrowserInputFocusChanged", "gainedfocus", NULL, false);
    m_Events.AddEvent("onClientBrowserResourceBlocked", "url, domain, reason", NULL, false);

    // Misc events
    m_Events.AddEvent("onClientFileDownloadComplete", "fileName, success", NULL, false);

    m_Events.AddEvent("onClientResourceFileDownload", "resource, fileName, fileSize, state", nullptr, false);
    m_Events.AddEvent("onClientTransferBoxProgressChange", "downloadedBytes, downloadTotalBytes", nullptr, false);
    m_Events.AddEvent("onClientTransferBoxVisibilityChange", "isVisible", nullptr, false);

    m_Events.AddEvent("onClientWeaponFire", "ped, x, y, z", NULL, false);

    m_Events.AddEvent("onClientWorldSound", "group, index, x, y, z", nullptr, false);
}

void CClientGame::DrawFPS()
{
    // Draw the background
    float fResWidth = static_cast<float>(g_pCore->GetGraphics()->GetViewportWidth());
    float fResHeight = static_cast<float>(g_pCore->GetGraphics()->GetViewportHeight());
    g_pCore->GetGraphics()->DrawRectangle(0.75f * fResWidth, 0.22f * fResHeight, 0.25f * fResWidth, 0.04f * fResHeight, 0x78000000);

    static char  x = 0;
    static float fDisp = 0.0f;
    if (x == 20)
    {
        x = 0;
        fDisp = g_pGame->GetFPS();
    }
    else
        x++;
    SString strBuffer("FrameRate: %4.2f\n", fDisp);

    // Print it
    m_pDisplayManager->DrawText2D(strBuffer, CVector(0.76f, 0.23f, 0), 1.0f, 0xFFFFFFFF);
}

#ifdef MTA_DEBUG

void CClientGame::DrawTasks(CClientPlayer* pPlayer)
{
    CTask* pTask = NULL;
    CTask* pSubTask = NULL;

    // Got a local player model
    if (pPlayer)
    {
        CTaskManager* man = pPlayer->GetTaskManager();
        if (man == NULL)
            return;
        if ((unsigned long)man == 0xDDDDDDDD)
        {
            m_pDisplayManager->DrawText2D("HELP! MANAGER FUCKED", CVector(0.05f, 0.5f, 0), 1.0f);
            return;
        }

        // Grab the current task
        SString strOutput;
        SString strSubOutput;

        pTask = man->GetTask(TASK_PRIORITY_PHYSICAL_RESPONSE);
        strOutput += SString("Physical Response: %s\n", pTask ? (pTask->GetTaskName()) : ("N/A"));
        strSubOutput += SString("%s\n", pTask && pTask->GetSubTask() ? (pTask->GetSubTask()->GetTaskName()) : ("N/A"));

        pTask = man->GetTask(TASK_PRIORITY_EVENT_RESPONSE_TEMP);
        strOutput += SString("Event Response Temp: %s\n", pTask ? (pTask->GetTaskName()) : ("N/A"));
        strSubOutput += SString("%s\n", pTask && pTask->GetSubTask() ? (pTask->GetSubTask()->GetTaskName()) : ("N/A"));

        pTask = man->GetTask(TASK_PRIORITY_EVENT_RESPONSE_NONTEMP);
        strOutput += SString("Event Response Non-temp: %s\n", pTask ? (pTask->GetTaskName()) : ("N/A"));
        strSubOutput += SString("%s\n", pTask && pTask->GetSubTask() ? (pTask->GetSubTask()->GetTaskName()) : ("N/A"));

        pTask = man->GetTask(TASK_PRIORITY_PRIMARY);
        strOutput += SString("Primary: %s\n", pTask ? (pTask->GetTaskName()) : ("N/A"));
        strSubOutput += SString("%s\n", pTask && pTask->GetSubTask() ? (pTask->GetSubTask()->GetTaskName()) : ("N/A"));

        pTask = man->GetTask(TASK_PRIORITY_DEFAULT);
        strOutput += SString("Default: %s\n", pTask ? (pTask->GetTaskName()) : ("N/A"));
        strSubOutput += SString("%s\n", pTask && pTask->GetSubTask() ? (pTask->GetSubTask()->GetTaskName()) : ("N/A"));

        pTask = man->GetTaskSecondary(TASK_SECONDARY_ATTACK);
        strOutput += SString("Secondary Attack: %s\n", pTask ? (pTask->GetTaskName()) : ("N/A"));
        strSubOutput += SString("%s\n", pTask && pTask->GetSubTask() ? (pTask->GetSubTask()->GetTaskName()) : ("N/A"));

        pTask = man->GetTaskSecondary(TASK_SECONDARY_DUCK);
        strOutput += SString("Secondary Duck: %s\n", pTask ? (pTask->GetTaskName()) : ("N/A"));
        strSubOutput += SString("%s\n", pTask && pTask->GetSubTask() ? (pTask->GetSubTask()->GetTaskName()) : ("N/A"));

        pTask = man->GetTaskSecondary(TASK_SECONDARY_SAY);
        strOutput += SString("Secondary Say: %s\n", pTask ? (pTask->GetTaskName()) : ("N/A"));
        strSubOutput += SString("%s\n", pTask && pTask->GetSubTask() ? (pTask->GetSubTask()->GetTaskName()) : ("N/A"));

        pTask = man->GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX);
        strOutput += SString("Secondary Facial Complex: %s\n", pTask ? (pTask->GetTaskName()) : ("N/A"));
        strSubOutput += SString("%s\n", pTask && pTask->GetSubTask() ? (pTask->GetSubTask()->GetTaskName()) : ("N/A"));

        pTask = man->GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);
        strOutput += SString("Secondary Partial Anim: %s\n", pTask ? (pTask->GetTaskName()) : ("N/A"));
        strSubOutput += SString("%s\n", pTask && pTask->GetSubTask() ? (pTask->GetSubTask()->GetTaskName()) : ("N/A"));

        pTask = man->GetTaskSecondary(TASK_SECONDARY_IK);
        strOutput += SString("Secondary IK: %s\n", pTask ? (pTask->GetTaskName()) : ("N/A"));
        strSubOutput += SString("%s\n", pTask && pTask->GetSubTask() ? (pTask->GetSubTask()->GetTaskName()) : ("N/A"));

        m_pDisplayManager->DrawText2D(strOutput, CVector(0.05f, 0.5f, 0), 1.0f);
        m_pDisplayManager->DrawText2D(strSubOutput, CVector(0.5f, 0.5f, 0), 1.0f);
    }
}

int  iPlayerTask = 0;
void CClientGame::DrawPlayerDetails(CClientPlayer* pPlayer)
{
    // Get the info
    CControllerState cs;

    pPlayer->GetControllerState(cs);

    CVector vecPosition;
    pPlayer->GetPosition(vecPosition);

    float fRotation = pPlayer->GetCurrentRotation();
    float fCameraRotation = pPlayer->GetCameraRotation();
    float fHealth = pPlayer->GetHealth();
    bool  bIsDucked = pPlayer->IsDucked();
    bool  bWearingGoggles = pPlayer->IsWearingGoggles();
    bool  bInVehicle = pPlayer->GetOccupiedVehicle() != NULL;
    float fWeaponRange = 0.0f;

    unsigned char  ucWeapon = 0;
    unsigned char  ucWeaponState = 0;
    unsigned short usWeaponAmmo = 0;
    CWeapon*       pWeapon = pPlayer->GetWeapon(pPlayer->GetCurrentWeaponSlot());
    if (pWeapon)
    {
        ucWeapon = static_cast<unsigned char>(pWeapon->GetType());
        ucWeaponState = static_cast<unsigned char>(pWeapon->GetState());
        usWeaponAmmo = static_cast<unsigned short>(pWeapon->GetAmmoInClip());
        float        fSkill = pPlayer->GetStat(g_pGame->GetStats()->GetSkillStatIndex(pWeapon->GetType()));
        CWeaponStat* pWeaponInfo = g_pGame->GetWeaponStatManager()->GetWeaponStatsFromSkillLevel(pWeapon->GetType(), fSkill);
        fWeaponRange = pWeaponInfo->GetWeaponRange();
    }

    float fAimX, fAimY;
    pPlayer->GetAim(fAimX, fAimY);
    const CVector&       vecAimSource = pPlayer->GetAimSource();
    const CVector&       vecAimTarget = pPlayer->GetAimTarget();
    eVehicleAimDirection ucDrivebyAim = pPlayer->GetVehicleAimAnim();

    g_pCore->GetGraphics()->DrawLine3DQueued(vecAimSource, vecAimTarget, 1.0f, 0x10DE1212, eRenderStage::POST_GUI);
    g_pCore->GetGraphics()->DrawLine3DQueued(vecAimSource, vecAimTarget, 1.0f, 0x90DE1212, eRenderStage::POST_FX);

    CTask* pPrimaryTask = pPlayer->GetCurrentPrimaryTask();
    int    iPrimaryTask = pPrimaryTask ? pPrimaryTask->GetTaskType() : -1;

    // Copy the stuff
    SString strBuffer(
        "Orient:\n"
        "Position: %f %f %f\n"
        "Rotation/camera: %f %f\n"
        "Health: %f\n"
        "\n"
        "Keys:\n"
        "LeftShoulder1: %hi\n"
        "RightShoulder1: %hi\n"
        "ButtonSquare: %hi\n"
        "ButtonCross: %hi\n"
        "ButtonCircle: %hi\n"
        "ShockButtonL: %hi\n"
        "PedWalk: %hi\n"
        "VehicleMouseLook: %hi\n"
        "LeftStickX: %hi\n"
        "LeftStickY: %hi\n"
        "\n"
        "Misc:\n"
        "Primary task: %d\n"
        "Ducked: %u\n"
        "Goggles: %u\n"
        "In vehicle: %u\n"
        "Weapon: %u\n"
        "Weapon state: %u\n"
        "Weapon ammo: %u\n"
        "Weapon range: %f\n"
        "Aim: %f %f\n"
        "Aim source: %f %f %f\n"
        "Aim target: %f %f %f\n"
        "Driveby aim: %u\n"
        "Frozen: %u\n",
        vecPosition.fX, vecPosition.fY, vecPosition.fZ, fRotation, fCameraRotation, fHealth, cs.LeftShoulder1, cs.RightShoulder1, cs.ButtonSquare,
        cs.ButtonCross, cs.ButtonCircle, cs.ShockButtonL, cs.m_bPedWalk, cs.m_bVehicleMouseLook, cs.LeftStickX, cs.LeftStickY, iPrimaryTask, bIsDucked,
        bWearingGoggles, bInVehicle, ucWeapon, ucWeaponState, usWeaponAmmo, fWeaponRange, fAimX, fAimY, vecAimSource.fX, vecAimSource.fY, vecAimSource.fZ,
        vecAimTarget.fX, vecAimTarget.fY, vecAimTarget.fZ, ucDrivebyAim, pPlayer->IsFrozen());

    // Draw it
    m_pDisplayManager->DrawText2D(strBuffer, CVector(0.45f, 0.05f, 0), 1.0f, 0xFFFFFFFF);
}

void CClientGame::DrawWeaponsyncData(CClientPlayer* pPlayer)
{
    CWeapon* pWeapon = pPlayer->GetWeapon(pPlayer->GetCurrentWeaponSlot());

    if (pWeapon)
    {
        CVector vecSource;
        CVector vecTarget;

        // red line: Draw their synced aim line
        pPlayer->GetShotData(&vecSource, &vecTarget);
        g_pCore->GetGraphics()->DrawLine3DQueued(vecSource, vecTarget, 2.0f, 0x10DE1212, eRenderStage::POST_GUI);
        g_pCore->GetGraphics()->DrawLine3DQueued(vecSource, vecTarget, 2.0f, 0x90DE1212, eRenderStage::POST_FX);

        // green line: Set muzzle as origin and perform a collision test for the target
        CColPoint* pCollision;
        CVector    vecTemp;
        bool       bCollision = g_pGame->GetWorld()->ProcessLineOfSight(&vecSource, &vecTarget, &pCollision, NULL);
        if (pCollision)
        {
            if (bCollision)
            {
                CVector vecBullet = pCollision->GetPosition() - vecSource;
                vecBullet.Normalize();
                CVector vecTarget = vecSource + (vecBullet * 200);
                g_pCore->GetGraphics()->DrawLine3DQueued(vecSource, vecTarget, 0.5f, 0x1012DE12, eRenderStage::POST_GUI);
                g_pCore->GetGraphics()->DrawLine3DQueued(vecSource, vecTarget, 0.5f, 0x9012DE12, eRenderStage::POST_FX);
            }
            pCollision->Destroy();
        }

        if (m_pLocalPlayer != pPlayer)
        {
            // Draw information about their weapon state, total ammo and ammo in clip
            CVector vecScreenPosition;
            CVector vecPosition;

            pPlayer->GetPosition(vecPosition);

            vecPosition.fZ += 1.0f;
            g_pCore->GetGraphics()->CalcScreenCoors(&vecPosition, &vecScreenPosition);

            SString strTemp;
            int     yoffset;

            yoffset = 0;
            strTemp.Format("Ammo in clip: %d", pWeapon->GetAmmoInClip());
            g_pCore->GetGraphics()->DrawString((int)vecScreenPosition.fX + 1, (int)vecScreenPosition.fY + 1 + yoffset, (int)vecScreenPosition.fX + 1,
                                               (int)vecScreenPosition.fY + 1 + yoffset, COLOR_ARGB(255, 255, 255, 255), strTemp, 1.0f, 1.0f,
                                               DT_NOCLIP | DT_CENTER);
            g_pCore->GetGraphics()->DrawString((int)vecScreenPosition.fX, (int)vecScreenPosition.fY + yoffset, (int)vecScreenPosition.fX,
                                               (int)vecScreenPosition.fY + yoffset, COLOR_ARGB(255, 0, 0, 0), strTemp, 1.0f, 1.0f, DT_NOCLIP | DT_CENTER);

            yoffset = 15;
            strTemp.Format("State: %d", pWeapon->GetState());
            g_pCore->GetGraphics()->DrawString((int)vecScreenPosition.fX + 1, (int)vecScreenPosition.fY + 1 + yoffset, (int)vecScreenPosition.fX + 1,
                                               (int)vecScreenPosition.fY + 1 + yoffset, COLOR_ARGB(255, 255, 255, 255), strTemp, 1.0f, 1.0f,
                                               DT_NOCLIP | DT_CENTER);
            g_pCore->GetGraphics()->DrawString((int)vecScreenPosition.fX, (int)vecScreenPosition.fY + yoffset, (int)vecScreenPosition.fX,
                                               (int)vecScreenPosition.fY + yoffset, COLOR_ARGB(255, 0, 0, 0), strTemp, 1.0f, 1.0f, DT_NOCLIP | DT_CENTER);
        }
    }
}

void CClientGame::UpdateMimics()
{
    // Got a local player?
    if (m_pLocalPlayer)
    {
        unsigned char ucWeaponType = 0;
        unsigned char ucWeaponState = 0;
        unsigned long ulWeaponAmmoInClip = 0;
        eWeaponSlot   weaponSlot = WEAPONSLOT_TYPE_UNARMED;

        CWeapon* pPlayerWeapon = m_pLocalPlayer->GetWeapon(m_pLocalPlayer->GetCurrentWeaponSlot());
        if (pPlayerWeapon)
        {
            ucWeaponType = static_cast<unsigned char>(pPlayerWeapon->GetType());
            ucWeaponState = static_cast<unsigned char>(pPlayerWeapon->GetState());
            weaponSlot = pPlayerWeapon->GetSlot();
            ulWeaponAmmoInClip = pPlayerWeapon->GetAmmoInClip();
        }

        // Simulate lag (or not)
        if (!m_bMimicLag || CClientTime::GetTime() >= m_ulLastMimicLag + 200)            // TICK_RATE )
        {
            m_ulLastMimicLag = CClientTime::GetTime();

            // Grab the local data
            CControllerState Controller;
            m_pLocalPlayer->GetControllerState(Controller);
            CVector vecPosition;
            m_pLocalPlayer->GetPosition(vecPosition);
            float   fRotation = m_pLocalPlayer->GetCurrentRotation();
            CVector vecMoveSpeed;
            m_pLocalPlayer->GetMoveSpeed(vecMoveSpeed);
            float fHealth = m_pLocalPlayer->GetHealth();
            float fArmor = m_pLocalPlayer->GetArmor();
            float fCameraRotation = g_pGame->GetCamera()->GetCameraRotation();
            bool  bDucked = m_pLocalPlayer->IsDucked();
            bool  bWearingGoggles = m_pLocalPlayer->IsWearingGoggles(true);
            bool  bHasJetpack = m_pLocalPlayer->HasJetPack();
            bool  bChoking = m_pLocalPlayer->IsChoking();
            bool  bSunbathing = m_pLocalPlayer->IsSunbathing();
            bool  bDoingDriveby = m_pLocalPlayer->IsDoingGangDriveby();
            bool  bStealthAiming = m_pLocalPlayer->IsStealthAiming();

            // Is the current weapon goggles (44 or 45) or a camera (43), or a detonator (40), don't apply the fire key
            if (weaponSlot == 11 || weaponSlot == 12 || ucWeaponType == 43)
                Controller.ButtonCircle = 0;

            CClientVehicle* pVehicle = m_pLocalPlayer->GetOccupiedVehicle();
            unsigned int    uiSeat = m_pLocalPlayer->GetOccupiedVehicleSeat();

            CShotSyncData* pShotSync = g_pMultiplayer->GetLocalShotSyncData();
            CVector        vecOrigin, vecTarget;
            m_pLocalPlayer->GetShotData(&vecOrigin, &vecTarget);
            float                fAimX = pShotSync->m_fArmDirectionX;
            float                fAimY = pShotSync->m_fArmDirectionY;
            eVehicleAimDirection cVehicleAimDirection = pShotSync->m_cInVehicleAimDirection;
            bool                 bAkimboUp = g_pMultiplayer->GetAkimboTargetUp();

            /*
            static CClientMarker *pOriginCorona = NULL, *pTargetCorona = NULL;
            if ( pOriginCorona == NULL )
            {
                pOriginCorona = new CClientMarker ( m_pManager, 1337, MARKER_CORONA );
                pOriginCorona->SetColor ( 0, 255, 0, 255 );
                pOriginCorona->SetSize ( 0.5f );
            }
            else
                pOriginCorona->SetPosition ( vecOrigin );

            if ( pTargetCorona == NULL )
            {
                pTargetCorona = new CClientMarker ( m_pManager, 1338, MARKER_CORONA );
                pTargetCorona->SetColor ( 255, 0, 0, 255 );
                pTargetCorona->SetSize ( 0.5f );
            }
            else
                pTargetCorona->SetPosition ( vecTarget );*/

            // Apply this to each of our mimic players
            unsigned int                         uiMimicIndex = 0;
            list<CClientPlayer*>::const_iterator iterMimics = m_Mimics.begin();
            for (; iterMimics != m_Mimics.end(); ++iterMimics, ++uiMimicIndex)
            {
                vecPosition.fX += 4.0f;
                vecOrigin.fX += 4.0f;
                vecTarget.fX += 4.0f;

                CClientPlayer* pMimic = *iterMimics;

                pMimic->SetHealth(fHealth);
                pMimic->LockHealth(fHealth);
                pMimic->SetArmor(fArmor);
                pMimic->LockArmor(fArmor);
                pMimic->SetWearingGoggles(bWearingGoggles);
                pMimic->SetHasJetPack(bHasJetpack);
                pMimic->SetChoking(bChoking);
                pMimic->SetSunbathing(bSunbathing);
                pMimic->SetDoingGangDriveby(bDoingDriveby);
                pMimic->SetStealthAiming(bStealthAiming);

                Controller.ShockButtonL = 0;

                if (m_bMimicLag)
                {
                    pMimic->SetTargetPosition(vecPosition, TICK_RATE);
                    pMimic->SetMoveSpeed(vecMoveSpeed);
                    pMimic->SetControllerState(Controller);
                    pMimic->SetTargetRotation(fRotation);
                    pMimic->SetCameraRotation(fCameraRotation);
                    pMimic->Duck(bDucked);
                }
                else
                {
                    pMimic->SetPosition(vecPosition);
                    pMimic->SetMoveSpeed(vecMoveSpeed);
                    pMimic->SetTargetRotation(fRotation);
                    pMimic->SetCameraRotation(fCameraRotation);
                    pMimic->SetControllerState(Controller);
                    pMimic->Duck(bDucked);
                }

                if (ucWeaponType != 0)
                {
                    if (ucWeaponType == 44 || ucWeaponType == 45)
                    {
                        Controller.ButtonCircle = 0;
                    }

                    if (m_bMimicLag)
                    {
                        pMimic->SetAimInterpolated(TICK_RATE, fAimX, fAimY, bAkimboUp, cVehicleAimDirection);
                        pMimic->SetTargetTarget(TICK_RATE, vecOrigin, vecTarget);

                        pMimic->AddChangeWeapon(TICK_RATE, weaponSlot, (unsigned char)ulWeaponAmmoInClip);
                    }
                    else
                    {
                        CWeapon*    pPlayerWeapon = pMimic->GetWeapon();
                        eWeaponType eCurrentWeapon = static_cast<eWeaponType>(ucWeaponType);
                        if ((pPlayerWeapon && pPlayerWeapon->GetType() != eCurrentWeapon) || !pPlayerWeapon)
                        {
                            pPlayerWeapon = pMimic->GiveWeapon(eCurrentWeapon, ulWeaponAmmoInClip);
                            if (pPlayerWeapon)
                            {
                                pPlayerWeapon->SetAsCurrentWeapon();
                            }
                        }

                        if (pPlayerWeapon)
                        {
                            pPlayerWeapon->SetAmmoTotal(9999);
                            pPlayerWeapon->SetAmmoInClip(ulWeaponAmmoInClip);
                            pPlayerWeapon->SetState(static_cast<eWeaponState>(ucWeaponState));
                        }
                        pMimic->SetAimInterpolated(TICK_RATE, fAimX, fAimY, bAkimboUp, cVehicleAimDirection);
                        pMimic->SetTargetTarget(TICK_RATE, vecOrigin, vecTarget);
                    }
                }
                else
                {
                    pMimic->SetCurrentWeaponSlot(WEAPONSLOT_TYPE_UNARMED);
                }

                CClientVehicle* pMimicVehicle = pMimic->GetOccupiedVehicle();
                if (pVehicle)
                {
                    unsigned int uiModel;
                    CVector      vecPosition, vecRotationDegrees;
                    CVector      vecMoveSpeed, vecMoveSpeedMeters, vecTurnSpeed, vecVelocity;
                    float        fHealth;

                    uiModel = pVehicle->GetModel();
                    pVehicle->GetPosition(vecPosition);
                    pVehicle->GetRotationDegrees(vecRotationDegrees);
                    pVehicle->GetMoveSpeed(vecMoveSpeed);
                    pVehicle->GetTurnSpeed(vecTurnSpeed);
                    fHealth = pVehicle->GetHealth();

                    if (pMimicVehicle && pMimicVehicle->GetModel() != uiModel)
                    {
                        delete pMimicVehicle;
                        pMimicVehicle = NULL;
                    }

                    vecPosition.fX += ((float)(uiMimicIndex + 1) * 10.0f);

                    if (pMimicVehicle == NULL)
                    {
                        pMimicVehicle = new CDeathmatchVehicle(m_pManager, m_pUnoccupiedVehicleSync, INVALID_ELEMENT_ID, uiModel, 0, 0);
                        pMimicVehicle->SetPosition(vecPosition);

                        const SSlotStates& usUpgrades = pVehicle->GetUpgrades()->GetSlotStates();
                        for (unsigned char uc = 0; uc < VEHICLE_UPGRADE_SLOTS; uc++)
                        {
                            if (usUpgrades[uc])
                            {
                                pMimicVehicle->GetUpgrades()->AddUpgrade(usUpgrades[uc], true);
                            }
                        }

                        m_vecLastMimicPos = vecPosition;
                    }

                    if (m_bMimicLag)
                    {
                        pMimicVehicle->SetTargetPosition(vecPosition, TICK_RATE);
                        pMimicVehicle->SetTargetRotation(vecRotationDegrees, TICK_RATE);
                        pMimicVehicle->SetMoveSpeed(vecMoveSpeed);
                        pMimicVehicle->SetTurnSpeed(vecTurnSpeed);
                    }
                    else
                    {
                        pMimicVehicle->SetPosition(vecPosition);
                        pMimicVehicle->SetRotationDegrees(vecRotationDegrees);
                        pMimicVehicle->SetMoveSpeed(vecMoveSpeed);
                        pMimicVehicle->SetTurnSpeed(vecTurnSpeed);
                    }
                    pMimicVehicle->SetHealth(fHealth);
                    if (pMimic->GetOccupiedVehicle() != pMimicVehicle)
                        pMimic->WarpIntoVehicle(pMimicVehicle, uiSeat);

                    unsigned int    uiTrailerLoop = 0;
                    CClientVehicle* pTrailer = pVehicle->GetTowedVehicle();
                    CClientVehicle* pMimicTrailer = NULL;
                    while (pTrailer)
                    {
                        uiModel = pTrailer->GetModel();
                        pTrailer->GetPosition(vecPosition);
                        pTrailer->GetRotationDegrees(vecRotationDegrees);
                        pTrailer->GetMoveSpeed(vecMoveSpeed);
                        pTrailer->GetTurnSpeed(vecTurnSpeed);
                        fHealth = pTrailer->GetHealth();

                        pMimicTrailer = DynamicCast<CClientVehicle>(CElementIDs::GetElement(static_cast<ElementID>(450 + uiMimicIndex + uiTrailerLoop)));

                        if (pMimicTrailer && pMimicTrailer->GetModel() != uiModel)
                        {
                            delete pMimicTrailer;
                            pMimicTrailer = NULL;
                        }

                        if (!pMimicTrailer)
                        {
                            pMimicTrailer = new CDeathmatchVehicle(m_pManager, m_pUnoccupiedVehicleSync,
                                                                   static_cast<ElementID>(450 + uiMimicIndex + uiTrailerLoop), uiModel, 0, 0);
                            pMimicVehicle->SetTowedVehicle(pMimicTrailer);
                        }

                        pTrailer = pTrailer->GetTowedVehicle();
                        pMimicTrailer = pMimicTrailer->GetTowedVehicle();
                        uiTrailerLoop++;
                    }
                    if (pMimicTrailer)
                    {
                        if (pMimicTrailer->GetTowedByVehicle())
                        {
                            pMimicTrailer->GetTowedByVehicle()->SetTowedVehicle(NULL);
                        }
                    }
                }
                else if (pMimicVehicle)
                {
                    pMimic->RemoveFromVehicle();
                    delete pMimicVehicle;
                }
            }
        }
    }
}

void CClientGame::DoPaintballs()
{
    if (m_pLocalPlayer)
    {
        CVector vecOrigin, vecTarget;
        m_pLocalPlayer->GetShotData(&vecOrigin, &vecTarget);

        CColPoint* pCollision = NULL;
        CEntity*   pCollisionEntity = NULL;
        m_pLocalPlayer->WorldIgnore(true);
        bool bCollision = g_pGame->GetWorld()->ProcessLineOfSight(&vecOrigin, &vecTarget, &pCollision, &pCollisionEntity);
        m_pLocalPlayer->WorldIgnore(false);

        static list<CClientMarker*> paintBalls;
        if (paintBalls.size() >= 5)
        {
            CClientMarker* pCorona = paintBalls.back();
            delete pCorona;
            paintBalls.pop_back();
        }

        CClientMarker* pCorona = new CClientMarker(m_pManager, INVALID_ELEMENT_ID, CClientMarker::MARKER_CORONA);
        paintBalls.push_front(pCorona);
        pCorona->SetSize(0.2f);
        if (bCollision && pCollision)
        {
            pCorona->SetPosition(pCollision->GetPosition());
            pCorona->SetColor(SColorRGBA(255, 0, 0, 255));
        }
        else
        {
            pCorona->SetPosition(vecTarget);
            pCorona->SetColor(SColorRGBA(255, 255, 0, 255));
        }

        // Destroy the colpoint
        if (pCollision)
        {
            pCollision->Destroy();
        }
    }
}

#endif

void CClientGame::QuitPlayer(CClientPlayer* pPlayer, eQuitReason Reason)
{
    // Get the nick pointer and echo the quit message to the chat
    const char* szNick = pPlayer->GetNick();
    const char* szReason = "Unknown";
    switch (Reason)
    {
        case QUIT_QUIT:
            szReason = "Quit";
            break;
        case QUIT_KICK:
            szReason = "Kicked";
            break;
        case QUIT_BAN:
            szReason = "Banned";
            break;
        case QUIT_CONNECTION_DESYNC:
            szReason = "Bad Connection";
            break;
        case QUIT_TIMEOUT:
            szReason = "Timed Out";
            break;
    }

        // In debug, make sure we don't look at this player's details
#ifdef MTA_DEBUG
    if (m_pShowPlayer == pPlayer)
    {
        m_pShowPlayer = NULL;
    }
#endif

    // Call our onClientPlayerQuit event
    CLuaArguments Arguments;
    Arguments.PushString(szReason);
    pPlayer->CallEvent("onClientPlayerQuit", Arguments, true);

    // Detach the camera from this player if we're watching them
    m_pManager->GetCamera()->UnreferencePlayer(pPlayer);

    // Create a list containing local player and peds we sync
    CMappedList<CClientPed*> listOfPeds(g_pClientGame->GetPedSync()->GetList());
    listOfPeds.push_front(g_pClientGame->GetLocalPlayer());
    for (auto iter = listOfPeds.begin(); iter != listOfPeds.end(); ++iter)
    {
        CClientPed* pPed = *iter;
        // Was this player jacking us?
        if (pPed->m_bIsGettingJacked && pPed->m_pGettingJackedBy == pPlayer)
        {
            pPed->ResetVehicleInOut();
            pPed->RemoveFromVehicle(false);
            pPed->SetVehicleInOutState(VEHICLE_INOUT_NONE);
        }
    }

    // Delete the player
    delete pPlayer;
}

void CClientGame::Event_OnIngame()
{
    // Unpause the game
    g_pGame->Pause(false);

    // Disable parts of the Hud
    CHud* pHud = g_pGame->GetHud();
    pHud->SetComponentVisible(HUD_HELP_TEXT, false);
    pHud->SetComponentVisible(HUD_VITAL_STATS, false);
    pHud->SetComponentVisible(HUD_AREA_NAME, false);

    g_pMultiplayer->DeleteAndDisableGangTags();

    g_pGame->GetBuildingRemoval()->ClearRemovedBuildingLists();
    g_pGame->GetWorld()->SetOcclusionsEnabled(true);

    g_pGame->ResetModelLodDistances();
    g_pGame->ResetModelFlags();
    g_pGame->ResetAlphaTransparencies();
    g_pGame->ResetModelTimes();

    // Make sure we can access all areas
    g_pGame->GetStats()->ModifyStat(CITIES_PASSED, 2.0);

    // This is to prevent the 'white arrows in checkpoints' bug (#274)
    CVector pos(0, 0, 0);
    g_pGame->Get3DMarkers()->CreateMarker(87654, (e3DMarkerType)5, &pos, 1, 0.2f, 0, 0, 0, 0);

    // Stop us getting 4 stars if we visit the SF or LV
    // g_pGame->GetPlayerInfo()->GetWanted()->SetMaximumWantedLevel ( 0 );
    g_pGame->GetPlayerInfo()->GetWanted()->SetWantedLevel(0);

    // Reset anything from last game
    ResetMapInfo();
    g_pGame->GetWaterManager()->Reset();            // Deletes all custom water elements, ResetMapInfo only reverts changes to water level
    g_pGame->GetWaterManager()->SetWaterDrawnLast(true);
    m_pCamera->SetCameraClip(true, true);

    // Deallocate all custom models
    m_pManager->GetModelManager()->RemoveAll();

    // Create a local player for us
    m_pLocalPlayer = new CClientPlayer(m_pManager, m_LocalID, true);

    // Set our parent the root entity
    m_pLocalPlayer->SetParent(m_pRootEntity);

    // Give the local player our nickname
    m_pLocalPlayer->SetNick(m_strLocalNick);

    // Freeze the player at some location we won't see
    m_pLocalPlayer->SetHealth(100);
    m_pLocalPlayer->SetPosition(CVector(0, 0, 0));
    m_pLocalPlayer->SetFrozen(true);
    m_pLocalPlayer->ResetInterpolation();

    // Reset him
    m_pLocalPlayer->ResetStats();

    // Make sure we never get tired
    g_pGame->GetPlayerInfo()->SetDoesNotGetTired(true);

    // Tell doggy we got the game running
    WatchDogCompletedSection("L1");
}

void CClientGame::Event_OnIngameAndConnected()
{
    m_ulVerifyTimeStart = CClientTime::GetTime();

    // Keep criminal records of how many times they've connected to servers
    SetApplicationSettingInt("times-connected", GetApplicationSettingInt("times-connected") + 1);
    if (m_ServerType == SERVER_TYPE_EDITOR)
        SetApplicationSettingInt("times-connected-editor", GetApplicationSettingInt("times-connected-editor") + 1);

    // Notify the server telling we're ingame
    m_pNetAPI->RPC(PLAYER_INGAME_NOTICE);
}

void CClientGame::SetupGlobalLuaEvents()
{
    // Setup onClientPaste event
    m_Delegate.connect(g_pCore->GetKeyBinds()->OnPaste,
                       [this](const SString& clipboardText)
                       {
                           // Don't trigger if main menu or console is open or the cursor is not visible
                           if (!AreCursorEventsEnabled() || g_pCore->IsMenuVisible() || g_pCore->GetConsole()->IsInputActive())
                               return;

                           // Also don't trigger if remote web browser view is focused
                           CWebViewInterface* pFocusedBrowser = g_pCore->IsWebCoreLoaded() ? g_pCore->GetWebCore()->GetFocusedWebView() : nullptr;
                           if (pFocusedBrowser && !pFocusedBrowser->IsLocal())
                               return;

                           // Call event now
                           CLuaArguments args;
                           args.PushString(clipboardText);
                           m_pRootEntity->CallEvent("onClientPaste", args, false);
                       });
}

bool CClientGame::StaticBreakTowLinkHandler(CVehicle* pTowingVehicle)
{
    return g_pClientGame->BreakTowLinkHandler(pTowingVehicle);
}

void CClientGame::StaticDrawRadarAreasHandler()
{
    g_pClientGame->DrawRadarAreasHandler();
}

bool CClientGame::StaticDamageHandler(CPed* pDamagePed, CEventDamage* pEvent)
{
    return g_pClientGame->DamageHandler(pDamagePed, pEvent);
}

void CClientGame::StaticDeathHandler(CPed* pKilledPed, unsigned char ucDeathReason, unsigned char ucBodyPart)
{
    g_pClientGame->DeathHandler(pKilledPed, ucDeathReason, ucBodyPart);
}

void CClientGame::StaticFireHandler(CFire* pFire)
{
    g_pClientGame->FireHandler(pFire);
}

void CClientGame::StaticRender3DStuffHandler()
{
    g_pClientGame->Render3DStuffHandler();
}

void CClientGame::StaticPreRenderSkyHandler()
{
    g_pClientGame->PreRenderSkyHandler();
}

void CClientGame::StaticRenderHeliLightHandler()
{
    g_pClientGame->GetManager()->GetPointLightsManager()->RenderHeliLightHandler();
}

void CClientGame::StaticRenderEverythingBarRoadsHandler()
{
    g_pClientGame->GetModelRenderer()->Render();
}

bool CClientGame::StaticChokingHandler(unsigned char ucWeaponType)
{
    return g_pClientGame->ChokingHandler(ucWeaponType);
}

void CClientGame::StaticCAnimBlendAssocDestructorHandler(CAnimBlendAssociationSAInterface* pThis)
{
    g_pClientGame->CAnimBlendAssocDestructorHandler(pThis);
}

CAnimBlendAssociationSAInterface* CClientGame::StaticAddAnimationHandler(RpClump* pClump, AssocGroupId animGroup, AnimationId animID)
{
    return g_pClientGame->AddAnimationHandler(pClump, animGroup, animID);
}

CAnimBlendAssociationSAInterface* CClientGame::StaticAddAnimationAndSyncHandler(RpClump* pClump, CAnimBlendAssociationSAInterface* pAnimAssocToSyncWith,
                                                                                AssocGroupId animGroup, AnimationId animID)
{
    return g_pClientGame->AddAnimationAndSyncHandler(pClump, pAnimAssocToSyncWith, animGroup, animID);
}

bool CClientGame::StaticAssocGroupCopyAnimationHandler(CAnimBlendAssociationSAInterface* pAnimAssoc, RpClump* pClump,
                                                       CAnimBlendAssocGroupSAInterface* pAnimAssocGroup, eAnimID animID)
{
    return g_pClientGame->AssocGroupCopyAnimationHandler(pAnimAssoc, pClump, pAnimAssocGroup, animID);
}

bool CClientGame::StaticBlendAnimationHierarchyHandler(CAnimBlendAssociationSAInterface* pAnimAssoc, CAnimBlendHierarchySAInterface** pOutAnimHierarchy,
                                                       int* pFlags, RpClump* pClump)
{
    return g_pClientGame->BlendAnimationHierarchyHandler(pAnimAssoc, pOutAnimHierarchy, pFlags, pClump);
}

void CClientGame::StaticPreWorldProcessHandler()
{
    g_pClientGame->PreWorldProcessHandler();
}

void CClientGame::StaticPostWorldProcessHandler()
{
    g_pClientGame->PostWorldProcessHandler();
}

void CClientGame::StaticPostWorldProcessPedsAfterPreRenderHandler()
{
    g_pClientGame->PostWorldProcessPedsAfterPreRenderHandler();
}

void CClientGame::StaticPreFxRenderHandler()
{
    g_pCore->OnPreFxRender();
}

void CClientGame::StaticPostColorFilterRenderHandler()
{
    g_pCore->OnPostColorFilterRender();
}

void CClientGame::StaticPreHudRenderHandler()
{
    g_pCore->OnPreHUDRender();
}

bool CClientGame::StaticProcessCollisionHandler(CEntitySAInterface* pThisInterface, CEntitySAInterface* pOtherInterface)
{
    return g_pClientGame->ProcessCollisionHandler(pThisInterface, pOtherInterface);
}

bool CClientGame::StaticVehicleCollisionHandler(CVehicleSAInterface*& pCollidingVehicle, CEntitySAInterface* pCollidedVehicle, int iModelIndex,
                                                float fDamageImpulseMag, float fCollidingDamageImpulseMag, uint16 usPieceType, CVector vecCollisionPos,
                                                CVector vecCollisionVelocity, bool isProjectile)
{
    return g_pClientGame->VehicleCollisionHandler(pCollidingVehicle, pCollidedVehicle, iModelIndex, fDamageImpulseMag, fCollidingDamageImpulseMag, usPieceType,
                                                  vecCollisionPos, vecCollisionVelocity, isProjectile);
}

bool CClientGame::StaticVehicleDamageHandler(CEntitySAInterface* pVehicleInterface, float fLoss, CEntitySAInterface* pAttackerInterface, eWeaponType weaponType,
                                             const CVector& vecDamagePos, uchar ucTyre)
{
    return g_pClientGame->VehicleDamageHandler(pVehicleInterface, fLoss, pAttackerInterface, weaponType, vecDamagePos, ucTyre);
}

bool CClientGame::StaticHeliKillHandler(CVehicleSAInterface* pHeliInterface, CEntitySAInterface* pHitInterface)
{
    return g_pClientGame->HeliKillHandler(pHeliInterface, pHitInterface);
}

bool CClientGame::StaticObjectDamageHandler(CObjectSAInterface* pObjectInterface, float fLoss, CEntitySAInterface* pAttackerInterface)
{
    return g_pClientGame->ObjectDamageHandler(pObjectInterface, fLoss, pAttackerInterface);
}

bool CClientGame::StaticObjectBreakHandler(CObjectSAInterface* pObjectInterface, CEntitySAInterface* pAttackerInterface)
{
    return g_pClientGame->ObjectBreakHandler(pObjectInterface, pAttackerInterface);
}

bool CClientGame::StaticWaterCannonHandler(CVehicleSAInterface* pCannonVehicle, CPedSAInterface* pHitPed)
{
    return g_pClientGame->WaterCannonHitHandler(pCannonVehicle, pHitPed);
}

bool CClientGame::StaticVehicleFellThroughMapHandler(CVehicleSAInterface* pVehicle)
{
    return g_pClientGame->VehicleFellThroughMapHandler(pVehicle);
}

void CClientGame::StaticGameObjectDestructHandler(CEntitySAInterface* pObject)
{
    g_pClientGame->GameObjectDestructHandler(pObject);
}

void CClientGame::StaticGameVehicleDestructHandler(CEntitySAInterface* pVehicle)
{
    g_pClientGame->GameVehicleDestructHandler(pVehicle);
}

void CClientGame::StaticGamePlayerDestructHandler(CEntitySAInterface* pPlayer)
{
    g_pClientGame->GamePlayerDestructHandler(pPlayer);
}

void CClientGame::StaticGameProjectileDestructHandler(CEntitySAInterface* pProjectile)
{
    g_pClientGame->GameProjectileDestructHandler(pProjectile);
}

void CClientGame::StaticGameModelRemoveHandler(ushort usModelId)
{
    g_pClientGame->GameModelRemoveHandler(usModelId);
}

void CClientGame::StaticGameRunNamedAnimDestructorHandler(class CTaskSimpleRunNamedAnimSAInterface* pTask)
{
    g_pClientGame->GameRunNamedAnimDestructorHandler(pTask);
}

void CClientGame::StaticGameEntityRenderHandler(CEntitySAInterface* pGameEntity)
{
    if (pGameEntity)
    {
        CPools* pPools = g_pGame->GetPools();
        // Map to client entity and pass to the texture replacer
        CClientEntity* pClientEntity = pPools->GetClientEntity((DWORD*)pGameEntity);
        if (pClientEntity)
        {
            int    iTypeMask;
            ushort usModelId = 0xFFFF;
            switch (pClientEntity->GetType())
            {
                case CCLIENTPED:
                case CCLIENTPLAYER:
                    iTypeMask = TYPE_MASK_PED;
                    usModelId = (ushort) static_cast<CClientPed*>(pClientEntity)->GetModel();
                    break;
                case CCLIENTVEHICLE:
                    iTypeMask = TYPE_MASK_VEHICLE;
                    break;
                case CCLIENTOBJECT:
                    iTypeMask = TYPE_MASK_OBJECT;
                    break;
                default:
                    iTypeMask = TYPE_MASK_OTHER;
                    break;
            }
            g_pGame->GetRenderWare()->SetRenderingClientEntity(pClientEntity, usModelId, iTypeMask);
            return;
        }
    }

    g_pGame->GetRenderWare()->SetRenderingClientEntity(NULL, 0xFFFF, TYPE_MASK_WORLD);
}

void CClientGame::StaticTaskSimpleBeHitHandler(CPedSAInterface* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId)
{
    g_pClientGame->TaskSimpleBeHitHandler(pPedAttacker, hitBodyPart, hitBodySide, weaponId);
}

void CClientGame::StaticFxSystemDestructionHandler(void* pFxSAInterface)
{
    g_pClientGame->GetManager()->GetEffectManager()->SAEffectDestroyed(pFxSAInterface);
}

AnimationId CClientGame::StaticDrivebyAnimationHandler(AnimationId animGroup, AssocGroupId animId)
{
    return g_pClientGame->DrivebyAnimationHandler(animGroup, animId);
}

void CClientGame::StaticPedStepHandler(CPedSAInterface* pPed, bool bFoot)
{
    return g_pClientGame->PedStepHandler(pPed, bFoot);
}

void CClientGame::StaticVehicleWeaponHitHandler(SVehicleWeaponHitEvent& event)
{
    g_pClientGame->VehicleWeaponHitHandler(event);
}

void CClientGame::StaticAudioZoneRadioSwitchHandler(DWORD dwStationID)
{
    g_pClientGame->AudioZoneRadioSwitchHandler(dwStationID);
}

void CClientGame::DrawRadarAreasHandler()
{
    m_pRadarAreaManager->DoPulse();
}

bool CClientGame::BreakTowLinkHandler(CVehicle* pTowedVehicle)
{
    CPools*                    pPools = g_pGame->GetPools();
    SClientEntity<CVehicleSA>* pVehicleClientEntity = pPools->GetVehicle((DWORD*)pTowedVehicle->GetInterface());
    if (pVehicleClientEntity)
    {
        CClientVehicle* pVehicle = reinterpret_cast<CClientVehicle*>(pVehicleClientEntity->pClientEntity);
        if (pVehicle)
        {
            // Check if this is a legal break
            bool bLegal =
                ((pVehicle->GetControllingPlayer() == m_pLocalPlayer) || (m_pUnoccupiedVehicleSync->Exists(static_cast<CDeathmatchVehicle*>(pVehicle))));

            // Not a legal break?
            if (!bLegal)
            {
                // Save the time it broke (used in UpdateTrailers)
                pVehicle->SetIllegalTowBreakTime(GetTickCount32());
            }
        }
    }

    // Allow it to break
    return true;
}

void CClientGame::FireHandler(CFire* pFire)
{
    // Disable spreading fires
    pFire->SetNumGenerationsAllowed(0);
}

void CClientGame::ProjectileInitiateHandler(CClientProjectile* pProjectile)
{
    if (pProjectile->IsLocal())
    {
        // Did the local player create this projectile?
        if (m_pLocalPlayer && pProjectile->GetCreator() == m_pLocalPlayer)
        {
            // Physics says our projectile should start off at our velocity
            CVector vecVelocity, vecPlayerVelocity;
            pProjectile->GetVelocity(vecVelocity);
            m_pLocalPlayer->GetMoveSpeed(vecPlayerVelocity);
            vecVelocity += vecPlayerVelocity;
            pProjectile->SetVelocity(vecVelocity);
        }

        SendProjectileSync(pProjectile);
    }

    // Renew the interior and dimension
    if (pProjectile->GetCreator())
    {
        pProjectile->SetInterior(pProjectile->GetCreator()->GetInterior());
        pProjectile->SetDimension(pProjectile->GetCreator()->GetDimension());
    }

    // Validate the projectile for our element tree
    pProjectile->SetParent(m_pRootEntity);

    // Call our creation event
    CLuaArguments Arguments;
    Arguments.PushElement(pProjectile->GetCreator());
    pProjectile->CallEvent("onClientProjectileCreation", Arguments, true);
}

void CClientGame::Render3DStuffHandler()
{
}

void CClientGame::PreRenderSkyHandler()
{
    g_pCore->GetGraphics()->GetRenderItemManager()->PreDrawWorld();
}

void CClientGame::PreWorldProcessHandler()
{
}

void CClientGame::PostWorldProcessHandler()
{
    m_pManager->GetMarkerManager()->DoPulse();
    m_pManager->GetPointLightsManager()->DoPulse();
    m_pManager->GetObjectManager()->DoPulse();

    double dTimeSlice = m_TimeSliceTimer.Get();
    m_TimeSliceTimer.Reset();
    m_uiFrameCount++;

    // Call onClientPreRender LUA event
    CLuaArguments Arguments;
    Arguments.PushNumber(dTimeSlice);
    m_pRootEntity->CallEvent("onClientPreRender", Arguments, false);
}

void CClientGame::PostWorldProcessPedsAfterPreRenderHandler()
{
    CLuaArguments Arguments;
    m_pRootEntity->CallEvent("onClientPedsProcessed", Arguments, false);

    g_pClientGame->GetModelRenderer()->Update();
}

void CClientGame::IdleHandler()
{
    // If we are minimized we do the pulsing here
    if (g_pCore->IsWindowMinimized())
    {
        if (!m_bWasMinimized)
        {
            m_bWasMinimized = true;
            // Call onClientMinimize LUA event
            CLuaArguments Arguments;
            m_pRootEntity->CallEvent("onClientMinimize", Arguments, false);

            bool bMuteAll = g_pCore->GetCVars()->GetValue<bool>("mute_master_when_minimized");

            // Apply mute on minimize options
            if (bMuteAll || g_pCore->GetCVars()->GetValue<bool>("mute_radio_when_minimized"))
                g_pGame->GetAudioEngine()->SetMusicMasterVolume(0);

            if (bMuteAll || g_pCore->GetCVars()->GetValue<bool>("mute_sfx_when_minimized"))
                g_pGame->GetAudioEngine()->SetEffectsMasterVolume(0);

            if (bMuteAll || g_pCore->GetCVars()->GetValue<bool>("mute_mta_when_minimized"))
                m_pManager->GetSoundManager()->SetMinimizeMuted(true);
        }
    }

    // Ensure dummy progress graphic will be displayed when using alt pulse order
    g_pCore->SetDummyProgressUpdateAlways(true);

    // Extrapolation test - Change the pulse order to reduce latency (Has side effects for peds)
    DoPulses2(true);

    g_pCore->SetDummyProgressUpdateAlways(false);
}

bool CClientGame::ChokingHandler(unsigned char ucWeaponType)
{
    if (!m_pLocalPlayer)
        return true;
    CLuaArguments Arguments;
    Arguments.PushNumber(ucWeaponType);
    return m_pLocalPlayer->CallEvent("onClientPlayerChoke", Arguments, true);
}

void CClientGame::CAnimBlendAssocDestructorHandler(CAnimBlendAssociationSAInterface* pThis)
{
    // printf("CClientGame::CAnimBlendAssocDestructorHandler called! sAnimGroupID: %d | sAnimID: %d\n", pThis->sAnimGroup, pThis->sAnimID);
    RemoveAnimationAssociationFromMap(pThis);
}

CAnimBlendAssociationSAInterface* CClientGame::AddAnimationHandler(RpClump* pClump, AssocGroupId animGroup, AnimationId animID)
{
    // printf ( "AddAnimationHandler called! pClump, GroupID, AnimID: %p, %d, %d\n", (void*)pClump, animGroup, animID );
    return nullptr;
}

CAnimBlendAssociationSAInterface* CClientGame::AddAnimationAndSyncHandler(RpClump* pClump, CAnimBlendAssociationSAInterface* pAnimAssocToSyncWith,
                                                                          AssocGroupId animGroup, AnimationId animID)
{
    // printf ( "AddAnimationAndSyncHandler called! pClump, GroupID, AnimID: %p, %d, %d\n", (void*)pClump, animGroup, animID );
    return nullptr;
}

bool CClientGame::AssocGroupCopyAnimationHandler(CAnimBlendAssociationSAInterface* pAnimAssocInterface, RpClump* pClump,
                                                 CAnimBlendAssocGroupSAInterface* pAnimAssocGroupInterface, eAnimID animID)
{
    bool          isCustomAnimationToPlay = false;
    CAnimManager* pAnimationManager = g_pGame->GetAnimManager();

    if ((DWORD)pAnimAssocGroupInterface < 0x250)
    {
        g_pCore->LogEvent(542, "AssocGroupCopyAnimationHandler", "Interface is corrupt",
                          SString("pAnimAssocGroupInterface = %p | AnimID = %d", pAnimAssocGroupInterface, animID), 542);
    }

    auto pAnimAssocGroup = pAnimationManager->GetAnimBlendAssocGroup(pAnimAssocGroupInterface);

    if ((DWORD)pAnimAssocGroup->GetInterface() < 0x250)
    {
        g_pCore->LogEvent(543, "AssocGroupCopyAnimationHandler", "GetAnimBlendAssocGroup corrupted the interface",
                          SString("pAnimAssocGroupInterface = %p | AnimID = %d", pAnimAssocGroup->GetInterface(), animID), 543);
    }

    eAnimGroup iGroupID = pAnimAssocGroup->GetGroupID();

    if (iGroupID == eAnimGroup::ANIM_GROUP_NONE || pAnimAssocGroup->GetAnimBlock() == nullptr)
    {
        g_pCore->LogEvent(544, "AssocGroupCopyAnimationHandler", "pAnimAssocGroupInterface was invalid (animation block is null?)",
                          SString("GetAnimBlock() = %p | GroupID = %d", pAnimAssocGroup->GetAnimBlock(), iGroupID), 544);
        return false;
    }

    auto pOriginalAnimStaticAssoc = pAnimationManager->GetAnimStaticAssociation(iGroupID, animID);
    auto pOriginalAnimHierarchyInterface = pOriginalAnimStaticAssoc->GetAnimHierachyInterface();
    auto pAnimAssociation = pAnimationManager->GetAnimBlendAssociation(pAnimAssocInterface);

    CClientPed* pClientPed = GetClientPedByClump(*pClump);
    if (pClientPed != nullptr)
    {
        std::unique_ptr<CAnimBlendHierarchy> pAnimHierarchy = nullptr;
        if (pClientPed->IsTaskToBeRestoredOnAnimEnd() && pClientPed->GetTaskTypeToBeRestoredOnAnimEnd() == TASK_SIMPLE_DUCK &&
            animID != eAnimID::ANIM_ID_WEAPON_CROUCH)
        {
            // check for idle animation
            if (animID == eAnimID::ANIM_ID_IDLE)
            {
                if (iGroupID == eAnimGroup::ANIM_GROUP_DEFAULT ||
                    (iGroupID >= eAnimGroup::ANIM_GROUP_PLAYER && iGroupID <= eAnimGroup::ANIM_GROUP_PLAYERJETPACK) || iGroupID >= eAnimGroup::ANIM_GROUP_MAN)
                {
                    auto pDuckAnimStaticAssoc = pAnimationManager->GetAnimStaticAssociation(eAnimGroup::ANIM_GROUP_DEFAULT, eAnimID::ANIM_ID_WEAPON_CROUCH);
                    pAnimHierarchy = pAnimationManager->GetCustomAnimBlendHierarchy(pDuckAnimStaticAssoc->GetAnimHierachyInterface());
                    isCustomAnimationToPlay = true;
                }
            }
        }
        else
        {
            auto pReplacedAnimation = pClientPed->GetReplacedAnimation(pOriginalAnimHierarchyInterface);
            if (pReplacedAnimation != nullptr)
            {
                std::shared_ptr<CIFPAnimations> pIFPAnimations = pReplacedAnimation->pIFP->GetIFPAnimationsPointer();
                InsertAnimationAssociationToMap(pAnimAssocInterface, pIFPAnimations);

                // Play our custom animation instead of default
                pAnimHierarchy = pAnimationManager->GetCustomAnimBlendHierarchy(pReplacedAnimation->pAnimationHierarchy);
                isCustomAnimationToPlay = true;
            }
        }
        if (isCustomAnimationToPlay)
        {
            pAnimationManager->UncompressAnimation(pAnimHierarchy.get());
            pAnimAssociation->InitializeForCustomAnimation(pClump, pAnimHierarchy->GetInterface());
            pAnimAssociation->SetFlags(pOriginalAnimStaticAssoc->GetFlags());
            pAnimAssociation->SetAnimID(pOriginalAnimStaticAssoc->GetAnimID());
            pAnimAssociation->SetAnimGroup(pOriginalAnimStaticAssoc->GetAnimGroup());
        }
    }

    if (!isCustomAnimationToPlay)
    {
        auto pAnimHierarchy = pAnimationManager->GetAnimBlendHierarchy(pOriginalAnimHierarchyInterface);

        // Play default internal animation
        pAnimationManager->UncompressAnimation(pAnimHierarchy.get());
        pAnimAssociation->Constructor(*pOriginalAnimStaticAssoc->GetInterface());
    }
    return isCustomAnimationToPlay;
}

bool CClientGame::BlendAnimationHierarchyHandler(CAnimBlendAssociationSAInterface* pAnimAssoc, CAnimBlendHierarchySAInterface** pOutAnimHierarchy, int* pFlags,
                                                 RpClump* pClump)
{
    bool          isCustomAnimationToPlay = false;
    CAnimManager* pAnimationManager = g_pGame->GetAnimManager();
    CClientPed*   pClientPed = GetClientPedByClump(*pClump);
    if (pClientPed != nullptr)
    {
        if (pClientPed->IsNextAnimationCustom())
        {
            std::shared_ptr<CClientIFP> pIFP = pClientPed->GetCustomAnimationIFP();
            if (pIFP)
            {
                const SString& strAnimationName = pClientPed->GetNextAnimationCustomName();
                auto           pCustomAnimBlendHierarchy = pIFP->GetAnimationHierarchy(strAnimationName);
                if (pCustomAnimBlendHierarchy != nullptr)
                {
                    std::shared_ptr<CIFPAnimations> pIFPAnimations = pIFP->GetIFPAnimationsPointer();
                    InsertAnimationAssociationToMap(pAnimAssoc, pIFPAnimations);

                    pClientPed->SetCurrentAnimationCustom(true);
                    pClientPed->SetNextAnimationNormal();

                    if (pIFP->IsUnloading())
                    {
                        pClientPed->DereferenceCustomAnimationBlock();
                    }
                    *pOutAnimHierarchy = pCustomAnimBlendHierarchy;
                    isCustomAnimationToPlay = true;
                    return isCustomAnimationToPlay;
                }
            }
        }

        pClientPed->SetCurrentAnimationCustom(false);
        pClientPed->SetNextAnimationNormal();
    }
    return isCustomAnimationToPlay;
}

bool CClientGame::ProcessCollisionHandler(CEntitySAInterface* pThisInterface, CEntitySAInterface* pOtherInterface)
{
    if (pThisInterface == pOtherInterface)
        return true;

    if (!m_BuiltCollisionMapThisFrame)
    {
        // Build a map of CPhysicalSAInterface*/CClientEntity*'s that have collision disabled
        m_BuiltCollisionMapThisFrame = true;
        m_CachedCollisionMap.clear();

        std::map<CClientEntity*, bool>::iterator iter = m_AllDisabledCollisions.begin();
        for (; iter != m_AllDisabledCollisions.end(); ++iter)
        {
            CClientEntity*      pEntity = iter->first;
            CEntity*            pGameEntity = pEntity->GetGameEntity();
            CEntitySAInterface* pInterface = pGameEntity ? pGameEntity->GetInterface() : NULL;

            if (pInterface)
                m_CachedCollisionMap[pInterface] = pEntity;
        }
    }

    CPools* pPools = g_pGame->GetPools();

    // Check both elements appear in the cached map before doing extra processing
    std::map<CEntitySAInterface*, CClientEntity*>::iterator iter1 = m_CachedCollisionMap.find((CEntitySAInterface*)pThisInterface);
    if (iter1 != m_CachedCollisionMap.end())
    {
        std::map<CEntitySAInterface*, CClientEntity*>::iterator iter2 = m_CachedCollisionMap.find((CEntitySAInterface*)pOtherInterface);
        if (iter2 != m_CachedCollisionMap.end())
        {
            // Re-get the entity pointers using a safer method
            CEntity* pGameEntity = pPools->GetEntity((DWORD*)pThisInterface);
            CEntity* pGameColEntity = pPools->GetEntity((DWORD*)pOtherInterface);

            if (pGameEntity && pGameColEntity)
            {
                CClientEntity* pEntity = pPools->GetClientEntity((DWORD*)pThisInterface);
                CClientEntity* pColEntity = pPools->GetClientEntity((DWORD*)pOtherInterface);

                if (pEntity && pColEntity)
                {
#if MTA_DEBUG
                    CClientEntity* ppThisEntity2 = iter1->second;
                    CClientEntity* ppOtherEntity2 = iter2->second;
                    // These should match, but its not essential.
                    assert(ppThisEntity2 == pEntity);
                    assert(ppOtherEntity2 == pColEntity);
#endif
                    if (!pEntity->IsCollidableWith(pColEntity))
                        return false;
                }
            }
        }
    }

    return true;
}

//
// On demand files
//
void CClientGame::DownloadSingularResourceFiles()
{
    if (!IsTransferringSingularFiles())
        return;

    if (!g_pNet->IsConnected())
        return;

    CNetHTTPDownloadManagerInterface* pHTTP = g_pNet->GetHTTPDownloadManager(EDownloadMode::RESOURCE_SINGULAR_FILES);
    if (!pHTTP->ProcessQueuedFiles())
    {
        // Downloading
    }
    else
    {
        // Can't clear list until all files have been processed
        if (m_pSingularFileDownloadManager->AllComplete())
        {
            SetTransferringSingularFiles(false);
            m_pSingularFileDownloadManager->ClearList();
        }
    }
}

// Get relevant death anim for animation purposes during death situation
void GetDeathAnim(CClientPed* pDamagedPed, CEventDamage* pEvent, AssocGroupId& outAnimGroup, AnimationId& outAnimID)
{
    if (pEvent)
    {
        pEvent->ComputeDeathAnim(pDamagedPed->GetGamePlayer(), false);
        outAnimGroup = pEvent->GetAnimGroup();
        outAnimID = pEvent->GetAnimId();
    }
    else
    {
        // Using same numbers as CClientGame::DoWastedCheck defaults
        outAnimGroup = 0;
        outAnimID = 15;
    }
}

//
// DamageHandler seems to be called 3 times for each bit of damage:
//
// pass 1 - preApplyDamage
//   is pass 1 if: fDamage == zero  &&  fCurrentHealth == fPreviousHealth
//      returning false stops any damage being inflicted
//      returning true applies damage and allows pass 2
// pass 2 - postApplyDamage/preApplyAnim
//   is pass 2 if: fDamage > zero  &&  fCurrentHealth != fPreviousHealth
//      returning false stops damage anim
//      returning true allows damage anim and allows pass 3
// pass 3 - postApplyAnim
//   is pass 3 if: fDamage > zero  &&  fCurrentHealth == fPreviousHealth
//      returning false ??
//      returning true ??
//
bool CClientGame::DamageHandler(CPed* pDamagePed, CEventDamage* pEvent)
{
    // CEventDamage::AffectsPed: This is/can be called more than once for each bit of damage (and may not actually take any more health (even if we return
    // true))

    // Grab some data from the event
    CEntity*          pInflictor = pEvent->GetInflictingEntity();
    eWeaponType       weaponUsed = pEvent->GetWeaponUsed();
    ePedPieceTypes    hitZone = pEvent->GetPedPieceType();
    CWeaponInfo*      pWeaponInfo = g_pGame->GetWeaponInfo(weaponUsed);
    float             fDamage = pEvent->GetDamageApplied();
    EDamageReasonType damageReason = pEvent->GetDamageReason();

    /* Causes too much desync right now
    // Is this shotgun damage?
    if ( weaponUsed == WEAPONTYPE_SHOTGUN || weaponUsed == WEAPONTYPE_SPAS12_SHOTGUN )
    {
        // Make the ped fall down
        pEvent->MakePedFallDown ();
    } */

    CPools* pPools = g_pGame->GetPools();

    // Grab the damaged ped
    CClientPed* pDamagedPed = nullptr;

    if (pDamagePed)
    {
        SClientEntity<CPedSA>* pPedClientEntity = pPools->GetPed(reinterpret_cast<DWORD*>(pDamagePed->GetInterface()));

        if (pPedClientEntity)
        {
            // NOTE(botder): Don't use the damaged ped if the associated game entity doesn't exist to avoid a crash
            //               in the function ApplyPedDamageFromGame
            if (pPedClientEntity->pClientEntity && pPedClientEntity->pClientEntity->GetGameEntity() != nullptr)
            {
                pDamagedPed = reinterpret_cast<CClientPed*>(pPedClientEntity->pClientEntity);
            }
        }
    }

    // Grab the inflictor
    CClientEntity* pInflictingEntity = NULL;
    if (pInflictor)
    {
        pInflictingEntity = pPools->GetClientEntity((DWORD*)pInflictor->GetInterface());
    }

    // If the damage was caused by an explosion
    if (weaponUsed == WEAPONTYPE_EXPLOSION)
    {
        CClientEntity* pLastExplosionCreator = m_pManager->GetExplosionManager()->m_pLastCreator;

        // If we don't have an inflictor, look for the last explosion creator
        if (!pInflictor && pLastExplosionCreator)
            pInflictingEntity = pLastExplosionCreator;

        // Change the weapon used to whatever created the explosion
        weaponUsed = m_pManager->GetExplosionManager()->m_LastWeaponType;
    }

    // Do we have a damaged ped?
    if (pDamagedPed)
    {
        ///////////////////////////////////////////////////////////////////////////
        //
        // Pass 1 stuff
        //
        // return false to stop any damage being inflicted
        //
        ///////////////////////////////////////////////////////////////////////////

        // Pass 1 checks for double shots
        if (fDamage == 0.0f && damageReason != EDamageReason::PISTOL_WHIP)
        {
            // Only check for remote players
            CClientPlayer* pInflictingPlayer = DynamicCast<CClientPlayer>(pInflictingEntity);
            if (pInflictingPlayer && !pInflictingPlayer->IsLocalPlayer())
            {
                bool bBulletSyncShot = (g_iDamageEventLimit != -1);            // Called from discharge weapon
                bool bBulletSyncWeapon = GetWeaponTypeUsesBulletSync(weaponUsed);

                if (bBulletSyncShot)
                {
                    if (g_iDamageEventLimit == 0)
                    {
                        AddReportLog(5501, SString("2nd pass 1 for BulletSyncShot damage. weaponUsed:%d", weaponUsed));
                        return false;
                    }
                    g_iDamageEventLimit--;
                    if (!bBulletSyncWeapon)
                    {
                        AddReportLog(5502, SString("BulletSyncShot but not bBulletSyncWeapon. weaponUsed:%d", weaponUsed));
                        return false;
                    }
                }
                else
                {
                    if (bBulletSyncWeapon)
                    {
                        AddReportLog(5503, SString("not BulletSyncShot but bBulletSyncWeapon. weaponUsed:%d", weaponUsed));
                        return false;
                    }
                }
            }
        }

        // Is the damaged ped a player?
        if (IS_PLAYER(pDamagedPed))
        {
            CClientPlayer* pDamagedPlayer = static_cast<CClientPlayer*>(pDamagedPed);

            // Do we have an inflicting entity?
            if (pInflictingEntity)
            {
                // Grab the inflicting player
                CClientPlayer* pInflictingPlayer = NULL;

                switch (pInflictingEntity->GetType())
                {
                    case CCLIENTPLAYER:
                    {
                        pInflictingPlayer = static_cast<CClientPlayer*>(pInflictingEntity);
                        break;
                    }
                    case CCLIENTVEHICLE:
                    {
                        CClientVehicle* pInflictingVehicle = static_cast<CClientVehicle*>(pInflictingEntity);
                        if (pInflictingVehicle && pInflictingVehicle->GetControllingPlayer())
                        {
                            CClientPed* pPed = static_cast<CClientPed*>(pInflictingVehicle->GetControllingPlayer());
                            if (pPed && pPed->GetType() == CCLIENTPLAYER)
                                pInflictingPlayer = static_cast<CClientPlayer*>(pPed);
                        }
                        break;
                    }
                    default:
                        break;
                }
                if (pInflictingPlayer)
                {
                    // Is the damaged player on a team
                    CClientTeam* pTeam = pDamagedPlayer->GetTeam();
                    if (pTeam)
                    {
                        // Is this friendly-fire from a team-mate?
                        if (pDamagedPlayer->IsOnMyTeam(pInflictingPlayer) && !pTeam->GetFriendlyFire() && pDamagedPlayer != pInflictingPlayer)
                            return false;
                    }
                }
            }
        }
        ///////////////////////////////////////////////////////////////////////////
        // Pass 1 end
        ///////////////////////////////////////////////////////////////////////////

        return ApplyPedDamageFromGame(weaponUsed, fDamage, hitZone, pDamagedPed, pInflictingEntity, pEvent);
    }

    // No damage anim for fire
    if (weaponUsed == WEAPONTYPE_FLAMETHROWER)
        return false;

    // Allow the damage processing to continue
    return true;
}

//
// Check GTA stored health with our stored health and do stuff
//
bool CClientGame::ApplyPedDamageFromGame(eWeaponType weaponUsed, float fDamage, uchar hitZone, CClientPed* pDamagedPed, CClientEntity* pInflictingEntity,
                                         CEventDamage* pEvent)
{
    float fPreviousHealth = pDamagedPed->m_fHealth;
    float fCurrentHealth = pDamagedPed->GetGamePlayer()->GetHealth();
    float fPreviousArmor = pDamagedPed->m_fArmor;
    float fCurrentArmor = pDamagedPed->GetGamePlayer()->GetArmor();

    // Have we taken any damage here?
    if ((fPreviousHealth != fCurrentHealth || fPreviousArmor != fCurrentArmor) && fDamage != 0.0f)
    {
        ///////////////////////////////////////////////////////////////////////////
        ///
        // Pass 2 stuff - (GTA has applied the damage)
        //
        // return false to stop damage anim (incl. death task)
        //
        ///////////////////////////////////////////////////////////////////////////

        g_fApplyDamageLastAmount = fDamage;
        g_ucApplyDamageLastHitZone = hitZone;
        g_pApplyDamageLastDamagedPed = pDamagedPed;

        CLuaArguments Arguments;
        if (pInflictingEntity)
            Arguments.PushElement(pInflictingEntity);
        else
            Arguments.PushBoolean(false);
        Arguments.PushNumber(static_cast<unsigned char>(weaponUsed));
        Arguments.PushNumber(static_cast<unsigned char>(hitZone));
        Arguments.PushNumber(fDamage);

        // Call our event
        if ((IS_PLAYER(pDamagedPed) && !pDamagedPed->CallEvent("onClientPlayerDamage", Arguments, true)) ||
            (!IS_PLAYER(pDamagedPed) && !pDamagedPed->CallEvent("onClientPedDamage", Arguments, true)))
        {
            // Stop here if they cancelEvent it
            if (pDamagedPed->IsLocalPlayer())
            {
                // Reget values in case they have been changed during onClientPlayerDamage event (Avoid AC#1 kick)
                fPreviousHealth = pDamagedPed->m_fHealth;
                fPreviousArmor = pDamagedPed->m_fArmor;
            }
            pDamagedPed->GetGamePlayer()->SetHealth(fPreviousHealth);
            pDamagedPed->GetGamePlayer()->SetArmor(fPreviousArmor);
            return false;
        }

        if (pDamagedPed->IsLocalPlayer())
        {
            // Reget values in case they have been changed during onClientPlayerDamage event (Avoid AC#1 kick)
            fCurrentHealth = pDamagedPed->GetGamePlayer()->GetHealth();
            fCurrentArmor = pDamagedPed->GetGamePlayer()->GetArmor();
        }

        bool bIsBeingShotWhilstAiming = (weaponUsed >= WEAPONTYPE_PISTOL && weaponUsed <= WEAPONTYPE_MINIGUN && pDamagedPed->IsUsingGun());
        bool bOldBehaviour = !IsGlitchEnabled(GLITCH_HITANIM);

        bool bAllowChoke = true;
        // Is this is a remote player?
        if (!pDamagedPed->IsLocalPlayer())
        {
            // Don't allow GTA to start the choking task
            if (weaponUsed == WEAPONTYPE_TEARGAS || weaponUsed == WEAPONTYPE_SPRAYCAN || weaponUsed == WEAPONTYPE_EXTINGUISHER)
                bAllowChoke = false;
        }

        // Check if their health or armor is locked, and if so prevent applying the damage locally
        if (pDamagedPed->IsHealthLocked() || pDamagedPed->IsArmorLocked())
        {
            // Restore health+armor
            pDamagedPed->GetGamePlayer()->SetHealth(pDamagedPed->GetHealth());
            pDamagedPed->GetGamePlayer()->SetArmor(pDamagedPed->GetArmor());

            if (bOldBehaviour)
            {
                // Don't play the animation if it's going to be a death one, or if it's going to interrupt aiming
                if (fCurrentHealth == 0.0f || bIsBeingShotWhilstAiming)
                    return false;

                // Allow animation for remote players (if currently we don't need block choke)
                return bAllowChoke;
            }

            // No hit animation for remote players
            return false;
        }

        // Update our stored health/armor
        pDamagedPed->m_fHealth = fCurrentHealth;
        pDamagedPed->m_fArmor = fCurrentArmor;

        ElementID damagerID = INVALID_ELEMENT_ID;
        if (pInflictingEntity && !pInflictingEntity->IsLocalEntity())
            damagerID = pInflictingEntity->GetID();

        // Is it the local player?
        if (pDamagedPed->IsLocalPlayer())
        {
            // Update our stored damage stuff
            m_ucDamageWeapon = static_cast<unsigned char>(weaponUsed);
            m_ucDamageBodyPiece = static_cast<unsigned char>(hitZone);
            m_pDamageEntity = pInflictingEntity;
            m_ulDamageTime = CClientTime::GetTime();
            m_DamagerID = damagerID;
            m_bDamageSent = false;
        }
        // Does this damage kill the player?
        if (fCurrentHealth == 0.0f)
        {
            if (pDamagedPed->GetType() == CCLIENTPLAYER)
            {
                // Is the local player dying?
                if (pDamagedPed->IsLocalPlayer() && fPreviousHealth > 0.0f)
                {
                    // Grab our death animation
                    AssocGroupId animGroup;
                    AnimationId  animID;
                    GetDeathAnim(pDamagedPed, pEvent, animGroup, animID);

                    // Check if we're dead
                    DoWastedCheck(damagerID, weaponUsed, hitZone, animGroup, animID);
                }

                // Allow GTA to kill us if we've fell to our death
                if (pDamagedPed->IsLocalPlayer() && weaponUsed == WEAPONTYPE_FALL)
                    return true;

                // Don't let GTA start the death task
                return false;
            }
            else
            {
                if (pDamagedPed->IsLocalEntity() && fPreviousHealth > 0.0f)
                {
                    // Client-side ped
                    pDamagedPed->CallEvent("onClientPedWasted", Arguments, true);
                    AssocGroupId animGroup;
                    AnimationId  animID;
                    GetDeathAnim(pDamagedPed, pEvent, animGroup, animID);
                    pDamagedPed->Kill(weaponUsed, hitZone, false, false, animGroup, animID);
                    return true;
                }
                if (fPreviousHealth > 0.0f)
                {
                    // Grab our death animation
                    AssocGroupId animGroup;
                    AnimationId  animID;
                    GetDeathAnim(pDamagedPed, pEvent, animGroup, animID);

                    SendPedWastedPacket(pDamagedPed, damagerID, weaponUsed, hitZone, animGroup, animID);
                }
            }
        }

        // Disallow choke task if it's necessary
        if (!bAllowChoke)
            return false;

        // Inhibit hit-by-gun animation for local player if required
        if (bOldBehaviour)
            if (pDamagedPed->IsLocalPlayer() && bIsBeingShotWhilstAiming)
                return false;

        ///////////////////////////////////////////////////////////////////////////
        // Pass 2 end
        ///////////////////////////////////////////////////////////////////////////
    }

    // No damage anim for fire
    if (weaponUsed == WEAPONTYPE_FLAMETHROWER)
        return false;

    // Allow the damage processing to continue
    return true;
}

void CClientGame::DeathHandler(CPed* pKilledPedSA, unsigned char ucDeathReason, unsigned char ucBodyPart)
{
    CPools*                pPools = g_pGame->GetPools();
    SClientEntity<CPedSA>* pPedEntity = pPools->GetPed((DWORD*)pKilledPedSA->GetInterface());
    if (!pPedEntity)
    {
        return;
    }
    CClientPed* pKilledPed = reinterpret_cast<CClientPed*>(pPedEntity->pClientEntity);
    if (!pKilledPed)
        return;

    // Not required for remote players. Local player is handled in DoPulses->DoWastedCheck
    if (IS_PLAYER(pKilledPed))
        return;

    // Set the health to zero (this is safe as GTA will do it anyway in a few ticks)
    pKilledPed->SetHealth(0.0f);

    // Call Lua
    CLuaArguments Arguments;
    Arguments.PushBoolean(false);
    Arguments.PushNumber(ucDeathReason);
    Arguments.PushNumber(ucBodyPart);

    pKilledPed->CallEvent("onClientPedWasted", Arguments, true);

    // Notify the server
    SendPedWastedPacket(pKilledPed, INVALID_ELEMENT_ID, ucDeathReason, ucBodyPart);
}

bool CClientGame::VehicleCollisionHandler(CVehicleSAInterface*& pCollidingVehicle, CEntitySAInterface* pCollidedWith, int iModelIndex, float fDamageImpulseMag,
                                          float fCollidingDamageImpulseMag, uint16 usPieceType, CVector vecCollisionPos, CVector vecCollisionVelocity, bool isProjectile)
{
    if (pCollidingVehicle && pCollidedWith)
    {
        CPools*                    pPools = g_pGame->GetPools();
        SClientEntity<CVehicleSA>* pColliderEntity = pPools->GetVehicle((DWORD*)pCollidingVehicle);
        if (pColliderEntity)
        {
            CClientEntity* pVehicleClientEntity = pColliderEntity->pClientEntity;
            if (!pVehicleClientEntity)
            {
                return false;
            }

            CClientVehicle* pClientVehicle = static_cast<CClientVehicle*>(pVehicleClientEntity);
            CClientEntity*  pCollidedWithClientEntity = !isProjectile ? pPools->GetClientEntity((DWORD*)pCollidedWith) : m_pManager->GetProjectileManager()->Get(pCollidedWith);

            CLuaArguments Arguments;
            if (pCollidedWithClientEntity)
            {
                Arguments.PushElement(pCollidedWithClientEntity);
            }
            else
            {
                Arguments.PushNil();
            }
            Arguments.PushNumber(fDamageImpulseMag);
            Arguments.PushNumber(usPieceType);
            Arguments.PushNumber(vecCollisionPos.fX);
            Arguments.PushNumber(vecCollisionPos.fY);
            Arguments.PushNumber(vecCollisionPos.fZ);
            Arguments.PushNumber(vecCollisionVelocity.fX);
            Arguments.PushNumber(vecCollisionVelocity.fY);
            Arguments.PushNumber(vecCollisionVelocity.fZ);
            Arguments.PushNumber(fCollidingDamageImpulseMag);
            Arguments.PushNumber(iModelIndex);

            pVehicleClientEntity->CallEvent("onClientVehicleCollision", Arguments, true);

            // Update the colliding vehicle, because it might have been invalidated in onClientVehicleCollision (e.g. fixVehicle)
            pCollidingVehicle = reinterpret_cast<CVehicleSAInterface*>(pVehicleClientEntity->GetGameEntity()->GetInterface());

            // Allocate a BitStream
            NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
            // Make sure it created
            if (pBitStream)
            {
                if (pBitStream->Version() >= 0x028)
                {
                    // Sync Stuff
                    // if it's not a local vehicle + it collided with the local player
                    if (pVehicleClientEntity->IsLocalEntity() == false && pCollidedWithClientEntity == g_pClientGame->GetLocalPlayer())
                    {
                        // is it below the anti spam threshold?
                        if (pClientVehicle->GetTimeSinceLastPush() >= MIN_PUSH_ANTISPAM_RATE)
                        {
                            // if there is no controlling player
                            if (!pClientVehicle->GetControllingPlayer())
                            {
                                CDeathmatchVehicle* Vehicle = static_cast<CDeathmatchVehicle*>(pVehicleClientEntity);
                                // if We aren't already syncing the vehicle
                                if (GetUnoccupiedVehicleSync()->Exists(Vehicle) == false)
                                {
                                    // Write the vehicle ID
                                    pBitStream->Write(pVehicleClientEntity->GetID());
                                    // Send!
                                    g_pNet->SendPacket(PACKET_ID_VEHICLE_PUSH_SYNC, pBitStream, PACKET_PRIORITY_MEDIUM,
                                                       PACKET_RELIABILITY_UNRELIABLE_SEQUENCED);
                                    // Reset our push time
                                    pClientVehicle->ResetLastPushTime();
                                }
                            }
                        }
                    }
                }
                g_pNet->DeallocateNetBitStream(pBitStream);
            }
            return true;
        }
    }
    return false;
}

bool CClientGame::HeliKillHandler(CVehicleSAInterface* pHeliInterface, CEntitySAInterface* pHitInterface)
{
    if (pHeliInterface && pHitInterface)
    {
        CPools* pPools = g_pGame->GetPools();

        // Get our heli and client heli
        SClientEntity<CVehicleSA>* pVehicleEntity = pPools->GetVehicle((DWORD*)pHeliInterface);
        if (pVehicleEntity)
        {
            CClientVehicle* pClientHeli = reinterpret_cast<CClientVehicle*>(pVehicleEntity->pClientEntity);
            if (!pClientHeli)
            {
                return true;
            }

            if (!pClientHeli->AreHeliBladeCollisionsEnabled())
            {
                return false;
            }

            SClientEntity<CPedSA>* pPedEntity = pPools->GetPed((DWORD*)pHitInterface);
            if (pPedEntity)
            {
                CClientPed* pClientPed = reinterpret_cast<CClientPed*>(pPedEntity->pClientEntity);
                if (!pClientPed)
                {
                    return false;
                }

                // Iterate our "stored" cancel state and find the heli in question
                std::pair<std::multimap<CClientVehicle*, CClientPed*>::iterator, std::multimap<CClientVehicle*, CClientPed*>::iterator> iterators =
                    m_HeliCollisionsMap.equal_range(pClientHeli);
                std::multimap<CClientVehicle*, CClientPed*>::const_iterator iter = iterators.first;
                for (; iter != iterators.second; ++iter)
                {
                    // If the Heli and ped collided within the clear rate return false
                    if ((*iter).first == pClientHeli && (*iter).second == pClientPed)
                        return false;
                }

                CLuaArguments Arguments;
                if (pClientHeli)
                {
                    // Push our heli
                    Arguments.PushElement(pClientHeli);
                }
                else
                {
                    Arguments.PushNil();
                }

                // Trigger our event
                bool bContinue;
                if (IS_PLAYER(pClientPed))
                    bContinue = pClientPed->CallEvent("onClientPlayerHeliKilled", Arguments, true);
                else
                    bContinue = pClientPed->CallEvent("onClientPedHeliKilled", Arguments, true);

                // Was our event cancelled
                if (!bContinue)
                {
                    // Add our heli and ped pair to the list
                    std::pair<CClientVehicle*, CClientPed*> pair = std::pair<CClientVehicle*, CClientPed*>(pClientHeli, pClientPed);
                    m_HeliCollisionsMap.insert(pair);
                }
                // Return if it was cancelled
                return bContinue;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool CClientGame::VehicleDamageHandler(CEntitySAInterface* pVehicleInterface, float fLoss, CEntitySAInterface* pAttackerInterface, eWeaponType weaponType,
                                       const CVector& vecDamagePos, uchar ucTyre)
{
    bool                       bAllowDamage = true;
    CPools*                    pPools = g_pGame->GetPools();
    SClientEntity<CVehicleSA>* pVehicleEntity = pPools->GetVehicle((DWORD*)pVehicleInterface);
    if (pVehicleEntity)
    {
        CClientVehicle* pClientVehicle = reinterpret_cast<CClientVehicle*>(pVehicleEntity->pClientEntity);
        if (!pClientVehicle)
        {
            return bAllowDamage;
        }

        CClientEntity* pClientAttacker = pPools->GetClientEntity((DWORD*)pAttackerInterface);

        // Compose arguments
        // attacker, weapon, loss, damagepos, tyreIdx
        CLuaArguments Arguments;
        if (pClientAttacker)
            Arguments.PushElement(pClientAttacker);
        else
            Arguments.PushNil();
        if (weaponType != WEAPONTYPE_INVALID)
            Arguments.PushNumber(weaponType);
        else
            Arguments.PushNil();
        Arguments.PushNumber(fLoss);
        Arguments.PushNumber(vecDamagePos.fX);
        Arguments.PushNumber(vecDamagePos.fY);
        Arguments.PushNumber(vecDamagePos.fZ);
        if (ucTyre != UCHAR_INVALID_INDEX)
            Arguments.PushNumber(ucTyre);
        else
            Arguments.PushNil();

        if (!pClientVehicle->CallEvent("onClientVehicleDamage", Arguments, true))
        {
            bAllowDamage = false;
        }
    }

    return bAllowDamage;
}

bool CClientGame::ObjectDamageHandler(CObjectSAInterface* pObjectInterface, float fLoss, CEntitySAInterface* pAttackerInterface)
{
    if (pObjectInterface)
    {
        CPools*                   pPools = g_pGame->GetPools();
        SClientEntity<CObjectSA>* pObjectEntity = pPools->GetObject((DWORD*)pObjectInterface);
        if (pObjectEntity)
        {
            CClientObject* pClientObject = reinterpret_cast<CClientObject*>(pObjectEntity->pClientEntity);
            if (!pClientObject)
            {
                return true;
            }
            CLuaArguments Arguments;
            Arguments.PushNumber(fLoss);

            CClientEntity* pClientAttacker = pPools->GetClientEntity((DWORD*)pAttackerInterface);
            if (pClientAttacker)
                Arguments.PushElement(pClientAttacker);
            else
                Arguments.PushNil();

            return pClientObject->CallEvent("onClientObjectDamage", Arguments, true);
        }
    }
    return true;
}

bool CClientGame::ObjectBreakHandler(CObjectSAInterface* pObjectInterface, CEntitySAInterface* pAttackerInterface)
{
    if (pObjectInterface)
    {
        CPools*                   pPools = g_pGame->GetPools();
        SClientEntity<CObjectSA>* pObjectEntity = pPools->GetObject((DWORD*)pObjectInterface);
        if (pObjectEntity)
        {
            CClientObject* pClientObject = reinterpret_cast<CClientObject*>(pObjectEntity->pClientEntity);
            if (!pClientObject)
            {
                return true;
            }

            if (!pClientObject->IsBreakable(false))
                return false;

            // Apply to MTA's "internal storage", too
            pClientObject->SetHealth(0.0f);

            CLuaArguments Arguments;

            CClientEntity* pClientAttacker = pPools->GetClientEntity((DWORD*)pAttackerInterface);
            if (pClientAttacker)
                Arguments.PushElement(pClientAttacker);
            else
                Arguments.PushNil();

            return pClientObject->CallEvent("onClientObjectBreak", Arguments, true);
        }
    }
    return true;
}

bool CClientGame::WaterCannonHitHandler(CVehicleSAInterface* pCannonVehicle, CPedSAInterface* pHitPed)
{
    if (pCannonVehicle && pHitPed)
    {
        CPools*                    pPools = g_pGame->GetPools();
        SClientEntity<CVehicleSA>* pVehicleEntity = pPools->GetVehicle((DWORD*)pCannonVehicle);
        if (pVehicleEntity)
        {
            CLuaArguments Arguments;

            CClientVehicle* pCannonClientVehicle = reinterpret_cast<CClientVehicle*>(pVehicleEntity->pClientEntity);
            if (!pCannonClientVehicle)
            {
                return false;
            }

            CClientPed*            pClientPed = nullptr;
            SClientEntity<CPedSA>* pPedEntity = pPools->GetPed((DWORD*)pHitPed);
            if (pPedEntity)
            {
                pClientPed = reinterpret_cast<CClientPed*>(pPedEntity->pClientEntity);
                if (pClientPed)
                {
                    Arguments.PushElement(pClientPed);
                }
            }
            else
            {
                Arguments.PushNil();
            }

            // Trigger our event
            bool bContinue = true;
            if (pClientPed && !IS_PLAYER(pClientPed))
                bContinue = pCannonClientVehicle->CallEvent("onClientPedHitByWaterCannon", Arguments, true);
            else
                bContinue = pCannonClientVehicle->CallEvent("onClientPlayerHitByWaterCannon", Arguments, true);

            // Return if it was cancelled
            return bContinue;
        }
    }
    return false;
}

bool CClientGame::VehicleFellThroughMapHandler(CVehicleSAInterface* pVehicleInterface)
{
    if (pVehicleInterface)
    {
        CPools*                    pPools = g_pGame->GetPools();
        SClientEntity<CVehicleSA>* pVehicleEntity = pPools->GetVehicle((DWORD*)pVehicleInterface);
        if (pVehicleEntity)
        {
            CClientVehicle* pClientVehicle = reinterpret_cast<CClientVehicle*>(pVehicleEntity->pClientEntity);
            if (pClientVehicle)
            {
                // handle or don't
                return pClientVehicle->OnVehicleFallThroughMap();
            }
        }
    }
    // unhandled
    return false;
}

// Validate known objects
void CClientGame::GameObjectDestructHandler(CEntitySAInterface* pObject)
{
    // m_pGameEntityXRefManager->OnGameEntityDestruct(pObject);
}

void CClientGame::GameVehicleDestructHandler(CEntitySAInterface* pVehicle)
{
    // m_pGameEntityXRefManager->OnGameEntityDestruct(pVehicle);
}

void CClientGame::GamePlayerDestructHandler(CEntitySAInterface* pPlayer)
{
    // m_pGameEntityXRefManager->OnGameEntityDestruct(pPlayer);
}

void CClientGame::GameProjectileDestructHandler(CEntitySAInterface* pProjectile)
{
    CClientProjectile* pClientProjectile = m_pManager->GetProjectileManager()->Get(pProjectile);
    // Happens when destroyElement is called rather than letting the projectile expire
    // Normal code path is destruction from CProjectileSAInterface -> CProjectileSA -> CClientProjectile
    // destroyElement is CClientProjectile -> CProjectileSA -> CProjectileSAInterface
    // which means the CClientProjectile element is deleted when we get here
    if (pClientProjectile)
        CStaticFunctionDefinitions::DestroyElement(*pClientProjectile);
}

void CClientGame::GameModelRemoveHandler(ushort usModelId)
{
    // m_pGameEntityXRefManager->OnGameModelRemove(usModelId);
}

void CClientGame::GameRunNamedAnimDestructorHandler(class CTaskSimpleRunNamedAnimSAInterface* pTask)
{
    auto it = m_mapOfRunNamedAnimTasks.find(pTask);
    if (it != m_mapOfRunNamedAnimTasks.end())
    {
        CClientPed* pPed = it->second;
        if (pPed && pPed->IsTaskToBeRestoredOnAnimEnd())
        {
            if (pPed->GetTaskTypeToBeRestoredOnAnimEnd() == TASK_SIMPLE_DUCK)
            {
                pPed->GetGamePlayer()->GetPedIntelligence()->SetTaskDuckSecondary(0);
                pPed->SetTaskToBeRestoredOnAnimEnd(false);
            }
        }
        m_mapOfRunNamedAnimTasks.erase(pTask);
    }
}

void CClientGame::TaskSimpleBeHitHandler(CPedSAInterface* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId)
{
    bool bOldBehaviour = !IsGlitchEnabled(GLITCH_HITANIM);
    if (bOldBehaviour)
        return;

    CPools*     pPools = g_pGame->GetPools();
    CClientPed* pClientPedAttacker = DynamicCast<CClientPed>(pPools->GetClientEntity((DWORD*)pPedAttacker));

    // Make sure cause was networked ped
    if (pClientPedAttacker && !pClientPedAttacker->IsLocalEntity())
    {
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        pBitStream->Write((ushort)TASK_SIMPLE_BE_HIT);
        pBitStream->Write(pClientPedAttacker->GetID());
        pBitStream->Write((uchar)hitBodyPart);
        pBitStream->Write((uchar)hitBodySide);
        pBitStream->Write((uchar)weaponId);
        g_pNet->SendPacket(PACKET_ID_PED_TASK, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
        g_pNet->DeallocateNetBitStream(pBitStream);
    }
}

bool CClientGame::StaticProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (g_pClientGame)
    {
        return g_pClientGame->ProcessMessage(hwnd, uMsg, wParam, lParam);
    }

    return false;
}

bool CClientGame::ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ProcessMessageForCursorEvents(hwnd, uMsg, wParam, lParam))
    {
        return true;
    }

    return false;
}

// Shot compensation (Jax):
// Positions the local player where he should of been on the shooting player's screen when he
// fired remotely. (the position we !reported! around the time he shot)
bool bShotCompensation = true;

// Temporary pointers for pre- and post-functions
CVector     vecWeaponFirePosition, vecRemoteWeaponFirePosition;
CPlayerPed* pWeaponFirePed = NULL;

bool CClientGame::PreWeaponFire(CPlayerPed* pPlayerPed, bool bStopIfUsingBulletSync)
{
    pWeaponFirePed = pPlayerPed;

    // Got a local player model?
    CClientPed* pLocalPlayer = g_pClientGame->m_pLocalPlayer;
    if (pLocalPlayer && pWeaponFirePed)
    {
        CClientVehicle* pVehicle = pLocalPlayer->GetRealOccupiedVehicle();

        // Get the CClientNetPlayer class with the specified player ped
        CClientPlayer* pPlayer = g_pClientGame->m_pPlayerManager->Get(pWeaponFirePed, true);

        // Move both players to where they should be for shot compensation
        if (pPlayer && !pPlayer->IsLocalPlayer())
        {
            if (bStopIfUsingBulletSync && pPlayer->IsCurrentWeaponUsingBulletSync())
                return false;            // Don't apply shot compensation & tell caller to not do bullet trace

            if (bShotCompensation)
            {
                if (!pVehicle || pLocalPlayer->GetOccupiedVehicleSeat() == 0)
                {
                    // Warp back in time to where we were when this player shot (their latency)

                    // We don't account for interpolation here, +250ms seems to work better
                    // ** Changed ajustment to +125ms as the position of this clients player on the firers screen
                    // has been changed. See CClientPed::UpdateTargetPosition() **
                    CVector        vecPosition;
                    unsigned short usLatency = (pPlayer->GetLatency() + 125);
                    g_pClientGame->m_pNetAPI->GetInterpolation(vecPosition, usLatency);

                    // Move the entity back
                    if (pVehicle)
                    {
                        pVehicle->GetPosition(vecWeaponFirePosition);
                        pVehicle->SetPosition(vecPosition, false, false);
                    }
                    else
                    {
                        pLocalPlayer->GetPosition(vecWeaponFirePosition);
                        pLocalPlayer->SetPosition(vecPosition, false, false);
                    }
                }
            }
        }
    }
    return true;
}

void CClientGame::PostWeaponFire()
{
    // Send bullet sync with local damage info
    if (g_bBulletFireVectorsValid)
    {
        g_bBulletFireVectorsValid = false;
        CClientPlayer* pLocalPlayer = g_pClientGame->m_pLocalPlayer;
        if (pLocalPlayer)
        {
            eWeaponType weaponType = pLocalPlayer->GetCurrentWeaponType();
            if (g_pClientGame->GetWeaponTypeUsesBulletSync(weaponType))
            {
                g_pClientGame->GetNetAPI()->SendBulletSyncFire(weaponType, g_vecBulletFireStartPosition, g_vecBulletFireEndPosition, g_fApplyDamageLastAmount,
                                                               g_ucApplyDamageLastHitZone, DynamicCast<CClientPlayer>(g_pApplyDamageLastDamagedPed));
            }
        }
    }

    // Got a local player model?
    CClientPed* pLocalPlayer = g_pClientGame->m_pLocalPlayer;
    if (pLocalPlayer && pWeaponFirePed)
    {
        CPools*                pPools = g_pGame->GetPools();
        CClientPed*            pPed = nullptr;
        SClientEntity<CPedSA>* pPedEntity = pPools->GetPed((DWORD*)pWeaponFirePed->GetInterface());
        if (pPedEntity)
        {
            pPed = reinterpret_cast<CClientPed*>(pPedEntity->pClientEntity);
        }
        if (pPed)
        {
            if (pPed->GetType() == CCLIENTPLAYER)
            {
                if (bShotCompensation)
                {
                    // Restore compensated positions
                    if (!pPed->IsLocalPlayer())
                    {
                        CClientVehicle* pVehicle = pLocalPlayer->GetRealOccupiedVehicle();
                        if (!pVehicle)
                        {
                            pLocalPlayer->SetPosition(vecWeaponFirePosition, false, false);
                        }
                        else if (pLocalPlayer->GetOccupiedVehicleSeat() == 0)
                        {
                            pVehicle->SetPosition(vecWeaponFirePosition, false, false);
                        }
                    }
                }
            }

            // Call some events
            CWeapon* pWeapon = pPed->GetWeapon();
            if (pWeapon)
            {
                CVector        vecCollision;
                CClientEntity* pCollisionEntity = NULL;

                if (pPed->GetBulletImpactData(&pCollisionEntity, &vecCollision) == false)
                {
                    CShotSyncData* pShotsyncData = pPed->m_shotSyncData;
                    CVector        vecOrigin, vecTarget;
                    pPed->GetShotData(&vecOrigin, &vecTarget);

                    CColPoint* pCollision = NULL;
                    CEntity*   pCollisionGameEntity = NULL;
                    vecCollision = vecTarget;
                    bool bCollision = g_pGame->GetWorld()->ProcessLineOfSight(&vecOrigin, &vecTarget, &pCollision, &pCollisionGameEntity);
                    if (bCollision && pCollision)
                        vecCollision = pCollision->GetPosition();

                    // Destroy the colpoint
                    if (pCollision)
                    {
                        pCollision->Destroy();
                    }

                    if (pCollisionGameEntity)
                        pCollisionEntity = pPools->GetClientEntity((DWORD*)pCollisionGameEntity->GetInterface());
                }
                else
                {
                    pPed->ClearBulletImpactData();
                }

                // Call our lua event
                CLuaArguments Arguments;
                Arguments.PushNumber((double)pWeapon->GetType());
                Arguments.PushNumber((double)pWeapon->GetAmmoTotal());
                Arguments.PushNumber((double)pWeapon->GetAmmoInClip());
                Arguments.PushNumber((double)vecCollision.fX);
                Arguments.PushNumber((double)vecCollision.fY);
                Arguments.PushNumber((double)vecCollision.fZ);
                if (pCollisionEntity)
                    Arguments.PushElement(pCollisionEntity);
                else
                    Arguments.PushNil();

                if (IS_PLAYER(pPed))
                {
                    CVector vecOrigin;
                    pPed->GetShotData(&vecOrigin);
                    Arguments.PushNumber((double)vecOrigin.fX);
                    Arguments.PushNumber((double)vecOrigin.fY);
                    Arguments.PushNumber((double)vecOrigin.fZ);
                    pPed->CallEvent("onClientPlayerWeaponFire", Arguments, true);
                }
                else
                    pPed->CallEvent("onClientPedWeaponFire", Arguments, true);
            }
            pPed->PostWeaponFire();
#ifdef MTA_DEBUG
            if (pPed->IsLocalPlayer() && g_pClientGame->m_bDoPaintballs)
            {
                g_pClientGame->DoPaintballs();
            }
#endif
        }
    }
    pWeaponFirePed = NULL;
}

void CClientGame::BulletImpact(CPed* pInitiator, CEntity* pVictim, const CVector* pStartPosition, const CVector* pEndPosition)
{
    // Got a local player model?
    CClientPed* pLocalPlayer = g_pClientGame->m_pLocalPlayer;
    if (pLocalPlayer && pInitiator)
    {
        CPools*                pPools = g_pGame->GetPools();
        SClientEntity<CPedSA>* pPedEntity = pPools->GetPed((DWORD*)pInitiator->GetInterface());
        if (pPedEntity)
        {
            // Find the client ped that initiated the bullet impact
            CClientPed* pInitiatorPed = reinterpret_cast<CClientPed*>(pPedEntity->pClientEntity);
            if (!pInitiatorPed)
            {
                return;
            }

            // Calculate the collision of the bullet
            CVector    vecCollision;
            CColPoint* pCollision = NULL;
            bool       bCollision = g_pGame->GetWorld()->ProcessLineOfSight(pStartPosition, pEndPosition, &pCollision, NULL);
            if (bCollision && pCollision)
            {
                vecCollision = pCollision->GetPosition();
            }
            else
            {
                // If we don't have a collision, use the end of the ray that the bullet is tracing.
                vecCollision = *pEndPosition;
            }

            // Destroy the colpoint
            if (pCollision)
            {
                pCollision->Destroy();
            }

            // Find the client entity for the victim.
            CClientEntity* pClientVictim = NULL;
            if (pVictim)
            {
                pClientVictim = pPools->GetClientEntity((DWORD*)pVictim->GetInterface());
            }

            // Store the data in the bullet fire initiator.
            pInitiatorPed->SetBulletImpactData(pClientVictim, vecCollision);
        }
    }
}

void CClientGame::BulletFire(CPed* pInitiator, const CVector* pStartPosition, const CVector* pEndPosition)
{
    g_bBulletFireVectorsValid = false;
    g_fApplyDamageLastAmount = 0;
    g_ucApplyDamageLastHitZone = 0;
    g_pApplyDamageLastDamagedPed = NULL;

    // Got a local player model?
    CClientPlayer* pLocalPlayer = g_pClientGame->m_pLocalPlayer;
    if (pLocalPlayer && pLocalPlayer->GetGamePlayer() == pInitiator)
    {
        g_bBulletFireVectorsValid = true;
        g_vecBulletFireStartPosition = *pStartPosition;
        g_vecBulletFireEndPosition = *pEndPosition;
    }
}

bool CClientGame::StaticProcessPacket(unsigned char ucPacketID, NetBitStreamInterface& bitStream)
{
    if (g_pClientGame)
    {
        g_pCore->UpdateDummyProgress();
        g_pClientGame->GetManager()->GetPacketRecorder()->RecordPacket(ucPacketID, bitStream);
        return g_pClientGame->m_pPacketHandler->ProcessPacket(ucPacketID, bitStream);
    }

    return false;
}

void CClientGame::SendExplosionSync(const CVector& vecPosition, eExplosionType Type, CClientEntity* pOrigin)
{
    SPositionSync position(false);
    position.data.vecPosition = vecPosition;

    // Create the bitstream
    NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
    if (pBitStream)
    {
        // Write our origin id
        if (pOrigin)
        {
            pBitStream->WriteBit(true);
            pBitStream->Write(pOrigin->GetID());

            // Because we use this packet to notify the server of blown vehicles,
            // we include a bit, whether the vehicle was blown without an explosion
            if (pBitStream->Can(eBitStreamVersion::VehicleBlowStateSupport))
            {
                if (pOrigin->GetType() == CCLIENTVEHICLE)
                {
                    auto vehicle = reinterpret_cast<CClientVehicle*>(pOrigin);
                    pBitStream->WriteBit(1);
                    pBitStream->WriteBit(vehicle->GetBlowState() == VehicleBlowState::BLOWN);
                }
                else
                {
                    pBitStream->WriteBit(0);
                }
            }

            // Convert position
            CVector vecTemp;
            pOrigin->GetPosition(vecTemp);
            position.data.vecPosition -= vecTemp;
        }
        else
            pBitStream->WriteBit(false);

        // Write the position and the type
        pBitStream->Write(&position);

        SExplosionTypeSync explosionType;
        explosionType.data.uiType = Type;
        pBitStream->Write(&explosionType);

        // Destroy it
        g_pNet->SendPacket(PACKET_ID_EXPLOSION, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
        g_pNet->DeallocateNetBitStream(pBitStream);
    }
}

void CClientGame::SendFireSync(CFire* pFire)
{
#ifdef MTA_DEBUG
    CVector* vecPos = pFire->GetPosition();
    if (vecPos)
        g_pCore->GetConsole()->Printf("we're sending fire: %f %f %f %f", pFire->GetStrength(), vecPos->fX, vecPos->fY, vecPos->fZ);
    else
        g_pCore->GetConsole()->Printf("we're sending a fire!");
#endif
}

void CClientGame::SendProjectileSync(CClientProjectile* pProjectile)
{
    // Create the bitstream
    NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
    if (pBitStream)
    {
        SPositionSync  origin(false);
        CClientEntity* pOriginSource = NULL;
        eWeaponType    weaponType = pProjectile->GetWeaponType();
        CClientEntity* pTarget = pProjectile->GetTargetEntity();
        origin.data.vecPosition = *pProjectile->GetOrigin();

        // Is this a heatseaking missile with a target? sync it relative to the target
        if (weaponType == WEAPONTYPE_ROCKET_HS && pTarget && !pTarget->IsLocalEntity())
            pOriginSource = pTarget;

        // Write the source of the projectile, if it has
        if (pOriginSource)
        {
            CVector vecTemp;
            pOriginSource->GetPosition(vecTemp);
            origin.data.vecPosition -= vecTemp;

            pBitStream->WriteBit(true);
            pBitStream->Write(pOriginSource->GetID());
        }
        else
            pBitStream->WriteBit(false);

        // Write the origin position
        pBitStream->Write(&origin);

        // Write the creator weapon type
        SWeaponTypeSync weaponTypeSync;
        weaponTypeSync.data.ucWeaponType = static_cast<unsigned char>(weaponType);
        pBitStream->Write(&weaponTypeSync);

        // Write the projectile's model
        if (pBitStream->Version() >= 0x4F)
            if (pBitStream->Version() >= 0x52 || pOriginSource)            // Fix possible error for 0x51 server
                pBitStream->Write(pProjectile->GetModel());

        switch (weaponType)
        {
            case WEAPONTYPE_GRENADE:
            case WEAPONTYPE_TEARGAS:
            case WEAPONTYPE_MOLOTOV:
            case WEAPONTYPE_REMOTE_SATCHEL_CHARGE:
            {
                SFloatSync<7, 17> projectileForce;
                projectileForce.data.fValue = pProjectile->GetForce();
                pBitStream->Write(&projectileForce);

                SVelocitySync velocity;
                pProjectile->GetVelocity(velocity.data.vecVelocity);
                pBitStream->Write(&velocity);

                break;
            }
            case WEAPONTYPE_ROCKET:
            case WEAPONTYPE_ROCKET_HS:
            {
                if (pTarget)
                {
                    pBitStream->WriteBit(true);
                    pBitStream->Write(pTarget->GetID());
                }
                else
                    pBitStream->WriteBit(false);

                SVelocitySync velocity;
                pProjectile->GetVelocity(velocity.data.vecVelocity);
                pBitStream->Write(&velocity);

                SRotationRadiansSync rotation(true);
                pProjectile->GetRotationRadians(rotation.data.vecRotation);
                pBitStream->Write(&rotation);

                break;
            }
            case WEAPONTYPE_FLARE:
            case WEAPONTYPE_FREEFALL_BOMB:
                break;
        }
        g_pNet->SendPacket(PACKET_ID_PROJECTILE, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);

        // Destroy it
        g_pNet->DeallocateNetBitStream(pBitStream);
    }
}

void CClientGame::ResetAmmoInClip()
{
    memset(&m_wasWeaponAmmoInClip[0], 0, sizeof(m_wasWeaponAmmoInClip));
}

void CClientGame::ResetMapInfo()
{
    // Restore some game stuff
    // Keybinds
    g_pCore->GetKeyBinds()->SetAllControlsEnabled(true, true, true);

    // Radarmap
    m_pRadarMap->SetForcedState(false);

    // Camera
    m_pCamera->FadeOut(0.0f, 0, 0, 0);
    g_pGame->GetWorld()->SetCurrentArea(0);
    m_pCamera->SetFocusToLocalPlayer();
    g_pGame->GetSettings()->ResetFieldOfViewFromScript();

    // Dimension and interiors
    SetAllDimensions(0);
    SetAllInteriors(0);

    // Hud
    g_pGame->GetHud()->SetComponentVisible(HUD_ALL, true);

    // Disable area names as they are on load until camera unfades
    g_pGame->GetHud()->SetComponentVisible(HUD_AREA_NAME, false);
    g_pGame->GetHud()->SetComponentVisible(HUD_VITAL_STATS, false);

    m_bHudAreaNameDisabled = false;

    // Reset world special properties, world properties, weather properties etc
    ResetWorldPropsInfo desc;
    desc.resetSpecialProperties = true;
    desc.resetWorldProperties = true;
    desc.resetWeatherProperties = true;
    desc.resetLODs = true;
    desc.resetSounds = true;
    ResetWorldProperties(desc);

    // Wanted-level
    SetWanted(0);

    // Money
    SetMoney(0, true);

    // Weather
    m_pBlendedWeather->SetWeather(0);

    // Grain effect
    g_pMultiplayer->SetGrainMultiplier(eGrainMultiplierType::ALL, 1.0f);
    g_pMultiplayer->SetGrainLevel(0);

    // Cheats
    g_pGame->ResetCheats();

    // Players
    m_pPlayerManager->ResetAll();

    // Disable the change of any player stats
    g_pMultiplayer->SetLocalStatsStatic(true);

    // Close all garages
    CGarage*  pGarage = NULL;
    CGarages* pGarages = g_pCore->GetGame()->GetGarages();

    for (unsigned char i = 0; (pGarage = pGarages->GetGarage(i)) != NULL; i++)
    {
        pGarage->SetOpen(false);
    }

    // Player specific stuff
    if (m_pLocalPlayer)
    {
        // Interior
        m_pLocalPlayer->SetInterior(0);

        // Headless state
        m_pLocalPlayer->SetHeadless(false);

        // Frozen state
        m_pLocalPlayer->SetFrozen(false);

        // Voice
        short sVoiceType, sVoiceID;
        m_pLocalPlayer->GetModelInfo()->GetVoice(&sVoiceType, &sVoiceID);
        m_pLocalPlayer->SetVoice(sVoiceType, sVoiceID);

        m_pLocalPlayer->DestroySatchelCharges(false, true);
        // Tell the server we want to destroy our satchels
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        if (pBitStream)
        {
            g_pNet->SendPacket(PACKET_ID_DESTROY_SATCHELS, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
            g_pNet->DeallocateNetBitStream(pBitStream);
        }
    }

    // Reset camera drunk/shake level
    CPlayerInfo* pPlayerInfo = g_pGame->GetPlayerInfo();

    if (pPlayerInfo)
        pPlayerInfo->SetCamDrunkLevel(static_cast<byte>(0));

    RestreamWorld();

    ReinitMarkers();
}

void CClientGame::SendPedWastedPacket(CClientPed* Ped, ElementID damagerID, unsigned char ucWeapon, unsigned char ucBodyPiece, AssocGroupId animGroup,
                                      AnimationId animID)
{
    if (Ped && Ped->GetHealth() == 0.0f)
    {
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        if (pBitStream)
        {
            // Write some death info
            pBitStream->WriteCompressed(animGroup);
            pBitStream->WriteCompressed(animID);

            pBitStream->Write(damagerID);

            SWeaponTypeSync weapon;
            weapon.data.ucWeaponType = ucWeapon;
            pBitStream->Write(&weapon);

            SBodypartSync bodyPart;
            bodyPart.data.uiBodypart = ucBodyPiece;
            pBitStream->Write(&bodyPart);

            // Write the position we died in
            SPositionSync pos(false);
            Ped->GetPosition(pos.data.vecPosition);
            pBitStream->Write(&pos);

            pBitStream->Write(Ped->GetID());

            // The ammo in our weapon and write the ammo total
            CWeapon*        pPlayerWeapon = Ped->GetWeapon();
            SWeaponAmmoSync ammo(ucWeapon, true, false);
            ammo.data.usTotalAmmo = 0;
            if (pPlayerWeapon)
                ammo.data.usTotalAmmo = static_cast<unsigned short>(pPlayerWeapon->GetAmmoTotal());
            pBitStream->Write(&ammo);

            // Send the packet
            g_pNet->SendPacket(PACKET_ID_PED_WASTED, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
            g_pNet->DeallocateNetBitStream(pBitStream);
        }
    }
}

void CClientGame::DoWastedCheck(ElementID damagerID, unsigned char ucWeapon, unsigned char ucBodyPiece, AssocGroupId animGroup, AnimationId animID)
{
    // Are we not already marked as dead? and have we run out of health?
    if (!m_pLocalPlayer->IsDeadOnNetwork() && m_pLocalPlayer->GetHealth() == 0.0f)
    {
        // Send the "i am dead" packet
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        if (pBitStream)
        {
            m_pLocalPlayer->SetDeadOnNetwork(true);

            // Call the onClientPlayerWasted event
            CClientEntity* pKiller = (damagerID != INVALID_ELEMENT_ID) ? CElementIDs::GetElement(damagerID) : NULL;
            CLuaArguments  Arguments;
            if (pKiller)
                Arguments.PushElement(pKiller);
            else
                Arguments.PushBoolean(false);
            if (ucWeapon != 0xFF)
                Arguments.PushNumber(ucWeapon);
            else
                Arguments.PushBoolean(false);
            if (ucBodyPiece != 0xFF)
                Arguments.PushNumber(ucBodyPiece);
            else
                Arguments.PushBoolean(false);
            Arguments.PushBoolean(false);
            Arguments.PushNumber(animGroup);
            Arguments.PushNumber(animID);
            m_pLocalPlayer->CallEvent("onClientPlayerWasted", Arguments, true);

            // Write some death info
            pBitStream->WriteCompressed(animGroup);
            pBitStream->WriteCompressed(animID);

            pBitStream->Write(damagerID);

            SWeaponTypeSync weapon;
            weapon.data.ucWeaponType = ucWeapon;
            pBitStream->Write(&weapon);

            SBodypartSync bodyPart;
            bodyPart.data.uiBodypart = ucBodyPiece;
            pBitStream->Write(&bodyPart);

            // Write the position we died in
            SPositionSync pos(false);
            m_pLocalPlayer->GetPosition(pos.data.vecPosition);
            pBitStream->Write(&pos);

            // The ammo in our weapon and write the ammo total
            CWeapon*        pPlayerWeapon = m_pLocalPlayer->GetWeapon();
            SWeaponAmmoSync ammo(ucWeapon, true, false);
            ammo.data.usTotalAmmo = 0;
            if (pPlayerWeapon)
                ammo.data.usTotalAmmo = static_cast<unsigned short>(pPlayerWeapon->GetAmmoTotal());
            pBitStream->Write(&ammo);

            // Send the packet
            g_pNet->SendPacket(PACKET_ID_PLAYER_WASTED, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
            g_pNet->DeallocateNetBitStream(pBitStream);

            const auto discord = g_pCore->GetDiscord();
            if (discord && discord->IsDiscordRPCEnabled() && discord->IsDiscordCustomDetailsDisallowed())
            {
                static const std::vector<std::string> states{
                    _("In a ditch"), _("En-route to hospital"), _("Meeting their maker"),
                    _("Regretting their decisions"), _("Wasted")
                };

                const std::string& state = states[rand() % states.size()];
                discord->SetPresenceState(state.c_str(), false);
            }
        }
    }
}

bool CClientGame::OnKeyDown(CGUIKeyEventArgs Args)
{
    return true;
}

bool CClientGame::OnMouseClick(CGUIMouseEventArgs Args)
{
    if (!Args.pWindow)
        return false;

    const char* szButton = NULL;
    const char* szState = NULL;
    switch (Args.button)
    {
        case CGUIMouse::LeftButton:
            szButton = "left";
            szState = "up";
            break;
        case CGUIMouse::MiddleButton:
            szButton = "middle";
            szState = "up";
            break;
        case CGUIMouse::RightButton:
            szButton = "right";
            szState = "up";
            break;
    }

    if (szButton)
    {
        CLuaArguments Arguments;
        Arguments.PushString(szButton);
        Arguments.PushString(szState);
        Arguments.PushNumber(Args.position.fX);
        Arguments.PushNumber(Args.position.fY);

        CClientGUIElement* pGUIElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pWindow);
        if (GetGUIManager()->Exists(pGUIElement))
        {
            pGUIElement->CallEvent("onClientGUIClick", Arguments, true);
        }
    }

    return true;
}

bool CClientGame::OnMouseDoubleClick(CGUIMouseEventArgs Args)
{
    if (!Args.pWindow)
        return false;

    const char* szButton = NULL;
    const char* szState = NULL;
    switch (Args.button)
    {
        case CGUIMouse::LeftButton:
            szButton = "left";
            szState = "up";
            break;
        case CGUIMouse::MiddleButton:
            szButton = "middle";
            szState = "up";
            break;
        case CGUIMouse::RightButton:
            szButton = "right";
            szState = "up";
            break;
    }

    if (szButton)
    {
        CLuaArguments Arguments;
        Arguments.PushString(szButton);
        Arguments.PushString(szState);
        Arguments.PushNumber(Args.position.fX);
        Arguments.PushNumber(Args.position.fY);

        CClientGUIElement* pGUIElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pWindow);
        if (GetGUIManager()->Exists(pGUIElement))
        {
            pGUIElement->CallEvent("onClientGUIDoubleClick", Arguments, true);
        }
    }

    return true;
}

bool CClientGame::OnMouseButtonDown(CGUIMouseEventArgs Args)
{
    if (!Args.pWindow)
        return false;

    const char* szButton = NULL;
    switch (Args.button)
    {
        case CGUIMouse::LeftButton:
            szButton = "left";
            break;
        case CGUIMouse::MiddleButton:
            szButton = "middle";
            break;
        case CGUIMouse::RightButton:
            szButton = "right";
            break;
    }

    if (szButton)
    {
        CLuaArguments Arguments;
        Arguments.PushString(szButton);
        Arguments.PushNumber(Args.position.fX);
        Arguments.PushNumber(Args.position.fY);

        CClientGUIElement* pGUIElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pWindow);
        if (GetGUIManager()->Exists(pGUIElement))
        {
            pGUIElement->CallEvent("onClientGUIMouseDown", Arguments, true);
        }
    }

    return true;
}

bool CClientGame::OnMouseButtonUp(CGUIMouseEventArgs Args)
{
    if (!Args.pWindow)
        return false;

    const char* szButton = NULL;
    switch (Args.button)
    {
        case CGUIMouse::LeftButton:
            szButton = "left";
            break;
        case CGUIMouse::MiddleButton:
            szButton = "middle";
            break;
        case CGUIMouse::RightButton:
            szButton = "right";
            break;
    }

    if (szButton)
    {
        CLuaArguments Arguments;
        Arguments.PushString(szButton);
        Arguments.PushNumber(Args.position.fX);
        Arguments.PushNumber(Args.position.fY);

        CClientGUIElement* pGUIElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pWindow);
        if (GetGUIManager()->Exists(pGUIElement))
        {
            pGUIElement->CallEvent("onClientGUIMouseUp", Arguments, true);
        }
    }

    return true;
}

bool CClientGame::OnMouseMove(CGUIMouseEventArgs Args)
{
    if (!Args.pWindow)
        return false;

    CLuaArguments Arguments;
    Arguments.PushNumber(Args.position.fX);
    Arguments.PushNumber(Args.position.fY);

    CClientGUIElement* pGUIElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pWindow);
    if (GetGUIManager()->Exists(pGUIElement))
        pGUIElement->CallEvent("onClientMouseMove", Arguments, true);

    return true;
}

bool CClientGame::OnMouseEnter(CGUIMouseEventArgs Args)
{
    if (!Args.pWindow)
        return false;

    CLuaArguments Arguments;
    Arguments.PushNumber(Args.position.fX);
    Arguments.PushNumber(Args.position.fY);
    if (Args.pSwitchedWindow)
    {
        CClientGUIElement* pGUISwitchedElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pSwitchedWindow);
        if (GetGUIManager()->Exists(pGUISwitchedElement))
            Arguments.PushElement(pGUISwitchedElement);
    }

    CClientGUIElement* pGUIElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pWindow);
    if (GetGUIManager()->Exists(pGUIElement))
        pGUIElement->CallEvent("onClientMouseEnter", Arguments, true);

    return true;
}

bool CClientGame::OnMouseLeave(CGUIMouseEventArgs Args)
{
    if (!Args.pWindow)
        return false;

    CLuaArguments Arguments;
    Arguments.PushNumber(Args.position.fX);
    Arguments.PushNumber(Args.position.fY);
    if (Args.pSwitchedWindow)
    {
        CClientGUIElement* pGUISwitchedElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pSwitchedWindow);
        if (GetGUIManager()->Exists(pGUISwitchedElement))
            Arguments.PushElement(pGUISwitchedElement);
    }

    CClientGUIElement* pGUIElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pWindow);
    if (GetGUIManager()->Exists(pGUIElement))
        pGUIElement->CallEvent("onClientMouseLeave", Arguments, true);

    return true;
}

bool CClientGame::OnMouseWheel(CGUIMouseEventArgs Args)
{
    if (!Args.pWindow)
        return false;

    CLuaArguments Arguments;
    Arguments.PushNumber(Args.wheelChange);

    CClientGUIElement* pGUIElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pWindow);
    if (GetGUIManager()->Exists(pGUIElement))
        pGUIElement->CallEvent("onClientMouseWheel", Arguments, true);

    return true;
}

bool CClientGame::OnMove(CGUIElement* pElement)
{
    if (!pElement)
        return false;

    CLuaArguments Arguments;

    CClientGUIElement* pGUIElement = CGUI_GET_CCLIENTGUIELEMENT(pElement);
    if (pGUIElement && GetGUIManager()->Exists(pGUIElement))
        pGUIElement->CallEvent("onClientGUIMove", Arguments, true);

    return true;
}

bool CClientGame::OnSize(CGUIElement* pElement)
{
    if (!pElement)
        return false;

    CLuaArguments Arguments;

    CClientGUIElement* pGUIElement = CGUI_GET_CCLIENTGUIELEMENT(pElement);
    if (GetGUIManager()->Exists(pGUIElement))
        pGUIElement->CallEvent("onClientGUISize", Arguments, true);

    return true;
}

bool CClientGame::OnFocusGain(CGUIFocusEventArgs Args)
{
    if (!Args.pActivatedWindow)
        return false;

    CLuaArguments Arguments;

    CClientGUIElement* pActivatedGUIElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pActivatedWindow);

    if (Args.pDeactivatedWindow)
    {
        CClientGUIElement* pDeactivatedGUIElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pDeactivatedWindow);
        if (GetGUIManager()->Exists(pDeactivatedGUIElement))
            pDeactivatedGUIElement->CallEvent("onClientGUIBlur", Arguments, true);
    }

    if (GetGUIManager()->Exists(pActivatedGUIElement))
        pActivatedGUIElement->CallEvent("onClientGUIFocus", Arguments, true);

    return true;
}

bool CClientGame::OnFocusLoss(CGUIFocusEventArgs Args)
{
    if (!Args.pDeactivatedWindow)
        return false;

    CLuaArguments Arguments;

    if (Args.pActivatedWindow)
    {
        // pDeactivatedWindow looses focus but an other window is now gaining it so we let CClientGame::OnFocusGain trigger both events in the right order
        return true;
    }

    CClientGUIElement* pDeactivatedGUIElement = CGUI_GET_CCLIENTGUIELEMENT(Args.pDeactivatedWindow);
    if (GetGUIManager()->Exists(pDeactivatedGUIElement))
        pDeactivatedGUIElement->CallEvent("onClientGUIBlur", Arguments, true);

    return true;
}

//
// Display a progress dialog if a big packet is coming in
//
void CClientGame::NotifyBigPacketProgress(unsigned long ulBytesReceived, unsigned long ulTotalSize)
{
    // Should display progress box?
    if (ulBytesReceived >= ulTotalSize || ulTotalSize < 50000)
    {
        if (m_bReceivingBigPacket)
        {
            // Switch off progress box
            m_bReceivingBigPacket = false;
            m_pBigPacketTransferBox->Hide();
        }
        return;
    }

    // Update progress box
    if (!m_bReceivingBigPacket || m_ulBigPacketSize != ulTotalSize)
    {
        m_bReceivingBigPacket = true;
        m_ulBigPacketSize = ulTotalSize;
        m_pBigPacketTransferBox->Hide();
        m_pBigPacketTransferBox->AddToDownloadTotalSize(ulTotalSize);
        m_pBigPacketTransferBox->Show();
    }

    m_pBigPacketTransferBox->DoPulse();
    m_pBigPacketTransferBox->SetDownloadProgress(std::min(ulTotalSize, ulBytesReceived));
}

bool CClientGame::SetGlitchEnabled(unsigned char ucGlitch, bool bEnabled)
{
    if (ucGlitch < NUM_GLITCHES && bEnabled != m_Glitches[ucGlitch])
    {
        m_Glitches[ucGlitch] = bEnabled;
        if (ucGlitch == GLITCH_QUICKRELOAD)
            g_pMultiplayer->DisableQuickReload(!bEnabled);
        if (ucGlitch == GLITCH_CLOSEDAMAGE)
            g_pMultiplayer->DisableCloseRangeDamage(!bEnabled);
        return true;
    }
    return false;
}

bool CClientGame::IsGlitchEnabled(unsigned char ucGlitch)
{
    return ucGlitch < NUM_GLITCHES && m_Glitches[ucGlitch];
}

bool CClientGame::SetWorldSpecialProperty(WorldSpecialProperty property, bool isEnabled)
{
    switch (property)
    {
        case WorldSpecialProperty::HOVERCARS:
        case WorldSpecialProperty::AIRCARS:
        case WorldSpecialProperty::EXTRABUNNY:
        case WorldSpecialProperty::EXTRAJUMP:
            return g_pGame->SetCheatEnabled(EnumToString(property), isEnabled);
        case WorldSpecialProperty::RANDOMFOLIAGE:
            g_pGame->SetRandomFoliageEnabled(isEnabled);
            return true;
        case WorldSpecialProperty::SNIPERMOON:
            g_pGame->SetMoonEasterEggEnabled(isEnabled);
            return true;
        case WorldSpecialProperty::EXTRAAIRRESISTANCE:
            g_pGame->SetExtraAirResistanceEnabled(isEnabled);
            return true;
        case WorldSpecialProperty::UNDERWORLDWARP:
            g_pGame->SetUnderWorldWarpEnabled(isEnabled);
            return true;
        case WorldSpecialProperty::VEHICLESUNGLARE:
            g_pGame->SetVehicleSunGlareEnabled(isEnabled);
            return true;
        case WorldSpecialProperty::CORONAZTEST:
            g_pGame->SetCoronaZTestEnabled(isEnabled);
            return true;
        case WorldSpecialProperty::WATERCREATURES:
            g_pGame->SetWaterCreaturesEnabled(isEnabled);
            return true;
        case WorldSpecialProperty::BURNFLIPPEDCARS:
            g_pGame->SetBurnFlippedCarsEnabled(isEnabled);
            return true;
        case WorldSpecialProperty::FIREBALLDESTRUCT:
            g_pGame->SetFireballDestructEnabled(isEnabled);
            return true;
        case WorldSpecialProperty::EXTENDEDWATERCANNONS:
            g_pGame->SetExtendedWaterCannonsEnabled(isEnabled);
        case WorldSpecialProperty::ROADSIGNSTEXT:
            g_pGame->SetRoadSignsTextEnabled(isEnabled);
            return true;
        case WorldSpecialProperty::TUNNELWEATHERBLEND:
            g_pGame->SetTunnelWeatherBlendEnabled(isEnabled);
            return true;
    }
    return false;
}

bool CClientGame::IsWorldSpecialProperty(WorldSpecialProperty property)
{
    switch (property)
    {
        case WorldSpecialProperty::HOVERCARS:
        case WorldSpecialProperty::AIRCARS:
        case WorldSpecialProperty::EXTRABUNNY:
        case WorldSpecialProperty::EXTRAJUMP:
            return g_pGame->IsCheatEnabled(EnumToString(property));
        case WorldSpecialProperty::RANDOMFOLIAGE:
            return g_pGame->IsRandomFoliageEnabled();
        case WorldSpecialProperty::SNIPERMOON:
            return g_pGame->IsMoonEasterEggEnabled();
        case WorldSpecialProperty::EXTRAAIRRESISTANCE:
            return g_pGame->IsExtraAirResistanceEnabled();
        case WorldSpecialProperty::UNDERWORLDWARP:
            return g_pGame->IsUnderWorldWarpEnabled();
        case WorldSpecialProperty::VEHICLESUNGLARE:
            return g_pGame->IsVehicleSunGlareEnabled();
        case WorldSpecialProperty::CORONAZTEST:
            return g_pGame->IsCoronaZTestEnabled();
        case WorldSpecialProperty::WATERCREATURES:
            return g_pGame->IsWaterCreaturesEnabled();
        case WorldSpecialProperty::BURNFLIPPEDCARS:
            return g_pGame->IsBurnFlippedCarsEnabled();
        case WorldSpecialProperty::FIREBALLDESTRUCT:
            return g_pGame->IsFireballDestructEnabled();
        case WorldSpecialProperty::EXTENDEDWATERCANNONS:
            return g_pGame->IsExtendedWaterCannonsEnabled();
        case WorldSpecialProperty::ROADSIGNSTEXT:
            return g_pGame->IsRoadSignsTextEnabled();
        case WorldSpecialProperty::TUNNELWEATHERBLEND:
            return g_pGame->IsTunnelWeatherBlendEnabled();
    }
    return false;
}

bool CClientGame::SetCloudsEnabled(bool bEnabled)
{
    m_bCloudsEnabled = bEnabled;
    return true;
}
bool CClientGame::GetCloudsEnabled()
{
    return m_bCloudsEnabled;
}

bool CClientGame::SetBirdsEnabled(bool bEnabled)
{
    m_bBirdsEnabled = bEnabled;
    return true;
}
bool CClientGame::GetBirdsEnabled()
{
    return m_bBirdsEnabled;
}

#pragma code_seg(".text")
bool CClientGame::VerifySADataFiles(int iEnableClientChecks)
{
    int& iCheckStatus = g_pGame->GetCheckStatus();

    if (!g_pGame->VerifySADataFileNames())
        iCheckStatus |= (1 << 11);

    __declspec(allocate(".text")) static char szVerifyData[][32] = {"data/carmods.dat",  "\x6c\xbe\x84\x53\x61\xe7\x6a\xae\x35\xdd\xca\x30\x08\x67\xca\xdf",
                                                                    "data/handling.cfg", "\x68\x68\xac\xce\xf9\x33\xf1\x85\x5e\xc2\x8c\xe1\x93\xa7\x81\x59",
                                                                    "data/melee.dat",    "\xb2\xf0\x56\x57\x98\x0e\x4a\x69\x3f\x8f\xf5\xea\xdc\xba\xd8\xf8",
                                                                    "data/object.dat",   "\x46\xa5\xe7\xdf\xf9\x00\x78\x84\x2e\x24\xd9\xde\x5e\x92\xcc\x3e",
                                                                    "data/surface.dat",  "\x9e\xb4\xe4\xe4\x74\xab\xd5\xda\x2f\x39\x61\xa5\xef\x54\x9f\x9e",
                                                                    "data/surfaud.dat",  "\xc3\x2c\x58\x6e\x8b\xa3\x57\x42\xe3\x56\xe6\x52\x56\x19\xf7\xc3",
                                                                    "data/surfinfo.dat", "\x60\x5d\xd0\xbe\xab\xcc\xc7\x97\xce\x94\xa5\x1a\x3e\x4a\x09\xeb",
                                                                    "data/vehicles.ide", "\xbd\xc3\xa0\xfc\xed\x24\x02\xc5\xbc\x61\x58\x57\x14\x45\x7d\x4b",
                                                                    "data/water.dat",    "\x69\x04\x00\xec\xc9\x21\x69\xd9\xea\xdd\xaa\xa9\x48\x90\x3e\xfb",
                                                                    "data/water1.dat",   "\x16\xfe\x5a\x3e\x8c\x57\xd0\x2e\xb6\x2a\x44\xa9\x6d\x8b\x9d\x39",
                                                                    "data/weapon.dat",   "\x0a\x9b\xb4\x90\x03\x68\x03\x64\xf9\xf9\x76\x8e\x9b\xce\xa9\x82",
                                                                    "anim/ped.ifp",      "\x47\x36\xB2\xC9\x0B\x00\x98\x12\x55\xF9\x50\x73\x08\xEE\x91\x74"};

    CMD5Hasher hasher;
    for (int i = 0; i < NUMELMS(szVerifyData); i += 2)
    {
        MD5 md5;
        if (!hasher.Calculate(szVerifyData[i], md5) || memcmp(md5.data, szVerifyData[i + 1], 0x10))
        {
            iCheckStatus |= (1 << i);
        }
    }

    if (iCheckStatus & iEnableClientChecks)
    {
        g_pCore->InitiateDataFilesFix();
        g_pCore->GetModManager()->RequestUnload();
        return false;
    }

    return true;
}

void CClientGame::InitVoice(bool bEnabled, unsigned int uiServerSampleRate, unsigned char ucQuality, unsigned int uiBitrate)
{
    if (m_pVoiceRecorder)
    {
        m_pVoiceRecorder->Init(bEnabled, uiServerSampleRate, ucQuality, uiBitrate);
    }
}

//
// If debug render mode is on, allow each element in range to draw some stuff
//
void CClientGame::DebugElementRender()
{
    if (!GetDevelopmentMode() || !GetShowCollision())
        return;

    CVector vecCameraPos;
    m_pCamera->GetPosition(vecCameraPos);
    float fDrawRadius = 200.f;

    // Get all entities within range
    CClientEntityResult result;
    GetClientSpatialDatabase()->SphereQuery(result, CSphere(vecCameraPos, fDrawRadius));

    // For each entity found
    for (CClientEntityResult::const_iterator it = result.begin(); it != result.end(); ++it)
    {
        CClientEntity* pEntity = *it;
        if (pEntity->GetParent())
            pEntity->DebugRender(vecCameraPos, fDrawRadius);
    }
}

//////////////////////////////////////////////////////////////////
// Click
//
void CClientGame::TakePlayerScreenShot(uint uiSizeX, uint uiSizeY, const SString& strTag, uint uiQuality, uint uiMaxBandwidth, uint uiMaxPacketSize,
                                       CResource* pResource, uint uiServerSentTime)
{
    if (!pResource)
        return;

    bool bAllowScreenUploadEnabled = 1;
    g_pCore->GetCVars()->Get("allow_screen_upload", bAllowScreenUploadEnabled);
    bool bWindowMinimized = g_pCore->IsWindowMinimized();

    if (bWindowMinimized || !bAllowScreenUploadEnabled)
    {
        // If alt-tabbed or opt-out
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        if (!bAllowScreenUploadEnabled)
            pBitStream->Write((uchar)EPlayerScreenShotResult::DISABLED);
        else
            pBitStream->Write((uchar)EPlayerScreenShotResult::MINIMIZED);
        pBitStream->Write(uiServerSentTime);
        if (pBitStream->Version() >= 0x053)
            pBitStream->Write(pResource->GetNetID());
        else
            pBitStream->WriteString(pResource->GetName());
        pBitStream->WriteString(strTag);
        g_pNet->SendPacket(PACKET_ID_PLAYER_SCREENSHOT, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE_ORDERED, PACKET_ORDERING_DATA_TRANSFER);
        g_pNet->DeallocateNetBitStream(pBitStream);
    }
    else
    {
        // Do grab and send
        SScreenShotArgs screenShotArgs;
        screenShotArgs.uiMaxBandwidth = uiMaxBandwidth;
        screenShotArgs.uiMaxPacketSize = uiMaxPacketSize;
        screenShotArgs.usResourceNetId = pResource->GetNetID();
        screenShotArgs.strTag = strTag;
        screenShotArgs.uiServerSentTime = uiServerSentTime;
        m_ScreenShotArgList.push_back(screenShotArgs);
        g_pCore->GetGraphics()->GetScreenGrabber()->QueueScreenShot(uiSizeX, uiSizeY, uiQuality, &CClientGame::StaticGottenPlayerScreenShot);
    }
}

//////////////////////////////////////////////////////////////////
// Callback from TakePlayerScreendsShot
//
void CClientGame::StaticGottenPlayerScreenShot(const CBuffer* pBuffer, uint uiTimeSpentInQueue, const SString& strError)
{
    if (g_pClientGame)
        g_pClientGame->GottenPlayerScreenShot(pBuffer, uiTimeSpentInQueue, strError);
}

//////////////////////////////////////////////////////////////////
// Break data into packets and put into delayed send list
//
void CClientGame::GottenPlayerScreenShot(const CBuffer* pBuffer, uint uiTimeSpentInQueue, const SString& strInError)
{
    SString strError = strInError;

    // Pop saved args
    if (m_ScreenShotArgList.empty())
        return;

    SScreenShotArgs screenShotArgs = m_ScreenShotArgList.front();
    m_ScreenShotArgList.pop_front();
    const uint    uiMaxBandwidth = Clamp<uint>(100, screenShotArgs.uiMaxBandwidth, 1000000);
    const uint    uiMaxPacketSize = Clamp<uint>(100, screenShotArgs.uiMaxPacketSize, 100000);
    const ushort  usResourceNetId = screenShotArgs.usResourceNetId;
    const SString strTag = screenShotArgs.strTag;
    const uint    uiServerGrabTime = screenShotArgs.uiServerSentTime + uiTimeSpentInQueue;

    // Silently ignore if resource has been stopped
    CResource* pResource = GetResourceManager()->GetResourceFromNetID(usResourceNetId);
    if (!pResource)
        return;

    // Validate buffer
    if (strError.empty())
    {
        if (!pBuffer || pBuffer->GetSize() == 0)
            strError = "Buffer empty";
    }

    // Handle error
    if (!strError.empty())
    {
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        pBitStream->Write((uchar)EPlayerScreenShotResult::ERROR_);
        pBitStream->Write(uiServerGrabTime);
        if (pBitStream->Version() >= 0x053)
            pBitStream->Write(pResource->GetNetID());
        else
            pBitStream->WriteString(pResource->GetName());
        pBitStream->WriteString(strTag);
        if (pBitStream->Version() >= 0x053)
            pBitStream->WriteString(strError);
        g_pNet->SendPacket(PACKET_ID_PLAYER_SCREENSHOT, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE_ORDERED, PACKET_ORDERING_DATA_TRANSFER);
        g_pNet->DeallocateNetBitStream(pBitStream);
        return;
    }

    // Calc constants stuff
    const uint      uiSendRate = Clamp<uint>(5, uiMaxBandwidth / uiMaxPacketSize, 20);
    const long long llPacketInterval = 1000 / uiSendRate;
    const uint      uiTotalByteSize = pBuffer->GetSize();
    const char*     pData = pBuffer->GetData();
    const uint      uiBytesPerPart = std::min(std::min(std::max(100U, uiMaxBandwidth / uiSendRate), uiTotalByteSize), 30000U);
    const uint      uiNumParts = std::max(1U, (uiTotalByteSize + uiBytesPerPart - 1) / uiBytesPerPart);

    // Calc variables stuff
    CTickCount tickCount = CTickCount::Now() + CTickCount(llPacketInterval);
    uint       uiBytesRemaining = uiTotalByteSize;
    m_usNextScreenShotId++;

    // Make each packet
    for (uint i = 0; i < uiNumParts; i++)
    {
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();

        ushort usPartNumber = i;
        ushort usBytesThisPart = std::min(uiBytesRemaining, uiBytesPerPart);
        assert(usBytesThisPart != 0);

        pBitStream->Write((uchar)EPlayerScreenShotResult::SUCCESS);
        pBitStream->Write(m_usNextScreenShotId);
        pBitStream->Write(usPartNumber);
        pBitStream->Write(usBytesThisPart);
        pBitStream->Write(pData, usBytesThisPart);

        // Write more info if first part
        if (usPartNumber == 0)
        {
            pBitStream->Write(uiServerGrabTime);
            pBitStream->Write(uiTotalByteSize);
            pBitStream->Write((ushort)uiNumParts);
            if (pBitStream->Version() >= 0x053)
                pBitStream->Write(pResource->GetNetID());
            else
                pBitStream->WriteString(pResource->GetName());
            pBitStream->WriteString(strTag);
        }

        // Add to delay send list
        SDelayedPacketInfo delayedPacketInfo;
        delayedPacketInfo.useTickCount = tickCount;
        delayedPacketInfo.ucPacketID = PACKET_ID_PLAYER_SCREENSHOT;
        delayedPacketInfo.pBitStream = pBitStream;
        delayedPacketInfo.packetPriority = PACKET_PRIORITY_LOW;
        delayedPacketInfo.packetReliability = PACKET_RELIABILITY_RELIABLE_ORDERED;
        delayedPacketInfo.packetOrdering = PACKET_ORDERING_DATA_TRANSFER;
        m_DelayedSendList.push_back(delayedPacketInfo);

        // Increment stuff
        pData += usBytesThisPart;
        uiBytesRemaining -= usBytesThisPart;
        tickCount += CTickCount(llPacketInterval);
    }

    assert(uiBytesRemaining == 0);
}

//////////////////////////////////////////////////////////////////
// Process delay send list
//
void CClientGame::ProcessDelayedSendList()
{
    CTickCount tickCount = CTickCount::Now();

    while (!m_DelayedSendList.empty())
    {
        SDelayedPacketInfo& info = m_DelayedSendList.front();
        if (info.useTickCount > tickCount)
            break;

        g_pNet->SendPacket(info.ucPacketID, info.pBitStream, info.packetPriority, info.packetReliability, info.packetOrdering);
        g_pNet->DeallocateNetBitStream(info.pBitStream);
        m_DelayedSendList.pop_front();
    }
}

//////////////////////////////////////////////////////////////////
//
// CClientGame::SetWeaponTypesUsingBulletSync
//
// Set whether the local player will send bulletsync messages for the supplied weapon type
//
//////////////////////////////////////////////////////////////////
void CClientGame::SetWeaponTypesUsingBulletSync(const std::set<eWeaponType>& weaponTypesUsingBulletSync)
{
    m_weaponTypesUsingBulletSync = weaponTypesUsingBulletSync;
}

//////////////////////////////////////////////////////////////////
//
// CClientGame::GetWeaponTypeUsesBulletSync
//
// Get whether the local player should send bulletsync messages for the supplied weapon type
//
//////////////////////////////////////////////////////////////////
bool CClientGame::GetWeaponTypeUsesBulletSync(eWeaponType weaponType)
{
    return MapContains(m_weaponTypesUsingBulletSync, weaponType);
}

//////////////////////////////////////////////////////////////////
//
// CClientGame::SetDevelopmentMode
//
// Special mode which enables commands such as showcol and showsound
//
//////////////////////////////////////////////////////////////////
void CClientGame::SetDevelopmentMode(bool bEnable, bool bEnableWeb)
{
    m_bDevelopmentMode = bEnable;

    if (g_pCore->IsWebCoreLoaded())
        g_pCore->GetWebCore()->SetTestModeEnabled(bEnableWeb);
}

//////////////////////////////////////////////////////////////////
//
// CClientGame::StaticWorldSoundHandler
//
// Handle callback from CAudioSA when a world sound is played
//
//////////////////////////////////////////////////////////////////
bool CClientGame::StaticWorldSoundHandler(const SWorldSoundEvent& event)
{
    return g_pClientGame->WorldSoundHandler(event);
}

//////////////////////////////////////////////////////////////////
//
// CClientGame::WorldSoundHandler
//
// Handle callback from CAudioSA when a world sound is played
//
//////////////////////////////////////////////////////////////////
bool CClientGame::WorldSoundHandler(const SWorldSoundEvent& event)
{
    if (m_bShowSound)
        m_pScriptDebugging->LogInformation(NULL, "%s - World sound group:%d index:%d", *GetLocalTimeString(false, true), event.uiGroup, event.uiIndex);

    // Audio events without a game entity could default to the root element, but the
    // best approach is to avoid spamming the event with the barely notable sounds (without a source).
    // Warning: Canceling sounds emitted by an audio entity (like vehicles do) will cause massive spam
    if (event.pGameEntity)
    {
        CPools*        pPools = g_pGame->GetPools();
        CClientEntity* pEntity = pPools->GetClientEntity((DWORD*)event.pGameEntity);
        if (pEntity)
        {
            CLuaArguments Arguments;
            Arguments.PushNumber(event.uiGroup);
            Arguments.PushNumber(event.uiIndex);
            Arguments.PushNumber(event.vecPosition.fX);
            Arguments.PushNumber(event.vecPosition.fY);
            Arguments.PushNumber(event.vecPosition.fZ);
            return pEntity->CallEvent("onClientWorldSound", Arguments, true);
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////
//
// CClientGame::IsUsingAlternatePulseOrder
//
// Returns true if should be using alternate pulse order
//
//////////////////////////////////////////////////////////////////
bool CClientGame::IsUsingAlternatePulseOrder(bool bAdvanceDelayCounter)
{
    if (m_MiscGameSettings.bUseAltPulseOrder)
    {
        // Only actually start using alternate pulse order after 100 frames
        if (m_uiAltPulseOrderCounter >= 100)
            return true;
        else if (bAdvanceDelayCounter)
            m_uiAltPulseOrderCounter++;
    }

    return false;
}

//////////////////////////////////////////////////////////////////
//
// CClientGame::OutputServerInfo
//
// Output info about the connected server for player
//
//////////////////////////////////////////////////////////////////
void CClientGame::OutputServerInfo()
{
    SString strTotalOutput;
    strTotalOutput += SString("Server info for %s", g_pNet->GetConnectedServer(true));
    strTotalOutput += "\n";
    strTotalOutput += SString("Ver: %s\n", *GetServerVersionSortable());
    strTotalOutput += SString("AC: %s\n", *m_strACInfo);

    {
        SString strVoice;
        if (m_pVoiceRecorder && m_pVoiceRecorder->IsEnabled())
            strVoice += SString("Enabled - Sample rate:%d  Quality:%d", m_pVoiceRecorder->GetSampleRate(), m_pVoiceRecorder->GetSampleQuality());
        else
            strVoice += "Disabled";

        strTotalOutput += SString("Voice: %s\n", *strVoice);
    }

    {
        SString     strEnabledGlitches;
        const char* szGlitchNames[] = {"Quick reload",         "Fast fire",  "Fast move", "Crouch bug", "Close damage", "Hit anim", "Fast sprint",
                                       "Bad driveby hitboxes", "Quick stand"};
        for (uint i = 0; i < NUM_GLITCHES; i++)
        {
            if (IsGlitchEnabled(i))
            {
                if (!strEnabledGlitches.empty())
                    strEnabledGlitches += ", ";
                if (i < NUMELMS(szGlitchNames))
                    strEnabledGlitches += szGlitchNames[i];
                else
                    strEnabledGlitches += SString("Unknown(#%d)", i + 1);
            }
        }
        if (strEnabledGlitches.empty())
            strEnabledGlitches = "None";
        strTotalOutput += SString("Glitches: %s\n", *strEnabledGlitches);
    }

    {
        SString strEnabledBulletSync;
        for (std::set<eWeaponType>::iterator iter = m_weaponTypesUsingBulletSync.begin(); iter != m_weaponTypesUsingBulletSync.end(); ++iter)
        {
            eWeaponType weaponType = *iter;
            if (!strEnabledBulletSync.empty())
                strEnabledBulletSync += ",";
            strEnabledBulletSync += SString("%d", weaponType);
        }
        if (strEnabledBulletSync.empty())
            strEnabledBulletSync = "None";
        strTotalOutput += SString("Bullet sync weapons: %s\n", *strEnabledBulletSync);
    }

    {
        SString strVehExtrapolate;
        if (m_VehExtrapolateSettings.bEnabled)
            strVehExtrapolate += SString("Amount:%d%%  (LimitMs:%d)", m_VehExtrapolateSettings.iScalePercent, m_VehExtrapolateSettings.iMaxMs);
        else
            strVehExtrapolate += "Disabled";

        strTotalOutput += SString("Vehicle extrapolation: %s\n", *strVehExtrapolate);
    }

    {
        SString strTickRates;
        strTickRates += SString("Plr:%d  Cam:%d  Ped:%d  UnocVeh:%d  KeyRot:%d  KeyJoy:%d", g_TickRateSettings.iPureSync, g_TickRateSettings.iCamSync,
                                g_TickRateSettings.iPedSync, g_TickRateSettings.iUnoccupiedVehicle, g_TickRateSettings.iKeySyncRotation,
                                g_TickRateSettings.iKeySyncAnalogMove);

        strTotalOutput += SString("Tick rates: %s\n", *strTickRates);
    }

    {
        SString strSyncerDists;
        strSyncerDists += SString("Ped:%d  UnoccupiedVehicle:%d ", g_TickRateSettings.iPedSyncerDistance, g_TickRateSettings.iUnoccupiedVehicleSyncerDistance);

        strTotalOutput += SString("Syncer distances: %s\n", *strSyncerDists);
    }

    g_pCore->GetConsole()->Print(strTotalOutput);
}

//////////////////////////////////////////////////////////////////
//
// CClientGame::TellServerSomethingImportant
//
// Report misc important warnings/errors to the current server
//
//////////////////////////////////////////////////////////////////
void CClientGame::TellServerSomethingImportant(uint uiId, const SString& strMessage, uint uiSendLimitForThisId)
{
    g_pCore->GetConsole()->Print(strMessage);
    AddReportLog(3400 + uiId, strMessage + g_pNet->GetConnectedServer(true), 10);

    if (uiSendLimitForThisId)
    {
        uint& uiCount = MapGet(m_SentMessageIds, uiId);
        if (uiCount++ >= uiSendLimitForThisId)
            return;
    }

    NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
    pBitStream->WriteString(SString("%d,%s", uiId, *strMessage));
    g_pNet->SendPacket(PACKET_ID_PLAYER_DIAGNOSTIC, pBitStream, PACKET_PRIORITY_MEDIUM, PACKET_RELIABILITY_UNRELIABLE);
    g_pNet->DeallocateNetBitStream(pBitStream);
}

//////////////////////////////////////////////////////////////////
//
// CClientGame::ChangeFloatPrecision
//
// Manage the change to high floating point precision
//
//////////////////////////////////////////////////////////////////
void CClientGame::ChangeFloatPrecision(bool bHigh)
{
    if (bHigh)
    {
        // Switch to 53 bit floating point precision on the first call
        if (m_uiPrecisionCallDepth++ == 0)
            _controlfp(_PC_53, MCW_PC);
    }
    else
    {
        // Even though is should never happen, m_uiPrecisionCallDepth is sometimes zero here
        dassert(m_uiPrecisionCallDepth != 0);
        if (m_uiPrecisionCallDepth != 0)
            m_uiPrecisionCallDepth--;
        // Switch back to 24 bit floating point precision on the last call
        if (m_uiPrecisionCallDepth == 0)
            _controlfp(_PC_24, MCW_PC);
    }
}

bool CClientGame::IsHighFloatPrecision() const
{
    return m_uiPrecisionCallDepth != 0;
}

AnimationId CClientGame::DrivebyAnimationHandler(AnimationId animId, AssocGroupId animGroupId)
{
    // Only apply if all clients support the fix
    if (!GetMiscGameSettings().bAllowBadDrivebyHitboxFix)
        return animId;

    // If the glitch is enabled, don't apply the fix
    if (IsGlitchEnabled(GLITCH_BADDRIVEBYHITBOX))
        return animId;

    // Bad animations are 232 and 236 of assoc group 72
    if (animGroupId != 72)
        return animId;

    if (animId == 232)
        return 235;
    else if (animId == 236)
        return 231;

    return animId;
}

//////////////////////////////////////////////////////////////////
//
// CClientGame::SetFileCacheRoot
//
// Figure out which directory to use for the client resource file cache
//
//////////////////////////////////////////////////////////////////
void CClientGame::SetFileCacheRoot()
{
    if (g_pCore->GetCVars()->GetValue<bool>("share_file_cache") == false)
    {
        // Not sharing, so use current mod directory
        m_strFileCacheRoot = GetModRoot();
        AddReportLog(7410, SString("CClientGame::SetFileCacheRoot - Not shared '%s'", *m_strFileCacheRoot));
    }
    else
    {
        // Get shared directory
        SString strFileCachePath = GetCommonRegistryValue("", "File Cache Path");
        // Check exists
        if (!strFileCachePath.empty() && DirectoryExists(strFileCachePath))
        {
            // Check writable
            SString strTestFileName = PathJoin(strFileCachePath, "resources", "_test.tmp");
            if (FileSave(strTestFileName, "x"))
            {
                FileDelete(strTestFileName);
                strTestFileName = PathJoin(strFileCachePath, "priv", "_test.tmp");
                if (FileSave(strTestFileName, "x"))
                {
                    FileDelete(strTestFileName);
                    // Use shared directory
                    m_strFileCacheRoot = strFileCachePath;
                    AddReportLog(7411, SString("CClientGame::SetFileCacheRoot - Is shared '%s'", *m_strFileCacheRoot));
                    return;
                }
            }
        }

        // Otherwise set this install mod directory as shared
        m_strFileCacheRoot = GetModRoot();
        SetCommonRegistryValue("", "File Cache Path", m_strFileCacheRoot);

        if (strFileCachePath.empty())
            AddReportLog(7412, SString("CClientGame::SetFileCacheRoot - Initial setting '%s'", *m_strFileCacheRoot));
        else
            AddReportLog(7413, SString("CClientGame::SetFileCacheRoot - Change shared from '%s' to '%s'", *strFileCachePath, *m_strFileCacheRoot));
    }
}

bool CClientGame::TriggerBrowserRequestResultEvent(const std::unordered_set<SString>& newPages)
{
    CLuaArguments Arguments;
    CLuaArguments LuaTable;
    int           i = 0;

    for (auto& domain : newPages)
    {
        LuaTable.PushNumber(++i);
        LuaTable.PushString(domain);
    }
    Arguments.PushTable(&LuaTable);

    return GetRootEntity()->CallEvent("onClientBrowserWhitelistChange", Arguments, false);
}

void CClientGame::RestreamModel(unsigned short usModel)
{
    // Is this a vehicle ID?
    if (CClientVehicleManager::IsValidModel(usModel))
    {
        // Stream the vehicles of that model out so we have no
        // loaded when we do the restore. The streamer will
        // eventually stream them back in with async loading.
        m_pManager->GetVehicleManager()->RestreamVehicles(usModel);
    }

    // Is this an object ID?
    else if (CClientObjectManager::IsValidModel(usModel))
    {
        if (CClientPedManager::IsValidWeaponModel(usModel))
        {
            // Stream the weapon of that model out so we have no
            // loaded when we do the restore. The streamer will
            // eventually stream them back in with async loading.
            m_pManager->GetPedManager()->RestreamWeapon(usModel);
            m_pManager->GetPickupManager()->RestreamPickups(usModel);
        }
        // Stream the objects of that model out so we have no
        // loaded when we do the restore. The streamer will
        // eventually stream them back in with async loading.
        m_pManager->GetObjectManager()->RestreamObjects(usModel);
        g_pGame->GetModelInfo(usModel)->RestreamIPL();
    }
    // Is this an ped ID?
    else if (CClientPlayerManager::IsValidModel(usModel))
    {
        // Stream the ped of that model out so we have no
        // loaded when we do the restore. The streamer will
        // eventually stream them back in with async loading.
        m_pManager->GetPedManager()->RestreamPeds(usModel);
    }
    else

        // 'Restream' upgrades after model replacement to propagate visual changes with immediate effect
        if (CClientObjectManager::IsValidModel(usModel) && CVehicleUpgrades::IsUpgrade(usModel))
            m_pManager->GetVehicleManager()->RestreamVehicleUpgrades(usModel);
}

void CClientGame::RestreamWorld()
{
    unsigned int numberOfFileIDs = g_pGame->GetCountOfAllFileIDs();

    for (unsigned int uiModelID = 0; uiModelID < numberOfFileIDs; uiModelID++)
    {
        g_pClientGame->GetModelCacheManager()->OnRestreamModel(uiModelID);
    }
    m_pManager->GetObjectManager()->RestreamAllObjects();
    m_pManager->GetVehicleManager()->RestreamAllVehicles();
    m_pManager->GetPedManager()->RestreamAllPeds();
    m_pManager->GetPickupManager()->RestreamAllPickups();

    g_pGame->GetStreaming()->RemoveBigBuildings();
    g_pGame->GetStreaming()->ReinitStreaming();
}

void CClientGame::ReinitMarkers()
{
    g_pGame->Get3DMarkers()->ReinitMarkers();
}

void CClientGame::ResetWorldProperties(const ResetWorldPropsInfo& resetPropsInfo)
{
    // Reset all setWorldSpecialPropertyEnabled to default
    if (resetPropsInfo.resetSpecialProperties)
    {
        g_pGame->SetCheatEnabled("hovercars", false);
        g_pGame->SetCheatEnabled("aircars", false);
        g_pGame->SetCheatEnabled("extrabunny", false);
        g_pGame->SetCheatEnabled("extrajump", false);

        g_pGame->SetRandomFoliageEnabled(true);
        g_pGame->SetMoonEasterEggEnabled(false);
        g_pGame->SetExtraAirResistanceEnabled(true);
        g_pGame->SetUnderWorldWarpEnabled(true);
        g_pGame->SetVehicleSunGlareEnabled(false);
        g_pGame->SetCoronaZTestEnabled(true);
        g_pGame->SetWaterCreaturesEnabled(true);
        g_pGame->SetBurnFlippedCarsEnabled(true);
        g_pGame->SetFireballDestructEnabled(true);
        g_pGame->SetRoadSignsTextEnabled(true);
        g_pGame->SetExtendedWaterCannonsEnabled(true);
        g_pGame->SetTunnelWeatherBlendEnabled(true);
    }

    // Reset all setWorldProperty to default
    if (resetPropsInfo.resetWorldProperties)
    {
        g_pMultiplayer->ResetAmbientColor();
        g_pMultiplayer->ResetAmbientObjectColor();
        g_pMultiplayer->ResetDirectionalColor();
        g_pMultiplayer->ResetSpriteSize();
        g_pMultiplayer->ResetSpriteBrightness();
        g_pMultiplayer->ResetPoleShadowStrength();
        g_pMultiplayer->ResetShadowStrength();
        g_pMultiplayer->ResetShadowsOffset();
        g_pMultiplayer->ResetLightsOnGroundBrightness();
        g_pMultiplayer->ResetLowCloudsColor();
        g_pMultiplayer->ResetBottomCloudsColor();
        g_pMultiplayer->ResetCloudsAlpha1();
        g_pMultiplayer->ResetIllumination();

        CWeather* weather = g_pGame->GetWeather();
        weather->ResetWetRoads();
        weather->ResetFoggyness();
        weather->ResetFog();
        weather->ResetRainFog();
        weather->ResetWaterFog();
        weather->ResetSandstorm();
        weather->ResetRainbow();
    }

    // Reset all weather stuff like heat haze, wind velocity etc
    if (resetPropsInfo.resetWeatherProperties)
    {
        g_pMultiplayer->ResetHeatHaze();
        g_pMultiplayer->RestoreFogDistance();
        g_pMultiplayer->ResetMoonSize();
        g_pMultiplayer->ResetSky();
        g_pMultiplayer->ResetSunColor();
        g_pMultiplayer->ResetSunSize();
        g_pMultiplayer->RestoreWindVelocity();
        g_pMultiplayer->ResetColorFilter();

        g_pGame->GetWeather()->ResetAmountOfRain();
    }

    // Reset LODs
    if (resetPropsInfo.resetLODs)
    {
        g_pGame->GetSettings()->ResetVehiclesLODDistance(true);
        g_pGame->GetSettings()->ResetPedsLODDistance(true);
    }

    // Reset & restore sounds
    if (resetPropsInfo.resetSounds)
    {
        g_pMultiplayer->SetInteriorSoundsEnabled(true);
        g_pGame->GetAudioEngine()->ResetAmbientSounds();
        g_pGame->GetAudioEngine()->ResetWorldSounds();
    }

    // Reset all other world stuff
    // Reset clip distances
    g_pMultiplayer->RestoreFarClipDistance();
    g_pMultiplayer->RestoreNearClipDistance();

    // Reset clouds
    g_pMultiplayer->SetCloudsEnabled(true);
    SetCloudsEnabled(true);

    // Reset birds
    g_pMultiplayer->DisableBirds(false);
    SetBirdsEnabled(true);

    // Reset occlusions
    g_pGame->GetWorld()->SetOcclusionsEnabled(true);

    // Reset gravity
    g_pMultiplayer->SetGlobalGravity(DEFAULT_GRAVITY);
    g_pCore->GetMultiplayer()->SetLocalPlayerGravity(DEFAULT_GRAVITY);
    g_pGame->SetGravity(DEFAULT_GRAVITY);

    // Reset game speed
    g_pGame->SetGameSpeed(DEFAULT_GAME_SPEED);

    // Reset aircraft max velocity & height
    g_pMultiplayer->SetAircraftMaxHeight(DEFAULT_AIRCRAFT_MAXHEIGHT);
    g_pMultiplayer->SetAircraftMaxVelocity(DEFAULT_AIRCRAFT_MAXVELOCITY);

    // Reset jetpack max height
    g_pGame->GetWorld()->SetJetpackMaxHeight(DEFAULT_JETPACK_MAXHEIGHT);

    // Restore furnitures in the interiors
    for (std::uint8_t i = 0; i <= 4; ++i)
        g_pMultiplayer->SetInteriorFurnitureEnabled(i, true);

    // Reset minute duration
    SetMinuteDuration(DEFAULT_MINUTE_DURATION);

    // Reset blur level
    g_pGame->GetSettings()->SetBlurControlledByScript(false);
    g_pGame->GetSettings()->ResetBlurEnabled();

    // Reset corona reflections
    g_pGame->GetSettings()->SetCoronaReflectionsControlledByScript(false);
    g_pGame->GetSettings()->ResetCoronaReflectionsEnabled();

    // Reset traffic lights
    g_pMultiplayer->SetTrafficLightsLocked(false);

    // Reset water color, water level & wave height
    g_pMultiplayer->ResetWater();
    GetManager()->GetWaterManager()->ResetWorldWaterLevel();
    GetManager()->GetWaterManager()->SetWaveLevel(0.0f);

    // Reset volumetric shadows
    g_pGame->GetSettings()->ResetVolumetricShadows();

    // Reset Frozen Time
    g_pGame->GetClock()->ResetTimeFrozen();

    // Reset DynamicPedShadows
    g_pGame->GetSettings()->ResetDynamicPedShadows();
}

void CClientGame::OnWindowFocusChange(bool state)
{
    if (state == m_bFocused)
        return;

    m_bFocused = state;

    CLuaArguments Arguments;
    Arguments.PushBoolean(state);
    m_pRootEntity->CallEvent("onClientMTAFocusChange", Arguments, false);
}

void CClientGame::InsertIFPPointerToMap(const unsigned int u32BlockNameHash, const std::shared_ptr<CClientIFP>& pIFP)
{
    m_mapOfIfpPointers[u32BlockNameHash] = pIFP;
}

std::shared_ptr<CClientIFP> CClientGame::GetIFPPointerFromMap(const unsigned int u32BlockNameHash)
{
    auto it = m_mapOfIfpPointers.find(u32BlockNameHash);
    if (it != m_mapOfIfpPointers.end())
    {
        return it->second;
    }
    return nullptr;
}

void CClientGame::RemoveIFPPointerFromMap(const unsigned int u32BlockNameHash)
{
    m_mapOfIfpPointers.erase(u32BlockNameHash);
}

void CClientGame::InsertPedPointerToSet(CClientPed* pPed)
{
    m_setOfPedPointers.insert(pPed);
}

void CClientGame::RemovePedPointerFromSet(CClientPed* pPed)
{
    m_setOfPedPointers.erase(pPed);
}

CClientPed* CClientGame::GetClientPedByClump(const RpClump& Clump)
{
    for (auto& pPed : m_setOfPedPointers)
    {
        CEntity* pEntity = pPed->GetGameEntity();
        if (pEntity != nullptr)
        {
            if (pEntity->GetRpClump() != nullptr)
            {
                const RpClump& entityClump = *pEntity->GetRpClump();
                if (std::addressof(entityClump) == std::addressof(Clump))
                {
                    return pPed;
                }
            }
        }
    }
    return nullptr;
}

void CClientGame::OnClientIFPUnload(const std::shared_ptr<CClientIFP>& IFP)
{
    IFP->MarkAsUnloading();
    for (auto& pPed : m_setOfPedPointers)
    {
        // Remove IFP animations from replaced animations of peds/players
        pPed->RestoreAnimations(IFP);

        // Make sure that streamed in pulses or changing model does not accidently
        // play our custom animation. We can do that by making the custom animation
        // untriggerable
        if (pPed->GetCustomAnimationBlockNameHash() == IFP->GetBlockNameHash())
        {
            if (pPed->IsCustomAnimationPlaying())
            {
                pPed->SetCustomAnimationUntriggerable();
            }

            // Important! As we are using a shared_ptr, we need to decrement the reference counter
            // by setting the shared_ptr to nullptr, this will avoid memory leak
            if (!pPed->IsNextAnimationCustom() && pPed->IsCurrentAnimationCustom())
            {
                pPed->DereferenceCustomAnimationBlock();
            }
        }
    }
}

void CClientGame::InsertAnimationAssociationToMap(CAnimBlendAssociationSAInterface* pAnimAssociation, const std::shared_ptr<CIFPAnimations>& pIFPAnimations)
{
    m_mapOfCustomAnimationAssociations[pAnimAssociation] = pIFPAnimations;
}

void CClientGame::RemoveAnimationAssociationFromMap(CAnimBlendAssociationSAInterface* pAnimAssociation)
{
    m_mapOfCustomAnimationAssociations.erase(pAnimAssociation);
}

void CClientGame::InsertRunNamedAnimTaskToMap(class CTaskSimpleRunNamedAnimSAInterface* pTask, CClientPed* pPed)
{
    m_mapOfRunNamedAnimTasks[pTask] = pPed;
}

void CClientGame::PedStepHandler(CPedSAInterface* pPedSA, bool bFoot)
{
    CLuaArguments Arguments;
    CPools*       pPools = g_pGame->GetPools();
    CClientPed*   pClientPed = DynamicCast<CClientPed>(pPools->GetClientEntity((DWORD*)pPedSA));
    if (pClientPed)
    {
        Arguments.PushBoolean(bFoot);
        pClientPed->CallEvent("onClientPedStep", Arguments, true);
    }
}

void CClientGame::VehicleWeaponHitHandler(SVehicleWeaponHitEvent& event)
{
    CPools*                    pPools = g_pGame->GetPools();
    SClientEntity<CVehicleSA>* pVehicleEntity = pPools->GetVehicle((DWORD*)event.pGameVehicle);
    if (!pVehicleEntity)
    {
        return;
    }

    CClientEntity* pVehicle = event.pGameVehicle ? reinterpret_cast<CClientVehicle*>(pVehicleEntity->pClientEntity) : nullptr;
    if (!pVehicle)
    {
        return;
    }

    CClientEntity* pEntity = event.pHitGameEntity ? pPools->GetClientEntity((DWORD*)event.pHitGameEntity) : nullptr;

    CLuaArguments arguments;
    arguments.PushNumber(static_cast<int>(event.weaponType));

    if (pEntity)
        arguments.PushElement(pEntity);
    else
        arguments.PushBoolean(false);

    arguments.PushNumber(event.vecPosition.fX);
    arguments.PushNumber(event.vecPosition.fY);
    arguments.PushNumber(event.vecPosition.fZ);
    arguments.PushNumber(event.iModel);
    arguments.PushNumber(event.iColSurface);
    pVehicle->CallEvent("onClientVehicleWeaponHit", arguments, false);
}

//////////////////////////////////////////////////////////////////
//
// CClientGame::AudioZoneRadioSwitchHandler
//
// Called when player either enter or leave audio zone which has defined radio station.
// We basically set player radio station here.
//
//////////////////////////////////////////////////////////////////
void CClientGame::AudioZoneRadioSwitchHandler(DWORD dwStationID)
{
    CClientPlayer* pPlayer = m_pPlayerManager->GetLocalPlayer();

    if (pPlayer && pPlayer->IsInVehicle())
    {
        // Do not change radio station if player is inside vehicle
        // because it is supposed to play own radio
        return;
    }

    if (dwStationID == 0)
    {
        g_pGame->GetAudioEngine()->StopRadio();
    }
    else
    {
        g_pGame->GetAudioEngine()->StartRadio(dwStationID);
    }
}
