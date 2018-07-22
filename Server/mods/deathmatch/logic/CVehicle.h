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

class CTrainTrack;

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

class CVehicle : public CElement
{
    friend class CPlayer;

public:
    ZERO_ON_NEW
    CVehicle(class CVehicleManager* pVehicleManager, CElement* pParent, CXMLNode* pNode, unsigned short usModel, unsigned char ucVariant,
             unsigned char ucVariant2);
    ~CVehicle(void);

    bool IsEntity(void) { return true; }

    void Unlink(void);
    bool ReadSpecialData(void);

    void DoPulse(void);

    unsigned short GetModel(void) { return m_usModel; };
    void           SetModel(unsigned short usModel);
    bool           HasValidModel(void);

    unsigned char GetVariant(void) { return m_ucVariant; };
    unsigned char GetVariant2(void) { return m_ucVariant2; };

    void SetVariants(unsigned char ucVariant, unsigned char ucVariant2);

    eVehicleType GetVehicleType(void) { return m_eVehicleType; };

    CVehicleColor& GetColor(void) { return m_Color; };
    void           SetColor(const CVehicleColor& Color) { m_Color = Color; };

    bool IsFrozen(void) { return m_bIsFrozen; };
    void SetFrozen(bool bIsFrozen) { m_bIsFrozen = bIsFrozen; };

    const CVector& GetPosition(void);
    void           SetPosition(const CVector& vecPosition);
    void           GetRotation(CVector& vecRotation);
    void           GetRotationDegrees(CVector& vecRotation);
    void           SetRotationDegrees(const CVector& vecRotation);
    void           GetMatrix(CMatrix& matrix);
    void           SetMatrix(const CMatrix& matrix);

    const CVector& GetVelocity(void) { return m_vecVelocity; };
    void           SetVelocity(const CVector& vecVelocity) { m_vecVelocity = vecVelocity; };
    const CVector& GetVelocityMeters(void) { return m_vecVelocityMeters; };
    void           SetVelocityMeters(const CVector& vecVelocityMeters) { m_vecVelocityMeters = vecVelocityMeters; };
    const CVector& GetTurnSpeed(void) { return m_vecTurnSpeed; };
    void           SetTurnSpeed(const CVector& vecTurnSpeed) { m_vecTurnSpeed = vecTurnSpeed; };

    float GetHealth(void) { return m_fHealth; };
    void  SetHealth(float fHealth) { m_fHealth = fHealth; };
    float GetLastSyncedHealth(void) { return m_fLastSyncedHealthHealth; };
    void  SetLastSyncedHealth(float fHealth) { m_fLastSyncedHealthHealth = fHealth; };

    CVehicleColor& RandomizeColor(void);

    float GetDoorOpenRatio(unsigned char ucDoor) const;
    void  SetDoorOpenRatio(unsigned char ucDoor, float fRatio);
    bool  IsLocked(void) { return m_bLocked; };
    void  SetLocked(bool bLocked) { m_bLocked = bLocked; };

    bool AreDoorsUndamageable(void) { return m_bDoorsUndamageable; };
    void SetDoorsUndamageable(bool bUndamageable) { m_bDoorsUndamageable = bUndamageable; };

    float GetTurretPositionX(void) { return m_fTurretPositionX; };
    float GetTurretPositionY(void) { return m_fTurretPositionY; };
    void  GetTurretPosition(float& fPositionX, float& fPositionY);
    void  SetTurretPosition(float fPositionX, float fPositionY);

    bool IsSirenActive(void) { return m_bSirenActive; };
    void SetSirenActive(bool bSirenActive) { m_bSirenActive = bSirenActive; };
    void SetTaxiLightOn(bool bTaxiLightState) { m_bTaxiLightState = bTaxiLightState; };
    bool IsTaxiLightOn(void) { return m_bTaxiLightState; };

    bool IsLandingGearDown(void) { return m_bLandingGearDown; };
    void SetLandingGearDown(bool bLandingGearDown) { m_bLandingGearDown = bLandingGearDown; };

    unsigned short GetAdjustableProperty(void) { return m_usAdjustableProperty; };
    void           SetAdjustableProperty(unsigned short usAdjustable) { m_usAdjustableProperty = usAdjustable; };

    CPed* GetOccupant(unsigned int uiSeat);
    CPed* GetFirstOccupant(void);
    bool  SetOccupant(CPed* pPed, unsigned int uiSeat);
    CPed* GetController(void);

    class CPlayer* GetSyncer(void) { return m_pSyncer; };
    void           SetSyncer(class CPlayer* pPlayer);

    bool IsUnoccupiedSyncable(void) { return m_bUnoccupiedSyncable; };
    void SetUnoccupiedSyncable(bool bUnoccupiedSynced) { m_bUnoccupiedSyncable = bUnoccupiedSynced; };

    unsigned char GetMaxPassengers(void);
    unsigned char GetFreePassengerSeat(void);

    void SetMaxPassengers(unsigned char ucPassengers) { m_ucMaxPassengersOverride = ucPassengers; };

    CVehicleUpgrades* GetUpgrades(void) { return m_pUpgrades; }
    void              SetUpgrades(CVehicleUpgrades* pUpgrades);

    unsigned char GetOverrideLights(void) { return m_ucOverrideLights; }
    void          SetOverrideLights(unsigned char ucLights) { m_ucOverrideLights = ucLights; }

    CVehicle* GetTowedVehicle(void) { return m_pTowedVehicle; }
    bool      SetTowedVehicle(CVehicle* pVehicle);
    CVehicle* GetTowedByVehicle(void) { return m_pTowedByVehicle; }
    bool      SetTowedByVehicle(CVehicle* pVehicle);

    const char* GetRegPlate(void) { return m_szRegPlate; }
    void        SetRegPlate(const char* szRegPlate);
    void        GenerateRegPlate(void);

    unsigned char GetPaintjob(void) { return m_ucPaintjob; }
    void          SetPaintjob(unsigned char ucPaintjob);

    bool GetGunsEnabled(void) { return m_bGunsEnabled; }
    void SetGunsEnabled(bool bGunsEnabled) { m_bGunsEnabled = bGunsEnabled; }
    bool IsFuelTankExplodable(void) { return m_bFuelTankExplodable; }
    void SetFuelTankExplodable(bool bFuelTankExplodable) { m_bFuelTankExplodable = bFuelTankExplodable; }

    bool IsEngineOn(void) { return m_bEngineOn; }
    void SetEngineOn(bool bEngineOn) { m_bEngineOn = bEngineOn; }

    bool IsOnGround(void) { return m_bOnGround; };
    void SetOnGround(bool bOnGround) { m_bOnGround = bOnGround; };

    bool IsSmokeTrailEnabled(void) { return m_bSmokeTrail; }
    void SetSmokeTrailEnabled(bool bEnabled) { m_bSmokeTrail = bEnabled; }

    unsigned char GetAlpha(void) { return m_ucAlpha; }
    void          SetAlpha(unsigned char ucAlpha) { m_ucAlpha = ucAlpha; }

    void GetInitialDoorStates(SFixedArray<unsigned char, MAX_DOORS>& ucOutDoorStates);

    CPlayer* GetJackingPlayer(void) { return m_pJackingPlayer; }
    void     SetJackingPlayer(CPlayer* pPlayer);

    bool IsInWater(void) { return m_bInWater; }
    void SetInWater(bool bInWater) { m_bInWater = bInWater; }

    bool IsDamageProof(void) { return m_bDamageProof; }
    void SetDamageProof(bool bDamageProof) { m_bDamageProof = bDamageProof; }

    bool IsDerailed(void) { return m_bDerailed; }
    void SetDerailed(bool bDerailed) { m_bDerailed = bDerailed; }
    bool IsDerailable(void) { return m_bIsDerailable; }
    void SetDerailable(bool bDerailable) { m_bIsDerailable = bDerailable; }
    bool GetTrainDirection(void) { return m_bTrainDirection; }
    void SetTrainDirection(bool bDirection) { m_bTrainDirection = bDirection; }

    float GetTrainSpeed(void) { return m_fTrainSpeed; }
    void  SetTrainSpeed(float fSpeed) { m_fTrainSpeed = fSpeed; }

    float GetTrainPosition(void) { return m_fTrainPosition; }
    void  SetTrainPosition(float fPosition) { m_fTrainPosition = fPosition; }

    CTrainTrack* GetTrainTrack(void) { return m_pTrainTrack; }
    void         SetTrainTrack(CTrainTrack* pTrainTrack) { m_pTrainTrack = pTrainTrack; }

    SColor GetHeadLightColor(void) { return m_HeadLightColor; }
    void   SetHeadLightColor(const SColor color) { m_HeadLightColor = color; }

    bool IsHeliSearchLightVisible(void) { return m_bHeliSearchLightVisible; }
    void SetHeliSearchLightVisible(bool bVisible) { m_bHeliSearchLightVisible = bVisible; }

    bool HasHandlingChanged(void) { return m_bHandlingChanged; }
    void SetHasHandlingChanged(bool bChanged) { m_bHandlingChanged = bChanged; }

    bool GetCollisionEnabled(void) { return m_bCollisionsEnabled; }
    void SetCollisionEnabled(bool bCollisionEnabled) { m_bCollisionsEnabled = bCollisionEnabled; }

    // Functions used to remember where this vehicle spawns
    const CVector& GetRespawnPosition(void) { return m_vecRespawnPosition; };
    void           SetRespawnPosition(const CVector& vecPosition) { m_vecRespawnPosition = vecPosition; };
    void           GetRespawnRotationDegrees(CVector& vecRotation) { vecRotation = m_vecRespawnRotationDegrees; };
    void           SetRespawnRotationDegrees(const CVector& vecRotation) { m_vecRespawnRotationDegrees = vecRotation; };
    float          GetRespawnHealth(void) { return m_fRespawnHealth; };
    void           SetRespawnHealth(float fHealth) { m_fRespawnHealth = fHealth; };
    bool           GetRespawnEnabled(void) { return m_bRespawnEnabled; }
    void           SetRespawnEnabled(bool bEnabled);
    void           SetBlowRespawnInterval(unsigned long ulTime) { m_ulBlowRespawnInterval = ulTime; }
    void           SetIdleRespawnInterval(unsigned long ulTime) { m_ulIdleRespawnInterval = ulTime; }

    void SpawnAt(const CVector& vecPosition, const CVector& vecRotation);
    void Respawn(void);

    void            GenerateHandlingData(void);
    CHandlingEntry* GetHandlingData(void) { return m_pHandlingEntry; }

    uint GetTimeSinceLastPush(void) { return (uint)(CTickCount::Now(true) - m_LastPushedTime).ToLongLong(); }
    void ResetLastPushTime(void) { m_LastPushedTime = CTickCount::Now(true); }

    bool DoesVehicleHaveSirens(void) { return m_tSirenBeaconInfo.m_bOverrideSirens; }
    void RemoveVehicleSirens(void);
    void SetVehicleSirenPosition(unsigned char ucSirenID, CVector vecPos);
    void SetVehicleSirenMinimumAlpha(unsigned char ucSirenID, DWORD dwPercentage);
    void SetVehicleSirenColour(unsigned char ucSirenID, SColor tVehicleSirenColour);
    void SetVehicleFlags(bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent);

    void ResetDoors(void);
    void ResetDoorsWheelsPanelsLights(void);
    void SetIsBlown(bool bBlown);
    bool GetIsBlown(void);
    bool IsBlowTimerFinished(void);
    void StopIdleTimer(void);
    void RestartIdleTimer(void);
    bool IsIdleTimerRunning(void);
    bool IsIdleTimerFinished(void);
    bool IsStationary(void);
    void OnRelayUnoccupiedSync(void);
    void HandleDimensionResync(void);

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
    CPlayer*              m_pJackingPlayer;
    SColor                m_HeadLightColor;
    bool                  m_bHeliSearchLightVisible;

    // Train specific data
    bool         m_bDerailed;
    bool         m_bIsDerailable;
    bool         m_bTrainDirection;
    float        m_fTrainSpeed;
    float        m_fTrainPosition;
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

    CHandlingEntry* m_pHandlingEntry;
    bool            m_bHandlingChanged;

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
