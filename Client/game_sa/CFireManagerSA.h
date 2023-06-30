/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CFireManagerSA.h
 *  PURPOSE:     Header file for fire manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CFireManager.h>

class CEntity;
class CFireSA;
class CFireSAInterface;

#define CLASS_CFireManager              0xB71F80 // multiplayer_sa/multiplayer_shotsync.cpp
#define DEFAULT_FIRE_PARTICLE_SIZE      1.8 // multiplayer_sa/multiplayer_shotsync.cpp
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
