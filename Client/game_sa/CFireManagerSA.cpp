/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFireManagerSA.cpp
 *  PURPOSE:     Fire manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CFireManagerSA.h"
#include "CFireSA.h"
#include "CGameSA.h"

extern CGameSA* pGame;

CFireManagerSA::CFireManagerSA()
{
    for (int i = 0; i < MAX_FIRES; i++)
        Fires[i] = new CFireSA(
            (CFireSAInterface*)(CLASS_CFireManager + 40 * i));            // + 4 because thats the position of CFire array in CFireManager (see source)
}

CFireManagerSA::~CFireManagerSA()
{
    for (int i = 0; i < MAX_FIRES; i++)
    {
        delete Fires[i];
    }
}

void CFireManagerSA::ExtinguishPoint(CVector& vecPosition, float fRadius)
{
    float fX = vecPosition.fX;
    float fY = vecPosition.fY;
    float fZ = vecPosition.fZ;
    DWORD dwFunction = FUNC_ExtinguishPoint;

    _asm
    {
        mov     ecx, CLASS_CFireManager
        push    fRadius
        push    fZ
        push    fY
        push    fX
        call    dwFunction
    }
}

CFire* CFireManagerSA::StartFire(CEntity* entityTarget, CEntity* entityCreator, float fSize = DEFAULT_FIRE_PARTICLE_SIZE)
{
    CFire* fire = FindFreeFire();

    if (fire != NULL)
    {
        fire->SetTarget(entityTarget);
        fire->SetStrength(fSize);
        fire->SetTimeToBurnOut(pGame->GetSystemTime() + 5000);
        fire->SetSilent(false);
        fire->Ignite();
    }

    return fire;
}

CFire* CFireManagerSA::StartFire(CVector& vecPosition, float fSize = DEFAULT_FIRE_PARTICLE_SIZE)
{
    CFire* fire = FindFreeFire();

    if (fire != NULL)
    {
        fire->SetPosition(vecPosition);
        fire->SetStrength(fSize);
        fire->SetTimeToBurnOut(pGame->GetSystemTime() + 5000);
        fire->SetSilent(false);
        fire->Ignite();
    }

    return fire;
}

void CFireManagerSA::ExtinguishAllFires()
{
    CFireSA* fire;
    for (int i = 0; i < MAX_FIRES; i++)
    {
        fire = (CFireSA*)GetFire(i);
        if (fire && fire->IsIgnited())
            fire->Extinguish();
    }
}

CFire* CFireManagerSA::GetFire(DWORD ID)
{
    if (ID < MAX_FIRES)
        return Fires[ID];
    else
        return NULL;
}

DWORD CFireManagerSA::GetFireCount()
{
    return *(DWORD*)CLASS_CFireManager;
}

CFire* CFireManagerSA::FindFreeFire()
{
    CFireSA* fire;
    for (int i = 0; i < MAX_FIRES; i++)
    {
        fire = (CFireSA*)GetFire(i);
        if (fire && !fire->IsIgnited())
            return fire;
    }
    return NULL;
}

CFire* CFireManagerSA::GetFire(CFireSAInterface* fire)
{
    DWORD dwID = ((DWORD)fire - CLASS_CFireManager + 4) / sizeof(CFireSAInterface);
    return GetFire(dwID);
}
