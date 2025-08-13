/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CExplosionManagerSA.cpp
 *  PURPOSE:     Explosion manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CExplosionManagerSA.h"
#include "CExplosionSA.h"
#include "CEntitySA.h"

CExplosionManagerSA::CExplosionManagerSA()
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
        Explosions[i] = new CExplosionSA((CExplosionSAInterface*)(ARRAY_Explosions + i * sizeof(CExplosionSAInterface)));
}

CExplosionManagerSA::~CExplosionManagerSA()
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        delete Explosions[i];
    }
}

/**
 * \todo Test this, replace with CExplosion::AddExplosion code if possible in order to ensure correct pointer
 */
CExplosion* CExplosionManagerSA::AddExplosion(CEntity* pExplodingEntity, CEntity* pOwner, eExplosionType explosionType, CVector& vecPosition,
                                              unsigned int uiActivationDelay, bool bMakeSound, float fCamShake, bool bNoDamage)
{
    DWORD       dwExplodingEntityInterface = (pExplodingEntity) ? (DWORD)pExplodingEntity->GetInterface() : 0;
    DWORD       dwOwnerInterface = (pOwner) ? (DWORD)pOwner->GetInterface() : 0;
    float       fX = vecPosition.fX, fY = vecPosition.fY, fZ = vecPosition.fZ;
    CExplosion* explosion = CExplosionManagerSA::FindFreeExplosion();
    bool        bReturn;
    DWORD       dwFunc = FUNC_CExplosion_AddExplosion;
    _asm
    {
        push    bNoDamage
        push    fCamShake
        push    bMakeSound
        push    uiActivationDelay
        push    fZ
        push    fY
        push    fX
        push    explosionType
        push    dwOwnerInterface
        push    dwExplodingEntityInterface

        // OUR CALL
        push    returnhere // simulate a call, by pusing our return address
        // NOW the code is actually inside CExplosion__AddExplosion, but may be patched by Multiplayer
        sub     esp, 0x1C
        push    ebx
        push    ebp
        push    esi
        mov     ebx, dwFunc // Now jump in 6 bytes later (6 bytes might be used for our patch-jump in Multiplayer)
        add     ebx, 6
        jmp     ebx

returnhere:
        add     esp, 0x28
        mov     bReturn, al
    }
    if (bReturn) return explosion;

    return NULL;
}

void CExplosionManagerSA::RemoveAllExplosions()
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
        if (Explosions[i]->IsActive())
            Explosions[i]->Remove();
}

CExplosion* CExplosionManagerSA::GetExplosion(DWORD ID)
{
    return Explosions[ID];
}

CExplosion* CExplosionManagerSA::FindFreeExplosion()
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
        if (!Explosions[i]->IsActive())
            return Explosions[i];
    return NULL;
}
