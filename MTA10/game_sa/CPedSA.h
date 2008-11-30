/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedSA.h
*  PURPOSE:     Header file for ped entity base class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPedSA;

#ifndef __CGAMESA_PED
#define __CGAMESA_PED

#include <game/CPed.h>
#include <game/CWeapon.h>

#include "CEntitySA.h"
#include "CFireSA.h"
#include "CPedIKSA.h"
#include "CPhysicalSA.h"

#include "CPedSoundSA.h"
#include "CPoolsSA.h"
#include "CVehicleSA.h"
#include "CWeaponSA.h"

class CPedIntelligenceSA;
class CPlayerPedDataSAInterface;
class CPedIntelligenceSAInterface;

#include "CPedIntelligenceSA.h"

#define SIZEOF_CPLAYERPED                           1956

#define FUNC_SetModelIndex							0x5E4880 // ##SA##
#define FUNC_AttachPedToBike						0x5E7E60 
#define FUNC_AttachPedToEntity						0x5E7CB0
#define FUNC_CanSeeEntity							0x5E0730
#define FUNC_GiveObjectToPedToHold					0x5E4390
#define FUNC_RestorePlayerStuffDuringResurrection	0x442060
#define FUNC_SetIsStanding							0x4ABBE0
#define FUNC_MakeChangesForNewWeapon_Slot           0x60D000

#define FUNC_CPedClothesDesc__SetTextureAndModel	0x5A8080
#define FUNC_CClothes__RebuildPlayer				0x5A82C0

#define FUNC_QuitEnteringCar                        0x650130 // really belongs in CCarEnterExit

#define FUNC_CanPedReturnToState		0x5018D0
#define FUNC_ClearAimFlag				0x50B4A0
#define FUNC_ClearAll					0x509DF0
#define FUNC_ClearAttackByRemovingAnim	0x52CF70
#define FUNC_ClearInvestigateEvent		0x526BA0
#define FUNC_ClearLookFlag				0x50B9C0
#define FUNC_ClearObjective				0x521720
#define FUNC_ClearPointGunAt			0x52DBE0
#define FUNC_ClearWeapons				0x4FF740
#define FUNC_CreateDeadPedMoney			0x43E2C0
#define FUNC_CreateDeadPedWeaponPickups	0x43DF30
#define FUNC_GetLocalDirection			0x5035F0
#define FUNC_GetWeaponSlot              0x5DF200 // ##SA##
#define FUNC_SetCurrentWeapon           0x5E61F0 // ##SA##
#define FUNC_GiveWeapon					0x5E6080 // ##SA##
#define FUNC_GetBonePosition            0x5E4280 // ##SA##
#define FUNC_GetTransformedBonePosition 0x5E01C0 // ##SA##
#define FUNC_IsWearingGoggles           0x479D10 // ##SA##
#define FUNC_TakeOffGoggles             0x5E6010 // ##SA##
#define FUNC_PutOnGoggles               0x5E3AE0 // ##SA##
#define FUNC_RemoveWeaponModel          0x5E3990 // ##SA## (call with -1 to remove any model, I think)
#define FUNC_RemoveGogglesModel         0x5DF170 // ##SA## 
#define FUNC_ClearWeapon                0x5E62B0 // ##SA##
#define FUNC_GetHealth					0x4ABC20
#define FUNC_SetCurrentWeaponFromID		0x4FF8E0
#define FUNC_SetCurrentWeaponFromSlot	0x4FF900
#define FUNC_IsPedInControl				0x501950
#define FUNC_IsPedShootable				0x501930
#define FUNC_SetAttackTimer				0x4FCAB0
#define FUNC_SetDead					0x4F6430
#define FUNC_SetDie						0x4F65C0
#define FUNC_SetEvasiveDive				0x4F6A20
#define FUNC_SetEvasiveStep				0x4F7170
#define FUNC_SetFall					0x4FD9F0
#define FUNC_SetFlee					0x4FBA90
#define FUNC_SetIdle					0x4FDFD0
#define FUNC_SetLeader					0x4F07D0
#define FUNC_SetLookFlag				0x50BB70
#define FUNC_SetLookTimer				0x4FCAF0
#define FUNC_SetMoveState				0x50D110
#define FUNC_SetObjective_ENTITY		0x521F10
#define FUNC_SetObjective				0x5224B0
#define FUNC_SetObjective_VECTOR		0x521840
#define FUNC_SetSeekCar					0x4F54D0
#define FUNC_SetShootTimer				0x4FCA90
#define FUNC_SetSolicit					0x4F1400
#define FUNC_SetStoredState				0x50CC40
#define FUNC_RestorePreviousState		0x50C600
#define FUNC_SetWaitState				0x4F28A0
#define FUNC_Teleport					0x4F5690
#define FUNC_WarpPedIntoCar				0x4EF8B0
#define FUNC_DetachPedFromEntity        0x5E7EC0
#define FUNC_CPed_RemoveBodyPart        0x5f0140

#define VAR_LocalPlayer					0x94AD28

/*
#ifndef CVehicleSA_DEFINED
#define CVehicleSA void
#endif

#ifdef CPedSA
#undef CPedSA
#endif

#ifdef CPedSAInterface
#undef CPedSAInterface
#endif
*/

//+1328 = Ped state
//+1344 = ped health
class CPedFlags
{
public:
unsigned int bIsStanding : 1; // is ped standing on something
unsigned int bWasStanding : 1; // was ped standing on something
unsigned int bIsLooking : 1; // is ped looking at something or in a direction
unsigned int bIsRestoringLook : 1; // is ped restoring head postion from a look
unsigned int bIsAimingGun : 1; // is ped aiming gun
unsigned int bIsRestoringGun : 1; // is ped moving gun back to default posn
unsigned int bCanPointGunAtTarget : 1; // can ped point gun at target
unsigned int bIsTalking : 1; // is ped talking(see Chat())

unsigned int bInVehicle : 1; // is in a vehicle
unsigned int bIsInTheAir : 1; // is in the air
unsigned int bIsLanding : 1; // is landing after being in the air
unsigned int bHitSomethingLastFrame : 1; // has been in a collision last fram
unsigned int bIsNearCar : 1; // has been in a collision last fram
unsigned int bRenderPedInCar : 1; // has been in a collision last fram
unsigned int bUpdateAnimHeading : 1; // update ped heading due to heading change during anim sequence
unsigned int bRemoveHead : 1; // waiting on AntiSpazTimer to remove head

unsigned int bFiringWeapon : 1; // is pulling trigger
unsigned int bHasACamera : 1; // does ped possess a camera to document accidents
unsigned int bPedIsBleeding : 1; // Ped loses a lot of blood if true
unsigned int bStopAndShoot : 1; // Ped cannot reach target to attack with fist, need to use gun
unsigned int bIsPedDieAnimPlaying : 1; // is ped die animation finished so can dead now
unsigned int bStayInSamePlace :1; // when set, ped stays put
unsigned int bKindaStayInSamePlace :1; // when set, ped doesn't seek out opponent or cover large distances. Will still shuffle and look for cover
unsigned int bBeingChasedByPolice :1; // use nodes for routefind

unsigned int bNotAllowedToDuck :1; // Is this ped allowed to duck at all?
unsigned int bCrouchWhenShooting :1; // duck behind cars etc
unsigned int bIsDucking :1; // duck behind cars etc
unsigned int bGetUpAnimStarted :1; // don't want to play getup anim if under something
unsigned int bDoBloodyFootprints :1; // unsigned int bIsLeader :1;
unsigned int bDontDragMeOutCar :1;
unsigned int bStillOnValidPoly :1; // set if the polygon the ped is on is still valid for collision
unsigned int bAllowMedicsToReviveMe :1;

unsigned int bResetWalkAnims :1;
unsigned int bOnBoat :1; // flee but only using nodes
unsigned int bBusJacked :1; // flee but only using nodes
unsigned int bFadeOut :1; // set if you want ped to fade out
unsigned int bKnockedUpIntoAir :1; // has ped been knocked up into the air by a car collision
unsigned int bHitSteepSlope :1; // has ped collided/is standing on a steep slope (surface type)
unsigned int bCullExtraFarAway :1; // special ped only gets culled if it's extra far away (for roadblocks)
unsigned int bTryingToReachDryLand :1; // has ped just exited boat and trying to get to dry land

unsigned int bCollidedWithMyVehicle :1;
unsigned int bRichFromMugging :1; // ped has lots of cash cause they've been mugging people
unsigned int bChrisCriminal :1; // Is a criminal as killed during Chris' police mission (should be counted as such)
unsigned int bShakeFist :1; // test shake hand at look entity
unsigned int bNoCriticalHits : 1; // ped cannot be killed by a single bullet
unsigned int bHasAlreadyBeenRecorded : 1; // Used for replays
unsigned int bUpdateMatricesRequired : 1; // if PedIK has altered bones so matrices need updated this frame
unsigned int bFleeWhenStanding :1; //

unsigned int bMiamiViceCop :1;  //
unsigned int bMoneyHasBeenGivenByScript :1; //
unsigned int bHasBeenPhotographed :1;  //
unsigned int bIsDrowning : 1;
unsigned int bDrownsInWater : 1;
unsigned int bHeadStuckInCollision : 1;
unsigned int bDeadPedInFrontOfCar :1;
unsigned int bStayInCarOnJack :1;

unsigned int bDontFight :1;
unsigned int bDoomAim :1;
unsigned int bCanBeShotInVehicle : 1;
unsigned int bPushedAlongByCar :1; // ped is getting pushed along by car collision (so don't take damage from horz velocity)
unsigned int bNeverEverTargetThisPed :1;
unsigned int bThisPedIsATargetPriority :1;
unsigned int bCrouchWhenScared :1;
unsigned int bKnockedOffBike :1;

unsigned int bDonePositionOutOfCollision :1;
unsigned int bDontRender : 1;
unsigned int bHasBeenAddedToPopulation :1;
unsigned int bHasJustLeftCar :1;
unsigned int bIsInDisguise :1;
unsigned int bDoesntListenToPlayerGroupCommands :1;
unsigned int bIsBeingArrested :1;
unsigned int bHasJustSoughtCover :1;

unsigned int bKilledByStealth :1;
unsigned int bDoesntDropWeaponsWhenDead :1;
unsigned int bCalledPreRender :1;
unsigned int bBloodPuddleCreated : 1; // Has a static puddle of blood been created yet
unsigned int bPartOfAttackWave :1;
unsigned int bClearRadarBlipOnDeath :1;
unsigned int bNeverLeavesGroup :1; // flag that we want to test 3 extra spheres on col model
unsigned int bTestForBlockedPositions :1; // this sets these indicator flags for various posisions on the front of the ped

unsigned int bRightArmBlocked :1;
unsigned int bLeftArmBlocked :1;
unsigned int bDuckRightArmBlocked :1;
unsigned int bMidriffBlockedForJump :1;
unsigned int bFallenDown :1;
unsigned int bUseAttractorInstantly :1;
unsigned int bDontAcceptIKLookAts :1;
unsigned int bHasAScriptBrain : 1;

unsigned int bWaitingForScriptBrainToLoad : 1;
unsigned int bHasGroupDriveTask :1;
unsigned int bCanExitCar :1;
unsigned int CantBeKnockedOffBike :2; // 0=Default(harder for mission peds) 1=Never 2=Always normal(also for mission peds)
unsigned int bHasBeenRendered : 1;
unsigned int bIsCached :1;
unsigned int bPushOtherPeds :1; // GETS RESET EVERY FRAME - SET IN TASK: want to push other peds around (eg. leader of a group or ped trying to get in a car)
unsigned int bHasBulletProofVest :1;

unsigned int bUsingMobilePhone :1;
unsigned int bUpperBodyDamageAnimsOnly :1;
unsigned int bStuckUnderCar :1;
unsigned int bKeepTasksAfterCleanUp :1; // If true ped will carry on with task even after cleanup
unsigned int bIsDyingStuck :1;
unsigned int bIgnoreHeightCheckOnGotoPointTask :1; // set when walking round buildings, reset when task quits
unsigned int bForceDieInCar:1;
unsigned int bCheckColAboveHead:1;

unsigned int bIgnoreWeaponRange : 1;
unsigned int bDruggedUp : 1;
unsigned int bWantedByPolice : 1; // if this is set, the cops will always go after this ped when they are doing a KillCriminal task
unsigned int bSignalAfterKill: 1;
unsigned int bCanClimbOntoBoat :1;
unsigned int bPedHitWallLastFrame: 1; // useful to store this so that AI knows (normal will still be available)
unsigned int bIgnoreHeightDifferenceFollowingNodes: 1;
unsigned int bMoveAnimSpeedHasBeenSetByTask: 1;

unsigned int bGetOutUpsideDownCar :1;
unsigned int bJustGotOffTrain :1;
unsigned int bDeathPickupsPersist :1;
unsigned int bTestForShotInVehicle :1;
//#ifdef GTA_REPLAY
unsigned int bUsedForReplay : 1; // This ped is controlled by replay and should be removed when replay is done.
//#endif

};

class CPedSAInterface : public CPhysicalSAInterface // +1420  = current vehicle   312 first byte
{
public:
    // current weapon slot 1184 ( and +1816?)
	//CPedIKSAInterface		pedIK; // 528
	//CWeaponSAInterface	Weapons[9];	// 1032
	BYTE bPad[348];
    CPedSoundSAInterface pedSound;
    BYTE bPad11[472 - sizeof(CPedSoundSAInterface)];
    CPedFlags pedFlags; // 1132 (16 bytes long including alignment probably)
    CPedIntelligenceSAInterface * pPedIntelligence;
    CPlayerPedDataSAInterface * pPlayerData; //1152
    BYTE bPad4a[80];
    int iMoveAnimGroup;      // 1236
    BYTE bPad4b[52];
    CPedIKSAInterface pedIK; // 1292 (length 32 bytes)
    int bPad5[5];

    float fHealth;
    int  iUnknown121;
    float fArmor;

    int iUnknown313 [3];        // +1356
    // +1368 = rotation
    float fCurrentRotation;
    float fTargetRotation;
    float fRotationSpeed;
    BYTE bPad8[4];
    CEntitySAInterface * pContactEntity;
    BYTE bPad3[32];
	CEntitySAInterface * CurrentObjective; // current vehicle    1420
    BYTE bPad2[8];
	BYTE bPedType;	// ped type? 0 = player, >1 = ped?
	BYTE bPad9[7];
    CWeaponSAInterface      Weapons[WEAPONSLOT_MAX];
    //weapons at +1440 ends at +1804
    BYTE bPad4[12];
    BYTE bCurrentWeaponSlot; // is actually here
    BYTE bPad6[20];
    BYTE bFightingStyle; // 1837
    BYTE bFightingStyleExtra;
    BYTE bPad7[1];
    CFireInterface* pFireOnPed;
    BYTE bPad10[104];
    CEntitySAInterface * pTargetedEntity; // 1948
};

class CPedSA : public virtual CPed, public virtual CPhysicalSA
{
	friend class CPoolsSA;
private:
//	CPedSAInterface		* internalInterface;
	CWeaponSA			* Weapons[WEAPONSLOT_MAX];
	CPedIKSA			* PedIK;
	CPedIntelligenceSA	* PedIntelligence;
    CPedSAInterface     * m_pPedInterface;

	DWORD				dwType;
    unsigned char       m_ucOccupiedSeat;
public:
	                    CPedSA(  );
	                    CPedSA( CPedSAInterface * pedInterface );
                        ~CPedSA();

    CPedSAInterface *   GetPedInterface ( void ) { return ( CPedSAInterface * ) GetInterface (); }
	VOID				Init();
	VOID				SetModelIndex ( DWORD dwModelIndex );
	VOID				AttachPedToBike(CEntity * entity, CVector * vector, unsigned short sUnk, FLOAT fUnk, FLOAT fUnk2, eWeaponType weaponType);
	VOID				AttachPedToEntity(DWORD dwEntityInterface, CVector * vector, unsigned short sDirection, FLOAT fRotationLimit, eWeaponType weaponType, BOOL bChangeCamera);
    VOID                DetachPedFromEntity ( void );
	
	BOOL				CanSeeEntity(CEntity * entity, FLOAT fDistance);
	CVehicle			* GetVehicle();
	VOID				Respawn (CVector * position, bool bCameraCut);
    BOOL                AddProjectile ( eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector * target, CEntity * targetEntity );

	FLOAT				GetHealth       ( void );
    void				SetHealth       ( float fHealth );

    float               GetArmor        ( void );
    void                SetArmor        ( float fArmor );

    CWeapon *           GiveWeapon      ( eWeaponType weaponType, unsigned int uiAmmo );
    CWeapon *           GetWeapon       ( eWeaponSlot weaponSlot );
    CWeapon *           GetWeapon       ( eWeaponType weaponType );
    void                ClearWeapons    ( void );
    void                RemoveWeaponModel ( int iModel );
    void                ClearWeapon     ( eWeaponType weaponType );

	VOID				SetIsStanding( bool bStanding );
	CPedIntelligence	* GetPedIntelligence ( void );
	DWORD				GetType ( void );
	VOID				SetType ( DWORD dwType );
    DWORD               * GetMemoryValue ( DWORD dwOffset );

	FLOAT		        GetCurrentRotation();
	FLOAT		        GetTargetRotation();
	VOID		        SetCurrentRotation(FLOAT fRotation);
	VOID		        SetTargetRotation(FLOAT fRotation);

    eWeaponSlot         GetCurrentWeaponSlot ();
    void                SetCurrentWeaponSlot ( eWeaponSlot weaponSlot );

    CVector *           GetBonePosition ( eBone bone, CVector * vecPosition );
    CVector *           GetTransformedBonePosition ( eBone bone, CVector * vecPosition );

    bool                IsDucking ( void );
    void                SetDucking ( bool bDuck );

	bool				IsInWater ( void );

    int                 GetCantBeKnockedOffBike ( void );
    void                SetCantBeKnockedOffBike ( int iCantBeKnockedOffBike );
    void                QuitEnteringCar ( CVehicle * vehicle, int iSeat, bool bUnknown );

    bool                IsWearingGoggles ( void );
    void                SetGogglesState ( bool bIsWearingThem );

	VOID			    SetClothesTextureAndModel ( char * szTexture, char * szModel, int textureType );
	VOID			    RebuildPlayer ( void );

    eFightingStyle      GetFightingStyle ( void );
    void                SetFightingStyle ( eFightingStyle style, BYTE bStyleExtra = 6 );

    CEntity*            GetContactEntity ( void );
    
    unsigned char       GetRunState ( void );

    CEntity*            GetTargetedEntity ( void );
    void                SetTargetedEntity ( CEntity* pEntity );

    bool                GetCanBeShotInVehicle       ( void );
    bool                GetTestForShotInVehicle     ( void );

    void                SetCanBeShotInVehicle       ( bool bShot );
    void                SetTestForShotInVehicle     ( bool bTest );

    bool                InternalAttachEntityToEntity ( DWORD dwEntityInterface, const CVector * vecPosition, const CVector * vecRotation );

    inline BYTE         GetOccupiedSeat ( void )                { return m_ucOccupiedSeat; }
    inline void         SetOccupiedSeat ( BYTE seat )           { m_ucOccupiedSeat = seat; }

    void                RemoveBodyPart ( int i, char c );

    bool                IsOnFire ( void );
    void                SetOnFire ( bool bOnFire );

    inline bool         GetStayInSamePlace ( void )             { return GetPedInterface ()->pedFlags.bStayInSamePlace; }
    void                SetStayInSamePlace ( bool bStay );

    void                GetPedVoice             ( const char** pszVoiceType, const char** pszVoice );
    void                SetPedVoice             ( const char* szVoiceType, const char* szVoice );
};

#endif