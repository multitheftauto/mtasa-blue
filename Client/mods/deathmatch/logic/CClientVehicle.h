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

enum class VehicleBlowState : std::uint8_t
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
    std::uint32_t    ulTime;
    std::uint8_t    ucType;
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
    CClientVehicle(CClientManager* pManager, ElementID ID, std::uint16_t usModel, std::uint8_t ucVariation, std::uint8_t ucVariation2);

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

    void  SetDoorOpenRatio(std::uint8_t ucDoor, float fRatio, std::uint32_t ulDelay = 0, bool bForced = false);
    float GetDoorOpenRatio(std::uint8_t ucDoor);
    void  SetSwingingDoorsAllowed(bool bAllowed);
    bool  AreSwingingDoorsAllowed() const;
    void  AllowDoorRatioSetting(std::uint8_t ucDoor, bool bAllow, bool bAutoReallowAfterDelay = true);
    bool  AreDoorsLocked();
    void  SetDoorsLocked(bool bLocked);

private:
    void SetDoorOpenRatioInterpolated(std::uint8_t ucDoor, float fRatio, std::uint32_t ulDelay);
    void ResetDoorInterpolation();
    void CancelDoorInterpolation(std::uint8_t ucDoor);
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

    std::uint16_t GetModel() { return m_usModel; };
    void           SetModelBlocking(std::uint16_t usModel, std::uint8_t ucVariant, std::uint8_t ucVariant2);

    std::uint8_t GetVariant() { return m_ucVariation; };
    std::uint8_t GetVariant2() { return m_ucVariation2; };

    void SetVariant(std::uint8_t ucVariant, std::uint8_t ucVariant2);

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
    std::uint16_t GetAdjustablePropertyValue();
    void           SetAdjustablePropertyValue(std::uint16_t usValue);
    bool           HasMovingCollision();

private:
    void _SetAdjustablePropertyValue(std::uint16_t usValue);

public:
    bool          HasDamageModel() { return m_bHasDamageModel; }
    std::uint8_t GetDoorStatus(std::uint8_t ucDoor);
    std::uint8_t GetWheelStatus(std::uint8_t ucWheel);
    bool          IsWheelCollided(std::uint8_t ucWheel);
    int           GetWheelFrictionState(std::uint8_t ucWheel);
    std::uint8_t GetPanelStatus(std::uint8_t ucPanel);
    std::uint8_t GetLightStatus(std::uint8_t ucLight);
    SString GetComponentNameForWheel(std::uint8_t ucWheel) const noexcept;

    bool AreLightsOn();

    void SetDoorStatus(std::uint8_t ucDoor, std::uint8_t ucStatus, bool spawnFlyingComponent);
    void SetWheelStatus(std::uint8_t ucWheel, std::uint8_t ucStatus, bool bSilent = true);
    void SetPanelStatus(std::uint8_t ucPanel, std::uint8_t ucStatus);
    void SetLightStatus(std::uint8_t ucLight, std::uint8_t ucStatus);
    bool GetWheelMissing(std::uint8_t ucWheel, const SString& strWheelName = "");

    // TODO: Make the class remember on virtualization
    float GetHeliRotorSpeed();
    float GetPlaneRotorSpeed();

    bool GetRotorSpeed(float&);
    bool SetRotorSpeed(float);

    void SetHeliRotorSpeed(float fSpeed);
    void SetPlaneRotorSpeed(float fSpeed);
    bool IsHeliSearchLightVisible();
    void SetHeliSearchLightVisible(bool bVisible);

    void ReportMissionAudioEvent(std::uint16_t usSound);

    bool IsCollisionEnabled() { return m_bIsCollisionEnabled; };
    void SetCollisionEnabled(bool bCollisionEnabled);

    bool GetCanShootPetrolTank();
    void SetCanShootPetrolTank(bool bCanShoot);

    bool GetCanBeTargettedByHeatSeekingMissiles();
    void SetCanBeTargettedByHeatSeekingMissiles(bool bEnabled);

    std::uint8_t GetAlpha() { return m_ucAlpha; }
    void          SetAlpha(std::uint8_t ucAlpha);

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

    std::uint8_t     GetOverrideLights() { return m_ucOverrideLights; }
    void              SetOverrideLights(std::uint8_t ucOverrideLights);
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

    std::uint8_t GetPaintjob();
    void          SetPaintjob(std::uint8_t ucPaintjob);

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

    void GetInitialDoorStates(SFixedArray<std::uint8_t, MAX_DOORS>& ucOutDoorStates);

    // Time dependent interpolation
    void GetTargetPosition(CVector& vecPosition) { vecPosition = m_interp.pos.vecTarget; }
    void SetTargetPosition(const CVector& vecPosition, std::uint32_t ulDelay, bool bValidVelocityZ = false, float fVelocityZ = 0.f);
    void RemoveTargetPosition();
    bool HasTargetPosition() { return (m_interp.pos.ulFinishTime != 0); }

    void GetTargetRotation(CVector& vecRotation) { vecRotation = m_interp.rot.vecTarget; }
    void SetTargetRotation(const CVector& vecRotation, std::uint32_t ulDelay);
    void RemoveTargetRotation();
    bool HasTargetRotation() { return (m_interp.rot.ulFinishTime != 0); }

    void UpdateTargetPosition();
    void UpdateTargetRotation();
    void UpdateUnderFloorFix(const CVector& vecTargetPosition, bool bValidVelocityZ, float fVelocityZ);

    std::uint32_t GetIllegalTowBreakTime() { return m_ulIllegalTowBreakTime; }
    void          SetIllegalTowBreakTime(std::uint32_t ulTime) { m_ulIllegalTowBreakTime = ulTime; }

    void GetGravity(CVector& vecGravity) const { vecGravity = m_vecGravity; }
    void SetGravity(const CVector& vecGravity);

    SColor GetHeadLightColor();
    void   SetHeadLightColor(const SColor color);

    int GetCurrentGear();

    bool IsEnterable();
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

    static void SetPedOccupiedVehicle(CClientPed* pClientPed, CClientVehicle* pVehicle, std::uint32_t uiSeat, std::uint8_t ucDoor);
    static void SetPedOccupyingVehicle(CClientPed* pClientPed, CClientVehicle* pVehicle, std::uint32_t uiSeat, std::uint8_t ucDoor);
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

    bool GiveVehicleSirens(std::uint8_t ucSirenType, std::uint8_t ucSirenCount);
    void SetVehicleSirenPosition(std::uint8_t ucSirenID, CVector vecPos);
    void SetVehicleSirenMinimumAlpha(std::uint8_t ucSirenID, DWORD dwPercentage);
    void SetVehicleSirenColour(std::uint8_t ucSirenID, SColor tVehicleSirenColour);
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
    std::uint16_t              m_usModel;
    bool                        m_bHasLandingGear;
    eClientVehicleType          m_eVehicleType;
    std::uint8_t               m_ucMaxPassengers;
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
    std::uint16_t               m_usAdjustablePropertyValue;
    std::map<eDoors, CTickCount> m_AutoReallowDoorRatioMap;
    SFixedArray<bool, 6>         m_bAllowDoorRatioSetting;
    SFixedArray<float, 6>        m_fDoorOpenRatio;
    struct
    {
        SFixedArray<float, 6>         fStart;
        SFixedArray<float, 6>         fTarget;
        SFixedArray<std::uint32_t, 6> ulStartTime;
        SFixedArray<std::uint32_t, 6> ulTargetTime;
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
    SFixedArray<std::uint8_t, MAX_DOORS>  m_ucDoorStates;
    SFixedArray<std::uint8_t, MAX_WHEELS> m_ucWheelStates;
    SFixedArray<std::uint8_t, MAX_PANELS> m_ucPanelStates;
    SFixedArray<std::uint8_t, MAX_LIGHTS> m_ucLightStates;
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
    std::uint8_t                          m_ucOverrideLights;
    CClientVehiclePtr                      m_pTowedVehicle;
    CClientVehiclePtr                      m_pTowedByVehicle;
    eWinchType                             m_eWinchType;
    CClientEntityPtr                       m_pPickedUpWinchEntity;
    SString                                m_strRegPlate;
    std::uint8_t                          m_ucPaintjob;
    float                                  m_fDirtLevel;
    bool                                   m_bSmokeTrail;
    std::uint8_t                          m_ucAlpha;
    bool                                   m_bAlphaChanged;
    double                                 m_dLastRotationTime;
    bool                                   m_blowAfterStreamIn;
    bool                                   m_bIsOnGround;
    bool                                   m_bHeliSearchLightVisible;
    float                                  m_fHeliRotorSpeed;
    float                                  m_fPlaneRotorSpeed;
    const CHandlingEntry*                  m_pOriginalHandlingEntry = nullptr;
    CHandlingEntry*                        m_pHandlingEntry = nullptr;
    const CFlyingHandlingEntry*            m_pOriginalFlyingHandlingEntry = nullptr;
    CFlyingHandlingEntry*                  m_pFlyingHandlingEntry = nullptr;
    const CBoatHandlingEntry*              m_pOriginalBoatHandlingEntry = nullptr;
    CBoatHandlingEntry*                    m_pBoatHandlingEntry = nullptr;
    const CBikeHandlingEntry*              m_pOriginalBikeHandlingEntry = nullptr;
    CBikeHandlingEntry*                    m_pBikeHandlingEntry = nullptr;
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
            std::uint32_t ulStartTime;
            std::uint32_t ulFinishTime;
        } pos;

        struct
        {
#ifdef MTA_DEBUG
            CVector vecStart;
#endif
            CVector       vecTarget;
            CVector       vecError;
            float         fLastAlpha;
            std::uint32_t ulStartTime;
            std::uint32_t ulFinishTime;
        } rot;
    } m_interp;

    std::uint32_t m_ulIllegalTowBreakTime;

    bool m_bHasDamageModel;

    VehicleBlowState m_blowState = VehicleBlowState::INTACT;

    bool                          m_bTaxiLightOn;
    std::list<CClientProjectile*> m_Projectiles;

    bool m_bIsInWater;

    bool m_bNitroActivated;

    CVector m_vecGravity;
    SColor  m_HeadLightColor;

    bool m_bHasCustomHandling;

    std::uint8_t m_ucVariation;
    std::uint8_t m_ucVariation2;

    CTickCount m_LastPushedTime;
    uint       m_uiForceLocalZCounter;

    bool                           m_bEnableHeliBladeCollisions;
    CMatrix                        m_matCreate;
    std::uint8_t                  m_ucFellThroughMapCount;
    SFixedArray<bool, MAX_WINDOWS> m_bWindowOpen;
    std::shared_ptr<CClientModel>  m_clientModel;

public:
#ifdef MTA_DEBUG
    CClientPlayer* m_pLastSyncer;
    std::uint32_t  m_ulLastSyncTime;
    const char*    m_szLastSyncType;
#endif
    SLastSyncedVehData*                      m_LastSyncedData;
    SSirenInfo                               m_tSirenBeaconInfo;
    std::map<SString, SVehicleComponentData> m_ComponentData;
    bool                                     m_bAsyncLoadingDisabled;

    std::array<CVector, VEHICLE_DUMMY_COUNT> m_dummyPositions;
    bool                                     m_copyDummyPositions = true;
};
