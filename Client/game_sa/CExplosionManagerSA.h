/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CExplosionManagerSA.h
 *  PURPOSE:     Header file for explosion manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CExplosionManager.h>

class CExplosion;
class CExplosionSA;

#define FUNC_CExplosion_AddExplosion    0x736A50

#define ARRAY_Explosions                0xC88950
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
