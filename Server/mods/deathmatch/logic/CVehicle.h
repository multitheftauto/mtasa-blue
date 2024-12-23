/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicle.h
 *  PURPOSE:     Vehicle entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CVehicle;

#pragma once

#include "CCommon.h"
#include "packets/CPacket.h"
#include "CElement.h"
#include "CEvents.h"
#include "CVehicleUpgrades.h"
#include "CHandlingEntry.h"

#define MAX_VEHICLE_SEATS 9
#define DEFAULT_VEHICLE_HEALTH 1000
#define MAX_VEHICLE_HEALTH 10000

enum eWheelStatus
{
    DT_WHEEL_INTACT = 0,
    DT_WHEEL_BURST,
    DT_WHEEL_MISSING,
    DT_WHEEL_INTACT_COLLISIONLESS,
};

enum eDoorStatus
{
    DT_DOOR_INTACT = 0,
    DT_DOOR_SWINGING_FREE,
    DT_DOOR_BASHED,
    DT_DOOR_BASHED_AND_SWINGING_FREE,
    DT_DOOR_MISSING
};

enum eComponentStatus
{
    DT_PANEL_INTACT = 0,
    DT_PANEL_BASHED,
    DT_PANEL_BASHED2,
    DT_PANEL_MISSING
};

enum eLightStatus
{
    DT_LIGHT_OK = 0,
    DT_LIGHT_SMASHED
};

enum eDoors
{
    BONNET = 0,
    BOOT,
    FRONT_LEFT_DOOR,
    FRONT_RIGHT_DOOR,
    REAR_LEFT_DOOR,
    REAR_RIGHT_DOOR,
    MAX_DOORS
};

enum eWheelPosition
{
    FRONT_LEFT_WHEEL = 0,
    REAR_LEFT_WHEEL,
    FRONT_RIGHT_WHEEL,
    REAR_RIGHT_WHEEL,
    MAX_WHEELS
};

enum ePanels
{
    FRONT_LEFT_PANEL = 0,
    FRONT_RIGHT_PANEL,
    REAR_LEFT_PANEL,
    REAR_RIGHT_PANEL,
    WINDSCREEN_PANEL,
    FRONT_BUMPER,
    REAR_BUMPER,
    MAX_PANELS
};

enum eLights
{
    LEFT_HEADLIGHT = 0,
    RIGHT_HEADLIGHT,
    LEFT_TAIL_LIGHT,
    RIGHT_TAIL_LIGHT,
    MAX_LIGHTS
};

enum eVehicleType
{
    VEHICLE_NONE = 0,
    VEHICLE_CAR,
    VEHICLE_BOAT,
    VEHICLE_TRAIN,
    VEHICLE_HELI,
    VEHICLE_PLANE,
    VEHICLE_BIKE,
    VEHICLE_MONSTERTRUCK,
    VEHICLE_QUADBIKE,
    VEHICLE_BMX,
    VEHICLE_TRAILER
};

enum class eCarNodes
{
    NONE = 0,
    CHASSIS,
    WHEEL_RF,
    WHEEL_RM,
    WHEEL_RB,
    WHEEL_LF,
    WHEEL_LM,
    WHEEL_LB,
    DOOR_RF,
    DOOR_RR,
    DOOR_LF,
    DOOR_LR,
    BUMP_FRONT,
    BUMP_REAR,
    WING_RF,
    WING_LF,
    BONNET,
    BOOT,
    WINDSCREEN,
    EXHAUST,
    MISC_A,
    MISC_B,
    MISC_C,
    MISC_D,
    MISC_E,

    NUM_NODES
};

enum class eCarComponentCollisionTypes
{
    COL_NODE_BUMPER = 0,
    COL_NODE_WHEEL,
    COL_NODE_DOOR,
    COL_NODE_BONNET,
    COL_NODE_BOOT,
    COL_NODE_PANEL,

    COL_NODES_NUM
};

#define SIREN_TYPE_FIRST 1
#define SIREN_TYPE_LAST 6
#define SIREN_ID_MAX 7
#define SIREN_COUNT_MAX 8

struct SSirenBeaconInfo
{
    CVector m_vecSirenPositions;
    SColor  m_RGBBeaconColour;
    DWORD   m_dwMinSirenAlpha;
};
struct SSirenInfo
{
    // Flags
    bool m_b360Flag;
    bool m_bDoLOSCheck;
    bool m_bUseRandomiser;
    bool m_bSirenSilent;
    // End of flags
    bool                             m_bOverrideSirens;
    unsigned char                    m_ucSirenType;
    unsigned char                    m_ucSirenCount;
    SFixedArray<SSirenBeaconInfo, 8> m_tSirenInfo;
};

enum class VehicleBlowState : unsigned char
{
    INTACT,
    AWAITING_EXPLOSION_SYNC,
    BLOWN,
};

class CTrainTrack;

class CVehicle final : public CElement
{
    friend class CPlayer;

public:
    ZERO_ON_NEW
    CVehicle(class CVehicleManager* pVehicleManager, CElement* pParent, unsigned short usModel, unsigned char ucVariant, unsigned char ucVariant2);
    ~CVehicle();
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    bool IsEntity() { return true; }

    void Unlink();

    void DoPulse();

    unsigned short GetModel() { return m_usModel; };
    void           SetModel(unsigned short usModel);
    bool           HasValidModel();

    unsigned char GetVariant() { return m_ucVariant; };
    unsigned char GetVariant2() { return m_ucVariant2; };

    void SetVariants(unsigned char ucVariant, unsigned char ucVariant2);

    eVehicleType GetVehicleType() { return m_eVehicleType; };

    CVehicleColor& GetColor() { return m_Color; };
    void           SetColor(const CVehicleColor& Color) { m_Color = Color; };

    bool IsFrozen() { return m_bIsFrozen; };
    void SetFrozen(bool bIsFrozen) { m_bIsFrozen = bIsFrozen; };

    const CVector& GetPosition();
    void           SetPosition(const CVector& vecPosition);
    void           GetRotation(CVector& vecRotation);
    void           GetRotationDegrees(CVector& vecRotation);
    void           SetRotationDegrees(const CVector& vecRotation);
    void           GetMatrix(CMatrix& matrix);
    void           SetMatrix(const CMatrix& matrix);

    const CVector& GetVelocity() { return m_vecVelocity; };
    void           SetVelocity(const CVector& vecVelocity) { m_vecVelocity = vecVelocity; };
    const CVector& GetVelocityMeters() { return m_vecVelocityMeters; };
    void           SetVelocityMeters(const CVector& vecVelocityMeters) { m_vecVelocityMeters = vecVelocityMeters; };
    const CVector& GetTurnSpeed() { return m_vecTurnSpeed; };
    void           SetTurnSpeed(const CVector& vecTurnSpeed) { m_vecTurnSpeed = vecTurnSpeed; };

    float GetHealth() { return m_fHealth; };
    void  SetHealth(float fHealth);
    float GetLastSyncedHealth() { return m_fLastSyncedHealthHealth; };
    void  SetLastSyncedHealth(float fHealth) { m_fLastSyncedHealthHealth = fHealth; };

    CVehicleColor& RandomizeColor();

    float GetDoorOpenRatio(unsigned char ucDoor) const;
    void  SetDoorOpenRatio(unsigned char ucDoor, float fRatio);
    bool  IsLocked() { return m_bLocked; };
    void  SetLocked(bool bLocked) { m_bLocked = bLocked; };

    bool AreDoorsUndamageable() { return m_bDoorsUndamageable; };
    void SetDoorsUndamageable(bool bUndamageable) { m_bDoorsUndamageable = bUndamageable; };

    float GetTurretPositionX() { return m_fTurretPositionX; };
    float GetTurretPositionY() { return m_fTurretPositionY; };
    void  GetTurretPosition(float& fPositionX, float& fPositionY);
    void  SetTurretPosition(float fPositionX, float fPositionY);

    bool IsSirenActive() { return m_bSirenActive; };
    void SetSirenActive(bool bSirenActive) { m_bSirenActive = bSirenActive; };
    void SetTaxiLightOn(bool bTaxiLightState) { m_bTaxiLightState = bTaxiLightState; };
    bool IsTaxiLightOn() { return m_bTaxiLightState; };

    bool IsLandingGearDown() { return m_bLandingGearDown; };
    void SetLandingGearDown(bool bLandingGearDown) { m_bLandingGearDown = bLandingGearDown; };

    unsigned short GetAdjustableProperty() { return m_usAdjustableProperty; };
    void           SetAdjustableProperty(unsigned short usAdjustable) { m_usAdjustableProperty = usAdjustable; };

    CPed* GetOccupant(unsigned int uiSeat);
    CPed* GetFirstOccupant();
    bool  SetOccupant(CPed* pPed, unsigned int uiSeat);
    CPed* GetController();

    class CPlayer* GetSyncer() { return m_pSyncer; };
    void           SetSyncer(class CPlayer* pPlayer);

    bool IsUnoccupiedSyncable() { return m_bUnoccupiedSyncable; };
    void SetUnoccupiedSyncable(bool bUnoccupiedSynced) { m_bUnoccupiedSyncable = bUnoccupiedSynced; };

    unsigned char GetMaxPassengers();
    unsigned char GetFreePassengerSeat();

    void SetMaxPassengers(unsigned char ucPassengers) { m_ucMaxPassengersOverride = ucPassengers; };

    CVehicleUpgrades* GetUpgrades() { return m_pUpgrades; }
    void              SetUpgrades(CVehicleUpgrades* pUpgrades);

    unsigned char GetOverrideLights() { return m_ucOverrideLights; }
    void          SetOverrideLights(unsigned char ucLights) { m_ucOverrideLights = ucLights; }

    CVehicle* GetTowedVehicle() { return m_pTowedVehicle; }
    bool      SetTowedVehicle(CVehicle* pVehicle);
    CVehicle* GetTowedByVehicle() { return m_pTowedByVehicle; }
    bool      SetTowedByVehicle(CVehicle* pVehicle);

    const char* GetRegPlate() { return m_szRegPlate; }
    void        SetRegPlate(const char* szRegPlate);
    void        GenerateRegPlate();

    unsigned char GetPaintjob() { return m_ucPaintjob; }
    void          SetPaintjob(unsigned char ucPaintjob);

    bool GetGunsEnabled() { return m_bGunsEnabled; }
    void SetGunsEnabled(bool bGunsEnabled) { m_bGunsEnabled = bGunsEnabled; }
    bool IsFuelTankExplodable() { return m_bFuelTankExplodable; }
    void SetFuelTankExplodable(bool bFuelTankExplodable) { m_bFuelTankExplodable = bFuelTankExplodable; }

    bool IsEngineOn() { return m_bEngineOn; }
    void SetEngineOn(bool bEngineOn) { m_bEngineOn = bEngineOn; }

    bool IsOnGround() { return m_bOnGround; };
    void SetOnGround(bool bOnGround) { m_bOnGround = bOnGround; };

    bool IsSmokeTrailEnabled() { return m_bSmokeTrail; }
    void SetSmokeTrailEnabled(bool bEnabled) { m_bSmokeTrail = bEnabled; }

    unsigned char GetAlpha() { return m_ucAlpha; }
    void          SetAlpha(unsigned char ucAlpha) { m_ucAlpha = ucAlpha; }

    void GetInitialDoorStates(SFixedArray<unsigned char, MAX_DOORS>& ucOutDoorStates);

    CPed* GetJackingPed() { return m_pJackingPed; }
    void  SetJackingPed(CPed* pPed);

    bool IsInWater() { return m_bInWater; }
    void SetInWater(bool bInWater) { m_bInWater = bInWater; }

    bool IsDamageProof() { return m_bDamageProof; }
    void SetDamageProof(bool bDamageProof) { m_bDamageProof = bDamageProof; }

    bool IsDerailed() { return m_bDerailed; }
    void SetDerailed(bool bDerailed) { m_bDerailed = bDerailed; }
    bool IsDerailable() { return m_bIsDerailable; }
    void SetDerailable(bool bDerailable) { m_bIsDerailable = bDerailable; }
    bool GetTrainDirection() { return m_bTrainDirection; }
    void SetTrainDirection(bool bDirection) { m_bTrainDirection = bDirection; }

    float GetTrainSpeed() { return m_fTrainSpeed; }
    void  SetTrainSpeed(float fSpeed) { m_fTrainSpeed = fSpeed; }

    float GetTrainPosition() { return m_fTrainPosition; }
    void  SetTrainPosition(float fPosition) { m_fTrainPosition = fPosition; }

    CTrainTrack* GetTrainTrack() { return m_pTrainTrack; }
    void         SetTrainTrack(CTrainTrack* pTrainTrack) { m_pTrainTrack = pTrainTrack; }

    SColor GetHeadLightColor() { return m_HeadLightColor; }
    void   SetHeadLightColor(const SColor color) { m_HeadLightColor = color; }

    bool IsHeliSearchLightVisible() { return m_bHeliSearchLightVisible; }
    void SetHeliSearchLightVisible(bool bVisible) { m_bHeliSearchLightVisible = bVisible; }

    bool HasHandlingChanged() { return m_bHandlingChanged; }
    void SetHasHandlingChanged(bool bChanged) { m_bHandlingChanged = bChanged; }

    bool GetCollisionEnabled() { return m_bCollisionsEnabled; }
    void SetCollisionEnabled(bool bCollisionEnabled) { m_bCollisionsEnabled = bCollisionEnabled; }

    // Functions used to remember where this vehicle spawns
    const CVector& GetRespawnPosition() { return m_vecRespawnPosition; };
    const CVector& GetRespawnRotationDegrees() { return m_vecRespawnRotationDegrees; };
    void           SetRespawnPosition(const CVector& vecPosition) { m_vecRespawnPosition = vecPosition; };
    void           SetRespawnRotationDegrees(const CVector& vecRotation) { m_vecRespawnRotationDegrees = vecRotation; };
    float          GetRespawnHealth() { return m_fRespawnHealth; };
    void           SetRespawnHealth(float fHealth) { m_fRespawnHealth = fHealth; };
    bool           GetRespawnEnabled() const noexcept { return m_bRespawnEnabled; }
    void           SetRespawnEnabled(bool bEnabled);
    std::uint32_t  GetBlowRespawnInterval() const noexcept { return m_ulBlowRespawnInterval; }
    void           SetBlowRespawnInterval(unsigned long ulTime) { m_ulBlowRespawnInterval = ulTime; }
    std::uint32_t  GetIdleRespawnInterval() const noexcept { return m_ulIdleRespawnInterval; }
    void           SetIdleRespawnInterval(unsigned long ulTime) { m_ulIdleRespawnInterval = ulTime; }

    void SpawnAt(const CVector& vecPosition, const CVector& vecRotation);
    void Respawn();

    void            GenerateHandlingData() noexcept;
    CHandlingEntry* GetHandlingData() noexcept { return m_HandlingEntry.get(); };

    uint GetTimeSinceLastPush() { return (uint)(CTickCount::Now(true) - m_LastPushedTime).ToLongLong(); }
    void ResetLastPushTime() { m_LastPushedTime = CTickCount::Now(true); }

    bool DoesVehicleHaveSirens() { return m_tSirenBeaconInfo.m_bOverrideSirens; }
    void RemoveVehicleSirens();
    void SetVehicleSirenPosition(unsigned char ucSirenID, CVector vecPos);
    void SetVehicleSirenMinimumAlpha(unsigned char ucSirenID, DWORD dwPercentage);
    void SetVehicleSirenColour(unsigned char ucSirenID, SColor tVehicleSirenColour);
    void SetVehicleFlags(bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent);

    void ResetDoors();
    void ResetDoorsWheelsPanelsLights();

    bool IsBlowTimerFinished();
    void ResetExplosionTimer();

    bool             IsBlown() const noexcept { return m_blowState != VehicleBlowState::INTACT; }
    void             SetBlowState(VehicleBlowState state);
    VehicleBlowState GetBlowState() const noexcept { return m_blowState; }

    void StopIdleTimer();
    void RestartIdleTimer();
    bool IsIdleTimerRunning();
    bool IsIdleTimerFinished();
    bool IsStationary();
    void OnRelayUnoccupiedSync();
    void HandleDimensionResync();

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    class CVehicleManager* m_pVehicleManager;

    CPlayer*                              m_pSyncer;
    SFixedArray<CPed*, MAX_VEHICLE_SEATS> m_pOccupants;

    unsigned short m_usModel;
    eVehicleType   m_eVehicleType;
    CVector        m_vecPosition;
    CVector        m_vecRotationDegrees;
    CVector        m_vecVelocity;
    CVector        m_vecVelocityMeters;
    CVector        m_vecTurnSpeed;
    float          m_fHealth;
    float          m_fLastSyncedHealthHealth;
    CTickCount     m_llBlowTime;
    CTickCount     m_llIdleTime;

    VehicleBlowState m_blowState = VehicleBlowState::INTACT;

    unsigned char m_ucMaxPassengersOverride;

    CVehicleColor m_Color;

    bool m_bIsFrozen;
    bool m_bUnoccupiedSyncable;

    CVehicleUpgrades* m_pUpgrades;

    unsigned char m_ucOverrideLights;

    CVehicle* m_pTowedVehicle;
    CVehicle* m_pTowedByVehicle;

    char          m_szRegPlate[9];
    unsigned char m_ucPaintjob;

    SFixedArray<float, 6> m_fDoorOpenRatio;
    bool                  m_bLocked;
    bool                  m_bDoorsUndamageable;
    bool                  m_bEngineOn;
    bool                  m_bGunsEnabled;
    bool                  m_bFuelTankExplodable;
    bool                  m_bOnGround;
    bool                  m_bSmokeTrail;
    unsigned char         m_ucAlpha;
    bool                  m_bInWater;
    CPed*                 m_pJackingPed;
    SColor                m_HeadLightColor;
    bool                  m_bHeliSearchLightVisible;

    // Train specific data
    bool  m_bDerailed;
    bool  m_bIsDerailable;
    bool  m_bTrainDirection;
    float m_fTrainSpeed;
    float m_fTrainPosition;

    CTrainTrack* m_pTrainTrack = nullptr;

    // Used to remember where this vehicle spawns
    CVector       m_vecRespawnPosition;
    CVector       m_vecRespawnRotationDegrees;
    float         m_fRespawnHealth;
    bool          m_bRespawnEnabled;
    unsigned long m_ulBlowRespawnInterval;
    unsigned long m_ulIdleRespawnInterval;

    // Vehicle specific data
    float          m_fTurretPositionX;
    float          m_fTurretPositionY;
    bool           m_bSirenActive;
    bool           m_bTaxiLightState;
    bool           m_bLandingGearDown;
    unsigned short m_usAdjustableProperty;
    bool           m_bCollisionsEnabled;

    std::unique_ptr<CHandlingEntry> m_HandlingEntry;
    bool                            m_bHandlingChanged;

    unsigned char m_ucVariant;
    unsigned char m_ucVariant2;

    CTickCount m_LastPushedTime;
    CVector    m_vecStationaryCheckPosition;
    bool       m_bNeedsDimensionResync;
    ushort     m_usLastUnoccupiedSyncDimension;

public:            // 'Safe' variables (that have no need for accessors)
    bool                                   m_bDamageProof;
    uint                                   m_uiDamageInfoSendPhase;
    SFixedArray<unsigned char, MAX_DOORS>  m_ucDoorStates;
    SFixedArray<unsigned char, MAX_WHEELS> m_ucWheelStates;
    SFixedArray<unsigned char, MAX_PANELS> m_ucPanelStates;
    SFixedArray<unsigned char, MAX_LIGHTS> m_ucLightStates;
    SSirenInfo                             m_tSirenBeaconInfo;
    bool                                   m_bOccupantChanged;
};
