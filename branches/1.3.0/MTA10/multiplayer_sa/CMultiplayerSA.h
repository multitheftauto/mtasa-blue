/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA.h
*  PURPOSE:     Multiplayer module class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Peter Beverloo <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMULTIPLAYERSA
#define __CMULTIPLAYERSA

#include <game/CGame.h>
#include <multiplayer/CMultiplayer.h>

#include "CPopulationSA.h"
#include "multiplayersa_init.h"
#include "CLimitsSA.h"

#include "CRemoteDataSA.h"
class CRemoteDataSA;

class CMultiplayerSA : public CMultiplayer
{
    friend class COffsetsMP;

private:
    CRemoteDataSA               * RemoteData;
    CPopulationSA               * Population;
public:
    ZERO_ON_NEW

    CMultiplayerSA();
    void                        InitHooks();
    void                        InitHooks_CrashFixHacks();
    void                        Init_13 ( void );
    void                        InitHooks_13 ( void );
    void                        InitMemoryCopies_13 ( void );
    void                        InitHooks_ClothesSpeedUp ( void );
    void                        InitHooks_FixBadAnimId ( void );
    void                        InitHooks_HookDestructors ( void );
    void                        InitHooks_RwResources ( void );
    void                        InitHooks_ClothesCache ( void );
    CRemoteDataStorage *        CreateRemoteDataStorage     ();
    void                        DestroyRemoteDataStorage    ( CRemoteDataStorage* pData );
    void                        AddRemoteDataStorage        ( CPlayerPed* pPed, CRemoteDataStorage* pData );
    void                        RemoveRemoteDataStorage     ( CPlayerPed* pPed );
    void                        EnableHooks_ClothesMemFix   ( bool bEnable );

    CPed *                      GetContextSwitchedPed       ( void );

    CPopulationMP               * GetPopulationMP           () { return Population; }
    void                        PreventLeavingVehicles      ();
    void                        HideRadar                   ( bool bHide );
    void                        SetCenterOfWorld            ( CEntity * entity, CVector * vecPosition, FLOAT fHeading );
    void                        DisablePadHandler           ( bool bDisabled );
    void                        DisableEnterExitVehicleKey  ( bool bDisabled );
    void                        DisableAllVehicleWeapons    ( bool bDisable );
    void                        DisableZoneNames            ( bool bDisabled );
    void                        DisableBirds                ( bool bDisabled );
    void                        DisableQuickReload          ( bool bDisable );
    void                        DisableCloseRangeDamage     ( bool bDisable );

    bool                        GetExplosionsDisabled       ();
    void                        DisableExplosions           ( bool bDisabled );
    void                        SetExplosionHandler         ( ExplosionHandler * pExplosionHandler );
    void                        SetDamageHandler            ( DamageHandler * pDamageHandler );
    void                        SetProjectileHandler        ( ProjectileHandler * pProjectileHandler );
    void                        SetProjectileStopHandler    ( ProjectileStopHandler * pProjectileHandler );
    void                        SetFireHandler              ( FireHandler * pFireHandler );
    void                        SetBreakTowLinkHandler      ( BreakTowLinkHandler * pBreakTowLinkHandler );
    void                        SetProcessCamHandler        ( ProcessCamHandler* pProcessCamHandler );
    void                        SetChokingHandler           ( ChokingHandler* pChokingHandler );
    void                        SetPreWorldProcessHandler   ( PreWorldProcessHandler * pHandler );
    void                        SetPostWorldProcessHandler  ( PostWorldProcessHandler * pHandler );
    void                        SetIdleHandler              ( IdleHandler * pHandler );
    void                        SetPreFxRenderHandler       ( PreFxRenderHandler * pHandler );
    void                        SetPreHudRenderHandler      ( PreHudRenderHandler * pHandler );
    void                        SetAddAnimationHandler      ( AddAnimationHandler * pHandler );
    void                        SetBlendAnimationHandler    ( BlendAnimationHandler * pHandler );
    void                        SetProcessCollisionHandler  ( ProcessCollisionHandler * pHandler );
    void                        SetVehicleCollisionHandler  ( VehicleCollisionHandler * pHandler );
    void                        SetHeliKillHandler          ( HeliKillHandler * pHandler );
    void                        SetWaterCannonHitHandler    ( WaterCannonHitHandler * pHandler );
    void                        SetGameObjectDestructHandler    ( GameObjectDestructHandler * pHandler );
    void                        SetGameVehicleDestructHandler   ( GameVehicleDestructHandler * pHandler );
    void                        SetGamePlayerDestructHandler    ( GamePlayerDestructHandler * pHandler );
    void                        SetGameModelRemoveHandler       ( GameModelRemoveHandler * pHandler );

    void                        AllowMouseMovement          ( bool bAllow );
    void                        DoSoundHacksOnLostFocus     ( bool bLostFocus );
    bool                        HasSkyColor                 ();
    void                        GetSkyColor                 ( unsigned char& TopRed, unsigned char& TopGreen, unsigned char& TopBlue, unsigned char& BottomRed, unsigned char& BottomGreen, unsigned char& BottomBlue );
    void                        SetSkyColor                 ( unsigned char TopRed, unsigned char TopGreen, unsigned char TopBlue, unsigned char BottomRed, unsigned char BottomGreen, unsigned char BottomBlue );
    void                        SetHeatHaze                 ( const SHeatHazeSettings& settings );
    void                        GetHeatHaze                 ( SHeatHazeSettings& settings );
    void                        ResetHeatHaze               ( void );
    void                        ResetSky                    ();
    bool                        HasWaterColor               ();
    void                        GetWaterColor               ( float& fWaterRed, float& fWaterGreen, float& fWaterBlue, float& fWaterAlpha );
    void                        SetWaterColor               ( float fWaterRed, float fWaterGreen, float fWaterBlue, float fWaterAlpha );
    void                        ResetWater                  ();
    void                        SetCloudsEnabled            ( bool bDisabled );
    void                        RebuildMultiplayerPlayer    ( CPed * player );
    bool                        GetInteriorSoundsEnabled    ();
    void                        SetInteriorSoundsEnabled    ( bool bEnabled );
    void                        SetWindVelocity             ( float fX, float fY, float fZ );
    void                        GetWindVelocity             ( float& fX, float& fY, float& fZ );
    void                        RestoreWindVelocity         ( void );
    float                       GetFarClipDistance          ( void );
    void                        SetFarClipDistance          ( float fDistance );
    void                        RestoreFarClipDistance      ( void );
    float                       GetFogDistance              ( void );
    void                        SetFogDistance              ( float fDistance );
    void                        RestoreFogDistance          ( void );
    void                        GetSunColor                 ( unsigned char& ucCoreRed, unsigned char& ucCoreGreen, unsigned char& ucCoreBlue, unsigned char& ucCoronaRed, unsigned char& ucCoronaGreen, unsigned char& ucCoronaBlue );
    void                        SetSunColor                 ( unsigned char ucCoreRed, unsigned char ucCoreGreen, unsigned char ucCoreBlue, unsigned char ucCoronaRed, unsigned char ucCoronaGreen, unsigned char ucCoronaBlue );
    void                        ResetSunColor               ( );
    float                       GetSunSize                  ( );
    void                        SetSunSize                  ( float fSize );
    void                        ResetSunSize                ( );

    void                        SetNightVisionEnabled       ( bool bEnabled );
    void                        SetThermalVisionEnabled     ( bool bEnabled );
    bool                        IsNightVisionEnabled        ( );
    bool                        IsThermalVisionEnabled      ( );

    void                        AllowWindowsCursorShowing   ( bool bAllow );

    CShotSyncData *             GetLocalShotSyncData        ();

    void                        SetPreContextSwitchHandler  ( PreContextSwitchHandler* pHandler );
    void                        SetPostContextSwitchHandler ( PostContextSwitchHandler* pHandler );
    void                        SetPreWeaponFireHandler     ( PreWeaponFireHandler* pHandler );
    void                        SetPostWeaponFireHandler    ( PostWeaponFireHandler* pHandler );
    void                        SetBulletImpactHandler      ( BulletImpactHandler* pHandler );
    void                        SetBulletFireHandler        ( BulletFireHandler* pHandler );
    void                        SetDrawRadarAreasHandler    ( DrawRadarAreasHandler * pRadarAreasHandler );
    void                        SetRender3DStuffHandler     ( Render3DStuffHandler * pHandler );

    void                        Reset                       ();

    void                        ConvertEulerAnglesToMatrix  ( CMatrix& Matrix, float fX, float fY, float fZ );
    void                        ConvertMatrixToEulerAngles  ( const CMatrix& Matrix, float& fX, float& fY, float& fZ );
    
    inline void                 ConvertEulerAnglesToMatrix  ( CMatrix& Matrix, CVector & vecAngles )            { ConvertEulerAnglesToMatrix ( Matrix, vecAngles.fX, vecAngles.fY, vecAngles.fZ ); }
    inline void                 ConvertMatrixToEulerAngles  ( const CMatrix& Matrix, CVector & vecAngles )      { ConvertMatrixToEulerAngles ( Matrix, vecAngles.fX, vecAngles.fY, vecAngles.fZ ); }

    float                       GetGlobalGravity            ();
    void                        SetGlobalGravity            ( float fGravity );

    float                       GetLocalPlayerGravity       ();
    void                        SetLocalPlayerGravity       ( float fGravity );

    unsigned char               GetTrafficLightState        ();
    void                        SetTrafficLightState        ( unsigned char ucState );

    bool                        GetTrafficLightsLocked      ();
    void                        SetTrafficLightsLocked      ( bool bLocked );

    void                        SetLocalStatValue           ( unsigned short usStat, float fValue );
    void                        SetLocalStatsStatic         ( bool bStatic );

    void                        SetLocalCameraRotation      ( float fRotation );
    bool                        IsCustomCameraRotationEnabled   ();
    void                        SetCustomCameraRotationEnabled  ( bool bEnabled );

    void                        SetDebugVars                ( float f1, float f2, float f3 );

    CVector&                    GetAkimboTarget             () { return m_vecAkimboTarget; };
    bool                        GetAkimboTargetUp           () { return m_bAkimboTargetUp; };

    void                        SetAkimboTarget             ( const CVector& vecTarget ) { m_vecAkimboTarget = vecTarget; };
    void                        SetAkimboTargetUp           ( bool bUp ) { m_bAkimboTargetUp = bUp; };

    void                        AllowCreatedObjectsInVerticalLineTest ( bool bOn );
    void                        DeleteAndDisableGangTags    ();
    
    CLimits*                    GetLimits () { return &m_limits; }

    void                        SetSuspensionEnabled        ( bool bEnabled );
    bool                        IsSuspensionEnabled         ( void )                    { return m_bSuspensionEnabled; };

    virtual void                SetFastClothesLoading       ( EFastClothesLoading fastClothesLoading );
    virtual void                SetLODSystemEnabled         ( bool bEnable );
    virtual void                SetAltWaterOrderEnabled     ( bool bEnable );

    float                       GetAircraftMaxHeight        ( void )                    { return m_fAircraftMaxHeight; };
    void                        SetAircraftMaxHeight        ( float fHeight )           { m_fAircraftMaxHeight = fHeight; };

    virtual void                GetRwResourceStats          ( SRwResourceStats& outStats );
    virtual void                GetClothesCacheStats        ( SClothesCacheStats& outStats );
    virtual CEntitySAInterface* GetRenderingGameEntity      ( void );

    CVector                     m_vecAkimboTarget;
    bool                        m_bAkimboTargetUp;
    static char*                ms_PlayerImgCachePtr;
private:
    bool                        m_bSuspensionEnabled;
    std::vector < char >        m_PlayerImgCache;
    EFastClothesLoading         m_FastClothesLoading;
    CLimitsSA                   m_limits;
    bool                        m_bEnabledLODSystem;
    bool                        m_bEnabledAltWaterOrder;
    bool                        m_bEnabledClothesMemFix;
    float                       m_fAircraftMaxHeight;

/*  VOID                        SetPlayerShotVectors(CPlayerPed* player, Vector3D * vecTarget, Vector3D * vecStart);
    VOID                        SetPlayerCameraVectors(CPlayerPed* player, Vector3D * vecSource, Vector3D * vecFront);
    Vector3D                    * GetLocalShotOriginVector();*/
    //Vector3D                  * GetLocalTargetVector();
    //Vector3D                  * GetLocalCrossProductVector();
    /*VOID                      SetOnFootPlayerKeyStates(CPlayerPed * player, DWORD keys);
    DWORD                       GetOnFootLocalPlayerKeyStates();
    VOID                        SetInVehiclePlayerKeyStates(CPlayerPed * player, WORD keys);
    WORD                        GetInVehicleLocalPlayerKeyStates();*/
//  Vector3D                    * GetLocalStartVector();
//  VOID                        SetPlayerStartVector(CPlayerPed* player, Vector3D * vecStart);

private:
    static unsigned long        HOOKPOS_FindPlayerCoors;
    static unsigned long        HOOKPOS_FindPlayerCentreOfWorld;
    static unsigned long        HOOKPOS_FindPlayerHeading;
    static unsigned long        HOOKPOS_CStreaming_Update_Caller;
    static unsigned long        HOOKPOS_CHud_Draw_Caller;
    static unsigned long        HOOKPOS_CRunningScript_Process;
    static unsigned long        HOOKPOS_CExplosion_AddExplosion;
    static unsigned long        HOOKPOS_CRealTimeShadowManager__ReturnRealTimeShadow;
    static unsigned long        HOOKPOS_CCustomRoadsignMgr__RenderRoadsignAtomic;
    static unsigned long        HOOKPOS_Trailer_BreakTowLink;
    static unsigned long        HOOKPOS_CRadar__DrawRadarGangOverlay;
    static unsigned long        HOOKPOS_CTaskComplexJump__CreateSubTask;
    static unsigned long        HOOKPOS_CTrain_ProcessControl_Derail;
    static unsigned long        HOOKPOS_CVehicle_SetupRender;
    static unsigned long        HOOKPOS_CVehicle_ResetAfterRender;
    static unsigned long        HOOKPOS_CObject_Render;
    static unsigned long        HOOKPOS_EndWorldColors;
    static unsigned long        HOOKPOS_CWorld_ProcessVerticalLineSectorList;
    static unsigned long        HOOKPOS_ComputeDamageResponse_StartChoking;
    static unsigned long        HOOKPOS_CAutomobile__ProcessSwingingDoor;

    static unsigned long        FUNC_CStreaming_Update;
    static unsigned long        FUNC_CAudioEngine__DisplayRadioStationName;
    static unsigned long        FUNC_CHud_Draw;

    static unsigned long        ADDR_CursorHiding;
    static unsigned long        ADDR_GotFocus;

    static unsigned long        FUNC_CPlayerInfoBase;
};

#endif
