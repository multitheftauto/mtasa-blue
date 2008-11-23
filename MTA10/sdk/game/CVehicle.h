/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CVehicle.h
*  PURPOSE:		Vehicle entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_VEHICLE
#define __CGAME_VEHICLE

#include "Common.h"
#include "CColPoint.h"
#include "CPhysical.h"
#include "CDamageManager.h"
#include "CHandlingManager.h"

#include <CVector.h>

#include <windows.h>

enum eWinchType 
{
    WINCH_NONE = 0,
    WINCH_BIG_MAGNET,
    WINCH_SMALL_MAGNET = 3
};
    
// forward declaration, avoid compile error
class CPed;
class CObject;
enum eWeaponType;

class CVehicle : public virtual CPhysical
{
public:
    virtual                     ~CVehicle ( void ) {};

    virtual BOOL                AddProjectile ( eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector * target, CEntity * targetEntity )=0;
    
 	virtual CPed*               GetDriver                   ( void ) = 0;
    virtual CPed*               GetPassenger                ( unsigned char ucSlot ) = 0;
	virtual BOOL		        IsBeingDriven               ( void ) = 0;

    virtual CVehicle *          GetNextTrainCarriage ( void ) = 0;
    virtual void                SetNextTrainCarriage ( CVehicle * next ) = 0;
    virtual CVehicle *          GetPreviousTrainCarriage ( void ) = 0;
    virtual void                SetPreviousTrainCarriage ( CVehicle * previous ) = 0;

    virtual bool                CanPedEnterCar ( void ) = 0;
    virtual bool                CanPedJumpOutCar ( CPed* pPed ) = 0;
    virtual void                AddVehicleUpgrade ( DWORD dwModelID ) = 0;
	virtual void				RemoveVehicleUpgrade ( DWORD dwModelID ) = 0;
    virtual bool                CanPedLeanOut ( CPed* pPed ) = 0;
    virtual bool                CanPedStepOutCar ( bool bUnknown ) = 0;

    virtual bool                AreDoorsLocked ( void )=0;
    virtual void                LockDoors ( bool bLocked )=0;
    virtual bool                AreDoorsUndamageable ( void )=0;
    virtual void                SetDoorsUndamageable ( bool bUndamageable )=0;

    virtual bool                CarHasRoof ( void ) = 0;
    virtual void                ExtinguishCarFire ( void ) = 0;
    virtual DWORD               GetBaseVehicleType ( void ) = 0;

    virtual void                SetBodyDirtLevel ( float fDirtLevel ) = 0;
    virtual float               GetBodyDirtLevel ( void ) = 0;

    virtual unsigned char       GetCurrentGear ( void ) = 0;
    virtual float               GetGasPedal ( void ) = 0;
    virtual float               GetHeightAboveRoad ( void ) = 0;
    virtual float               GetSteerAngle ( void ) = 0;
    virtual bool                GetTowBarPos ( CVector* pVector ) = 0;
    virtual bool                GetTowHitchPos ( CVector* pVector ) = 0;
    virtual bool                IsOnItsSide ( void ) = 0;
    virtual bool                IsLawEnforcementVehicle ( void ) = 0;
    virtual bool                IsPassenger ( CPed* pPed ) = 0;
    virtual bool                IsSphereTouchingVehicle ( CVector* vecOrigin, float fRadius ) = 0;
    virtual bool                IsUpsideDown ( void ) = 0;
    virtual void                MakeDirty ( CColPoint* pPoint ) = 0;

    virtual bool				IsEngineBroken ( void )=0;
    virtual void				SetEngineBroken ( bool bEngineBroken )=0;
    virtual bool                IsScriptLocked ( void )=0;
	virtual void				SetScriptLocked ( bool bLocked )=0;

	virtual VOID				PlaceBikeOnRoadProperly()=0;
	virtual VOID				PlaceAutomobileOnRoadProperly()=0;
	virtual VOID				SetColor ( char color1, char color2, char color3, char color4 )=0;
	virtual VOID				GetColor ( char * color1, char * color2, char * color3, char * color4 )=0;
    virtual void                Fix ( void )=0;
    virtual BOOL                IsSirenOrAlarmActive ( )=0;
    virtual VOID                SetSirenOrAlarmActive ( BOOL bActive )=0;
    virtual DWORD               * GetMemoryValue ( DWORD dwOffset )=0;

    virtual void                BlowUp ( CEntity* pCreator, unsigned long ulUnknown ) = 0;
    virtual void                BlowUpCutSceneNoExtras ( unsigned long ulUnknown1, unsigned long ulUnknown2, unsigned long ulUnknown3, unsigned long ulUnknown4 ) = 0;

    virtual CDamageManager		* GetDamageManager() =0 ;
    virtual VOID                FadeOut ( bool bFadeOut )=0;
    virtual bool                IsFadingOut ( void )=0;

    virtual bool                SetTowLink ( CVehicle* pVehicle )=0;
    virtual bool                BreakTowLink ( void )=0;
    virtual CVehicle *          GetTowedVehicle ( void )=0;
    virtual CVehicle *          GetTowedByVehicle ( void )=0;

    virtual void                SetWinchType ( eWinchType winchType )=0;
    virtual void                PickupEntityWithWinch ( CEntity* pEntity )=0;
    virtual void                ReleasePickedUpEntityWithWinch ( void )=0;
    virtual void                SetRopeHeightForHeli ( float fRopeHeight )=0;
    virtual CPhysical *         QueryPickedUpEntityWithWinch ( )=0;

    virtual void                SetRemap ( int iRemap )=0;
    virtual int                 GetRemapIndex ( void )=0;
    virtual void                SetRemapTexDictionary ( int iRemapTextureDictionary )=0;

    virtual bool                GetCanBeTargettedByHeatSeekingMissiles  ( void ) = 0;
    virtual bool                IsDamaged                               ( void ) = 0;
    virtual bool                IsDrowning                              ( void ) = 0;
    virtual bool                IsEngineOn                              ( void ) = 0;
    virtual bool                IsHandbrakeOn                           ( void ) = 0;
    virtual bool                IsRCVehicle                             ( void ) = 0;
    virtual bool                GetAlwaysLeaveSkidMarks                 ( void ) = 0;
    virtual bool                GetCanBeDamaged                         ( void ) = 0;
    virtual bool                GetCanShootPetrolTank                   ( void ) = 0;
    virtual bool                GetChangeColourWhenRemapping            ( void ) = 0;
    virtual bool                GetComedyControls                       ( void ) = 0;
    virtual bool                GetGunSwitchedOff                       ( void ) = 0;
    virtual bool                GetLightsOn                             ( void ) = 0;
    virtual unsigned int        GetOverrideLights                       ( void ) = 0;
    virtual bool                GetTakeLessDamage                       ( void ) = 0;
    virtual bool                GetTyresDontBurst                       ( void ) = 0;
    virtual unsigned short      GetAdjustablePropertyValue              ( void ) = 0;
    virtual float               GetHelicopterRotorSpeed                 ( void ) = 0;
    virtual unsigned long       GetExplodeTime                          ( void ) = 0;
    
    virtual void                SetAlwaysLeaveSkidMarks                 ( bool bAlwaysLeaveSkidMarks ) = 0;
    virtual void                SetCanBeDamaged                         ( bool bCanBeDamaged ) = 0;
    virtual void                SetCanBeTargettedByHeatSeekingMissiles  ( bool bEnabled ) = 0;
    virtual void                SetCanShootPetrolTank                   ( bool bCanShoot ) = 0;
    virtual void                SetChangeColourWhenRemapping            ( bool bChangeColour ) = 0;
    virtual void                SetComedyControls                       ( bool bComedyControls ) = 0;
    virtual void                SetEngineOn                             ( bool bEngineOn ) = 0;
    virtual void                SetGunSwitchedOff                       ( bool bGunsOff ) = 0;
    virtual void                SetHandbrakeOn                          ( bool bHandbrakeOn ) = 0;
    virtual void                SetLightsOn                             ( bool bLightsOn ) = 0;
    virtual void                SetOverrideLights                       ( unsigned int uiOverrideLights ) = 0;
    virtual void                SetTakeLessDamage                       ( bool bTakeLessDamage ) = 0;
    virtual void                SetTyresDontBurst                       ( bool bTyresDontBurst ) = 0;
    virtual void                SetAdjustablePropertyValue              ( unsigned short usAdjustableProperty ) = 0;
    virtual void                SetHelicopterRotorSpeed                 ( float fSpeed ) = 0;
    virtual void                SetExplodeTime                          ( unsigned long ulTime ) = 0;

    virtual CHandlingEntry*     GetHandlingData                         ( void ) = 0;
    virtual void                SetHandlingData                         ( CHandlingEntry* pHandling ) = 0;

    virtual void                BurstTyre                               ( BYTE bTyre ) = 0;

    virtual BYTE                GetBikeWheelStatus                      ( BYTE bWheel ) = 0;
    virtual void                SetBikeWheelStatus                      ( BYTE bWheel, BYTE bStatus ) = 0;

    virtual class CVehicleSAInterface*  GetVehicleInterface             ( void ) = 0;

	
	virtual FLOAT		GetHealth()=0;
	virtual VOID		SetHealth( FLOAT fHealth )=0;
	virtual void		SetLandingGearDown ( bool bLandingGearDown )=0;
	virtual float		GetLandingGearPosition ( )=0;
	virtual bool		IsLandingGearDown ( )=0;
    virtual void        SetLandingGearPosition ( float fPosition )=0;

	virtual VOID		GetTurretRotation ( float * fHorizontal, float * fVertical ) = 0;
	virtual VOID		SetTurretRotation ( float fHorizontal, float fVertical ) = 0;

    virtual unsigned char   GetNumberGettingIn ( void ) = 0;
    virtual unsigned char   GetPassengerCount ( void ) = 0;
    virtual unsigned char   GetMaxPassengerCount ( void ) = 0;

    virtual bool        IsSmokeTrailEnabled ( void ) = 0;
    virtual void        SetSmokeTrailEnabled ( bool bEnabled ) = 0;
};

#endif