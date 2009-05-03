/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/multiplayer/CMultiplayer.h
*  PURPOSE:		Multiplayer subsystem interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMULTIPLAYER
#define __CMULTIPLAYER

#include <CMatrix.h>
#include <CVector.h>

#include <game/CStats.h>
#include "CPopulationMP.h"

#define DEFAULT_WAVES -1.0f

typedef bool ( ExplosionHandler ) ( class CEntity* pExplodingEntity, class CEntity* pCreator, const CVector& vecPosition, enum eExplosionType ExplosionType );
typedef void ( PreContextSwitchHandler ) ( class CPlayerPed* pPlayer );
typedef void ( PostContextSwitchHandler ) ( void );
typedef void ( PreWeaponFireHandler ) ( class CPlayerPed* pPlayer );
typedef void ( PostWeaponFireHandler ) ( void );
typedef bool ( DamageHandler ) ( class CPed* pDamagePed, class CEventDamage * pEvent );
typedef void ( FireHandler ) ( class CFire* pFire );
typedef bool ( ProjectileStopHandler ) ( class CEntity * owner, enum eWeaponType weaponType, class CVector * origin, float fForce, class CVector * target, class CEntity * targetEntity );
typedef void ( ProjectileHandler ) ( class CEntity * owner, class CProjectile* projectile, class CProjectileInfo* projectileInfo, enum eWeaponType weaponType, class CVector * origin, float fForce, class CVector * target, class CEntity * targetEntity );
typedef bool ( BreakTowLinkHandler ) ( class CVehicle * towingVehicle );
typedef void ( ProcessCamHandler ) ( class CCam* pCam );
typedef void ( DrawRadarAreasHandler ) ( void );
typedef void ( Render3DStuffHandler ) ( void );
typedef void ( GameProcessHandler ) ( void );

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
    virtual CRemoteDataStorage *        CreateRemoteDataStorage     () = 0;
    virtual void                        DestroyRemoteDataStorage    ( CRemoteDataStorage* pData ) = 0;
    virtual void                        AddRemoteDataStorage        ( CPlayerPed* pPed, CRemoteDataStorage* pData ) = 0;
    virtual void                        RemoveRemoteDataStorage     ( CPlayerPed* pPed ) = 0;

    virtual class CPed *                GetContextSwitchedPed       ( void ) = 0;

	virtual class CPopulationMP         * GetPopulationMP           () = 0;
	virtual void						PreventLeavingVehicles      () = 0;
	virtual void						HideRadar                   ( bool bHide ) = 0;
	virtual void						SetCenterOfWorld            ( class CEntity * entity, class CVector * vecPosition, FLOAT fHeading ) = 0;
	virtual void						DisablePadHandler           ( bool bDisabled ) = 0;
	virtual void						DisableHeatHazeEffect       ( bool bDisable ) = 0;
    virtual void                        DisableAllVehicleWeapons    ( bool bDisable ) = 0;

    virtual bool                        GetExplosionsDisabled       () = 0;
    virtual void                        DisableExplosions           ( bool bDisabled ) = 0;
	virtual void						SetExplosionHandler         ( ExplosionHandler * pExplosionHandler ) = 0;
    virtual void                        SetBreakTowLinkHandler      ( BreakTowLinkHandler * pBreakTowLinkHandler ) = 0;
    virtual void                        SetDamageHandler            ( DamageHandler * pDamageHandler ) = 0;
    virtual void                        SetFireHandler              ( FireHandler * pFireHandler ) = 0;
    virtual void                        SetProcessCamHandler        ( ProcessCamHandler* pProcessCamHandler ) = 0;
	virtual void						SetGameProcessHandler       ( GameProcessHandler* pProcessHandler ) = 0;

    virtual void                        SetProjectileHandler        ( ProjectileHandler * pProjectileHandler ) = 0;
    virtual void                        SetProjectileStopHandler    ( ProjectileStopHandler * pProjectileHandler ) = 0;

    virtual void                        AllowMouseMovement          ( bool bAllow ) = 0;
    virtual void                        DoSoundHacksOnLostFocus     ( bool bLostFocus ) = 0;
    virtual void                        DisableZoneNames            ( bool bDisabled ) = 0; // move to CHud
    virtual void                        DisableBirds                ( bool bDisabled ) = 0;
    virtual void                        SetSkyColor                 ( unsigned char TopRed, unsigned char TopGreen, unsigned char TopBlue, unsigned char BottomRed, unsigned char BottomGreen, unsigned char BottomBlue ) = 0;
    virtual void                        ResetSky                    () = 0;
    virtual void                        DisableClouds               ( bool bDisabled ) = 0;
    virtual void                        DisableEnterExitVehicleKey  ( bool bDisabled ) = 0;

    virtual void                        RebuildMultiplayerPlayer    ( class CPed * player ) = 0;

    virtual void                        AllowWindowsCursorShowing   ( bool bAllow ) = 0;

    virtual class CShotSyncData *       GetLocalShotSyncData        () = 0;

    virtual void                        SetPreContextSwitchHandler  ( PreContextSwitchHandler* pHandler ) = 0;
    virtual void                        SetPostContextSwitchHandler ( PostContextSwitchHandler* pHandler ) = 0;
    virtual void                        SetPreWeaponFireHandler     ( PreWeaponFireHandler* pHandler ) = 0;
    virtual void                        SetPostWeaponFireHandler    ( PostWeaponFireHandler* pHandler ) = 0;
    virtual void                        SetDrawRadarAreasHandler    ( DrawRadarAreasHandler * pRadarAreasHandler ) = 0;
    virtual void                        SetRender3DStuffHandler     ( Render3DStuffHandler * pHandler ) = 0;

    virtual void                        Reset                       () = 0;

    virtual void                        ConvertEulerAnglesToMatrix  ( CMatrix& Matrix, float fX, float fY, float fZ ) = 0;
    virtual void                        ConvertMatrixToEulerAngles  ( const CMatrix& Matrix, float& fX, float& fY, float& fZ ) = 0;

    virtual float                       GetGlobalGravity            () = 0;
    virtual void                        SetGlobalGravity            ( float fGravity ) = 0;

    virtual float                       GetLocalPlayerGravity       () = 0;
    virtual void                        SetLocalPlayerGravity       ( float fGravity ) = 0;

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
};

#endif
