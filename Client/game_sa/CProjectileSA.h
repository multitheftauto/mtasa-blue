/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileSA.h
 *  PURPOSE:     Header file for projectile entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CProjectile.h>
#include "CObjectSA.h"

class CProjectileSAInterface : public CObjectSAInterface            // entirely inherited from CObject
{
public:
    bool IsProjectableVTBL() const { return GetVTBL() == (void*)0x867030; };
};

class CProjectileSA : public virtual CProjectile, public virtual CObjectSA
{
private:
    CProjectileSAInterface* internalInterface;
    class CProjectileInfo*  projectileInfo;
    bool                    m_bDestroyed;

public:
    CProjectileSA(class CProjectileSAInterface* projectileInterface);
    ~CProjectileSA();
    void Destroy(bool bBlow = true);
    CProjectileSAInterface* GetProjectileInterface() const { return static_cast<CProjectileSAInterface*>(m_pInterface); };

    bool CalculateImpactPosition(CEntitySAInterface* pCollidedWith, CVector vecInputStart, CVector& vecInputEnd);

    void SetProjectileInfo(CProjectileInfo* pProjectileInfo) { projectileInfo = pProjectileInfo; }
    bool CorrectPhysics();
};
