/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFireSA.cpp
 *  PURPOSE:     Fire
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEntitySA.h"
#include "CFireSA.h"
#include "CGameSA.h"
#include "CPoolsSA.h"
#include <game/CTaskManager.h>
#include <game/TaskTypes.h>

extern CGameSA* pGame;

/**
 * Put the fire out
 */
void CFireSA::Extinguish()
{
    DWORD dwFunction = FUNC_Extinguish;
    DWORD dwPointer = (DWORD)internalInterface;
    _asm
    {
        mov     ecx, dwPointer
        call    dwFunction
    }
    internalInterface->bActive = false;
}

/**
 * Gets the position the fire is burning at
 * @return CVector * containing the fire's position
 */
CVector* CFireSA::GetPosition()
{
    return &internalInterface->vecPosition;
}

/**
 * Set the position the fire is burning at. This won't have any effect if this fire has a target set
 * @param vecPosition CVector * containing the desired position for the fire.
 * @see CFireSA::SetTarget
 */
void CFireSA::SetPosition(CVector& vecPosition)
{
    internalInterface->entityTarget = 0;
    MemCpyFast(&internalInterface->vecPosition, &vecPosition, sizeof(CVector));
}

/**
 * Set the time that the fire will be extinguished.
 * @param dwTime DWORD containing the time that the fire will be extinguished.  This is in game-time units which can be got from CGame::GetSystemTime;
 */
void CFireSA::SetTimeToBurnOut(DWORD dwTime)
{
    internalInterface->nTimeToBurn = dwTime;
}

DWORD CFireSA::GetTimeToBurnOut()
{
    return internalInterface->nTimeToBurn;
}

CEntity* CFireSA::GetCreator()
{
    CEntity*            creatorEntity = NULL;
    CEntitySAInterface* createEntitySA = internalInterface->entityCreator;
    CPoolsSA*           pPools = ((CPoolsSA*)pGame->GetPools());
    if (pPools && createEntitySA)
    {
        switch (createEntitySA->nType)
        {
            case ENTITY_TYPE_PED:
            {
                SClientEntity<CPedSA>* pPedClientEntity = pPools->GetPed((DWORD*)createEntitySA);
                if (pPedClientEntity)
                {
                    creatorEntity = pPedClientEntity->pEntity;
                }
                break;
            }
            case ENTITY_TYPE_VEHICLE:
            {
                SClientEntity<CVehicleSA>* pVehicleClientEntity = pPools->GetVehicle((DWORD*)createEntitySA);
                if (pVehicleClientEntity)
                {
                    creatorEntity = pVehicleClientEntity->pEntity;
                }
                break;
            }
            default:
            {
                creatorEntity = NULL;
            }
        }
    }
    return creatorEntity;
}

CEntity* CFireSA::GetEntityOnFire()
{
    CEntity*            TargetEntity = NULL;
    CEntitySAInterface* TargetEntitySA = internalInterface->entityTarget;
    CPoolsSA*           pPools = ((CPoolsSA*)pGame->GetPools());
    if (pPools && TargetEntitySA)
    {
        switch (TargetEntitySA->nType)
        {
            case ENTITY_TYPE_PED:
            {
                SClientEntity<CPedSA>* pPedClientEntity = pPools->GetPed((DWORD*)TargetEntitySA);
                if (pPedClientEntity)
                {
                    TargetEntity = pPedClientEntity->pEntity;
                }
                break;
            }
            case ENTITY_TYPE_VEHICLE:
            {
                SClientEntity<CVehicleSA>* pVehicleClientEntity = pPools->GetVehicle((DWORD*)TargetEntitySA);
                if (pVehicleClientEntity)
                {
                    TargetEntity = pVehicleClientEntity->pEntity;
                }
                break;
            }
            default:
            {
                TargetEntity = NULL;
            }
        }
    }
    return TargetEntity;
}

void CFireSA::SetTarget(CEntity* entity)
{
    if (entity)
    {
        CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(entity);
        if (pEntitySA)
            internalInterface->entityTarget = pEntitySA->GetInterface();
    }
    else
    {
        internalInterface->entityTarget = NULL;
    }
}

bool CFireSA::IsIgnited()
{
    return internalInterface->bActive;
}

bool CFireSA::IsFree()
{
    if (!internalInterface->bActive && !internalInterface->bCreatedByScript)
        return true;
    else
        return false;
}

void CFireSA::SetSilent(bool bSilent)
{
    internalInterface->bMakesNoise = !bSilent;
}

bool CFireSA::IsBeingExtinguished()
{
    return internalInterface->bBeingExtinguished;
}

void CFireSA::Ignite()
{
    internalInterface->bActive = true;

    CVector* vecPosition = GetPosition();
    DWORD    dwFunc = FUNC_CreateFxSysForStrength;
    DWORD    dwThis = (DWORD)internalInterface;
    _asm
    {
        mov     ecx, dwThis
        push    0
        push    vecPosition
        call    dwFunc
    }

    internalInterface->bBeingExtinguished = 0;
    internalInterface->bFirstGeneration = 1;
    internalInterface->nNumGenerationsAllowed = 100;
}

float CFireSA::GetStrength()
{
    return internalInterface->Strength;
}

void CFireSA::SetStrength(float fStrength)
{
    internalInterface->Strength = fStrength;
}

void CFireSA::SetNumGenerationsAllowed(char generations)
{
    internalInterface->nNumGenerationsAllowed = generations;
}

////////////////////////////////////////////////////////////////////////
// CFire::Extinguish
//
// Fix GH #3249 (PLAYER_ON_FIRE task is not aborted after the fire is extinguished)
////////////////////////////////////////////////////////////////////////
static void AbortFireTask(CEntitySAInterface* entityOnFire, DWORD returnAddress)
{
    // We can't and shouldn't remove the task if we're in CTaskSimplePlayerOnFire::ProcessPed. Otherwise we will crash.
    if (returnAddress == 0x633783)
        return;

    auto ped = pGame->GetPools()->GetPed(reinterpret_cast<DWORD*>(entityOnFire));
    if (!ped || !ped->pEntity)
        return;

    CTaskManager* taskManager = ped->pEntity->GetPedIntelligence()->GetTaskManager();
    if (!taskManager)
        return;

    taskManager->RemoveTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM, TASK_SIMPLE_PLAYER_ON_FIRE);
}

#define HOOKPOS_CFire_Extinguish 0x539429
#define HOOKSIZE_CFire_Extinguish 6
static constexpr std::uintptr_t CONTINUE_CFire_Extinguish = 0x53942F;
static void _declspec(naked) HOOK_CFire_Extinguish()
{
    _asm
    {
        mov [eax+730h], edi
        mov ebx, [esp+8]

        push ebx
        push eax
        call AbortFireTask
        add esp, 8

        jmp CONTINUE_CFire_Extinguish
    }
}

void CFireSA::StaticSetHooks()
{
    EZHookInstall(CFire_Extinguish);
}
