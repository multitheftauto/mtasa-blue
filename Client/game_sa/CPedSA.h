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

#pragma once

#include <game/CPed.h>
#include <game/CWeapon.h>

#include "CEntitySA.h"
#include "CFireSA.h"
#include "CPedIKSA.h"
#include "CPhysicalSA.h"
#include "CPedSoundSA.h"
#include "CPoolsSA.h"
#include "CWeaponSA.h"
#include "CPedIntelligenceSA.h"

class CPlayerPedDataSAInterface;
class CVehicleSAInterface;

// CPed
#define FUNC_AttachPedToEntity          0x5E7CB0            // CPed::AttachPedToEntity
#define FUNC_DetachPedFromEntity        0x5E7EC0            // CPed::DettachPedFromEntity
#define FUNC_SetModelIndex              0x5E4880            // CPed::SetModelIndex
#define FUNC_SetIsStanding              0x4ABBE0            // CPed::SetIsStanding
#define FUNC_SetCurrentWeapon           0x5E61F0            // CPed::SetCurrentWeapon
#define FUNC_GiveWeapon                 0x5E6080            // CPed::GiveWeapon
#define FUNC_ClearWeapon                0x5E62B0            // CPed::ClearWeapon
#define FUNC_RemoveWeaponModel          0x5E3990            // CPed::RemoveWeaponModel
#define FUNC_GetBonePosition            0x5E4280            // CPed::GetBonePosition
#define FUNC_GetTransformedBonePosition 0x5E01C0            // CPed::GetTransformedBonePosition
#define FUNC_TakeOffGoggles             0x5E6010            // CPed::TakeOffGoggles
#define FUNC_PutOnGoggles               0x5E3AE0            // CPed::PutOnGoggles
#define FUNC_RemoveGogglesModel         0x5DF170            // CPed::RemoveGogglesModel
#define FUNC_CPed_RemoveBodyPart        0x5F0140            // CPed::RemoveBodyPart
#define FUNC_PreRenderAfterTest         0x5E65A0            // CPed::PreRenderAfterTest

// CPlayerPed
#define FUNC_MakeChangesForNewWeapon_Slot 0x60D000            // CPlayerPed::MakeChangesForNewWeapon

// CPedClothesDesc && CClothes
#define FUNC_CPedClothesDesc__SetTextureAndModel 0x5A8080            // CPedClothesDesc::SetTextureAndModel
#define FUNC_CClothes__RebuildPlayer             0x5A82C0            // CClothes::RebuildPlayer

// CAEPedWeaponAudioEntity
#define FUNC_CAEPedWeaponAudioEntity__AddAudioEvent 0x4E69F0            // CAEPedWeaponAudioEntity::AddAudioEvent

// CGameLogic
#define FUNC_RestorePlayerStuffDuringResurrection 0x442060            // CGameLogic::RestorePlayerStuffDuringResurrection
#define FUNC_SortOutStreamingAndMemory            0x441440            // CGameLogic::SortOutStreamingAndMemory

class CPedFlags
{
public:
    // m_nPedFlags
    unsigned int bIsStanding : 1;                     // is ped standing on something
    unsigned int bWasStanding : 1;                    // was ped standing on something
    unsigned int bIsLooking : 1;                      // is ped looking at something or in a direction
    unsigned int bIsRestoringLook : 1;                // is ped restoring head postion from a look
    unsigned int bIsAimingGun : 1;                    // is ped aiming gun
    unsigned int bIsRestoringGun : 1;                 // is ped moving gun back to default posn
    unsigned int bCanPointGunAtTarget : 1;            // can ped point gun at target
    unsigned int bIsTalking : 1;                      // is ped talking(see Chat())

    unsigned int bInVehicle : 1;                        // is in a vehicle
    unsigned int bIsInTheAir : 1;                       // is in the air
    unsigned int bIsLanding : 1;                        // is landing after being in the air
    unsigned int bHitSomethingLastFrame : 1;            // has been in a collision last fram
    unsigned int bIsNearCar : 1;                        // has been in a collision last fram
    unsigned int bRenderPedInCar : 1;                   // has been in a collision last fram
    unsigned int bUpdateAnimHeading : 1;                // update ped heading due to heading change during anim sequence
    unsigned int bRemoveHead : 1;                       // waiting on AntiSpazTimer to remove head

    unsigned int bFiringWeapon : 1;                   // is pulling trigger
    unsigned int bHasACamera : 1;                     // does ped possess a camera to document accidents
    unsigned int bPedIsBleeding : 1;                  // Ped loses a lot of blood if true
    unsigned int bStopAndShoot : 1;                   // Ped cannot reach target to attack with fist, need to use gun
    unsigned int bIsPedDieAnimPlaying : 1;            // is ped die animation finished so can dead now
    unsigned int bStayInSamePlace : 1;                // when set, ped stays put
    unsigned int bKindaStayInSamePlace : 1;                    // when set, ped doesn't seek out opponent or cover large distances. Will still shuffle and look for cover
    unsigned int bBeingChasedByPolice : 1;            // use nodes for routefind

    unsigned int bNotAllowedToDuck : 1;              // Is this ped allowed to duck at all?
    unsigned int bCrouchWhenShooting : 1;            // duck behind cars etc
    unsigned int bIsDucking : 1;                     // duck behind cars etc
    unsigned int bGetUpAnimStarted : 1;              // don't want to play getup anim if under something
    unsigned int bDoBloodyFootprints : 1;            // unsigned int bIsLeader :1;
    unsigned int bDontDragMeOutCar : 1;
    unsigned int bStillOnValidPoly : 1;            // set if the polygon the ped is on is still valid for collision
    unsigned int bAllowMedicsToReviveMe : 1;

    // m_nSecondPedFlags
    unsigned int bResetWalkAnims : 1;
    unsigned int bOnBoat : 1;                          // flee but only using nodes
    unsigned int bBusJacked : 1;                       // flee but only using nodes
    unsigned int bFadeOut : 1;                         // set if you want ped to fade out
    unsigned int bKnockedUpIntoAir : 1;                // has ped been knocked up into the air by a car collision
    unsigned int bHitSteepSlope : 1;                   // has ped collided/is standing on a steep slope (surface type)
    unsigned int bCullExtraFarAway : 1;                // special ped only gets culled if it's extra far away (for roadblocks)
    unsigned int bTryingToReachDryLand : 1;            // has ped just exited boat and trying to get to dry land

    unsigned int bCollidedWithMyVehicle : 1;
    unsigned int bRichFromMugging : 1;                   // ped has lots of cash cause they've been mugging people
    unsigned int bChrisCriminal : 1;                     // Is a criminal as killed during Chris' police mission (should be counted as such)
    unsigned int bShakeFist : 1;                         // test shake hand at look entity
    unsigned int bNoCriticalHits : 1;                    // ped cannot be killed by a single bullet
    unsigned int bHasAlreadyBeenRecorded : 1;            // Used for replays
    unsigned int bUpdateMatricesRequired : 1;            // if PedIK has altered bones so matrices need updated this frame
    unsigned int bFleeWhenStanding : 1;                  //

    unsigned int bMiamiViceCop : 1;                         //
    unsigned int bMoneyHasBeenGivenByScript : 1;            //
    unsigned int bHasBeenPhotographed : 1;                  //
    unsigned int bIsDrowning : 1;
    unsigned int bDrownsInWater : 1;
    unsigned int bHeadStuckInCollision : 1;
    unsigned int bDeadPedInFrontOfCar : 1;
    unsigned int bStayInCarOnJack : 1;

    unsigned int bDontFight : 1;
    unsigned int bDoomAim : 1;
    unsigned int bCanBeShotInVehicle : 1;
    unsigned int bPushedAlongByCar : 1;            // ped is getting pushed along by car collision (so don't take damage from horz velocity)
    unsigned int bNeverEverTargetThisPed : 1;
    unsigned int bThisPedIsATargetPriority : 1;
    unsigned int bCrouchWhenScared : 1;
    unsigned int bKnockedOffBike : 1;

    // m_nThirdPedFlags
    unsigned int bDonePositionOutOfCollision : 1;
    unsigned int bDontRender : 1;
    unsigned int bHasBeenAddedToPopulation : 1;
    unsigned int bHasJustLeftCar : 1;
    unsigned int bIsInDisguise : 1;
    unsigned int bDoesntListenToPlayerGroupCommands : 1;
    unsigned int bIsBeingArrested : 1;
    unsigned int bHasJustSoughtCover : 1;

    unsigned int bKilledByStealth : 1;
    unsigned int bDoesntDropWeaponsWhenDead : 1;
    unsigned int bCalledPreRender : 1;
    unsigned int bBloodPuddleCreated : 1;            // Has a static puddle of blood been created yet
    unsigned int bPartOfAttackWave : 1;
    unsigned int bClearRadarBlipOnDeath : 1;
    unsigned int bNeverLeavesGroup : 1;                   // flag that we want to test 3 extra spheres on col model
    unsigned int bTestForBlockedPositions : 1;            // this sets these indicator flags for various posisions on the front of the ped

    unsigned int bRightArmBlocked : 1;
    unsigned int bLeftArmBlocked : 1;
    unsigned int bDuckRightArmBlocked : 1;
    unsigned int bMidriffBlockedForJump : 1;
    unsigned int bFallenDown : 1;
    unsigned int bUseAttractorInstantly : 1;
    unsigned int bDontAcceptIKLookAts : 1;
    unsigned int bHasAScriptBrain : 1;

    unsigned int bWaitingForScriptBrainToLoad : 1;
    unsigned int bHasGroupDriveTask : 1;
    unsigned int bCanExitCar : 1;
    unsigned int CantBeKnockedOffBike : 2;            // 0=Default(harder for mission peds) 1=Never 2=Always normal(also for mission peds)
    unsigned int bHasBeenRendered : 1;
    unsigned int bIsCached : 1;
    unsigned int bPushOtherPeds : 1;            // GETS RESET EVERY FRAME - SET IN TASK: want to push other peds around (eg. leader of a group or ped trying to
                                                // get in a car)

    // m_nFourthPedFlags
    unsigned int bHasBulletProofVest : 1;
    unsigned int bUsingMobilePhone : 1;
    unsigned int bUpperBodyDamageAnimsOnly : 1;
    unsigned int bStuckUnderCar : 1;
    unsigned int bKeepTasksAfterCleanUp : 1;            // If true ped will carry on with task even after cleanup
    unsigned int bIsDyingStuck : 1;
    unsigned int bIgnoreHeightCheckOnGotoPointTask : 1;            // set when walking round buildings, reset when task quits
    unsigned int bForceDieInCar : 1;
    unsigned int bCheckColAboveHead : 1;

    unsigned int bIgnoreWeaponRange : 1;
    unsigned int bDruggedUp : 1;
    unsigned int bWantedByPolice : 1;            // if this is set, the cops will always go after this ped when they are doing a KillCriminal task
    unsigned int bSignalAfterKill : 1;
    unsigned int bCanClimbOntoBoat : 1;
    unsigned int bPedHitWallLastFrame : 1;            // useful to store this so that AI knows (normal will still be available)
    unsigned int bIgnoreHeightDifferenceFollowingNodes : 1;
    unsigned int bMoveAnimSpeedHasBeenSetByTask : 1;

    unsigned int bGetOutUpsideDownCar : 1;
    unsigned int bJustGotOffTrain : 1;
    unsigned int bDeathPickupsPersist : 1;
    unsigned int bTestForShotInVehicle : 1;
    unsigned int bUsedForReplay : 1;            // This ped is controlled by replay and should be removed when replay is done.
};

class CPedSAInterface : public CPhysicalSAInterface
{
public:
    CPedSoundEntitySAInterface       pedAudio;            // CAEPedAudioEntity
    CPedSoundSAInterface             pedSound;            // CAEPedSpeechAudioEntity
    CPedWeaponAudioEntitySAInterface weaponAudioEntity;
    std::uint8_t                     unk_43C[36];
    std::uint8_t                     unk_460[8];
    int                              unk_468;
    CPedFlags                        pedFlags;
    CPedIntelligenceSAInterface*     pPedIntelligence;
    CPlayerPedDataSAInterface*       pPlayerData;
    std::uint8_t                     createdBy;
    std::uint8_t                     _pad0[3];
    void*                            pedNodes[19];            // AnimBlendFrameData*
    int                              iMoveAnimGroup;
    CVector2D                        vecAnimMovingShiftLocal;
    std::uint8_t                     pedAcquaintance[20];            // CPedAcquaintance
    RpClump*                         pWeaponObject;
    RwFrame*                         pGunflashObject;
    RpClump*                         pGogglesObject;
    bool*                            pGogglesState;
    std::int16_t                     weaponGunflashStateLeftHand;
    std::int16_t                     unk_506;
    std::int16_t                     weaponGunflashStateRightHand;
    std::int16_t                     unk_50A;
    CPedIKSAInterface                pedIK;
    int                              unk_52C;
    int                              pedState;
    int                              moveState;
    int                              swimmingMoveState;
    int                              unk_53C;
    float                            fHealth;
    float                            fMaxHealth;
    float                            fArmor;
    DWORD                            dwTimeTillWeNeedThisPed;
    CVector2D                        vecAnimMovingShift;
    float                            fCurrentRotation;
    float                            fTargetRotation;
    float                            fRotationSpeed;
    float                            fMoveAnim;
    CEntitySAInterface*              pContactEntity;
    CVector                          unk_56C;
    CVector                          unk_578;
    CEntitySAInterface*              pLastContactEntity;
    CVehicleSAInterface*             pLastVehicle;
    CVehicleSAInterface*             pVehicle;
    int                              unk_590;
    int                              unk_594;
    int                              bPedType;            // ped type? 0 = player, >1 = ped?
    void*                            pPedStats;
    CWeaponSAInterface               Weapons[WEAPONSLOT_MAX];
    int                              savedWeapon;
    int                              delayedWeapon;
    DWORD                            dwDelayedWeaponAmmo;
    std::uint8_t                     bCurrentWeaponSlot;
    std::uint8_t                     weaponShootingRate;
    std::uint8_t                     weaponAccuracy;
    std::uint8_t                     _pad;
    CEntitySAInterface*              pTargetedObject;
    int                              unk_720;
    int                              unk_724;
    int                              unk_728;
    std::uint8_t                     weaponSkill;
    std::uint8_t                     bFightingStyle;
    std::uint8_t                     bFightingStyleExtra;
    std::uint8_t                     bPad7;
    CFireSAInterface*                pFireOnPed;
    int                              unk_734;
    CEntitySAInterface*              pLookAtEntity;
    float                            fLookHeading;
    DWORD                            dwWeaponModelID;
    int                              unk_744;
    DWORD                            dwLookTime;
    int                              unk_74C;
    DWORD                            dwTimeWhenDead;            // death time in MS
    std::uint8_t                     bodyPartToRemove;
    std::int16_t                     unk_755;
    std::int16_t                     moneyCount;
    std::int16_t                     unk_758;
    int                              unk_75C;
    std::uint8_t                     lastWeaponDamage;
    std::uint8_t                     unk_761[3];
    CEntitySAInterface*              pTargetedEntity;
    std::int16_t                     unk_768;
    CVector                          vecTurretOffset;
    float                            fTurretAngleA;
    float                            fTurretAngleB;
    DWORD                            dwTurretPosnMode;
    DWORD                            dwTurretAmmo;
    void*                            pCoverPoint;            // CCoverPoint*
    void*                            pEnex;                  // CEntryExit*
    float                            fRemovalDistMultiplier;
    std::int16_t                     specialModelIndex;
    std::int16_t                     unk_796;
    int                              unk_798;
};
static_assert(sizeof(CPedSAInterface) == 0x79C, "Invalid size for CPedSAInterface");

class CPedSA : public virtual CPed, public virtual CPhysicalSA
{
    friend class CPoolsSA;

private:
    CWeaponSA*          m_pWeapons[WEAPONSLOT_MAX]{};
    CPedIKSA*           m_pPedIK{};
    CPedIntelligenceSA* m_pPedIntelligence{};
    CPedSAInterface*    m_pPedInterface{};
    CPedSoundSA*        m_pPedSound{};

    std::int16_t m_sDefaultVoiceType;
    std::int16_t m_sDefaultVoiceID;

    DWORD         m_dwType;
    std::uint8_t  m_ucOccupiedSeat;

protected:
    int m_iCustomMoveAnim{ 0 };

public:
    CPedSA(CPedSAInterface* pedInterface = nullptr);
    ~CPedSA();

    void             SetInterface(CEntitySAInterface* intInterface) noexcept { m_pInterface = intInterface; }
    CPedSAInterface* GetPedInterface() noexcept { return static_cast<CPedSAInterface*>(m_pInterface); }
    const CPedSAInterface* GetPedInterface() const noexcept { return static_cast<const CPedSAInterface*>(m_pInterface); }
    void             Init();
    
    void             SetModelIndex(DWORD modelIndex);
    void             AttachPedToEntity(DWORD entityInteface, CVector* vector, std::uint16_t direction, float rotationLimit, eWeaponType weaponType, bool changeCamera);
    void             DetachPedFromEntity();

    CVehicle* GetVehicle() const noexcept;
    void      Respawn(const CVector* position, bool cameraCut);
    bool      AddProjectile(eWeaponType weaponType, CVector origin, float force, const CVector* target, const CEntity* targetEntity);

    float GetHealth() const { return GetPedInterface()->fHealth; }
    void  SetHealth(float health) { GetPedInterface()->fHealth = health; }

    float GetArmor() const { return GetPedInterface()->fArmor; }
    void  SetArmor(float armor) { GetPedInterface()->fArmor = armor; }

    float GetOxygenLevel() const;
    void SetOxygenLevel(float oxygen);

    CWeapon* GiveWeapon(eWeaponType weaponType, std::uint32_t ammo, eWeaponSkill skill);
    CWeapon* GetWeapon(eWeaponSlot weaponSlot) const noexcept;
    CWeapon* GetWeapon(eWeaponType weaponType) const noexcept;
    void     ClearWeapons() noexcept;
    void     RemoveWeaponModel(int model);
    void     ClearWeapon(eWeaponType weaponType);

    void              SetIsStanding(bool standing);
    CPedIntelligence* GetPedIntelligence() const noexcept { return m_pPedIntelligence; }
    CPedSound*        GetPedSound() const noexcept { return m_pPedSound; }
    DWORD             GetType() const noexcept { return m_dwType; }
    void              SetType(DWORD type) noexcept { m_dwType = type; }

    virtual void RestoreLastGoodPhysicsState();
    float        GetCurrentRotation() const { return GetPedInterface()->fCurrentRotation; }
    float        GetTargetRotation() const { return GetPedInterface()->fTargetRotation; }
    void         SetCurrentRotation(float rotation) { GetPedInterface()->fCurrentRotation = rotation; }
    void         SetTargetRotation(float rotation) { GetPedInterface()->fTargetRotation = rotation; }

    eWeaponSlot GetCurrentWeaponSlot() const { return static_cast<eWeaponSlot>(GetPedInterface()->bCurrentWeaponSlot); }
    void        SetCurrentWeaponSlot(eWeaponSlot weaponSlot);

    CVector* GetBonePosition(eBone bone, CVector* position);
    CVector* GetTransformedBonePosition(eBone bone, CVector* position);
    void     ApplySwimAndSlopeRotations();

    bool IsDucking() const { return GetPedInterface()->pedFlags.bIsDucking; }
    void SetDucking(bool duck) { GetPedInterface()->pedFlags.bIsDucking = duck; }

    bool IsInWater() const noexcept;

    int  GetCantBeKnockedOffBike() const { return GetPedInterface()->pedFlags.CantBeKnockedOffBike; }
    void SetCantBeKnockedOffBike(int cantBeKnockedOffBike) { GetPedInterface()->pedFlags.CantBeKnockedOffBike = cantBeKnockedOffBike; }

    bool IsWearingGoggles() const { return GetPedInterface()->pGogglesObject != nullptr; }
    void SetGogglesState(bool isWearingThem);

    void SetClothesTextureAndModel(const char* texture, const char* model, int textureType);
    void RebuildPlayer();

    eFightingStyle GetFightingStyle() const { return static_cast<eFightingStyle>(GetPedInterface()->bFightingStyle); }
    void           SetFightingStyle(eFightingStyle style, std::uint8_t styleExtra = 6);

    CEntity* GetContactEntity() const noexcept;

    std::uint8_t GetRunState() const { return static_cast<std::uint8_t>(GetPedInterface()->moveState); }

    CEntity* GetTargetedEntity() const noexcept;
    void     SetTargetedEntity(CEntity* targetEntity);

    bool GetCanBeShotInVehicle() const { return GetPedInterface()->pedFlags.bCanBeShotInVehicle; }
    bool GetTestForShotInVehicle() const { return GetPedInterface()->pedFlags.bTestForShotInVehicle; }

    void SetCanBeShotInVehicle(bool shot) { GetPedInterface()->pedFlags.bCanBeShotInVehicle = shot; }
    void SetTestForShotInVehicle(bool test) { GetPedInterface()->pedFlags.bTestForShotInVehicle = test; }

    bool InternalAttachEntityToEntity(DWORD entityInterface, const CVector* position, const CVector* rotation) override;

    std::uint8_t GetOccupiedSeat() const noexcept { return m_ucOccupiedSeat; }
    void SetOccupiedSeat(std::uint8_t seat) noexcept { m_ucOccupiedSeat = seat; }

    void RemoveBodyPart(std::uint8_t boneID, std::uint8_t direction);

    void         SetFootBlood(std::uint32_t footBlood);
    std::uint32_t GetFootBlood() const;

    bool IsBleeding() const { return GetPedInterface()->pedFlags.bPedIsBleeding; }
    void SetBleeding(bool bleeding) { GetPedInterface()->pedFlags.bPedIsBleeding = bleeding; }

    bool IsOnFire() const { return GetPedInterface()->pFireOnPed != nullptr; }
    void SetOnFire(bool onFire);

    bool GetStayInSamePlace() const { return GetPedInterface()->pedFlags.bStayInSamePlace; }
    void SetStayInSamePlace(bool stay) { GetPedInterface()->pedFlags.bStayInSamePlace = stay; }

    void GetVoice(std::int16_t* voiceType, std::int16_t* voiceID) const;
    void GetVoice(const char** voiceType, const char** voice) const;
    void SetVoice(std::int16_t voiceType, std::int16_t voiceID);
    void SetVoice(const char* voiceType, const char* voice);
    void ResetVoice() { SetVoice(m_sDefaultVoiceType, m_sDefaultVoiceID); }
    void SetLanding(bool isLanding) { GetPedInterface()->pedFlags.bIsLanding = isLanding; }
    void SetUpdateMetricsRequired(bool required) { GetPedInterface()->pedFlags.bUpdateMatricesRequired = required; }

    CWeaponStat* GetCurrentWeaponStat() const noexcept;
    float        GetCurrentWeaponRange() const noexcept;
    void         AddWeaponAudioEvent(EPedWeaponAudioEventType audioEventType);

    virtual int GetCustomMoveAnim() const noexcept { return m_iCustomMoveAnim; }
    bool        IsDoingGangDriveby() const noexcept;

    CPedIKSAInterface*      GetPedIKInterface() { return &GetPedInterface()->pedIK; }
    void*                   GetPedNodeInterface(std::int32_t nodeId) { return GetPedInterface()->pedNodes[nodeId]; }
    std::unique_ptr<CPedIK> GetPedIK() { return std::make_unique<CPedIKSA>(GetPedIKInterface()); }
    static void             StaticSetHooks();

    void GetAttachedSatchels(std::vector<SSatchelsData> &satchelsList) const override;
};
