/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/multiplayer/CMultiplayer.h
*  PURPOSE:     Multiplayer subsystem interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMULTIPLAYER
#define __CMULTIPLAYER

#include <CMatrix.h>
#include <CVector.h>

#include <game/CExplosion.h>
#include <game/CStats.h>
#include "CPopulationMP.h"
#include "CLimits.h"

struct SRwResourceStats
{
    uint uiTextures;
    uint uiRasters;
    uint uiGeometries;
};

struct SClothesCacheStats
{
    uint uiCacheHit;
    uint uiCacheMiss;
    uint uiNumTotal;
    uint uiNumUnused;
    uint uiNumRemoved;
};

typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;

typedef bool ( ExplosionHandler ) ( class CEntity* pExplodingEntity, class CEntity* pCreator, const CVector& vecPosition, enum eExplosionType ExplosionType );
typedef void ( PreContextSwitchHandler ) ( class CPlayerPed* pPlayer );
typedef void ( PostContextSwitchHandler ) ( void );
typedef void ( PreWeaponFireHandler ) ( class CPlayerPed* pPlayer );
typedef void ( PostWeaponFireHandler ) ( void );
typedef void ( BulletImpactHandler ) ( class CPed* pInitiator, class CEntity* pVictim, const CVector* pvecStartPosition, const CVector* pvecEndPosition );
typedef void ( BulletFireHandler ) ( class CPed* pInitiator, const CVector* pvecStartPosition, const CVector* pvecEndPosition );
typedef bool ( DamageHandler ) ( class CPed* pDamagePed, class CEventDamage * pEvent );
typedef void ( FireHandler ) ( class CFire* pFire );
typedef bool ( ProjectileStopHandler ) ( class CEntity * owner, enum eWeaponType weaponType, class CVector * origin, float fForce, class CVector * target, class CEntity * targetEntity );
typedef void ( ProjectileHandler ) ( class CEntity * owner, class CProjectile* projectile, class CProjectileInfo* projectileInfo, enum eWeaponType weaponType, class CVector * origin, float fForce, class CVector * target, class CEntity * targetEntity );
typedef bool ( BreakTowLinkHandler ) ( class CVehicle * towingVehicle );
typedef bool ( ProcessCamHandler ) ( class CCam* pCam );
typedef void ( DrawRadarAreasHandler ) ( void );
typedef void ( Render3DStuffHandler ) ( void );
typedef bool ( ChokingHandler ) ( unsigned char ucWeaponType );
typedef void ( PreWorldProcessHandler ) ( void );
typedef void ( PostWorldProcessHandler ) ( void );
typedef void ( IdleHandler ) ( void );
typedef void ( PreFxRenderHandler ) ( void );
typedef void ( PreHudRenderHandler ) ( void );
typedef void ( AddAnimationHandler ) ( RpClump * pClump, AssocGroupId animGroup, AnimationId animID );
typedef void ( BlendAnimationHandler ) ( RpClump * pClump, AssocGroupId animGroup, AnimationId animID, float fBlendDelta );
typedef bool ( ProcessCollisionHandler ) ( class CEntitySAInterface* pThisInterface, class CEntitySAInterface* pOtherInterface );
typedef bool ( VehicleCollisionHandler ) ( class CVehicleSAInterface* pCollidingVehicle, class CEntitySAInterface* pCollidedVehicle, int iModelIndex, float fDamageImpulseMag, float fCollidingDamageImpulseMag, uint16 usPieceType, CVector vecCollisionPos, CVector vecCollisionVelocity );
typedef bool ( HeliKillHandler ) ( class CVehicleSAInterface* pVehicle, class CPedSAInterface* pPed );
typedef bool ( WaterCannonHitHandler ) ( class CVehicleSAInterface* pCannonVehicle, class CPedSAInterface* pHitPed );
typedef void ( GameObjectDestructHandler ) ( CEntitySAInterface* pObject );
typedef void ( GameVehicleDestructHandler ) ( CEntitySAInterface* pVehicle );
typedef void ( GamePlayerDestructHandler ) ( CEntitySAInterface* pPlayer );
typedef void ( GameModelRemoveHandler ) ( ushort usModelId );
typedef void ( GameEntityRenderHandler ) ( CEntitySAInterface* pEntity );

/**
 * This class contains information used for shot syncing, one exists per player.
 */
class CShotSyncData
{
public:
    CVector m_vecShotTarget;
    CVector m_vecShotOrigin;
    // so we can make the arm move vertically (mainly while on foot) and horizontally (mainly while in vehicles)
    float m_fArmDirectionX; 
    float m_fArmDirectionY;
    // only for in-vehicle shooting
    char m_cInVehicleAimDirection; // 0 = forwards, 1 = left, 2 = back, 3 = right
    // use origin
    bool m_bUseOrigin;

    bool m_bRemoteBulletSyncVectorsValid;
    CVector m_vecRemoteBulletSyncStart;
    CVector m_vecRemoteBulletSyncEnd;
};

class CStatsData
{
public:
    float       StatTypesFloat[MAX_FLOAT_STATS];
    int         StatTypesInt[MAX_INT_STATS];
    float       StatReactionValue[MAX_REACTION_STATS];
};

class CRemoteDataStorage
{
public:
    virtual class CControllerState *    CurrentControllerState ( void ) = 0;
    virtual class CControllerState *    LastControllerState ( void ) = 0;
    virtual class CShotSyncData *       ShotSyncData ( void ) = 0;
    virtual class CStatsData *          Stats ( void ) = 0;
    virtual float                       GetCameraRotation ( void ) = 0;
    virtual void                        SetCameraRotation ( float fCameraRotation ) = 0;
    virtual void                        SetGravity ( float fGravity ) = 0;
    virtual void                        SetProcessPlayerWeapon ( bool bProcess ) = 0;
    virtual CVector&                    GetAkimboTarget     ( void ) = 0;
    virtual bool                        GetAkimboTargetUp   ( void ) = 0;
    virtual void                        SetAkimboTarget     ( const CVector& vecTarget ) = 0;
    virtual void                        SetAkimboTargetUp   ( bool bUp ) = 0;
};

/**
 * \todo Remove the commented out functions if they aren't actually needed (from here and the source)
 */
class CMultiplayer
{
public:
    enum EFastClothesLoading
    {
        FAST_CLOTHES_OFF = 0,
        FAST_CLOTHES_AUTO = 1,
        FAST_CLOTHES_ON = 2,
    };

    virtual CRemoteDataStorage *        CreateRemoteDataStorage     () = 0;
    virtual void                        DestroyRemoteDataStorage    ( CRemoteDataStorage* pData ) = 0;
    virtual void                        AddRemoteDataStorage        ( CPlayerPed* pPed, CRemoteDataStorage* pData ) = 0;
    virtual void                        RemoveRemoteDataStorage     ( CPlayerPed* pPed ) = 0;

    virtual class CPed *                GetContextSwitchedPed       ( void ) = 0;

    virtual class CPopulationMP         * GetPopulationMP           () = 0;
    virtual void                        PreventLeavingVehicles      () = 0;
    virtual void                        HideRadar                   ( bool bHide ) = 0;
    virtual void                        SetCenterOfWorld            ( class CEntity * entity, class CVector * vecPosition, FLOAT fHeading ) = 0;
    virtual void                        DisablePadHandler           ( bool bDisabled ) = 0;
    virtual void                        DisableAllVehicleWeapons    ( bool bDisable ) = 0;
    virtual void                        DisableZoneNames            ( bool bDisabled ) = 0; // move to CHud
    virtual void                        DisableBirds                ( bool bDisabled ) = 0;
    virtual void                        DisableQuickReload          ( bool bDisable ) = 0;
    virtual void                        DisableCloseRangeDamage     ( bool bDisable ) = 0;

    virtual bool                        GetExplosionsDisabled       () = 0;
    virtual void                        DisableExplosions           ( bool bDisabled ) = 0;
    virtual void                        SetExplosionHandler         ( ExplosionHandler * pExplosionHandler ) = 0;
    virtual void                        SetBreakTowLinkHandler      ( BreakTowLinkHandler * pBreakTowLinkHandler ) = 0;
    virtual void                        SetDamageHandler            ( DamageHandler * pDamageHandler ) = 0;
    virtual void                        SetFireHandler              ( FireHandler * pFireHandler ) = 0;
    virtual void                        SetProcessCamHandler        ( ProcessCamHandler* pProcessCamHandler ) = 0;
    virtual void                        SetChokingHandler           ( ChokingHandler* pChokingHandler ) = 0;
    virtual void                        SetProjectileHandler        ( ProjectileHandler * pProjectileHandler ) = 0;
    virtual void                        SetProjectileStopHandler    ( ProjectileStopHandler * pProjectileHandler ) = 0;
    virtual void                        SetPreWorldProcessHandler   ( PreWorldProcessHandler * pHandler ) = 0;
    virtual void                        SetPostWorldProcessHandler  ( PostWorldProcessHandler * pHandler ) = 0;
    virtual void                        SetIdleHandler              ( IdleHandler * pHandler ) = 0;
    virtual void                        SetPreFxRenderHandler       ( PreFxRenderHandler * pHandler ) = 0;
    virtual void                        SetPreHudRenderHandler      ( PreHudRenderHandler * pHandler ) = 0;
    virtual void                        SetAddAnimationHandler      ( AddAnimationHandler * pHandler ) = 0;
    virtual void                        SetBlendAnimationHandler    ( BlendAnimationHandler * pHandler ) = 0;
    virtual void                        SetProcessCollisionHandler  ( ProcessCollisionHandler * pHandler ) = 0;
    virtual void                        SetVehicleCollisionHandler  ( VehicleCollisionHandler * pHandler ) = 0;
    virtual void                        SetHeliKillHandler          ( HeliKillHandler * pHandler ) = 0;
    virtual void                        SetWaterCannonHitHandler    ( WaterCannonHitHandler * pHandler ) = 0;
    virtual void                        SetGameObjectDestructHandler    ( GameObjectDestructHandler * pHandler ) = 0;
    virtual void                        SetGameVehicleDestructHandler   ( GameVehicleDestructHandler * pHandler ) = 0;
    virtual void                        SetGamePlayerDestructHandler    ( GamePlayerDestructHandler * pHandler ) = 0;
    virtual void                        SetGameModelRemoveHandler       ( GameModelRemoveHandler * pHandler ) = 0;
    virtual void                        SetGameEntityRenderHandler      ( GameEntityRenderHandler * pHandler ) = 0;

    virtual void                        AllowMouseMovement          ( bool bAllow ) = 0;
    virtual void                        DoSoundHacksOnLostFocus     ( bool bLostFocus ) = 0;
    virtual bool                        HasSkyColor                 () = 0;
    virtual void                        GetSkyColor                 ( unsigned char& TopRed, unsigned char& TopGreen, unsigned char& TopBlue, unsigned char& BottomRed, unsigned char& BottomGreen, unsigned char& BottomBlue ) = 0;
    virtual void                        SetSkyColor                 ( unsigned char TopRed, unsigned char TopGreen, unsigned char TopBlue, unsigned char BottomRed, unsigned char BottomGreen, unsigned char BottomBlue ) = 0;
    virtual void                        ResetSky                    () = 0;
    virtual void                        SetHeatHaze                 ( const SHeatHazeSettings& settings ) = 0;
    virtual void                        GetHeatHaze                 ( SHeatHazeSettings& settings ) = 0;
    virtual void                        ResetHeatHaze               ( void ) = 0;
    virtual bool                        HasWaterColor               () = 0;
    virtual void                        GetWaterColor               ( float& fWaterRed, float& fWaterGreen, float& fWaterBlue, float& fWaterAlpha ) = 0;
    virtual void                        SetWaterColor               ( float fWaterRed, float fWaterGreen, float fWaterBlue, float fWaterAlpha ) = 0;
    virtual void                        ResetWater                  ( void ) = 0;
    virtual void                        SetCloudsEnabled            ( bool bDisabled ) = 0;
    virtual bool                        GetInteriorSoundsEnabled    () = 0;
    virtual void                        SetInteriorSoundsEnabled    ( bool bEnabled ) = 0;
    virtual void                        SetWindVelocity             ( float fX, float fY, float fZ ) = 0;
    virtual void                        GetWindVelocity             ( float& fX, float& fY, float& fZ ) = 0;
    virtual void                        RestoreWindVelocity         ( void ) = 0;
    virtual void                        SetFarClipDistance          ( float fDistance ) = 0;
    virtual float                       GetFarClipDistance          ( void ) = 0;
    virtual void                        RestoreFarClipDistance      ( void ) = 0;
    virtual void                        SetFogDistance              ( float fDistance ) = 0;
    virtual float                       GetFogDistance              ( void ) = 0;
    virtual void                        RestoreFogDistance          ( void ) = 0;
    virtual void                        GetSunColor                 ( unsigned char& ucCoreRed, unsigned char& ucCoreGreen, unsigned char& ucCoreBlue, unsigned char& ucCoronaRed, unsigned char& ucCoronaGreen, unsigned char& ucCoronaBlue ) = 0;
    virtual void                        SetSunColor                 ( unsigned char ucCoreRed, unsigned char ucCoreGreen, unsigned char ucCoreBlue, unsigned char ucCoronaRed, unsigned char ucCoronaGreen, unsigned char ucCoronaBlue ) = 0;
    virtual void                        ResetSunColor               ( ) = 0;
    virtual float                       GetSunSize                  ( ) = 0;
    virtual void                        SetSunSize                  ( float fSize ) = 0;
    virtual void                        ResetSunSize                ( ) = 0;

    virtual void                        DisableEnterExitVehicleKey  ( bool bDisabled ) = 0;

    virtual void                        SetNightVisionEnabled       ( bool bEnabled ) = 0;
    virtual void                        SetThermalVisionEnabled     ( bool bEnabled ) = 0;
    virtual bool                        IsNightVisionEnabled        ( ) = 0;
    virtual bool                        IsThermalVisionEnabled      ( ) = 0;

    virtual void                        RebuildMultiplayerPlayer    ( class CPed * player ) = 0;

    virtual void                        AllowWindowsCursorShowing   ( bool bAllow ) = 0;

    virtual class CShotSyncData *       GetLocalShotSyncData        () = 0;

    virtual void                        SetPreContextSwitchHandler  ( PreContextSwitchHandler* pHandler ) = 0;
    virtual void                        SetPostContextSwitchHandler ( PostContextSwitchHandler* pHandler ) = 0;
    virtual void                        SetPreWeaponFireHandler     ( PreWeaponFireHandler* pHandler ) = 0;
    virtual void                        SetPostWeaponFireHandler    ( PostWeaponFireHandler* pHandler ) = 0;
    virtual void                        SetBulletImpactHandler      ( BulletImpactHandler* pHandler ) = 0;
    virtual void                        SetBulletFireHandler        ( BulletFireHandler* pHandler ) = 0;
    virtual void                        SetDrawRadarAreasHandler    ( DrawRadarAreasHandler * pRadarAreasHandler ) = 0;
    virtual void                        SetRender3DStuffHandler     ( Render3DStuffHandler * pHandler ) = 0;

    virtual void                        Reset                       () = 0;

    virtual void                        ConvertEulerAnglesToMatrix  ( CMatrix& Matrix, float fX, float fY, float fZ ) = 0;
    virtual void                        ConvertMatrixToEulerAngles  ( const CMatrix& Matrix, float& fX, float& fY, float& fZ ) = 0;

    virtual void                        ConvertEulerAnglesToMatrix  ( CMatrix& Matrix, CVector & vecAngles ) = 0;
    virtual void                        ConvertMatrixToEulerAngles  ( const CMatrix& Matrix, CVector & vecAngles ) = 0;

    virtual float                       GetGlobalGravity            () = 0;
    virtual void                        SetGlobalGravity            ( float fGravity ) = 0;

    virtual float                       GetLocalPlayerGravity       () = 0;
    virtual void                        SetLocalPlayerGravity       ( float fGravity ) = 0;

    virtual unsigned char               GetTrafficLightState        () = 0;
    virtual void                        SetTrafficLightState        ( unsigned char ucState ) = 0;

    virtual bool                        GetTrafficLightsLocked      () = 0;
    virtual void                        SetTrafficLightsLocked      ( bool bLocked ) = 0;

    virtual void                        SetLocalStatValue           ( unsigned short usStat, float fValue ) = 0;
    virtual void                        SetLocalStatsStatic         ( bool bStatic ) = 0;

    virtual void                        SetLocalCameraRotation      ( float fRotation ) = 0;
    virtual bool                        IsCustomCameraRotationEnabled () = 0;
    virtual void                        SetCustomCameraRotationEnabled ( bool bEnabled ) = 0;

    virtual void                        SetDebugVars                ( float f1, float f2, float f3 ) = 0;

    virtual CVector&                    GetAkimboTarget             () = 0;
    virtual bool                        GetAkimboTargetUp           () = 0;

    virtual void                        SetAkimboTarget             ( const CVector& vecTarget ) = 0;
    virtual void                        SetAkimboTargetUp           ( bool bUp ) = 0;

    virtual void                        AllowCreatedObjectsInVerticalLineTest ( bool bOn ) = 0;
    virtual void                        DeleteAndDisableGangTags    () = 0;

    virtual CLimits*                    GetLimits                   () = 0;

    virtual bool                        IsSuspensionEnabled         ( ) = 0;
    virtual void                        SetSuspensionEnabled        ( bool bEnabled ) = 0;

    virtual void                        SetFastClothesLoading       ( EFastClothesLoading fastClothesLoading ) = 0;
    virtual void                        SetLODSystemEnabled         ( bool bEnable ) = 0;
    virtual void                        SetAltWaterOrderEnabled     ( bool bEnable ) = 0;

    virtual float                       GetAircraftMaxHeight        ( void ) = 0;
    virtual void                        SetAircraftMaxHeight        ( float fHeight ) = 0;

    virtual void                        SetAutomaticVehicleStartupOnPedEnter    ( bool bSet ) = 0;

    virtual void                        GetRwResourceStats          ( SRwResourceStats& outStats ) = 0;
    virtual void                        GetClothesCacheStats        ( SClothesCacheStats& outStats ) = 0;
    virtual void                        SetDamageEventLimit         ( int iLimit ) = 0;
};

#endif
