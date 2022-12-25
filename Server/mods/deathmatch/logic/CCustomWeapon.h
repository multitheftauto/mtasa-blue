/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CCustomWeapon.h
 *  PURPOSE:     header file for custom weapons.
 *
 *****************************************************************************/

#pragma once
#include "CCommon.h"
#include "CCustomWeaponManager.h"
#include "CWeaponStat.h"

struct SLineOfSightFlags
{
    SLineOfSightFlags()
        : bCheckBuildings(true),
          bCheckVehicles(true),
          bCheckPeds(true),
          bCheckObjects(true),
          bCheckDummies(true),
          bSeeThroughStuff(false),
          bIgnoreSomeObjectsForCamera(false),
          bShootThroughStuff(false),
          bCheckCarTires(false)
    {
    }
    bool bCheckBuildings;
    bool bCheckVehicles;
    bool bCheckPeds;
    bool bCheckObjects;
    bool bCheckDummies;
    bool bSeeThroughStuff;
    bool bIgnoreSomeObjectsForCamera;
    bool bShootThroughStuff;            // not used for IsLineOfSightClear
    bool bCheckCarTires;
};

enum eTargetType
{
    TARGET_TYPE_VECTOR,
    TARGET_TYPE_ENTITY,
    TARGET_TYPE_FIXED,
};

struct SWeaponConfiguration
{
    bool              bShootIfTargetOutOfRange;
    bool              bShootIfTargetBlocked;
    bool              bDisableWeaponModel;
    bool              bInstantReload;
    SLineOfSightFlags flags;
};

class CCustomWeapon final : public CObject
{
public:
    CCustomWeapon(CElement* pParent, CObjectManager* pObjectManager, CCustomWeaponManager* pWeaponManager, eWeaponType weaponType);
    ~CCustomWeapon();

    void SetWeaponTarget(CElement* pTarget, int subTarget);
    void SetWeaponTarget(CVector vecTarget);
    void ResetWeaponTarget();

    eWeaponType GetWeaponType() { return m_Type; }

    eWeaponState GetWeaponState() { return m_State; }
    void         SetWeaponState(eWeaponState state) { m_State = state; }

    void SetClipAmmo(int iAmmo) { m_nAmmoInClip = iAmmo; }
    int  GetClipAmmo() { return m_nAmmoInClip; }

    void SetAmmo(int iAmmo) { m_nAmmoTotal = iAmmo; }
    int  GetAmmo() { return m_nAmmoTotal; }

    CWeaponStat* GetWeaponStat() { return m_pWeaponStat; }

    CPlayer* GetOwner() { return m_pOwner; }
    void     SetOwner(CPlayer* pOwner) { m_pOwner = pOwner; }

    eTargetType GetTargetType() { return m_targetType; }
    eBone       GetTargetBone() { return m_targetBone; }
    int         GetTargetWheel() { return m_itargetWheel; }
    CVector     GetVectorTarget() { return m_vecTarget; }
    CElement*   GetElementTarget() { return m_pTarget; }
    bool        HasChangedStats() { return m_pWeaponStat->HasChanged(); }

    bool SetFlags(eWeaponFlags flags, bool bData);
    bool SetFlags(const SLineOfSightFlags flags);
    void SetFlags(const SWeaponConfiguration weaponConfig) { m_weaponConfig = weaponConfig; }

    bool                 GetFlags(eWeaponFlags flags, bool& bData);
    bool                 GetFlags(SLineOfSightFlags& flags);
    SWeaponConfiguration GetFlags() { return m_weaponConfig; }

    static int GetWeaponFireTime(CWeaponStat* pWeaponStat);
    void       SetWeaponFireTime(int iWeaponFireTime);
    int        GetWeaponFireTime();
    void       ResetWeaponFireTime();

private:
    eWeaponType          m_Type;
    eWeaponState         m_State;
    DWORD                m_dwAmmoInClip;
    CVector              m_vecTargetDirection;
    CVector              m_vecLastDirection;
    bool                 m_bHasTargetDirection;
    short                m_sDamage;
    CWeaponStat*         m_pWeaponStat;
    CElement*            m_pTarget;
    CVector              m_vecTarget;
    eTargetType          m_targetType;
    eBone                m_targetBone;
    int                  m_itargetWheel;
    SWeaponConfiguration m_weaponConfig;
    CPlayer*             m_pOwner;            // For lag compensation
    int                  m_nAmmoTotal;
    int                  m_nAmmoInClip;
    eWeaponState         m_PreviousState;
    CElapsedTime         m_fireTimer;
    CElapsedTime         m_reloadTimer;
    unsigned char        m_ucCounter;
    int                  m_iWeaponFireRate;

    CCustomWeaponManager* m_pWeaponManager;
};
