/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientProjectile.h
 *  PURPOSE:     Projectile entity class header
 *
 *****************************************************************************/

class CClientProjectile;

#pragma once

#include "CClientEntity.h"
#include "CClientCommon.h"

class CProjectile;
class CProjectileInfo;
class CClientEntity;
class CVector;
enum eWeaponType;
class CClientProjectileManager;
class CClientPed;
class CClientVehicle;

class CProjectileInitiateData
{
public:
    CProjectileInitiateData()
    {
        pvecPosition = NULL;
        pvecRotation = NULL;
        pvecVelocity = NULL;
        usModel = 0;
    }
    ~CProjectileInitiateData()
    {
        if (pvecPosition)
            delete pvecPosition;
        if (pvecRotation)
            delete pvecRotation;
        if (pvecVelocity)
            delete pvecVelocity;
    }
    CVector*       pvecPosition;
    CVector*       pvecRotation;
    CVector*       pvecVelocity;
    unsigned short usModel;
};

class CClientProjectile final : public CClientEntity
{
    DECLARE_CLASS(CClientProjectile, CClientEntity)
    friend class CClientProjectileManager;
    friend class CClientPed;
    friend class CClientVehicle;

public:
    CClientProjectile(class CClientManager* pManager, CProjectile* pProjectile, CProjectileInfo* pProjectileInfo, CClientEntity* pCreator,
                      CClientEntity* pTarget, eWeaponType weaponType, CVector* pvecOrigin, CVector* pvecTarget, float fForce, bool bLocal);
    ~CClientProjectile();

    eClientEntityType GetType() const { return CCLIENTPROJECTILE; }
    CEntity*          GetGameEntity() { return m_pProjectile; }
    const CEntity*    GetGameEntity() const { return m_pProjectile; }
    void              Unlink();

    void DoPulse();
    void Initiate(CVector& vecPosition, CVector& vecRotation, CVector& vecVelocity, unsigned short usModel);
    void Destroy(bool bBlow = true);

    bool           IsActive();
    bool           GetMatrix(CMatrix& matrix) const;
    bool           SetMatrix(const CMatrix& matrix);
    void           GetPosition(CVector& vecPosition) const;
    void           SetPosition(const CVector& vecPosition);
    void           GetRotationRadians(CVector& vecRotation) const;
    void           GetRotationDegrees(CVector& vecRotation) const;
    void           SetRotationRadians(const CVector& vecRotation);
    void           SetRotationDegrees(const CVector& vecRotation);
    void           GetVelocity(CVector& vecVelocity);
    void           SetVelocity(CVector& vecVelocity);
    unsigned short GetModel();
    void           SetModel(unsigned short usModel);
    void           SetCounter(DWORD dwCounter);
    DWORD          GetCounter();
    CClientEntity* GetCreator() { return m_pCreator; }
    CClientEntity* GetTargetEntity() { return m_pTarget; }
    eWeaponType    GetWeaponType() { return m_weaponType; }
    CVector*       GetOrigin() { return m_pvecOrigin; }
    CVector*       GetTarget() { return m_pvecTarget; }
    float          GetForce() { return m_fForce; }
    bool           IsLocal() { return m_bLocal; }
    CClientEntity* GetSatchelAttachedTo();

protected:
    CClientProjectileManager* m_pProjectileManager;
    bool                      m_bLinked;

    CProjectile*     m_pProjectile;
    CProjectileInfo* m_pProjectileInfo;

    CClientEntityPtr m_pCreator;
    CClientEntityPtr m_pTarget;
    eWeaponType      m_weaponType;
    CVector*         m_pvecOrigin;
    CVector*         m_pvecTarget;
    float            m_fForce;
    bool             m_bLocal;
    long long        m_llCreationTime;

    bool                     m_bInitiate;
    CProjectileInitiateData* m_pInitiateData;
    bool                     m_bCorrected;
};
