/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWeaponSA.h
 *  PURPOSE:     Header file for weapon class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CWeapon.h>
#include <game/CPed.h>

#define FUNC_Shutdown                                   0x73A380
#define FUNC_CWeapon_CheckForShootingVehicleOccupant    0x73f480
#define FUNC_CWeapon_Initialize                         0x73b4a0
#define FUNC_CWeapon_Update                             0x73db40
#define FUNC_CWeapon_Fire                               0x742300
#define FUNC_CWeapon_AddGunshell                        0x73a3e0
#define FUNC_CWeapon_DoBulletImpact                     0x73b550
#define FUNC_CWeapon_GenerateDamageEvent                0x73a530
#define FUNC_CWeapon_FireInstantHit                     0x73FB10
#define FUNC_CWeaponInfo_GetWeaponReloadTime            0x743D70
#define FUNC_CBirds_CheckForHit                         0x712E40
#define FUNC_CShadows_CheckForHit                       0x707550

class CWeaponSAInterface
{
public:
    eWeaponType  m_eWeaponType;
    eWeaponState m_eState;
    DWORD        m_nAmmoInClip;
    DWORD        m_nAmmoTotal;
    DWORD        m_nTimer;
    DWORD        m_Unknown;
    DWORD        m_Unknown_2;
};

class CWeaponSA : public CWeapon
{
private:
    CWeaponSAInterface* m_pInterface;
    CPed*               m_pOwner;
    eWeaponSlot         m_weaponSlot;

public:
    CWeaponSA(CWeaponSAInterface* pInterface, CPed* pOwner, eWeaponSlot weaponSlot);
    eWeaponType  GetType();
    void         SetType(eWeaponType type);
    eWeaponState GetState();
    void         SetState(eWeaponState state);
    DWORD        GetAmmoInClip();
    void         SetAmmoInClip(DWORD dwAmmoInClip);
    DWORD        GetAmmoTotal();
    void         SetAmmoTotal(DWORD dwAmmoTotal);

    CPed*       GetPed();
    eWeaponSlot GetSlot();

    void          SetAsCurrentWeapon();
    CWeaponInfo* GetInfo(eWeaponSkill skill);
    void         Destroy();
    void         Remove();
    void         Initialize(eWeaponType type, unsigned int uiAmmo, CPed* pPed);
    void         Update(CPed* pPed);
    bool         Fire(CEntity* pFiringEntity, CVector* pvecOrigin, CVector* pvecOffset, CEntity* pTargetEntity, CVector* pvec_1, CVector* pvec_2);
    void         AddGunshell(CEntity* pFiringEntity, CVector* pvecOrigin, CVector2D* pvecDirection, float fSize);
    void DoBulletImpact(CEntity* pFiringEntity, CEntitySAInterface* pEntityInterface, CVector* pvecOrigin, CVector* pvecTarget, CColPoint* pColPoint, int i_1);
    unsigned char GenerateDamageEvent(CPed* pPed, CEntity* pResponsible, eWeaponType weaponType, int iDamagePerHit, ePedPieceTypes hitZone, int i_2);
    bool ProcessLineOfSight(const CVector* vecStart, const CVector* vecEnd, CColPoint** colCollision, CEntity** CollisionEntity, const SLineOfSightFlags flags,
                            SLineOfSightBuildingResult* pBuildingResult, eWeaponType weaponType, CEntitySAInterface** pEntity);
    bool FireInstantHit(CEntity* pFiringEntity, const CVector* pvecOrigin, const CVector* pvecMuzzle, CEntity* pTargetEntity, const CVector* pvecTarget,
                        const CVector* pvec, bool bCrossHairGun, bool bCreateGunFx);
    bool FireBullet(CEntity* pFiringEntity, const CVector& vecOrigin, const CVector& vecTarget);
    int  GetWeaponReloadTime(CWeaponStat* pWeaponStat);
    static int GetWeaponFireTime(CWeaponStat* pWeaponStat);
};
