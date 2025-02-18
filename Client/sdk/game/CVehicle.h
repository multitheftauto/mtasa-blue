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

#pragma once

#include <vector>
#include <map>
#include <CVector.h>
#include "Common.h"
#include "CPhysical.h"
#include "CWeaponInfo.h"
#include "CDamageManager.h"

class CAEVehicleAudioEntity;
class CColModel;
class CDoor;
class CEntity;
class CFlyingHandlingEntry;
class CHandlingEntry;
class CMatrix;
class CObject;
class CPed;
class SString;
struct RwFrame;

enum eWinchType
{
    WINCH_NONE = 0,
    WINCH_BIG_MAGNET,
    WINCH_SMALL_MAGNET = 3
};

enum eDoorLock : int32_t
{
    DOOR_LOCK_NOT_USED,
    DOOR_LOCK_UNLOCKED,
    DOOR_LOCK_LOCKED,
    DOOR_LOCK_LOCKOUT_PLAYER_ONLY,
    DOOR_LOCK_LOCKED_PLAYER_INSIDE,
    DOOR_LOCK_COP_CAR,
    DOOR_LOCK_FORCE_SHUT_DOORS,
    DOOR_LOCK_SKIP_SHUT_DOORS,
};

#define SIREN_TYPE_FIRST 1
#define SIREN_TYPE_LAST 6
#define SIREN_ID_MAX 7
#define SIREN_COUNT_MAX 8

struct SSirenBeaconInfo
{
    CVector            m_vecSirenPositions;
    SharedUtil::SColor m_RGBBeaconColour;
    DWORD              m_dwMinSirenAlpha{0};
};

struct SSirenInfo
{
    // Flags
    bool m_b360Flag;
    bool m_bDoLOSCheck;
    bool m_bUseRandomiser;
    bool m_bSirenSilent{false};
    // End of flags
    bool                             m_bOverrideSirens{false};
    unsigned char                    m_ucSirenType;
    unsigned char                    m_ucSirenCount;
    unsigned char                    m_ucCurrentSirenID;
    unsigned char                    m_ucCurrentSirenRandomiser;
    SFixedArray<SSirenBeaconInfo, 8> m_tSirenInfo;
    SharedUtil::SColor               m_tPointLightColour;
};

struct SVehicleFrame
{
    SVehicleFrame(RwFrame* pFrame = NULL, bool bReadOnly = true) : pFrame(pFrame), bReadOnly(bReadOnly) {}

    RwFrame*              pFrame;
    bool                  bReadOnly;
    std::vector<RwFrame*> frameList;            // Frames from root to parent
};

class CVehicle : public virtual CPhysical
{
public:
    virtual ~CVehicle(){};

    virtual bool AddProjectile(eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector* target, CEntity* targetEntity) = 0;

    virtual CPed* GetDriver() = 0;
    virtual CPed* GetPassenger(unsigned char ucSlot) = 0;
    virtual bool  IsBeingDriven() = 0;

    virtual CVehicle* GetNextTrainCarriage() = 0;
    virtual void      SetNextTrainCarriage(CVehicle* pNext) = 0;
    virtual CVehicle* GetPreviousTrainCarriage() = 0;
    virtual void      SetPreviousTrainCarriage(CVehicle* pPrevious) = 0;
    virtual float     GetDistanceToCarriage(CVehicle* pCarriage) = 0;
    virtual void      AttachTrainCarriage(CVehicle* pCarriage) = 0;
    virtual void      DetachTrainCarriage(CVehicle* pCarriage) = 0;
    virtual bool      IsChainEngine() = 0;
    virtual void      SetIsChainEngine(bool bChainEngine = true) = 0;

    virtual bool  IsDerailed() = 0;
    virtual void  SetDerailed(bool bDerailed) = 0;
    virtual bool  IsDerailable() = 0;
    virtual void  SetDerailable(bool bDerailable) = 0;
    virtual float GetTrainSpeed() = 0;
    virtual void  SetTrainSpeed(float fSpeed) = 0;
    virtual bool  GetTrainDirection() = 0;
    virtual void  SetTrainDirection(bool bDirection) = 0;
    virtual BYTE  GetRailTrack() = 0;
    virtual void  SetRailTrack(BYTE ucTrackID) = 0;
    virtual float GetTrainPosition() = 0;
    virtual void  SetTrainPosition(float fPosition, bool bRecalcOnRailDistance = true) = 0;

    virtual void AddVehicleUpgrade(DWORD dwModelID) = 0;
    virtual void RemoveVehicleUpgrade(DWORD dwModelID) = 0;
    virtual bool DoesSupportUpgrade(const SString& strFrameName) = 0;

    virtual CDoor* GetDoor(unsigned char ucDoor) = 0;
    virtual void   OpenDoor(unsigned char ucDoor, float fRatio, bool bMakeNoise = false) = 0;
    virtual void   SetSwingingDoorsAllowed(bool bAllowed) = 0;
    virtual bool   AreSwingingDoorsAllowed() const = 0;
    virtual bool   AreDoorsLocked() = 0;
    virtual void   LockDoors(bool bLocked) = 0;
    virtual bool   AreDoorsUndamageable() = 0;
    virtual void   SetDoorsUndamageable(bool bUndamageable) = 0;

    virtual DWORD GetBaseVehicleType() = 0;

    virtual void  SetBodyDirtLevel(float fDirtLevel) = 0;
    virtual float GetBodyDirtLevel() = 0;

    virtual unsigned char GetCurrentGear() = 0;
    virtual float         GetGasPedal() = 0;
    virtual bool          GetTowBarPos(CVector* pVector, CVehicle* pTrailer) = 0;
    virtual bool          GetTowHitchPos(CVector* pVector) = 0;
    virtual bool          IsUpsideDown() = 0;

    virtual bool IsEngineBroken() = 0;
    virtual void SetEngineBroken(bool bEngineBroken) = 0;

    virtual void          PlaceBikeOnRoadProperly() = 0;
    virtual void          PlaceAutomobileOnRoadProperly() = 0;
    virtual void          SetColor(SharedUtil::SColor color1, SharedUtil::SColor color2, SharedUtil::SColor color3, SharedUtil::SColor color4, int) = 0;
    virtual void          GetColor(SharedUtil::SColor* color1, SharedUtil::SColor* color2, SharedUtil::SColor* color3, SharedUtil::SColor* color4,
                                   bool bFixedForGTA) = 0;
    virtual void          Fix() = 0;
    virtual bool          IsSirenOrAlarmActive() = 0;
    virtual void          SetSirenOrAlarmActive(bool bActive) = 0;
    virtual void          SetAlpha(unsigned char ucAlpha) = 0;
    virtual unsigned char GetAlpha() = 0;

    virtual void BlowUp(CEntity* pCreator, unsigned long ulUnknown) = 0;

    virtual CDamageManager* GetDamageManager() = 0;
    virtual void            FadeOut(bool bFadeOut) = 0;
    virtual bool            IsFadingOut() = 0;

    virtual void      SetTowLink(CVehicle* pVehicle) = 0;
    virtual bool      BreakTowLink() = 0;
    virtual CVehicle* GetTowedVehicle() = 0;
    virtual CVehicle* GetTowedByVehicle() = 0;

    virtual void       SetWinchType(eWinchType winchType) = 0;
    virtual void       PickupEntityWithWinch(CEntity* pEntity) = 0;
    virtual void       ReleasePickedUpEntityWithWinch() = 0;
    virtual void       SetRopeHeightForHeli(float fRopeHeight) = 0;
    virtual CPhysical* QueryPickedUpEntityWithWinch() = 0;

    virtual void SetRemap(int iRemap) = 0;
    virtual int  GetRemapIndex() = 0;
    virtual void SetRemapTexDictionary(int iRemapTextureDictionary) = 0;

    virtual bool           GetCanBeTargettedByHeatSeekingMissiles() = 0;
    virtual bool           IsDamaged() = 0;
    virtual bool           IsDrowning() = 0;
    virtual bool           IsEngineOn() = 0;
    virtual bool           IsHandbrakeOn() = 0;
    virtual bool           IsRCVehicle() = 0;
    virtual bool           GetAlwaysLeaveSkidMarks() = 0;
    virtual bool           GetCanBeDamaged() = 0;
    virtual bool           GetCanShootPetrolTank() = 0;
    virtual bool           GetChangeColourWhenRemapping() = 0;
    virtual bool           GetComedyControls() = 0;
    virtual bool           GetGunSwitchedOff() = 0;
    virtual bool           GetLightsOn() = 0;
    virtual unsigned int   GetOverrideLights() = 0;
    virtual bool           GetTakeLessDamage() = 0;
    virtual bool           GetTyresDontBurst() = 0;
    virtual unsigned short GetAdjustablePropertyValue() = 0;
    virtual float          GetHeliRotorSpeed() const = 0;
    virtual bool           GetVehicleRotorState() const noexcept = 0;
    virtual float          GetPlaneRotorSpeed() = 0;
    virtual unsigned long  GetExplodeTime() = 0;

    virtual char  GetNitroCount() = 0;
    virtual float GetNitroLevel() = 0;

    virtual void SetAlwaysLeaveSkidMarks(bool bAlwaysLeaveSkidMarks) = 0;
    virtual void SetCanBeDamaged(bool bCanBeDamaged) = 0;
    virtual void SetCanBeTargettedByHeatSeekingMissiles(bool bEnabled) = 0;
    virtual void SetCanShootPetrolTank(bool bCanShoot) = 0;
    virtual void SetChangeColourWhenRemapping(bool bChangeColour) = 0;
    virtual void SetComedyControls(bool bComedyControls) = 0;
    virtual void SetEngineOn(bool bEngineOn) = 0;
    virtual void SetGunSwitchedOff(bool bGunsOff) = 0;
    virtual void SetHandbrakeOn(bool bHandbrakeOn) = 0;
    virtual void SetLightsOn(bool bLightsOn) = 0;
    virtual void SetOverrideLights(unsigned int uiOverrideLights) = 0;
    virtual void SetTakeLessDamage(bool bTakeLessDamage) = 0;
    virtual void SetTyresDontBurst(bool bTyresDontBurst) = 0;
    virtual void SetAdjustablePropertyValue(unsigned short usAdjustableProperty) = 0;
    virtual void SetHeliRotorSpeed(float fSpeed) = 0;
    virtual void SetVehicleRotorState(bool state, bool stopRotor, bool isHeli) noexcept = 0;
    virtual void SetPlaneRotorSpeed(float fSpeed) = 0;
    virtual bool SetVehicleWheelRotation(float fRot1, float fRot2, float fRot3, float fRot4) noexcept = 0;
    virtual void SetTaxiLightOn(bool bLightState) = 0;
    virtual void SetExplodeTime(unsigned long ulTime) = 0;
    virtual void SetRadioStatus(bool bStatus) = 0;

    virtual void SetNitroCount(char cNitroCount) = 0;
    virtual void SetNitroLevel(float fNitroLevel) = 0;

    virtual CHandlingEntry* GetHandlingData() = 0;
    virtual void            SetHandlingData(CHandlingEntry* pHandling) = 0;

    virtual CFlyingHandlingEntry* GetFlyingHandlingData() = 0;
    virtual void                  SetFlyingHandlingData(CFlyingHandlingEntry* pHandling) = 0;

    virtual void BurstTyre(BYTE bTyre) = 0;

    virtual BYTE GetBikeWheelStatus(BYTE bWheel) = 0;
    virtual void SetBikeWheelStatus(BYTE bWheel, BYTE bStatus) = 0;

    virtual bool IsWheelCollided(BYTE bWheel) = 0;
    virtual int  GetWheelFrictionState(BYTE bWheel) = 0;

    virtual class CVehicleSAInterface* GetVehicleInterface() = 0;

    virtual float GetHealth() = 0;
    virtual void  SetHealth(float fHealth) = 0;
    virtual void  SetLandingGearDown(bool bLandingGearDown) = 0;
    virtual float GetLandingGearPosition() = 0;
    virtual bool  IsLandingGearDown() = 0;
    virtual void  SetLandingGearPosition(float fPosition) = 0;

    virtual void GetTurretRotation(float* fHorizontal, float* fVertical) = 0;
    virtual void SetTurretRotation(float fHorizontal, float fVertical) = 0;

    virtual bool IsSmokeTrailEnabled() = 0;
    virtual void SetSmokeTrailEnabled(bool bEnabled) = 0;

    virtual void GetGravity(CVector* pvecGravity) const = 0;
    virtual void SetGravity(const CVector* pvecGravity) = 0;

    virtual SColor GetHeadLightColor() = 0;
    virtual void   SetHeadLightColor(const SColor color) = 0;

    virtual bool     SpawnFlyingComponent(const eCarNodes& nodeIndex, const eCarComponentCollisionTypes& collisionType, std::int32_t removalTime = -1) = 0;
    virtual void     SetWheelVisibility(eWheelPosition wheel, bool bVisible) = 0;
    virtual CVector  GetWheelPosition(eWheelPosition wheel) = 0;

    virtual bool IsHeliSearchLightVisible() = 0;
    virtual void SetHeliSearchLightVisible(bool bVisible) = 0;

    virtual CColModel* GetSpecialColModel() = 0;
    virtual bool       UpdateMovingCollision(float fAngle) = 0;

    virtual void RecalculateHandling() = 0;

    virtual void* GetPrivateSuspensionLines() = 0;

    virtual bool DoesVehicleHaveSirens() = 0;

    virtual void                              RemoveVehicleSirens() = 0;
    virtual void                              GiveVehicleSirens(unsigned char ucSirenType, unsigned char ucSirenCount) = 0;
    virtual void                              SetVehicleSirenMinimumAlpha(unsigned char ucSirenID, DWORD dwPercentage) = 0;
    virtual void                              SetVehicleSirenPosition(unsigned char ucSirenID, CVector vecPos) = 0;
    virtual void                              GetVehicleSirenPosition(unsigned char ucSirenID, CVector& vecPos) = 0;
    virtual unsigned char                     GetVehicleSirenCount() = 0;
    virtual unsigned char                     GetVehicleSirenType() = 0;
    virtual DWORD                             GetVehicleSirenMinimumAlpha(unsigned char ucSirenID) = 0;
    virtual SharedUtil::SColor                GetVehicleSirenColour(unsigned char ucSirenCount) = 0;
    virtual void                              SetVehicleSirenColour(unsigned char ucSirenID, SharedUtil::SColor tVehicleSirenColour) = 0;
    virtual void                              SetVehicleCurrentSirenID(unsigned char ucCurrentSirenID) = 0;
    virtual unsigned char                     GetVehicleCurrentSirenID() = 0;
    virtual unsigned char                     GetSirenRandomiser() = 0;
    virtual void                              SetSirenRandomiser(unsigned char ucSirenRandomiser) = 0;
    virtual void                              SetPointLightColour(SharedUtil::SColor tPointLightColour) = 0;
    virtual SharedUtil::SColor                GetPointLightColour() = 0;
    virtual bool                              IsSiren360EffectEnabled() = 0;
    virtual bool                              IsSirenLOSCheckEnabled() = 0;
    virtual bool                              IsSirenRandomiserEnabled() = 0;
    virtual bool                              IsSirenSilentEffectEnabled() = 0;
    virtual void                              SetVehicleFlags(bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent) = 0;
    virtual bool                              SetComponentRotation(const SString& vehicleComponent, const CVector& vecRotation) = 0;
    virtual bool                              GetComponentRotation(const SString& vehicleComponent, CVector& vecRotation) = 0;
    virtual bool                              SetComponentPosition(const SString& vehicleComponent, const CVector& vecPosition) = 0;
    virtual bool                              GetComponentPosition(const SString& vehicleComponent, CVector& vecPositionModelling) = 0;
    virtual bool                              SetComponentScale(const SString& vehicleComponent, const CVector& vecScale) = 0;
    virtual bool                              GetComponentScale(const SString& vehicleComponent, CVector& vecScaleModelling) = 0;
    virtual bool                              IsComponentPresent(const SString& vehicleComponent) = 0;
    virtual bool                              SetComponentMatrix(const SString& vehicleComponent, const CMatrix& matOrientation) = 0;
    virtual bool                              GetComponentMatrix(const SString& vehicleComponent, CMatrix& matOutOrientation) = 0;
    virtual bool                              GetComponentParentToRootMatrix(const SString& vehicleComponent, CMatrix& matOutParentToRoot) = 0;
    virtual bool                              SetComponentVisible(const SString& vehicleComponent, bool bVisible) = 0;
    virtual bool                              GetComponentVisible(const SString& vehicleComponent, bool& bVisible) = 0;
    virtual std::map<SString, SVehicleFrame>& GetComponentMap() = 0;
    virtual void                              UpdateLandingGearPosition() = 0;
    virtual bool                              SetPlateText(const SString& strText) = 0;
    virtual bool                              SetWindowOpenFlagState(unsigned char ucWindow, bool bState) = 0;
    virtual float                             GetWheelScale() = 0;
    virtual void                              SetWheelScale(float fWheelScale) = 0;
    virtual CAEVehicleAudioEntity*            GetVehicleAudioEntity() = 0;

    virtual bool GetDummyPosition(eVehicleDummies dummy, CVector& position) const = 0;
    virtual bool SetDummyPosition(eVehicleDummies dummy, const CVector& position) = 0;

    virtual const CVector* GetDummyPositions() const = 0;

};
