/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CGameSA.cpp
 *  PURPOSE:     Base game logic handling
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#define ALLOC_STATS_MODULE_NAME "game_sa"
#include "SharedUtil.hpp"
#include "SharedUtil.MemAccess.hpp"
#include "C3DMarkersSA.h"
#include "CAEAudioHardwareSA.h"
#include "CAERadioTrackManagerSA.h"
#include "CAESoundManagerSA.h"
#include "CAnimManagerSA.h"
#include "CAudioContainerSA.h"
#include "CCameraSA.h"
#include "CCarEnterExitSA.h"
#include "CCheckpointsSA.h"
#include "CClockSA.h"
#include "CColStoreSA.h"
#include "CControllerConfigManagerSA.h"
#include "CCoronasSA.h"
#include "CEventListSA.h"
#include "CExplosionManagerSA.h"
#include "CFileLoaderSA.h"
#include "CFireManagerSA.h"
#include "CFxSA.h"
#include "CFxSystemSA.h"
#include "CGameSA.h"
#include "CGaragesSA.h"
#include "CHandlingManagerSA.h"
#include "CHudSA.h"
#include "CKeyGenSA.h"
#include "CObjectGroupPhysicalPropertiesSA.h"
#include "CPadSA.h"
#include "CPickupsSA.h"
#include "CPlayerInfoSA.h"
#include "CPointLightsSA.h"
#include "CProjectileInfoSA.h"
#include "CRadarSA.h"
#include "CRopesSA.h"
#include "CSettingsSA.h"
#include "CStatsSA.h"
#include "CTaskManagementSystemSA.h"
#include "CTasksSA.h"
#include "CVisibilityPluginsSA.h"
#include "CWaterManagerSA.h"
#include "CWeaponInfoSA.h"
#include "CWeaponStatManagerSA.h"
#include "CWeatherSA.h"
#include "CWorldSA.h"
#include "D3DResourceSystemSA.h"
#include "CIplStoreSA.h"
#include "CBuildingRemovalSA.h"
#include "CCheckpointSA.h"
#include "CPtrNodeSingleLinkPoolSA.h"

extern CGameSA* pGame;

unsigned int&  CGameSA::ClumpOffset = *(unsigned int*)0xB5F878;

unsigned int OBJECTDYNAMICINFO_MAX = *(uint32_t*)0x59FB4C != 0x90909090 ? *(uint32_t*)0x59FB4C : 160;            // default: 160

/**
 * \todo allow the addon to change the size of the pools (see 0x4C0270 - CPools::Initialise) (in start game?)
 */
CGameSA::CGameSA()
{
    try
    {
        pGame = this;

        // Find the game version and initialize m_eGameVersion so GetGameVersion() will return the correct value
        FindGameVersion();

        m_bAsyncScriptEnabled = false;
        m_bAsyncScriptForced = false;
        m_bASyncLoadingSuspended = false;
        m_iCheckStatus = 0;

        const unsigned int modelInfoMax = GetCountOfAllFileIDs();
        ModelInfo = new CModelInfoSA[modelInfoMax];
        ObjectGroupsInfo = new CObjectGroupPhysicalPropertiesSA[OBJECTDYNAMICINFO_MAX];

        SetInitialVirtualProtect();

        // Set the model ids for all the CModelInfoSA instances
        for (unsigned int i = 0; i < modelInfoMax; i++)
        {
            ModelInfo[i].SetModelID(i);
        }

        // Prepare all object dynamic infos for CObjectGroupPhysicalPropertiesSA instances
        for (unsigned int i = 0; i < OBJECTDYNAMICINFO_MAX; i++)
        {
            ObjectGroupsInfo[i].SetGroup(i);
        }

        m_pAudioEngine = new CAudioEngineSA((CAudioEngineSAInterface*)CLASS_CAudioEngine);
        m_pAEAudioHardware = new CAEAudioHardwareSA((CAEAudioHardwareSAInterface*)CLASS_CAEAudioHardware);
        m_pAESoundManager = new CAESoundManagerSA((CAESoundManagerSAInterface*)CLASS_CAESoundManager);
        m_pAudioContainer = new CAudioContainerSA();
        m_pWorld = new CWorldSA();
        m_Pools = std::make_unique<CPoolsSA>();
        m_pClock = new CClockSA();
        m_pRadar = new CRadarSA();
        m_pCamera = new CCameraSA((CCameraSAInterface*)CLASS_CCamera);
        m_pCoronas = new CCoronasSA();
        m_pCheckpoints = new CCheckpointsSA();
        m_pPickups = new CPickupsSA();
        m_pExplosionManager = new CExplosionManagerSA();
        m_pHud = new CHudSA();
        m_pFireManager = new CFireManagerSA();
        m_p3DMarkers = new C3DMarkersSA();
        m_pPad = new CPadSA((CPadSAInterface*)CLASS_CPad);
        m_pCAERadioTrackManager = new CAERadioTrackManagerSA();
        m_pWeather = new CWeatherSA();
        m_pStats = new CStatsSA();
        m_pTaskManagementSystem = new CTaskManagementSystemSA();
        m_pSettings = new CSettingsSA();
        m_pCarEnterExit = new CCarEnterExitSA();
        m_pControllerConfigManager = new CControllerConfigManagerSA();
        m_pProjectileInfo = new CProjectileInfoSA();
        m_pRenderWare = new CRenderWareSA();
        m_HandlingManager = std::make_unique<CHandlingManagerSA>();
        m_pEventList = new CEventListSA();
        m_pGarages = new CGaragesSA((CGaragesSAInterface*)CLASS_CGarages);
        m_pTasks = new CTasksSA((CTaskManagementSystemSA*)m_pTaskManagementSystem);
        m_pAnimManager = new CAnimManagerSA;
        m_pStreaming = new CStreamingSA;
        m_pVisibilityPlugins = new CVisibilityPluginsSA;
        m_pKeyGen = new CKeyGenSA;
        m_pRopes = new CRopesSA;
        m_pFx = new CFxSA((CFxSAInterface*)CLASS_CFx);
        m_pFxManager = new CFxManagerSA((CFxManagerSAInterface*)CLASS_CFxManager);
        m_pWaterManager = new CWaterManagerSA();
        m_pWeaponStatsManager = new CWeaponStatManagerSA();
        m_pPointLights = new CPointLightsSA();
        m_collisionStore = new CColStoreSA();
        m_pIplStore = new CIplStoreSA();
        m_pCoverManager = new CCoverManagerSA();
        m_pPlantManager = new CPlantManagerSA();
        m_pBuildingRemoval = new CBuildingRemovalSA();

        m_pRenderer = std::make_unique<CRendererSA>();

        // Normal weapon types (WEAPONSKILL_STD)
        for (int i = 0; i < NUM_WeaponInfosStdSkill; i++)
        {
            eWeaponType weaponType = (eWeaponType)(WEAPONTYPE_PISTOL + i);
            WeaponInfos[i] = new CWeaponInfoSA((CWeaponInfoSAInterface*)(ARRAY_WeaponInfo + i * CLASSSIZE_WeaponInfo), weaponType);
            m_pWeaponStatsManager->CreateWeaponStat(WeaponInfos[i], (eWeaponType)(weaponType - WEAPONTYPE_PISTOL), WEAPONSKILL_STD);
        }

        // Extra weapon types for skills (WEAPONSKILL_POOR,WEAPONSKILL_PRO,WEAPONSKILL_SPECIAL)
        int          index;
        eWeaponSkill weaponSkill = eWeaponSkill::WEAPONSKILL_POOR;
        for (int skill = 0; skill < 3; skill++)
        {
            // STD is created first, then it creates "extra weapon types" (poor, pro, special?) but in the enum 1 = STD which meant the STD weapon skill
            // contained pro info
            if (skill >= 1)
            {
                if (skill == 1)
                {
                    weaponSkill = eWeaponSkill::WEAPONSKILL_PRO;
                }
                if (skill == 2)
                {
                    weaponSkill = eWeaponSkill::WEAPONSKILL_SPECIAL;
                }
            }
            for (int i = 0; i < NUM_WeaponInfosOtherSkill; i++)
            {
                eWeaponType weaponType = (eWeaponType)(WEAPONTYPE_PISTOL + i);
                index = NUM_WeaponInfosStdSkill + skill * NUM_WeaponInfosOtherSkill + i;
                WeaponInfos[index] = new CWeaponInfoSA((CWeaponInfoSAInterface*)(ARRAY_WeaponInfo + index * CLASSSIZE_WeaponInfo), weaponType);
                m_pWeaponStatsManager->CreateWeaponStat(WeaponInfos[index], weaponType, weaponSkill);
            }
        }

        m_pPlayerInfo = new CPlayerInfoSA((CPlayerInfoSAInterface*)CLASS_CPlayerInfo);

        // Init cheat name => address map
        m_Cheats[CHEAT_HOVERINGCARS] = new SCheatSA((BYTE*)VAR_HoveringCarsEnabled);
        m_Cheats[CHEAT_FLYINGCARS] = new SCheatSA((BYTE*)VAR_FlyingCarsEnabled);
        m_Cheats[CHEAT_EXTRABUNNYHOP] = new SCheatSA((BYTE*)VAR_ExtraBunnyhopEnabled);
        m_Cheats[CHEAT_EXTRAJUMP] = new SCheatSA((BYTE*)VAR_ExtraJumpEnabled);

        // New cheats for Anticheat
        m_Cheats[CHEAT_TANKMODE] = new SCheatSA((BYTE*)VAR_TankModeEnabled, false);
        m_Cheats[CHEAT_NORELOAD] = new SCheatSA((BYTE*)VAR_NoReloadEnabled, false);
        m_Cheats[CHEAT_PERFECTHANDLING] = new SCheatSA((BYTE*)VAR_PerfectHandling, false);
        m_Cheats[CHEAT_ALLCARSHAVENITRO] = new SCheatSA((BYTE*)VAR_AllCarsHaveNitro, false);
        m_Cheats[CHEAT_BOATSCANFLY] = new SCheatSA((BYTE*)VAR_BoatsCanFly, false);
        m_Cheats[CHEAT_INFINITEOXYGEN] = new SCheatSA((BYTE*)VAR_InfiniteOxygen, false);
        m_Cheats[CHEAT_WALKUNDERWATER] = new SCheatSA((BYTE*)VAR_WalkUnderwater, false);
        m_Cheats[CHEAT_FASTERCLOCK] = new SCheatSA((BYTE*)VAR_FasterClock, false);
        m_Cheats[CHEAT_FASTERGAMEPLAY] = new SCheatSA((BYTE*)VAR_FasterGameplay, false);
        m_Cheats[CHEAT_SLOWERGAMEPLAY] = new SCheatSA((BYTE*)VAR_SlowerGameplay, false);
        m_Cheats[CHEAT_ALWAYSMIDNIGHT] = new SCheatSA((BYTE*)VAR_AlwaysMidnight, false);
        m_Cheats[CHEAT_FULLWEAPONAIMING] = new SCheatSA((BYTE*)VAR_FullWeaponAiming, false);
        m_Cheats[CHEAT_INFINITEHEALTH] = new SCheatSA((BYTE*)VAR_InfiniteHealth, false);
        m_Cheats[CHEAT_NEVERWANTED] = new SCheatSA((BYTE*)VAR_NeverWanted, false);
        m_Cheats[CHEAT_HEALTARMORMONEY] = new SCheatSA((BYTE*)VAR_HealthArmorMoney, false);

        // Change pool sizes here
        m_Pools->SetPoolCapacity(TASK_POOL, 5000);                                               // Default is 500
        m_Pools->SetPoolCapacity(OBJECT_POOL, MAX_OBJECTS);                                      // Default is 350
        m_Pools->SetPoolCapacity(EVENT_POOL, 5000);                                              // Default is 200
        m_Pools->SetPoolCapacity(COL_MODEL_POOL, 12000);                                         // Default is 10150
        m_Pools->SetPoolCapacity(ENV_MAP_MATERIAL_POOL, 16000);                                  // Default is 4096
        m_Pools->SetPoolCapacity(ENV_MAP_ATOMIC_POOL, 4000);                                     // Default is 1024
        m_Pools->SetPoolCapacity(SPEC_MAP_MATERIAL_POOL, 16000);                                 // Default is 4096
        m_Pools->SetPoolCapacity(ENTRY_INFO_NODE_POOL, MAX_ENTRY_INFO_NODES);                    // Default is 500
        m_Pools->SetPoolCapacity(POINTER_SINGLE_LINK_POOL, MAX_POINTER_SINGLE_LINKS);            // Default is 70000
        m_Pools->SetPoolCapacity(POINTER_DOUBLE_LINK_POOL, MAX_POINTER_DOUBLE_LINKS);            // Default is 3200
        dassert(m_Pools->GetPoolCapacity(POINTER_SINGLE_LINK_POOL) == MAX_POINTER_SINGLE_LINKS);

        // Increase streaming object instances list size
        MemPut<WORD>(0x05B8E55, MAX_RWOBJECT_INSTANCES * 12);            // Default is 1000 * 12
        MemPut<WORD>(0x05B8EB0, MAX_RWOBJECT_INSTANCES * 12);            // Default is 1000 * 12

        // Increase matrix array size
        MemPut<int>(0x054F3A1, MAX_OBJECTS * 3);            // Default is 900

        CEntitySAInterface::StaticSetHooks();
        CPhysicalSAInterface::StaticSetHooks();
        CObjectSA::StaticSetHooks();
        CModelInfoSA::StaticSetHooks();
        CPlayerPedSA::StaticSetHooks();
        CRenderWareSA::StaticSetHooks();
        CRenderWareSA::StaticSetClothesReplacingHooks();
        CTasksSA::StaticSetHooks();
        CPedSA::StaticSetHooks();
        CSettingsSA::StaticSetHooks();
        CFxSystemSA::StaticSetHooks();
        CFileLoaderSA::StaticSetHooks();
        D3DResourceSystemSA::StaticSetHooks();
        CVehicleSA::StaticSetHooks();
        CCheckpointSA::StaticSetHooks();
        CHudSA::StaticSetHooks();
        CFireSA::StaticSetHooks();
        CPtrNodeSingleLinkPoolSA::StaticSetHooks();
    }
    catch (const std::bad_alloc& e)
    {
        std::string error = _("Failed initialization game_sa");
        error += "\n";
        error += _("Memory allocations failed");
        error += ": ";
        error += e.what();

        MessageBoxUTF8(nullptr, error, _("Error"), MB_ICONERROR | MB_OK);
        ExitProcess(EXIT_FAILURE);
    }
    catch (const std::exception& e)
    {
        std::string error = _("Failed initialization game_sa");
        error += "\n";
        error += _("Information");
        error += ": ";
        error += e.what();

        MessageBoxUTF8(nullptr, error, _("Error"), MB_ICONERROR | MB_OK);
        ExitProcess(EXIT_FAILURE);
    }
}

CGameSA::~CGameSA()
{
    delete reinterpret_cast<CPlayerInfoSA*>(m_pPlayerInfo);

    for (int i = 0; i < NUM_WeaponInfosTotal; i++)
    {
        delete reinterpret_cast<CWeaponInfoSA*>(WeaponInfos[i]);
    }

    delete reinterpret_cast<CFxSA*>(m_pFx);
    delete reinterpret_cast<CRopesSA*>(m_pRopes);
    delete reinterpret_cast<CKeyGenSA*>(m_pKeyGen);
    delete reinterpret_cast<CVisibilityPluginsSA*>(m_pVisibilityPlugins);
    delete reinterpret_cast<CStreamingSA*>(m_pStreaming);
    delete reinterpret_cast<CAnimManagerSA*>(m_pAnimManager);
    delete reinterpret_cast<CTasksSA*>(m_pTasks);
    delete reinterpret_cast<CTaskManagementSystemSA*>(m_pTaskManagementSystem);
    delete reinterpret_cast<CStatsSA*>(m_pStats);
    delete reinterpret_cast<CWeatherSA*>(m_pWeather);
    delete reinterpret_cast<CAERadioTrackManagerSA*>(m_pCAERadioTrackManager);
    delete reinterpret_cast<CPadSA*>(m_pPad);
    delete reinterpret_cast<C3DMarkersSA*>(m_p3DMarkers);
    delete reinterpret_cast<CFireManagerSA*>(m_pFireManager);
    delete reinterpret_cast<CHudSA*>(m_pHud);
    delete reinterpret_cast<CExplosionManagerSA*>(m_pExplosionManager);
    delete reinterpret_cast<CPickupsSA*>(m_pPickups);
    delete reinterpret_cast<CCheckpointsSA*>(m_pCheckpoints);
    delete reinterpret_cast<CCoronasSA*>(m_pCoronas);
    delete reinterpret_cast<CCameraSA*>(m_pCamera);
    delete reinterpret_cast<CRadarSA*>(m_pRadar);
    delete reinterpret_cast<CClockSA*>(m_pClock);
    delete reinterpret_cast<CWorldSA*>(m_pWorld);
    delete reinterpret_cast<CAudioEngineSA*>(m_pAudioEngine);
    delete reinterpret_cast<CAEAudioHardwareSA*>(m_pAEAudioHardware);
    delete reinterpret_cast<CAudioContainerSA*>(m_pAudioContainer);
    delete reinterpret_cast<CPointLightsSA*>(m_pPointLights);
    delete static_cast<CColStoreSA*>(m_collisionStore);
    delete static_cast<CIplStore*>(m_pIplStore);
    delete static_cast<CBuildingRemovalSA*>(m_pBuildingRemoval);
    delete m_pCoverManager;
    delete m_pPlantManager;

    delete[] ModelInfo;
    delete[] ObjectGroupsInfo;
}

CWeaponInfo* CGameSA::GetWeaponInfo(eWeaponType weapon, eWeaponSkill skill)
{
    if ((skill == WEAPONSKILL_STD && weapon >= WEAPONTYPE_UNARMED && weapon < WEAPONTYPE_LAST_WEAPONTYPE) ||
        (skill != WEAPONSKILL_STD && weapon >= WEAPONTYPE_PISTOL && weapon <= WEAPONTYPE_TEC9))
    {
        int offset = 0;
        switch (skill)
        {
            case WEAPONSKILL_STD:
                offset = 0;
                break;
            case WEAPONSKILL_POOR:
                offset = NUM_WeaponInfosStdSkill - WEAPONTYPE_PISTOL;
                break;
            case WEAPONSKILL_PRO:
                offset = NUM_WeaponInfosStdSkill + NUM_WeaponInfosOtherSkill - WEAPONTYPE_PISTOL;
                break;
            case WEAPONSKILL_SPECIAL:
                offset = NUM_WeaponInfosStdSkill + 2 * NUM_WeaponInfosOtherSkill - WEAPONTYPE_PISTOL;
                break;
            default:
                break;
        }
        return WeaponInfos[offset + weapon];
    }
    else
        return NULL;
}

void CGameSA::Pause(bool bPaused)
{
    MemPutFast<bool>(0xB7CB49, bPaused);            // CTimer::m_UserPause
}

CModelInfo* CGameSA::GetModelInfo(DWORD dwModelID, bool bCanBeInvalid)
{
    if (dwModelID < GetCountOfAllFileIDs())
    {
        if (ModelInfo[dwModelID].IsValid() || bCanBeInvalid)
        {
            return &ModelInfo[dwModelID];
        }
        return nullptr;
    }
    return nullptr;
}

/**
 * Starts the game
 * \todo make addresses into constants
 */
void CGameSA::StartGame()
{
    SetSystemState(GS_INIT_PLAYING_GAME);
    MemPutFast<BYTE>(0xB7CB49, 0);            // CTimer::m_UserPause
    MemPutFast<BYTE>(0xBA67A4, 0);            // FrontEndMenuManager + 0x5C
}

/**
 * Sets the part of the game loading process the game is in.
 * @param dwState DWORD containing a valid state 0 - 9
 */
void CGameSA::SetSystemState(eSystemState State)
{
    MemPutFast<DWORD>(0xC8D4C0, State); // gGameState
}

eSystemState CGameSA::GetSystemState()
{
    return *(eSystemState*)0xC8D4C0; // gGameState
}

/**
 * This adds the local player to the ped pool, nothing else
 * @return BOOL TRUE if success, FALSE otherwise
 */
bool CGameSA::InitLocalPlayer(CClientPed* pClientPed)
{
    CPoolsSA* pools = (CPoolsSA*)GetPools();
    if (pools)
    {
        //* HACKED IN HERE FOR NOW *//
        CPedSAInterface* pInterface = pools->GetPedInterface((DWORD)1);

        if (pInterface)
        {
            pools->ResetPedPoolCount();
            pools->AddPed(pClientPed, (DWORD*)pInterface);
            return TRUE;
        }

        return false;
    }
    return true;
}

float CGameSA::GetGravity()
{
    return *(float*)(0x863984);
}

void CGameSA::SetGravity(float fGravity)
{
    MemPut<float>(0x863984, fGravity);
}

float CGameSA::GetGameSpeed()
{
    return *(float*)(0xB7CB64);
}

void CGameSA::SetGameSpeed(float fSpeed)
{
    MemPutFast<float>(0xB7CB64, fSpeed);
}

void CGameSA::Reset()
{
    // Things to do if the game was loaded
    if (GetSystemState() == GS_PLAYING_GAME)
    {
        // Extinguish all fires
        m_pFireManager->ExtinguishAllFires();

        // Restore camera stuff
        m_pCamera->Restore();
        m_pCamera->SetFadeColor(0, 0, 0);
        m_pCamera->Fade(0, FADE_OUT);

        Pause(false);            // We don't have to pause as the fadeout will stop the sound. Pausing it will make the fadein next start ugly
        m_pHud->Disable(false);

        // Restore the HUD
        m_pHud->Disable(false);
        m_pHud->SetComponentVisible(HUD_ALL, true);

        // Restore model dummies' positions
        CModelInfoSA::ResetAllVehicleDummies();
        CModelInfoSA::RestoreAllObjectsPropertiesGroups();
        // restore default properties of all CObjectGroupPhysicalPropertiesSA instances
        CObjectGroupPhysicalPropertiesSA::RestoreDefaultValues();

        // Restore vehicle model wheel sizes
        CModelInfoSA::ResetAllVehiclesWheelSizes();

        // Restore changed TXD IDs
        CModelInfoSA::StaticResetTextureDictionaries();

        // Restore default world state
        RestoreGameWorld();
    }
}

void CGameSA::Terminate()
{
    // Initiate the destruction
    delete this;

    // Dump any memory leaks if DETECT_LEAK is defined
    #ifdef DETECT_LEAKS
    DumpUnfreed();
    #endif
}

void CGameSA::Initialize()
{
    // Initialize garages
    m_pGarages->Initialize();
    SetupSpecialCharacters();
    SetupBrokenModels();
    m_pRenderWare->Initialize();

    // *Sebas* Hide the GTA:SA Main menu.
    MemPutFast<BYTE>(CLASS_CMenuManager + 0x5C, 0);
}

eGameVersion CGameSA::GetGameVersion()
{
    return m_eGameVersion;
}

eGameVersion CGameSA::FindGameVersion()
{
    unsigned char ucA = *reinterpret_cast<unsigned char*>(0x748ADD);
    unsigned char ucB = *reinterpret_cast<unsigned char*>(0x748ADE);
    if (ucA == 0xFF && ucB == 0x53)
    {
        m_eGameVersion = VERSION_US_10;
    }
    else if (ucA == 0x0F && ucB == 0x84)
    {
        m_eGameVersion = VERSION_EU_10;
    }
    else if (ucA == 0xFE && ucB == 0x10)
    {
        m_eGameVersion = VERSION_11;
    }
    else
    {
        m_eGameVersion = VERSION_UNKNOWN;
    }

    return m_eGameVersion;
}

float CGameSA::GetFPS()
{
    return *(float*)0xB7CB50; // CTimer::game_FPS
}

float CGameSA::GetTimeStep()
{
    return *(float*)0xB7CB5C; // CTimer::ms_fTimeStep
}

float CGameSA::GetOldTimeStep()
{
    return *(float*)0xB7CB54; // CTimer::ms_fOldTimeStep
}

float CGameSA::GetTimeScale()
{
    return *(float*)0xB7CB64; // CTimer::ms_fTimeScale
}

void CGameSA::SetTimeScale(float fTimeScale)
{
    MemPutFast<float>(0xB7CB64, fTimeScale); // CTimer::ms_fTimeScale
}

unsigned char CGameSA::GetBlurLevel()
{
    return *(unsigned char*)0x8D5104;            // CPostEffects::m_SpeedFXAlpha
}

void CGameSA::SetBlurLevel(unsigned char ucLevel)
{
    MemPutFast<unsigned char>(0x8D5104, ucLevel);            // CPostEffects::m_SpeedFXAlpha
}

unsigned long CGameSA::GetMinuteDuration()
{
    return *(unsigned long*)0xB7015C;            // CClock::ms_nMillisecondsPerGameMinute
}

void CGameSA::SetMinuteDuration(unsigned long ulTime)
{
    MemPutFast<unsigned long>(0xB7015C, ulTime);            // CClock::ms_nMillisecondsPerGameMinute
}

bool CGameSA::IsCheatEnabled(const char* szCheatName)
{
    std::map<std::string, SCheatSA*>::iterator it = m_Cheats.find(szCheatName);
    if (it == m_Cheats.end())
        return false;
    return *(it->second->m_byAddress) != 0;
}

bool CGameSA::SetCheatEnabled(const char* szCheatName, bool bEnable)
{
    std::map<std::string, SCheatSA*>::iterator it = m_Cheats.find(szCheatName);
    if (it == m_Cheats.end())
        return false;
    if (!it->second->m_bCanBeSet)
        return false;
    MemPutFast<BYTE>(it->second->m_byAddress, bEnable);
    it->second->m_bEnabled = bEnable;
    return true;
}

void CGameSA::ResetCheats()
{
    std::map<std::string, SCheatSA*>::iterator it;
    for (it = m_Cheats.begin(); it != m_Cheats.end(); it++)
    {
        if (it->second->m_byAddress > (BYTE*)0x8A4000)
            MemPutFast<BYTE>(it->second->m_byAddress, 0);
        else
            MemPut<BYTE>(it->second->m_byAddress, 0);
        it->second->m_bEnabled = false;
    }
}

bool CGameSA::IsRandomFoliageEnabled()
{
    return *(unsigned char*)0x5DD01B == 0x74;
}

void CGameSA::SetRandomFoliageEnabled(bool bEnabled)
{
    // 0xEB skip random foliage generation
    MemPut<BYTE>(0x5DD01B, bEnabled ? 0x74 : 0xEB);
    // 0x74 destroy random foliage loaded
    MemPut<BYTE>(0x5DC536, bEnabled ? 0x75 : 0x74);
}

bool CGameSA::IsMoonEasterEggEnabled()
{
    return *(unsigned char*)0x73ABCF == 0x75;
}

void CGameSA::SetMoonEasterEggEnabled(bool bEnable)
{
    // replace JNZ with JMP (short)
    MemPut<BYTE>(0x73ABCF, bEnable ? 0x75 : 0xEB);
}

bool CGameSA::IsExtraAirResistanceEnabled()
{
    return *(unsigned char*)0x72DDD9 == 0x01;
}

void CGameSA::SetExtraAirResistanceEnabled(bool bEnable)
{
    MemPut<BYTE>(0x72DDD9, bEnable ? 0x01 : 0x00);
}

void CGameSA::SetUnderWorldWarpEnabled(bool bEnable)
{
    m_bUnderworldWarp = !bEnable;
}

bool CGameSA::IsUnderWorldWarpEnabled()
{
    return !m_bUnderworldWarp;
}

bool CGameSA::GetJetpackWeaponEnabled(eWeaponType weaponType)
{
    if (weaponType >= WEAPONTYPE_BRASSKNUCKLE && weaponType < WEAPONTYPE_LAST_WEAPONTYPE)
    {
        return m_JetpackWeapons[weaponType];
    }
    return false;
}

void CGameSA::SetJetpackWeaponEnabled(eWeaponType weaponType, bool bEnabled)
{
    if (weaponType >= WEAPONTYPE_BRASSKNUCKLE && weaponType < WEAPONTYPE_LAST_WEAPONTYPE)
    {
        m_JetpackWeapons[weaponType] = bEnabled;
    }
}

void CGameSA::SetVehicleSunGlareEnabled(bool bEnabled)
{
    // State turning will be handled in hooks handler
    CVehicleSA::SetVehiclesSunGlareEnabled(bEnabled);
}

bool CGameSA::IsVehicleSunGlareEnabled()
{
    return CVehicleSA::GetVehiclesSunGlareEnabled();
}

void CGameSA::SetCoronaZTestEnabled(bool isEnabled)
{
    if (m_isCoronaZTestEnabled == isEnabled)
        return;

    if (isEnabled)
    {
        // Enable ZTest (PC)
        MemPut<BYTE>(0x6FB17C + 0, 0xFF);
        MemPut<BYTE>(0x6FB17C + 1, 0x51);
        MemPut<BYTE>(0x6FB17C + 2, 0x20);
    }
    else
    {
        // Disable ZTest (PS2)
        MemSet((void*)0x6FB17C, 0x90, 3);
    }

    m_isCoronaZTestEnabled = isEnabled;
}

void CGameSA::SetWaterCreaturesEnabled(bool isEnabled) 
{
    if (isEnabled == m_areWaterCreaturesEnabled)
        return;

    const auto manager = reinterpret_cast<class WaterCreatureManager_c*>(0xC1DF30);
    if (isEnabled)
    {
        unsigned char(__thiscall * Init)(WaterCreatureManager_c*) = reinterpret_cast<decltype(Init)>(0x6E3F90);
        Init(manager);
    }
    else
    {
        void(__thiscall * Exit)(WaterCreatureManager_c*) = reinterpret_cast<decltype(Exit)>(0x6E3FD0);
        Exit(manager);
    }

    m_areWaterCreaturesEnabled = isEnabled;
}

void CGameSA::SetTunnelWeatherBlendEnabled(bool isEnabled)
{
    if (isEnabled == m_isTunnelWeatherBlendEnabled)
        return;
    // CWeather::UpdateInTunnelness
    DWORD functionAddress = 0x72B630; 
    if (isEnabled)
    {
        // Restore original bytes: 83 EC 20
        MemPut<BYTE>(functionAddress, 0x83);                // Restore 83
        MemPut<BYTE>(functionAddress + 1, 0xEC);            // Restore EC
        MemPut<BYTE>(functionAddress + 2, 0x20);            // Restore 20
    }
    else
    {
        // Patch CWeather::UpdateInTunnelness               (Found By AlexTMjugador)
        MemPut<BYTE>(functionAddress, 0xC3);                // Write C3 (RET)
        MemPut<BYTE>(functionAddress + 1, 0x90);            // Write 90 (NOP)
        MemPut<BYTE>(functionAddress + 2, 0x90);            // Write 90 (NOP)
    }
    m_isTunnelWeatherBlendEnabled = isEnabled;
}

void CGameSA::SetBurnFlippedCarsEnabled(bool isEnabled)
{
    if (isEnabled == m_isBurnFlippedCarsEnabled)
        return;

    // CAutomobile::VehicleDamage
    if (isEnabled)
    {
        BYTE originalCodes[6] = {0xD9, 0x9E, 0xC0, 0x04, 0x00, 0x00};
        MemCpy((void*)0x6A776B, &originalCodes, 6);
    }
    else
    {
        BYTE newCodes[6] = {0xD8, 0xDD, 0x90, 0x90, 0x90, 0x90};
        MemCpy((void*)0x6A776B, &newCodes, 6);
    }

    // CPlayerInfo::Process
    if (isEnabled)
    {
        BYTE originalCodes[6] = {0xD9, 0x99, 0xC0, 0x04, 0x00, 0x00};
        MemCpy((void*)0x570E7F, &originalCodes, 6);
    }
    else
    {
        BYTE newCodes[6] = {0xD8, 0xDD, 0x90, 0x90, 0x90, 0x90};
        MemCpy((void*)0x570E7F, &newCodes, 6);
    }

    m_isBurnFlippedCarsEnabled = isEnabled;
}

void CGameSA::SetFireballDestructEnabled(bool isEnabled)
{
    if (isEnabled == m_isFireballDestructEnabled)
        return;

    if (isEnabled)
    {
        BYTE originalCodes[7] = {0x81, 0x66, 0x1C, 0x7E, 0xFF, 0xFF, 0xFF};
        MemCpy((void*)0x6CCE45, &originalCodes, 7); // CPlane::BlowUpCar
        MemCpy((void*)0x6C6E01, &originalCodes, 7); // CHeli::BlowUpCar
    }
    else
    {
        MemSet((void*)0x6CCE45, 0x90, 7); // CPlane::BlowUpCar
        MemSet((void*)0x6C6E01, 0x90, 7); // CHeli::BlowUpCar
    }

    m_isFireballDestructEnabled = isEnabled;
}

void CGameSA::SetExtendedWaterCannonsEnabled(bool isEnabled)
{
    if (isEnabled == m_isExtendedWaterCannonsEnabled)
        return;

    // Allocate memory for new bigger array or use default aCannons array
    void* aCannons = isEnabled ? malloc(MAX_WATER_CANNONS * SIZE_CWaterCannon) : (void*)ARRAY_aCannons;

    int newLimit = isEnabled ? MAX_WATER_CANNONS : NUM_CWaterCannon_DefaultLimit; // default: 3
    MemSetFast(aCannons, 0, newLimit * SIZE_CWaterCannon); // clear aCannons array

    // Get current limit
    int currentLimit = *(int*)NUM_WaterCannon_Limit;

    // Get current aCannons array
    void* currentACannons = *(void**)ARRAY_aCannons_CurrentPtr;

    // Call CWaterCannon destructor
    for (int i = 0; i < currentLimit; i++)
    {
        char* currentCannon = (char*)currentACannons + i * SIZE_CWaterCannon;

        ((void(__thiscall*)(int, void*, bool))FUNC_CAESoundManager_CancelSoundsOwnedByAudioEntity)(STRUCT_CAESoundManager, currentCannon + NUM_CWaterCannon_Audio_Offset, true); // CAESoundManager::CancelSoundsOwnedByAudioEntity to prevent random crashes from CAESound::UpdateParameters
        ((void(__thiscall*)(void*))FUNC_CWaterCannon_Destructor)(currentCannon); // CWaterCannon::~CWaterCannon
    }

    // Call CWaterCannon constructor & CWaterCannon::Init
    for (int i = 0; i < newLimit; ++i)
    {
        char* currentCannon = (char*)aCannons + i * SIZE_CWaterCannon;

        ((void(__thiscall*)(void*))FUNC_CWaterCannon_Constructor)(currentCannon); // CWaterCannon::CWaterCannon
        ((void(__thiscall*)(void*))FUNC_CWaterCannon_Init)(currentCannon); // CWaterCannon::Init
    }

    // Patch references to array
    MemPut((void*)0x728C83, aCannons);                // CWaterCannons::Init
    MemPut((void*)0x728CCB, aCannons);                // CWaterCannons::UpdateOne
    MemPut((void*)0x728CEB, aCannons);                // CWaterCannons::UpdateOne
    MemPut((void*)0x728D0D, aCannons);                // CWaterCannons::UpdateOne
    MemPut((void*)0x728D71, aCannons);                // CWaterCannons::UpdateOne
    MemPutFast((void*)0x729B33, aCannons);            // CWaterCannons::Render
    MemPut((void*)0x72A3C5, aCannons);                // CWaterCannons::UpdateOne
    MemPut((void*)0x855432, aCannons);                // 0x855431
    MemPut((void*)0x856BFD, aCannons);                // 0x856BFC

    // CWaterCannons::Init
    MemPut<BYTE>(0x728C88, newLimit);

    // CWaterCannons::Update
    MemPut<BYTE>(0x72A3F2, newLimit);

    // CWaterCanons::UpdateOne
    MemPut<BYTE>(0x728CD4, newLimit);
    MemPut<BYTE>(0x728CF6, newLimit);
    MemPut<BYTE>(0x728CFF, newLimit);
    MemPut<BYTE>(0x728D62, newLimit);

    // CWaterCannons::Render
    MemPutFast<BYTE>(0x729B38, newLimit);

    // 0x85542A
    MemPut<BYTE>(0x85542B, newLimit);

    // 0x856BF5
    MemPut<BYTE>(0x856BF6, newLimit);

    // Free previous allocated memory
    if (!isEnabled && currentACannons != nullptr)
        free(currentACannons);

    m_isExtendedWaterCannonsEnabled = isEnabled;
}

void CGameSA::SetRoadSignsTextEnabled(bool isEnabled)
{
    if (isEnabled == m_isRoadSignsTextEnabled)
        return;

    // Skip JMP to CCustomRoadsignMgr::RenderRoadsignAtomic
    MemPut<BYTE>(0x5342ED, isEnabled ? 0xEB : 0x74);

    m_isRoadSignsTextEnabled = isEnabled;
}

void CGameSA::SetIgnoreFireStateEnabled(bool isEnabled)
{
    if (isEnabled == m_isIgnoreFireStateEnabled)
        return;

    if (isEnabled)
    {
        MemSet((void*)0x6511B9, 0x90, 10);            // CCarEnterExit::IsVehicleStealable
        MemSet((void*)0x643A95, 0x90, 14);            // CTaskComplexEnterCar::CreateFirstSubTask
        MemSet((void*)0x6900B5, 0x90, 14);            // CTaskComplexCopInCar::ControlSubTask
        MemSet((void*)0x64F3DB, 0x90, 14);            // CCarEnterExit::IsPlayerToQuitCarEnter

        MemSet((void*)0x685A7F, 0x90, 14);            // CTaskSimplePlayerOnFoot::ProcessPlayerWeapon
    }
    else
    {
        // Restore original bytes
        MemCpy((void*)0x6511B9, "\x88\x86\x90\x04\x00\x00\x85\xC0\x75\x3E", 10);
        MemCpy((void*)0x643A95, "\x8B\x88\x90\x04\x00\x00\x85\xC9\x0F\x85\x99\x01\x00\x00", 14);
        MemCpy((void*)0x6900B5, "\x8B\x81\x90\x04\x00\x00\x85\xC0\x0F\x85\x1A\x01\x00\x00", 14);
        MemCpy((void*)0x64F3DB, "\x8B\x85\x90\x04\x00\x00\x85\xC0\x0F\x85\x1B\x01\x00\x00", 14);

        MemCpy((void*)0x685A7F, "\x8B\x86\x30\x07\x00\x00\x85\xC0\x0F\x85\x1D\x01\x00\x00", 14);
    }

    m_isIgnoreFireStateEnabled = isEnabled;
}

bool CGameSA::PerformChecks()
{
    std::map<std::string, SCheatSA*>::iterator it;
    for (it = m_Cheats.begin(); it != m_Cheats.end(); it++)
    {
        if (*(it->second->m_byAddress) != BYTE(it->second->m_bEnabled))
            return false;
    }
    return true;
}
bool CGameSA::VerifySADataFileNames()
{
    return !strcmp(*(char**)0x5B65AE, "DATA\\CARMODS.DAT") && !strcmp(*(char**)0x5BD839, "DATA") && !strcmp(*(char**)0x5BD84C, "HANDLING.CFG") &&
           !strcmp(*(char**)0x5BEEE8, "DATA\\melee.dat") && !strcmp(*(char**)0x4D563E, "ANIM\\PED.IFP") && !strcmp(*(char**)0x5B925B, "DATA\\OBJECT.DAT") &&
           !strcmp(*(char**)0x55D0FC, "data\\surface.dat") && !strcmp(*(char**)0x55F2BB, "data\\surfaud.dat") &&
           !strcmp(*(char**)0x55EB9E, "data\\surfinfo.dat") && !strcmp(*(char**)0x6EAEF8, "DATA\\water.dat") &&
           !strcmp(*(char**)0x6EAEC3, "DATA\\water1.dat") && !strcmp(*(char**)0x5BE686, "DATA\\WEAPON.DAT");
}

void CGameSA::SetAsyncLoadingFromScript(bool bScriptEnabled, bool bScriptForced)
{
    m_bAsyncScriptEnabled = bScriptEnabled;
    m_bAsyncScriptForced = bScriptForced;
}

void CGameSA::SuspendASyncLoading(bool bSuspend, uint uiAutoUnsuspendDelay)
{
    m_bASyncLoadingSuspended = bSuspend;
    // Setup auto unsuspend time if required
    if (uiAutoUnsuspendDelay && bSuspend)
        m_llASyncLoadingAutoUnsuspendTime = CTickCount::Now() + CTickCount((long long)uiAutoUnsuspendDelay);
    else
        m_llASyncLoadingAutoUnsuspendTime = CTickCount();
}

bool CGameSA::IsASyncLoadingEnabled(bool bIgnoreSuspend)
{
    // Process auto unsuspend time if set
    if (m_llASyncLoadingAutoUnsuspendTime.ToLongLong() != 0)
    {
        if (CTickCount::Now() > m_llASyncLoadingAutoUnsuspendTime)
        {
            m_llASyncLoadingAutoUnsuspendTime = CTickCount();
            m_bASyncLoadingSuspended = false;
        }
    }

    if (m_bASyncLoadingSuspended && !bIgnoreSuspend)
        return false;

    if (m_bAsyncScriptForced)
        return m_bAsyncScriptEnabled;
    return true;
}

void CGameSA::SetupSpecialCharacters()
{
    ModelInfo[1].MakePedModel("TRUTH");
    ModelInfo[2].MakePedModel("MACCER");

    ModelInfo[3].MakePedModel("CDEPUT");
    ModelInfo[4].MakePedModel("SFPDM1");
    ModelInfo[5].MakePedModel("BB");
    ModelInfo[6].MakePedModel("WFYCRP");
    ModelInfo[8].MakePedModel("WMYCD2");
    ModelInfo[42].MakePedModel("SUZIE");
    ModelInfo[65].MakePedModel("VWMYAP");
    ModelInfo[86].MakePedModel("VHFYST");
    ModelInfo[119].MakePedModel("LVPDM1");

    ModelInfo[265].MakePedModel("TENPEN");
    ModelInfo[266].MakePedModel("PULASKI");
    ModelInfo[267].MakePedModel("HERN");
    ModelInfo[268].MakePedModel("DWAYNE");
    ModelInfo[269].MakePedModel("SMOKE");
    ModelInfo[270].MakePedModel("SWEET");
    ModelInfo[271].MakePedModel("RYDER");
    ModelInfo[272].MakePedModel("FORELLI");
    ModelInfo[273].MakePedModel("MEDIATR");
    ModelInfo[289].MakePedModel("SOMYAP");
    ModelInfo[290].MakePedModel("ROSE");
    ModelInfo[291].MakePedModel("PAUL");
    ModelInfo[292].MakePedModel("CESAR");
    ModelInfo[293].MakePedModel("OGLOC");
    ModelInfo[294].MakePedModel("WUZIMU");
    ModelInfo[295].MakePedModel("TORINO");
    ModelInfo[296].MakePedModel("JIZZY");
    ModelInfo[297].MakePedModel("MADDOGG");
    ModelInfo[298].MakePedModel("CAT");
    ModelInfo[299].MakePedModel("CLAUDE");
    ModelInfo[300].MakePedModel("RYDER2");
    ModelInfo[301].MakePedModel("RYDER3");
    ModelInfo[302].MakePedModel("EMMET");
    ModelInfo[303].MakePedModel("ANDRE");
    ModelInfo[304].MakePedModel("KENDL");
    ModelInfo[305].MakePedModel("JETHRO");
    ModelInfo[306].MakePedModel("ZERO");
    ModelInfo[307].MakePedModel("TBONE");
    ModelInfo[308].MakePedModel("SINDACO");
    ModelInfo[309].MakePedModel("JANITOR");
    ModelInfo[310].MakePedModel("BBTHIN");
    ModelInfo[311].MakePedModel("SMOKEV");
    ModelInfo[312].MakePedModel("PSYCHO");

    // ModelInfo[190].MakePedModel ( "BARBARA" );
    // ModelInfo[191].MakePedModel ( "HELENA" );
    // ModelInfo[192].MakePedModel ( "MICHELLE" );
    // ModelInfo[193].MakePedModel ( "KATIE" );
    // ModelInfo[194].MakePedModel ( "MILLIE" );
    // ModelInfo[195].MakePedModel ( "DENISE" );
    /* Hot-coffee only models
    ModelInfo[313].MakePedModel ( "GANGRL2" );
    ModelInfo[314].MakePedModel ( "MECGRL2" );
    ModelInfo[315].MakePedModel ( "GUNGRL2" );
    ModelInfo[316].MakePedModel ( "COPGRL2" );
    ModelInfo[317].MakePedModel ( "NURGRL2" );
    */
}

void CGameSA::FixModelCol(uint iFixModel, uint iFromModel)
{
    CBaseModelInfoSAInterface* pFixModelInterface = ModelInfo[iFixModel].GetInterface();
    if (!pFixModelInterface || pFixModelInterface->pColModel)
        return;

    CBaseModelInfoSAInterface* pAviableModelInterface = ModelInfo[iFromModel].GetInterface();

    if (!pAviableModelInterface)
        return;

    pFixModelInterface->pColModel = pAviableModelInterface->pColModel;
}

void CGameSA::SetupBrokenModels()
{
    FixModelCol(3118, 3059);
    FixModelCol(3553, 3554);
}

// Well, has it?
bool CGameSA::HasCreditScreenFadedOut()
{
    BYTE ucAlpha = *(BYTE*)0xBAB320;            // CLoadingScreen::m_FadeAlpha
    bool bCreditScreenFadedOut = (GetSystemState() >= 7) && (ucAlpha < 6);
    return bCreditScreenFadedOut;
}

// Ensure replaced/restored textures for models in the GTA map are correct
void CGameSA::FlushPendingRestreamIPL()
{
    CModelInfoSA::StaticFlushPendingRestreamIPL();
    m_pRenderWare->ResetStats();
}

void CGameSA::GetShaderReplacementStats(SShaderReplacementStats& outStats)
{
    m_pRenderWare->GetShaderReplacementStats(outStats);
}

void CGameSA::RemoveGameWorld()
{
    m_pIplStore->SetDynamicIplStreamingEnabled(false);

    m_pCoverManager->RemoveAllCovers();
    m_pPlantManager->RemoveAllPlants();

    // Remove all shadows in CStencilShadowObjects::dtorAll
    ((void* (*)())0x711390)();

    m_Pools->GetDummyPool().RemoveAllWithBackup();
    m_Pools->GetBuildingsPool().RemoveAllWithBackup();

    static_cast<CBuildingRemovalSA*>(m_pBuildingRemoval)->DropCaches();

    m_isGameWorldRemoved = true;
}

void CGameSA::RestoreGameWorld()
{
    m_Pools->GetBuildingsPool().RestoreBackup();
    m_Pools->GetDummyPool().RestoreBackup();

    m_pIplStore->SetDynamicIplStreamingEnabled(true, [](CIplSAInterface* ipl) { return memcmp("barriers", ipl->name, 8) != 0; });
    m_isGameWorldRemoved = false;
}

bool CGameSA::SetBuildingPoolSize(size_t size)
{
    const bool shouldRemoveWorld = !m_isGameWorldRemoved;
    if (shouldRemoveWorld)
        RemoveGameWorld();
    else
        static_cast<CBuildingRemovalSA*>(m_pBuildingRemoval)->DropCaches();

    bool status = m_Pools->GetBuildingsPool().Resize(size);

    if (shouldRemoveWorld)
        RestoreGameWorld();

    return status;
}

// Ensure models have the default lod distances
void CGameSA::ResetModelLodDistances()
{
    CModelInfoSA::StaticResetLodDistances();
}

void CGameSA::ResetModelFlags()
{
    CModelInfoSA::StaticResetFlags();
}

void CGameSA::ResetModelTimes()
{
    CModelInfoSA::StaticResetModelTimes();
}

void CGameSA::ResetAlphaTransparencies()
{
    CModelInfoSA::StaticResetAlphaTransparencies();
}

// Disable VSync by forcing what normally happends at the end of the loading screens
// Note #1: This causes the D3D device to be reset after the next frame
// Note #2: Some players do not need this to disable VSync. (Possibly because their video card driver settings override it somewhere)
void CGameSA::DisableVSync()
{
    MemPutFast<BYTE>(0xBAB318, 0);            // CLoadingScreen::m_bActive
}
CWeapon* CGameSA::CreateWeapon()
{
    return new CWeaponSA(new CWeaponSAInterface, NULL, WEAPONSLOT_MAX);
}

CWeaponStat* CGameSA::CreateWeaponStat(eWeaponType weaponType, eWeaponSkill weaponSkill)
{
    return m_pWeaponStatsManager->CreateWeaponStatUnlisted(weaponType, weaponSkill);
}

void CGameSA::SetWeaponRenderEnabled(bool enabled)
{
    if (IsWeaponRenderEnabled() == enabled)
        return;

    if (!enabled)
    {
        // Disable calls to CVisibilityPlugins::RenderWeaponPedsForPC
        MemSet((void*)0x53EAC4, 0x90, 5); // Idle
        MemSet((void*)0x705322, 0x90, 5); // CPostEffects::Render
        MemSet((void*)0x7271E3, 0x90, 5); // CMirrors::BeforeMainRender
    }
    else
    {
        // Restore original bytes
        MemCpy((void*)0x53EAC4, "\xE8\x67\x44\x1F\x00", 5);
        MemCpy((void*)0x705322, "\xE8\x09\xDC\x02\x00", 5);
        MemCpy((void*)0x7271E3, "\xE8\x48\xBD\x00\x00", 5);
    }
}

bool CGameSA::IsWeaponRenderEnabled() const
{
    return *(unsigned char*)0x53EAC4 == 0xE8;
}

void CGameSA::OnPedContextChange(CPed* pPedContext)
{
    m_pPedContext = pPedContext;
}

CPed* CGameSA::GetPedContext()
{
    if (!m_pPedContext)
        m_pPedContext = pGame->GetPools()->GetPedFromRef((DWORD)1);
    return m_pPedContext;
}

CObjectGroupPhysicalProperties* CGameSA::GetObjectGroupPhysicalProperties(unsigned char ucObjectGroup)
{
    if (ucObjectGroup < OBJECTDYNAMICINFO_MAX && ObjectGroupsInfo[ucObjectGroup].IsValid())
        return &ObjectGroupsInfo[ucObjectGroup];

    return nullptr;
}
