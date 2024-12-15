/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileInfoSA.cpp
 *  PURPOSE:     Projectile type information
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CProjectileInfoSA.h"
#include "CVehicleSA.h"
#include "CWorldSA.h"

extern CGameSA* pGame;

CProjectile* CProjectileInfoSA::GetProjectile(void* projectilePointer)
{
    // This must be destroyed later
    return new CProjectileSA((CProjectileSAInterface*)projectilePointer);
}

CProjectileInfo* CProjectileInfoSA::GetProjectileInfo(void* projectileInfoInterface)
{
    return projectileInfo[((DWORD)projectileInfoInterface - ARRAY_CProjectileInfo) / sizeof(CProjectileInfoSAInterface)];
}

void CProjectileInfoSA::RemoveProjectile(CProjectileInfo* pProjectileInfo, CProjectile* pProjectile, bool bBlow)
{
    CProjectileInfoSAInterface* projectileInfoInterface = ((CProjectileInfoSA*)pProjectileInfo)->internalInterface;

    CProjectileSA* pProjectileSA = dynamic_cast<CProjectileSA*>(pProjectile);
    if (!pProjectileSA)
        return;

    CEntitySAInterface* projectileInterface = pProjectileSA->GetInterface();

    // Check that this infact is a CProjectile
    // This is perhaps the fix for a crash where it jumps to 0x42480000
    // The proper cause should be figured out instead though as this is a rather unsafe fix.
    if ((DWORD)projectileInterface->vtbl == VTBL_CProjectile)
    {
        // Has it not already been removed by GTA?
        if (pProjectileInfo->IsActive())
        {
            if (bBlow)
            {
                DWORD dwFunc = FUNC_RemoveProjectile;
                _asm
                {
                    push    projectileInterface
                    push    projectileInfoInterface
                    call    dwFunc
                    add     esp, 8
                }
            }
            else
            {
                DWORD dwFunc = FUNC_RemoveIfThisIsAProjectile;
                _asm
                {
                    push   projectileInterface
                    call   dwFunc
                    add    esp, 4
                }
            }
        }
    }
}

CProjectileInfo* CProjectileInfoSA::GetProjectileInfo(DWORD dwIndex)
{
    return projectileInfo[dwIndex];
}

/**
 * Creates a projectile based on an entity's position at a specified offset from the entity's position (VC)
 * @param creator The entity that is emmitting the projectile
 * @param eWeapon The type of projectile
 * @param vecOffset How far away from the creator entity the projectile is created
 * @param fForce How fast the projectile is moving when it is created
 * \note The projectile is created at the same angle as the creator entity. If the creator entity is the player
 * and the camera is in standard 3rd person mode, then the camera's rotation is used as the basis for the angle.
 * SA: public: static bool __cdecl CProjectileInfo::AddProjectile(class CEntity *,enum eWeaponType,class CVector,float,class CVector *,class CEntity *)
 */

bool CProjectileInfoSA::AddProjectile(CEntity* creator, eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector* target, CEntity* targetEntity)
{
    DWORD               dwFunction = FUNC_AddProjectile;
    DWORD               dwReturn = 0;
    CEntitySAInterface* creatorVC = nullptr;
    if (creator != nullptr)
    {
        CEntitySA* pCreatorSA = dynamic_cast<CEntitySA*>(creator);
        if (pCreatorSA)
        {
            creatorVC = pCreatorSA->GetInterface();
            pGame->GetWorld()->IgnoreEntity(creator);
        }
    }

    CEntitySAInterface* targetVC = nullptr;

    if (targetEntity != nullptr)
    {
        CEntitySA* pTargetEntitySA = dynamic_cast<CEntitySA*>(targetEntity);
        if (pTargetEntitySA)
            targetVC = pTargetEntitySA->GetInterface();
    }

    _asm
    {
        push    eax

        push    targetVC
        push    target
        push    fForce
        lea     eax, vecOrigin
        push    [eax+8]
        push    [eax+4]
        push    [eax]
        push    eWeapon
        push    creatorVC
        call    dwFunction
        add     esp, 32
        mov     dwReturn, eax

        pop     eax
    }
    pGame->GetWorld()->IgnoreEntity(nullptr);
    return dwReturn != 0;
}

CEntity* CProjectileInfoSA::GetTarget()
{
    CEntitySAInterface* pTargetInterface = internalInterface->pEntProjectileTarget;
    CEntity*            pTarget = nullptr;
    if (pTargetInterface)
    {
        switch (pTargetInterface->nType)
        {
            case ENTITY_TYPE_PED:
            {
                SClientEntity<CPedSA>* pPedClientEntity = pGame->GetPools()->GetPed((DWORD*)pTargetInterface);
                pTarget = pPedClientEntity ? pPedClientEntity->pEntity : nullptr;
                break;
            }
            case ENTITY_TYPE_VEHICLE:
            {
                SClientEntity<CVehicleSA>* pVehicleClientEntity = pGame->GetPools()->GetVehicle((DWORD*)pTargetInterface);
                pTarget = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
                break;
            }
        }
    }
    return pTarget;
}

void CProjectileInfoSA::SetTarget(CEntity* pEntity)
{
    CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(pEntity);
    if (pEntitySA)
        internalInterface->pEntProjectileTarget = pEntitySA->GetInterface();
}

bool CProjectileInfoSA::IsActive()
{
    return (internalInterface->bProjectileActive == 1 && internalInterface->dwProjectileType != 0);
}

void CProjectileInfoSA::SetCounter(DWORD dwCounter)
{
    internalInterface->dwCounter = dwCounter + pGame->GetSystemTime();
}

DWORD CProjectileInfoSA::GetCounter()
{
    return internalInterface->dwCounter - pGame->GetSystemTime();
}

void CProjectileInfoSA::RemoveEntityReferences(CEntity* entity)
{
    const CEntitySAInterface* entityInterface = entity->GetInterface();
    for (int i = 0; i < PROJECTILE_INFO_COUNT; i++)
    {
        auto projectileInterface = projectileInfo[i]->internalInterface;

        if (projectileInterface->pEntProjectileOwner == entityInterface)
            projectileInterface->pEntProjectileOwner = nullptr;

        if (projectileInterface->pEntProjectileTarget == entityInterface)
            projectileInterface->pEntProjectileTarget = nullptr;
    }
}
