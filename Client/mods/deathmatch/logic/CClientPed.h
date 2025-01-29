/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPed.h
 *  PURPOSE:     Ped entity class
 *
 *****************************************************************************/

class CClientPed;

#pragma once

#include "CAntiCheatModule.h"
#include "CClientCommon.h"
#include "CClientStreamElement.h"

#include <multiplayer/CMultiplayer.h>
#include "CClientPad.h"
#include "CClientModel.h"
#include <memory>

#include <game/CPad.h>
#include <game/TaskTypes.h>

class CAnimBlock;
class CClientCamera;
class CClientManager;
class CClientModelRequestManager;
class CClientPed;
class CClientPedManager;
class CClientPlayer;
class CClientPlayerClothes;
class CClientProjectile;
class CClientVehicle;
class CTask;

enum eDelayedSyncData
{
    DELAYEDSYNC_KEYSYNC,
    DELAYEDSYNC_CHANGEWEAPON,
    DELAYEDSYNC_MOVESPEED,
};

enum eVehicleInOutState
{
    VEHICLE_INOUT_NONE = 0,
    VEHICLE_INOUT_GETTING_IN,
    VEHICLE_INOUT_GETTING_OUT,
    VEHICLE_INOUT_JACKING,
    VEHICLE_INOUT_GETTING_JACKED,
};

enum eBodyPart
{
    BODYPART_TORSO = 3,
    BODYPART_ASS = 4,
    BODYPART_LEFT_ARM = 5,
    BODYPART_RIGHT_ARM = 6,
    BODYPART_LEFT_LEG = 7,
    BODYPART_RIGHT_LEG = 8,
    BODYPART_HEAD = 9,
};

enum eMovementState
{
    MOVEMENTSTATE_UNKNOWN,
    MOVEMENTSTATE_STAND,                // Standing still
    MOVEMENTSTATE_WALK,                 // Walking
    MOVEMENTSTATE_POWERWALK,            // Walking quickly
    MOVEMENTSTATE_JOG,                  // Jogging
    MOVEMENTSTATE_SPRINT,               // Sprinting
    MOVEMENTSTATE_CROUCH,               // Crouching still
    MOVEMENTSTATE_CRAWL,                // Crouch-moving
    MOVEMENTSTATE_ROLL,                 // Crouch-rolling (Needs adding)
    MOVEMENTSTATE_JUMP,                 // Jumping
    MOVEMENTSTATE_FALL,                 // Falling
    MOVEMENTSTATE_CLIMB                 // Climbing
};

enum eDeathAnims
{
    DEATH_ANIM_HEAD = 19,
    DEATH_ANIM_TORSO = 20,
};

struct SDelayedSyncData
{
    unsigned long    ulTime;
    unsigned char    ucType;
    CControllerState State;
    bool             bDucking;
    eWeaponSlot      slot;
    unsigned short   usWeaponAmmo;
    CVector          vecTarget;
    bool             bUseSource;
    CVector          vecSource;
};

struct SLastSyncedPedData
{
    float   fHealth;
    float   fArmour;
    CVector vPosition;
    CVector vVelocity;
    float   fRotation;
    float   cameraRotation{};
    bool    bOnFire;
    bool    bIsInWater;
    bool    isReloadingWeapon;
};

struct SRestoreWeaponItem
{
    DWORD       dwAmmo;
    DWORD       dwClipAmmo;
    bool        bCurrentWeapon;
    eWeaponType eWeaponID;
};

class CClientIFP;

struct SReplacedAnimation
{
    std::shared_ptr<CClientIFP>     pIFP;
    CAnimBlendHierarchySAInterface* pAnimationHierarchy;
};

struct SAnimationCache
{
    std::string strName;
    int         iTime{-1};
    bool        bLoop{false};
    bool        bUpdatePosition{false};
    bool        bInterruptable{false};
    bool        bFreezeLastFrame{true};
    int         iBlend{250};
    float       progress{0.0f};
    float       speed{1.0f};
    bool        progressWaitForStreamIn{false}; // for sync anim only
    float       elapsedTime{0.0f}; // for sync anim only
};

class CClientObject;

// To hide the ugly "pointer truncation from DWORD* to unsigned long warning
#pragma warning(disable:4311)

class CClientPed : public CClientStreamElement, public CAntiCheatModule
{
    DECLARE_CLASS(CClientPed, CClientStreamElement)
    typedef std::map<CAnimBlendHierarchySAInterface*, SReplacedAnimation> ReplacedAnim_type;
    friend class CClientCamera;
    friend class CClientPlayer;
    friend class CClientVehicle;
    friend class CClientPed;
    friend class CClientPedManager;

public:
    CClientPed(CClientManager* pManager, unsigned long ulModelID, ElementID ID);
    ~CClientPed();

    void Unlink() {};

    virtual eClientEntityType GetType() const { return CCLIENTPED; }

    CPlayerPed*    GetGamePlayer() noexcept { return m_pPlayerPed; }
    CEntity*       GetGameEntity() noexcept { return m_pPlayerPed; }
    const CEntity* GetGameEntity() const noexcept { return m_pPlayerPed; }

    bool IsLocalPlayer() const noexcept { return m_bIsLocalPlayer; }
    bool IsSyncing() const noexcept { return m_bIsSyncing; }
    void SetSyncing(bool bIsSyncing);

    bool            GetMatrix(CMatrix& Matrix) const;
    bool            SetMatrix(const CMatrix& Matrix);
    virtual CSphere GetWorldBoundingSphere();

    void GetPosition(CVector& vecPosition) const;
    void SetPosition(const CVector& vecPosition) noexcept { SetPosition(vecPosition, true, true); }
    void SetPosition(const CVector& vecPosition, bool bResetInterpolation, bool bAllowGroundLoadFreeze = true);

    void SetInterior(unsigned char ucInterior);

    void GetRotationDegrees(CVector& vecRotation) const;
    void GetRotationRadians(CVector& vecRotation) const;
    void SetRotationDegrees(const CVector& vecRotation);
    void SetRotationRadians(const CVector& vecRotation);

    void GetRotationDegreesNew(CVector& vecRotation) const;
    void GetRotationRadiansNew(CVector& vecRotation) const;
    void SetRotationDegreesNew(const CVector& vecRotation);
    void SetRotationRadiansNew(const CVector& vecRotation);
    void SetCurrentRotationNew(float fRotation);

    void Teleport(const CVector& vecPosition);

    // This function spawns/respawns this ped in any location. This will force a recreation
    // and restoration of initial state. This will also remove all weapons, unfreeze,
    // remove jetpack, etc...
    void Spawn(const CVector& vecPosition, float fRotation, unsigned short usModel, unsigned char ucInterior);

    void ResetInterpolation();

    float GetCurrentRotation();
    void  SetCurrentRotation(float fRotation, bool bIncludeTarget = true);
    void  SetTargetRotation(float fRotation);
    void  SetTargetRotation(unsigned long ulDelay, std::optional<float> rotation, std::optional<float> cameraRotation);

    float GetCameraRotation();
    void  SetCameraRotation(float fRotation);

    void GetMoveSpeed(CVector& vecMoveSpeed) const;
    void SetMoveSpeed(const CVector& vecMoveSpeed);

    void GetTurnSpeed(CVector& vecTurnSpeed) const;
    void SetTurnSpeed(const CVector& vecTurnSpeed);

    void GetControllerState(CControllerState& ControllerState);
    void GetLastControllerState(CControllerState& ControllerState);
    void SetControllerState(const CControllerState& ControllerState);

    void AddKeysync(unsigned long ulDelay, const CControllerState& ControllerState, bool bDucking);
    void AddChangeWeapon(unsigned long ulDelay, eWeaponSlot slot, unsigned short usWeaponAmmo);
    void AddMoveSpeed(unsigned long ulDelay, const CVector& vecMoveSpeed);

    void SetTargetTarget(unsigned long ulDelay, const CVector& vecSource, const CVector& vecTarget);

    int  GetVehicleInOutState() const noexcept { return m_iVehicleInOutState; };
    void SetVehicleInOutState(int iState) noexcept { m_iVehicleInOutState = iState; };

    unsigned long GetModel() const noexcept { return m_ulModel; };
    bool          SetModel(unsigned long ulModel, bool bTemp = false);

    bool GetCanBeKnockedOffBike();
    void SetCanBeKnockedOffBike(bool bCanBeKnockedOffBike);

    bool            IsInVehicle() const noexcept { return GetOccupiedVehicle() != NULL; };

    CClientVehicle*       GetOccupiedVehicle() noexcept { return m_pOccupiedVehicle; };
    const CClientVehicle* GetOccupiedVehicle() const noexcept { return m_pOccupiedVehicle; };

    unsigned int    GetOccupiedVehicleSeat() const noexcept { return m_uiOccupiedVehicleSeat; };

    CClientVehicle*       GetOccupyingVehicle() noexcept { return m_pOccupyingVehicle; };
    const CClientVehicle* GetOccupyingVehicle() const noexcept { return m_pOccupyingVehicle; };

    CClientVehicle* GetRealOccupiedVehicle();
    CClientVehicle* GetClosestEnterableVehicle(bool bGetPositionFromClosestDoor, bool bCheckDriverDoor, bool bCheckPassengerDoors,
                                               bool bCheckStreamedOutVehicles, unsigned int* uiClosestDoor = NULL, CVector* pClosestDoorPosition = NULL,
                                               float fWithinRange = 6000.0f, bool localVehicles = false);
    bool            GetClosestDoor(CClientVehicle* pVehicle, bool bCheckDriverDoor, bool bCheckPassengerDoors, unsigned int& uiClosestDoor,
                                   CVector* pClosestDoorPosition = NULL);

    void GetIntoVehicle(CClientVehicle* pVehicle, unsigned int uiSeat = 0, unsigned char ucDoor = 0);
    void GetOutOfVehicle(unsigned char ucDoor);

    void            WarpIntoVehicle(CClientVehicle* pVehicle, unsigned int uiSeat = 0);
    CClientVehicle* RemoveFromVehicle(bool bIgnoreIfGettingOut = false);

    bool IsVisible();
    void SetVisible(bool bVisible);
    bool GetUsesCollision();
    void SetUsesCollision(bool bUsesCollision);

    float GetMaxHealth();
    float GetHealth();
    void  SetHealth(float fHealth);
    void  InternalSetHealth(float fHealth);
    float GetArmor() const noexcept;
    void  SetArmor(float armor) noexcept;
    float GetOxygenLevel();
    void  SetOxygenLevel(float fOxygen);

    void LockHealth(float fHealth);
    void LockArmor(float armor) noexcept;
    void UnlockHealth() noexcept { m_bHealthLocked = false; };
    void UnlockArmor() noexcept { m_armorLocked = false; };
    bool IsHealthLocked() const noexcept { return m_bHealthLocked; };
    bool IsArmorLocked() const noexcept { return m_armorLocked; };

    bool IsDying();
    bool IsDead();
    void SetIsDead(bool bDead) noexcept { m_bDead = bDead; };
    void Kill(eWeaponType weaponType, unsigned char ucBodypart, bool bStealth = false, bool bSetDirectlyDead = false, AssocGroupId animGroup = 0,
              AnimationId animID = 15);
    void StealthKill(CClientPed* pPed);
    void BeHit(CClientPed* pClientPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId);

    int  GetRespawnState() const noexcept { return m_pRespawnState; };
    void SetRespawnState(int iRespawnState) noexcept { m_pRespawnState = iRespawnState; };

    CWeapon*    GiveWeapon(eWeaponType weaponType, unsigned int uiAmmo, bool bSetAsCurrent = false);
    bool        SetCurrentWeaponSlot(eWeaponSlot weaponSlot);
    eWeaponSlot GetCurrentWeaponSlot();
    eWeaponType GetCurrentWeaponType();
    eWeaponType GetWeaponType(eWeaponSlot slot);
    CWeapon*    GetWeapon();
    CWeapon*    GetWeapon(eWeaponSlot weaponSlot);
    CWeapon*    GetWeapon(eWeaponType weaponType);
    bool        HasWeapon(eWeaponType weaponType);
    void        RemoveWeapon(eWeaponType weaponType);
    void        RemoveAllWeapons();
    bool        IsCurrentWeaponUsingBulletSync();
    void        ValidateRemoteWeapons();

    std::map<eMovementState, std::string> m_MovementStateNames;
    eMovementState                        GetMovementState();
    bool                                  GetMovementState(std::string& strStateName);

    CTask* GetCurrentPrimaryTask();
    bool   IsSimplestTask(int iTaskType);
    bool   HasTask(int iTaskType, int iTaskPriority = -1, bool bPrimary = true);
    bool   SetTask(CTask* pTask, int iTaskPriority);
    bool   SetTaskSecondary(CTask* pTask, int iTaskPriority);
    bool   KillTask(int iTaskPriority, bool bGracefully = true);
    bool   KillTaskSecondary(int iTaskPriority, bool bGracefully = true);

    CVector* GetBonePosition(eBone bone, CVector& vecPosition) const;
    CVector* GetTransformedBonePosition(eBone bone, CVector& vecPosition) const;

    void GetAim(float& fDirectionX, float& fDirectionY)
    {
        if (m_shotSyncData)
        {
            fDirectionX = m_shotSyncData->m_fArmDirectionX;
            fDirectionY = m_shotSyncData->m_fArmDirectionY;
        }
    };
    CVector              GetAim() const;
    const CVector&       GetAimSource() const noexcept { return m_shotSyncData->m_vecShotOrigin; };
    const CVector&       GetAimTarget() const noexcept { return m_shotSyncData->m_vecShotTarget; };
    eVehicleAimDirection GetVehicleAimAnim() const noexcept { return m_shotSyncData->m_cInVehicleAimDirection; };
    void                 SetAim(float fArmDirectionX, float fArmDirectionY, eVehicleAimDirection cInVehicleAimAnim);
    void SetAimInterpolated(unsigned long ulDelay, float fArmDirectionX, float fArmDirectionY, bool bAkimboAimUp, eVehicleAimDirection cInVehicleAimAnim);
    void SetAimingData(unsigned long ulDelay, const CVector& vecTargetPosition, float fArmDirectionX, float fArmDirectionY,
                       eVehicleAimDirection cInVehicleAimAnim, CVector* pSource, bool bInterpolateAim);

    void Duck(bool bDuck);
    bool IsDucked();

    void SetChoking(bool bChoking);
    bool IsChoking();

    void SetWearingGoggles(bool bWearing, bool animationEnabled = true);
    bool IsWearingGoggles(bool bCheckMoving = false);
    bool IsMovingGoggles(bool& bPuttingOn);

    void WorldIgnore(bool bIgnore);

    void ResetToOutOfVehicleWeapon();

    void  RebuildModel(bool bDelayChange = false);
    void  ProcessRebuildPlayer(bool bNeedsClothesUpdate);
    void  SetStat(unsigned short usStat, float fValue);
    float GetStat(unsigned short usStat);
    void  ResetStats();

    CClientPlayerClothes*       GetClothes() noexcept { return m_pClothes; }
    const CClientPlayerClothes* GetClothes() const noexcept { return m_pClothes; }

    // This is kinda hacky, should be private but something depends on this. Should depend on some
    // streamer func. Perhaps use SetNeverStreamOut, but need something to reset that.
    void StreamIn(bool bInstantly);
    void StreamOut();

    void StreamOutWeaponForABit(eWeaponSlot eSlot);

    bool SetHasJetPack(bool bHasJetPack);
    bool HasJetPack();

    float GetDistanceFromGround();

    void SetInWater(bool bIsInWater) { m_bIsInWater = bIsInWater; };
    bool IsInWater();
    bool IsOnGround();

    bool          IsClimbing();
    bool          IsRadioOn() const noexcept { return m_bRadioOn; };
    void          NextRadioChannel();
    void          PreviousRadioChannel();
    bool          SetCurrentRadioChannel(unsigned char ucChannel);
    unsigned char GetCurrentRadioChannel() const noexcept { return m_ucRadioChannel; };

    CTaskManager*       GetTaskManager() noexcept { return m_pTaskManager; }
    const CTaskManager* GetTaskManager() const noexcept { return m_pTaskManager; }

    bool    GetShotData(CVector* pvecOrigin, CVector* pvecTarget = NULL, CVector* pvecGunMuzzle = NULL, CVector* pvecFireOffset = NULL, float* fAimX = NULL,
                        float* fAimY = NULL);
    CVector AdjustShotOriginForWalls(const CVector& vecOrigin, const CVector& vecTarget, float fMaxPullBack);

    eFightingStyle GetFightingStyle();
    void           SetFightingStyle(eFightingStyle style);

    eMoveAnim GetMoveAnim();
    void      SetMoveAnim(eMoveAnim iAnim);

    void                                    AddProjectile(CClientProjectile* pProjectile) { m_Projectiles.push_back(pProjectile); }
    void                                    RemoveProjectile(CClientProjectile* pProjectile) { m_Projectiles.remove(pProjectile); }
    std::list<CClientProjectile*>::iterator ProjectilesBegin() { return m_Projectiles.begin(); }
    std::list<CClientProjectile*>::iterator ProjectilesEnd() { return m_Projectiles.end(); }
    unsigned int                            CountProjectiles(eWeaponType weaponType = WEAPONTYPE_UNARMED);

    std::list<CClientProjectile*>& GetProjectiles() noexcept { return m_Projectiles; }
    const std::list<CClientProjectile*>& GetProjectiles() const noexcept { return m_Projectiles; }

    void RemoveAllProjectiles();
    void DestroySatchelCharges(bool bBlow = true, bool bDestroy = true);

    CRemoteDataStorage*       GetRemoteData() noexcept { return m_remoteDataStorage; }
    const CRemoteDataStorage* GetRemoteData() const noexcept { return m_remoteDataStorage; }

    bool IsEnteringVehicle();
    bool IsLeavingVehicle();
    bool IsGettingIntoVehicle();
    bool IsGettingOutOfVehicle();
    bool IsGettingJacked();

    CClientEntity* GetContactEntity();

    bool HasAkimboPointingUpwards();

    float GetDistanceFromCentreOfMassToBaseOfModel();

    unsigned char GetAlpha() const noexcept { return m_ucAlpha; }
    void          SetAlpha(unsigned char ucAlpha);

    bool           HasTargetPosition() const noexcept { return m_interp.pos.ulFinishTime != 0; }
    CClientEntity* GetTargetOriginSource() noexcept { return m_interp.pTargetOriginSource; }
    const CClientEntity* GetTargetOriginSource() const noexcept { return m_interp.pTargetOriginSource; }
    void           GetTargetPosition(CVector& vecPosition);
    void           SetTargetPosition(const CVector& vecPosition, unsigned long ulDelay, CClientEntity* pTargetOriginSource = NULL);
    void           RemoveTargetPosition();
    void           UpdateTargetPosition();
    void           UpdateUnderFloorFix(const CVector& vecTargetPosition, const CVector& vecOrigin);

    CClientEntity* GetTargetedEntity();
    CClientPed*    GetTargetedPed();

    CClientEntity*       GetCurrentContactEntity() noexcept { return m_pCurrentContactEntity; }
    const CClientEntity* GetCurrentContactEntity() const noexcept { return m_pCurrentContactEntity; }
    void           SetCurrentContactEntity(CClientEntity* pEntity) noexcept { m_pCurrentContactEntity = pEntity; }

    bool IsSunbathing();
    void SetSunbathing(bool bSunbathing, bool bStartStanding = true);

    bool LookAt(CVector vecOffset, int iTime = 1000, int iBlend = 1000, CClientEntity* pEntity = NULL);
    bool UseGun(CVector vecTarget, CClientEntity* pEntity);

    bool               IsAttachToable();
    static const char* GetBodyPartName(unsigned char ucID);

    bool IsDoingGangDriveby();
    void SetDoingGangDriveby(bool bDriveby);

    bool GetRunningAnimationName(SString& strBlockName, SString& strAnimName);
    bool IsRunningAnimation();
    void RunNamedAnimation(std::unique_ptr<CAnimBlock>& pBlock, const char* szAnimName, int iTime = -1, int iBlend = 250, bool bLoop = true,
                           bool bUpdatePosition = true, bool bInterruptable = false, bool bFreezeLastFrame = true, bool bRunInSequence = false,
                           bool bOffsetPed = false, bool bHoldLastFrame = false);
    void KillAnimation();
    std::unique_ptr<CAnimBlock> GetAnimationBlock();
    const SAnimationCache&      GetAnimationCache() const noexcept { return m_AnimationCache; }
    void                        RunAnimationFromCache();

    bool IsUsingGun();

    bool IsHeadless() const noexcept { return m_bHeadless; }
    void SetHeadless(bool bHeadless);

    bool IsFrozen() const noexcept { return m_bFrozen; }
    void SetFrozen(bool bFrozen);
    bool IsFrozenWaitingForGroundToLoad() const;
    void SetFrozenWaitingForGroundToLoad(bool bFrozen);

    bool IsFootBloodEnabled();
    void SetFootBloodEnabled(bool bHasFootBlood);

    bool IsBleeding() const noexcept { return m_bBleeding; };
    void SetBleeding(bool bBleeding);

    bool IsOnFire() override { return m_pPlayerPed ? m_pPlayerPed->IsOnFire() : m_bIsOnFire; }
    bool SetOnFire(bool bOnFire) override;

    void GetVoice(short* psVoiceType, short* psVoiceID);
    void GetVoice(const char** pszVoiceType, const char** pszVoice);
    void SetVoice(short sVoiceType, short sVoiceID);
    void SetVoice(const char* szVoiceType, const char* szVoice);
    void ResetVoice();

    bool IsSpeechEnabled();
    void SetSpeechEnabled(bool bEnabled);

    void PostWeaponFire();
    void SetBulletImpactData(CClientEntity* pVictim, const CVector& vecHitPosition);
    bool GetBulletImpactData(CClientEntity** ppVictim = 0, CVector* pvecHitPosition = 0);
    void ClearBulletImpactData() { m_bBulletImpactData = false; }

    bool CanReloadWeapon() noexcept;
    bool ReloadWeapon() noexcept;
    bool IsReloadingWeapon() noexcept;

    bool ShouldBeStealthAiming();
    bool IsStealthAiming() { return m_bStealthAiming; }
    void SetStealthAiming(bool bAiming);

    std::unique_ptr<CAnimBlendAssociation> AddAnimation(AssocGroupId group, AnimationId id);
    std::unique_ptr<CAnimBlendAssociation> BlendAnimation(AssocGroupId group, AnimationId id, float fBlendDelta);
    std::unique_ptr<CAnimBlendAssociation> GetAnimation(AnimationId id);
    std::unique_ptr<CAnimBlendAssociation> GetFirstAnimation();

    void                        DereferenceCustomAnimationBlock() { m_pCustomAnimationIFP = nullptr; }
    std::shared_ptr<CClientIFP> GetCustomAnimationIFP() const noexcept { return m_pCustomAnimationIFP; }
    bool IsCustomAnimationPlaying() noexcept
    {
        return (m_bRequestedAnimation || m_AnimationCache.bLoop)
            && m_pAnimationBlock && m_bisCurrentAnimationCustom;
    }
    void SetCustomAnimationUntriggerable() noexcept
    {
        m_bRequestedAnimation = false;
        m_AnimationCache.bLoop = false;
    }
    bool            IsNextAnimationCustom() const noexcept { return m_bisNextAnimationCustom; }
    void            SetNextAnimationCustom(const std::shared_ptr<CClientIFP>& pIFP, const SString& strAnimationName);
    void            SetCurrentAnimationCustom(bool bCustom) noexcept { m_bisCurrentAnimationCustom = bCustom; }
    bool            IsCurrentAnimationCustom() const noexcept { return m_bisCurrentAnimationCustom; }
    CIFPAnimations*       GetIFPAnimationsPointer() noexcept { return m_pIFPAnimations; }
    const CIFPAnimations* GetIFPAnimationsPointer() const noexcept { return m_pIFPAnimations; }

    void SetIFPAnimationsPointer(CIFPAnimations* pIFPAnimations) noexcept { m_pIFPAnimations = pIFPAnimations; }

    // This will indicate that we have played custom animation, so next animation can be internal GTA animation
    // You must call this function after playing a custom animation
    void                SetNextAnimationNormal() noexcept { m_bisNextAnimationCustom = false; }
    const SString&      GetNextAnimationCustomBlockName() const noexcept { return m_strCustomIFPBlockName; }
    const SString&      GetNextAnimationCustomName() const noexcept { return m_strCustomIFPAnimationName; }
    const unsigned int& GetCustomAnimationBlockNameHash() const noexcept { return m_u32CustomBlockNameHash; }
    const unsigned int& GetCustomAnimationNameHash() const noexcept { return m_u32CustomAnimationNameHash; }

    void                ReplaceAnimation(std::unique_ptr<CAnimBlendHierarchy>& pInternalAnimHierarchy, const std::shared_ptr<CClientIFP>& pIFP,
                                         CAnimBlendHierarchySAInterface* pCustomAnimHierarchy);
    void                RestoreAnimation(std::unique_ptr<CAnimBlendHierarchy>& pInternalAnimHierarchy);
    void                RestoreAnimations(const std::shared_ptr<CClientIFP>& IFP);
    void                RestoreAnimations(CAnimBlock& animationBlock);
    void                RestoreAllAnimations();
    SReplacedAnimation* GetReplacedAnimation(CAnimBlendHierarchySAInterface* pInternalHierarchyInterface);

    std::unique_ptr<CAnimBlendAssociation> GetAnimAssociation(CAnimBlendHierarchySAInterface* pHierarchyInterface);

    void SetHasSyncedAnim(bool synced) noexcept { m_hasSyncedAnim = synced; }
    bool HasSyncedAnim() const noexcept { return m_hasSyncedAnim; }

protected:
    // This constructor is for peds managed by a player. These are unknown to the ped manager.
    CClientPed(CClientManager* pManager, unsigned long ulModelID, ElementID ID, bool bIsLocalPlayer);

    void Init(CClientManager* pManager, unsigned long ulModelID, bool bIsLocalPlayer);

    void StreamedInPulse(bool bDoStandardPulses);
    void ApplyControllerStateFixes(CControllerState& Current);

    void Interpolate();
    void UpdateKeysync(bool bCleanup = false);

    // Used to destroy the current game ped and create a new one in the same state.
    void ReCreateModel();
    void ReCreateGameEntity();

    void _CreateModel();
    void _CreateLocalModel();
    void _DestroyModel();
    void _DestroyLocalModel();
    void _ChangeModel();

    void ModelRequestCallback(CModelInfo* pModelInfo);

    void InternalWarpIntoVehicle(CVehicle* pGameVehicle);
    void InternalRemoveFromVehicle(CVehicle* pGameVehicle);

    bool PerformChecks();
    void HandleWaitingForGroundToLoad();
    void UpdateStreamPosition(const CVector& vecPosition);

    // Used to start and stop radio for local player
    void StartRadio();
    void StopRadio();
    bool m_bDontChangeRadio;

public:
    void _GetIntoVehicle(CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor);
    // Used to control and sync entering/exiting
    bool EnterVehicle(CClientVehicle* pVehicle, bool bPassenger);
    bool ExitVehicle();
    void ResetVehicleInOut();
    void UpdateVehicleInOut();

    void Respawn(CVector* pvecPosition = NULL, bool bRestoreState = false, bool bCameraCut = false);

    void      SetTaskToBeRestoredOnAnimEnd(bool bSetOnEnd) noexcept { m_bTaskToBeRestoredOnAnimEnd = bSetOnEnd; }
    bool      IsTaskToBeRestoredOnAnimEnd() const noexcept { return m_bTaskToBeRestoredOnAnimEnd; }
    void      SetTaskTypeToBeRestoredOnAnimEnd(eTaskType taskType) noexcept { m_eTaskTypeToBeRestoredOnAnimEnd = taskType; }
    eTaskType GetTaskTypeToBeRestoredOnAnimEnd() const noexcept { return m_eTaskTypeToBeRestoredOnAnimEnd; }

    bool IsWarpInToVehicleRequired() const noexcept { return m_bWarpInToVehicleRequired; }
    void SetWarpInToVehicleRequired(bool warp) noexcept { m_bWarpInToVehicleRequired = warp; }

    void NotifyCreate();
    void NotifyDestroy();

    CClientModelRequestManager* m_pRequester;
    CPlayerPed*                 m_pPlayerPed;
    CTaskManager*               m_pTaskManager;
    CPad*                       m_pPad;
    bool                        m_bIsLocalPlayer;
    int                         m_pRespawnState;
    unsigned long               m_ulModel;
    CMatrix                     m_matFrozen;
    bool                        m_bRadioOn;
    unsigned char               m_ucRadioChannel;
    bool                        m_bHealthLocked;
    bool                        m_armorLocked;
    unsigned long               m_ulLastOnScreenTime;
    CClientVehiclePtr           m_pOccupiedVehicle;
    CClientVehiclePtr           m_pOccupyingVehicle;
    // unsigned int                m_uiOccupyingSeat;
    unsigned int      m_uiOccupiedVehicleSeat;
    bool              m_bForceGettingIn;
    bool              m_bForceGettingOut;
    CShotSyncData*    m_shotSyncData;
    CStatsData*       m_stats;
    CControllerState* m_currentControllerState;
    CControllerState* m_lastControllerState;
    CControllerState m_rawControllerState;            // copy of lastControllerState before CClientPed::ApplyControllerStateFixes is applied (modifies states to
                                                      // prevent stuff like rapid input glitch)
    CRemoteDataStorage*                      m_remoteDataStorage;
    unsigned long                            m_ulLastTimeFired;
    unsigned long                            m_ulLastTimeBeganAiming;
    unsigned long                            m_ulLastTimeEndedAiming;
    unsigned long                            m_ulLastTimeBeganCrouch;
    unsigned long                            m_ulLastTimeBeganStand;
    unsigned long                            m_ulLastTimeMovedWhileCrouched;
    unsigned long                            m_ulLastTimePressedLeftOrRight;
    unsigned long                            m_ulLastTimeUseGunCrouched;
    unsigned long                            m_ulLastTimeSprintPressed;
    unsigned long                            m_ulBlockSprintReleaseTime;
    bool                                     m_bWasSprintButtonDown;
    CModelInfo*                              m_pLoadedModelInfo;
    eWeaponSlot                              m_pOutOfVehicleWeaponSlot;
    float                                    m_fBeginAimX;
    float                                    m_fBeginAimY;
    float                                    m_fTargetAimX;
    float                                    m_fTargetAimY;
    unsigned long                            m_ulBeginAimTime;
    unsigned long                            m_ulTargetAimTime;
    bool                                     m_bTargetAkimboUp;
    unsigned long                            m_ulBeginRotationTime;
    unsigned long                            m_ulEndRotationTime;
    float                                    m_fBeginRotation;
    float                                    m_fTargetRotationA;
    float                                    m_fBeginCameraRotation;
    float                                    m_fTargetCameraRotation;
    unsigned long                            m_ulBeginTarget;
    unsigned long                            m_ulEndTarget;
    CVector                                  m_vecBeginSource;
    CVector                                  m_vecBeginTarget;
    CVector                                  m_vecBeginTargetAngle;
    CVector                                  m_vecTargetSource;
    CVector                                  m_vecTargetTarget;
    CVector                                  m_vecTargetTargetAngle;
    CVector                                  m_vecTargetInterpolateAngle;
    CClientEntityPtr                         m_pTargetedEntity;
    std::list<SDelayedSyncData*>             m_SyncBuffer;
    bool                                     m_bDucked;
    bool                                     m_bWasDucked;            // For knowing when to register standing up
    bool                                     m_bIsChoking;
    bool                                     m_bWearingGoggles;
    bool                                     m_bVisible;
    bool                                     m_bUsesCollision;
    float                                    m_fHealth;
    float                                    m_armor;
    bool                                     m_bDead;
    bool                                     m_bWorldIgnored;
    float                                    m_fCurrentRotation;
    float                                    m_fMoveSpeed;
    bool                                     m_bCanBeKnockedOffBike;
    bool                                     m_bBleeding;
    CMatrix                                  m_Matrix;
    CVector                                  m_vecMoveSpeed;
    CVector                                  m_vecTurnSpeed;
    eWeaponSlot                              m_CurrentWeaponSlot;
    SFixedArray<eWeaponType, WEAPONSLOT_MAX> m_WeaponTypes;
    SFixedArray<ushort, WEAPONSLOT_MAX>      m_usWeaponAmmo;
    bool                                     m_bHasJetPack;
    CClientPlayerClothes*                    m_pClothes;
    eFightingStyle                           m_FightingStyle;
    eMoveAnim                                m_MoveAnim;
    std::list<CClientProjectile*>            m_Projectiles;
    unsigned char                            m_ucAlpha;
    float                                    m_fTargetRotation;
    int                                      m_iVehicleInOutState;
    bool                                     m_bRecreatingModel;
    CClientEntityPtr                         m_pCurrentContactEntity;
    bool                                     m_bSunbathing;
    CClientPad                               m_Pad;
    bool                                     m_bDestroyingSatchels;
    bool                                     m_bDoingGangDriveby;
    std::unique_ptr<CAnimBlock>              m_pAnimationBlock;
    bool                                     m_bRequestedAnimation;
    SAnimationCache                          m_AnimationCache;
    bool                                     m_bHeadless;
    bool                                     m_bFrozen;
    bool                                     m_bFrozenWaitingForGroundToLoad;
    float                                    m_fGroundCheckTolerance;
    float                                    m_fObjectsAroundTolerance;
    int                                      m_iLoadAllModelsCounter;
    bool                                     m_bIsOnFire;
    bool                                     m_bIsInWater;
    SLastSyncedPedData*                      m_LastSyncedData;
    bool                                     m_bSpeechEnabled;
    bool                                     m_bStealthAiming;
    float                                    m_fLighting;
    unsigned char                            m_ucEnteringDoor;
    unsigned char                            m_ucLeavingDoor;
    bool                                     m_bPendingRebuildPlayer;
    uint                                     m_uiFrameLastRebuildPlayer;
    bool                                     m_bIsSyncing;
    bool                                     m_shouldRecreate{false};

    bool             m_bBulletImpactData;
    CClientEntityPtr m_pBulletImpactEntity;
    CVector          m_vecBulletImpactHit;

    // Time dependent interpolation
    struct
    {
        struct
        {
            CVector       vecTarget;
            CVector       vecError;
            unsigned long ulStartTime;
            unsigned long ulFinishTime;
            float         fLastAlpha;
        } pos;

        CClientEntityPtr pTargetOriginSource;
        // These variables are used to track the last known position
        // of the contact entity for if it's removed during the
        // interpolation.
        bool    bHadOriginSource;
        CVector vecOriginSourceLastPosition;
    } m_interp;

    // Hacks for player model replacement and weapon model replacement respectively
    unsigned long                 m_ulStoredModel;
    std::list<SRestoreWeaponItem> m_RestoreWeaponList;

    CVector m_vecPrevTargetPosition;
    uint    m_uiForceLocalCounter;

    // This is checked within AddAnimation and AddAnimationAndSync
    // It is set to false when custom animation is played.
    bool                        m_bisNextAnimationCustom;
    bool                        m_bisCurrentAnimationCustom;
    SString                     m_strCustomIFPBlockName;
    SString                     m_strCustomIFPAnimationName;
    unsigned int                m_u32CustomBlockNameHash;
    unsigned int                m_u32CustomAnimationNameHash;
    CIFPAnimations*             m_pIFPAnimations;
    std::shared_ptr<CClientIFP> m_pCustomAnimationIFP;

    // Key: Internal GTA animation, Value: Custom Animation
    ReplacedAnim_type m_mapOfReplacedAnimations;
    bool              m_bTaskToBeRestoredOnAnimEnd;
    eTaskType         m_eTaskTypeToBeRestoredOnAnimEnd;
    bool              m_bWarpInToVehicleRequired = false;

    // Enter/exit variables
    unsigned long m_ulLastVehicleInOutTime;            // Last tick where we sent an enter/exit request
    bool          m_bIsGettingOutOfVehicle;            // Indicates we are exiting a vehicle
    bool          m_bIsGettingIntoVehicle;             // Indicates we are entering a vehicle
    bool          m_bIsJackingVehicle;                 // Indicates we are jacking a vehicle
    bool          m_bIsGettingJacked;                  // Indicates we are getting jacked
    ElementID     m_VehicleInOutID;                    // ElementID of vehicle received from server
    unsigned char m_ucVehicleInOutSeat;                // Seat ID we are entering/exiting received from server
    bool          m_bNoNewVehicleTask;                 // When set, we are not allowed to initiate a new enter/exit task because we are waiting for server reply
    ElementID     m_NoNewVehicleTaskReasonID;            // ElementID of the vehicle that we are waiting on
    CClientPed*   m_pGettingJackedBy;                    // The ped that is jacking us

    std::shared_ptr<CClientModel> m_clientModel;

    bool m_hasSyncedAnim{};
    bool m_animationOverridedByClient{};
};
