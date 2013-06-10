/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CVehicle.h
*  PURPOSE:     Vehicle entity interface
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
#include "CDoor.h"
#include "CWeaponInfo.h"

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
class CColModel;

#define SIREN_TYPE_FIRST 1
#define SIREN_TYPE_LAST 6
#define SIREN_ID_MAX 7
#define SIREN_COUNT_MAX 8

struct SSirenBeaconInfo
{
    CVector                     m_vecSirenPositions;
    SColor                      m_RGBBeaconColour;
    DWORD                       m_dwMinSirenAlpha;
};
struct SSirenInfo
{
    // Flags
    bool                        m_b360Flag;
    bool                        m_bDoLOSCheck;
    bool                        m_bUseRandomiser;
    bool                        m_bSirenSilent;
    // End of flags
    bool                        m_bOverrideSirens;
    unsigned char               m_ucSirenType;
    unsigned char               m_ucSirenCount;
    unsigned char               m_ucCurrentSirenID;
    unsigned char               m_ucCurrentSirenRandomiser;
    SFixedArray < SSirenBeaconInfo, 8 > m_tSirenInfo;
    SColor                      m_tPointLightColour;
};
class CVehicle : public virtual CPhysical
{
public:
    virtual                     ~CVehicle () {};

    virtual bool                AddProjectile ( eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector * target, CEntity * targetEntity )=0;
    
    virtual CPed*               GetDriver                   () = 0;
    virtual CPed*               GetPassenger                ( unsigned char ucSlot ) = 0;
    virtual bool                IsBeingDriven               () = 0;

    virtual CVehicle *          GetNextTrainCarriage        () = 0;
    virtual void                SetNextTrainCarriage        ( CVehicle * next ) = 0;
    virtual CVehicle *          GetPreviousTrainCarriage    () = 0;
    virtual void                SetPreviousTrainCarriage    ( CVehicle * previous ) = 0;

    virtual bool                IsDerailed                  () = 0;
    virtual void                SetDerailed                 ( bool bDerailed ) = 0;
    virtual bool                IsDerailable                () = 0;
    virtual void                SetDerailable               ( bool bDerailable ) = 0;
    virtual float               GetTrainSpeed               () = 0;
    virtual void                SetTrainSpeed               ( float fSpeed ) = 0;
    virtual bool                GetTrainDirection           () = 0;
    virtual void                SetTrainDirection           ( bool bDirection ) = 0;
    virtual BYTE                GetRailTrack                () = 0;
    virtual void                SetRailTrack                ( BYTE ucTrackID ) = 0;

    virtual bool                CanPedEnterCar              () = 0;
    virtual bool                CanPedJumpOutCar            ( CPed* pPed ) = 0;
    virtual void                AddVehicleUpgrade           ( DWORD dwModelID ) = 0;
    virtual void                RemoveVehicleUpgrade        ( DWORD dwModelID ) = 0;
    virtual bool                CanPedLeanOut               ( CPed* pPed ) = 0;
    virtual bool                CanPedStepOutCar            ( bool bUnknown ) = 0;

    virtual CDoor*              GetDoor                     ( unsigned char ucDoor ) = 0;
    virtual void                OpenDoor                    ( unsigned char ucDoor, float fRatio, bool bMakeNoise = false ) = 0;
    virtual void                SetSwingingDoorsAllowed     ( bool bAllowed ) = 0;
    virtual bool                AreSwingingDoorsAllowed     () const = 0;
    virtual bool                AreDoorsLocked              () = 0;
    virtual void                LockDoors                   ( bool bLocked ) = 0;
    virtual bool                AreDoorsUndamageable        () = 0;
    virtual void                SetDoorsUndamageable        ( bool bUndamageable ) = 0;

    virtual bool                CarHasRoof                  () = 0;
    virtual void                ExtinguishCarFire           () = 0;
    virtual DWORD               GetBaseVehicleType          () = 0;

    virtual void                SetBodyDirtLevel            ( float fDirtLevel ) = 0;
    virtual float               GetBodyDirtLevel            () = 0;

    virtual unsigned char       GetCurrentGear              () = 0;
    virtual float               GetGasPedal                 () = 0;
    virtual float               GetHeightAboveRoad          () = 0;
    virtual float               GetSteerAngle               () = 0;
    virtual bool                GetTowBarPos                ( CVector* pVector, CVehicle* pTrailer ) = 0;
    virtual bool                GetTowHitchPos              ( CVector* pVector ) = 0;
    virtual bool                IsOnItsSide                 () = 0;
    virtual bool                IsLawEnforcementVehicle     () = 0;
    virtual bool                IsPassenger                 ( CPed* pPed ) = 0;
    virtual bool                IsSphereTouchingVehicle     ( CVector* vecOrigin, float fRadius ) = 0;
    virtual bool                IsUpsideDown                () = 0;
    virtual void                MakeDirty                   ( CColPoint* pPoint ) = 0;

    virtual bool                IsEngineBroken              () = 0;
    virtual void                SetEngineBroken             ( bool bEngineBroken ) = 0;
    virtual bool                IsScriptLocked              () = 0;
    virtual void                SetScriptLocked             ( bool bLocked ) = 0;

    virtual void                PlaceBikeOnRoadProperly     () = 0;
    virtual void                PlaceAutomobileOnRoadProperly() = 0;
    virtual void                SetColor                    ( SColor color1, SColor color2, SColor color3, SColor color4, int ) = 0;
    virtual void                GetColor                    ( SColor* color1, SColor* color2, SColor* color3, SColor* color4, int ) = 0;
    virtual void                Fix                         () = 0;
    virtual bool                IsSirenOrAlarmActive        () = 0;
    virtual void                SetSirenOrAlarmActive       ( bool bActive ) = 0;
    virtual DWORD               * GetMemoryValue            ( DWORD dwOffset ) = 0;
    virtual void                SetAlpha                    ( unsigned char ucAlpha ) = 0;
    virtual unsigned char       GetAlpha                    () = 0;

    virtual void                BlowUp                      ( CEntity* pCreator, unsigned long ulUnknown ) = 0;
    virtual void                BlowUpCutSceneNoExtras      ( unsigned long ulUnknown1, unsigned long ulUnknown2, unsigned long ulUnknown3, unsigned long ulUnknown4 ) = 0;

    virtual CDamageManager      * GetDamageManager          () = 0;
    virtual void                FadeOut                     ( bool bFadeOut ) = 0;
    virtual bool                IsFadingOut                 () = 0;

    virtual void                SetTowLink                  ( CVehicle* pVehicle ) = 0;
    virtual bool                BreakTowLink                () = 0;
    virtual CVehicle *          GetTowedVehicle             () = 0;
    virtual CVehicle *          GetTowedByVehicle           () = 0;

    virtual void                SetWinchType                ( eWinchType winchType ) = 0;
    virtual void                PickupEntityWithWinch       ( CEntity* pEntity ) = 0;
    virtual void                ReleasePickedUpEntityWithWinch () = 0;
    virtual void                SetRopeHeightForHeli        ( float fRopeHeight ) = 0;
    virtual CPhysical *         QueryPickedUpEntityWithWinch () = 0;

    virtual void                SetRemap                    ( int iRemap ) = 0;
    virtual int                 GetRemapIndex               () = 0;
    virtual void                SetRemapTexDictionary       ( int iRemapTextureDictionary ) = 0;

    virtual bool                GetCanBeTargettedByHeatSeekingMissiles  () = 0;
    virtual bool                IsDamaged                               () = 0;
    virtual bool                IsDrowning                              () = 0;
    virtual bool                IsEngineOn                              () = 0;
    virtual bool                IsHandbrakeOn                           () = 0;
    virtual bool                IsRCVehicle                             () = 0;
    virtual bool                GetAlwaysLeaveSkidMarks                 () = 0;
    virtual bool                GetCanBeDamaged                         () = 0;
    virtual bool                GetCanShootPetrolTank                   () = 0;
    virtual bool                GetChangeColourWhenRemapping            () = 0;
    virtual bool                GetComedyControls                       () = 0;
    virtual bool                GetGunSwitchedOff                       () = 0;
    virtual bool                GetLightsOn                             () = 0;
    virtual unsigned int        GetOverrideLights                       () = 0;
    virtual bool                GetTakeLessDamage                       () = 0;
    virtual bool                GetTyresDontBurst                       () = 0;
    virtual unsigned short      GetAdjustablePropertyValue              () = 0;
    virtual float               GetHeliRotorSpeed                       () = 0;
    virtual unsigned long       GetExplodeTime                          () = 0;
    
    virtual char                GetNitroCount                           () = 0;
    virtual float               GetNitroLevel                           () = 0;

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
    virtual void                SetHeliRotorSpeed                       ( float fSpeed ) = 0;
    virtual void                SetTaxiLightOn                          ( bool bLightState ) = 0;
    virtual void                SetExplodeTime                          ( unsigned long ulTime ) = 0;

    virtual void                SetNitroCount                           ( char cNitroCount ) = 0;
    virtual void                SetNitroLevel                           ( float fNitroLevel ) = 0;

    virtual CHandlingEntry*     GetHandlingData                         () = 0;
    virtual void                SetHandlingData                         ( CHandlingEntry* pHandling ) = 0;

    virtual void                BurstTyre                               ( BYTE bTyre ) = 0;

    virtual BYTE                GetBikeWheelStatus                      ( BYTE bWheel ) = 0;
    virtual void                SetBikeWheelStatus                      ( BYTE bWheel, BYTE bStatus ) = 0;

    virtual class CVehicleSAInterface*  GetVehicleInterface             () = 0;

    
    virtual float                GetHealth                              () = 0;
    virtual void                 SetHealth                              ( float fHealth ) = 0;
    virtual void                 SetLandingGearDown                     ( bool bLandingGearDown ) = 0;
    virtual float                GetLandingGearPosition                 () = 0;
    virtual bool                 IsLandingGearDown                      () = 0;
    virtual void                 SetLandingGearPosition                 ( float fPosition ) = 0;

    virtual void                 GetTurretRotation                      ( float * fHorizontal, float * fVertical ) = 0;
    virtual void                 SetTurretRotation                      ( float fHorizontal, float fVertical ) = 0;

    virtual unsigned char        GetNumberGettingIn                     () = 0;
    virtual unsigned char        GetPassengerCount                      () = 0;
    virtual unsigned char        GetMaxPassengerCount                   () = 0;

    virtual bool                 IsSmokeTrailEnabled                    () = 0;
    virtual void                 SetSmokeTrailEnabled                   ( bool bEnabled ) = 0;

    virtual void                 GetGravity                             ( CVector* pvecGravity ) const = 0;
    virtual void                 SetGravity                             ( const CVector* pvecGravity ) = 0;

    virtual SColor               GetHeadLightColor                      ( void ) = 0;
    virtual void                 SetHeadLightColor                      ( const SColor color ) = 0;

    virtual CObject *            SpawnFlyingComponent                   ( int i_1, unsigned int ui_2 ) = 0;
    virtual void                 SetWheelVisibility                     ( eWheels wheel, bool bVisible ) = 0;
    virtual CVector              GetWheelPosition                       ( eWheels wheel ) = 0;

    virtual bool                 IsHeliSearchLightVisible               ( void ) = 0;
    virtual void                 SetHeliSearchLightVisible              ( bool bVisible ) = 0;

    virtual CColModel*           GetSpecialColModel                     ( void ) = 0;
    virtual bool                 UpdateMovingCollision                  ( float fAngle ) = 0;

    virtual void                 RecalculateHandling                    ( void ) = 0;

    virtual void*                GetPrivateSuspensionLines              ( void ) = 0;

    virtual bool                 CheckVTBL                              ( void ) = 0;

    virtual bool                 DoesVehicleHaveSirens                  ( void ) = 0;

    virtual void                 RemoveVehicleSirens                    ( void ) = 0;
    virtual void                 GiveVehicleSirens                      ( unsigned char ucSirenType, unsigned char ucSirenCount ) = 0;
    virtual void                 SetVehicleSirenMinimumAlpha            ( unsigned char ucSirenID, DWORD dwPercentage ) = 0;
    virtual void                 SetVehicleSirenPosition                ( unsigned char ucSirenID, CVector vecPos ) = 0;
    virtual void                 GetVehicleSirenPosition                ( unsigned char ucSirenID, CVector & vecPos ) = 0;
    virtual unsigned char        GetVehicleSirenCount                   ( void ) = 0;
    virtual unsigned char        GetVehicleSirenType                    ( void ) = 0;
    virtual DWORD                GetVehicleSirenMinimumAlpha            ( unsigned char ucSirenID ) = 0;
    virtual SColor               GetVehicleSirenColour                  ( unsigned char ucSirenCount ) = 0;
    virtual void                 SetVehicleSirenColour                  ( unsigned char ucSirenID, SColor tVehicleSirenColour ) = 0;
    virtual void                 SetVehicleCurrentSirenID               ( unsigned char ucCurrentSirenID ) = 0;
    virtual unsigned char        GetVehicleCurrentSirenID               ( void ) = 0;
    virtual unsigned char        GetSirenRandomiser                     ( void ) = 0;
    virtual void                 SetSirenRandomiser                     ( unsigned char ucSirenRandomiser ) = 0;
    virtual void                 SetPointLightColour                    ( SColor tPointLightColour ) = 0;
    virtual SColor               GetPointLightColour                    ( void ) = 0;
    virtual bool                 IsSiren360EffectEnabled                ( void ) = 0;
    virtual bool                 IsSirenLOSCheckEnabled                 ( void ) = 0;
    virtual bool                 IsSirenRandomiserEnabled               ( void ) = 0;
    virtual bool                 IsSirenSilentEffectEnabled             ( void ) = 0;
    virtual void                 SetVehicleFlags                        ( bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent ) = 0;
    virtual bool                 SetComponentRotation                   ( SString vehicleComponent, CVector vecRotation ) = 0;
    virtual bool                 GetComponentRotation                   ( SString vehicleComponent, CVector &vecRotation ) = 0;
    virtual bool                 SetComponentPosition                   ( SString vehicleComponent, CVector vecPosition ) = 0;
    virtual bool                 GetComponentPosition                   ( SString vehicleComponent, CVector &vecPositionModelling ) = 0;
    virtual bool                 IsComponentPresent                     ( SString vehicleComponent ) = 0;
    virtual bool                 GetComponentMatrix                     ( SString vehicleComponent, RwMatrix &ltm, RwMatrix &modelling ) = 0;
    virtual bool                 SetComponentMatrix                     ( SString vehicleComponent, RwMatrix &ltm, RwMatrix &modelling ) = 0;
    virtual bool                 SetComponentVisible                    ( SString vehicleComponent, bool bVisible ) = 0;
    virtual bool                 GetComponentVisible                    ( SString vehicleComponent, bool &bVisible ) = 0;
    virtual std::map < SString, RwFrame * > & GetComponentMap       ( void ) = 0;

};

#endif
