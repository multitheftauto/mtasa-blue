/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPed.h
 *  PURPOSE:     Ped entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

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
    MOVE_DEFAULT = 0,
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

inline bool IsValidMoveAnim(uint iMoveAnim)
{
    return (iMoveAnim == MOVE_DEFAULT) || (iMoveAnim >= MOVE_PLAYER && iMoveAnim <= MOVE_JETPACK) || (iMoveAnim >= MOVE_MAN && iMoveAnim <= MOVE_SKATE);
}

enum eBone
{
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
    CWeapon()
    {
        ucType = 0;
        usAmmo = 0;
        usAmmoInClip = 0;
    }
    std::uint8_t  ucType;
    std::uint16_t usAmmo;
    std::uint16_t usAmmoInClip;
};

class CPed : public CElement
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

    CPed(class CPedManager* pPedManager, CElement* pParent, std::uint16_t usModel);
    ~CPed();
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    bool IsEntity() { return true; }

    virtual void Unlink();

    bool HasValidModel();

    bool           IsPlayer() { return m_bIsPlayer; }
    std::uint16_t GetModel() { return m_usModel; };
    void           SetModel(std::uint16_t usModel) { m_usModel = usModel; };

    bool IsDucked() { return m_bDucked; };
    void SetDucked(bool bDucked) { m_bDucked = bDucked; };

    bool IsChoking() { return m_bIsChoking; };
    void SetChoking(bool bChoking) { m_bIsChoking = bChoking; };

    bool IsWearingGoggles() { return m_bWearingGoggles; };
    void SetWearingGoggles(bool bWearingGoggles) { m_bWearingGoggles = bWearingGoggles; };

    bool IsOnFire() { return m_bIsOnFire; }
    void SetOnFire(bool bOnFire) { m_bIsOnFire = bOnFire; }

    CWeapon*       GetWeapon(std::uint8_t ucSlot = 0xFF);
    std::uint8_t  GetWeaponSlot() { return m_ucWeaponSlot; }
    void           SetWeaponSlot(std::uint8_t ucSlot);
    std::uint8_t  GetCurrentWeaponState() { return m_ucCurrentWeaponState; };
    void           SetCurrentWeaponState(std::uint8_t ucWeaponState) { m_ucCurrentWeaponState = ucWeaponState; };
    std::uint8_t  GetWeaponType(std::uint8_t ucSlot = 0xFF);
    void           SetWeaponType(std::uint8_t ucType, std::uint8_t ucSlot = 0xFF);
    std::uint16_t GetWeaponAmmoInClip(std::uint8_t ucSlot = 0xFF);
    void           SetWeaponAmmoInClip(std::uint16_t uscAmmoInClip, std::uint8_t ucSlot = 0xFF);
    std::uint16_t GetWeaponTotalAmmo(std::uint8_t ucSlot = 0xFF);
    void           SetWeaponTotalAmmo(std::uint16_t usTotalAmmo, std::uint8_t ucSlot = 0xFF);
    bool           HasWeaponType(std::uint8_t ucWeaponType);

    float GetMaxHealth();
    float GetHealth() { return m_fHealth; }
    void  SetHealth(float fHealth) { m_fHealth = fHealth; }
    float GetArmor() { return m_fArmor; }
    void  SetArmor(float fArmor) { m_fArmor = fArmor; }

    float GetPlayerStat(std::uint16_t usStat) { return (usStat < NUM_PLAYER_STATS) ? m_fStats[usStat] : 0; }
    void  SetPlayerStat(std::uint16_t usStat, float fValue)
    {
        if (usStat < NUM_PLAYER_STATS)
            m_fStats[usStat] = fValue;
    }

    CPlayerClothes* GetClothes() { return m_pClothes; }

    static const char* GetBodyPartName(std::uint8_t ucID);

    bool HasJetPack() { return m_bHasJetPack; }
    void SetHasJetPack(bool bHasJetPack) { m_bHasJetPack = bHasJetPack; }

    bool IsInWater() { return m_bInWater; }
    void SetInWater(bool bInWater) { m_bInWater = bInWater; }

    bool IsOnGround() { return m_bOnGround; }
    void SetOnGround(bool bOnGround) { m_bOnGround = bOnGround; }

    std::uint8_t GetAlpha() { return m_ucAlpha; }
    void          SetAlpha(std::uint8_t ucAlpha) { m_ucAlpha = ucAlpha; }

    CPlayerTasks* GetTasks() { return m_pTasks; }

    CElement* GetContactElement() { return m_pContactElement; }
    void      SetContactElement(CElement* pElement);

    void GetContactPosition(CVector& vecPosition) { vecPosition = m_vecContactPosition; }
    void SetContactPosition(CVector& vecPosition) { m_vecContactPosition = vecPosition; }

    bool IsDead() { return m_bIsDead; };
    void SetIsDead(bool bDead);

    bool IsSpawned() { return m_bSpawned; }
    void SetSpawned(bool bSpawned) { m_bSpawned = bSpawned; }

    float GetRotation() { return m_fRotation; }
    void  SetRotation(float fRotation) { m_fRotation = fRotation; }

    void GetRotation(CVector& vecRotation);
    void GetMatrix(CMatrix& matrix);
    void SetMatrix(const CMatrix& matrix);

    CElement* GetTargetedElement() { return m_pTargetedEntity; }
    void      SetTargetedElement(CElement* pEntity) { m_pTargetedEntity = pEntity; }

    std::uint8_t GetFightingStyle() { return m_ucFightingStyle; }
    void          SetFightingStyle(std::uint8_t ucStyle) { m_ucFightingStyle = ucStyle; }

    std::uint8_t GetMoveAnim() { return static_cast<std::uint8_t>(m_iMoveAnim); }
    void          SetMoveAnim(int iMoveAnim) { m_iMoveAnim = iMoveAnim; }

    float GetGravity() { return m_fGravity; }
    void  SetGravity(float fGravity) { m_fGravity = fGravity; }

    CVehicle*    GetOccupiedVehicle() { return m_pVehicle; };
    std::uint32_t GetOccupiedVehicleSeat() { return m_uiVehicleSeat; };
    CVehicle*    SetOccupiedVehicle(CVehicle* pVehicle, std::uint32_t uiSeat);

    std::uint32_t GetVehicleAction() { return m_uiVehicleAction; };
    void         SetVehicleAction(std::uint32_t uiAction);

    bool IsAttachToable();

    void GetVelocity(CVector& vecVelocity) { vecVelocity = m_vecVelocity; };
    void SetVelocity(const CVector& vecVelocity) { m_vecVelocity = vecVelocity; };

    bool IsDoingGangDriveby() { return m_bDoingGangDriveby; }
    void SetDoingGangDriveby(bool bDriveby) { m_bDoingGangDriveby = bDriveby; }

    bool IsHeadless() { return m_bHeadless; };
    void SetHeadless(bool bHeadless) { m_bHeadless = bHeadless; };

    bool IsFrozen() { return m_bFrozen; };
    void SetFrozen(bool bFrozen) { m_bFrozen = bFrozen; };

    class CPlayer* GetSyncer() { return m_pSyncer; };
    void           SetSyncer(class CPlayer* pPlayer);

    bool     IsSyncable() { return m_bSyncable; };
    void     SetSyncable(bool bSynced) { m_bSyncable = bSynced; };
    CPlayer* m_pSyncer;

    CVehicle* GetJackingVehicle() { return m_pJackingVehicle; }
    void      SetJackingVehicle(CVehicle* pVehicle);

    bool IsStealthAiming() { return m_bStealthAiming; }
    void SetStealthAiming(bool bAiming) { m_bStealthAiming = bAiming; }

    bool GetCollisionEnabled() { return m_bCollisionsEnabled; }
    void SetCollisionEnabled(bool bCollisionEnabled) { m_bCollisionsEnabled = bCollisionEnabled; }

    long long GetLastFarSyncTick() { return m_llLastFarSyncTick; }
    void      SetLastFarSyncTick(long long llLastSyncTick) { m_llLastFarSyncTick = llLastSyncTick; }

    void                                  ClearNearPlayersList() { m_nearPlayersList.clear(); }
    void                                  AddPlayerToNearList(CPlayer* pPlayer) { m_nearPlayersList.push_back(pPlayer); }
    bool                                  IsNearPlayersListEmpty() { return m_nearPlayersList.empty(); }
    std::vector<CPlayer*>::const_iterator NearPlayersIterBegin() { return m_nearPlayersList.begin(); }
    std::vector<CPlayer*>::const_iterator NearPlayersIterEnd() { return m_nearPlayersList.end(); }

protected:
    bool ReadSpecialData(const int iLine) override;

protected:
    std::uint16_t                       m_usModel;
    CMatrix                              m_Matrix;
    bool                                 m_bDucked;
    bool                                 m_bIsChoking;
    bool                                 m_bWearingGoggles;
    bool                                 m_bIsOnFire;
    float                                m_fHealth;
    float                                m_fArmor;
    SFixedArray<float, NUM_PLAYER_STATS> m_fStats;
    CPlayerClothes*                      m_pClothes;
    bool                                 m_bHasJetPack;
    bool                                 m_bInWater;
    bool                                 m_bOnGround;
    bool                                 m_bIsPlayer;
    CPlayerTasks*                        m_pTasks;
    SFixedArray<CWeapon, WEAPON_SLOTS>   m_Weapons;
    std::uint8_t                        m_ucWeaponSlot;
    std::uint8_t                        m_ucCurrentWeaponState;
    std::uint8_t                        m_ucAlpha;
    CElement*                            m_pContactElement;
    CVector                              m_vecContactPosition;
    bool                                 m_bIsDead;
    float                                m_fRotation;
    bool                                 m_bSpawned;
    CElement*                            m_pTargetedEntity;
    std::uint8_t                        m_ucFightingStyle;
    int                                  m_iMoveAnim;
    float                                m_fGravity;
    CVector                              m_vecVelocity;
    bool                                 m_bDoingGangDriveby;
    bool                                 m_bHeadless;
    bool                                 m_bFrozen;
    bool                                 m_bStealthAiming;
    CVehicle*                            m_pJackingVehicle;

    CVehicle*    m_pVehicle;
    std::uint32_t m_uiVehicleSeat;
    std::uint32_t m_uiVehicleAction;

    bool m_bSyncable;
    bool m_bCollisionsEnabled;

    long long             m_llLastFarSyncTick = 0;
    std::vector<CPlayer*> m_nearPlayersList;

private:
    CPedManager* m_pPedManager;
};
