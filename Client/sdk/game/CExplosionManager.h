/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CExplosionManager.h
 *  PURPOSE:     Explosion manager interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"
#include "CExplosion.h"

class CEntity;
class CExplosion;
class CVector;

class CExplosionManager
{
public:
    virtual CExplosion* AddExplosion(CEntity* pExplodingEntity, CEntity* pOwner, eExplosionType explosionType, CVector& vecPosition,
                                     unsigned int uiActivationDelay = 0, bool bMakeSound = true, float fCamShake = -1.0f, bool bNoDamage = false) = 0;
    virtual void        RemoveAllExplosions() = 0;
    virtual CExplosion* GetExplosion(DWORD ID) = 0;
    virtual CExplosion* FindFreeExplosion() = 0;
};
