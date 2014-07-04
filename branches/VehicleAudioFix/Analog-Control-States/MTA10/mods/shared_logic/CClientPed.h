/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPed.h
*  PURPOSE:     Ped entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

class CClientPed;

#ifndef __CCLIENTPED_H
#define __CCLIENTPED_H

#include "CAntiCheatModule.h"
#include "CClientCommon.h"
#include "CClientStreamElement.h"

#include <multiplayer/CMultiplayer.h>
#include "net/Packets.h"
#include "CClientPad.h"

class CClientCamera;
class CClientManager;
class CClientModelRequestManager;
class CClientPed;
class CClientPedManager;
class CClientPlayer;
class CClientPlayerClothes;
class CClientVehicle;
class CClientProjectile;

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
    MOVEMENTSTATE_STAND, //Standing still
    MOVEMENTSTATE_WALK, //Walking
    MOVEMENTSTATE_POWERWALK, //Walking quickly
    MOVEMENTSTATE_JOG, //Jogging
    MOVEMENTSTATE_SPRINT, //Sprinting
    MOVEMENTSTATE_CROUCH, //Crouching still
    // Duds for now.  We should add methods to detect these
    MOVEMENTSTATE_CRAWL, //Crouch-moving
    MOVEMENTSTATE_ROLL, //Crouch-rolling
};

enum eDeathAnims
{
    DEATH_ANIM_HEAD = 19,
    DEATH_ANIM_TORSO = 20,
};

struct SDelayedSyncData
{
    unsigned long       ulTime;
    unsigned char       ucType;
    CControllerState    State;
    bool                bDucking;
    eWeaponSlot         slot;
    unsigned short      usWeaponAmmo;
    CVector             vecTarget;
    bool                bUseSource;
    CVector             vecSource;
};

struct SLastSyncedPedData
{
    float               fHealth;
    float               fArmour;
    CVector             vPosition;
    CVector             vVelocity;
    float               fRotation;
};

class CClientObject;

// To hide the ugly "pointer truncation from DWORD* to unsigned long warning
#pragma warning(disable:4311)

class CClientPed : public CClientStreamElement, public CAntiCheatModule
{
    DECLARE_CLASS( CClientPed, CClientStreamElement )
    friend class CClientCamera;
    friend class CClientPlayer;
    friend class CClientVehicle;
    friend class CClientPed;
    friend class CClientPedManager;

public:
                                CClientPed                  ( CClientManager* pManager, unsigned long ulModelID, ElementID ID );
                                ~CClientPed                 ( void );

    inline void                 Unlink                      ( void ) {};

    virtual eClientEntityType   GetType                     ( void ) const                              { return CCLIENTPED; }
    
    inline CPlayerPed*          GetGamePlayer               ( void )                                    { return m_pPlayerPed; }
    inline CEntity*             GetGameEntity               ( void )                                    { return m_pPlayerPed; }
    inline const CEntity*       GetGameEntity               ( void ) const                              { return m_pPlayerPed; }

    inline bool                 IsLocalPlayer               ( void )                                    { return m_bIsLocalPlayer; }

    bool                        GetMatrix                   ( CMatrix& Matrix ) const;
    bool                        SetMatrix                   ( const CMatrix& Matrix );
    virtual CSphere             GetWorldBoundingSphere      ( void );

    void                        GetPosition                 ( CVector& vecPosition ) const;
    void                        SetPosition                 ( const CVector& vecPosition )              { SetPosition ( vecPosition, true ); }
    void                        SetPosition                 ( const CVector& vecPosition, bool bResetInterpolation );

    void                        SetInterior                 ( unsigned char ucInterior );

    void                        GetRotationDegrees          ( CVector& vecRotation ) const;
    void                        GetRotationRadians          ( CVector& vecRotation ) const;
    void                        SetRotationDegrees          ( const CVector& vecRotation );
    void                        SetRotationRadians          ( const CVector& vecRotation );

    void                        Teleport                    ( const CVector& vecPosition );

    // This function spawns/respawns this ped in any location. This will force a recreation
    // and restoration of initial state. This will also remove all weapons, unfreeze,
    // remove jetpack, etc...
    void                        Spawn                       ( const CVector& vecPosition,
                                                              float fRotation,
                                                              unsigned short usModel,
                                                              unsigned char ucInterior );

    void                        SetTargetPosition           ( unsigned long ulDelay, const CVector& vecPosition );
    void                        ResetInterpolation          ( void );

    float                       GetCurrentRotation          ( void );
    void                        SetCurrentRotation          ( float fRotation, bool bIncludeTarget = true );
    void                        SetTargetRotation           ( float fRotation );
    void                        SetTargetRotation           ( unsigned long ulDelay, float fRotation, float fCameraRotation );

    float                       GetCameraRotation           ( void );
    void                        SetCameraRotation           ( float fRotation );

    void                        GetMoveSpeed                ( CVector& vecMoveSpeed ) const;
    void                        SetMoveSpeed                ( const CVector& vecMoveSpeed );

    void                        GetTurnSpeed                ( CVector& vecTurnSpeed ) const;
    void                        SetTurnSpeed                ( const CVector& vecTurnSpeed );

    void                        GetControllerState          ( CControllerState& ControllerState );
    void                        GetLastControllerState      ( CControllerState& ControllerState );
    void                        SetControllerState          ( const CControllerState& ControllerState );

    void                        AddKeysync                  ( unsigned long ulDelay, const CControllerState& ControllerState, bool bDucking );
    void                        AddChangeWeapon             ( unsigned long ulDelay, eWeaponSlot slot, unsigned short usWeaponAmmo );
    void                        AddMoveSpeed                ( unsigned long ulDelay, const CVector& vecMoveSpeed );

    void                        SetTargetTarget             ( unsigned long ulDelay, const CVector& vecSource, const CVector& vecTarget );

    inline int                  GetVehicleInOutState        ( void )                                    { return m_iVehicleInOutState; };
    inline void                 SetVehicleInOutState        ( int iState )                              { m_iVehicleInOutState = iState; };

    inline unsigned long        GetModel                    ( void )                                    { return m_ulModel; };
    bool                        SetModel                    ( unsigned long ulModel, bool bTemp = false );

    bool                        GetCanBeKnockedOffBike      ( void );
    void                        SetCanBeKnockedOffBike      ( bool bCanBeKnockedOffBike );

    inline bool                 IsInVehicle                 ( void )                                    { return GetOccupiedVehicle () != NULL; };
    inline CClientVehicle*      GetOccupiedVehicle          ( void )                                    { return m_pOccupiedVehicle; };
    inline unsigned int         GetOccupiedVehicleSeat      ( void )                                    { return m_uiOccupiedVehicleSeat; };
    inline CClientVehicle*      GetOccupyingVehicle         ( void )                                    { return m_pOccupyingVehicle; };

    CClientVehicle*             GetRealOccupiedVehicle      ( void );
    CClientVehicle*             GetClosestVehicleInRange    ( bool bGetPositionFromClosestDoor, bool bCheckDriverDoor, bool bCheckPassengerDoors, bool bCheckStreamedOutVehicles, unsigned int* uiClosestDoor = NULL, CVector* pClosestDoorPosition = NULL, float fWithinRange = 6000.0f );
    bool                        GetClosestDoor              ( CClientVehicle* pVehicle, bool bCheckDriverDoor, bool bCheckPassengerDoors, unsigned int& uiClosestDoor, CVector* pClosestDoorPosition = NULL );

    void                        GetIntoVehicle              ( CClientVehicle* pVehicle, unsigned int uiSeat = 0, unsigned char ucDoor = 0 );
    void                        GetOutOfVehicle             ( unsigned char ucDoor );

    void                        WarpIntoVehicle             ( CClientVehicle* pVehicle, unsigned int uiSeat = 0 );
    CClientVehicle*             RemoveFromVehicle           ( bool bIgnoreIfGettingOut = false );

    bool                        IsVisible                   ( void );
    void                        SetVisible                  ( bool bVisible );
    bool                        GetUsesCollision            ( void );
    void                        SetUsesCollision            ( bool bUsesCollision );

    float                       GetMaxHealth                ( void );
    float                       GetHealth                   ( void );
    void                        SetHealth                   ( float fHealth );
    void                        InternalSetHealth           ( float fHealth );
    float                       GetArmor                    ( void );
    void                        SetArmor                    ( float fArmor );

    void                        LockHealth                  ( float fHealth );
    void                        LockArmor                   ( float fArmor );
    inline void                 UnlockHealth                ( void )                                    { m_bHealthLocked = false; };
    inline void                 UnlockArmor                 ( void )                                    { m_bArmorLocked = false; };
    inline bool                 IsHealthLocked              ( void ) const                              { return m_bHealthLocked; };
    inline bool                 IsArmorLocked               ( void ) const                              { return m_bArmorLocked; };

    bool                        IsDying                     ( void );
    bool                        IsDead                      ( void );
    void                        Kill                        ( eWeaponType weaponType, unsigned char ucBodypart, bool bStealth = false, bool bSetDirectlyDead = false, AssocGroupId animGroup = 0, AnimationId animID = 15 );
    void                        StealthKill                 ( CClientPed * pPed );

    inline int                  GetRespawnState             ( void )                                    { return m_pRespawnState; };
    inline void                 SetRespawnState             ( int iRespawnState )                       { m_pRespawnState = iRespawnState; };

    CWeapon*                    GiveWeapon                  ( eWeaponType weaponType, unsigned int uiAmmo );
    bool                        SetCurrentWeaponSlot        ( eWeaponSlot weaponSlot );
    eWeaponSlot                 GetCurrentWeaponSlot        ( void );
    eWeaponType                 GetCurrentWeaponType        ( void );
    CWeapon*                    GetWeapon                   ( void );
    CWeapon*                    GetWeapon                   ( eWeaponSlot weaponSlot );
    CWeapon*                    GetWeapon                   ( eWeaponType weaponType );
    bool                        HasWeapon                   ( eWeaponType weaponType );
    void                        RemoveWeapon                ( eWeaponType weaponType );
    void                        RemoveAllWeapons            ( void );

    std::map<eMovementState,std::string> m_MovementStateNames;
    eMovementState              GetMovementState            ( void );
    bool                        GetMovementState            ( std::string& strStateName );

    CTask*                      GetCurrentPrimaryTask       ( void );
    bool                        IsSimplestTask              ( int iTaskType );
    bool                        HasTask                     ( int iTaskType, int iTaskPriority = -1, bool bPrimary = true );
    bool                        SetTask                     ( CTask* pTask, int iTaskPriority );
    bool                        SetTaskSecondary            ( CTask* pTask, int iTaskPriority );
    bool                        KillTask                    ( int iTaskPriority, bool bGracefully = true );
    bool                        KillTaskSecondary           ( int iTaskPriority, bool bGracefully = true );

    CVector*                    GetBonePosition             ( eBone bone, CVector& vecPosition ) const;
    CVector*                    GetTransformedBonePosition  ( eBone bone, CVector& vecPosition ) const;

    inline void                 GetAim                      ( float& fDirectionX, float& fDirectionY )  { if ( m_shotSyncData ) { fDirectionX = m_shotSyncData->m_fArmDirectionX; fDirectionY = m_shotSyncData->m_fArmDirectionY; } };
    CVector                     GetAim                      ( void ) const;
    inline const CVector&       GetAimSource                ( void )                                    { return m_shotSyncData->m_vecShotOrigin; };
    inline const CVector&       GetAimTarget                ( void )                                    { return m_shotSyncData->m_vecShotTarget; };
    inline unsigned char        GetVehicleAimAnim           ( void )                                    { return m_shotSyncData->m_cInVehicleAimDirection; };
    void                        SetAim                      ( float fArmDirectionX, float fArmDirectionY, unsigned char cInVehicleAimAnim );
    void                        SetAimInterpolated          ( unsigned long ulDelay, float fArmDirectionX, float fArmDirectionY, bool bAkimboAimUp, unsigned char cInVehicleAimAnim );
    void                        SetAimingData               ( unsigned long ulDelay, const CVector& vecTargetPosition, float fArmDirectionX, float fArmDirectionY, char cInVehicleAimAnim, CVector* pSource, bool bInterpolateAim );

    inline unsigned long        GetMemoryValue              ( unsigned long ulOffset )                  { return ( m_pPlayerPed ) ? *m_pPlayerPed->GetMemoryValue ( ulOffset ) : 0; };
    inline unsigned long        GetGameBaseAddress          ( void )                                    { return ( m_pPlayerPed ) ? (unsigned long)m_pPlayerPed->GetMemoryValue ( 0 ) : 0; };

    void                        Duck                        ( bool bDuck );
    bool                        IsDucked                    ( void );

    void                        SetChoking                  ( bool bChoking );
    bool                        IsChoking                   ( void );
  
    void                        SetWearingGoggles           ( bool bWearing );
    inline bool                 IsWearingGoggles            ( bool bCheckMoving = false );
    bool                        IsMovingGoggles             ( bool & bPuttingOn );

    void                        WorldIgnore                 ( bool bIgnore );

    void                        ResetToOutOfVehicleWeapon   ( void );

    void                        RebuildModel                ( bool bForceClothes = false, bool bDelayChange = false );
    void                        ProcessRebuildPlayer        ( void );
    void                        SetStat                     ( unsigned short usStat, float fValue );
    float                       GetStat                     ( unsigned short usStat );
    void                        ResetStats                  ( void );

    inline CClientPlayerClothes* GetClothes                 ( void ) { return m_pClothes; }

    // This is kinda hacky, should be private but something depends on this. Should depend on some
    // streamer func. Perhaps use SetNeverStreamOut, but need something to reset that.
    void                        StreamIn                    ( bool bInstantly );
    void                        StreamOut                   ( void );

    void                        StreamOutWeaponForABit      ( eWeaponSlot eSlot );

    bool                        SetHasJetPack               ( bool bHasJetPack );
    bool                        HasJetPack                  ( void );

    float                       GetDistanceFromGround       ( void );

    bool                        IsInWater                   ( void );
    bool                        IsOnGround                  ( void );

    bool                        IsClimbing                  ( void );
    bool                        IsRadioOn                   ( void ) { return m_bRadioOn; };
    void                        NextRadioChannel            ( void );
    void                        PreviousRadioChannel        ( void );
    bool                        SetCurrentRadioChannel      ( unsigned char ucChannel );
    inline unsigned char        GetCurrentRadioChannel      ( void ) { return m_ucRadioChannel; };

    inline CTaskManager*        GetTaskManager              ( void ) { return m_pTaskManager; }

    bool                        GetShotData                 ( CVector * pvecOrigin, CVector * pvecTarget = NULL, CVector * pvecGunMuzzle = NULL, CVector * pvecFireOffset = NULL, float* fAimX = NULL, float* fAimY = NULL );
    CVector                     AdjustShotOriginForWalls    ( const CVector& vecOrigin, const CVector& vecTarget, float fMaxPullBack );

    eFightingStyle              GetFightingStyle            ( void );
    void                        SetFightingStyle            ( eFightingStyle style );

    eMoveAnim                   GetMoveAnim                 ( void );
    void                        SetMoveAnim                 ( eMoveAnim iAnim );

    inline void                 AddProjectile               ( CClientProjectile * pProjectile )         { m_Projectiles.push_back ( pProjectile ); }
    inline void                 RemoveProjectile            ( CClientProjectile * pProjectile )         { m_Projectiles.remove ( pProjectile ); }
    std::list < CClientProjectile* > ::iterator ProjectilesBegin ( void )                               { return m_Projectiles.begin (); }
    std::list < CClientProjectile* > ::iterator ProjectilesEnd   ( void )                               { return m_Projectiles.end (); }
    unsigned int                CountProjectiles            ( eWeaponType weaponType = WEAPONTYPE_UNARMED );

    void                        RemoveAllProjectiles        ( void );
    void                        DestroySatchelCharges       ( bool bBlow = true, bool bDestroy = true );
    
    CRemoteDataStorage*         GetRemoteData               ( void ) { return m_remoteDataStorage; }

    bool                        IsEnteringVehicle           ( void );
    bool                        IsLeavingVehicle            ( void );
    bool                        IsGettingIntoVehicle        ( void );
    bool                        IsGettingOutOfVehicle       ( void );
    bool                        IsGettingJacked             ( void );

    CClientEntity*              GetContactEntity            ( void );

    bool                        HasAkimboPointingUpwards    ( void );

    float                       GetDistanceFromCentreOfMassToBaseOfModel ( void );

    inline unsigned char        GetAlpha                    ( void )                                    { return m_ucAlpha; }
    void                        SetAlpha                    ( unsigned char ucAlpha );

    inline bool                 HasTargetPosition       ( void )                                        { return ( m_interp.pos.ulFinishTime != 0 ); }
    inline CClientEntity *      GetTargetOriginSource   ( void )                                        { return m_interp.pTargetOriginSource; }
    void                        GetTargetPosition       ( CVector & vecPosition );
    void                        SetTargetPosition       ( const CVector& vecPosition, unsigned long ulDelay, CClientEntity* pTargetOriginSource = NULL );
    void                        RemoveTargetPosition    ( void );
    void                        UpdateTargetPosition    ( void );

    CClientEntity*              GetTargetedEntity       ( void );    
    CClientPed*                 GetTargetedPed          ( void );

    inline CClientEntity *      GetCurrentContactEntity ( void )                                        { return m_pCurrentContactEntity; }
    inline void                 SetCurrentContactEntity ( CClientEntity * pEntity )                     { m_pCurrentContactEntity = pEntity; }

    bool                        IsSunbathing            ( void );
    void                        SetSunbathing           ( bool bSunbathing, bool bStartStanding = true );

    bool                        LookAt                  ( CVector vecOffset, int iTime = 1000, int iBlend = 1000, CClientEntity * pEntity = NULL );
    bool                        UseGun                  ( CVector vecTarget, CClientEntity * pEntity );

    bool                        IsAttachToable            ( void );
    static const char*          GetBodyPartName         ( unsigned char ucID );

    bool                        IsDoingGangDriveby      ( void );
    void                        SetDoingGangDriveby     ( bool bDriveby );

    bool                        IsRunningAnimation      ( void );
    void                        RunAnimation            ( AssocGroupId animGroup, AnimationId animID );
    void                        RunNamedAnimation       ( CAnimBlock * pBlock, const char * szAnimName, int iTime = -1, bool bLoop = true, bool bUpdatePosition = true, bool bInterruptable = false, bool bFreezeLastFrame = true, bool bRunInSequence = false, bool bOffsetPed = false, bool bHoldLastFrame = false );
    void                        KillAnimation           ( void );
    inline CAnimBlock *         GetAnimationBlock       ( void )                                        { return m_pAnimationBlock; }
    const char*                 GetAnimationName        ( void )                                        { return m_strAnimationName; }

    bool                        IsUsingGun              ( void );

    inline bool                 IsHeadless              ( void )                                        { return m_bHeadless; }
    void                        SetHeadless             ( bool bHeadless );

    inline bool                 IsFrozen                ( void ) const                                  { return m_bFrozen; }
    void                        SetFrozen               ( bool bFrozen );
    bool                        IsFrozenWaitingForGroundToLoad      ( void ) const;
    void                        SetFrozenWaitingForGroundToLoad     ( bool bFrozen );

    bool                        IsFootBloodEnabled      ( void );
    void                        SetFootBloodEnabled     ( bool bHasFootBlood );

    inline bool                 IsOnFire                ( void )                                        { return m_bIsOnFire; }
    void                        SetOnFire               ( bool bOnFire );

    void                        GetVoice                ( short* psVoiceType, short* psVoiceID );
    void                        GetVoice                ( const char** pszVoiceType, const char** pszVoice );
    void                        SetVoice                ( short sVoiceType, short sVoiceID );
    void                        SetVoice                ( const char* szVoiceType, const char* szVoice );

    bool                        IsSpeechEnabled         ( void );
    void                        SetSpeechEnabled        ( bool bEnabled );

    void                        PostWeaponFire          ( void );
    void                        SetBulletImpactData     ( CClientEntity* pVictim, const CVector& vecHitPosition );
    bool                        GetBulletImpactData     ( CClientEntity** ppVictim = 0, CVector* pvecHitPosition = 0 );
    void                        ClearBulletImpactData   ( void ) { m_bBulletImpactData = false; }

    bool                        CanReloadWeapon         ( void );
    bool                        ReloadWeapon            ( void );

    bool                        ShouldBeStealthAiming   ( void );
    inline bool                 IsStealthAiming         ( void )            { return m_bStealthAiming; }
    void                        SetStealthAiming        ( bool bAiming );

    CAnimBlendAssociation *     AddAnimation            ( AssocGroupId group, AnimationId id );
    CAnimBlendAssociation *     BlendAnimation          ( AssocGroupId group, AnimationId id, float fBlendDelta );
    CAnimBlendAssociation *     GetAnimation            ( AnimationId id );
    CAnimBlendAssociation *     GetFirstAnimation       ( void );

protected:
    // This constructor is for peds managed by a player. These are unknown to the ped manager.
                                CClientPed                  ( CClientManager* pManager, unsigned long ulModelID, ElementID ID, bool bIsLocalPlayer );

    void                        Init                        ( CClientManager* pManager, unsigned long ulModelID, bool bIsLocalPlayer );

    void                        Dump                        ( FILE* pFile, bool bDumpDetails, unsigned int uiIndex );
    void                        StreamedInPulse             ( void );

    void                        Interpolate                 ( void );
    void                        UpdateKeysync               ( bool bCleanup = false );

    // Used to destroy the current game ped and create a new one in the same state.
    void                        ReCreateModel               ( void );

    void                        _CreateModel                ( void );
    void                        _CreateLocalModel           ( void );
    void                        _DestroyModel               ( void );
    void                        _DestroyLocalModel          ( void );
    void                        _ChangeModel                ( void );

    void                        ModelRequestCallback        ( CModelInfo* pModelInfo );

    void                        InternalWarpIntoVehicle     ( CVehicle* pGameVehicle );
    void                        InternalRemoveFromVehicle   ( CVehicle* pGameVehicle );

    bool                        PerformChecks               ( void );
    void                        HandleWaitingForGroundToLoad ( void );

    // Used to start and stop radio for local player
    void                        StartRadio                  ( void );
    void                        StopRadio                   ( void );
    bool                        m_bDontChangeRadio;

public:
    void                        _GetIntoVehicle             ( CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor );

    void                        Respawn                     ( CVector * pvecPosition = NULL, bool bRestoreState = false, bool bCameraCut = false );

    void                        NotifyCreate            ( void );
    void                        NotifyDestroy           ( void );

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
    bool                        m_bArmorLocked;
    unsigned long               m_ulLastOnScreenTime;
    CClientVehicle*             m_pOccupiedVehicle;
    CClientVehicle*             m_pOccupyingVehicle;
    //unsigned int                m_uiOccupyingSeat;
    unsigned int                m_uiOccupiedVehicleSeat;
    bool                        m_bForceGettingIn;
    bool                        m_bForceGettingOut;
    CShotSyncData*              m_shotSyncData;
    CStatsData*                 m_stats;
    CControllerState*           m_currentControllerState;
    CControllerState*           m_lastControllerState;
    CRemoteDataStorage*         m_remoteDataStorage;
    unsigned long               m_ulLastTimeFired;
    unsigned long               m_ulLastTimeAimed;
    unsigned long               m_ulLastTimeBeganCrouch;
    unsigned long               m_ulLastTimeBeganStand;
    unsigned long               m_ulLastTimeMovedWhileCrouched;
    CModelInfo*                 m_pLoadedModelInfo;
    eWeaponSlot                 m_pOutOfVehicleWeaponSlot;
    float                       m_fBeginAimX;
    float                       m_fBeginAimY;
    float                       m_fTargetAimX;
    float                       m_fTargetAimY;
    unsigned long               m_ulBeginAimTime;
    unsigned long               m_ulTargetAimTime;
    bool                        m_bTargetAkimboUp;
    unsigned long               m_ulBeginRotationTime;
    unsigned long               m_ulEndRotationTime;
    float                       m_fBeginRotation;
    float                       m_fTargetRotationA;
    float                       m_fBeginCameraRotation;
    float                       m_fTargetCameraRotation;
    unsigned long               m_ulBeginTarget;
    unsigned long               m_ulEndTarget;
    CVector                     m_vecBeginSource;
    CVector                     m_vecBeginTarget;
    CVector                     m_vecBeginTargetAngle;
    CVector                     m_vecTargetSource;
    CVector                     m_vecTargetTarget;
    CVector                     m_vecTargetTargetAngle;
    CVector                     m_vecTargetInterpolateAngle;
    CClientEntity*              m_pTargetedEntity;
    std::list < SDelayedSyncData* >  m_SyncBuffer;
    bool                        m_bDucked;
    bool                        m_bWasDucked; //For knowing when to register standing up
    bool                        m_bIsChoking;
    bool                        m_bWearingGoggles;
    bool                        m_bVisible;
    bool                        m_bUsesCollision;
    float                       m_fHealth;
    float                       m_fArmor;
    bool                        m_bWorldIgnored;
    float                       m_fCurrentRotation;
    float                       m_fMoveSpeed;
    bool                        m_bCanBeKnockedOffBike;
    CMatrix                     m_Matrix;
    CVector                     m_vecMoveSpeed;
    CVector                     m_vecTurnSpeed;
    eWeaponSlot                 m_CurrentWeaponSlot;
    SFixedArray < eWeaponType, WEAPONSLOT_MAX > m_WeaponTypes;
    bool                        m_bHasJetPack;
    CClientPlayerClothes*       m_pClothes;
    eFightingStyle              m_FightingStyle;
    eMoveAnim                   m_MoveAnim;
    std::list < CClientProjectile* > m_Projectiles;
    unsigned char               m_ucAlpha;
    float                       m_fTargetRotation;
    int                         m_iVehicleInOutState;
    bool                        m_bRecreatingModel;
    CClientEntity *             m_pCurrentContactEntity;
    bool                        m_bSunbathing;
    CClientPad                  m_Pad;
    bool                        m_bDestroyingSatchels;
    bool                        m_bDoingGangDriveby;
    CAnimBlock *                m_pAnimationBlock;
    SString                     m_strAnimationName;
    bool                        m_bRequestedAnimation;
    int                         m_iTimeAnimation;
    bool                        m_bLoopAnimation;
    bool                        m_bUpdatePositionAnimation;
    bool                        m_bInterruptableAnimation;
    bool                        m_bFreezeLastFrameAnimation;
    bool                        m_bHeadless;
    bool                        m_bFrozen;
    bool                        m_bFrozenWaitingForGroundToLoad;
    float                       m_fGroundCheckTolerance;
    float                       m_fObjectsAroundTolerance;
    int                         m_iLoadAllModelsCounter;
    bool                        m_bIsOnFire;
    SLastSyncedPedData*         m_LastSyncedData;
    bool                        m_bSpeechEnabled;
    bool                        m_bStealthAiming;
    float                       m_fLighting;
    unsigned char               m_ucEnteringDoor;
    unsigned char               m_ucLeavingDoor;
    bool                        m_bPendingRebuildPlayer;
    uint                        m_uiFrameLastRebuildPlayer;

    bool                        m_bBulletImpactData;
    CClientEntity*              m_pBulletImpactEntity;
    CVector                     m_vecBulletImpactHit;

    // Time dependent interpolation
    struct
    {
        struct
        {
            CVector         vecTarget;
            CVector         vecError;
            unsigned long   ulStartTime;
            unsigned long   ulFinishTime;
            float           fLastAlpha;
        } pos;

        CClientEntity*      pTargetOriginSource;
        // These variables are used to track the last known position
        // of the contact entity for if it's removed during the
        // interpolation.
        bool                bHadOriginSource;
        CVector             vecOriginSourceLastPosition;
    } m_interp;

    // Hacks for player model replacement and weapon model replacement respectively
    unsigned long               m_ulStoredModel;
    DWORD                       m_dwStoredAmmo;
    DWORD                       m_dwStoredClipAmmo;
    bool                        m_bStoredCurrentWeapon;
    eWeaponType                 m_eStoredWeaponID;
};

#endif
