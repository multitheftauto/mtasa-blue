/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTaskManagementSystemSA.cpp
 *  PURPOSE:     Task management system
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CTaskManagementSystemSA.h"
#include "TaskAttackSA.h"
#include "TaskBasicSA.h"
#include "TaskCarAccessoriesSA.h"
#include "TaskCarSA.h"
#include "TaskGoToSA.h"
#include "TaskIKSA.h"
#include "TaskJumpFallSA.h"
#include "TaskPhysicalResponseSA.h"
#include "TaskSA.h"
#include "TaskSecondarySA.h"

extern CGameSA* pGame;

using namespace std;

void HOOK_CTask_Operator_Delete();

CTaskSAInterface* pTempTaskInterface = 0;

CTaskManagementSystemSA::CTaskManagementSystemSA()
{
    // Install our hook used to delete our tasks when GTA does
    HookInstall(FUNC_CTask_Operator_Delete, (DWORD)HOOK_CTask_Operator_Delete, 6);
}

CTaskManagementSystemSA::~CTaskManagementSystemSA()
{
    // Delete all the entries
    list<STaskListItem*>::const_iterator iter = m_TaskList.begin();
    for (; iter != m_TaskList.end(); iter++)
    {
        delete *iter;
    }

    // Clear the list
    m_TaskList.clear();
}

CTaskSA* CTaskManagementSystemSA::AddTask(CTaskSA* pTask)
{
    if (!pTask)
        return NULL;

    assert(pTask->IsValid());
    STaskListItem* pItem = new STaskListItem;
    pItem->pTaskSA = pTask;
    pItem->taskInterface = pTask->GetInterface();
    m_TaskList.push_back(pItem);

    return pTask;
}

// Only called from HOOK_CTask_Operator_Delete
void CTaskManagementSystemSA::RemoveTask(CTaskSAInterface* pTaskInterface)
{
    // Stops crash on exit
    if (m_TaskList.size() == 0)
        return;

    assert(pTaskInterface);

    // Find it in our list
    STaskListItem*                 pItem;
    list<STaskListItem*>::iterator iter = m_TaskList.begin();
    for (; iter != m_TaskList.end(); iter++)
    {
        pItem = *iter;
        if (pItem->taskInterface == pTaskInterface)
        {
            // Grab the task SA
            CTaskSA* pTaskSA = pItem->pTaskSA;

            // Delete the object
            delete pItem;

            // Remove from list
            m_TaskList.erase(iter);

            // Eventually destroy the task
            if (pTaskSA)
                pTaskSA->DestroyJustThis();

            // Done (don't continue or we waste resources/crash)
            return;
        }
    }
}

CTaskSA* CTaskManagementSystemSA::GetTask(CTaskSAInterface* pTaskInterface)
{
    // Return NULL if we got passed NULL
    if (pTaskInterface == 0)
        return NULL;

    // Find it in our list
    STaskListItem*                       pListItem;
    list<STaskListItem*>::const_iterator iter = m_TaskList.begin();
    for (; iter != m_TaskList.end(); iter++)
    {
        // Matches?
        pListItem = *iter;
        if (pListItem->taskInterface == pTaskInterface && pListItem->pTaskSA)
        {
            // Yes it has existed before, return it
            return pListItem->pTaskSA;
        }
    }

    // its not existed before, lets create the task
    // First, we create a temp task
    int   iTaskType = 9999;
    DWORD dwFunc = pTaskInterface->VTBL->GetTaskType;
    if (dwFunc && dwFunc != 0x82263A)
    {
        _asm
        {
            mov     ecx, pTaskInterface
            call    dwFunc
            mov     iTaskType, eax
        }
    }

    // Create it and add it to our list
    CTaskSA* pTask = dynamic_cast<CTaskSA*>(CreateAppropriateTask(pTaskInterface, iTaskType));
    if (pTask)
    {
        pListItem = new STaskListItem;
        pListItem->pTaskSA = pTask;
        pListItem->taskInterface = pTaskInterface;
        m_TaskList.push_back(pListItem);
    }

    return pTask;
}

CTaskSA* CTaskManagementSystemSA::CreateAppropriateTask(CTaskSAInterface* pTaskInterface, int iTaskType)
{
    CTaskSA* pTaskSA = NULL;

    switch (iTaskType)
    {
        // Attack
        case TASK_SIMPLE_GANG_DRIVEBY:
            pTaskSA = new CTaskSimpleGangDriveBySA;
            break;
        case TASK_SIMPLE_USE_GUN:
            pTaskSA = new CTaskSimpleUseGunSA;
            break;
        case TASK_SIMPLE_FIGHT:
            pTaskSA = new CTaskSimpleFightSA;
            break;

        // Basic
        case TASK_COMPLEX_USE_MOBILE_PHONE:
            pTaskSA = new CTaskComplexUseMobilePhoneSA;
            break;
        case TASK_SIMPLE_ANIM:
            pTaskSA = new CTaskSimpleRunAnimSA;
            break;
        case TASK_SIMPLE_NAMED_ANIM:
            pTaskSA = new CTaskSimpleRunNamedAnimSA;
            break;
        case TASK_COMPLEX_DIE:
            pTaskSA = new CTaskComplexDieSA;
            break;
        case TASK_SIMPLE_STEALTH_KILL:
            pTaskSA = new CTaskSimpleStealthKillSA;
            break;
        case TASK_COMPLEX_SUNBATHE:
            pTaskSA = new CTaskComplexSunbatheSA;
            break;

        // Car accessories
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
            pTaskSA = new CTaskSimpleCarSetPedInAsPassengerSA;
            break;
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
            pTaskSA = new CTaskSimpleCarSetPedInAsDriverSA;
            break;
        case TASK_SIMPLE_CAR_SET_PED_OUT:
            pTaskSA = new CTaskSimpleCarSetPedOutSA;
            break;

        // Car
        case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
            pTaskSA = new CTaskComplexEnterCarAsDriverSA;
            break;
        case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
            pTaskSA = new CTaskComplexEnterCarAsPassengerSA;
            break;
        case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
            pTaskSA = new CTaskComplexEnterBoatAsDriverSA;
            break;
        case TASK_COMPLEX_LEAVE_CAR:
            pTaskSA = new CTaskComplexLeaveCarSA;
            break;

        // GoTo
        case TASK_COMPLEX_WANDER:
            pTaskSA = new CTaskComplexWanderStandardSA;
            break;

        // IK
        case TASK_SIMPLE_IK_CHAIN:
            pTaskSA = new CTaskSimpleIKChainSA;
            break;
        case TASK_SIMPLE_IK_LOOK_AT:
            pTaskSA = new CTaskSimpleIKLookAtSA;
            break;
        case TASK_SIMPLE_IK_MANAGER:
            pTaskSA = new CTaskSimpleIKManagerSA;
            break;

        // JumpFall
        case TASK_SIMPLE_CLIMB:
            pTaskSA = new CTaskSimpleClimbSA;
            break;
        case TASK_SIMPLE_JETPACK:
            pTaskSA = new CTaskSimpleJetPackSA;
            break;

        // Physical response
        case TASK_SIMPLE_CHOKING:
            pTaskSA = new CTaskSimpleChokingSA;
            break;

        // Secondary
        case TASK_SIMPLE_DUCK:
            pTaskSA = new CTaskSimpleDuckSA;
            break;

        // Just create the baseclass
        default:
            pTaskSA = new CTaskSA;
            break;
    }

    assert(pTaskSA && !pTaskSA->GetInterface());

    // Set the internal interface
    pTaskSA->SetInterface(pTaskInterface);
    return pTaskSA;
}

// HOOKS
__declspec(noinline) void OnMY_Task_Operator_Delete(CTaskSAInterface* pTaskInterface)
{
    pGame->GetTaskManagementSystem()->RemoveTask(pTempTaskInterface);
}

void _declspec(naked) HOOK_CTask_Operator_Delete()
{
    _asm
        {
        mov     eax, [esp+4]
        mov     pTempTaskInterface, eax

        pushad
        }

    OnMY_Task_Operator_Delete(pTempTaskInterface);

    // Continue on our merry way....
    _asm
    {
        popad

        mov     eax, 0xB744A8
        mov     ecx, dword ptr [eax]
        mov     eax, FUNC_CTask_Operator_Delete
        add     eax, 6
        jmp     eax
    }
}
