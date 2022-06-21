/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskIKSA.cpp
 *  PURPOSE:     Inverse kinematics game tasks
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

extern CGameSA* pGame;

CTaskSimpleIKChainSA::CTaskSimpleIKChainSA(char* idString, int effectorBoneTag, CVector effectorVec, int pivotBoneTag, CEntity* pEntity, int offsetBoneTag,
                                           CVector offsetPos, float speed, int time, int blendTime)
{
    // TODO: Find out the real size
    this->CreateTaskInterface(1024);
    if (!IsValid())
        return;

    CEntitySAInterface* pEntityInterface = pEntity ? pEntity->GetInterface() : nullptr;

    // CTaskSimpleIKChain::CTaskSimpleIKChain
    ((void*(__thiscall*)(CTaskSAInterface*, char*, int, CVector, int, CEntitySAInterface*, int, CVector, float, int, int))FUNC_CTaskSimpleIKChain__Constructor)(
        this->GetInterface(), idString, effectorBoneTag, effectorVec, pivotBoneTag, pEntityInterface, offsetBoneTag, offsetPos, speed, time, blendTime);
}

CTaskSimpleIKLookAtSA::CTaskSimpleIKLookAtSA(char* idString, CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso,
                                             float speed, int blendTime, int m_priority)
{
    // TODO: Find out the real size
    this->CreateTaskInterface(1024);
    if (!IsValid())
        return;

    CEntitySAInterface* pEntityInterface = pEntity ? pEntity->GetInterface() : nullptr;

    // CTaskSimpleIKLookAt::CTaskSimpleIKLookAt
    ((void*(__thiscall*)(CTaskSAInterface*, char*, CEntitySAInterface*, int, int, CVector, unsigned char, float, int,
                         int))FUNC_CTaskSimpleIKLookAt__Constructor)(this->GetInterface(), idString, pEntityInterface, time, offsetBoneTag, offsetPos, useTorso,
                                                                     speed, blendTime, m_priority);
}

CTaskSimpleIKManagerSA::CTaskSimpleIKManagerSA()
{
    this->m_pTaskManagementSystem = (CTaskManagementSystemSA*)(pGame->GetTaskManagementSystem());
}

int CTaskSimpleIKManagerSA::AddIKChainTask(CTaskSimpleIKChain* pIKChainTask, int slotID)
{
    CTaskSimpleIKManagerSAInterface* pInterface = (CTaskSimpleIKManagerSAInterface*)this->GetInterface();
    if (!pInterface->m_pIKChainTasks[slotID])
    {
        pInterface->m_pIKChainTasks[slotID] = (CTaskSimpleIKChainSAInterface*)(pIKChainTask->GetInterface());
        return slotID;
    }
    for (int i = 0; i < slotID; i++)
    {
        if (!pInterface->m_pIKChainTasks[i])
        {
            pInterface->m_pIKChainTasks[i] = (CTaskSimpleIKChainSAInterface*)(pIKChainTask->GetInterface());
            return i;
        }
    }
    return -1;
}

void CTaskSimpleIKManagerSA::RemoveIKChainTask(int slotID)
{
    CTaskSimpleIKManagerSAInterface* pInterface = (CTaskSimpleIKManagerSAInterface*)this->GetInterface();
    if (pInterface->m_pIKChainTasks[slotID])
    {
        // Grab the task
        CTask* pTask = m_pTaskManagementSystem->GetTask(pInterface->m_pIKChainTasks[slotID]);
        assert(pTask);            // Leave this here temporarily for debugging (shouldn't ever be null)
        pTask->Destroy();
        pInterface->m_pIKChainTasks[slotID] = NULL;
    }
}

void CTaskSimpleIKManagerSA::BlendOut(int slotID, int blendOutTime)
{
    // TODO: fill me
}

unsigned char CTaskSimpleIKManagerSA::IsSlotEmpty(int slotID)
{
    CTaskSimpleIKManagerSAInterface* pInterface = (CTaskSimpleIKManagerSAInterface*)this->GetInterface();
    return (!pInterface->m_pIKChainTasks[slotID]);
}

CTaskSimpleIKChain* CTaskSimpleIKManagerSA::GetTaskAtSlot(int slotID)
{
    CTaskSimpleIKManagerSAInterface* pInterface = (CTaskSimpleIKManagerSAInterface*)this->GetInterface();
    if (pInterface->m_pIKChainTasks[slotID])
    {
        // Grab the task
        CTask* pTask = m_pTaskManagementSystem->GetTask(pInterface->m_pIKChainTasks[slotID]);
        assert(pTask);            // Leave this here temporarily for debugging (shouldn't ever be null)
        return dynamic_cast<CTaskSimpleIKChain*>(pTask);
    }
    return NULL;
}

CTaskSimpleTriggerLookAtSA::CTaskSimpleTriggerLookAtSA(CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed,
                                                       int blendTime, int priority)
{
    // TODO: Find out the real size
    this->CreateTaskInterface(1024);
    if (!IsValid())
        return;

    CEntitySAInterface* pEntityInterface = pEntity ? pEntity->GetInterface() : nullptr;

    // CTaskSimpleTriggerLookAt::CTaskSimpleTriggerLookAt
    ((void*(__thiscall*)(CTaskSAInterface*, CEntitySAInterface*, int, int, CVector, unsigned char, float, int, int))FUNC_CTaskSimpleTriggerLookAt__Constructor)(
        this->GetInterface(), pEntityInterface, time, offsetBoneTag, offsetPos, useTorso, speed, blendTime, priority);
}
