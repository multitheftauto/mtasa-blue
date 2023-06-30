/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CExplosionManagerSA.h
 *  PURPOSE:     Header file for explosion manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CExplosionManager.h>

class CExplosion;
class CExplosionSA;

#define MAX_EXPLOSIONS                  48

class CExplosionManagerSA : public CExplosionManager
{
public:
    CExplosionManagerSA();
    ~CExplosionManagerSA();

    CExplosion* AddExplosion(CEntity* pExplodingEntity, CEntity* pOwner, eExplosionType explosionType, CVector& vecPosition, unsigned int uiActivationDelay = 0,
                             bool bMakeSound = true, float fCamShake = -1.0f, bool bNoDamage = false);
    void        RemoveAllExplosions();
    CExplosion* GetExplosion(DWORD ID);
    CExplosion* FindFreeExplosion();

private:
    CExplosionSA* Explosions[MAX_EXPLOSIONS];
};
