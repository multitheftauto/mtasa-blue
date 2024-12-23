/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientVehicle.h
 *  PURPOSE:     Vehicle entity class header
 *
 *****************************************************************************/

struct CClientVehicleProperties;
class CClientVehicle;

#pragma once

#include <game/CPlane.h>
#include <game/CVehicle.h>
#include <game/CModelInfo.h>

#include "CClientCommon.h"
#include "CClientCamera.h"
#include "CClientModelRequestManager.h"
#include "CClientPed.h"
#include "CClientStreamElement.h"
#include "CClientVehicleManager.h"
#include "CVehicleUpgrades.h"
#include "CClientModel.h"

class CBikeHandlingEntry;
class CBoatHandlingEntry;
class CClientProjectile;

#define INVALID_PASSENGER_SEAT 0xFF
#define DEFAULT_VEHICLE_HEALTH 1000
#define MAX_VEHICLE_HEALTH     10000

enum eClientVehicleType
{
    CLIENTVEHICLE_NONE,
    CLIENTVEHICLE_CAR,
    CLIENTVEHICLE_BOAT,
    CLIENTVEHICLE_TRAIN,
    CLIENTVEHICLE_HELI,
    CLIENTVEHICLE_PLANE,
    CLIENTVEHICLE_BIKE,
    CLIENTVEHICLE_MONSTERTRUCK,
    CLIENTVEHICLE_QUADBIKE,
    CLIENTVEHICLE_BMX,
    CLIENTVEHICLE_TRAILER
};

static constexpr int NUM_VEHICLE_TYPES = 11; 

enum eDelayedSyncVehicleData
{
    DELAYEDSYNC_VEHICLE_KEYSYNC,
    DELAYEDSYNC_VEHICLE_ROTATION,
    DELAYEDSYNC_VEHICLE_MOVESPEED,
    DELAYEDSYNC_VEHICLE_TURNSPEED,
};

enum eWindow
{
    WINDOW_BIKESHIELD = 0,
    WINDOW_REAR,
    WINDOW_RIGHT_FRONT,
    WINDOW_RIGHT_BACK,
    WINDOW_LEFT_FRONT,
    WINDOW_LEFT_BACK,
    WINDOW_WINDSHIELD,
    MAX_WINDOWS
};

enum class VehicleBlowState : unsigned char
{
    INTACT,
    AWAITING_EXPLOSION_SYNC,
    BLOWN,
};

struct VehicleBlowFlags
{
    bool withMovement : 1;
    bool withExplosion : 1;

    constexpr VehicleBlowFlags() : withMovement(true), withExplosion(true) {}
};

namespace EComponentBase
{
    enum EComponentBaseType
    {
        WORLD,
        ROOT,
        PARENT,
    };
}
using EComponentBase::EComponentBaseType;

struct SDelayedSyncVehicleData
{
    unsigned long    ulTime;
    unsigned char    ucType;
    CControllerState State;
    CVector          vecTarget;
    CVector          vecTarget2;
    CVector          vecTarget3;
};

struct SLastSyncedVehData
{
    SLastSyncedVehData()
    {
        // Initialize to a known state
        memset(this, 0, sizeof(*this));
    }

    CVector   vecPosition;
    CVector   vecRotation;
    CVector   vecMoveSpeed;
    CVector   vecTurnSpeed;
    float     fHealth;
    ElementID Trailer;
    bool      bEngineOn;
    bool      bDerailed;
    bool      bIsInWater;
};
struct SVehicleComponentData
{
    SVehicleComponentData()
    {
        m_bPositionChanged = false;
        m_bRotationChanged = false;
        m_bScaleChanged = false;
        m_bVisible = true;
    }
    SString m_strParentName;
    CVector m_vecComponentPosition;                    // Parent relative
    CVector m_vecComponentRotation;                    // Parent relative radians
    CVector m_vecComponentScale;                       // Parent relative
    CVector m_vecOriginalComponentPosition;            // Parent relative
    CVector m_vecOriginalComponentRotation;            // Parent relative radians
    CVector m_vecOriginalComponentScale;               // Parent relative
    bool    m_bPositionChanged;
    bool    m_bRotationChanged;
    bool    m_bScaleChanged;
    bool    m_bVisible;
};

static std::array<std::string, NUM_VEHICLE_TYPES> g_vehicleTypePrefixes;

class CClientVehicle : public CClientStreamElement
{
    DECLARE_CLASS(CClientVehicle, CClientStreamElement)
    friend class CClientCamera;
    friend class CClientPed;
    friend class CClientVehicleManager;
    friend class CClientGame;            // TEMP HACK

protected:            // Use CDeathmatchVehicle constructor for now. Will get removed later when this class is
                      // cleaned up.
    CClientVehicle(CClientManager* pManager, ElementID ID, unsigned short usModel, unsigned char ucVariation, unsigned char ucVariation2);

public:
    ~CClientVehicle();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTVEHICLE; };

    const char*        GetNamePointer() { return m_pModelInfo->GetNameIfVehicle(); };
    eClientVehicleType GetVehicleType() { return m_eVehicleType; };

    void GetPosition(CVector& vecPosition) const;
    void SetPosition(const CVector& vecPosition) { SetPosition(vecPosition, true); }
    void SetPosition(const CVector& vecPosition, bool bResetInterpolation, bool bAllowGroundLoadFreeze = true);

    void UpdatePedPositions(const CVector& vecPosition);

    void GetRotationDegrees(CVector& vecRotation) const;
    void GetRotationRadians(CVector& vecRotation) const;
    void SetRotationDegrees(const CVector& vecRotation) { SetRotationDegrees(vecRotation, true); }
    void SetRotationDegrees(const CVector& vecRotation, bool bResetInterpolation);
    void SetRotationRadians(const CVector& vecRotation) { SetRotationRadians(vecRotation, true); }
    void SetRotationRadians(const CVector& vecRotation, bool bResetInterpolation);

    float GetDistanceFromCentreOfMassToBaseOfModel();

    bool            GetMatrix(CMatrix& Matrix) const;
    bool            SetMatrix(const CMatrix& Matrix);
    virtual CSphere GetWorldBoundingSphere();

    void GetMoveSpeed(CVector& vecMoveSpeed) const;
    void SetMoveSpeed(const CVector& vecMoveSpeed);
    void GetTurnSpeed(CVector& vecTurnSpeed) const;
    void SetTurnSpeed(const CVector& vecTurnSpeed);

    bool IsVisible();
    void SetVisible(bool bVisible);

    void  SetDoorOpenRatio(unsigned char ucDoor, float fRatio, unsigned long ulDelay = 0, bool bForced = false);
    float GetDoorOpenRatio(unsigned char ucDoor);
    void  SetSwingingDoorsAllowed(bool bAllowed);
    bool  AreSwingingDoorsAllowed() const;
    void  AllowDoorRatioSetting(unsigned char ucDoor, bool bAllow, bool bAutoReallowAfterDelay = true);
    bool  AreDoorsLocked();
    void  SetDoorsLocked(bool bLocked);

private:
    void SetDoorOpenRatioInterpolated(unsigned char ucDoor, float fRatio, unsigned long ulDelay);
    void ResetDoorInterpolation();
    void CancelDoorInterpolation(unsigned char ucDoor);
    void ProcessDoorInterpolation();

public:
    bool AreDoorsUndamageable();
    void SetDoorsUndamageable(bool bUndamageable);

    float GetHealth() const;
    void  SetHealth(float health);
    void  Fix();

    void             Blow(VehicleBlowFlags blow);
    bool             IsBlown() const noexcept { return m_blowState != VehicleBlowState::INTACT; }
    void             SetBlowState(VehicleBlowState state) { m_blowState = state; }
    VehicleBlowState GetBlowState() const noexcept { return m_blowState; }

    CVehicleColor& GetColor();
    void           SetColor(const CVehicleColor& color);

    void GetTurretRotation(float& fHorizontal, float& fVertical);
    void SetTurretRotation(float fHorizontal, float fVertical);

    unsigned short GetModel() { return m_usModel; };
    void           SetModelBlocking(unsigned short usModel, unsigned char ucVariant, unsigned char ucVariant2);

    unsigned char GetVariant() { return m_ucVariation; };
    unsigned char GetVariant2() { return m_ucVariation2; };

    void SetVariant(unsigned char ucVariant, unsigned char ucVariant2);

    bool IsEngineBroken();
    void SetEngineBroken(bool bEngineBroken);

    bool IsEngineOn();
    void SetEngineOn(bool bEngineOn);

    bool CanBeDamaged();
    void CalcAndUpdateCanBeDamagedFlag();
    void SetScriptCanBeDamaged(bool bCanBeDamaged);
    void SetSyncUnoccupiedDamage(bool bCanBeDamaged);
    bool GetScriptCanBeDamaged() { return m_bScriptCanBeDamaged; };

    bool GetTyresCanBurst();
    void CalcAndUpdateTyresCanBurstFlag();

    float GetGasPedal();

    bool IsBelowWater() const;
    bool IsDrowning() const;
    bool IsDriven() const;
    bool IsUpsideDown() const;

    bool IsSirenOrAlarmActive();
    void SetSirenOrAlarmActive(bool bActive);

    bool  HasLandingGear() { return m_bHasLandingGear; };
    float GetLandingGearPosition();
    void  SetLandingGearPosition(float fPosition);
    bool  IsLandingGearDown();
    void  SetLandingGearDown(bool bLandingGearDown);

    bool           HasAdjustableProperty() { return m_bHasAdjustableProperty; };
    unsigned short GetAdjustablePropertyValue();
    void           SetAdjustablePropertyValue(unsigned short usValue);
    bool           HasMovingCollision();

private:
    void _SetAdjustablePropertyValue(unsigned short usValue);

public:
    bool          HasDamageModel() { return m_bHasDamageModel; }
    unsigned char GetDoorStatus(unsigned char ucDoor);
    unsigned char GetWheelStatus(unsigned char ucWheel);
    bool          IsWheelCollided(unsigned char ucWheel);
    int           GetWheelFrictionState(unsigned char ucWheel);
    unsigned char GetPanelStatus(unsigned char ucPanel);
    unsigned char GetLightStatus(unsigned char ucLight);
    SString GetComponentNameForWheel(unsigned char ucWheel) const noexcept;

    bool AreLightsOn();

    void SetDoorStatus(unsigned char ucDoor, unsigned char ucStatus, bool spawnFlyingComponent);
    void SetWheelStatus(unsigned char ucWheel, unsigned char ucStatus, bool bSilent = true);
    void SetPanelStatus(unsigned char ucPanel, unsigned char ucStatus);
    void SetLightStatus(unsigned char ucLight, unsigned char ucStatus);
    bool GetWheelMissing(unsigned char ucWheel, const SString& strWheelName = "");

    // TODO: Make the class remember on virtualization
    float GetHeliRotorSpeed();
    float GetPlaneRotorSpeed();

    bool GetRotorSpeed(float&);
    bool SetRotorSpeed(float);
    bool SetWheelsRotation(float fRot1, float fRot2, float fRot3, float fRot4) noexcept;
    void SetHeliRotorSpeed(float fSpeed);
    void SetPlaneRotorSpeed(float fSpeed);
    bool IsHeliSearchLightVisible();
    void SetHeliSearchLightVisible(bool bVisible);

    void ReportMissionAudioEvent(unsigned short usSound);

    bool IsCollisionEnabled() { return m_bIsCollisionEnabled; };
    void SetCollisionEnabled(bool bCollisionEnabled);

    bool GetCanShootPetrolTank();
    void SetCanShootPetrolTank(bool bCanShoot);

    bool GetCanBeTargettedByHeatSeekingMissiles();
    void SetCanBeTargettedByHeatSeekingMissiles(bool bEnabled);

    unsigned char GetAlpha() { return m_ucAlpha; }
    void          SetAlpha(unsigned char ucAlpha);

    CClientPed* GetOccupant(int iSeat = 0) const;
    CClientPed* GetControllingPlayer();
    void        ClearForOccupants();

    void PlaceProperlyOnGround();

    void FuckCarCompletely(bool bKeepWheels);

    void WorldIgnore(bool bWorldIgnore);

    bool IsVirtual() { return m_pVehicle == NULL; };

    void FadeOut(bool bFadeOut);
    bool IsFadingOut();

    bool IsFrozen() { return m_bIsFrozen; };
    void SetFrozen(bool bFrozen);
    void SetScriptFrozen(bool bFrozen) { m_bScriptFrozen = bFrozen; };
    bool IsFrozenWaitingForGroundToLoad() const;
    void SetFrozenWaitingForGroundToLoad(bool bFrozen, bool bDisableAsyncLoading);

    CClientVehicle* GetPreviousTrainCarriage();
    CClientVehicle* GetNextTrainCarriage();
    void            SetPreviousTrainCarriage(CClientVehicle* pPrevious);
    void            SetNextTrainCarriage(CClientVehicle* pNext);
    bool            IsChainEngine() { return m_bChainEngine; };
    void            SetIsChainEngine(bool bChainEngine = true, bool bTemporary = false);
    CClientVehicle* GetChainEngine();
    bool            IsTrainConnectedTo(CClientVehicle* pTrailer);

    bool IsDerailed();
    void SetDerailed(bool bDerailed);
    bool IsDerailable();
    void SetDerailable(bool bDerailable);

    bool GetTrainDirection();
    void SetTrainDirection(bool bDirection);

    float GetTrainSpeed();
    void  SetTrainSpeed(float fSpeed);

    float GetTrainPosition();
    void  SetTrainPosition(float fTrainPosition, bool bRecalcOnRailDistance = true);

    uchar GetTrainTrack();
    void  SetTrainTrack(uchar ucTrack);

    unsigned char     GetOverrideLights() { return m_ucOverrideLights; }
    void              SetOverrideLights(unsigned char ucOverrideLights);
    bool              SetTaxiLightOn(bool bLightOn);
    bool              IsTaxiLightOn() { return m_bTaxiLightOn; }
    CVehicle*         GetGameVehicle() { return m_pVehicle; }
    CEntity*          GetGameEntity() { return m_pVehicle; }
    const CEntity*    GetGameEntity() const { return m_pVehicle; }
    CVehicleUpgrades* GetUpgrades() { return m_pUpgrades; }
    CModelInfo*       GetModelInfo() { return m_pModelInfo; }

    CClientVehicle* GetTowedVehicle();
    CClientVehicle* GetRealTowedVehicle();
    bool            SetTowedVehicle(CClientVehicle* pVehicle, const CVector* vecRotationDegrees = NULL);
    CClientVehicle* GetTowedByVehicle() { return m_pTowedByVehicle; }
    bool            InternalSetTowLink(CClientVehicle* pTrailer);
    bool            IsTowableBy(CClientVehicle* towingVehicle);

    eWinchType     GetWinchType() { return m_eWinchType; }
    bool           SetWinchType(eWinchType winchType);
    bool           PickupEntityWithWinch(CClientEntity* pEntity);
    bool           ReleasePickedUpEntityWithWinch();
    void           SetRopeHeightForHeli(float fRopeHeight);
    CClientEntity* GetPickedUpEntityWithWinch();

    const char* GetRegPlate() { return m_strRegPlate.empty() ? NULL : m_strRegPlate.c_str(); }
    bool        SetRegPlate(const char* szPlate);

    unsigned char GetPaintjob();
    void          SetPaintjob(unsigned char ucPaintjob);

    float GetDirtLevel();
    void  SetDirtLevel(float fDirtLevel);

    char  GetNitroCount();
    float GetNitroLevel();
    void  SetNitroCount(char cCount);
    void  SetNitroLevel(float fLevel);

    bool IsWindowOpen(uchar ucWindow);
    bool SetWindowOpen(uchar ucWindow, bool bOpen);

    bool IsNitroInstalled();

    float GetDistanceFromGround();

    void SetInWater(bool bState) { m_bIsInWater = bState; }
    bool IsInWater();
    bool IsOnGround();
    bool IsOnWater();
    void LockSteering(bool bLock);

    bool IsSmokeTrailEnabled();
    void SetSmokeTrailEnabled(bool bEnabled);

    void ResetInterpolation();

    void Interpolate();
    void UpdateKeysync();

    void GetInitialDoorStates(SFixedArray<unsigned char, MAX_DOORS>& ucOutDoorStates);

    // Time dependent interpolation
    void GetTargetPosition(CVector& vecPosition) { vecPosition = m_interp.pos.vecTarget; }
    void SetTargetPosition(const CVector& vecPosition, unsigned long ulDelay, bool bValidVelocityZ = false, float fVelocityZ = 0.f);
    void RemoveTargetPosition();
    bool HasTargetPosition() { return (m_interp.pos.ulFinishTime != 0); }

    void GetTargetRotation(CVector& vecRotation) { vecRotation = m_interp.rot.vecTarget; }
    void SetTargetRotation(const CVector& vecRotation, unsigned long ulDelay);
    void RemoveTargetRotation();
    bool HasTargetRotation() { return (m_interp.rot.ulFinishTime != 0); }

    void UpdateTargetPosition();
    void UpdateTargetRotation();
    void UpdateUnderFloorFix(const CVector& vecTargetPosition, bool bValidVelocityZ, float fVelocityZ);

    unsigned long GetIllegalTowBreakTime() { return m_ulIllegalTowBreakTime; }
    void          SetIllegalTowBreakTime(unsigned long ulTime) { m_ulIllegalTowBreakTime = ulTime; }

    void GetGravity(CVector& vecGravity) const { vecGravity = m_vecGravity; }
    void SetGravity(const CVector& vecGravity);

    SColor GetHeadLightColor();
    void   SetHeadLightColor(const SColor color);

    int GetCurrentGear();

    bool IsEnterable(bool localEntity = false);
    bool HasRadio();
    bool HasPoliceRadio();

    void ReCreate();

    void ModelRequestCallback(CModelInfo* pModelInfo);

    // Warning: Don't use this to create a vehicle if CClientVehicleManager::IsVehicleLimitReached
    //          returns true. Also this messes with streaming so don't Destroy something unless
    //          you're going to recreate it very quickly again. CClientVehicleManager::IsVehicleLimitReached
    //          returns true often when the player is in an area full of vehicles so don't fatal
    //          error or something if it does return true.
    void Create();
    void Destroy();

    void                                    AddProjectile(CClientProjectile* pProjectile) { m_Projectiles.push_back(pProjectile); }
    void                                    RemoveProjectile(CClientProjectile* pProjectile) { m_Projectiles.remove(pProjectile); }
    std::list<CClientProjectile*>::iterator ProjectilesBegin() { return m_Projectiles.begin(); }
    std::list<CClientProjectile*>::iterator ProjectilesEnd() { return m_Projectiles.end(); }

    void RemoveAllProjectiles();

    static void SetPedOccupiedVehicle(CClientPed* pClientPed, CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor);
    static void SetPedOccupyingVehicle(CClientPed* pClientPed, CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor);
    static void ValidatePedAndVehiclePair(CClientPed* pClientPed, CClientVehicle* pVehicle);
    static void UnpairPedAndVehicle(CClientPed* pClientPed, CClientVehicle* pVehicle);
    static void UnpairPedAndVehicle(CClientPed* pClientPed);

    void                  ApplyHandling();
    CHandlingEntry*       GetHandlingData();
    const CHandlingEntry* GetOriginalHandlingData() { return m_pOriginalHandlingEntry; };

    CFlyingHandlingEntry*       GetFlyingHandlingData();
    const CFlyingHandlingEntry* GetOriginalFlyingHandlingData() { return m_pOriginalFlyingHandlingEntry; };

    CBoatHandlingEntry*       GetBoatHandlingData();
    const CBoatHandlingEntry* GetOriginalBoatHandlingData() { return m_pOriginalBoatHandlingEntry; };

    CBikeHandlingEntry*       GetBikeHandlingData();
    const CBikeHandlingEntry* GetOriginalBikeHandlingData() { return m_pOriginalBikeHandlingEntry; };

    uint GetTimeSinceLastPush() { return (uint)(CTickCount::Now() - m_LastPushedTime).ToLongLong(); }
    void ResetLastPushTime() { m_LastPushedTime = CTickCount::Now(); }

    bool DoesVehicleHaveSirens() { return m_tSirenBeaconInfo.m_bOverrideSirens; }

    bool GiveVehicleSirens(unsigned char ucSirenType, unsigned char ucSirenCount);
    void SetVehicleSirenPosition(unsigned char ucSirenID, CVector vecPos);
    void SetVehicleSirenMinimumAlpha(unsigned char ucSirenID, DWORD dwPercentage);
    void SetVehicleSirenColour(unsigned char ucSirenID, SColor tVehicleSirenColour);
    void SetVehicleFlags(bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent);
    void RemoveVehicleSirens();

    bool ResetComponentPosition(const SString& vehicleComponent);
    bool SetComponentPosition(const SString& vehicleComponent, CVector vecPosition, EComponentBaseType base = EComponentBase::PARENT);
    bool GetComponentPosition(const SString& vehicleComponent, CVector& vecPosition, EComponentBaseType base = EComponentBase::PARENT);

    bool ResetComponentRotation(const SString& vehicleComponent);
    bool SetComponentRotation(const SString& vehicleComponent, CVector vecRotation, EComponentBaseType base = EComponentBase::PARENT);
    bool GetComponentRotation(const SString& vehicleComponent, CVector& vecRotation, EComponentBaseType base = EComponentBase::PARENT);

    bool ResetComponentScale(const SString& vehicleComponent);
    bool SetComponentScale(const SString& vehicleComponent, CVector vecScale, EComponentBaseType base = EComponentBase::PARENT);
    bool GetComponentScale(const SString& vehicleComponent, CVector& vecScale, EComponentBaseType base = EComponentBase::PARENT);

    bool                                               SetComponentVisible(const SString& vehicleComponent, bool bVisible);
    bool                                               GetComponentVisible(const SString& vehicleComponent, bool& bVisible);
    std::map<SString, SVehicleComponentData>::iterator ComponentsBegin() { return m_ComponentData.begin(); }
    std::map<SString, SVehicleComponentData>::iterator ComponentsEnd() { return m_ComponentData.end(); }

    bool DoesSupportUpgrade(const SString& strFrameName);

    bool AreHeliBladeCollisionsEnabled() { return m_bEnableHeliBladeCollisions; }

    void SetHeliBladeCollisionsEnabled(bool bEnable) { m_bEnableHeliBladeCollisions = bEnable; }

    float GetWheelScale();
    void  SetWheelScale(float fWheelScale);
    void  ResetWheelScale();

    bool OnVehicleFallThroughMap();

    bool GetDummyPosition(eVehicleDummies dummy, CVector& position) const;
    bool SetDummyPosition(eVehicleDummies dummy, const CVector& position);
    bool ResetDummyPositions();

    bool SpawnFlyingComponent(const eCarNodes& nodeID, const eCarComponentCollisionTypes& collisionType, std::int32_t removalTime);

    CVector GetEntryPoint(std::uint32_t entryPointIndex);

protected:
    void ConvertComponentRotationBase(const SString& vehicleComponent, CVector& vecInOutRotation, EComponentBaseType inputBase, EComponentBaseType outputBase);
    void ConvertComponentPositionBase(const SString& vehicleComponent, CVector& vecInOutPosition, EComponentBaseType inputBase, EComponentBaseType outputBase);
    void ConvertComponentScaleBase(const SString& vehicleComponent, CVector& vecScale, EComponentBaseType inputBase, EComponentBaseType outputBase);
    void ConvertComponentMatrixBase(const SString& vehicleComponent, CMatrix& matInOutOrientation, EComponentBaseType inputBase, EComponentBaseType outputBase);
    void GetComponentParentToRootMatrix(const SString& vehicleComponent, CMatrix& matOutParentToRoot);

    void StreamIn(bool bInstantly);
    void StreamOut();

    void NotifyCreate();
    void NotifyDestroy();

    bool DoCheckHasLandingGear();
    void HandleWaitingForGroundToLoad();
    bool DoesNeedToWaitForGroundToLoad();

    void StreamedInPulse();

    class CClientObjectManager* m_pObjectManager;
    CClientVehicleManager*      m_pVehicleManager;
    CClientModelRequestManager* m_pModelRequester;
    unsigned short              m_usModel;
    bool                        m_bHasLandingGear;
    eClientVehicleType          m_eVehicleType;
    unsigned char               m_ucMaxPassengers;
    bool                        m_bIsVirtualized;
    CVehicle*                   m_pVehicle;
    CClientPedPtr               m_pDriver;
    SFixedArray<CClientPed*, 8> m_pPassengers;
    CClientPedPtr               m_pOccupyingDriver;
    SFixedArray<CClientPed*, 8> m_pOccupyingPassengers;
    RpClump*                    m_pClump;
    short                       m_usRemoveTimer;

    CClientVehiclePtr            m_pPreviousLink;
    CClientVehiclePtr            m_pNextLink;
    CMatrix                      m_Matrix;
    CVector                      m_vecMoveSpeed;
    CVector                      m_vecTurnSpeed;
    float                        m_fHealth;
    float                        m_fTurretHorizontal;
    float                        m_fTurretVertical;
    float                        m_fGasPedal;
    bool                         m_bVisible;
    bool                         m_bIsCollisionEnabled;
    bool                         m_bEngineOn;
    bool                         m_bEngineBroken;
    bool                         m_bSireneOrAlarmActive;
    bool                         m_bLandingGearDown;
    bool                         m_bHasAdjustableProperty;
    unsigned short               m_usAdjustablePropertyValue;
    std::map<eDoors, CTickCount> m_AutoReallowDoorRatioMap;
    SFixedArray<bool, 6>         m_bAllowDoorRatioSetting;
    SFixedArray<float, 6>        m_fDoorOpenRatio;
    struct
    {
        SFixedArray<float, 6>         fStart;
        SFixedArray<float, 6>         fTarget;
        SFixedArray<unsigned long, 6> ulStartTime;
        SFixedArray<unsigned long, 6> ulTargetTime;
    } m_doorInterp;
    bool                                   m_bSwingingDoorsAllowed;
    bool                                   m_bDoorsLocked;
    bool                                   m_bDoorsUndamageable;
    bool                                   m_bCanShootPetrolTank;
    bool                                   m_bCanBeTargettedByHeatSeekingMissiles;
    bool                                   m_bCanBeDamaged;
    bool                                   m_bScriptCanBeDamaged;
    bool                                   m_bSyncUnoccupiedDamage;
    bool                                   m_bTyresCanBurst;
    SFixedArray<unsigned char, MAX_DOORS>  m_ucDoorStates;
    SFixedArray<unsigned char, MAX_WHEELS> m_ucWheelStates;
    SFixedArray<unsigned char, MAX_PANELS> m_ucPanelStates;
    SFixedArray<unsigned char, MAX_LIGHTS> m_ucLightStates;
    bool                                   m_bJustBlewUp;
    eEntityStatus                          m_NormalStatus;
    bool                                   m_bColorSaved;
    CVehicleColor                          m_Color;
    bool                                   m_bIsFrozen;
    bool                                   m_bScriptFrozen;
    bool                                   m_bFrozenWaitingForGroundToLoad;
    float                                  m_fGroundCheckTolerance;
    float                                  m_fObjectsAroundTolerance;
    CVector                                m_vecWaitingForGroundSavedMoveSpeed;
    CVector                                m_vecWaitingForGroundSavedTurnSpeed;
    CMatrix                                m_matFrozen;
    CVehicleUpgrades*                      m_pUpgrades;
    unsigned char                          m_ucOverrideLights;
    CClientVehiclePtr                      m_pTowedVehicle;
    CClientVehiclePtr                      m_pTowedByVehicle;
    eWinchType                             m_eWinchType;
    CClientEntityPtr                       m_pPickedUpWinchEntity;
    SString                                m_strRegPlate;
    unsigned char                          m_ucPaintjob;
    float                                  m_fDirtLevel;
    bool                                   m_bSmokeTrail;
    unsigned char                          m_ucAlpha;
    bool                                   m_bAlphaChanged;
    double                                 m_dLastRotationTime;
    bool                                   m_blowAfterStreamIn;
    bool                                   m_bIsOnGround;
    bool                                   m_bHeliSearchLightVisible;
    float                                  m_fHeliRotorSpeed;
    float                                  m_fPlaneRotorSpeed;
    const CHandlingEntry*                  m_pOriginalHandlingEntry = nullptr;
    std::unique_ptr<CHandlingEntry>        m_HandlingEntry = nullptr;
    const CFlyingHandlingEntry*            m_pOriginalFlyingHandlingEntry = nullptr;
    std::unique_ptr<CFlyingHandlingEntry>  m_FlyingHandlingEntry = nullptr;
    const CBoatHandlingEntry*              m_pOriginalBoatHandlingEntry = nullptr;
    std::unique_ptr<CBoatHandlingEntry>    m_BoatHandlingEntry = nullptr;
    const CBikeHandlingEntry*              m_pOriginalBikeHandlingEntry = nullptr;
    std::unique_ptr<CBikeHandlingEntry>    m_BikeHandlingEntry = nullptr;
    float                                  m_fNitroLevel;
    char                                   m_cNitroCount;
    float                                  m_fWheelScale;

    bool  m_bChainEngine;
    bool  m_bIsDerailed;
    bool  m_bIsDerailable;
    bool  m_bTrainDirection;
    float m_fTrainSpeed;
    float m_fTrainPosition;
    uchar m_ucTrackID;
    bool  m_bJustStreamedIn;
    bool  m_bWheelScaleChanged;

    // Time dependent error compensation interpolation
    struct
    {
        struct
        {
#ifdef MTA_DEBUG
            CVector vecStart;
#endif
            CVector       vecTarget;
            CVector       vecError;
            float         fLastAlpha;
            unsigned long ulStartTime;
            unsigned long ulFinishTime;
        } pos;

        struct
        {
#ifdef MTA_DEBUG
            CVector vecStart;
#endif
            CVector       vecTarget;
            CVector       vecError;
            float         fLastAlpha;
            unsigned long ulStartTime;
            unsigned long ulFinishTime;
        } rot;
    } m_interp;

    unsigned long m_ulIllegalTowBreakTime;

    bool m_bHasDamageModel;

    VehicleBlowState m_blowState = VehicleBlowState::INTACT;

    bool                          m_bTaxiLightOn;
    std::list<CClientProjectile*> m_Projectiles;

    bool m_bIsInWater;

    bool m_bNitroActivated;

    CVector m_vecGravity;
    SColor  m_HeadLightColor;

    bool m_bHasCustomHandling;

    unsigned char m_ucVariation;
    unsigned char m_ucVariation2;

    CTickCount m_LastPushedTime;
    uint       m_uiForceLocalZCounter;

    bool                           m_bEnableHeliBladeCollisions;
    CMatrix                        m_matCreate;
    unsigned char                  m_ucFellThroughMapCount;
    SFixedArray<bool, MAX_WINDOWS> m_bWindowOpen;
    std::shared_ptr<CClientModel>  m_clientModel;

public:
#ifdef MTA_DEBUG
    CClientPlayer* m_pLastSyncer;
    unsigned long  m_ulLastSyncTime;
    const char*    m_szLastSyncType;
#endif
    SLastSyncedVehData*                      m_LastSyncedData;
    SSirenInfo                               m_tSirenBeaconInfo;
    std::map<SString, SVehicleComponentData> m_ComponentData;
    bool                                     m_bAsyncLoadingDisabled;

    std::array<CVector, VEHICLE_DUMMY_COUNT> m_dummyPositions;
    bool                                     m_copyDummyPositions = true;
};
