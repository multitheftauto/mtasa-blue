/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedSA.h
 *  PURPOSE:     Header file for ped entity base class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPedSA;

#pragma once

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

#define FUNC_SetModelIndex                          0x5E4880
#define FUNC_AttachPedToEntity                      0x5E7CB0
#define FUNC_RestorePlayerStuffDuringResurrection   0x442060
#define FUNC_SetIsStanding                          0x4ABBE0
#define FUNC_MakeChangesForNewWeapon_Slot           0x60D000

#define FUNC_CPedClothesDesc__SetTextureAndModel    0x5A8080
#define FUNC_CClothes__RebuildPlayer                0x5A82C0
#define FUNC_CAEPedWeaponAudioEntity__AddAudioEvent 0x4E69F0

#define FUNC_QuitEnteringCar                        0x650130 // really belongs in CCarEnterExit

#define FUNC_SetCurrentWeapon           0x5E61F0
#define FUNC_GiveWeapon                 0x5E6080
#define FUNC_GetBonePosition            0x5E4280
#define FUNC_GetTransformedBonePosition 0x5E01C0
#define FUNC_IsWearingGoggles           0x479D10
#define FUNC_TakeOffGoggles             0x5E6010
#define FUNC_PutOnGoggles               0x5E3AE0
#define FUNC_RemoveWeaponModel          0x5E3990
#define FUNC_RemoveGogglesModel         0x5DF170
#define FUNC_ClearWeapon                0x5E62B0
#define FUNC_DetachPedFromEntity        0x5E7EC0
#define FUNC_CPed_RemoveBodyPart        0x5f0140
#define FUNC_PreRenderAfterTest         0x5E65A0

#define VAR_LocalPlayer                 0x94AD28

//+1328 = Ped state
//+1344 = ped health
class CPedFlags
{
public:
    std::uint32_t bIsStanding : 1;                     // is ped standing on something
    std::uint32_t bWasStanding : 1;                    // was ped standing on something
    std::uint32_t bIsLooking : 1;                      // is ped looking at something or in a direction
    std::uint32_t bIsRestoringLook : 1;                // is ped restoring head postion from a look
    std::uint32_t bIsAimingGun : 1;                    // is ped aiming gun
    std::uint32_t bIsRestoringGun : 1;                 // is ped moving gun back to default posn
    std::uint32_t bCanPointGunAtTarget : 1;            // can ped point gun at target
    std::uint32_t bIsTalking : 1;                      // is ped talking(see Chat())

    std::uint32_t bInVehicle : 1;                        // is in a vehicle
    std::uint32_t bIsInTheAir : 1;                       // is in the air
    std::uint32_t bIsLanding : 1;                        // is landing after being in the air
    std::uint32_t bHitSomethingLastFrame : 1;            // has been in a collision last fram
    std::uint32_t bIsNearCar : 1;                        // has been in a collision last fram
    std::uint32_t bRenderPedInCar : 1;                   // has been in a collision last fram
    std::uint32_t bUpdateAnimHeading : 1;                // update ped heading due to heading change during anim sequence
    std::uint32_t bRemoveHead : 1;                       // waiting on AntiSpazTimer to remove head

    std::uint32_t bFiringWeapon : 1;                   // is pulling trigger
    std::uint32_t bHasACamera : 1;                     // does ped possess a camera to document accidents
    std::uint32_t bPedIsBleeding : 1;                  // Ped loses a lot of blood if true
    std::uint32_t bStopAndShoot : 1;                   // Ped cannot reach target to attack with fist, need to use gun
    std::uint32_t bIsPedDieAnimPlaying : 1;            // is ped die animation finished so can dead now
    std::uint32_t bStayInSamePlace : 1;                // when set, ped stays put
    std::uint32_t
        bKindaStayInSamePlace : 1;                    // when set, ped doesn't seek out opponent or cover large distances. Will still shuffle and look for cover
    std::uint32_t bBeingChasedByPolice : 1;            // use nodes for routefind

    std::uint32_t bNotAllowedToDuck : 1;              // Is this ped allowed to duck at all?
    std::uint32_t bCrouchWhenShooting : 1;            // duck behind cars etc
    std::uint32_t bIsDucking : 1;                     // duck behind cars etc
    std::uint32_t bGetUpAnimStarted : 1;              // don't want to play getup anim if under something
    std::uint32_t bDoBloodyFootprints : 1;            // std::uint32_t bIsLeader :1;
    std::uint32_t bDontDragMeOutCar : 1;
    std::uint32_t bStillOnValidPoly : 1;            // set if the polygon the ped is on is still valid for collision
    std::uint32_t bAllowMedicsToReviveMe : 1;

    std::uint32_t bResetWalkAnims : 1;
    std::uint32_t bOnBoat : 1;                          // flee but only using nodes
    std::uint32_t bBusJacked : 1;                       // flee but only using nodes
    std::uint32_t bFadeOut : 1;                         // set if you want ped to fade out
    std::uint32_t bKnockedUpIntoAir : 1;                // has ped been knocked up into the air by a car collision
    std::uint32_t bHitSteepSlope : 1;                   // has ped collided/is standing on a steep slope (surface type)
    std::uint32_t bCullExtraFarAway : 1;                // special ped only gets culled if it's extra far away (for roadblocks)
    std::uint32_t bTryingToReachDryLand : 1;            // has ped just exited boat and trying to get to dry land

    std::uint32_t bCollidedWithMyVehicle : 1;
    std::uint32_t bRichFromMugging : 1;                   // ped has lots of cash cause they've been mugging people
    std::uint32_t bChrisCriminal : 1;                     // Is a criminal as killed during Chris' police mission (should be counted as such)
    std::uint32_t bShakeFist : 1;                         // test shake hand at look entity
    std::uint32_t bNoCriticalHits : 1;                    // ped cannot be killed by a single bullet
    std::uint32_t bHasAlreadyBeenRecorded : 1;            // Used for replays
    std::uint32_t bUpdateMatricesRequired : 1;            // if PedIK has altered bones so matrices need updated this frame
    std::uint32_t bFleeWhenStanding : 1;                  //

    std::uint32_t bMiamiViceCop : 1;                         //
    std::uint32_t bMoneyHasBeenGivenByScript : 1;            //
    std::uint32_t bHasBeenPhotographed : 1;                  //
    std::uint32_t bIsDrowning : 1;
    std::uint32_t bDrownsInWater : 1;
    std::uint32_t bHeadStuckInCollision : 1;
    std::uint32_t bDeadPedInFrontOfCar : 1;
    std::uint32_t bStayInCarOnJack : 1;

    std::uint32_t bDontFight : 1;
    std::uint32_t bDoomAim : 1;
    std::uint32_t bCanBeShotInVehicle : 1;
    std::uint32_t bPushedAlongByCar : 1;            // ped is getting pushed along by car collision (so don't take damage from horz velocity)
    std::uint32_t bNeverEverTargetThisPed : 1;
    std::uint32_t bThisPedIsATargetPriority : 1;
    std::uint32_t bCrouchWhenScared : 1;
    std::uint32_t bKnockedOffBike : 1;

    std::uint32_t bDonePositionOutOfCollision : 1;
    std::uint32_t bDontRender : 1;
    std::uint32_t bHasBeenAddedToPopulation : 1;
    std::uint32_t bHasJustLeftCar : 1;
    std::uint32_t bIsInDisguise : 1;
    std::uint32_t bDoesntListenToPlayerGroupCommands : 1;
    std::uint32_t bIsBeingArrested : 1;
    std::uint32_t bHasJustSoughtCover : 1;

    std::uint32_t bKilledByStealth : 1;
    std::uint32_t bDoesntDropWeaponsWhenDead : 1;
    std::uint32_t bCalledPreRender : 1;
    std::uint32_t bBloodPuddleCreated : 1;            // Has a static puddle of blood been created yet
    std::uint32_t bPartOfAttackWave : 1;
    std::uint32_t bClearRadarBlipOnDeath : 1;
    std::uint32_t bNeverLeavesGroup : 1;                   // flag that we want to test 3 extra spheres on col model
    std::uint32_t bTestForBlockedPositions : 1;            // this sets these indicator flags for various posisions on the front of the ped

    std::uint32_t bRightArmBlocked : 1;
    std::uint32_t bLeftArmBlocked : 1;
    std::uint32_t bDuckRightArmBlocked : 1;
    std::uint32_t bMidriffBlockedForJump : 1;
    std::uint32_t bFallenDown : 1;
    std::uint32_t bUseAttractorInstantly : 1;
    std::uint32_t bDontAcceptIKLookAts : 1;
    std::uint32_t bHasAScriptBrain : 1;

    std::uint32_t bWaitingForScriptBrainToLoad : 1;
    std::uint32_t bHasGroupDriveTask : 1;
    std::uint32_t bCanExitCar : 1;
    std::uint32_t CantBeKnockedOffBike : 2;            // 0=Default(harder for mission peds) 1=Never 2=Always normal(also for mission peds)
    std::uint32_t bHasBeenRendered : 1;
    std::uint32_t bIsCached : 1;
    std::uint32_t bPushOtherPeds : 1;            // GETS RESET EVERY FRAME - SET IN TASK: want to push other peds around (eg. leader of a group or ped trying to
                                                // get in a car)
    std::uint32_t bHasBulletProofVest : 1;

    std::uint32_t bUsingMobilePhone : 1;
    std::uint32_t bUpperBodyDamageAnimsOnly : 1;
    std::uint32_t bStuckUnderCar : 1;
    std::uint32_t bKeepTasksAfterCleanUp : 1;            // If true ped will carry on with task even after cleanup
    std::uint32_t bIsDyingStuck : 1;
    std::uint32_t bIgnoreHeightCheckOnGotoPointTask : 1;            // set when walking round buildings, reset when task quits
    std::uint32_t bForceDieInCar : 1;
    std::uint32_t bCheckColAboveHead : 1;

    std::uint32_t bIgnoreWeaponRange : 1;
    std::uint32_t bDruggedUp : 1;
    std::uint32_t bWantedByPolice : 1;            // if this is set, the cops will always go after this ped when they are doing a KillCriminal task
    std::uint32_t bSignalAfterKill : 1;
    std::uint32_t bCanClimbOntoBoat : 1;
    std::uint32_t bPedHitWallLastFrame : 1;            // useful to store this so that AI knows (normal will still be available)
    std::uint32_t bIgnoreHeightDifferenceFollowingNodes : 1;
    std::uint32_t bMoveAnimSpeedHasBeenSetByTask : 1;

    std::uint32_t bGetOutUpsideDownCar : 1;
    std::uint32_t bJustGotOffTrain : 1;
    std::uint32_t bDeathPickupsPersist : 1;
    std::uint32_t bTestForShotInVehicle : 1;
    // #ifdef GTA_REPLAY
    std::uint32_t bUsedForReplay : 1;            // This ped is controlled by replay and should be removed when replay is done.
    // #endif
};

class CPedWeaponAudioEntitySAInterface
{
public:
};

class CPedSAInterface : public CPhysicalSAInterface            // +1420  = current vehicle   312 first byte
{
public:
    // current weapon slot 1184 ( and +1816?)
    // CPedIKSAInterface     pedIK; // 528
    // CWeaponSAInterface    Weapons[9]; // 1032
    BYTE                             bPad[348];
    CPedSoundSAInterface             pedSound;
    BYTE                             bPad11[256 - sizeof(CPedSoundSAInterface)];
    CPedWeaponAudioEntitySAInterface weaponAudioEntity;
    BYTE                             bPad12[216 - sizeof(CPedWeaponAudioEntitySAInterface)];
    CPedFlags                        pedFlags;            // 1132 (16 bytes long including alignment probably)
    CPedIntelligenceSAInterface*     pPedIntelligence;
    CPlayerPedDataSAInterface*       pPlayerData;            // 1152
    BYTE                             bPad4a[4];
    void*                            pedNodes[19];
    int                              iMoveAnimGroup;            // 1236
    BYTE                             bPad4b[52];
    CPedIKSAInterface                pedIK;            // 1292 (length 32 bytes)
    int                              bPad5[5];

    float fHealth;
    int   iUnknown121;
    float fArmor;

    int iUnknown313[3];            // +1356
    // +1368 = rotation
    float               fCurrentRotation;
    float               fTargetRotation;
    float               fRotationSpeed;
    BYTE                bPad8[4];
    CEntitySAInterface* pContactEntity;
    BYTE                bPad3[32];
    CEntitySAInterface* CurrentObjective;            // current vehicle    1420
    BYTE                bPad2[8];                    // 1424
    BYTE                bPedType;                    // ped type? 0 = player, >1 = ped?  // 1432
    BYTE                bPad9[7];
    CWeaponSAInterface  Weapons[WEAPONSLOT_MAX];
    // weapons at +1440 ends at +1804
    BYTE                bPad4[12];
    BYTE                bCurrentWeaponSlot;            // is actually here
    BYTE                bPad6[20];
    BYTE                bFightingStyle;            // 1837
    BYTE                bFightingStyleExtra;
    BYTE                bPad7[1];
    CFireSAInterface*   pFireOnPed;
    BYTE                bPad10[104];
    CEntitySAInterface* pTargetedEntity;            // 1948
};

class CPedSA : public virtual CPed, public virtual CPhysicalSA
{
    friend class CPoolsSA;

private:
    CWeaponSA*          m_pWeapons[WEAPONSLOT_MAX];
    CPedIKSA*           m_pPedIK;
    CPedIntelligenceSA* m_pPedIntelligence;
    CPedSAInterface*    m_pPedInterface;
    CPedSoundSA*        m_pPedSound;
    CPedSoundSA*        m_pDefaultPedSound;

    DWORD         m_dwType;
    std::uint8_t m_ucOccupiedSeat;

protected:
    int m_iCustomMoveAnim;

public:
    CPedSA();
    CPedSA(CPedSAInterface* pedInterface);
    ~CPedSA();

    void             SetInterface(CEntitySAInterface* intInterface);
    CPedSAInterface* GetPedInterface() { return (CPedSAInterface*)GetInterface(); }
    void             Init();
    void             SetModelIndex(DWORD dwModelIndex);
    void             RemoveGeometryRef();
    void             AttachPedToEntity(DWORD dwEntityInterface, CVector* vector, std::uint16_t sDirection, float fRotationLimit, eWeaponType weaponType,
                                       bool bChangeCamera);
    void             DetachPedFromEntity();

    CVehicle* GetVehicle();
    void      Respawn(CVector* position, bool bCameraCut);
    bool      AddProjectile(eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector* target, CEntity* targetEntity);

    float GetHealth();
    void  SetHealth(float fHealth);

    float GetArmor();
    void  SetArmor(float fArmor);

    float GetOxygenLevel();
    void  SetOxygenLevel(float fOxygen);

    CWeapon* GiveWeapon(eWeaponType weaponType, std::uint32_t uiAmmo, eWeaponSkill skill);
    CWeapon* GetWeapon(eWeaponSlot weaponSlot);
    CWeapon* GetWeapon(eWeaponType weaponType);
    void     ClearWeapons();
    void     RemoveWeaponModel(int iModel);
    void     ClearWeapon(eWeaponType weaponType);

    void              SetIsStanding(bool bStanding);
    CPedIntelligence* GetPedIntelligence() { return m_pPedIntelligence; }
    CPedSound*        GetPedSound() { return m_pPedSound; }
    DWORD             GetType();
    void              SetType(DWORD dwType);

    virtual void RestoreLastGoodPhysicsState();
    float        GetCurrentRotation();
    float        GetTargetRotation();
    void         SetCurrentRotation(float fRotation);
    void         SetTargetRotation(float fRotation);

    eWeaponSlot GetCurrentWeaponSlot();
    void        SetCurrentWeaponSlot(eWeaponSlot weaponSlot);

    CVector* GetBonePosition(eBone bone, CVector* vecPosition);
    CVector* GetTransformedBonePosition(eBone bone, CVector* vecPosition);
    void     ApplySwimAndSlopeRotations();

    bool IsDucking();
    void SetDucking(bool bDuck);

    bool IsInWater();

    int  GetCantBeKnockedOffBike();
    void SetCantBeKnockedOffBike(int iCantBeKnockedOffBike);
    void QuitEnteringCar(CVehicle* vehicle, int iSeat, bool bUnknown);

    bool IsWearingGoggles();
    void SetGogglesState(bool bIsWearingThem);

    void SetClothesTextureAndModel(const char* szTexture, const char* szModel, int textureType);
    void RebuildPlayer();

    eFightingStyle GetFightingStyle();
    void           SetFightingStyle(eFightingStyle style, BYTE bStyleExtra = 6);

    CEntity* GetContactEntity();

    std::uint8_t GetRunState();

    CEntity* GetTargetedEntity();
    void     SetTargetedEntity(CEntity* pEntity);

    bool GetCanBeShotInVehicle();
    bool GetTestForShotInVehicle();

    void SetCanBeShotInVehicle(bool bShot);
    void SetTestForShotInVehicle(bool bTest);

    bool InternalAttachEntityToEntity(DWORD dwEntityInterface, const CVector* vecPosition, const CVector* vecRotation);

    BYTE GetOccupiedSeat() { return m_ucOccupiedSeat; }
    void SetOccupiedSeat(BYTE seat) { m_ucOccupiedSeat = seat; }

    void RemoveBodyPart(int i, char c);

    void         SetFootBlood(std::uint32_t uiFootBlood);
    std::uint32_t GetFootBlood();

    bool IsBleeding();
    void SetBleeding(bool bBleeding);

    bool IsOnFire();
    void SetOnFire(bool bOnFire);

    bool GetStayInSamePlace() { return GetPedInterface()->pedFlags.bStayInSamePlace; }
    void SetStayInSamePlace(bool bStay);

    void GetVoice(short* psVoiceType, short* psVoiceID);
    void GetVoice(const char** pszVoiceType, const char** pszVoice);
    void SetVoice(short sVoiceType, short sVoiceID);
    void SetVoice(const char* szVoiceType, const char* szVoice);
    void ResetVoice();
    void SetLanding(bool bIsLanding) { GetPedInterface()->pedFlags.bIsLanding = bIsLanding; }
    void SetUpdateMetricsRequired(bool required) { GetPedInterface()->pedFlags.bUpdateMatricesRequired = required; }

    CWeaponStat* GetCurrentWeaponStat();
    float        GetCurrentWeaponRange();
    void         AddWeaponAudioEvent(EPedWeaponAudioEventType audioEventType);

    virtual int GetCustomMoveAnim();
    bool        IsDoingGangDriveby();

    CPedIKSAInterface*      GetPedIKInterface() { return &reinterpret_cast<CPedSAInterface*>(m_pInterface)->pedIK; }
    void*                   GetPedNodeInterface(std::int32_t nodeId) { return reinterpret_cast<CPedSAInterface*>(m_pInterface)->pedNodes[nodeId]; }
    std::unique_ptr<CPedIK> GetPedIK() { return std::make_unique<CPedIKSA>(GetPedIKInterface()); }
    static void             StaticSetHooks();
};
