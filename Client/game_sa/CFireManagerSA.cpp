/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CFireManagerSA.cpp
 *  PURPOSE:     Fire manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
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
    for (int i = 0; i < MAX_FIRES; i++) { delete Fires[i]; }
}

void CFireManagerSA::ExtinguishPoint(CVector& vecPosition, float fRadius)
{
    float fX = vecPosition.fX;
    float fY = vecPosition.fY;
    float fZ = vecPosition.fZ;
    DWORD dwFunction = 0x539450;
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
    CFire* pFire = FindFreeFire();
    if (pFire != nullptr)
    {
        pFire->SetTarget(entityTarget);
        pFire->SetStrength(fSize);
        pFire->SetTimeToBurnOut(pGame->GetSystemTime() + 5000);
        pFire->SetSilent(false);
        pFire->Ignite();
    }
    return pFire;
}

CFire* CFireManagerSA::StartFire(CVector& vecPosition, float fSize = DEFAULT_FIRE_PARTICLE_SIZE)
{
    CFire* pFire = FindFreeFire();
    if (pFire != nullptr)
    {
        pFire->SetPosition(vecPosition);
        pFire->SetStrength(fSize);
        pFire->SetTimeToBurnOut(pGame->GetSystemTime() + 5000);
        pFire->SetSilent(false);
        pFire->Ignite();
    }
    return pFire;
}

void CFireManagerSA::ExtinguishAllFires()
{
    CFireSA* pFireSA;
    for (int i = 0; i < MAX_FIRES; i++)
    {
        pFireSA = (CFireSA*)GetFire(i);
        if (pFireSA && pFireSA->IsIgnited())
            pFireSA->Extinguish();
    }
}

CFire* CFireManagerSA::GetFire(DWORD ID)
{
    if (ID < MAX_FIRES) { return Fires[ID]; }
    else { return nullptr; }
}

DWORD CFireManagerSA::GetFireCount()
{
    return *(DWORD*)CLASS_CFireManager;
}

CFire* CFireManagerSA::FindFreeFire()
{
    CFireSA* pFireSA;
    for (int i = 0; i < MAX_FIRES; i++)
    {
        pFireSA = (CFireSA*)GetFire(i);
        if (pFireSA && !pFireSA->IsIgnited())
            return pFireSA;
    }
    return nullptr;
}

CFire* CFireManagerSA::GetFire(CFireSAInterface* fire)
{
    return GetFire(((DWORD)fire - CLASS_CFireManager + 4) / sizeof(CFireSAInterface));
}
