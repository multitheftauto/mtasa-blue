/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CTaskManagerSA.cpp
 *  PURPOSE:     Task manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
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
    m_ped = ped;
    internalInterface = taskManagerInterface;
    m_pTaskManagementSystem = pGame->GetTaskManagementSystem();
}

void CTaskManagerSA::RemoveTask(const int iTaskPriority)
{
    if (iTaskPriority != TASK_PRIORITY_DEFAULT) { SetTask(nullptr, iTaskPriority); } // TASK_PRIORITY_DEFAULT removed = crash
}

void CTaskManagerSA::SetTask(CTaskSA* pTaskPrimary, const int iTaskPriority, const bool bForceNewTask)
{
    DWORD             dwFunc = 0x681AF0;
    CTaskSAInterface* pTaskSAInterface = nullptr;
    if (pTaskPrimary) pTaskSAInterface = pTaskPrimary->GetInterface();
    DWORD dwInterface = (DWORD)GetInterface();
    _asm
    {
        xor     eax, eax
        movzx   eax, bForceNewTask
        push    eax
        push    iTaskPriority
        push    pTaskSAInterface
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
    DWORD dwFunc = 0x681720;
    DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax
    }

    CTaskSAInterface* pActiveTask = (CTaskSAInterface*)dwReturn;
    if (dwReturn) return m_pTaskManagementSystem->GetTask((CTaskSAInterface*)dwReturn);
    else return nullptr;
}

CTask* CTaskManagerSA::GetSimplestActiveTask()
{
    DWORD dwFunc = 0x6819D0;
    DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax
    }

    if (dwReturn) return m_pTaskManagementSystem->GetTask((CTaskSAInterface*)dwReturn);
    else return nullptr;
}

CTask* CTaskManagerSA::GetSimplestTask(const int iPriority)
{
    DWORD dwFunc = 0x681A00;
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
    else return nullptr;
}

CTask* CTaskManagerSA::FindActiveTaskByType(const int iTaskType)
{
    DWORD dwFunc = 0x681740;
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
    else return nullptr;
}

CTask* CTaskManagerSA::FindTaskByType(const int iPriority, const int iTaskType)
{
    DWORD dwFunc = 0x6817D0;
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
    else return nullptr;
}

void CTaskManagerSA::RemoveTaskSecondary(const int iTaskPriority)
{
    SetTaskSecondary(nullptr, iTaskPriority);
}

void CTaskManagerSA::SetTaskSecondary(CTaskSA* pTaskSecondary, const int iType)
{
    DWORD             dwFunc = 0x681B60;
    CTaskSAInterface* pTaskSAInterface = nullptr;
    if (pTaskSecondary)
        pTaskSAInterface = pTaskSecondary->GetInterface();
    DWORD dwInterface = (DWORD)GetInterface();
    _asm
    {
        push    iType
        push    pTaskSAInterface
        mov     ecx, dwInterface
        call    dwFunc
    }
}

/**
 * \todo Convert to our tasks not gta's (same above too)
 */
CTask* CTaskManagerSA::GetTaskSecondary(const int iType)
{
    if (iType < TASK_SECONDARY_MAX) return m_pTaskManagementSystem->GetTask(GetInterface()->m_tasksSecondary[iType]);
    else return nullptr;
}

bool CTaskManagerSA::HasTaskSecondary(const CTask* pTaskSecondary)
{
    DWORD dwFunc = 0x681820;
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
    DWORD dwFunc = 0x681BD0;
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
            pTask->MakeAbortable(m_ped, iPriority, nullptr);
        }
    }
    for (int i = 0; i < TASK_SECONDARY_MAX; i++)
    {
        CTask* pTask = GetTaskSecondary(i);
        if (pTask)
        {
            pTask->MakeAbortable(m_ped, iPriority, nullptr);
        }
    }
}
