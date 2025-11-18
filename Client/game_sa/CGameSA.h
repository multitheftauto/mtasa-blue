/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CGameSA.h
 *  PURPOSE:     Header file for base game logic handling class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include <game/CGame.h>
#include <game/RenderWare.h>
#include "CFxManagerSA.h"
#include "CModelInfoSA.h"
#include "CStreamingSA.h"
#include "CCoverManagerSA.h"
#include "CPlantManagerSA.h"
#include "CRendererSA.h"
#include "CVehicleAudioSettingsManagerSA.h"

class CAnimBlendClumpDataSAInterface;
class CObjectGroupPhysicalPropertiesSA;
class CTaskManagementSystemSA;

extern unsigned int OBJECTDYNAMICINFO_MAX;            // default: 160

#define CLASS_CPlayerInfo               0xB7CD98
#define CLASS_CCamera                   0xB6F028
#define CLASS_CPad                      0xB73458
#define CLASS_CFx                       0xa9ae00
#define CLASS_CFxManager                0xA9AE80
#define CLASS_CMenuManager              0xBA6748
#define CLASS_RwCamera                  0xB6F97C

#define ARRAY_WeaponInfo                0xC8AAB8
#define CLASSSIZE_WeaponInfo            112
#define NUM_WeaponInfosStdSkill         WEAPONTYPE_LAST_WEAPONTYPE
#define NUM_WeaponInfosOtherSkill       11
#define NUM_WeaponInfosTotal            (NUM_WeaponInfosStdSkill + (3*NUM_WeaponInfosOtherSkill)) // std, (poor, pro, special)

#define MODELINFO_DFF_MAX               20000
#define MODELINFO_TXD_MAX               25000
#define MODELINFO_MAX                   26000       // Actual max is 25755

#define VAR_FlyingCarsEnabled           0x969160
#define VAR_ExtraBunnyhopEnabled        0x969161
#define VAR_HoveringCarsEnabled         0x969152
#define VAR_ExtraJumpEnabled            0x96916C
#define VAR_TankModeEnabled             0x969164
#define VAR_NoReloadEnabled             0x969178
#define VAR_PerfectHandling             0x96914C
#define VAR_AllCarsHaveNitro            0x969165
#define VAR_BoatsCanFly                 0x969153
#define VAR_InfiniteOxygen              0x96916E
#define VAR_FasterClock                 0x96913B
#define VAR_FasterGameplay              0x96913C
#define VAR_SlowerGameplay              0x96913D
#define VAR_AlwaysMidnight              0x969167
#define VAR_FullWeaponAiming            0x969179
#define VAR_InfiniteHealth              0x96916D
#define VAR_NeverWanted                 0x969171
#define VAR_HealthArmorMoney            0x969133
#define VAR_WalkUnderwater              0x6C2759

#define CHEAT_HOVERINGCARS          "hovercars"
#define CHEAT_FLYINGCARS            "aircars"
#define CHEAT_EXTRABUNNYHOP         "extrabunny"
#define CHEAT_EXTRAJUMP             "extrajump"
#define CHEAT_TANKMODE              "tankmode"
#define CHEAT_NORELOAD              "noreload"
#define CHEAT_PERFECTHANDLING       "perfecthandling"
#define CHEAT_ALLCARSHAVENITRO      "allcarshavenitro"
#define CHEAT_BOATSCANFLY           "airboats"
#define CHEAT_INFINITEOXYGEN        "infiniteoxygen"
#define CHEAT_WALKUNDERWATER        "walkunderwater"
#define CHEAT_FASTERCLOCK           "fasterclock"
#define CHEAT_FASTERGAMEPLAY        "fastergameplay"
#define CHEAT_SLOWERGAMEPLAY        "slowergameplay"
#define CHEAT_ALWAYSMIDNIGHT        "alwaysmidnight"
#define CHEAT_FULLWEAPONAIMING      "fullweaponaiming"
#define CHEAT_INFINITEHEALTH        "infinitehealth"
#define CHEAT_NEVERWANTED           "neverwanted"
#define CHEAT_HEALTARMORMONEY       "healtharmormoney"

#define FUNC_CAESoundManager_CancelSoundsOwnedByAudioEntity 0x4EFCD0
#define STRUCT_CAESoundManager                              0xB62CB0
#define FUNC_CWaterCannon_Constructor                       0x728B10
#define FUNC_CWaterCannon_Destructor                        0x728B30
#define FUNC_CWaterCannon_Init                              0x728B40
#define ARRAY_aCannons                                      0xC80740
#define ARRAY_aCannons_CurrentPtr                           0x728C83
#define NUM_WaterCannon_Limit                               0x728C88
#define SIZE_CWaterCannon                                   0x3CC
#define NUM_CWaterCannon_Audio_Offset                       0x32C
#define NUM_CWaterCannon_DefaultLimit                       3

constexpr int MAX_WATER_CANNONS = 30; // extended CWaterCannon limit, it can be increased

struct SCheatSA
{
    BYTE* m_byAddress;            // Cheat Address
    bool  m_bEnabled;             // Cheat State
    bool  m_bCanBeSet;            // Cheat can be set with setWorldSpecialPropertyEnabled
    SCheatSA(BYTE* Address, bool bCanBeSet = true)
    {
        m_byAddress = Address;
        m_bCanBeSet = bCanBeSet;
    }
};

class CGameSA : public CGame
{
    friend class COffsets;
    typedef std::unique_ptr<CAnimBlendAssocGroup> AssocGroup_type;

private:
    CWeaponInfo*                      WeaponInfos[NUM_WeaponInfosTotal];
    CModelInfoSA*                     ModelInfo;
    CObjectGroupPhysicalPropertiesSA* ObjectGroupsInfo;

public:
    ZERO_ON_NEW

    CGameSA();
    ~CGameSA();

    CPools*                   GetPools() const noexcept { return m_Pools.get(); }
    CPlayerInfo*              GetPlayerInfo() { return m_pPlayerInfo; }
    CProjectileInfo*          GetProjectileInfo() { return m_pProjectileInfo; }
    CRadar*                   GetRadar() { return m_pRadar; }
    CClock*                   GetClock() { return m_pClock; }
    CCoronas*                 GetCoronas() { return m_pCoronas; }
    CCheckpoints*             GetCheckpoints() { return m_pCheckpoints; }
    CEventList*               GetEventList() { return m_pEventList; }
    CFireManager*             GetFireManager() { return m_pFireManager; }
    CExplosionManager*        GetExplosionManager() { return m_pExplosionManager; }
    CGarages*                 GetGarages() { return m_pGarages; }
    CHud*                     GetHud() { return m_pHud; }
    CWeather*                 GetWeather() { return m_pWeather; }
    CWorld*                   GetWorld() { return m_pWorld; }
    CCamera*                  GetCamera() { return m_pCamera; }
    CPickups*                 GetPickups() { return m_pPickups; }
    C3DMarkers*               Get3DMarkers() { return m_p3DMarkers; }
    CPad*                     GetPad() { return m_pPad; }
    CAERadioTrackManager*     GetAERadioTrackManager() { return m_pCAERadioTrackManager; }
    CAudioEngine*             GetAudioEngine() { return m_pAudioEngine; }
    CAEAudioHardware*         GetAEAudioHardware() { return m_pAEAudioHardware; }
    CAESoundManager*          GetAESoundManager() override { return m_pAESoundManager; }
    CAudioContainer*          GetAudioContainer() { return m_pAudioContainer; }
    CStats*                   GetStats() { return m_pStats; }
    CTaskManagementSystemSA*  GetTaskManagementSystem() { return m_pTaskManagementSystem; }
    CTasks*                   GetTasks() { return m_pTasks; }
    CGameSettings*            GetSettings() { return m_pSettings; }
    CCarEnterExit*            GetCarEnterExit() { return m_pCarEnterExit; }
    CControllerConfigManager* GetControllerConfigManager() { return m_pControllerConfigManager; }
    CRenderWare*              GetRenderWare() { return m_pRenderWare; }
    CHandlingManager*         GetHandlingManager() const noexcept { return m_HandlingManager.get(); }
    CAnimManager*             GetAnimManager() { return m_pAnimManager; }
    CStreaming*               GetStreaming() { return m_pStreaming; }
    CVisibilityPlugins*       GetVisibilityPlugins() { return m_pVisibilityPlugins; }
    CKeyGen*                  GetKeyGen() { return m_pKeyGen; }
    CRopes*                   GetRopes() { return m_pRopes; }
    CFx*                      GetFx() { return m_pFx; }
    CFxManager*               GetFxManager() { return m_pFxManager; }
    CWaterManager*            GetWaterManager() { return m_pWaterManager; }
    CWeaponStatManager*       GetWeaponStatManager() { return m_pWeaponStatsManager; }
    CPointLights*             GetPointLights() { return m_pPointLights; }
    CColStore*                GetCollisionStore() override { return m_collisionStore; }
    CRenderWareSA*            GetRenderWareSA() { return m_pRenderWare; }
    CFxManagerSA*             GetFxManagerSA() { return m_pFxManager; }
    CIplStore*                GetIplStore() { return m_pIplStore; };
    CCoverManagerSA*          GetCoverManager() const noexcept { return m_pCoverManager; };
    CPlantManagerSA*          GetPlantManager() const noexcept { return m_pPlantManager; };
    CBuildingRemoval*         GetBuildingRemoval() { return m_pBuildingRemoval; }
    CRenderer*                GetRenderer() const noexcept override { return m_pRenderer.get(); }

    CVehicleAudioSettingsManager* GetVehicleAudioSettingsManager() const noexcept override
    {
        return m_pVehicleAudioSettingsManager.get();
    }

    CWeaponInfo*                    GetWeaponInfo(eWeaponType weapon, eWeaponSkill skill = WEAPONSKILL_STD);
    CModelInfo*                     GetModelInfo(DWORD dwModelID, bool bCanBeInvalid = false);
    CModelInfo*                     GetModelInfo(CBaseModelInfoSAInterface* baseModelInfo);
    CObjectGroupPhysicalProperties* GetObjectGroupPhysicalProperties(unsigned char ucObjectGroup);

    uint32_t GetBaseIDforDFF() override { return 0; }
    uint32_t GetBaseIDforTXD() override { return *(uint32_t*)(0x407104 + 2); }
    uint32_t GetBaseIDforCOL() override { return *(uint32_t*)(0x410344 + 2); }
    uint32_t GetBaseIDforIPL() override { return *(uint32_t*)(0x4044F4 + 2); }
    uint32_t GetBaseIDforDAT() override { return *(uint32_t*)(0x44D064 + 2); }
    uint32_t GetBaseIDforIFP() override { return *(uint32_t*)(0x407124 + 2); }
    uint32_t GetBaseIDforRRR() override { return *(uint32_t*)(0x4594A1 + 2); }
    uint32_t GetBaseIDforSCM() override { return *(uint32_t*)(0x46A574 + 2); }
    uint32_t GetCountOfAllFileIDs() override { return (*(char**)(0x5B8AFA + 2) - *(char**)(0x5B8B08 + 6)) / sizeof(CStreamingInfo); }

    DWORD GetSystemTime() { return *(DWORD*)0xB7CB84; } // CTimer::m_snTimeInMilliseconds
    int   GetSystemFrameCounter() const { return *(int*)0xB7CB4C; } // CTimer::m_FrameCounter

    bool IsAtMenu() { return *(unsigned long*)0xBA677B != 0; } // FrontEndMenuManager + 0x33

    void         StartGame();
    void         SetSystemState(SystemState State);
    SystemState  GetSystemState();
    void         Pause(bool bPaused);

    void Initialize();
    void Reset();
    void Terminate();

    eGameVersion GetGameVersion();
    eGameVersion FindGameVersion();

    float GetFPS();
    float GetTimeStep();
    float GetOldTimeStep();
    float GetTimeScale();
    void  SetTimeScale(float fTimeScale);

    bool InitLocalPlayer(class CClientPed* pClientPed);

    float GetGravity();
    void  SetGravity(float fGravity);

    float GetGameSpeed();
    void  SetGameSpeed(float fSpeed);

    unsigned char GetBlurLevel();
    void          SetBlurLevel(unsigned char ucLevel);

    void SetJetpackWeaponEnabled(eWeaponType weaponType, bool bEnabled);
    bool GetJetpackWeaponEnabled(eWeaponType weaponType);

    void SetVehicleSunGlareEnabled(bool bEnabled);
    bool IsVehicleSunGlareEnabled();

    void SetCoronaZTestEnabled(bool isEnabled) override;
    bool IsCoronaZTestEnabled() const noexcept override { return m_isCoronaZTestEnabled; }

    bool IsWaterCreaturesEnabled() const noexcept override { return m_areWaterCreaturesEnabled; }
    void SetWaterCreaturesEnabled(bool isEnabled) override;

    bool IsBurnFlippedCarsEnabled() const noexcept override { return m_isBurnFlippedCarsEnabled; }
    void SetBurnFlippedCarsEnabled(bool isEnabled) override;

    bool IsFireballDestructEnabled() const noexcept override { return m_isFireballDestructEnabled; }
    void SetFireballDestructEnabled(bool isEnabled) override;

    bool IsExtendedWaterCannonsEnabled() const noexcept override { return m_isExtendedWaterCannonsEnabled; }
    void SetExtendedWaterCannonsEnabled(bool isEnabled) override;

    bool IsRoadSignsTextEnabled() const noexcept override { return m_isRoadSignsTextEnabled; }
    void SetRoadSignsTextEnabled(bool isEnabled) override;

    bool IsTunnelWeatherBlendEnabled() const noexcept override { return m_isTunnelWeatherBlendEnabled; }
    void SetTunnelWeatherBlendEnabled(bool isEnabled) override;

    bool IsIgnoreFireStateEnabled() const noexcept override { return m_isIgnoreFireStateEnabled; }
    void SetIgnoreFireStateEnabled(bool isEnabled) override;

    bool IsVehicleBurnExplosionsEnabled() const noexcept override { return m_isVehicleBurnExplosionsEnabled; }
    void SetVehicleBurnExplosionsEnabled(bool isEnabled) override;

    unsigned long GetMinuteDuration();
    void          SetMinuteDuration(unsigned long ulTime);

    bool IsCheatEnabled(const char* szCheatName);
    bool SetCheatEnabled(const char* szCheatName, bool bEnable);
    void ResetCheats();

    bool IsRandomFoliageEnabled();
    void SetRandomFoliageEnabled(bool bEnable);

    bool IsMoonEasterEggEnabled();
    void SetMoonEasterEggEnabled(bool bEnabled);

    bool IsExtraAirResistanceEnabled();
    void SetExtraAirResistanceEnabled(bool bEnable);

    bool IsUnderWorldWarpEnabled();
    void SetUnderWorldWarpEnabled(bool bEnable);

    bool VerifySADataFileNames();
    bool PerformChecks();
    int& GetCheckStatus() { return m_iCheckStatus; }

    void SetAsyncLoadingFromScript(bool bScriptEnabled, bool bScriptForced);
    void SuspendASyncLoading(bool bSuspend, uint uiAutoUnsuspendDelay = 0);
    bool IsASyncLoadingEnabled(bool bIgnoreSuspend = false);

    void         SetupSpecialCharacters();
    void         FixModelCol(uint iFixModel, uint iFromModel);
    void         SetupBrokenModels();
    CWeapon*     CreateWeapon();
    CWeaponStat* CreateWeaponStat(eWeaponType weaponType, eWeaponSkill weaponSkill);
    void         SetWeaponRenderEnabled(bool enabled) override;
    bool         IsWeaponRenderEnabled() const override;
    void         FlushPendingRestreamIPL();
    void         ResetModelLodDistances();
    void         ResetModelFlags();
    void         ResetAlphaTransparencies();
    void         DisableVSync();
    void         ResetModelTimes();
    void         ResetModelAnimations() override;

    void  OnPedContextChange(CPed* pPedContext);
    CPed* GetPedContext();

    void GetShaderReplacementStats(SShaderReplacementStats& outStats);

    void SetPreWeaponFireHandler(PreWeaponFireHandler* pPreWeaponFireHandler) { m_pPreWeaponFireHandler = pPreWeaponFireHandler; }
    void SetPostWeaponFireHandler(PostWeaponFireHandler* pPostWeaponFireHandler) { m_pPostWeaponFireHandler = pPostWeaponFireHandler; }
    void SetTaskSimpleBeHitHandler(TaskSimpleBeHitHandler* pTaskSimpleBeHitHandler) { m_pTaskSimpleBeHitHandler = pTaskSimpleBeHitHandler; }
    CAnimBlendClumpDataSAInterface** GetClumpData(RpClump* clump) { return RWPLUGINOFFSET(CAnimBlendClumpDataSAInterface*, clump, ClumpOffset); }

    StreamingRemoveModelCallback GetStreamingRemoveModelCallback() const noexcept override;

    PreWeaponFireHandler*   m_pPreWeaponFireHandler;
    PostWeaponFireHandler*  m_pPostWeaponFireHandler;
    TaskSimpleBeHitHandler* m_pTaskSimpleBeHitHandler;

    void RemoveGameWorld();
    void RestoreGameWorld();

    bool SetBuildingPoolSize(size_t size);

private:
    std::unique_ptr<CPools>           m_Pools;
    CPlayerInfo*                      m_pPlayerInfo;
    CProjectileInfo*                  m_pProjectileInfo;
    CRadar*                           m_pRadar;
    CClock*                           m_pClock;
    CCoronas*                         m_pCoronas;
    CCheckpoints*                     m_pCheckpoints;
    CEventList*                       m_pEventList;
    CFireManager*                     m_pFireManager;
    CGarages*                         m_pGarages;
    CHud*                             m_pHud;
    CWeather*                         m_pWeather;
    CWorld*                           m_pWorld;
    CCamera*                          m_pCamera;
    CModelInfo*                       m_pModelInfo;
    CPickups*                         m_pPickups;
    CWeaponInfo*                      m_pWeaponInfo;
    CExplosionManager*                m_pExplosionManager;
    C3DMarkers*                       m_p3DMarkers;
    CRenderWareSA*                    m_pRenderWare;
    std::unique_ptr<CHandlingManager> m_HandlingManager;
    CAnimManager*                     m_pAnimManager;
    CStreaming*                       m_pStreaming;
    CVisibilityPlugins*               m_pVisibilityPlugins;
    CKeyGen*                          m_pKeyGen;
    CRopes*                           m_pRopes;
    CFx*                              m_pFx;
    CFxManagerSA*                     m_pFxManager;
    CWaterManager*                    m_pWaterManager;
    CWeaponStatManager*               m_pWeaponStatsManager;
    CPointLights*                     m_pPointLights;
    CColStore*                        m_collisionStore;
    CObjectGroupPhysicalProperties*   m_pObjectGroupPhysicalProperties;
    CCoverManagerSA*                  m_pCoverManager;
    CPlantManagerSA*                  m_pPlantManager;
    CBuildingRemoval*                 m_pBuildingRemoval;

    std::unique_ptr<CVehicleAudioSettingsManagerSA> m_pVehicleAudioSettingsManager;

    std::unique_ptr<CRendererSA>    m_pRenderer;

    CPad*                     m_pPad;
    CAERadioTrackManager*     m_pCAERadioTrackManager;
    CAudioEngine*             m_pAudioEngine;
    CAEAudioHardware*         m_pAEAudioHardware;
    CAESoundManager*          m_pAESoundManager;
    CAudioContainer*          m_pAudioContainer;
    CStats*                   m_pStats;
    CTaskManagementSystemSA*  m_pTaskManagementSystem;            // not used outside the game_sa
    CTasks*                   m_pTasks;
    CGameSettings*            m_pSettings;
    CCarEnterExit*            m_pCarEnterExit;
    CControllerConfigManager* m_pControllerConfigManager;
    CIplStore*                m_pIplStore;

    eGameVersion m_eGameVersion;
    bool         m_bAsyncScriptEnabled;
    bool         m_bAsyncScriptForced;
    bool         m_bASyncLoadingSuspended;
    int          m_iCheckStatus;
    bool         m_bUnderworldWarp;
    bool         m_isCoronaZTestEnabled{true};
    bool         m_isTunnelWeatherBlendEnabled{true}; 
    bool         m_areWaterCreaturesEnabled{true};
    bool         m_isBurnFlippedCarsEnabled{true};
    bool         m_isFireballDestructEnabled{true};
    bool         m_isRoadSignsTextEnabled{true};
    bool         m_isGameWorldRemoved{false};
    bool         m_isExtendedWaterCannonsEnabled{false};
    bool         m_isIgnoreFireStateEnabled{false};
    bool         m_isVehicleBurnExplosionsEnabled{true};

    static unsigned int&  ClumpOffset;

    std::map<std::string, SCheatSA*> m_Cheats;

    SFixedArray<bool, WEAPONTYPE_LAST_WEAPONTYPE> m_JetpackWeapons;

    CPed*      m_pPedContext;
    CTickCount m_llASyncLoadingAutoUnsuspendTime;
};
