/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CGameSA.h
*  PURPOSE:     Header file for base game logic handling class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA
#define __CGAMESA

#include "CModelInfoSA.h"

#define     MAX_MEMORY_OFFSET_1_0           0xCAF008

#define     CLASS_CPlayerInfo               0xB7CD98    // ##SA##
#define     CLASS_CCamera                   0xB6F028    // ##SA##
#define     CLASS_CPad                      0xB73458    // ##SA##
#define     CLASS_CGarages                  0x96C048    // ##SA##
#define     CLASS_CFx                       0xa9ae00    // ##SA##
#define     CLASS_CMenuManager              0xBA6748    // ##SA##

#define     CLASS_RwCamera                  0xB6F97C

#define     ARRAY_WeaponInfo                0xC8AAB8    // ##SA##
#define     CLASSSIZE_WeaponInfo            112         // ##SA##
#define     NUM_WeaponInfosStdSkill         WEAPONTYPE_LAST_WEAPONTYPE
#define     NUM_WeaponInfosOtherSkill       11
#define     NUM_WeaponInfosTotal            (NUM_WeaponInfosStdSkill + (3*NUM_WeaponInfosOtherSkill)) // std, (poor, pro, special)

#define     MODELINFO_MAX                   26000       // Actual max is 25755

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

struct SCheatSA {
    BYTE*   m_byAddress; //Cheat Address
    bool    m_bEnabled; //Cheat State
    bool    m_bCanBeSet; //Cheat can be set with setWorldSpecialPropertyEnabled
    SCheatSA(BYTE* Address, bool bCanBeSet = true) {
        m_byAddress = Address;
        m_bCanBeSet = bCanBeSet;
    }
};
class CGameSA : public CGame
{
    friend class COffsets;

private:
    CWeaponInfo         * WeaponInfos[NUM_WeaponInfosTotal];
    CModelInfoSA        ModelInfo[MODELINFO_MAX];
public:
    ZERO_ON_NEW

    CGameSA(); // constructor
    ~CGameSA ();

    inline CPoolsSA                     * GetPools()                { DEBUG_TRACE("CPools     * GetPools()"); return m_pPools; };
    inline CPlayerInfoSA                * GetPlayerInfo()           { DEBUG_TRACE("CPlayerInfo    * GetPlayerInfo()");return m_pPlayerInfo; };
    inline CProjectileInfoSA            * GetProjectileInfo()       { DEBUG_TRACE("CProjectileInfo   * GetProjectileInfo()");return m_pProjectileInfo; };
    inline CRadarSA                     * GetRadar()                { DEBUG_TRACE("CRadar     * GetRadar()"); return m_pRadar; };
    inline CRestartSA                   * GetRestart()              { DEBUG_TRACE("CRestart    * GetRestart()"); return m_pRestart; };
    inline CClockSA                     * GetClock()                { DEBUG_TRACE("CClock     * GetClock()"); return m_pClock; };
    inline CCoronasSA                   * GetCoronas()              { DEBUG_TRACE("CCoronas    * GetCoronas()"); return m_pCoronas; };
    inline CCheckpointsSA               * GetCheckpoints()          { DEBUG_TRACE("CCheckpoints   * GetCheckpoints()"); return m_pCheckpoints; };
    inline CEventListSA                 * GetEventList()            { DEBUG_TRACE("CEventList    * GetEventList()"); return m_pEventList; };
    inline CFireManagerSA               * GetFireManager()          { DEBUG_TRACE("CFireManager   * GetFireManager()"); return m_pFireManager; };
    inline CExplosionManagerSA          * GetExplosionManager()     { DEBUG_TRACE("CExplosionManager  * GetExplosionManager()");return m_pExplosionManager; };
    inline CGaragesSA                   * GetGarages()              { DEBUG_TRACE("CGarages    * GetGarages()"); return m_pGarages; };
    inline CHudSA                       * GetHud()                  { DEBUG_TRACE("CHud     * GetHud()"); return m_pHud; };
    inline CWeatherSA                   * GetWeather()              { DEBUG_TRACE("CWeather    * GetWeather()");return m_pWeather; };
    inline CWorldSA                     * GetWorld()                { DEBUG_TRACE("CWorld     * GetWorld()"); return m_pWorld; };
    inline CCameraSA                    * GetCamera()               { DEBUG_TRACE("CCamera     * GetCamera()"); return m_pCamera; };
    inline CPickupsSA                   * GetPickups()              { DEBUG_TRACE("CPickups    * GetPickups()"); return m_pPickups; };
    inline C3DMarkersSA                 * Get3DMarkers()            { DEBUG_TRACE("C3DMarkers    * Get3DMarkers()");return m_p3DMarkers; };
    inline CPadSA                       * GetPad()                  { DEBUG_TRACE("CPad     * GetPad()");return m_pPad; };
    inline CTheCarGeneratorsSA          * GetTheCarGenerators()     { DEBUG_TRACE("CTheCarGenerators  * GetTheCarGenerators()");return m_pTheCarGenerators; };
    inline CAERadioTrackManagerSA       * GetAERadioTrackManager()  { DEBUG_TRACE("CAERadioTrackManager * GetAERadioTrackManager()");return m_pCAERadioTrackManager; };
    inline CAudioEngineSA               * GetAudioEngine()          { DEBUG_TRACE("CAudio     * GetAudioEngine()");return m_pAudioEngine; };
    inline CAudioEngineSA               * GetAudio()                { DEBUG_TRACE("CAudio     * GetAudioEngine()");return m_pAudioEngine; };
    inline CAudioContainerSA            * GetAudioContainer()       { DEBUG_TRACE("CAudioContainer  * GetAudioContainer()");return m_pAudioContainer; }
    inline CMenuManagerSA               * GetMenuManager()          { DEBUG_TRACE("CMenuManager         * GetMenuManager()");return m_pMenuManager; };
    inline CTextSA                      * GetText()                 { DEBUG_TRACE("CText                    * GetText()");return m_pText; };
    inline CStatsSA                     * GetStats()                { DEBUG_TRACE("CStats                   * GetStats()");return m_pStats; };
    inline CFontSA                      * GetFont()                 { DEBUG_TRACE("CFont                    * GetFont()");return m_pFont; };
    inline CPathFindSA                  * GetPathFind()             { DEBUG_TRACE("CPathFind                * GetPathFind()");return m_pPathFind; };
    inline CPopulationSA                * GetPopulation()           { DEBUG_TRACE("CPopulation              * GetPopulation()");return m_pPopulation; };
    inline CTaskManagementSystemSA      * GetTaskManagementSystem() { DEBUG_TRACE("CTaskManagementSystemSA * GetTaskManagementSystem()");return m_pTaskManagementSystem; };
    inline CTasksSA                     * GetTasks()                { DEBUG_TRACE("CTasks * GetTasks()");return m_pTasks; };
    inline CGameSettings                * GetSettings()             { DEBUG_TRACE("CGameSettings * GetSettings()");return m_pSettings; };
    inline CCarEnterExitSA              * GetCarEnterExit()         { DEBUG_TRACE("CCarEnterExit           * GetCarEnterExit()");return m_pCarEnterExit; };
    inline CControllerConfigManagerSA   * GetControllerConfigManager()  { DEBUG_TRACE("CControllerConfigManager* GetControllerConfigManager()");return m_pControllerConfigManager; };
    inline CRenderWareSA                * GetRenderWare()           { DEBUG_TRACE("CRenderWare * GetRenderWare()");return m_pRenderWare; };
    inline CTextureManagerSA            * GetTextureManager()        { return m_textureManager; }
    inline CHandlingManagerSA           * GetHandlingManager ()      { return m_pHandlingManager; };
    inline CAnimManagerSA               * GetAnimManager ()          { return m_pAnimManager; }
    inline CStreamingSA                 * GetStreaming ()            { return m_pStreaming; }
    inline CVisibilityPluginsSA         * GetVisibilityPlugins ()    { return m_pVisibilityPlugins; }
    inline CKeyGenSA                    * GetKeyGen ()               { return m_pKeyGen; }
    inline CRopesSA                     * GetRopes ()                { return m_pRopes; }
    inline CRecordingsSA                * GetRecordings()            { return m_pRecordings; }
    inline CFxSA                        * GetFx ()                   { return m_pFx; }
    inline CWaterManagerSA              * GetWaterManager ()         { return m_pWaterManager; }
    inline CWeaponStatManagerSA         * GetWeaponStatManager()     { return m_pWeaponStatsManager; }
    inline CPointLightsSA               * GetPointLights ()          { return m_pPointLights; }

    CWeaponInfo             * GetWeaponInfo(eWeaponType weapon,eWeaponSkill skill=WEAPONSKILL_STD);
    CModelInfoSA            * GetModelInfo( DWORD dwModelID );

    inline DWORD            GetSystemTime (  )      { DEBUG_TRACE("DWORD     GetSystemTime (  )");return *VAR_SystemTime; };
    inline BOOL             IsAtMenu (  )           { DEBUG_TRACE("BOOL     IsAtMenu (  )");if(*VAR_IsAtMenu) return TRUE; else return FALSE; };
    inline BOOL             IsGameLoaded (  )       { DEBUG_TRACE("BOOL     IsGameLoaded (  )");if(*VAR_IsGameLoaded) return TRUE; else return FALSE; };
    VOID                    StartGame ( );
    VOID                    SetSystemState ( eSystemState State );
    eSystemState            GetSystemState ( );
    inline BOOL             IsNastyGame (  )                    { DEBUG_TRACE("BOOL     IsNastyGame (  )"); return *VAR_IsNastyGame; };
    inline VOID             SetNastyGame ( BOOL IsNasty )       { DEBUG_TRACE("VOID     SetNastyGame ( BOOL IsNasty )"); *VAR_IsNastyGame = IsNasty?true:false; };
    VOID                    Pause ( bool bPaused );
    bool                    IsPaused ( );
    bool                    IsInForeground ( );
    VOID                    DisableRenderer( bool bDisabled );
    VOID                    TakeScreenshot ( char * szFileName );
    DWORD                   * GetMemoryValue ( DWORD dwOffset );

    VOID                    SetRenderHook ( InRenderer* pInRenderer );

    void                    Initialize  ( void );
    void                    Reset                       ( void );
    void                    Terminate ( void );

    eGameVersion            GetGameVersion ( void );
    eGameVersion            FindGameVersion ( void );

    float                   GetFPS ( void );
    float                   GetTimeStep ( void );
    float                   GetOldTimeStep ( void );
    float                   GetTimeScale ( void );
    void                    SetTimeScale ( float fTimeScale );

    BOOL                    InitLocalPlayer(  );

    float                   GetGravity              ( void );
    void                    SetGravity              ( float fGravity );

    float                   GetGameSpeed            ( void );
    void                    SetGameSpeed            ( float fSpeed );

    unsigned char           GetBlurLevel            ( void );
    void                    SetBlurLevel            ( unsigned char ucLevel );

    void                    SetJetpackWeaponEnabled     ( eWeaponType weaponType, bool bEnabled );
    bool                    GetJetpackWeaponEnabled     ( eWeaponType weaponType );

    unsigned long           GetMinuteDuration       ( void );
    void                    SetMinuteDuration       ( unsigned long ulTime );

    bool                    IsCheatEnabled          ( const char* szCheatName );
    bool                    SetCheatEnabled         ( const char* szCheatName, bool bEnable );
    void                    ResetCheats             ();

    bool                    VerifySADataFileNames   ();
    bool                    PerformChecks           ();
    int&                    GetCheckStatus          ( void )            { return m_iCheckStatus; }


    // Clothes stuff that needs research. (CClothesBuilder.cache.cpp)
    void                    FlushClothesCache               ( void );
    void                    GetClothesCacheStats            ( SClothesCacheStats& outStats );
    void                    InitHooks_ClothesCache          ( void );

    // Rendering callback stuff that needs research. (CEntitySA.renderfeedback.cpp)
    void                    SetGameEntityRenderHandler      ( GameEntityRenderHandler * pHandler );
    void                    SetPreRenderSkyHandler          ( PreRenderSkyHandler * pHandler );
    void                    SetIsMinimizedAndNotConnected   ( bool bIsMinimizedAndNotConnected );
    void                    SetMirrorsEnabled               ( bool bEnabled );
    void                    InitHooks_Rendering             ( void );

    // LOD System stuff (CEntitySA.lod.cpp)
    void                    SetLODSystemEnabled             ( bool bEnable );

    // RenderWare resource statistics
    void                    GetRwResourceStats              ( SRwResourceStats& outStats );
    void                    InitHooks_RwResources           ( void );

    void                    SetAsyncLoadingFromScript       ( bool bScriptEnabled, bool bScriptForced );
    void                    SuspendASyncLoading             ( bool bSuspend );
    bool                    IsASyncLoadingEnabled           ( bool bIgnoreSuspend = false );

    bool                    HasCreditScreenFadedOut         ( void );

    void                    SetupSpecialCharacters          ( void );
    CWeapon *               CreateWeapon                    ( void );
    CWeaponStat *           CreateWeaponStat                ( eWeaponType weaponType, eWeaponSkill weaponSkill );
    void                    FlushPendingRestreamIPL         ( void );
    void                    ResetModelLodDistances          ( void );
    void                    ResetAlphaTransparencies         ( void );
    void                    DisableVSync                    ( void );

    void                    OnPedContextChange              ( CPed* pPedContext );
    CPed*                   GetPedContext                   ( void );

    void                    GetShaderReplacementStats       ( SShaderReplacementStats& outStats );

    void                    SetPreWeaponFireHandler         ( PreWeaponFireHandler* pPreWeaponFireHandler )     { m_pPreWeaponFireHandler = pPreWeaponFireHandler; }
    void                    SetPostWeaponFireHandler        ( PostWeaponFireHandler* pPostWeaponFireHandler )   { m_pPostWeaponFireHandler = pPostWeaponFireHandler; }
    void                    SetTaskSimpleBeHitHandler       ( TaskSimpleBeHitHandler* pTaskSimpleBeHitHandler ) { m_pTaskSimpleBeHitHandler = pTaskSimpleBeHitHandler; }

    PreWeaponFireHandler*   m_pPreWeaponFireHandler;
    PostWeaponFireHandler*  m_pPostWeaponFireHandler;
    TaskSimpleBeHitHandler* m_pTaskSimpleBeHitHandler;
private:
    CPoolsSA                * m_pPools;
    CPlayerInfoSA           * m_pPlayerInfo;
    CProjectileInfoSA       * m_pProjectileInfo;
    CRadarSA                * m_pRadar;
    CRestartSA              * m_pRestart;
    CClockSA                * m_pClock;
    CCoronasSA              * m_pCoronas;
    CCheckpointsSA          * m_pCheckpoints;
    CEventListSA            * m_pEventList;
    CFireManagerSA          * m_pFireManager;
    CGaragesSA              * m_pGarages;
    CHudSA                  * m_pHud;
    CWantedSA               * m_pWanted;
    CWeatherSA              * m_pWeather;
    CWorldSA                * m_pWorld;
    CCameraSA               * m_pCamera;
    CModelInfoSA            * m_pModelInfo; 
    CPickupsSA              * m_pPickups;
    CWeaponInfoSA           * m_pWeaponInfo;
    CExplosionManagerSA     * m_pExplosionManager;
    C3DMarkersSA            * m_p3DMarkers;
    CRenderWareSA           * m_pRenderWare;
    CRwExtensionManagerSA   * m_pRwExtensionManager;
    CTextureManagerSA       * m_textureManager;
    CHandlingManagerSA      * m_pHandlingManager;
    CAnimManagerSA          * m_pAnimManager;
    CStreamingSA            * m_pStreaming;
    CVisibilityPluginsSA    * m_pVisibilityPlugins;
    CKeyGenSA               * m_pKeyGen;
    CRopesSA                * m_pRopes;
    CFxSA                   * m_pFx;
    CRecordingsSA           * m_pRecordings;
    CWaterManagerSA         * m_pWaterManager;
    CWeaponStatManagerSA    * m_pWeaponStatsManager;
    CPointLightsSA          * m_pPointLights;

    CPadSA                      * m_pPad;
    CTheCarGeneratorsSA         * m_pTheCarGenerators;
    CAERadioTrackManagerSA      * m_pCAERadioTrackManager;
    CAudioEngineSA              * m_pAudioEngine;
    CAudioContainerSA           * m_pAudioContainer;
    CMenuManagerSA              * m_pMenuManager;
    CTextSA                     * m_pText;
    CStatsSA                    * m_pStats;
    CFontSA                     * m_pFont;
    CPathFindSA                 * m_pPathFind;
    CPopulationSA               * m_pPopulation;
    CTaskManagementSystemSA     * m_pTaskManagementSystem; // not used outside the game_sa
    CTasksSA                    * m_pTasks;
    CGameSettings               * m_pSettings;
    CCarEnterExitSA             * m_pCarEnterExit;
    CControllerConfigManagerSA  * m_pControllerConfigManager;

    eGameVersion            m_eGameVersion;
    bool                    m_bAsyncScriptEnabled;
    bool                    m_bAsyncScriptForced;
    bool                    m_bASyncLoadingSuspended;
    int                     m_iCheckStatus;

    static unsigned long*   VAR_SystemTime;
    static unsigned long*   VAR_IsAtMenu;
    static unsigned long*   VAR_IsGameLoaded;
    static bool*            VAR_GamePaused;
    static bool*            VAR_IsForegroundWindow;;
    static unsigned long*   VAR_SystemState;
    static void*            VAR_StartGame;
    static bool*            VAR_IsNastyGame;
    static float*           VAR_TimeScale;
    static float*           VAR_FPS;
    static float*           VAR_OldTimeStep;
    static float*           VAR_TimeStep;
    static unsigned long*   VAR_Framelimiter;

    std::map < std::string, SCheatSA* > m_Cheats;

    SFixedArray < bool, WEAPONTYPE_LAST_WEAPONTYPE > m_JetpackWeapons;

    CPed*                   m_pPedContext;

    // Custom modules stuff.
    bool                    m_bEnabledLODSystem;
};

extern CFileTranslator *gameFileRoot;

// Utility functions
CFile*  OpenGlobalStream( const char *filename, const char *mode );

#endif

