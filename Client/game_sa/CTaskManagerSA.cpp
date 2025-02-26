/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTaskManagerSA.cpp
 *  PURPOSE:     Task manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CTaskManagementSystemSA.h"
#include "CTaskManagerSA.h"
#include "TaskSA.h"

extern CGameSA* pGame;

CTaskManagerSA::CTaskManagerSA(CTaskManagerSAInterface* taskManagerInterface, CPed* ped)
{
    this->ped = ped;
    internalInterface = taskManagerInterface;
    m_pTaskManagementSystem = pGame->GetTaskManagementSystem();
}

void CTaskManagerSA::RemoveTask(const int iTaskPriority)
{
    if (iTaskPriority != TASK_PRIORITY_DEFAULT)            // TASK_PRIORITY_DEFAULT removed = crash
    {
        SetTask(NULL, iTaskPriority);
    }
}

void CTaskManagerSA::SetTask(CTaskSA* pTaskPrimary, const int iTaskPriority, const bool bForceNewTask)
{
    DWORD             dwFunc = FUNC_SetTask;
    CTaskSAInterface* taskInterface = NULL;
    if (pTaskPrimary)
        taskInterface = pTaskPrimary->GetInterface();

    DWORD dwInterface = (DWORD)GetInterface();
    _asm
    {
        xor     eax, eax
        movzx   eax, bForceNewTask
        push    eax
        push    iTaskPriority
        push    taskInterface
        mov     ecx, dwInterface
        call    dwFunc
    }
}

CTask* CTaskManagerSA::GetTask(const int iTaskPriority)
{
    CTaskManagerSAInterface* pTaskManagerInterface = GetInterface();
    return m_pTaskManagementSystem->GetTask(pTaskManagerInterface->m_tasks[iTaskPriority]);
}

CTask* CTaskManagerSA::GetActiveTask()
{
    DWORD dwFunc = FUNC_GetActiveTask;
    DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax
    }

    CTaskSAInterface* pActiveTask = (CTaskSAInterface*)dwReturn;
    if (dwReturn)
        return m_pTaskManagementSystem->GetTask((CTaskSAInterface*)dwReturn);
    else
        return NULL;
}

CTask* CTaskManagerSA::GetSimplestActiveTask()
{
    DWORD dwFunc = FUNC_GetSimplestActiveTask;
    DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax
    }

    if (dwReturn) return m_pTaskManagementSystem->GetTask((CTaskSAInterface*)dwReturn);
    else return NULL;
}

CTask* CTaskManagerSA::GetSimplestTask(const int iPriority)
{
    DWORD dwFunc = FUNC_GetSimplestTask;
    DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    iPriority
        call    dwFunc
        mov     dwReturn, eax
    }

    if (dwReturn) return m_pTaskManagementSystem->GetTask((CTaskSAInterface*)dwReturn);
    else return NULL;
}

CTask* CTaskManagerSA::FindActiveTaskByType(const int iTaskType)
{
    DWORD dwFunc = FUNC_FindActiveTaskByType;
    DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    iTaskType
        call    dwFunc
        mov     dwReturn, eax
    }

    if (dwReturn) return m_pTaskManagementSystem->GetTask((CTaskSAInterface*)dwReturn);
    else return NULL;
}

CTask* CTaskManagerSA::FindTaskByType(const int iPriority, const int iTaskType)
{
    DWORD dwFunc = FUNC_FindTaskByType;
    DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    iTaskType
        push    iPriority
        call    dwFunc
        mov     dwReturn, eax
    }

    if (dwReturn) return m_pTaskManagementSystem->GetTask((CTaskSAInterface*)dwReturn);
    else return NULL;
}

void CTaskManagerSA::RemoveTaskSecondary(const int iTaskPriority)
{
    SetTaskSecondary(NULL, iTaskPriority);
}

bool CTaskManagerSA::RemoveTaskSecondary(const int taskPriority, const int taskType)
{
    CTask* task = GetTaskSecondary(taskPriority);
    if (task && task->GetTaskType() == taskType)
    {
        RemoveTaskSecondary(taskPriority);
        return true;
    }

    return false;
}

void CTaskManagerSA::SetTaskSecondary(CTaskSA* pTaskSecondary, const int iType)
{
    DWORD             dwFunc = FUNC_SetTaskSecondary;
    CTaskSAInterface* taskInterface = NULL;
    if (pTaskSecondary)
        taskInterface = pTaskSecondary->GetInterface();
    DWORD dwInterface = (DWORD)GetInterface();
    _asm
    {
        push    iType
        push    taskInterface
        mov     ecx, dwInterface
        call    dwFunc
    }
}

/**
 * \todo Convert to our tasks not gta's (same above too)
 */
CTask* CTaskManagerSA::GetTaskSecondary(const int iType)
{
    if (iType < TASK_SECONDARY_MAX)
        return m_pTaskManagementSystem->GetTask(GetInterface()->m_tasksSecondary[iType]);
    else
        return NULL;
}

bool CTaskManagerSA::HasTaskSecondary(const CTask* pTaskSecondary)
{
    DWORD dwFunc = FUNC_HasTaskSecondary;
    bool  bReturn = false;
    _asm
    {
        push    pTaskSecondary
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CTaskManagerSA::ClearTaskEventResponse()
{
    DWORD dwFunc = FUNC_ClearTaskEventResponse;
    _asm
    {
        call    dwFunc
    }
}

void CTaskManagerSA::Flush(const int iPriority)
{
    for (int i = 0; i < TASK_PRIORITY_MAX; i++)
    {
        CTask* pTask = GetTask(i);
        if (pTask)
        {
            pTask->MakeAbortable(ped, iPriority, NULL);
        }
    }
    for (int i = 0; i < TASK_SECONDARY_MAX; i++)
    {
        CTask* pTask = GetTaskSecondary(i);
        if (pTask)
        {
            pTask->MakeAbortable(ped, iPriority, NULL);
        }
    }
}
