/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CVehicleSA.h
 *  PURPOSE:     Header file for vehicle base entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/RenderWare.h>
#include <game/CVehicle.h>
#include "CPedSA.h"
#include "CPhysicalSA.h"
#include "CPoolsSA.h"
#include "CHandlingManagerSA.h"
#include "CDamageManagerSA.h"
#include "CDoorSA.h"
#include "CColPointSA.h"
#include "CAEVehicleAudioEntitySA.h"
#include "CNodeSA.h"

class CFxSystemSAInterface;
class CTrainSAInterface;
struct RwTexture;

#define SIZEOF_CHELI   2584
#define MAX_PASSENGERS 8
#define NUM_RAILTRACKS 4

constexpr DWORD FUNC_CCarCtrlCreateCarForScript          = 0x431f80;
constexpr DWORD FUNC_CVehicle_AddVehicleUpgrade          = 0x6E3290;
constexpr DWORD FUNC_CVehicle_RemoveVehicleUpgrade       = 0x6DF930;
constexpr DWORD FUNC_CVehicle_GetBaseVehicleType         = 0x411D50;
constexpr DWORD FUNC_CVehicle_IsUpsideDown               = 0x6D1D90;
constexpr DWORD FUNC_CVehicle_SetEngineOn                = 0x41BDD0;
constexpr DWORD FUNC_CTrain_FindPositionOnTrackFromCoors = 0x6F6CC0;

constexpr DWORD FUNC_CVehicle_QueryPickedUpEntityWithWinch   = 0x6d3cf0;
constexpr DWORD FUNC_CVehicle_PickUpEntityWithWinch          = 0x6d3cd0;
constexpr DWORD FUNC_CVehicle_ReleasePickedUpEntityWithWinch = 0x6d3cb0;
constexpr DWORD FUNC_CVehicle_SetRopeHeightForHeli           = 0x6d3d30;

constexpr DWORD FUNC_CVehicle__SetRemapTexDictionary       = 0x6D0BC0;
constexpr DWORD FUNC_CVehicle__GetRemapIndex               = 0x6D0B70;
constexpr DWORD FUNC_CVehicle__SetRemap                    = 0x6D0C00;
constexpr DWORD FUNC_CVehicle_CustomCarPlate_TextureCreate = 0x6D10E0;

constexpr DWORD FUNC_CVehicle_SetWindowOpenFlag   = 0x6D3080;
constexpr DWORD FUNC_CVehicle_ClearWindowOpenFlag = 0x6D30B0;

// from CBike
constexpr DWORD FUNC_Bike_PlaceOnRoadProperly       = 0x6BEEB0;
constexpr DWORD FUNC_Automobile_PlaceOnRoadProperly = 0x6AF420;
constexpr DWORD FUNC_CBike_Fix                      = 0x6B7050;

// from CPlane
constexpr DWORD FUNC_CPlane__Fix = 0x6CABB0;

// from CAutomobile
constexpr DWORD FUNC_CAutomobile__Fix                   = 0x6A3440;
constexpr DWORD FUNC_CAutomobile__SpawnFlyingComponent  = 0x6a8580;
constexpr DWORD FUNC_CAutomobile__UpdateMovingCollision = 0x6a1460;

// from CHeli
constexpr DWORD FUNC_CHeli__Fix = 0x6C4530;

// from CQuadBike
constexpr DWORD FUNC_CQuadBike__Fix = 0x6CE2B0;

constexpr DWORD VARS_CarCounts = 0x969094; // 5 values for each vehicle type

// Used when deleting vehicles
constexpr DWORD VTBL_CPlaceable = 0x863C40;

constexpr DWORD ARRAY_NumRailTrackNodes     = 0xC38014;    // NUM_RAILTRACKS dwords
constexpr DWORD ARRAY_RailTrackNodePointers = 0xC38024;    // NUM_RAILTRACKS pointers to arrays of SRailNode

constexpr DWORD VAR_CVehicle_SpecialColModels = 0xc1cc78;

constexpr DWORD FUNC_CAutomobile__GetDoorAngleOpenRatio = 0x6A2270;
constexpr DWORD FUNC_CTrain__GetDoorAngleOpenRatio      = 0x6F59C0;

constexpr DWORD HANDLING_NOS_Flag        = 0x00080000;
constexpr DWORD HANDLING_Hydraulics_Flag = 0x00020000;

constexpr DWORD VAR_CVehicle_Variation1 = 0x8A6458;
constexpr DWORD VAR_CVehicle_Variation2 = 0x8A6459;

// for vehicle sun glare
constexpr DWORD FUNC_CAutomobile_OnVehiclePreRender = 0x6ABCFD;
constexpr DWORD FUNC_CVehicle_DoSunGlare            = 0x6DD6F0;

struct SRailNodeSA
{
    WORD sX;                       // x coordinate times 8
    WORD sY;                       // y coordinate times 8
    WORD sZ;                       // z coordinate times 8
    WORD sRailDistance;            // on-rail distance times 3.33333334
    WORD padding;
};

class CVehicleSAInterfaceVTBL : public CEntitySAInterfaceVTBL
{
public:
    DWORD ProcessEntityCollision;                  // +58h
    DWORD ProcessControlCollisionCheck;            // +5Ch
    DWORD ProcessControlInputs;                    // +60h
    DWORD GetComponentWorldPosition;               // +64h
    DWORD IsComponentPresent;                      // +68h
    DWORD OpenDoor;                                // +6Ch
    DWORD ProcessOpenDoor;                         // +70h
    DWORD GetDoorAngleOpenRatio;                   // +74h
    DWORD GetDoorAngleOpenRatio_;                  // +78h
    DWORD IsDoorReady;                             // +7Ch
    DWORD IsDoorReady_;                            // +80h
    DWORD IsDoorFullyOpen;                         // +84h
    DWORD IsDoorFullyOpen_;                        // +88h
    DWORD IsDoorClosed;                            // +8Ch
    DWORD IsDoorClosed_;                           // +90h
    DWORD IsDoorMissing;                           // +94h
    DWORD IsDoorMissing_;                          // +98h
    DWORD IsOpenTopCar;                            // +9Ch
    DWORD RemoveRefsToVehicle;                     // +A0h
    DWORD BlowUpCar;                               // +A4h
    DWORD BlowUpCarCutSceneNoExtras;               // +A8h
    DWORD SetUpWheelColModel;                      // +ACh
    DWORD BurstTyre;                               // +B0h
    DWORD IsRoomForPedToLeaveCar;                  // +B4h
    DWORD ProcessDrivingAnims;                     // +B8h
    DWORD GetRideAnimData;                         // +BCh
    DWORD SetupSuspensionLines;                    // +C0h
    DWORD AddMovingCollisionSpeed;                 // +C4h
    DWORD Fix;                                     // +C8h
    DWORD SetupDamageAfterLoad;                    // +CCh
    DWORD DoBurstAndSoftGroundRatios;              // +D0h
    DWORD GetHeightAboveRoad;                      // +D4h
    DWORD PlayCarHorn;                             // +D8h
    DWORD GetNumContactWheels;                     // +DCh
    DWORD VehicleDamage;                           // +E0h
    DWORD CanPedStepOutCar;                        // +E4h
    DWORD CanPedJumpOutCar;                        // +E8h
    DWORD GetTowHitchPos;                          // +ECh
    DWORD GetTowbarPos;                            // +F0h
    DWORD SetTowLink;                              // +F4h
    DWORD BreakTowLink;                            // +F8h
    DWORD FindWheelWidth;                          // +FCh
    DWORD Save;                                    // +100h
    DWORD Load;                                    // +104h
};

struct CVehicleFlags
{
    std::uint8_t bIsLawEnforcer : 1;                // Is this guy chasing the player at the moment
    std::uint8_t bIsAmbulanceOnDuty : 1;            // Ambulance trying to get to an accident
    std::uint8_t bIsFireTruckOnDuty : 1;            // Firetruck trying to get to a fire
    std::uint8_t bIsLocked : 1;                     // Is this guy locked by the script (cannot be removed)
    std::uint8_t bEngineOn : 1;                     // For sound purposes. Parked cars have their engines switched off (so do destroyed cars)
    std::uint8_t bIsHandbrakeOn : 1;                // How's the handbrake doing ?
    std::uint8_t bLightsOn : 1;                     // Are the lights switched on ?
    std::uint8_t bFreebies : 1;                     // Any freebies left in this vehicle ?

    std::uint8_t bIsVan : 1;                       // Is this vehicle a van (doors at back of vehicle)
    std::uint8_t bIsBus : 1;                       // Is this vehicle a bus
    std::uint8_t bIsBig : 1;                       // Is this vehicle a bus
    std::uint8_t bLowVehicle : 1;                  // Need this for sporty type cars to use low getting-in/out anims
    std::uint8_t bComedyControls : 1;              // Will make the car hard to control (hopefully in a funny way)
    std::uint8_t bWarnedPeds : 1;                  // Has scan and warn peds of danger been processed?
    std::uint8_t bCraneMessageDone : 1;            // A crane message has been printed for this car allready
    // std::uint8_t bExtendedRange: 1; // This vehicle needs to be a bit further away to get deleted
    std::uint8_t bTakeLessDamage : 1;            // This vehicle is stronger (takes about 1/4 of damage)

    std::uint8_t bIsDamaged : 1;                       // This vehicle has been damaged and is displaying all its components
    std::uint8_t bHasBeenOwnedByPlayer : 1;            // To work out whether stealing it is a crime
    std::uint8_t bFadeOut : 1;                         // Fade vehicle out
    std::uint8_t bIsBeingCarJacked : 1;                // Fade vehicle out
    std::uint8_t bCreateRoadBlockPeds : 1;             // If this vehicle gets close enough we will create peds (coppers or gang members) round it
    std::uint8_t bCanBeDamaged : 1;                    // Set to FALSE during cut scenes to avoid explosions
    // std::uint8_t bUsingSpecialColModel : 1;
    // Is player vehicle using special collision model, stored in player strucure
    std::uint8_t bOccupantsHaveBeenGenerated : 1;            // Is true if the occupants have already been generated. (Shouldn't happen again)
    std::uint8_t bGunSwitchedOff : 1;                        // Level designers can use this to switch off guns on boats

    std::uint8_t bVehicleColProcessed : 1;               // Has ProcessEntityCollision been processed for this car?
    std::uint8_t bIsCarParkVehicle : 1;                  // Car has been created using the special CAR_PARK script command
    std::uint8_t bHasAlreadyBeenRecorded : 1;            // Used for replays
    std::uint8_t bPartOfConvoy : 1;
    std::uint8_t bHeliMinimumTilt : 1;              // This heli should have almost no tilt really
    std::uint8_t bAudioChangingGear : 1;            // sounds like vehicle is changing gear
    std::uint8_t bIsDrowning : 1;                   // is vehicle occupants taking damage in water (i.e. vehicle is dead in water)
    std::uint8_t bTyresDontBurst : 1;               // If this is set the tyres are invincible

    std::uint8_t bCreatedAsPoliceVehicle : 1;            // True if this guy was created as a police vehicle (enforcer, policecar, miamivice car etc)
    std::uint8_t bRestingOnPhysical : 1;                 // Dont go static cause car is sitting on a physical object that might get removed
    std::uint8_t bParking : 1;
    std::uint8_t bCanPark : 1;
    std::uint8_t bFireGun : 1;                                // Does the ai of this vehicle want to fire it's gun?
    std::uint8_t bDriverLastFrame : 1;                        // Was there a driver present last frame ?
    std::uint8_t bNeverUseSmallerRemovalRange : 1;            // Some vehicles (like planes) we don't want to remove just behind the camera.
    std::uint8_t bIsRCVehicle : 1;                            // Is this a remote controlled (small) vehicle. True whether the player or AI controls it.

    std::uint8_t bAlwaysSkidMarks : 1;                      // This vehicle leaves skidmarks regardless of the wheels' states.
    std::uint8_t bEngineBroken : 1;                         // Engine doesn't work. Player can get in but the vehicle won't drive
    std::uint8_t bVehicleCanBeTargetted : 1;                // The ped driving this vehicle can be targetted, (for Torenos plane mission)
    std::uint8_t bPartOfAttackWave : 1;                     // This car is used in an attack during a gang war
    std::uint8_t bWinchCanPickMeUp : 1;                     // This car cannot be picked up by any ropes.
    std::uint8_t bImpounded : 1;                            // Has this vehicle been in a police impounding garage
    std::uint8_t bVehicleCanBeTargettedByHS : 1;            // Heat seeking missiles will not target this vehicle.
    std::uint8_t bSirenOrAlarm : 1;                         // Set to TRUE if siren or alarm active, else FALSE

    std::uint8_t bHasGangLeaningOn : 1;
    std::uint8_t bGangMembersForRoadBlock : 1;            // Will generate gang members if NumPedsForRoadBlock > 0
    std::uint8_t bDoesProvideCover : 1;                   // If this is false this particular vehicle can not be used to take cover behind.
    std::uint8_t bMadDriver : 1;                          // This vehicle is driving like a lunatic
    std::uint8_t bUpgradedStereo : 1;                     // This vehicle has an upgraded stereo
    std::uint8_t bConsideredByPlayer : 1;                 // This vehicle is considered by the player to enter
    std::uint8_t bPetrolTankIsWeakPoint : 1;              // If false shootong the petrol tank will NOT Blow up the car
    std::uint8_t bDisableParticles : 1;                   // Disable particles from this car. Used in garage.

    std::uint8_t bHasBeenResprayed : 1;                      // Has been resprayed in a respray garage. Reset after it has been checked.
    std::uint8_t bUseCarCheats : 1;                          // If this is true will set the car cheat stuff up in ProcessControl()
    std::uint8_t bDontSetColourWhenRemapping : 1;            // If the texture gets remapped we don't want to change the colour with it.
    std::uint8_t bUsedForReplay : 1;                         // This car is controlled by replay and should be removed when replay is done.
};

struct CTransmissionGear
{
    float maxGearVelocity;    // +0
    float changeUpVelocity;   // +4
    float changeDownVelocity; // +8
};

struct CTransmission
{
    CTransmissionGear gears[6];        // +0

    std::uint8_t driveType;            // +48
    std::uint8_t engineType;           // +49
    std::uint8_t numOfGears;           // +4A
    std::uint8_t pad;                  // +4B
    std::uint32_t handlingFlags;       // +4C
    float engineAcceleration;          // +50
    float engineInertia;               // +54
    float maxGearVelocity;             // +58
    std::uint32_t pad2;                // +5C
    float maxReverseGearVelocity;      // +60
    float curSpeed;                    // +64
};

class CAutoPilot
{
public:
    CNodeAddressSA* m_currentAddress;               // +0
    CNodeAddressSA* m_startingRouteNode;            // +4
    CNodeAddressSA* field_8;                        // +8
    std::uint32_t   dwordC;                         // +C

    std::uint32_t   m_speedScaleFactor;                     // +10
    std::uint16_t   m_currentPathNodeInfo;                  // +14
    std::uint16_t   m_nextPathNodeInfo;                     // +16
    std::uint16_t   m_previousPathNodeInfo;                 // +18
    std::uint8_t    _pad1[2];                               // +1A
    std::uint32_t   m_timeToStartMission;                   // +1C
    std::uint32_t   m_timeSwitchedToRealPhysics;            // +20
    std::uint8_t    field_24;                               // +24
    std::uint8_t    _smthCurr;                              // +25
    std::uint8_t    _smthNext;                              // +26
    std::uint8_t    m_currentLane;                          // +27
    std::uint8_t    m_nextLane;                             // +28
    std::uint8_t    m_carDrivingStyle;                      // +29
    std::uint8_t    m_carMission;                           // +2A
    std::uint8_t    m_tempAction;                           // +2B
    std::uint32_t   m_tempActionTime;                       // +2C
    std::uint32_t   m_someStartTime;                        // +30
    std::uint8_t    byte34;                                 // +34
    std::uint8_t    byte35;                                 // +35
    std::uint8_t    f36[2];                                 // +36
    std::uint32_t   m_speed;                                // +38
    std::uint32_t   m_maxTrafficSpeed;                      // +3C
    std::uint8_t    m_cruiseSpeed;                          // +40
    std::uint8_t    byte41;                                 // +41
    std::uint8_t    f42[2];                                 // +42
    std::uint32_t   float44;                                // +44
    std::uint8_t    f48;                                    // +48
    std::uint8_t    byte49;                                 // +49
    std::uint8_t    byte4A;                                 // +4A
    std::uint8_t    m_carCtrlFlags;                         // +4B
    std::uint8_t    byte4C;                                 // +4C
    std::uint8_t    m_straightLineDistance;                 // +4D
    std::uint8_t    byte4E;                                 // +4E
    std::uint8_t    byte4F;                                 // +4F
    std::uint8_t    byte50;                                 // +50
    std::uint8_t    f51[11];                                // +51
    CVector         m_destinationCoors;                     // +5C
    CNodeAddressSA* m_pathFindNodesInfo[8];                 // +68
    std::uint16_t   m_pathFindNodesCount;                   // +88
    std::uint8_t    f8A[2];                                 // +8A
    std::uint32_t   m_targetCar;                            // +8C
    std::uint32_t   m_carWeMakingSlowDownFor;               // +90
    std::uint8_t    m_vehicleRecordingId;                   // +94
    std::uint8_t    m_planeDogfightSomething;               // +95
    std::uint16_t   field_96;                               // +96
};
static_assert(sizeof(CAutoPilot) == 0x98, "Invalid size");

#define MAX_UPGRADES_ATTACHED 15 // perhaps?

class CVehicleSAInterface : public CPhysicalSAInterface
{
public:
    CAEVehicleAudioEntitySAInterface m_vehicleAudioEntity; // +138

    tHandlingDataSA*       m_handlingData;                 // +384
    tFlyingHandlingDataSA* m_flyingHandlingData;           // +388
    DWORD                  m_handlingFlags;                // +38C

    CAutoPilot    m_autoPilot;                             // +390
    CVehicleFlags m_vehicleFlags;                          // +428
    std::uint32_t m_creationTime;                          // +430 ; GetTimeInMilliseconds when this vehicle was created.

    std::uint8_t m_primaryColor;                           // +434
    std::uint8_t m_secondaryColor;                         // +435
    std::uint8_t m_tertiaryColor;                          // +436
    std::uint8_t m_quaternaryColor;                        // +437
    std::uint8_t m_extras[2];                              // +438
    std::uint16_t m_upgrades[MAX_UPGRADES_ATTACHED];       // +43A
    float         m_wheelScale;                            // +458

    std::uint16_t CarAlarmState;               // 1116
    std::uint16_t ForcedRandomSeed;            // if this is non-zero the random wander gets deterministic

    CPedSAInterface* pDriver;
    CPedSAInterface* pPassengers[MAX_PASSENGERS];
    std::uint8_t m_nNumPassengers;
    std::uint8_t m_nNumGettingIn;
    std::uint8_t m_nGettingInFlags;
    std::uint8_t m_nGettingOutFlags;
    std::uint8_t m_nMaxPassengers;
    std::uint8_t m_windowsOpenFlags;
    char         m_nNitroBoosts;            // +1162
    // float m_fNitroTimer; // +2212

    std::uint8_t m_nSpecialColModel;
    CEntity*      pEntityWeAreOnForVisibilityCheck;
    CFire*        m_pFire;

    float m_fSteerAngle;               // +1172
    float m_f2ndSteerAngle;            // used for steering 2nd set of wheels or elevators etc..
    float m_fGasPedal;                 // 0...1  // +1180
    float m_fBrakePedal;               // 0...1

    std::uint8_t VehicleCreatedBy;            // Contains information on whether this vehicle can be deleted
                                               // or not. Probably only need this or IsLocked.
    short ExtendedRemovalRange;

    std::uint8_t BombOnBoard : 3;            // 0 = None. 1 = Timed. 2 = On ignition, 3 = remotely set ? 4 = Timed Bomb has been activated. 5 = On ignition has
                                              // been activated.
    std::uint8_t OverrideLights : 2;                  // uses enum NO_CAR_LIGHT_OVERRIDE, FORCE_CAR_LIGHTS_OFF, FORCE_CAR_LIGHTS_ON
    std::uint8_t WinchType : 2;                       // Does this vehicle use a winch?
    std::uint8_t m_GunsCycleIndex : 2;                // Cycle through alternate gun hardpoints on planes/helis
    std::uint8_t m_OrdnanceCycleIndex : 2;            // Cycle through alternate ordnance hardpoints on planes/helis

    std::uint8_t nUsedForCover;            // Has n number of cops hiding/attempting to hid behind it
    std::uint8_t AmmoInClip;               // Used to make the guns on boat do a reload.
    std::uint8_t PacMansCollected;
    std::uint8_t PedsPositionForRoadBlock;
    std::uint8_t NumPedsForRoadBlock;

    // 1200
    float nBodyDirtLevel;            // Dirt level of vehicle body texture: 0.0f=fully clean, 15.0f=maximum dirt visible, it may be altered at any time while
                                     // vehicle's cycle of lige

    // values used by transmission
    std::uint8_t m_nCurrentGear;
    BYTE          bla[3];
    float         m_fGearChangeCount;

    float m_fWheelSpinForAudio;

    // 1216
    float m_health;            // 1000.0f = full health. 0 -> explode

    CVehicleSAInterface* m_towingVehicle;             // 1220
    CVehicleSAInterface* m_trailerVehicle;            // 1224

    CPedSAInterface* m_bombPlanter;                         // 1228
    std::uint32_t    m_deleteAfterTime;                     // 1232
    std::uint32_t    m_lastGunFireTime;                     // 1236
    std::uint32_t    m_lastBlowUpTime;                      // 1240
    std::uint16_t    m_policeChaseLeaveCarTimer;            // 1244
    std::uint16_t    m_delayedExplosionTimer;               // 1246
    void*            m_responsibleForDetonation;            // 1248
    float            m_frontGroundZ;                        // 1252
    float            m_rearGroundZ;                         // 1256

    /*** BEGIN SECTION that was added by us ***/
    std::uint8_t _padding1262[8];            // 1260
    CVehicle* m_pVehicle;                 // 1268
    /*** END SECTION that was added by us ***/

    // 1272
    eDoorLock m_doorLock;

    // 1276
    BYTE Padding210[24];

    // 1300
    std::uint32_t m_isUsingHornOrSecondarySiren;

    // 1304
    uint8_t Padding220[96];

    // 1400
    CFxSystemSAInterface* m_overheatParticle;
    CFxSystemSAInterface* m_fireParticle;
    CFxSystemSAInterface* m_dustParticle;
    std::uint32_t         m_renderLights;

    // 1416
    RwTexture* m_pCustomPlateTexture;

    float m_steeringLeftRight;

    // 1424
    std::uint8_t  m_vehicleClass;
    std::uint32_t m_vehicleSubClass;

    std::int16_t m_peviousRemapTxd;
    std::int16_t m_remapTxd;
    RwTexture* m_pRemapTexture;

public:
    void __thiscall SetRemap(int remap);
    int __thiscall GetRemapIndex() const;
    void __thiscall SetRemapTexDictionary(int remapDict);
};
static_assert(sizeof(CVehicleSAInterface) == 0x5A0, "Invalid size for CVehicleSAInterface");

class CAutomobileSAInterface;

class CVehicleSA : public virtual CVehicle, public virtual CPhysicalSA
{
    friend class CPoolsSA;

private:
    CDamageManagerSA*                m_pDamageManager{nullptr};
    CAEVehicleAudioEntitySA*         m_pVehicleAudioEntity{nullptr};
    CHandlingEntrySA*                m_pHandlingData{nullptr};
    CFlyingHandlingEntrySA*          m_pFlyingHandlingData{nullptr};
    void*                            m_pSuspensionLines{nullptr};
    bool                             m_bIsDerailable{true};
    std::uint8_t                     m_ucAlpha{255};
    CVector                          m_vecGravity{0.0f, 0.0f, -1.0f};
    SharedUtil::SColor               m_HeadLightColor{SharedUtil::SColorRGBA{255, 255, 255, 255}};
    SharedUtil::SColor               m_RGBColors[4];
    SharedUtil::SColor               m_RGBColorsFixed[4];
    CDoorSA                          m_doors[6];
    bool                             m_bSwingingDoorsAllowed{false};
    SSirenInfo                       m_tSirenInfo;
    std::map<SString, SVehicleFrame> m_ExtraFrames;
    std::uint8_t                     m_ucVariant;
    std::uint8_t                     m_ucVariant2;
    std::uint8_t                     m_ucVariantCount{0};
    bool                             m_doorsUndamageable{false};

    std::array<CVector, VEHICLE_DUMMY_COUNT> m_dummyPositions;

public:
    CVehicleSA() = default;
    ~CVehicleSA();

    CVehicleSAInterface* GetVehicleInterface() { return reinterpret_cast<CVehicleSAInterface*>(GetInterface()); }
    const CVehicleSAInterface* GetVehicleInterface() const noexcept {
        return reinterpret_cast<const CVehicleSAInterface*>(GetInterface());
    }

    void Init();

    // CEntitySA interface
    virtual void OnChangingPosition(const CVector& vecNewPosition);

    // Override of CPhysicalSA::SetMoveSpeed to take trains into account
    void SetMoveSpeed(CVector* vecMoveSpeed);

    bool AddProjectile(eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector* target, CEntity* targetEntity);

    CTrainSAInterface* GetNextCarriageInTrain();
    CVehicle*          GetNextTrainCarriage();
    void               SetNextTrainCarriage(CVehicle* pNext);
    CTrainSAInterface* GetPreviousCarriageInTrain();
    CVehicle*          GetPreviousTrainCarriage();
    void               SetPreviousTrainCarriage(CVehicle* pPrevious);
    float              GetDistanceToCarriage(const CVehicle* pCarriage) const;
    void               AttachTrainCarriage(CVehicle* pCarriage);
    void               DetachTrainCarriage(CVehicle* pCarriage);
    bool               IsChainEngine() const;
    void               SetIsChainEngine(bool bChainEngine = true);

    bool IsTrain() const noexcept;

    bool  IsDerailed() const;
    void  SetDerailed(bool bDerailed);
    bool  IsDerailable() const noexcept { return m_bIsDerailable; }
    void  SetDerailable(bool bDerailable) noexcept { m_bIsDerailable = bDerailable; }
    float GetTrainSpeed() const;
    void  SetTrainSpeed(float fSpeed);
    bool  GetTrainDirection() const;
    void  SetTrainDirection(bool bDirection);
    BYTE  GetRailTrack() const;
    void  SetRailTrack(BYTE ucTrackID);
    float GetTrainPosition() const;
    void  SetTrainPosition(float fPosition, bool bRecalcOnRailDistance = true);

    void AddVehicleUpgrade(DWORD dwModelID);
    void RemoveVehicleUpgrade(DWORD dwModelID);
    bool DoesSupportUpgrade(const SString& strFrameName);

    CDoorSA* GetDoor(std::uint8_t ucDoor) noexcept;
    void     OpenDoor(std::uint8_t ucDoor, float fRatio, bool bMakeNoise = false);
    void     SetSwingingDoorsAllowed(bool bAllowed);
    bool     AreSwingingDoorsAllowed() const;
    bool     AreDoorsLocked() const noexcept;
    void     LockDoors(bool bLocked);
    bool     AreDoorsUndamageable() { return m_doorsUndamageable; }
    void     SetDoorsUndamageable(bool bUndamageable) { m_doorsUndamageable = bUndamageable; }

    DWORD GetBaseVehicleType();

    void  SetBodyDirtLevel(float fDirtLevel);
    float GetBodyDirtLevel();

    std::uint8_t GetCurrentGear();
    float         GetGasPedal();

    bool GetTowBarPos(CVector* pVector, CVehicle* pTrailer = NULL);
    bool GetTowHitchPos(CVector* pVector);
    bool IsUpsideDown();

    CPed* GetDriver();
    CPed* GetPassenger(std::uint8_t ucSlot);
    bool  IsBeingDriven();

    bool IsEngineBroken() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bEngineBroken; };
    void SetEngineBroken(bool broken) noexcept { GetVehicleInterface()->m_vehicleFlags.bEngineBroken = broken; }

    void          PlaceBikeOnRoadProperly();
    void          PlaceAutomobileOnRoadProperly();
    void          SetColor(SharedUtil::SColor color1, SharedUtil::SColor color2, SharedUtil::SColor color3, SharedUtil::SColor color4, int);
    void          GetColor(SharedUtil::SColor* color1, SharedUtil::SColor* color2, SharedUtil::SColor* color3, SharedUtil::SColor* color4, bool bFixedForGTA);
    bool          IsSirenOrAlarmActive() const;
    void          SetSirenOrAlarmActive(bool bActive);
    void          SetAlpha(std::uint8_t alpha) noexcept { m_ucAlpha = alpha; }
    std::uint8_t GetAlpha() const noexcept { return m_ucAlpha; }

    void  SetLandingGearDown(bool bLandingGearDown);
    float GetLandingGearPosition() const;
    void  SetLandingGearPosition(float fPosition);
    bool  IsLandingGearDown() const;
    void  Fix();

    void BlowUp(CEntity* pCreator, unsigned long ulUnknown);

    CDamageManager* GetDamageManager();

    void      SetTowLink(CVehicle* pVehicle);
    bool      BreakTowLink();
    CVehicle* GetTowedVehicle();
    CVehicle* GetTowedByVehicle();

    void FadeOut(bool bFadeOut);
    bool IsFadingOut();

    void       SetWinchType(eWinchType winchType);
    void       PickupEntityWithWinch(CEntity* pEntity);
    void       ReleasePickedUpEntityWithWinch();
    void       SetRopeHeightForHeli(float fRopeHeight);
    CPhysical* QueryPickedUpEntityWithWinch();

    void SetRemap(int remap);
    int  GetRemapIndex() const;
    void SetRemapTexDictionary(int iRemapTextureDictionary);

    bool           IsDamaged() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bIsDamaged; };
    bool           IsDrowning() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bIsDrowning; };
    bool           IsEngineOn() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bEngineOn; };
    bool           IsHandbrakeOn() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bIsHandbrakeOn; };
    bool           IsRCVehicle() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bIsRCVehicle; };
    bool           GetAlwaysLeaveSkidMarks() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bAlwaysSkidMarks; };
    bool           GetCanBeDamaged() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bCanBeDamaged; };
    bool           GetCanBeTargettedByHeatSeekingMissiles() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bVehicleCanBeTargettedByHS; };
    bool           GetCanShootPetrolTank() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bPetrolTankIsWeakPoint; };
    bool           GetChangeColourWhenRemapping() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bDontSetColourWhenRemapping; };
    bool           GetComedyControls() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bComedyControls; };
    bool           GetGunSwitchedOff() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bGunSwitchedOff; };
    bool           GetLightsOn() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bLightsOn; };
    std::uint32_t  GetOverrideLights() const noexcept { return GetVehicleInterface()->OverrideLights; }
    bool           GetTakeLessDamage() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bTakeLessDamage; };
    bool           GetTyresDontBurst() const noexcept { return GetVehicleInterface()->m_vehicleFlags.bTyresDontBurst; };
    std::uint16_t  GetAdjustablePropertyValue() const noexcept { return *reinterpret_cast<std::uint16_t*>(reinterpret_cast<unsigned long>(m_pInterface) + 2156); };
    float          GetHeliRotorSpeed() const noexcept { return *reinterpret_cast<float*>(reinterpret_cast<std::uint32_t>(m_pInterface) + 2124); };
    float          GetPlaneRotorSpeed() const;

    unsigned long  GetExplodeTime() const noexcept { return *reinterpret_cast<unsigned long*>(reinterpret_cast<std::uint32_t>(m_pInterface) + 1240); };

    char  GetNitroCount() const noexcept { return GetVehicleInterface()->m_nNitroBoosts; }
    float GetNitroLevel();

    void SetAlwaysLeaveSkidMarks(bool bAlwaysLeaveSkidMarks) { GetVehicleInterface()->m_vehicleFlags.bAlwaysSkidMarks = bAlwaysLeaveSkidMarks; };
    void SetCanBeDamaged(bool bCanBeDamaged) { GetVehicleInterface()->m_vehicleFlags.bCanBeDamaged = bCanBeDamaged; };
    void SetCanBeTargettedByHeatSeekingMissiles(bool bEnabled) { GetVehicleInterface()->m_vehicleFlags.bVehicleCanBeTargettedByHS = bEnabled; };
    void SetCanShootPetrolTank(bool bCanShoot) { GetVehicleInterface()->m_vehicleFlags.bPetrolTankIsWeakPoint = bCanShoot; };
    void SetChangeColourWhenRemapping(bool bChangeColour) { GetVehicleInterface()->m_vehicleFlags.bDontSetColourWhenRemapping; };
    void SetComedyControls(bool bComedyControls) { GetVehicleInterface()->m_vehicleFlags.bComedyControls = bComedyControls; };
    void SetEngineOn(bool bEngineOn);
    void SetGunSwitchedOff(bool bGunsOff) { GetVehicleInterface()->m_vehicleFlags.bGunSwitchedOff = bGunsOff; };
    void SetHandbrakeOn(bool bHandbrakeOn) { GetVehicleInterface()->m_vehicleFlags.bIsHandbrakeOn = bHandbrakeOn; };
    void SetLightsOn(bool bLightsOn) { GetVehicleInterface()->m_vehicleFlags.bLightsOn = bLightsOn; };
    void SetOverrideLights(std::uint32_t uiOverrideLights) { GetVehicleInterface()->OverrideLights = uiOverrideLights; }
    void SetTaxiLightOn(bool bLightOn);
    void SetTakeLessDamage(bool bTakeLessDamage) { GetVehicleInterface()->m_vehicleFlags.bTakeLessDamage = bTakeLessDamage; };
    void SetTyresDontBurst(bool bTyresDontBurst) { GetVehicleInterface()->m_vehicleFlags.bTyresDontBurst = bTyresDontBurst; };
    void SetAdjustablePropertyValue(std::uint16_t usAdjustableProperty)
    {
        *reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uint32_t>(m_pInterface) + 2156) = usAdjustableProperty;
    };
    void SetHeliRotorSpeed(float fSpeed) { *reinterpret_cast<float*>(reinterpret_cast<std::uint32_t>(m_pInterface) + 2124) = fSpeed; };
    void SetPlaneRotorSpeed(float fSpeed);
    bool SetVehicleWheelRotation(float fWheelRot1, float fWheelRot2, float fWheelRot3, float fWheelRot4) noexcept;
    void SetExplodeTime(unsigned long ulTime) { *reinterpret_cast<unsigned long*>(reinterpret_cast<std::uint32_t>(m_pInterface) + 1240) = ulTime; };
    void SetRadioStatus(bool bStatus) { *reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint32_t>(m_pInterface) + 0x1D3) = bStatus; };

    void SetNitroCount(char cNitroCount) { GetVehicleInterface()->m_nNitroBoosts = cNitroCount; }
    void SetNitroLevel(float fLevel);

    float GetHealth();
    void  SetHealth(float fHealth);

    void GetTurretRotation(float* fHorizontal, float* fVertical);
    void SetTurretRotation(float fHorizontal, float fVertical);

    bool IsSmokeTrailEnabled();
    void SetSmokeTrailEnabled(bool bEnabled);

    CHandlingEntry* GetHandlingData();
    void            SetHandlingData(CHandlingEntry* pHandling);

    CFlyingHandlingEntry* GetFlyingHandlingData();
    void                  SetFlyingHandlingData(CFlyingHandlingEntry* pHandling);

    void BurstTyre(BYTE bTyre);

    BYTE GetBikeWheelStatus(BYTE bWheel);
    void SetBikeWheelStatus(BYTE bWheel, BYTE bStatus);

    bool IsWheelCollided(BYTE eWheelPosition);
    int  GetWheelFrictionState(BYTE eWheelPosition);

    void GetGravity(CVector* pvecGravity) const { *pvecGravity = m_vecGravity; }
    void SetGravity(const CVector* pvecGravity);

    SharedUtil::SColor GetHeadLightColor() { return m_HeadLightColor; }
    void               SetHeadLightColor(const SharedUtil::SColor color) { m_HeadLightColor = color; }

    CObject* SpawnFlyingComponent(int i_1, std::uint32_t ui_2);
    void     SetWheelVisibility(eWheelPosition wheel, bool bVisible);
    CVector  GetWheelPosition(eWheelPosition wheel);

    bool IsHeliSearchLightVisible();
    void SetHeliSearchLightVisible(bool bVisible);

    bool       UpdateMovingCollision(float fAngle);

    void RecalculateHandling();

    void* GetPrivateSuspensionLines();

    bool DoesVehicleHaveSirens() { return m_tSirenInfo.m_bOverrideSirens; }

    void GiveVehicleSirens(std::uint8_t ucSirenType, std::uint8_t ucSirenCount);
    void RemoveVehicleSirens() { m_tSirenInfo.m_bOverrideSirens = false; }
    void SetVehicleSirenMinimumAlpha(std::uint8_t ucSirenCount, DWORD dwPercentage)
    {
        m_tSirenInfo.m_tSirenInfo[ucSirenCount].m_dwMinSirenAlpha = dwPercentage;
    }
    void               SetVehicleSirenPosition(std::uint8_t ucSirenID, CVector vecPos);
    void               GetVehicleSirenPosition(std::uint8_t ucSirenID, CVector& vecPos);
    std::uint8_t      GetVehicleSirenCount() { return m_tSirenInfo.m_ucSirenCount; }
    std::uint8_t      GetVehicleSirenType() { return m_tSirenInfo.m_ucSirenType; }
    DWORD              GetVehicleSirenMinimumAlpha(std::uint8_t ucSirenID) { return m_tSirenInfo.m_tSirenInfo[ucSirenID].m_dwMinSirenAlpha; }
    SharedUtil::SColor GetVehicleSirenColour(std::uint8_t ucSirenID) { return m_tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour; }
    void               SetVehicleSirenColour(std::uint8_t ucSirenID, SharedUtil::SColor tVehicleSirenColour)
    {
        m_tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour = tVehicleSirenColour;
    }
    void                              SetVehicleCurrentSirenID(std::uint8_t ucCurrentSirenID) { m_tSirenInfo.m_ucCurrentSirenID = ucCurrentSirenID; }
    std::uint8_t                     GetVehicleCurrentSirenID() { return m_tSirenInfo.m_ucCurrentSirenID; }
    std::uint8_t                     GetSirenRandomiser() { return m_tSirenInfo.m_ucCurrentSirenRandomiser; }
    void                              SetSirenRandomiser(std::uint8_t ucSirenRandomiser) { m_tSirenInfo.m_ucCurrentSirenRandomiser = ucSirenRandomiser; }
    void                              SetPointLightColour(SharedUtil::SColor tPointLightColour) { m_tSirenInfo.m_tPointLightColour = tPointLightColour; }
    SharedUtil::SColor                GetPointLightColour() { return m_tSirenInfo.m_tPointLightColour; }
    bool                              IsSiren360EffectEnabled() { return m_tSirenInfo.m_b360Flag; }
    bool                              IsSirenLOSCheckEnabled() { return m_tSirenInfo.m_bDoLOSCheck; }
    bool                              IsSirenRandomiserEnabled() { return m_tSirenInfo.m_bUseRandomiser; }
    bool                              IsSirenSilentEffectEnabled() { return m_tSirenInfo.m_bSirenSilent; }
    void                              SetVehicleFlags(bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent);
    bool                              SetComponentRotation(const SString& vehicleComponent, const CVector& vecRotation);
    bool                              GetComponentRotation(const SString& vehicleComponent, CVector& vecPositionModelling);
    bool                              SetComponentPosition(const SString& vehicleComponent, const CVector& vecPosition);
    bool                              GetComponentPosition(const SString& vehicleComponent, CVector& vecPositionModelling);
    bool                              SetComponentScale(const SString& vehicleComponent, const CVector& vecScale);
    bool                              GetComponentScale(const SString& vehicleComponent, CVector& vecScaleModelling);
    bool                              IsComponentPresent(const SString& vehicleComponent);
    bool                              SetComponentMatrix(const SString& vehicleComponent, const CMatrix& matOrientation);
    bool                              GetComponentMatrix(const SString& vehicleComponent, CMatrix& matOutOrientation);
    bool                              GetComponentParentToRootMatrix(const SString& vehicleComponent, CMatrix& matOutParentToRoot);
    bool                              SetComponentVisible(const SString& vehicleComponent, bool bVisible);
    void                              AddComponent(RwFrame* pFrame, bool bReadOnly);
    bool                              GetComponentVisible(const SString& vehicleComponent, bool& bVisible);
    std::map<SString, SVehicleFrame>& GetComponentMap() { return m_ExtraFrames; }
    bool                              SetPlateText(const SString& strText);
    bool                              SetWindowOpenFlagState(std::uint8_t ucWindow, bool bState);
    float                             GetWheelScale() const override { return GetVehicleInterface()->m_wheelScale; }
    void                              SetWheelScale(float fWheelScale) override { GetVehicleInterface()->m_wheelScale = fWheelScale; }

    void UpdateLandingGearPosition();

    CAEVehicleAudioEntitySA* GetVehicleAudioEntity() { return m_pVehicleAudioEntity; };

    bool GetDummyPosition(eVehicleDummies dummy, CVector& position) const override;
    bool SetDummyPosition(eVehicleDummies dummy, const CVector& position) override;

    CVector*       GetDummyPositions() { return m_dummyPositions.data(); }
    const CVector* GetDummyPositions() const override { return m_dummyPositions.data(); }

    static void StaticSetHooks();
    static void SetVehiclesSunGlareEnabled(bool bEnabled);
    static bool GetVehiclesSunGlareEnabled();

private:
    static void SetAutomobileDummyPosition(CAutomobileSAInterface* automobile, eVehicleDummies dummy, const CVector& position);

    void           RecalculateSuspensionLines();
    void           CopyGlobalSuspensionLinesToPrivate();
    SVehicleFrame* GetVehicleComponent(const SString& vehicleComponent);
    void           FinalizeFramesList();
    void           DumpVehicleFrames();
};
