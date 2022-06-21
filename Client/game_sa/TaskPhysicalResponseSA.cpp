/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskPhysicalResponseSA.cpp
 *  PURPOSE:     Physical response game tasks
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// ##############################################################################
// ## Name:    CTaskSimpleChoking
// ## Purpose: Make the ped choke
// ##############################################################################

CTaskSimpleChokingSA::CTaskSimpleChokingSA(CPed* pAttacker, bool bIsTearGas)
{
    CPedSA* pAttackerSA = dynamic_cast<CPedSA*>(pAttacker);

    DWORD dwFunc = FUNC_CTaskSimpleChoking__Constructor;
    DWORD dwIsTearGas = bIsTearGas;

    // Grab the GTA class for the attacker if any
    CPedSAInterface* pAttackerInterface = NULL;
    if (pAttackerSA)
        pAttackerInterface = pAttackerSA->GetPedInterface();

    this->CreateTaskInterface(sizeof(CTaskSimpleChokingSAInterface));
    if (!IsValid())
        return;

    // CTaskSimpleChoking::CTaskSimpleChoking
    ((void*(__thiscall*)(void*, CPedSAInterface*, unsigned char))FUNC_CTaskSimpleChoking__Constructor)(
        this->GetInterface(), pAttackerInterface, bIsTearGas);
}

CPed* CTaskSimpleChokingSA::GetAttacker()
{
    CTaskSimpleChokingSAInterface* internalInterface = (CTaskSimpleChokingSAInterface*)this->GetInterface();
    SClientEntity<CPedSA>*         pPedClientEntity = pGame->GetPools()->GetPed((DWORD*)internalInterface->m_pAttacker);
    return pPedClientEntity ? pPedClientEntity->pEntity : nullptr;
}

unsigned int CTaskSimpleChokingSA::GetTimeRemaining()
{
    CTaskSimpleChokingSAInterface* internalInterface = (CTaskSimpleChokingSAInterface*)this->GetInterface();
    return internalInterface->m_nTimeRemaining;
}

unsigned int CTaskSimpleChokingSA::GetTimeStarted()
{
    CTaskSimpleChokingSAInterface* internalInterface = (CTaskSimpleChokingSAInterface*)this->GetInterface();
    return internalInterface->m_nTimeStarted;
}

bool CTaskSimpleChokingSA::IsTeargas()
{
    CTaskSimpleChokingSAInterface* internalInterface = (CTaskSimpleChokingSAInterface*)this->GetInterface();
    return internalInterface->m_bIsTeargas ? true : false;
}

bool CTaskSimpleChokingSA::IsFinished()
{
    CTaskSimpleChokingSAInterface* internalInterface = (CTaskSimpleChokingSAInterface*)this->GetInterface();
    return internalInterface->m_bIsFinished;
}

void CTaskSimpleChokingSA::UpdateChoke(CPed* pPed, CPed* pAttacker, bool bIsTearGas)
{
    // Get game interfaces
    CPedSA* pPedSA = dynamic_cast<CPedSA*>(pPed);
    if (!pPedSA)
        return;

    CPedSAInterface* pPedInterface = pPedSA->GetPedInterface();

    CPedSAInterface* pAttackerInterface = NULL;
    if (pAttacker)
    {
        CPedSA* pAttackerSA = dynamic_cast<CPedSA*>(pAttacker);
        if (pAttackerSA)
            pAttackerInterface = pAttackerSA->GetPedInterface();
    }

    // CTaskSimpleChoking::UpdateChoke
    ((void(__thiscall*)(void*, CPedSAInterface*, CPedSAInterface*, unsigned char))FUNC_CTaskSimpleChoking__UpdateChoke)(
        this->GetInterface(), pPedInterface, pAttackerInterface, bIsTearGas);
}
