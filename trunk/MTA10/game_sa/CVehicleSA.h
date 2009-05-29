/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleSA.h
*  PURPOSE:     Header file for vehicle base entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CVehicleSA;

#ifndef __CGAMESA_VEHICLE
#define __CGAMESA_VEHICLE

#include <game/CVehicle.h>

#include "Common.h"
#include "CPedSA.h"
#include "CPhysicalSA.h"
#include "CPoolsSA.h"
#include "CHandlingManagerSA.h"
#include "CDamageManagerSA.h"

#define SIZEOF_CHELI                            2584

// 00431f80	public: static class CVehicle * __cdecl CCarCtrl::CreateCarForScript(int,class CVector,unsigned char)
#define FUNC_CCarCtrlCreateCarForScript		    0x431f80 // ##SA##

//006d6a40	public: virtual void __thiscall CVehicle::SetModelIndex(unsigned int)
#define FUNC_CVehicleSetModelIndex			    0x6d6a40

//006d16a0	public: void __thiscall CVehicle::SetDriver(class CPed *)
#define FUNC_CVehicleSetDriver				    0x6d16a0

//006d14d0	public: bool __thiscall CVehicle::AddPassenger(class CPed *,unsigned char)
#define FUNC_CVehicleAddPassengerWithSeat	    0x6d14d0

//006d13a0	public: bool __thiscall CVehicle::AddPassenger(class CPed *)
#define FUNC_CVehicleAddPassenger			    0x6d13a0

//006d1610	public: void __thiscall CVehicle::RemovePassenger(class CPed *)
#define FUNC_CVehicleRemovePassenger		    0x6d1610

//006E3290  public: void __thiscall CVehicle::AddVehicleUpgrade(DWORD dwModelID)
#define FUNC_CVehicle_AddVehicleUpgrade         0x6E3290

//006DF930     ; public: void __thiscall CVehicle::RemoveVehicleUpgrade(int)
#define FUNC_CVehicle_RemoveVehicleUpgrade      0x6DF930

//006D1E80  public: bool __thiscall CVehicle::CanPedEnterCar(void)
#define FUNC_CVehicle_CanPedEnterCar            0x6D1E80

//006D2030  public: bool __thiscall CVehicle::CanPedJumpOutCar(void)
#define FUNC_CVehicle_CanPedJumpOutCar          0x6D2030

//006D5CF0  public: bool __thiscall CVehicle::CanPedLeanOut(CPed*)
#define FUNC_CVehicle_CanPedLeanOut             0x6D5CF0

//006D1F30  public: bool __thiscall CVehicle::CanPedStepOutCar(void)
#define FUNC_CVehicle_CanPedStepOutCar          0x6D1F30

//006D25D0  public: bool __thiscall CVehicle::CarHasRoof(void)
#define FUNC_CVehicle_CarHasRoof                0x6D25D0

//006D2460  public: void __thiscall CVehicle::ExtinguishCarFire(void)
#define FUNC_CVehicle_ExtinguishCarFire         0x6D2460

//006D2460  public: DWORD __thiscall CVehicle::GetBaseVehicleType(void)
#define FUNC_CVehicle_GetBaseVehicleType        0x411D50

//006D63F0  public: DWORD __thiscall CVehicle::GetHeightAboveRoad(void)
#define FUNC_CVehicle_GetHeightAboveRoad        0x6D63F0

//006DFBE0  public: DWORD __thiscall CVehicle::GetTowBarPos(CVector*, 1, 0)
#define FUNC_CVehicle_GetTowBarPos              0x6DFBE0

//006DFB70  public: void __thiscall CVehicle::GetTowHitchPos(CVector*, 1, 0)
#define FUNC_CVehicle_GetTowHitchPos            0x6DFB70

//006D1DD0  public: bool __thiscall CVehicle::IsOnItsSide(void)
#define FUNC_CVehicle_IsOnItsSide               0x6D1DD0

//006D2370  public: bool __thiscall CVehicle::IsLawEnforcementVehicle(void)
#define FUNC_CVehicle_IsLawEnforcementVehicle   0x6D2370

//006D1BD0  public: bool __thiscall CVehicle::IsPassenger(CPed*)
#define FUNC_CVehicle_IsPassenger               0x6D1BD0

//006D84D0  public: bool __thiscall CVehicle.:IsSphereTouchingVehicle(float, float, float, float)
#define FUNC_CVehicle_IsSphereTouchingVehicle   0x6D84D0

//006D1D90  public: bool __thiscall CVehicle::IsUpsideDown(void)
#define FUNC_CVehicle_IsUpsideDown              0x6D1D90

//006D2BF0  public: void __thiscall CVehicle::MakeDirty(CColPoint*)
#define FUNC_CVehicle_MakeDirty                 0x6D2BF0

//0041BDD0  public: void __thiscall CVehicle::SetEngineOn(bool)
#define FUNC_CVehicle_SetEngineOn               0x41BDD0

//006F6CC0  public: void __thiscall CVehicle::RecalcOnRailDistance(void)
#define FUNC_CVehicle_RecalcOnRailDistance           0x6F6CC0

//006F6BD0  int GetTrainNodeNearPoint(float x, float y, float z, int* pTrackID) places track ID in *pTrackID and returns node ID
#define FUNC_GetTrainNodeNearPoint              0x6F6BD0

#define FUNC_CVehicle_QueryPickedUpEntityWithWinch              0x6d3cf0
#define FUNC_CVehicle_PickUpEntityWithWinch                     0x6d3cd0
#define FUNC_CVehicle_ReleasePickedUpEntityWithWinch            0x6d3cb0
#define FUNC_CVehicle_SetRopeHeightForHeli                      0x6d3d30

#define FUNC_CVehicle__SetRemapTexDictionary                    0x6D0BC0
#define FUNC_CVehicle__GetRemapIndex                            0x6D0B70
#define FUNC_CVehicle__SetRemap                                 0x6D0C00

// from CBike
#define FUNC_Bike_PlaceOnRoadProperly			0x6BEEB0
#define FUNC_Automobile_PlaceOnRoadProperly		0x6AF420
#define FUNC_CBike_Fix                          0x6B7050

// from CPlane
#define FUNC_CPlane__SetGearUp			        0x6CAC20
#define FUNC_CPlane__SetGearDown		        0x6CAC70
#define FUNC_CPlane__Fix                        0x6CABB0

// from CAutomobile
#define FUNC_CAutomobile__Fix                   0x6A3440

// from CHeli
#define FUNC_CHeli__Fix                         0x6C4530

// from CQuadBike
#define FUNC_CQuadBike__Fix                     0x6CE2B0

#define VARS_CarCounts							0x969094 // 5 values for each vehicle type

// Used when deleting vehicles
#define VTBL_CPlaceable							0x863C40

#define MAX_PASSENGERS						    8

#define NUM_RAILTRACKS                          4
#define ARRAY_NumRailTrackNodes                 0xC38014    // NUM_RAILTRACKS dwords
#define ARRAY_RailTrackNodePointers             0xC38024    // NUM_RAILTRACKS pointers to arrays of SRailNode

typedef struct
{
    short sX;               // x coordinate times 8
    short sY;               // y coordinate times 8
    short sZ;               // z coordinate times 8
    WORD  sRailDistance;    // on-rail distance times 3
    WORD  padding;
} SRailNodeSA;

class CVehicleSAInterfaceVTBL : public CEntitySAInterfaceVTBL
{
public:
    DWORD ProcessEntityCollision; 
    DWORD ProcessControlCollisionCheck;
    DWORD ProcessControlInputs;
    DWORD GetComponentWorldPosition;
    DWORD IsComponentPresent;
    DWORD OpenDoor;
    DWORD ProcessOpenDoor;
    DWORD GetDoorAngleOpenRatio;
    DWORD GetDoorAngleOpenRatio_;
    DWORD IsDoorReady;
    DWORD IsDoorReady_;
    DWORD IsDoorFullyOpen;
    DWORD IsDoorFullyOpen_;
    DWORD IsDoorClosed;
    DWORD IsDoorClosed_;
    DWORD IsDoorMissing;
    DWORD IsDoorMissing_;
    DWORD IsOpenTopCar;
    DWORD RemoveRefsToVehicle;
    DWORD BlowUpCar;
    DWORD BlowUpCarCutSceneNoExtras;
    DWORD SetUpWheelColModel;
    DWORD BurstTyre;
    DWORD IsRoomForPedToLeaveCar;
    DWORD ProcessDrivingAnims;
    DWORD GetRideAnimData;
    DWORD SetupSuspensionLines;
    DWORD AddMovingCollisionSpeed;
    DWORD Fix;
    DWORD SetupDamageAfterLoad;
    DWORD DoBurstAndSoftGroundRatios;
    DWORD GetHeightAboveRoad;
    DWORD PlayCarHorn;
    DWORD GetNumContactWheels;
    DWORD VehicleDamage;
    DWORD CanPedStepOutCar;
    DWORD CanPedJumpOutCar;
    DWORD GetTowHitchPos;
    DWORD GetTowbarPos;
    DWORD SetTowLink;
    DWORD BreakTowLink;
    DWORD FindWheelWidth;
    DWORD Save;
    DWORD Load;
};

struct CVehicleFlags
{
	unsigned char bIsLawEnforcer: 1; // Is this guy chasing the player at the moment
	unsigned char bIsAmbulanceOnDuty: 1; // Ambulance trying to get to an accident
	unsigned char bIsFireTruckOnDuty: 1; // Firetruck trying to get to a fire
	unsigned char bIsLocked: 1; // Is this guy locked by the script (cannot be removed)
	unsigned char bEngineOn: 1; // For sound purposes. Parked cars have their engines switched off (so do destroyed cars)
	unsigned char bIsHandbrakeOn: 1; // How's the handbrake doing ?
	unsigned char bLightsOn: 1; // Are the lights switched on ?
	unsigned char bFreebies: 1; // Any freebies left in this vehicle ?

	unsigned char bIsVan: 1; // Is this vehicle a van (doors at back of vehicle)
	unsigned char bIsBus: 1; // Is this vehicle a bus
	unsigned char bIsBig: 1; // Is this vehicle a bus
	unsigned char bLowVehicle: 1; // Need this for sporty type cars to use low getting-in/out anims
	unsigned char bComedyControls: 1; // Will make the car hard to control (hopefully in a funny way)
	unsigned char bWarnedPeds: 1; // Has scan and warn peds of danger been processed?
	unsigned char bCraneMessageDone: 1; // A crane message has been printed for this car allready
	// unsigned char bExtendedRange: 1; // This vehicle needs to be a bit further away to get deleted
	unsigned char bTakeLessDamage: 1; // This vehicle is stronger (takes about 1/4 of damage)

	unsigned char bIsDamaged: 1; // This vehicle has been damaged and is displaying all its components
	unsigned char bHasBeenOwnedByPlayer : 1;// To work out whether stealing it is a crime
	unsigned char bFadeOut: 1; // Fade vehicle out
	unsigned char bIsBeingCarJacked: 1; // Fade vehicle out
	unsigned char bCreateRoadBlockPeds : 1;// If this vehicle gets close enough we will create peds (coppers or gang members) round it
	unsigned char bCanBeDamaged: 1; // Set to FALSE during cut scenes to avoid explosions
	// unsigned char bUsingSpecialColModel : 1;
	// Is player vehicle using special collision model, stored in player strucure
	unsigned char bOccupantsHaveBeenGenerated : 1; // Is true if the occupants have already been generated. (Shouldn't happen again)
	unsigned char bGunSwitchedOff: 1; // Level designers can use this to switch off guns on boats
	
	unsigned char bVehicleColProcessed : 1;// Has ProcessEntityCollision been processed for this car?
	unsigned char bIsCarParkVehicle: 1; // Car has been created using the special CAR_PARK script command
	unsigned char bHasAlreadyBeenRecorded : 1; // Used for replays
	unsigned char bPartOfConvoy: 1;
	unsigned char bHeliMinimumTilt: 1; // This heli should have almost no tilt really
	unsigned char bAudioChangingGear: 1; // sounds like vehicle is changing gear
	unsigned char bIsDrowning: 1; // is vehicle occupants taking damage in water (i.e. vehicle is dead in water)
	unsigned char bTyresDontBurst: 1; // If this is set the tyres are invincible

	unsigned char bCreatedAsPoliceVehicle : 1;// True if this guy was created as a police vehicle (enforcer, policecar, miamivice car etc)
	unsigned char bRestingOnPhysical: 1; // Dont go static cause car is sitting on a physical object that might get removed
		unsigned char      bParking                    : 1;
		unsigned char      bCanPark                    : 1;
	unsigned char bFireGun: 1; // Does the ai of this vehicle want to fire it's gun?
	unsigned char bDriverLastFrame: 1; // Was there a driver present last frame ?
	unsigned char bNeverUseSmallerRemovalRange: 1;// Some vehicles (like planes) we don't want to remove just behind the camera.
	unsigned char bIsRCVehicle: 1; // Is this a remote controlled (small) vehicle. True whether the player or AI controls it.

	unsigned char bAlwaysSkidMarks: 1; // This vehicle leaves skidmarks regardless of the wheels' states.
	unsigned char bEngineBroken: 1; // Engine doesn't work. Player can get in but the vehicle won't drive
	unsigned char bVehicleCanBeTargetted : 1;// The ped driving this vehicle can be targetted, (for Torenos plane mission)
	unsigned char bPartOfAttackWave: 1; // This car is used in an attack during a gang war
	unsigned char bWinchCanPickMeUp: 1; // This car cannot be picked up by any ropes.
	unsigned char bImpounded: 1; // Has this vehicle been in a police impounding garage
	unsigned char bVehicleCanBeTargettedByHS  : 1;// Heat seeking missiles will not target this vehicle.
	unsigned char bSirenOrAlarm: 1; // Set to TRUE if siren or alarm active, else FALSE

	unsigned char bHasGangLeaningOn: 1;
	unsigned char bGangMembersForRoadBlock : 1;// Will generate gang members if NumPedsForRoadBlock > 0
	unsigned char bDoesProvideCover: 1; // If this is false this particular vehicle can not be used to take cover behind.
	unsigned char bMadDriver: 1; // This vehicle is driving like a lunatic
	unsigned char bUpgradedStereo: 1; // This vehicle has an upgraded stereo
	unsigned char bConsideredByPlayer: 1; // This vehicle is considered by the player to enter
	unsigned char bPetrolTankIsWeakPoint : 1;// If false shootong the petrol tank will NOT Blow up the car
	unsigned char bDisableParticles: 1; // Disable particles from this car. Used in garage.

	unsigned char bHasBeenResprayed: 1; // Has been resprayed in a respray garage. Reset after it has been checked.
	unsigned char bUseCarCheats: 1; // If this is true will set the car cheat stuff up in ProcessControl()
	unsigned char bDontSetColourWhenRemapping : 1;// If the texture gets remapped we don't want to change the colour with it.
	unsigned char bUsedForReplay: 1; // This car is controlled by replay and should be removed when replay is done.
};

struct CTrainFlags
{
    unsigned char unknown1 : 3;
    unsigned char bIsTheChainEngine : 1; // Only the first created train on the chain gets this set to true, others get it set to false.
    unsigned char unknown1 : 1; // This is always set to true in mission trains construction.
    unsigned char bIsAtNode : 1;
    unsigned char bDirection : 1;
    unsigned char unknown2 : 1; // If the mission train was placed at the node, this is set to false in construction.

    unsigned char bIsDerailed : 1;
    unsigned char unknown3 : 1 ;
    unsigned char bIsDrivenByBrownSteak : 1;
    unsigned char unknown3 : 5;

    unsigned char unknown4 : 8;

    unsigned char unknown5 : 8;
};

/*
#ifndef CPEDSA_DEFINED
#define CPedSA void
#endif

#ifndef CPEDSAINTERFACE_DEFINED
#define CPedSAInterface void
#endif

#ifdef CVehicleSA
#undef CVehicleSA
#endif

#ifdef CVehicleSAInterface
#undef CVehicleSAInterface
#endif
*/

// TODO: Size?
class CAEVehicleAudioEntity
{
	BYTE pad1[154];
public:
	BYTE bCurrentRadioStation;
private:
    BYTE pad2;
};

class CAutoPilot
{
	BYTE pad[56];
};

#define MAX_UPGRADES_ATTACHED 15 // perhaps?

/**
 * \todo GAME RELEASE: Update CVehicleSAInterface
 */
class CVehicleSAInterface : public CPhysicalSAInterface
{
public:
    //char        pad [1158];     /* IMPORTANT: KEEP THIS UP-TO-DATE */
    //FLOAT       fDamage;
	CAEVehicleAudioEntity m_VehicleAudioEntity; // 312

    int padaudio[108];

    tHandlingDataSA* pHandlingData;                             // +900
    int pad52321 [23];

    DWORD dwUnknown1201;                                        // +996
    DWORD dwUnknown1202;                                        // +1000
	unsigned int hFlagsLocal;                                         // +1004

	CAutoPilot AutoPilot; // +1008
	CVehicleFlags m_nVehicleFlags; // +1064?
	unsigned int m_TimeOfCreation; // GetTimeInMilliseconds when this vehicle was created.

	unsigned char m_colour1, m_colour2, m_colour3, m_colour4;
	char m_comp1, m_comp2;
	short m_upgrades[MAX_UPGRADES_ATTACHED]; // 1082
	float m_wheelScale;//1112

	unsigned short CarAlarmState; //1116
	unsigned short ForcedRandomSeed; // if this is non-zero the random wander gets deterministic

	CPedSAInterface *pDriver;
	CPedSAInterface *pPassengers[MAX_PASSENGERS];
	unsigned char m_nNumPassengers;
	unsigned char m_nNumGettingIn;
	unsigned char m_nGettingInFlags;
	unsigned char m_nGettingOutFlags;
	unsigned char m_nMaxPassengers;
	unsigned char m_windowsOpenFlags;
	char m_nNitroBoosts;

	char m_nSpecialColModel;
	CEntity *pEntityWeAreOnForVisibilityCheck;
	CFire *m_pFire;

	float m_fSteerAngle; // +1172
	float m_f2ndSteerAngle; // used for steering 2nd set of wheels or elevators etc..
	float m_fGasPedal; // 0...1  // +1180
	float m_fBrakePedal; // 0...1

	unsigned char VehicleCreatedBy; // Contains information on whether this vehicle can be deleted 
							// or not. Probably only need this or IsLocked.
	short ExtendedRemovalRange;

	unsigned char BombOnBoard : 3; // 0 = None. 1 = Timed. 2 = On ignition, 3 = remotely set ? 4 = Timed Bomb has been activated. 5 = On ignition has been activated.
	unsigned char OverrideLights  : 2; // uses enum NO_CAR_LIGHT_OVERRIDE, FORCE_CAR_LIGHTS_OFF, FORCE_CAR_LIGHTS_ON
	unsigned char WinchType: 2; // Does this vehicle use a winch?
	unsigned char m_GunsCycleIndex : 2;// Cycle through alternate gun hardpoints on planes/helis
	unsigned char m_OrdnanceCycleIndex : 2; // Cycle through alternate ordnance hardpoints on planes/helis

	unsigned char nUsedForCover;// Has n number of cops hiding/attempting to hid behind it
	unsigned char AmmoInClip;// Used to make the guns on boat do a reload.
	unsigned char PacMansCollected;
	unsigned char PedsPositionForRoadBlock;
	unsigned char NumPedsForRoadBlock;

	//1200
	float nBodyDirtLevel; // Dirt level of vehicle body texture: 0.0f=fully clean, 15.0f=maximum dirt visible, it may be altered at any time while vehicle's cycle of lige

	// values used by transmission
	unsigned char m_nCurrentGear;
	float m_fGearChangeCount;

	float m_fWheelSpinForAudio;

	//1216
	float m_nHealth; // 1000.0f = full health. 0 -> explode

	/*** BEGIN SECTION that was added by us ***/
    BYTE Padding200[48]; //1220
	CVehicle* m_pVehicle; //1268
	/*** END SECTION that was added by us ***/

    //1272
    unsigned long ul_doorstate;

    //1276
    BYTE Padding210[24];

    //1300
    unsigned int m_isUsingHornOrSecondarySiren;

    //1304
    BYTE Padding220[136];

    //1440
    unsigned char m_ucTrackNodeID;  // Current node on train tracks
    BYTE Padding230[3];

    //1444
    float m_fTrainSpeed;           // Speed along rails
    //1448
    float m_fTrainRailDistance;    // Distance along rail starting from first rail node (determines train position when on rails)
    
    //1452
    float m_fDistanceToNextCarriage;
    DWORD padding240[2];

    //1464
    CTrainFlags trainFlags;

    //1468
    DWORD padding250[1];

    //1472
    BYTE m_ucRailTrackID;

    //1473
    BYTE padding260[15];

    //1488
    CVehicleSAInterface* m_prevCarriage;
    CVehicleSAInterface* m_nextCarriage;

    //1496
    BYTE padding270[780];

    // 2276
    float m_fBurningTime;
};


class CVehicleSA : public virtual CVehicle, public virtual CPhysicalSA
{
	friend class CPoolsSA;
private:
	CDamageManagerSA		* damageManager;
    CHandlingEntrySA*       m_pHandlingData;
    bool                    m_bIsDerailable;
    unsigned char           m_ucAlpha;

public:
								CVehicleSA                      ();
                                CVehicleSA                      ( CVehicleSAInterface * vehicleInterface );
								CVehicleSA                      ( eVehicleTypes dwModelID );
								~CVehicleSA                     ();

    // Override of CPhysicalSA::SetMoveSpeed to take trains into account
    VOID                        SetMoveSpeed                    ( CVector* vecMoveSpeed );

    bool                        AddProjectile                   ( eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector * target, CEntity * targetEntity );

    CVehicleSAInterface *       GetNextCarriageInTrain          ();
    CVehicle *                  GetNextTrainCarriage            ();
    void                        SetNextTrainCarriage            ( CVehicle * next );
    CVehicleSAInterface *       GetPreviousCarriageInTrain      ();
    CVehicle *                  GetPreviousTrainCarriage        ();
    void                        SetPreviousTrainCarriage        ( CVehicle * pPrevious );

    bool                        IsDerailed                      ();
    void                        SetDerailed                     ( bool bDerailed );
    inline bool                 IsDerailable                    () { return m_bIsDerailable; }
    inline void                 SetDerailable                   ( bool bDerailable ) { m_bIsDerailable = bDerailable; }
    float                       GetTrainSpeed                   ();
    void                        SetTrainSpeed                   ( float fSpeed );
    bool                        GetTrainDirection               ();
    void                        SetTrainDirection               ( bool bDirection );
    BYTE                        GetRailTrack                    ();
    void                        SetRailTrack                    ( BYTE ucTrackID );

    bool                        CanPedEnterCar                  ();
    bool                        CanPedJumpOutCar                ( CPed* pPed );
    void                        AddVehicleUpgrade               ( DWORD dwModelID );
    void                        RemoveVehicleUpgrade            ( DWORD dwModelID );
    bool                        CanPedLeanOut                   ( CPed* pPed );
    bool                        CanPedStepOutCar                ( bool bUnknown );

    bool                        AreDoorsLocked                  ();
    void                        LockDoors                       ( bool bLocked );
    bool                        AreDoorsUndamageable            ();
    void                        SetDoorsUndamageable            ( bool bUndamageable );

    bool                        CarHasRoof                      ();
    void                        ExtinguishCarFire               ();
    DWORD                       GetBaseVehicleType              ();

    void                        SetBodyDirtLevel                ( float fDirtLevel );
    float                       GetBodyDirtLevel                ();

    unsigned char               GetCurrentGear                  ();
    float                       GetGasPedal                     ();
    //GetIsHandbrakeOn    Use CVehicleSAInterface value
    float                       GetHeightAboveRoad              ();
    float                       GetSteerAngle                   ();
    bool                        GetTowBarPos                    ( CVector* pVector );
    bool                        GetTowHitchPos                  ( CVector* pVector );
    bool                        IsOnItsSide                     ();
    bool                        IsLawEnforcementVehicle         ();
    bool                        IsPassenger                     ( CPed* pPed );
    bool                        IsSphereTouchingVehicle         ( CVector * vecOrigin, float fRadius );
    bool                        IsUpsideDown                    ();
    void                        MakeDirty                       ( CColPoint* pPoint );

	CPed*                       GetDriver                       ();
    CPed*                       GetPassenger                    ( unsigned char ucSlot );
	bool                        IsBeingDriven                   ();
    
    bool                        IsEngineBroken                  () { return GetVehicleInterface ()->m_nVehicleFlags.bEngineBroken; };
    void						SetEngineBroken                 ( bool bEngineBroken ) { GetVehicleInterface ()->m_nVehicleFlags.bEngineBroken = bEngineBroken; }
    bool                        IsScriptLocked                  () { return GetVehicleInterface ()->m_nVehicleFlags.bIsLocked; }
    void						SetScriptLocked                 ( bool bLocked ) { GetVehicleInterface ()->m_nVehicleFlags.bIsLocked = bLocked; }

	void                        PlaceBikeOnRoadProperly         ();
	void                        PlaceAutomobileOnRoadProperly   ();
	void                        SetColor                        ( unsigned char color1, unsigned char color2, unsigned char color3, unsigned char color4 );
	void                        GetColor                        ( unsigned char* color1, unsigned char* color2, unsigned char* color3, unsigned char* color4 );
    bool                        IsSirenOrAlarmActive            ();
    void                        SetSirenOrAlarmActive           ( bool bActive );
    inline void                 SetAlpha                        ( unsigned char ucAlpha ) { m_ucAlpha = ucAlpha; }
    inline unsigned char        GetAlpha                        ( ) { return m_ucAlpha; }

	void				        SetLandingGearDown              ( bool bLandingGearDown );
	float				        GetLandingGearPosition          ();
    void                        SetLandingGearPosition          ( float fPosition );
	bool				        IsLandingGearDown               ();
    void                        Fix                             ();
    DWORD                       * GetMemoryValue                ( DWORD dwOffset );

    void                        BlowUp                          ( CEntity* pCreator, unsigned long ulUnknown );
    void                        BlowUpCutSceneNoExtras          ( unsigned long ulUnknown1, unsigned long ulUnknown2, unsigned long ulUnknown3, unsigned long ulUnknown4 );

    CDamageManager			    * GetDamageManager              ();

    bool                        SetTowLink                      ( CVehicle* pVehicle );
    bool                        BreakTowLink                    ();
    CVehicle *                  GetTowedVehicle                 ();
    CVehicle *                  GetTowedByVehicle               ();

	// remove these, no longer used
	BYTE                        GetRadioStation                 ();
	void                        SetRadioStation                 ( BYTE bRadioStation );

    void                        FadeOut                         ( bool bFadeOut );
    bool                        IsFadingOut                     ();

    void                        SetWinchType                    ( eWinchType winchType );
    void                        PickupEntityWithWinch           ( CEntity* pEntity );
    void                        ReleasePickedUpEntityWithWinch  ();
    void                        SetRopeHeightForHeli            ( float fRopeHeight );
    CPhysical *                 QueryPickedUpEntityWithWinch    ();

    void                        SetRemap                        ( int iRemap );
    int                         GetRemapIndex                   ();
    void                        SetRemapTexDictionary           ( int iRemapTextureDictionary );

    bool                        IsDamaged                               () { return GetVehicleInterface ()->m_nVehicleFlags.bIsDamaged; };
    bool                        IsDrowning                              () { return GetVehicleInterface ()->m_nVehicleFlags.bIsDrowning; };
    bool                        IsEngineOn                              () { return GetVehicleInterface ()->m_nVehicleFlags.bEngineOn; };
    bool                        IsHandbrakeOn                           () { return GetVehicleInterface ()->m_nVehicleFlags.bIsHandbrakeOn; };
    bool                        IsRCVehicle                             () { return GetVehicleInterface ()->m_nVehicleFlags.bIsRCVehicle; };
    bool                        GetAlwaysLeaveSkidMarks                 () { return GetVehicleInterface ()->m_nVehicleFlags.bAlwaysSkidMarks; };
    bool                        GetCanBeDamaged                         () { return GetVehicleInterface ()->m_nVehicleFlags.bCanBeDamaged; };
    bool                        GetCanBeTargettedByHeatSeekingMissiles  () { return GetVehicleInterface ()->m_nVehicleFlags.bVehicleCanBeTargettedByHS; };
    bool                        GetCanShootPetrolTank                   () { return GetVehicleInterface ()->m_nVehicleFlags.bPetrolTankIsWeakPoint; };
    bool                        GetChangeColourWhenRemapping            () { return GetVehicleInterface ()->m_nVehicleFlags.bDontSetColourWhenRemapping; };
    bool                        GetComedyControls                       () { return GetVehicleInterface ()->m_nVehicleFlags.bComedyControls; };
    bool                        GetGunSwitchedOff                       () { return GetVehicleInterface ()->m_nVehicleFlags.bGunSwitchedOff; };
    bool                        GetLightsOn                             () { return GetVehicleInterface ()->m_nVehicleFlags.bLightsOn; };
    unsigned int                GetOverrideLights                       () { return GetVehicleInterface ()->OverrideLights; }
    bool                        GetTakeLessDamage                       () { return GetVehicleInterface ()->m_nVehicleFlags.bTakeLessDamage; };
    bool                        GetTyresDontBurst                       () { return GetVehicleInterface ()->m_nVehicleFlags.bTyresDontBurst; };
    unsigned short              GetAdjustablePropertyValue              () { return *reinterpret_cast < unsigned short* > ( reinterpret_cast < unsigned long > ( m_pInterface ) + 2156 ); };
    float                       GetHelicopterRotorSpeed                 () { return *reinterpret_cast < float* > ( reinterpret_cast < unsigned int > ( m_pInterface ) + 2124 ); };
    unsigned long               GetExplodeTime                          () { return *reinterpret_cast < unsigned long* > ( reinterpret_cast < unsigned int > ( m_pInterface ) + 1240 ); };
    
    void                        SetAlwaysLeaveSkidMarks                 ( bool bAlwaysLeaveSkidMarks )      { GetVehicleInterface ()->m_nVehicleFlags.bAlwaysSkidMarks = bAlwaysLeaveSkidMarks; };
    void                        SetCanBeDamaged                         ( bool bCanBeDamaged )              { GetVehicleInterface ()->m_nVehicleFlags.bCanBeDamaged = bCanBeDamaged; };
    void                        SetCanBeTargettedByHeatSeekingMissiles  ( bool bEnabled )                   { GetVehicleInterface ()->m_nVehicleFlags.bVehicleCanBeTargettedByHS = bEnabled; };
    void                        SetCanShootPetrolTank                   ( bool bCanShoot )                  { GetVehicleInterface ()->m_nVehicleFlags.bPetrolTankIsWeakPoint = bCanShoot; };
    void                        SetChangeColourWhenRemapping            ( bool bChangeColour )              { GetVehicleInterface ()->m_nVehicleFlags.bDontSetColourWhenRemapping; };
    void                        SetComedyControls                       ( bool bComedyControls )            { GetVehicleInterface ()->m_nVehicleFlags.bComedyControls = bComedyControls; };
    void                        SetEngineOn                             ( bool bEngineOn );
    void                        SetGunSwitchedOff                       ( bool bGunsOff )                   { GetVehicleInterface ()->m_nVehicleFlags.bGunSwitchedOff = bGunsOff; };
    void                        SetHandbrakeOn                          ( bool bHandbrakeOn )               { GetVehicleInterface ()->m_nVehicleFlags.bIsHandbrakeOn = bHandbrakeOn; };
    void                        SetLightsOn                             ( bool bLightsOn )                  { GetVehicleInterface ()->m_nVehicleFlags.bLightsOn = bLightsOn; };
    void                        SetOverrideLights                       ( unsigned int uiOverrideLights )   { GetVehicleInterface ()->OverrideLights = uiOverrideLights; }
    void                        SetTaxiLightOn                          ( bool bLightOn );
    void                        SetTakeLessDamage                       ( bool bTakeLessDamage )            { GetVehicleInterface ()->m_nVehicleFlags.bTakeLessDamage = bTakeLessDamage; };
    void                        SetTyresDontBurst                       ( bool bTyresDontBurst )            { GetVehicleInterface ()->m_nVehicleFlags.bTyresDontBurst = bTyresDontBurst; };
    void                        SetAdjustablePropertyValue              ( unsigned short usAdjustableProperty ) { *reinterpret_cast < unsigned short* > ( reinterpret_cast < unsigned int > ( m_pInterface ) + 2156 ) = usAdjustableProperty; };
    void                        SetHelicopterRotorSpeed                 ( float fSpeed )                        { *reinterpret_cast < float* > ( reinterpret_cast < unsigned int > ( m_pInterface ) + 2124 ) = fSpeed; };
    void                        SetExplodeTime                          ( unsigned long ulTime )                { *reinterpret_cast < unsigned long* > ( reinterpret_cast < unsigned int > ( m_pInterface ) + 1240 ) = ulTime; };
	
	float                       GetHealth                       ();
	void                        SetHealth                       ( float fHealth );

	void                        GetTurretRotation               ( float * fHorizontal, float * fVertical );
	void                        SetTurretRotation               ( float fHorizontal, float fVertical );

    unsigned char               GetNumberGettingIn              ();
    unsigned char               GetPassengerCount               ();
    unsigned char               GetMaxPassengerCount            ();

    bool                        IsSmokeTrailEnabled             ();
    void                        SetSmokeTrailEnabled            ( bool bEnabled );

    CHandlingEntry*             GetHandlingData                 ();
    void                        SetHandlingData                 ( CHandlingEntry* pHandling );

    void                        BurstTyre                       ( BYTE bTyre );

    BYTE                        GetBikeWheelStatus              ( BYTE bWheel );
    void                        SetBikeWheelStatus              ( BYTE bWheel, BYTE bStatus );

    CVehicleSAInterface*        GetVehicleInterface             ()  { return (CVehicleSAInterface*) m_pInterface; }
};

#endif
