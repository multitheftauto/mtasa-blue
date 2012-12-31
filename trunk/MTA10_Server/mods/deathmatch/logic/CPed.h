/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPed.h
*  PURPOSE:     Ped entity class
*  DEVELOPERS:  Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPed_H
#define __CPed_H

#include "CCommon.h"
#include "CElement.h"
#include "CPlayerClothes.h"
#include "CVehicle.h"
#include "CPlayerTasks.h"
#include <CMatrix.h>

#define INVALID_VEHICLE_SEAT 0xFF
#define NUM_PLAYER_STATS 343
#define WEAPON_SLOTS 13
#define STEALTH_KILL_RANGE 2.5f

enum ePedMoveAnim
{
    MOVE_PLAYER = 54,
    MOVE_PLAYER_F,
    MOVE_PLAYER_M,
    MOVE_ROCKET,
    MOVE_ROCKET_F,
    MOVE_ROCKET_M,
    MOVE_ARMED,
    MOVE_ARMED_F,
    MOVE_ARMED_M,
    MOVE_BBBAT,
    MOVE_BBBAT_F,
    MOVE_BBBAT_M,
    MOVE_CSAW,
    MOVE_CSAW_F,
    MOVE_CSAW_M,
    MOVE_SNEAK,
    MOVE_JETPACK,
    MOVE_MAN = 118,
    MOVE_SHUFFLE,
    MOVE_OLDMAN,
    MOVE_GANG1,
    MOVE_GANG2,
    MOVE_OLDFATMAN,
    MOVE_FATMAN,
    MOVE_JOGGER,
    MOVE_DRUNKMAN,
    MOVE_BLINDMAN,
    MOVE_SWAT,
    MOVE_WOMAN,
    MOVE_SHOPPING,
    MOVE_BUSYWOMAN,
    MOVE_SEXYWOMAN,
    MOVE_PRO,
    MOVE_OLDWOMAN,
    MOVE_FATWOMAN,
    MOVE_JOGWOMAN,
    MOVE_OLDFATWOMAN,
    MOVE_SKATE,
};

enum eBone { 
    BONE_PELVIS1 = 1,
    BONE_PELVIS,
    BONE_SPINE1,
    BONE_UPPERTORSO,
    BONE_NECK,
    BONE_HEAD2,
    BONE_HEAD1,
    BONE_HEAD,
    BONE_RIGHTUPPERTORSO = 21,
    BONE_RIGHTSHOULDER,
    BONE_RIGHTELBOW,
    BONE_RIGHTWRIST,
    BONE_RIGHTHAND,
    BONE_RIGHTTHUMB,
    BONE_LEFTUPPERTORSO = 31,
    BONE_LEFTSHOULDER,
    BONE_LEFTELBOW,
    BONE_LEFTWRIST,
    BONE_LEFTHAND,
    BONE_LEFTTHUMB,
    BONE_LEFTHIP = 41,
    BONE_LEFTKNEE,
    BONE_LEFTANKLE,
    BONE_LEFTFOOT,
    BONE_RIGHTHIP = 51,
    BONE_RIGHTKNEE,
    BONE_RIGHTANKLE,
    BONE_RIGHTFOOT
};

class CWeapon
{
public:
    inline  CWeapon ( void ) { ucType = 0; usAmmo = 0; usAmmoInClip = 0; }
    unsigned char ucType;
    unsigned short usAmmo;
    unsigned short usAmmoInClip;
};

class CPed: public CElement
{
    friend class CElement;
public:
    enum
    {
        VEHICLEACTION_NONE,
        VEHICLEACTION_ENTERING,
        VEHICLEACTION_EXITING,
        VEHICLEACTION_JACKING,
        VEHICLEACTION_JACKED,
    };

                                                CPed                        ( class CPedManager* pPedManager, CElement* pParent, CXMLNode* pNode, unsigned short usModel );
                                                ~CPed                       ( void );

    bool                                        IsEntity                    ( void )                    { return true; }

    virtual void                                Unlink                      ( void );
    virtual bool                                ReadSpecialData             ( void );

    bool                                        HasValidModel               ( void );

    inline bool                                 IsPlayer                    ( void )                            { return m_bIsPlayer; }
    inline unsigned short                       GetModel                    ( void )                            { return m_usModel; };
    inline void                                 SetModel                    ( unsigned short usModel )          { m_usModel = usModel; };

    inline bool                                 IsDucked                    ( void )                            { return m_bDucked; };
    inline void                                 SetDucked                   ( bool bDucked )                    { m_bDucked = bDucked; };

    inline bool                                 IsChoking                   ( void )                            { return m_bIsChoking; };
    inline void                                 SetChoking                  ( bool bChoking )                   { m_bIsChoking = bChoking; };

    inline bool                                 IsWearingGoggles            ( void )                            { return m_bWearingGoggles; };
    inline void                                 SetWearingGoggles           ( bool bWearingGoggles )            { m_bWearingGoggles = bWearingGoggles; };

    inline bool                                 IsOnFire                    ( void )                            { return m_bIsOnFire; }
    inline void                                 SetOnFire                   ( bool bOnFire )                    { m_bIsOnFire = bOnFire; }

    CWeapon*                                    GetWeapon                   ( unsigned char ucSlot = 0xFF );
    inline unsigned char                        GetWeaponSlot               ( void )                            { return m_ucWeaponSlot; }
    void                                        SetWeaponSlot               ( unsigned char ucSlot );
    inline unsigned char                        GetCurrentWeaponState       ( void )                            { return m_ucCurrentWeaponState; };
    inline void                                 SetCurrentWeaponState       ( unsigned char ucWeaponState )     { m_ucCurrentWeaponState = ucWeaponState; };
    unsigned char                               GetWeaponType               ( unsigned char ucSlot = 0xFF );
    void                                        SetWeaponType               ( unsigned char ucType, unsigned char ucSlot = 0xFF );
    unsigned short                              GetWeaponAmmoInClip         ( unsigned char ucSlot = 0xFF );
    void                                        SetWeaponAmmoInClip         ( unsigned short uscAmmoInClip, unsigned char ucSlot = 0xFF );
    unsigned short                              GetWeaponTotalAmmo          ( unsigned char ucSlot = 0xFF );
    void                                        SetWeaponTotalAmmo          ( unsigned short usTotalAmmo, unsigned char ucSlot = 0xFF );

    float                                       GetMaxHealth                ( void );
    inline float                                GetHealth                   ( void )                            { return m_fHealth; }
    inline void                                 SetHealth                   ( float fHealth )                   { m_fHealth = fHealth; }
    inline unsigned long                        GetHealthChangeTime         ( void )                            { return m_ulHealthChangeTime; }
    inline void                                 SetHealthChangeTime         ( unsigned long ulTime )            { m_ulHealthChangeTime = ulTime; }
    inline float                                GetArmor                    ( void )                            { return m_fArmor; }
    inline void                                 SetArmor                    ( float fArmor )                    { m_fArmor = fArmor; }
    inline unsigned long                        GetArmorChangeTime          ( void )                            { return m_ulArmorChangeTime; }
    inline void                                 SetArmorChangeTime          ( unsigned long ulTime )            { m_ulArmorChangeTime = ulTime; }
    
    inline float                                GetPlayerStat               ( unsigned short usStat )       { return ( usStat < NUM_PLAYER_STATS ) ? m_fStats [ usStat ] : 0; }
    inline void                                 SetPlayerStat               ( unsigned short usStat, float fValue ) { if ( usStat < NUM_PLAYER_STATS ) m_fStats [ usStat ] = fValue; } 

    inline CPlayerClothes*                      GetClothes                  ( void )                        { return m_pClothes; }

    static const char*                          GetBodyPartName             ( unsigned char ucID );

    inline bool                                 HasJetPack                  ( void )                        { return m_bHasJetPack; }
    inline void                                 SetHasJetPack               ( bool bHasJetPack )            { m_bHasJetPack = bHasJetPack; }

    inline bool                                 IsInWater                   ( void )                        { return m_bInWater; }
    inline void                                 SetInWater                  ( bool bInWater )               { m_bInWater = bInWater; }

    inline bool                                 IsOnGround                  ( void )                        { return m_bOnGround; }
    inline void                                 SetOnGround                 ( bool bOnGround )              { m_bOnGround = bOnGround; }

    inline unsigned char                        GetAlpha                    ( void )                        { return m_ucAlpha; }
    inline void                                 SetAlpha                    ( unsigned char ucAlpha )       { m_ucAlpha = ucAlpha; }

    inline CPlayerTasks*                        GetTasks                    ( void )                        { return m_pTasks; }
    
    inline CElement*                            GetContactElement           ( void )                        { return m_pContactElement; }
    void                                        SetContactElement           ( CElement* pElement );
    
    inline void                                 GetContactPosition          ( CVector& vecPosition )        { vecPosition = m_vecContactPosition; }
    inline void                                 SetContactPosition          ( CVector& vecPosition )        { m_vecContactPosition = vecPosition; }

    inline bool                                 IsDead                      ( void )                        { return m_bIsDead; };
    void                                        SetIsDead                   ( bool bDead );
    inline unsigned long                        GetLastDieTime              ( void )                        { return m_ulLastDieTime; };

    inline bool                                 IsSpawned                   ( void )                        { return m_bSpawned; }
    inline void                                 SetSpawned                  ( bool bSpawned )               { m_bSpawned = bSpawned; }

    inline float                                GetRotation                 ( void )                        { return m_fRotation; }
    inline void                                 SetRotation                 ( float fRotation )             { m_fRotation = fRotation; }

    inline CElement*                            GetTargetedElement          ( void )                        { return m_pTargetedEntity; }
    inline void                                 SetTargetedElement          ( CElement* pEntity )           { m_pTargetedEntity = pEntity; }
    
    inline unsigned char                        GetFightingStyle            ( void )                        { return m_ucFightingStyle; }
    inline void                                 SetFightingStyle            ( unsigned char ucStyle )       { m_ucFightingStyle = ucStyle; }

    inline unsigned char                        GetMoveAnim                 ( void )                        { return m_iMoveAnim; }
    inline void                                 SetMoveAnim                 ( int iMoveAnim )               { m_iMoveAnim = iMoveAnim; }

    inline float                                GetGravity                  ( void )                        { return m_fGravity; }
    inline void                                 SetGravity                  ( float fGravity )              { m_fGravity = fGravity; }

    inline CVehicle*                            GetOccupiedVehicle          ( void )                        { return m_pVehicle; };
    inline unsigned int                         GetOccupiedVehicleSeat      ( void )                        { return m_uiVehicleSeat; };
    CVehicle*                                   SetOccupiedVehicle          ( CVehicle* pVehicle, unsigned int uiSeat );
    inline unsigned long                        GetVehicleActionStartTime   ( void )                        { return m_ulVehicleActionStartTime; };

    inline unsigned int                         GetVehicleAction            ( void )                        { return m_uiVehicleAction; };
    void                                        SetVehicleAction            ( unsigned int uiAction );

    bool                                        IsAttachToable              ( void );

    inline void                                 GetVelocity                 ( CVector& vecVelocity )               { vecVelocity = m_vecVelocity; };
    inline void                                 SetVelocity                 ( const CVector& vecVelocity )         { m_vecVelocity = vecVelocity; };

    inline bool                                 IsDoingGangDriveby          ( void )                        { return m_bDoingGangDriveby; }
    inline void                                 SetDoingGangDriveby         ( bool bDriveby )               { m_bDoingGangDriveby = bDriveby; }

    inline bool                                 IsHeadless                  ( void )                        { return m_bHeadless; };
    inline void                                 SetHeadless                 ( bool bHeadless )              { m_bHeadless = bHeadless; };

    inline bool                                 IsFrozen                    ( void )                        { return m_bFrozen; };
    inline void                                 SetFrozen                   ( bool bFrozen )                { m_bFrozen = bFrozen; };

    inline class CPlayer*                       GetSyncer                   ( void )                        { return m_pSyncer; };
    void                                        SetSyncer                   ( class CPlayer* pPlayer );

    bool                                        IsSyncable                  ( void )                        { return m_bSyncable; };
    void                                        SetSyncable                 ( bool bSynced )                { m_bSyncable = bSynced; };
    CPlayer*                                    m_pSyncer;

    inline bool                                 IsStealthAiming             ( void )                        { return m_bStealthAiming; }
    inline void                                 SetStealthAiming            ( bool bAiming )                { m_bStealthAiming = bAiming; }

    inline bool                                 GetCollisionEnabled         ( void )                        { return m_bCollisionsEnabled; }
    inline void                                 SetCollisionEnabled         ( bool bCollisionEnabled )      { m_bCollisionsEnabled = bCollisionEnabled; }

protected:
    unsigned short                              m_usModel;
    CMatrix                                     m_Matrix;    
    bool                                        m_bDucked;
    bool                                        m_bIsChoking;
    bool                                        m_bWearingGoggles;
    bool                                        m_bIsOnFire;
    float                                       m_fHealth;
    unsigned long                               m_ulHealthChangeTime;
    float                                       m_fArmor;
    unsigned long                               m_ulArmorChangeTime;
    SFixedArray < float, NUM_PLAYER_STATS >    m_fStats;
    CPlayerClothes*                             m_pClothes;
    bool                                        m_bHasJetPack;
    bool                                        m_bInWater;
    bool                                        m_bOnGround;
    bool                                        m_bIsPlayer;
    CPlayerTasks*                               m_pTasks;
    SFixedArray < CWeapon, WEAPON_SLOTS >      m_Weapons;
    unsigned char                               m_ucWeaponSlot;
    unsigned char                               m_ucCurrentWeaponState;
    unsigned char                               m_ucAlpha;
    CElement*                                   m_pContactElement;
    CVector                                     m_vecContactPosition;
    bool                                        m_bIsDead;
    unsigned long                               m_ulLastDieTime;
    float                                       m_fRotation;
    bool                                        m_bSpawned;
    CElement*                                   m_pTargetedEntity;
    unsigned char                               m_ucFightingStyle;
    int                                         m_iMoveAnim;
    float                                       m_fGravity;
    CVector                                     m_vecVelocity;
    bool                                        m_bDoingGangDriveby;
    bool                                        m_bHeadless;
    bool                                        m_bFrozen;
    bool                                        m_bStealthAiming;

    CVehicle*                                   m_pVehicle;
    unsigned int                                m_uiVehicleSeat;
    unsigned int                                m_uiVehicleAction;
    unsigned long                               m_ulVehicleActionStartTime;

    bool                                        m_bSyncable;
    bool                                        m_bCollisionsEnabled;

private:
    CPedManager*                                m_pPedManager;
};

#endif
