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

class CFxSystemSAInterface;
class CTrainSAInterface;
struct RwTexture;

#define SIZEOF_CHELI                            2584

#define FUNC_CCarCtrlCreateCarForScript         0x431f80
#define FUNC_CVehicle_AddVehicleUpgrade         0x6E3290
#define FUNC_CVehicle_RemoveVehicleUpgrade      0x6DF930
#define FUNC_CVehicle_GetBaseVehicleType        0x411D50
#define FUNC_CVehicle_IsUpsideDown              0x6D1D90
#define FUNC_CVehicle_SetEngineOn               0x41BDD0
#define FUNC_CTrain_FindPositionOnTrackFromCoors           0x6F6CC0

#define FUNC_CVehicle_QueryPickedUpEntityWithWinch              0x6d3cf0
#define FUNC_CVehicle_PickUpEntityWithWinch                     0x6d3cd0
#define FUNC_CVehicle_ReleasePickedUpEntityWithWinch            0x6d3cb0
#define FUNC_CVehicle_SetRopeHeightForHeli                      0x6d3d30

#define FUNC_CVehicle__SetRemapTexDictionary                    0x6D0BC0
#define FUNC_CVehicle__GetRemapIndex                            0x6D0B70
#define FUNC_CVehicle__SetRemap                                 0x6D0C00
#define FUNC_CVehicle_CustomCarPlate_TextureCreate              0x6D10E0

#define FUNC_CVehicle_SetWindowOpenFlag                         0x6D3080
#define FUNC_CVehicle_ClearWindowOpenFlag                       0x6D30B0

// from CBike
#define FUNC_Bike_PlaceOnRoadProperly           0x6BEEB0
#define FUNC_Automobile_PlaceOnRoadProperly     0x6AF420
#define FUNC_CBike_Fix                          0x6B7050

// from CPlane
#define FUNC_CPlane__Fix                        0x6CABB0

// from CAutomobile
#define FUNC_CAutomobile__Fix                   0x6A3440
#define FUNC_CAutomobile__SpawnFlyingComponent  0x6a8580
#define FUNC_CAutomobile__UpdateMovingCollision 0x6a1460

// from CHeli
#define FUNC_CHeli__Fix                         0x6C4530

// from CQuadBike
#define FUNC_CQuadBike__Fix                     0x6CE2B0

#define VARS_CarCounts                          0x969094 // 5 values for each vehicle type

// Used when deleting vehicles
#define VTBL_CPlaceable                         0x863C40

#define MAX_PASSENGERS                          8

#define NUM_RAILTRACKS                          4
#define ARRAY_NumRailTrackNodes                 0xC38014    // NUM_RAILTRACKS dwords
#define ARRAY_RailTrackNodePointers             0xC38024    // NUM_RAILTRACKS pointers to arrays of SRailNode

#define VAR_CVehicle_SpecialColModels           0xc1cc78

#define FUNC_CAutomobile__GetDoorAngleOpenRatio 0x6A2270
#define FUNC_CTrain__GetDoorAngleOpenRatio      0x6F59C0

#define HANDLING_NOS_Flag                       0x00080000
#define HANDLING_Hydraulics_Flag                0x00020000

#define VAR_CVehicle_Variation1                 0x8A6458
#define VAR_CVehicle_Variation2                 0x8A6459

// for vehicle sun glare
#define FUNC_CAutomobile_OnVehiclePreRender 0x6ABCFD
#define FUNC_CVehicle_DoSunGlare            0x6DD6F0

// CClumpModelInfo::GetFrameFromName
#define FUNC_CClumpModelInfo_GetFrameFromName 0x4C5400

// CAutomobile::m_aCarNodes
// CTrain::m_aTrainNodes
// CBike::m_apModelNodes
// CBoat::pBoatParts
#define OFFSET_CAutomobile_Nodes 0x648
#define OFFSET_CTrain_Nodes 0x668
#define OFFSET_CBike_Nodes 0x5A0
#define OFFSET_CBoat_Nodes 0x5B0

struct SRailNodeSA
{
    short sX;                       // x coordinate times 8
    short sY;                       // y coordinate times 8
    short sZ;                       // z coordinate times 8
    WORD  sRailDistance;            // on-rail distance times 3.33333334
    WORD  padding;
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
    unsigned char bIsLawEnforcer : 1;                // Is this guy chasing the player at the moment
    unsigned char bIsAmbulanceOnDuty : 1;            // Ambulance trying to get to an accident
    unsigned char bIsFireTruckOnDuty : 1;            // Firetruck trying to get to a fire
    unsigned char bIsLocked : 1;                     // Is this guy locked by the script (cannot be removed)
    unsigned char bEngineOn : 1;                     // For sound purposes. Parked cars have their engines switched off (so do destroyed cars)
    unsigned char bIsHandbrakeOn : 1;                // How's the handbrake doing ?
    unsigned char bLightsOn : 1;                     // Are the lights switched on ?
    unsigned char bFreebies : 1;                     // Any freebies left in this vehicle ?

    unsigned char bIsVan : 1;                       // Is this vehicle a van (doors at back of vehicle)
    unsigned char bIsBus : 1;                       // Is this vehicle a bus
    unsigned char bIsBig : 1;                       // Is this vehicle a bus
    unsigned char bLowVehicle : 1;                  // Need this for sporty type cars to use low getting-in/out anims
    unsigned char bComedyControls : 1;              // Will make the car hard to control (hopefully in a funny way)
    unsigned char bWarnedPeds : 1;                  // Has scan and warn peds of danger been processed?
    unsigned char bCraneMessageDone : 1;            // A crane message has been printed for this car allready
    // unsigned char bExtendedRange: 1; // This vehicle needs to be a bit further away to get deleted
    unsigned char bTakeLessDamage : 1;            // This vehicle is stronger (takes about 1/4 of damage)

    unsigned char bIsDamaged : 1;                       // This vehicle has been damaged and is displaying all its components
    unsigned char bHasBeenOwnedByPlayer : 1;            // To work out whether stealing it is a crime
    unsigned char bFadeOut : 1;                         // Fade vehicle out
    unsigned char bIsBeingCarJacked : 1;                // Fade vehicle out
    unsigned char bCreateRoadBlockPeds : 1;             // If this vehicle gets close enough we will create peds (coppers or gang members) round it
    unsigned char bCanBeDamaged : 1;                    // Set to FALSE during cut scenes to avoid explosions
    // unsigned char bUsingSpecialColModel : 1;
    // Is player vehicle using special collision model, stored in player strucure
    unsigned char bOccupantsHaveBeenGenerated : 1;            // Is true if the occupants have already been generated. (Shouldn't happen again)
    unsigned char bGunSwitchedOff : 1;                        // Level designers can use this to switch off guns on boats

    unsigned char bVehicleColProcessed : 1;               // Has ProcessEntityCollision been processed for this car?
    unsigned char bIsCarParkVehicle : 1;                  // Car has been created using the special CAR_PARK script command
    unsigned char bHasAlreadyBeenRecorded : 1;            // Used for replays
    unsigned char bPartOfConvoy : 1;
    unsigned char bHeliMinimumTilt : 1;              // This heli should have almost no tilt really
    unsigned char bAudioChangingGear : 1;            // sounds like vehicle is changing gear
    unsigned char bIsDrowning : 1;                   // is vehicle occupants taking damage in water (i.e. vehicle is dead in water)
    unsigned char bTyresDontBurst : 1;               // If this is set the tyres are invincible

    unsigned char bCreatedAsPoliceVehicle : 1;            // True if this guy was created as a police vehicle (enforcer, policecar, miamivice car etc)
    unsigned char bRestingOnPhysical : 1;                 // Dont go static cause car is sitting on a physical object that might get removed
    unsigned char bParking : 1;
    unsigned char bCanPark : 1;
    unsigned char bFireGun : 1;                                // Does the ai of this vehicle want to fire it's gun?
    unsigned char bDriverLastFrame : 1;                        // Was there a driver present last frame ?
    unsigned char bNeverUseSmallerRemovalRange : 1;            // Some vehicles (like planes) we don't want to remove just behind the camera.
    unsigned char bIsRCVehicle : 1;                            // Is this a remote controlled (small) vehicle. True whether the player or AI controls it.

    unsigned char bAlwaysSkidMarks : 1;                      // This vehicle leaves skidmarks regardless of the wheels' states.
    unsigned char bEngineBroken : 1;                         // Engine doesn't work. Player can get in but the vehicle won't drive
    unsigned char bVehicleCanBeTargetted : 1;                // The ped driving this vehicle can be targetted, (for Torenos plane mission)
    unsigned char bPartOfAttackWave : 1;                     // This car is used in an attack during a gang war
    unsigned char bWinchCanPickMeUp : 1;                     // This car cannot be picked up by any ropes.
    unsigned char bImpounded : 1;                            // Has this vehicle been in a police impounding garage
    unsigned char bVehicleCanBeTargettedByHS : 1;            // Heat seeking missiles will not target this vehicle.
    unsigned char bSirenOrAlarm : 1;                         // Set to TRUE if siren or alarm active, else FALSE

    unsigned char bHasGangLeaningOn : 1;
    unsigned char bGangMembersForRoadBlock : 1;            // Will generate gang members if NumPedsForRoadBlock > 0
    unsigned char bDoesProvideCover : 1;                   // If this is false this particular vehicle can not be used to take cover behind.
    unsigned char bMadDriver : 1;                          // This vehicle is driving like a lunatic
    unsigned char bUpgradedStereo : 1;                     // This vehicle has an upgraded stereo
    unsigned char bConsideredByPlayer : 1;                 // This vehicle is considered by the player to enter
    unsigned char bPetrolTankIsWeakPoint : 1;              // If false shootong the petrol tank will NOT Blow up the car
    unsigned char bDisableParticles : 1;                   // Disable particles from this car. Used in garage.

    unsigned char bHasBeenResprayed : 1;                      // Has been resprayed in a respray garage. Reset after it has been checked.
    unsigned char bUseCarCheats : 1;                          // If this is true will set the car cheat stuff up in ProcessControl()
    unsigned char bDontSetColourWhenRemapping : 1;            // If the texture gets remapped we don't want to change the colour with it.
    unsigned char bUsedForReplay : 1;                         // This car is controlled by replay and should be removed when replay is done.
};

struct CTransmissionGear
{
    float maxGearVelocity;
    float changeUpVelocity;
    float changeDownVelocity;
};

struct CTransmission
{
    CTransmissionGear gears[6];
    byte              driveType;
    byte              engineType;
    byte              numOfGears;
    byte              pad;
    DWORD             handlingFlags;
    float             engineAcceleration;
    float             engineInertia;
    float             maxGearVelocity;
    byte              pad2[4];
    float             minGearVelocity;
    float             curSpeed;
};

class CAutoPilot
{
    BYTE pad[56];
};

#define MAX_UPGRADES_ATTACHED 15 // perhaps?

class CVehicleSAInterface : public CPhysicalSAInterface
{
public:
    CAEVehicleAudioEntitySAInterface m_VehicleAudioEntity;            // 312

    tHandlingDataSA*       pHandlingData;                  // +900
    tFlyingHandlingDataSA* pFlyingHandlingData;            // +904
    DWORD                  dwHandlingFlags;                // +908
    int                    pad52321[21];

    DWORD        dwUnknown1201;            // +996
    DWORD        dwUnknown1202;            // +1000
    unsigned int hFlagsLocal;              // +1004

    CAutoPilot    AutoPilot;                   // +1008
    CVehicleFlags m_nVehicleFlags;             // +1064?
    unsigned int  m_TimeOfCreation;            // GetTimeInMilliseconds when this vehicle was created.

    unsigned char m_colour1, m_colour2, m_colour3, m_colour4;
    char          m_comp1, m_comp2;
    short         m_upgrades[MAX_UPGRADES_ATTACHED];            // 1082
    float         m_fWheelScale;                                // 1112

    unsigned short CarAlarmState;               // 1116
    unsigned short ForcedRandomSeed;            // if this is non-zero the random wander gets deterministic

    CPedSAInterface* pDriver;
    CPedSAInterface* pPassengers[MAX_PASSENGERS];
    unsigned char    m_nNumPassengers;
    unsigned char    m_nNumGettingIn;
    unsigned char    m_nGettingInFlags;
    unsigned char    m_nGettingOutFlags;
    unsigned char    m_nMaxPassengers;
    unsigned char    m_windowsOpenFlags;
    char             m_nNitroBoosts;            // +1162
    // float m_fNitroTimer; // +2212

    unsigned char m_nSpecialColModel;
    CEntity*      pEntityWeAreOnForVisibilityCheck;
    CFire*        m_pFire;

    float m_fSteerAngle;               // +1172
    float m_f2ndSteerAngle;            // used for steering 2nd set of wheels or elevators etc..
    float m_fGasPedal;                 // 0...1  // +1180
    float m_fBrakePedal;               // 0...1

    unsigned char VehicleCreatedBy;            // Contains information on whether this vehicle can be deleted
                                               // or not. Probably only need this or IsLocked.
    short ExtendedRemovalRange;

    unsigned char BombOnBoard : 3;            // 0 = None. 1 = Timed. 2 = On ignition, 3 = remotely set ? 4 = Timed Bomb has been activated. 5 = On ignition has
                                              // been activated.
    unsigned char OverrideLights : 2;                  // uses enum NO_CAR_LIGHT_OVERRIDE, FORCE_CAR_LIGHTS_OFF, FORCE_CAR_LIGHTS_ON
    unsigned char WinchType : 2;                       // Does this vehicle use a winch?
    unsigned char m_GunsCycleIndex : 2;                // Cycle through alternate gun hardpoints on planes/helis
    unsigned char m_OrdnanceCycleIndex : 2;            // Cycle through alternate ordnance hardpoints on planes/helis

    unsigned char nUsedForCover;            // Has n number of cops hiding/attempting to hid behind it
    unsigned char AmmoInClip;               // Used to make the guns on boat do a reload.
    unsigned char PacMansCollected;
    unsigned char PedsPositionForRoadBlock;
    unsigned char NumPedsForRoadBlock;

    // 1200
    float nBodyDirtLevel;            // Dirt level of vehicle body texture: 0.0f=fully clean, 15.0f=maximum dirt visible, it may be altered at any time while
                                     // vehicle's cycle of lige

    // values used by transmission
    unsigned char m_nCurrentGear;
    BYTE          bla[3];
    float         m_fGearChangeCount;

    float m_fWheelSpinForAudio;

    // 1216
    float m_nHealth;            // 1000.0f = full health. 0 -> explode

    CVehicleSAInterface* m_towingVehicle;             // 1220
    CVehicleSAInterface* m_trailerVehicle;            // 1224

    CPedSAInterface* m_bombPlanter;                         // 1228
    uint32_t         m_deleteAfterTime;                     // 1232
    uint32_t         m_lastGunFireTime;                     // 1236
    uint32_t         m_lastBlowUpTime;                      // 1240
    uint16_t         m_policeChaseLeaveCarTimer;            // 1244
    uint16_t         m_delayedExplosionTimer;               // 1246
    void*            m_responsibleForDetonation;            // 1248
    float            m_frontGroundZ;                        // 1252
    float            m_rearGroundZ;                         // 1256

    /*** BEGIN SECTION that was added by us ***/
    uint8_t   _padding1262[8];            // 1260
    CVehicle* m_pVehicle;                 // 1268
    /*** END SECTION that was added by us ***/

    // 1272
    eDoorLock m_doorLock;

    // 1276
    BYTE Padding210[24];

    // 1300
    unsigned int m_isUsingHornOrSecondarySiren;

    // 1304
    uint8_t Padding220[96];

    // 1400
    CFxSystemSAInterface* m_overheatParticle;
    CFxSystemSAInterface* m_fireParticle;
    CFxSystemSAInterface* m_dustParticle;
    uint32_t              m_renderLights;

    // 1416
    RwTexture* m_pCustomPlateTexture;

    float m_steeringLeftRight;

    // 1424
    uint8_t  m_vehicleClass;
    uint32_t m_vehicleSubClass;

    int16_t    m_peviousRemapTxd;
    int16_t    m_remapTxd;
    RwTexture* m_pRemapTexture;
};
static_assert(sizeof(CVehicleSAInterface) == 1440, "Invalid size for CVehicleSAInterface");

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
    unsigned char                    m_ucAlpha{255};
    CVector                          m_vecGravity{0.0f, 0.0f, -1.0f};
    SharedUtil::SColor               m_HeadLightColor{SharedUtil::SColorRGBA{255, 255, 255, 255}};
    SharedUtil::SColor               m_RGBColors[4];
    SharedUtil::SColor               m_RGBColorsFixed[4];
    CDoorSA                          m_doors[6];
    bool                             m_bSwingingDoorsAllowed{false};
    SSirenInfo                       m_tSirenInfo;
    std::map<SString, SVehicleFrame> m_ExtraFrames;
    unsigned char                    m_ucVariant;
    unsigned char                    m_ucVariant2;
    unsigned char                    m_ucVariantCount{0};
    bool                             m_doorsUndamageable{false};

    std::array<CVector, VEHICLE_DUMMY_COUNT> m_dummyPositions;

public:
    CVehicleSA() = default;
    ~CVehicleSA();

    CVehicleSAInterface* GetVehicleInterface() { return reinterpret_cast<CVehicleSAInterface*>(GetInterface()); }

    void Init();

    // CEntitySA interface
    virtual void OnChangingPosition(const CVector& vecNewPosition);

    // Override of CPhysicalSA::SetMoveSpeed to take trains into account
    void SetMoveSpeed(const CVector& vecMoveSpeed) noexcept;

    bool AddProjectile(eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector* target, CEntity* targetEntity);

    CTrainSAInterface* GetNextCarriageInTrain();
    CVehicle*          GetNextTrainCarriage();
    void               SetNextTrainCarriage(CVehicle* pNext);
    CTrainSAInterface* GetPreviousCarriageInTrain();
    CVehicle*          GetPreviousTrainCarriage();
    void               SetPreviousTrainCarriage(CVehicle* pPrevious);
    float              GetDistanceToCarriage(CVehicle* pCarriage);
    void               AttachTrainCarriage(CVehicle* pCarriage);
    void               DetachTrainCarriage(CVehicle* pCarriage);
    bool               IsChainEngine();
    void               SetIsChainEngine(bool bChainEngine = true);

    bool  IsDerailed();
    void  SetDerailed(bool bDerailed);
    bool  IsDerailable() { return m_bIsDerailable; }
    void  SetDerailable(bool bDerailable) { m_bIsDerailable = bDerailable; }
    float GetTrainSpeed();
    void  SetTrainSpeed(float fSpeed);
    bool  GetTrainDirection();
    void  SetTrainDirection(bool bDirection);
    BYTE  GetRailTrack();
    void  SetRailTrack(BYTE ucTrackID);
    float GetTrainPosition();
    void  SetTrainPosition(float fPosition, bool bRecalcOnRailDistance = true);

    void AddVehicleUpgrade(DWORD dwModelID);
    void RemoveVehicleUpgrade(DWORD dwModelID);
    bool DoesSupportUpgrade(const SString& strFrameName);

    CDoorSA* GetDoor(unsigned char ucDoor);
    void     OpenDoor(unsigned char ucDoor, float fRatio, bool bMakeNoise = false);
    void     SetSwingingDoorsAllowed(bool bAllowed);
    bool     AreSwingingDoorsAllowed() const;
    bool     AreDoorsLocked();
    void     LockDoors(bool bLocked);
    bool     AreDoorsUndamageable() { return m_doorsUndamageable; }
    void     SetDoorsUndamageable(bool bUndamageable) { m_doorsUndamageable = bUndamageable; }

    DWORD GetBaseVehicleType();

    void  SetBodyDirtLevel(float fDirtLevel);
    float GetBodyDirtLevel();

    unsigned char GetCurrentGear();
    float         GetGasPedal();

    bool GetTowBarPos(CVector* pVector, CVehicle* pTrailer = NULL);
    bool GetTowHitchPos(CVector* pVector);
    bool IsUpsideDown();

    CPed* GetDriver();
    CPed* GetPassenger(unsigned char ucSlot);
    bool  IsBeingDriven();

    bool IsEngineBroken() { return GetVehicleInterface()->m_nVehicleFlags.bEngineBroken; };
    void SetEngineBroken(bool bEngineBroken) { GetVehicleInterface()->m_nVehicleFlags.bEngineBroken = bEngineBroken; }

    void          PlaceBikeOnRoadProperly();
    void          PlaceAutomobileOnRoadProperly();
    void          SetColor(SharedUtil::SColor color1, SharedUtil::SColor color2, SharedUtil::SColor color3, SharedUtil::SColor color4, int);
    void          GetColor(SharedUtil::SColor* color1, SharedUtil::SColor* color2, SharedUtil::SColor* color3, SharedUtil::SColor* color4, bool bFixedForGTA);
    bool          IsSirenOrAlarmActive();
    void          SetSirenOrAlarmActive(bool bActive);
    void          SetAlpha(unsigned char ucAlpha) { m_ucAlpha = ucAlpha; }
    unsigned char GetAlpha() { return m_ucAlpha; }

    void  SetLandingGearDown(bool bLandingGearDown);
    float GetLandingGearPosition();
    void  SetLandingGearPosition(float fPosition);
    bool  IsLandingGearDown();
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

    void SetRemap(int iRemap);
    int  GetRemapIndex();
    void SetRemapTexDictionary(int iRemapTextureDictionary);

    bool           IsDamaged() { return GetVehicleInterface()->m_nVehicleFlags.bIsDamaged; };
    bool           IsDrowning() { return GetVehicleInterface()->m_nVehicleFlags.bIsDrowning; };
    bool           IsEngineOn() { return GetVehicleInterface()->m_nVehicleFlags.bEngineOn; };
    bool           IsHandbrakeOn() { return GetVehicleInterface()->m_nVehicleFlags.bIsHandbrakeOn; };
    bool           IsRCVehicle() { return GetVehicleInterface()->m_nVehicleFlags.bIsRCVehicle; };
    bool           GetAlwaysLeaveSkidMarks() { return GetVehicleInterface()->m_nVehicleFlags.bAlwaysSkidMarks; };
    bool           GetCanBeDamaged() { return GetVehicleInterface()->m_nVehicleFlags.bCanBeDamaged; };
    bool           GetCanBeTargettedByHeatSeekingMissiles() { return GetVehicleInterface()->m_nVehicleFlags.bVehicleCanBeTargettedByHS; };
    bool           GetCanShootPetrolTank() { return GetVehicleInterface()->m_nVehicleFlags.bPetrolTankIsWeakPoint; };
    bool           GetChangeColourWhenRemapping() { return GetVehicleInterface()->m_nVehicleFlags.bDontSetColourWhenRemapping; };
    bool           GetComedyControls() { return GetVehicleInterface()->m_nVehicleFlags.bComedyControls; };
    bool           GetGunSwitchedOff() { return GetVehicleInterface()->m_nVehicleFlags.bGunSwitchedOff; };
    bool           GetLightsOn() { return GetVehicleInterface()->m_nVehicleFlags.bLightsOn; };
    unsigned int   GetOverrideLights() { return GetVehicleInterface()->OverrideLights; }
    bool           GetTakeLessDamage() { return GetVehicleInterface()->m_nVehicleFlags.bTakeLessDamage; };
    bool           GetTyresDontBurst() { return GetVehicleInterface()->m_nVehicleFlags.bTyresDontBurst; };
    unsigned short GetAdjustablePropertyValue() { return *reinterpret_cast<unsigned short*>(reinterpret_cast<unsigned long>(m_pInterface) + 2156); };
    float          GetHeliRotorSpeed() { return *reinterpret_cast<float*>(reinterpret_cast<unsigned int>(m_pInterface) + 2124); };
    float          GetPlaneRotorSpeed();

    unsigned long  GetExplodeTime() { return *reinterpret_cast<unsigned long*>(reinterpret_cast<unsigned int>(m_pInterface) + 1240); };

    char  GetNitroCount() { return GetVehicleInterface()->m_nNitroBoosts; }
    float GetNitroLevel();

    void SetAlwaysLeaveSkidMarks(bool bAlwaysLeaveSkidMarks) { GetVehicleInterface()->m_nVehicleFlags.bAlwaysSkidMarks = bAlwaysLeaveSkidMarks; };
    void SetCanBeDamaged(bool bCanBeDamaged) { GetVehicleInterface()->m_nVehicleFlags.bCanBeDamaged = bCanBeDamaged; };
    void SetCanBeTargettedByHeatSeekingMissiles(bool bEnabled) { GetVehicleInterface()->m_nVehicleFlags.bVehicleCanBeTargettedByHS = bEnabled; };
    void SetCanShootPetrolTank(bool bCanShoot) { GetVehicleInterface()->m_nVehicleFlags.bPetrolTankIsWeakPoint = bCanShoot; };
    void SetChangeColourWhenRemapping(bool bChangeColour) { GetVehicleInterface()->m_nVehicleFlags.bDontSetColourWhenRemapping; };
    void SetComedyControls(bool bComedyControls) { GetVehicleInterface()->m_nVehicleFlags.bComedyControls = bComedyControls; };
    void SetEngineOn(bool bEngineOn);
    void SetGunSwitchedOff(bool bGunsOff) { GetVehicleInterface()->m_nVehicleFlags.bGunSwitchedOff = bGunsOff; };
    void SetHandbrakeOn(bool bHandbrakeOn) { GetVehicleInterface()->m_nVehicleFlags.bIsHandbrakeOn = bHandbrakeOn; };
    void SetLightsOn(bool bLightsOn) { GetVehicleInterface()->m_nVehicleFlags.bLightsOn = bLightsOn; };
    void SetOverrideLights(unsigned int uiOverrideLights) { GetVehicleInterface()->OverrideLights = uiOverrideLights; }
    void SetTaxiLightOn(bool bLightOn);
    void SetTakeLessDamage(bool bTakeLessDamage) { GetVehicleInterface()->m_nVehicleFlags.bTakeLessDamage = bTakeLessDamage; };
    void SetTyresDontBurst(bool bTyresDontBurst) { GetVehicleInterface()->m_nVehicleFlags.bTyresDontBurst = bTyresDontBurst; };
    void SetAdjustablePropertyValue(unsigned short usAdjustableProperty)
    {
        *reinterpret_cast<unsigned short*>(reinterpret_cast<unsigned int>(m_pInterface) + 2156) = usAdjustableProperty;
    };
    void SetHeliRotorSpeed(float fSpeed) { *reinterpret_cast<float*>(reinterpret_cast<unsigned int>(m_pInterface) + 2124) = fSpeed; };
    void SetPlaneRotorSpeed(float fSpeed);
    bool SetVehicleWheelRotation(float fWheelRot1, float fWheelRot2, float fWheelRot3, float fWheelRot4) noexcept;
    void SetExplodeTime(unsigned long ulTime) { *reinterpret_cast<unsigned long*>(reinterpret_cast<unsigned int>(m_pInterface) + 1240) = ulTime; };
    void SetRadioStatus(bool bStatus) { *reinterpret_cast<unsigned char*>(reinterpret_cast<unsigned int>(m_pInterface) + 0x1D3) = bStatus; };

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

    bool     SpawnFlyingComponent(const eCarNodes& nodeIndex, const eCarComponentCollisionTypes& collisionType, std::int32_t removalTime = -1);
    void     SetWheelVisibility(eWheelPosition wheel, bool bVisible);
    CVector  GetWheelPosition(eWheelPosition wheel);

    bool IsHeliSearchLightVisible();
    void SetHeliSearchLightVisible(bool bVisible);

    CColModel* GetSpecialColModel();
    bool       UpdateMovingCollision(float fAngle);

    void RecalculateHandling();

    void* GetPrivateSuspensionLines();

    bool DoesVehicleHaveSirens() { return m_tSirenInfo.m_bOverrideSirens; }

    void GiveVehicleSirens(unsigned char ucSirenType, unsigned char ucSirenCount);
    void RemoveVehicleSirens() { m_tSirenInfo.m_bOverrideSirens = false; }
    void SetVehicleSirenMinimumAlpha(unsigned char ucSirenCount, DWORD dwPercentage)
    {
        m_tSirenInfo.m_tSirenInfo[ucSirenCount].m_dwMinSirenAlpha = dwPercentage;
    }
    void               SetVehicleSirenPosition(unsigned char ucSirenID, CVector vecPos);
    void               GetVehicleSirenPosition(unsigned char ucSirenID, CVector& vecPos);
    unsigned char      GetVehicleSirenCount() { return m_tSirenInfo.m_ucSirenCount; }
    unsigned char      GetVehicleSirenType() { return m_tSirenInfo.m_ucSirenType; }
    DWORD              GetVehicleSirenMinimumAlpha(unsigned char ucSirenID) { return m_tSirenInfo.m_tSirenInfo[ucSirenID].m_dwMinSirenAlpha; }
    SharedUtil::SColor GetVehicleSirenColour(unsigned char ucSirenID) { return m_tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour; }
    void               SetVehicleSirenColour(unsigned char ucSirenID, SharedUtil::SColor tVehicleSirenColour)
    {
        m_tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour = tVehicleSirenColour;
    }
    void                              SetVehicleCurrentSirenID(unsigned char ucCurrentSirenID) { m_tSirenInfo.m_ucCurrentSirenID = ucCurrentSirenID; }
    unsigned char                     GetVehicleCurrentSirenID() { return m_tSirenInfo.m_ucCurrentSirenID; }
    unsigned char                     GetSirenRandomiser() { return m_tSirenInfo.m_ucCurrentSirenRandomiser; }
    void                              SetSirenRandomiser(unsigned char ucSirenRandomiser) { m_tSirenInfo.m_ucCurrentSirenRandomiser = ucSirenRandomiser; }
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
    bool                              SetWindowOpenFlagState(unsigned char ucWindow, bool bState);
    float                             GetWheelScale() override { return GetVehicleInterface()->m_fWheelScale; }
    void                              SetWheelScale(float fWheelScale) override { GetVehicleInterface()->m_fWheelScale = fWheelScale; }

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
