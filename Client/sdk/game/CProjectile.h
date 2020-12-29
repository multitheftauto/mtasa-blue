/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CProjectile.h
 *  PURPOSE:     Projectile entity interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CObject.h"

class CProjectile : public virtual CObject
{
public:
    virtual ~CProjectile(){};

    virtual void Destroy(bool bBlow) = 0;
    virtual void SetProjectileInfo(class CProjectileInfo* pProjectileInfo) = 0;
    virtual bool CorrectPhysics() = 0;
};
