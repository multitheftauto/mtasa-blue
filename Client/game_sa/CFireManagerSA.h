/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFireManagerSA.h
 *  PURPOSE:     Header file for fire manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CFireManager.h>

class CEntity;
class CFireSA;
class CFireSAInterface;

#define FUNC_ExtinguishPoint            0x539450

#define ARRAY_CFire                     (VAR_CFireCount + 4)
#define CLASS_CFireManager              0xB71F80

#define DEFAULT_FIRE_PARTICLE_SIZE      1.8
#define MAX_FIRES                       60

class CFireManagerSA : public CFireManager
{
private:
    CFireSA* Fires[MAX_FIRES];

public:
    CFireManagerSA();
    ~CFireManagerSA();

    void   ExtinguishPoint(CVector& vecPosition, float fRadius);
    CFire* StartFire(CEntity* entityTarget, CEntity* entityCreator, float fSize);
    CFire* StartFire(CVector& vecPosition, float fSize);
    void   ExtinguishAllFires();
    CFire* GetFire(DWORD ID);
    DWORD  GetFireCount();
    CFire* FindFreeFire();
    CFire* GetFire(CFireSAInterface* fire);
};
