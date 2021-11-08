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

#include "CModelInfoSA.h"
#include "CObjectGroupPhysicalPropertiesSA.h"
#include "CFxManagerSA.h"
#include <game/CStreaming.h>

#define     MAX_MEMORY_OFFSET_1_0           0xCAF008

#define     CLASS_CPlayerInfo               0xB7CD98    // ##SA##
#define     CLASS_CCamera                   0xB6F028    // ##SA##
#define     CLASS_CPad                      0xB73458    // ##SA##
#define     CLASS_CGarages                  0x96C048    // ##SA##
#define     CLASS_CFx                       0xa9ae00    // ##SA##
#define     CLASS_CFxManager                0xA9AE80    // ##SA##
#define     CLASS_CMenuManager              0xBA6748    // ##SA##

#define     CLASS_RwCamera                  0xB6F97C

#define     ARRAY_WeaponInfo                0xC8AAB8    // ##SA##
#define     CLASSSIZE_WeaponInfo            112         // ##SA##
#define     NUM_WeaponInfosStdSkill         WEAPONTYPE_LAST_WEAPONTYPE
#define     NUM_WeaponInfosOtherSkill       11
#define     NUM_WeaponInfosTotal            (NUM_WeaponInfosStdSkill + (3*NUM_WeaponInfosOtherSkill)) // std, (poor, pro, special)

extern unsigned int OBJECTDYNAMICINFO_MAX;            // default: 160

#define     FUNC_GetLevelFromPosition       0x4DD300

#define     FUNC_CDebug_DebugDisplayTextBuffer      0x532260
#define     FUNC_JPegCompressScreenToFile   0x5D0820

#define     VAR_FlyingCarsEnabled           0x969160 // ##SA##
#define     VAR_ExtraBunnyhopEnabled        0x969161 // ##SA##
#define     VAR_HoveringCarsEnabled         0x969152 // ##SA##
#define     VAR_ExtraJumpEnabled            0x96916C // ##SA##
#define     VAR_TankModeEnabled             0x969164 // ##SA##
#define     VAR_NoReloadEnabled             0x969178 // ##SA##
#define     VAR_PerfectHandling             0x96914C // ##SA##
#define     VAR_AllCarsHaveNitro            0x969165 // ##SA##
#define     VAR_BoatsCanFly                 0x969153 // ##SA##
#define     VAR_InfiniteOxygen              0x96916E // ##SA##
#define     VAR_FasterClock                 0x96913B // ##SA##
#define     VAR_FasterGameplay              0x96913C // ##SA##
#define     VAR_SlowerGameplay              0x96913D // ##SA##
#define     VAR_AlwaysMidnight              0x969167 // ##SA##
#define     VAR_FullWeaponAiming            0x969179 // ##SA##
#define     VAR_InfiniteHealth              0x96916D // ##SA##
#define     VAR_NeverWanted                 0x969171 // ##SA##
#define     VAR_HealthArmorMoney            0x969133 // ##SA##
#define     VAR_WalkUnderwater              0x6C2759

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

#define PROP_RANDOM_FOLIAGE         "randomfoliage"
#define PROP_SNIPER_MOON            "snipermoon"
#define PROP_EXTRA_AIR_RESISTANCE   "extraairresistance"
#define PROP_UNDERWORLD_WARP        "underworldwarp"

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

class CAnimBlendClumpDataSAInterface;

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

    CGameSA();            // constructor
    ~CGameSA();

    CPools* GetPools()
    {
        DEBUG_TRACE("CPools     * GetPools()");
        return m_pPools;
    };
    CPlayerInfo* GetPlayerInfo()
    {
        DEBUG_TRACE("CPlayerInfo    * GetPlayerInfo()");
        return m_pPlayerInfo;
    };
    CProjectileInfo* GetProjectileInfo()
    {
        DEBUG_TRACE("CProjectileInfo   * GetProjectileInfo()");
        return m_pProjectileInfo;
    };
    CRadar* GetRadar()
    {
        DEBUG_TRACE("CRadar     * GetRadar()");
        return m_pRadar;
    };
    CRestart* GetRestart()
    {
        DEBUG_TRACE("CRestart    * GetRestart()");
        return m_pRestart;
    };
    CClock* GetClock()
    {
        DEBUG_TRACE("CClock     * GetClock()");
        return m_pClock;
    };
    CCoronas* GetCoronas()
    {
        DEBUG_TRACE("CCoronas    * GetCoronas()");
        return m_pCoronas;
    };
    CCheckpoints* GetCheckpoints()
    {
        DEBUG_TRACE("CCheckpoints   * GetCheckpoints()");
        return m_pCheckpoints;
    };
    CEventList* GetEventList()
    {
        DEBUG_TRACE("CEventList    * GetEventList()");
        return m_pEventList;
    };
    CFireManager* GetFireManager()
    {
        DEBUG_TRACE("CFireManager   * GetFireManager()");
        return m_pFireManager;
    };
    CExplosionManager* GetExplosionManager()
    {
        DEBUG_TRACE("CExplosionManager  * GetExplosionManager()");
        return m_pExplosionManager;
    };
    CGarages* GetGarages()
    {
        DEBUG_TRACE("CGarages    * GetGarages()");
        return m_pGarages;
    };
    CHud* GetHud()
    {
        DEBUG_TRACE("CHud     * GetHud()");
        return m_pHud;
    };
    CWeather* GetWeather()
    {
        DEBUG_TRACE("CWeather    * GetWeather()");
        return m_pWeather;
    };
    CWorld* GetWorld()
    {
        DEBUG_TRACE("CWorld     * GetWorld()");
        return m_pWorld;
    };
    CCamera* GetCamera()
    {
        DEBUG_TRACE("CCamera     * GetCamera()");
        return m_pCamera;
    };
    CPickups* GetPickups()
    {
        DEBUG_TRACE("CPickups    * GetPickups()");
        return m_pPickups;
    };
    C3DMarkers* Get3DMarkers()
    {
        DEBUG_TRACE("C3DMarkers    * Get3DMarkers()");
        return m_p3DMarkers;
    };
    CPad* GetPad()
    {
        DEBUG_TRACE("CPad     * GetPad()");
        return m_pPad;
    };
    CTheCarGenerators* GetTheCarGenerators()
    {
        DEBUG_TRACE("CTheCarGenerators  * GetTheCarGenerators()");
        return m_pTheCarGenerators;
    };
    CAERadioTrackManager* GetAERadioTrackManager()
    {
        DEBUG_TRACE("CAERadioTrackManager * GetAERadioTrackManager()");
        return m_pCAERadioTrackManager;
    };
    CAudioEngine* GetAudioEngine()
    {
        DEBUG_TRACE("CAudio     * GetAudioEngine()");
        return m_pAudioEngine;
    };
    CAEAudioHardware* GetAEAudioHardware()
    {
        DEBUG_TRACE("CAEAudioHardware     * GetAEAudioHardware()");
        return m_pAEAudioHardware;
    };
    CAESoundManager* GetAESoundManager() override { return m_pAESoundManager; }
    CAudioContainer* GetAudioContainer()
    {
        DEBUG_TRACE("CAudio     * GetAudioContainer()");
        return m_pAudioContainer;
    };
    CMenuManager* GetMenuManager()
    {
        DEBUG_TRACE("CMenuManager         * GetMenuManager()");
        return m_pMenuManager;
    };
    CStats* GetStats()
    {
        DEBUG_TRACE("CStats                   * GetStats()");
        return m_pStats;
    };
    CFont* GetFont()
    {
        DEBUG_TRACE("CFont                    * GetFont()");
        return m_pFont;
    };
    CPathFind* GetPathFind()
    {
        DEBUG_TRACE("CPathFind                * GetPathFind()");
        return m_pPathFind;
    };
    CPopulation* GetPopulation()
    {
        DEBUG_TRACE("CPopulation              * GetPopulation()");
        return m_pPopulation;
    };
    CTaskManagementSystem* GetTaskManagementSystem()
    {
        DEBUG_TRACE("CTaskManagementSystemSA * GetTaskManagementSystem()");
        return m_pTaskManagementSystem;
    };
    CTasks* GetTasks()
    {
        DEBUG_TRACE("CTasks * GetTasks()");
        return m_pTasks;
    };
    CGameSettings* GetSettings()
    {
        DEBUG_TRACE("CGameSettings * GetSettings()");
        return m_pSettings;
    };
    CCarEnterExit* GetCarEnterExit()
    {
        DEBUG_TRACE("CCarEnterExit           * GetCarEnterExit()");
        return m_pCarEnterExit;
    };
    CControllerConfigManager* GetControllerConfigManager()
    {
        DEBUG_TRACE("CControllerConfigManager* GetControllerConfigManager()");
        return m_pControllerConfigManager;
    };
    CRenderWare* GetRenderWare()
    {
        DEBUG_TRACE("CRenderWare * GetRenderWare()");
        return m_pRenderWare;
    };
    CHandlingManager*   GetHandlingManager() { return m_pHandlingManager; };
    CAnimManager*       GetAnimManager() { return m_pAnimManager; }
    CStreaming*         GetStreaming() { return m_pStreaming; }
    CVisibilityPlugins* GetVisibilityPlugins() { return m_pVisibilityPlugins; }
    CKeyGen*            GetKeyGen() { return m_pKeyGen; }
    CRopes*             GetRopes() { return m_pRopes; }
    CFx*                GetFx() { return m_pFx; }
    CFxManager*         GetFxManager() { return m_pFxManager; }
    CWaterManager*      GetWaterManager() { return m_pWaterManager; }
    CWeaponStatManager* GetWeaponStatManager() { return m_pWeaponStatsManager; }
    CPointLights*       GetPointLights() { return m_pPointLights; }
    CColStore*          GetCollisionStore() override { return m_collisionStore; }
    CRenderWareSA*      GetRenderWareSA() { return m_pRenderWare; }
    CFxManagerSA*       GetFxManagerSA() { return m_pFxManager; }

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

    DWORD GetSystemTime()
    {
        DEBUG_TRACE("DWORD     GetSystemTime (  )");
        return *VAR_SystemTime;
    };
    BOOL IsAtMenu()
    {
        DEBUG_TRACE("BOOL     IsAtMenu (  )");
        if (*VAR_IsAtMenu)
            return TRUE;
        else
            return FALSE;
    };
    BOOL IsGameLoaded()
    {
        DEBUG_TRACE("BOOL     IsGameLoaded (  )");
        if (*VAR_IsGameLoaded)
            return TRUE;
        else
            return FALSE;
    };
    VOID         StartGame();
    VOID         SetSystemState(eSystemState State);
    eSystemState GetSystemState();
    BOOL         IsNastyGame()
    {
        DEBUG_TRACE("BOOL     IsNastyGame (  )");
        return *VAR_IsNastyGame;
    };
    VOID SetNastyGame(BOOL IsNasty)
    {
        DEBUG_TRACE("VOID     SetNastyGame ( BOOL IsNasty )");
        *VAR_IsNastyGame = IsNasty ? true : false;
    };
    VOID   Pause(bool bPaused);
    bool   IsPaused();
    bool   IsInForeground();
    VOID   DisableRenderer(bool bDisabled);
    VOID   TakeScreenshot(char* szFileName);
    DWORD* GetMemoryValue(DWORD dwOffset);

    VOID SetRenderHook(InRenderer* pInRenderer);

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

    BOOL InitLocalPlayer(class CClientPed* pClientPed);

    float GetGravity();
    void  SetGravity(float fGravity);

    float GetGameSpeed();
    void  SetGameSpeed(float fSpeed);

    unsigned char GetBlurLevel();
    void          SetBlurLevel(unsigned char ucLevel);

    void SetJetpackWeaponEnabled(eWeaponType weaponType, bool bEnabled);
    bool GetJetpackWeaponEnabled(eWeaponType weaponType);

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
    CRestart*                       m_pRestart;
    CClock*                         m_pClock;
    CCoronas*                       m_pCoronas;
    CCheckpoints*                   m_pCheckpoints;
    CEventList*                     m_pEventList;
    CFireManager*                   m_pFireManager;
    CGarages*                       m_pGarages;
    CHud*                           m_pHud;
    CWanted*                        m_pWanted;
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
    CTheCarGenerators*        m_pTheCarGenerators;
    CAERadioTrackManager*     m_pCAERadioTrackManager;
    CAudioEngine*             m_pAudioEngine;
    CAEAudioHardware*         m_pAEAudioHardware;
    CAESoundManager*          m_pAESoundManager;
    CAudioContainer*          m_pAudioContainer;
    CMenuManager*             m_pMenuManager;
    CStats*                   m_pStats;
    CFont*                    m_pFont;
    CPathFind*                m_pPathFind;
    CPopulation*              m_pPopulation;
    CTaskManagementSystem*    m_pTaskManagementSystem;            // not used outside the game_sa
    CTasks*                   m_pTasks;
    CGameSettings*            m_pSettings;
    CCarEnterExit*            m_pCarEnterExit;
    CControllerConfigManager* m_pControllerConfigManager;

    eGameVersion m_eGameVersion;
    bool         m_bAsyncScriptEnabled;
    bool         m_bAsyncScriptForced;
    bool         m_bASyncLoadingSuspended;
    int          m_iCheckStatus;
    bool         m_bUnderworldWarp;

    static unsigned int&  ClumpOffset;
    static unsigned long* VAR_SystemTime;
    static unsigned long* VAR_IsAtMenu;
    static unsigned long* VAR_IsGameLoaded;
    static bool*          VAR_GamePaused;
    static bool*          VAR_IsForegroundWindow;
    ;
    static unsigned long* VAR_SystemState;
    static void*          VAR_StartGame;
    static bool*          VAR_IsNastyGame;
    static float*         VAR_TimeScale;
    static float*         VAR_FPS;
    static float*         VAR_OldTimeStep;
    static float*         VAR_TimeStep;
    static unsigned long* VAR_Framelimiter;

    std::map<std::string, SCheatSA*> m_Cheats;

    SFixedArray<bool, WEAPONTYPE_LAST_WEAPONTYPE> m_JetpackWeapons;

    CPed*      m_pPedContext;
    CTickCount m_llASyncLoadingAutoUnsuspendTime;
};
