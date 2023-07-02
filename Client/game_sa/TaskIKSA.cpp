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
#include "CEntitySA.h"
#include "CGameSA.h"
#include "CTaskManagementSystemSA.h"
#include "TaskIKSA.h"

extern CGameSA* pGame;

CTaskSimpleIKChainSA::CTaskSimpleIKChainSA(char* idString, int effectorBoneTag, CVector effectorVec, int pivotBoneTag, CEntity* pEntity, int offsetBoneTag,
                                           CVector offsetPos, float speed, int time, int blendTime)
{
    DWORD dwFunc = FUNC_CTaskSimpleIKChain__Constructor;
    // TODO: Find out the real size
    CreateTaskInterface(1024);
    if (!IsValid())
        return;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwEntityInterface = 0;
    if (pEntity)
        dwEntityInterface = (DWORD)pEntity->GetInterface();
    float fEffectorX = effectorVec.fX, fEffectorY = effectorVec.fY, fEffectorZ = effectorVec.fZ;
    float fX = offsetPos.fX, fY = offsetPos.fY, fZ = offsetPos.fZ;
    _asm
    {
        mov     ecx, dwThisInterface
        push    blendTime
        push    time
        push    speed
        push    fZ
        push    fY
        push    fX
        push    offsetBoneTag
        push    dwEntityInterface
        push    pivotBoneTag
        push    fEffectorZ
        push    fEffectorY
        push    fEffectorX
        push    effectorBoneTag
        push    idString
        call    dwFunc
    }
}

CTaskSimpleIKLookAtSA::CTaskSimpleIKLookAtSA(char* idString, CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso,
                                             float speed, int blendTime, int m_priority)
{
    DWORD dwFunc = FUNC_CTaskSimpleIKLookAt__Constructor;
    // TODO: Find out the real size
    CreateTaskInterface(1024);
    if (!IsValid())
        return;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwEntityInterface = 0;
    if (pEntity)
        dwEntityInterface = (DWORD)pEntity->GetInterface();
    float fX = offsetPos.fX, fY = offsetPos.fY, fZ = offsetPos.fZ;
    _asm
    {
        mov     ecx, dwThisInterface
        push    m_priority
        push    blendTime
        push    speed
        push    useTorso
        push    fZ
        push    fY
        push    fX
        push    offsetBoneTag
        push    time
        push    dwEntityInterface
        push    idString
        call    dwFunc
    }
}

CTaskSimpleIKManagerSA::CTaskSimpleIKManagerSA()
{
    m_pTaskManagementSystem = pGame->GetTaskManagementSystem();
}

int CTaskSimpleIKManagerSA::AddIKChainTask(CTaskSimpleIKChain* pIKChainTask, int slotID)
{
    CTaskSimpleIKManagerSAInterface* pInterface = (CTaskSimpleIKManagerSAInterface*)GetInterface();
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
    CTaskSimpleIKManagerSAInterface* pInterface = (CTaskSimpleIKManagerSAInterface*)GetInterface();
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
    CTaskSimpleIKManagerSAInterface* pInterface = (CTaskSimpleIKManagerSAInterface*)GetInterface();
    return (!pInterface->m_pIKChainTasks[slotID]);
}

CTaskSimpleIKChain* CTaskSimpleIKManagerSA::GetTaskAtSlot(int slotID)
{
    CTaskSimpleIKManagerSAInterface* pInterface = (CTaskSimpleIKManagerSAInterface*)GetInterface();
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
    DWORD dwFunc = FUNC_CTaskSimpleTriggerLookAt__Constructor;
    // TODO: Find out the real size
    CreateTaskInterface(1024);
    if (!IsValid())
        return;
    DWORD dwThisInterface = (DWORD)GetInterface();
    DWORD dwEntityInterface = 0;
    if (pEntity)
        dwEntityInterface = (DWORD)pEntity->GetInterface();
    float fX = offsetPos.fX, fY = offsetPos.fY, fZ = offsetPos.fZ;
    _asm
    {
        mov     ecx, dwThisInterface
        push    priority
        push    blendTime
        push    speed
        push    useTorso
        push    fZ
        push    fY
        push    fX
        push    offsetBoneTag
        push    time
        push    dwEntityInterface
        call    dwFunc
    }
}
