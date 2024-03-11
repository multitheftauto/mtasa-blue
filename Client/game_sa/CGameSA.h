/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CGameSA.h
 *  PURPOSE:     Header file for base game logic handling class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include <game/CGame.h>
#include <game/RenderWare.h>
#include "CFxManagerSA.h"
#include "CModelInfoSA.h"
#include "CStreamingSA.h"

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

    CPools*                   GetPools() { return m_pPools; }
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
    CHandlingManager*         GetHandlingManager() { return m_pHandlingManager; }
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
    CVehicleAudioSettingsManager* GetVehicleAudioSettingsManager() { return m_pVehicleAudioSettingsManager; }

    CWeaponInfo*                    GetWeaponInfo(eWeaponType weapon, eWeaponSkill skill = WEAPONSKILL_STD);
    CModelInfo*                     GetModelInfo(DWORD dwModelID, bool bCanBeInvalid = false);
    CObjectGroupPhysicalProperties* GetObjectGroupPhysicalProperties(unsigned char ucObjectGroup);

    int32_t GetBaseIDforDFF() { return 0; }
    int32_t GetBaseIDforTXD() { return *(int32_t*)(0x407104 + 2); }
    int32_t GetBaseIDforCOL() { return *(int32_t*)(0x410344 + 2); }
    int32_t GetBaseIDforIPL() { return *(int32_t*)(0x4044F4 + 2); }
    int32_t GetBaseIDforDAT() { return *(int32_t*)(0x44D064 + 2); }
    int32_t GetBaseIDforIFP() { return *(int32_t*)(0x407124 + 2); }
    int32_t GetBaseIDforRRR() { return *(int32_t*)(0x4594A1 + 2); }
    int32_t GetBaseIDforSCM() { return *(int32_t*)(0x46A574 + 2); }
    int32_t GetCountOfAllFileIDs() { return (*(char**)(0x5B8AFA + 2) - *(char**)(0x5B8B08 + 6)) / sizeof(CStreamingInfo); }

    DWORD GetSystemTime() { return *(DWORD*)0xB7CB84; } // CTimer::m_snTimeInMilliseconds

    bool IsAtMenu() { return *(unsigned long*)0xBA677B != 0; } // FrontEndMenuManager + 0x33

    void         StartGame();
    void         SetSystemState(eSystemState State);
    eSystemState GetSystemState();
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

    bool HasCreditScreenFadedOut();

    void         SetupSpecialCharacters();
    void         FixModelCol(uint iFixModel, uint iFromModel);
    void         SetupBrokenModels();
    CWeapon*     CreateWeapon();
    CWeaponStat* CreateWeaponStat(eWeaponType weaponType, eWeaponSkill weaponSkill);
    void         FlushPendingRestreamIPL();
    void         ResetModelLodDistances();
    void         ResetModelFlags();
    void         ResetAlphaTransparencies();
    void         DisableVSync();
    void         ResetModelTimes();

    void  OnPedContextChange(CPed* pPedContext);
    CPed* GetPedContext();

    void GetShaderReplacementStats(SShaderReplacementStats& outStats);

    void SetPreWeaponFireHandler(PreWeaponFireHandler* pPreWeaponFireHandler) { m_pPreWeaponFireHandler = pPreWeaponFireHandler; }
    void SetPostWeaponFireHandler(PostWeaponFireHandler* pPostWeaponFireHandler) { m_pPostWeaponFireHandler = pPostWeaponFireHandler; }
    void SetTaskSimpleBeHitHandler(TaskSimpleBeHitHandler* pTaskSimpleBeHitHandler) { m_pTaskSimpleBeHitHandler = pTaskSimpleBeHitHandler; }
    CAnimBlendClumpDataSAInterface** GetClumpData(RpClump* clump) { return RWPLUGINOFFSET(CAnimBlendClumpDataSAInterface*, clump, ClumpOffset); }

    PreWeaponFireHandler*   m_pPreWeaponFireHandler;
    PostWeaponFireHandler*  m_pPostWeaponFireHandler;
    TaskSimpleBeHitHandler* m_pTaskSimpleBeHitHandler;

private:
    CPools*                         m_pPools;
    CPlayerInfo*                    m_pPlayerInfo;
    CProjectileInfo*                m_pProjectileInfo;
    CRadar*                         m_pRadar;
    CClock*                         m_pClock;
    CCoronas*                       m_pCoronas;
    CCheckpoints*                   m_pCheckpoints;
    CEventList*                     m_pEventList;
    CFireManager*                   m_pFireManager;
    CGarages*                       m_pGarages;
    CHud*                           m_pHud;
    CWeather*                       m_pWeather;
    CWorld*                         m_pWorld;
    CCamera*                        m_pCamera;
    CModelInfo*                     m_pModelInfo;
    CPickups*                       m_pPickups;
    CWeaponInfo*                    m_pWeaponInfo;
    CExplosionManager*              m_pExplosionManager;
    C3DMarkers*                     m_p3DMarkers;
    CRenderWareSA*                  m_pRenderWare;
    CHandlingManager*               m_pHandlingManager;
    CAnimManager*                   m_pAnimManager;
    CStreaming*                     m_pStreaming;
    CVisibilityPlugins*             m_pVisibilityPlugins;
    CKeyGen*                        m_pKeyGen;
    CRopes*                         m_pRopes;
    CFx*                            m_pFx;
    CFxManagerSA*                   m_pFxManager;
    CWaterManager*                  m_pWaterManager;
    CWeaponStatManager*             m_pWeaponStatsManager;
    CPointLights*                   m_pPointLights;
    CColStore*                      m_collisionStore;
    CObjectGroupPhysicalProperties* m_pObjectGroupPhysicalProperties;

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
    CControllerConfigManager*     m_pControllerConfigManager;
    CVehicleAudioSettingsManager* m_pVehicleAudioSettingsManager;

    eGameVersion m_eGameVersion;
    bool         m_bAsyncScriptEnabled;
    bool         m_bAsyncScriptForced;
    bool         m_bASyncLoadingSuspended;
    int          m_iCheckStatus;
    bool         m_bUnderworldWarp;
    bool         m_isCoronaZTestEnabled{true};
    bool         m_areWaterCreaturesEnabled{true};
    bool         m_isBurnFlippedCarsEnabled{true};
    bool         m_isFireballDestructEnabled{true};

    static unsigned int&  ClumpOffset;

    std::map<std::string, SCheatSA*> m_Cheats;

    SFixedArray<bool, WEAPONTYPE_LAST_WEAPONTYPE> m_JetpackWeapons;

    CPed*      m_pPedContext;
    CTickCount m_llASyncLoadingAutoUnsuspendTime;
};
