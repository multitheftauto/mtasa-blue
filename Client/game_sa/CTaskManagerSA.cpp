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

CTaskManagerSA::CTaskManagerSA(CTaskManagerSAInterface* taskManagerInterface, CPed* ped)
{
    DEBUG_TRACE("CTaskManagerSA::CTaskManagerSA(CTaskManagerSAInterface * taskManagerInterface, CPed * ped)");
    this->ped = ped;
    this->internalInterface = taskManagerInterface;
    this->m_pTaskManagementSystem = (CTaskManagementSystemSA*)(pGame->GetTaskManagementSystem());
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
    DEBUG_TRACE("void CTaskManagerSA::SetTask(CTask* pTaskPrimary, const int iTaskPriority, const bool bForceNewTask)");

    CTaskSAInterface* pTaskInterface = pTaskPrimary ? pTaskPrimary->GetInterface() : nullptr;

    // CTaskManager::SetTask
    ((void(__thiscall*)(void*, CTaskSAInterface*, int, bool))FUNC_SetTask)(GetInterface(), pTaskInterface, iTaskPriority, bForceNewTask);
}

CTask* CTaskManagerSA::GetTask(const int iTaskPriority)
{
    DEBUG_TRACE("CTask* CTaskManagerSA::GetTask(const int iTaskPriority)");
    CTaskManagerSAInterface* pTaskManagerInterface = this->GetInterface();
    return m_pTaskManagementSystem->GetTask(pTaskManagerInterface->m_tasks[iTaskPriority]);
}

CTask* CTaskManagerSA::GetActiveTask()
{
    DEBUG_TRACE("CTask* CTaskManagerSA::GetActiveTask()");

    // CTaskManager::GetActiveTask
    CTaskSAInterface* pTaskInterface = ((CTaskSAInterface * (__thiscall*)(CTaskManagerSAInterface*)) FUNC_GetActiveTask)(this->GetInterface());

    if (pTaskInterface)
        return m_pTaskManagementSystem->GetTask(pTaskInterface);

    return nullptr;
}

CTask* CTaskManagerSA::GetSimplestActiveTask()
{
    DEBUG_TRACE("CTask* CTaskManagerSA::GetSimplestActiveTask()");

    // CTaskManager::GetSimplestActiveTask
    CTaskSAInterface* pTaskInterface = ((CTaskSAInterface * (__thiscall*)(CTaskManagerSAInterface*)) FUNC_GetSimplestActiveTask)(this->GetInterface());

    if (pTaskInterface)
        return m_pTaskManagementSystem->GetTask(pTaskInterface);

    return nullptr;
}

CTask* CTaskManagerSA::GetSimplestTask(const int iPriority)
{
    DEBUG_TRACE("CTask* CTaskManagerSA::GetSimplestTask(const int iPriority)");

    // CTaskManager::GetSimplestTask
    CTaskSAInterface* pTaskInterface =
        ((CTaskSAInterface * (__thiscall*)(CTaskManagerSAInterface*, int)) FUNC_GetSimplestTask)(this->GetInterface(), iPriority);

    if (pTaskInterface)
        return m_pTaskManagementSystem->GetTask(pTaskInterface);

    return nullptr;
}

CTask* CTaskManagerSA::FindActiveTaskByType(const int iTaskType)
{
    DEBUG_TRACE("CTask* CTaskManagerSA::FindActiveTaskByType(const int iTaskType)");

    // CTaskManager::FindActiveTaskByType
    CTaskSAInterface* pTaskInterface =
        ((CTaskSAInterface * (__thiscall*)(CTaskManagerSAInterface*, int)) FUNC_FindActiveTaskByType)(this->GetInterface(), iTaskType);

    if (pTaskInterface)
        return m_pTaskManagementSystem->GetTask(pTaskInterface);

    return nullptr;
}

CTask* CTaskManagerSA::FindTaskByType(const int iPriority, const int iTaskType)
{
    DEBUG_TRACE("CTask* CTaskManagerSA::FindTaskByType(const int iPriority, const int iTaskType)");

    // CTaskManager::FindTaskByType
    CTaskSAInterface* pTaskInterface =
        ((CTaskSAInterface * (__thiscall*)(CTaskManagerSAInterface*, int, int)) FUNC_FindTaskByType)(this->GetInterface(), iPriority, iTaskType);

    if (pTaskInterface)
        return m_pTaskManagementSystem->GetTask(pTaskInterface);

    return nullptr;
}

void CTaskManagerSA::RemoveTaskSecondary(const int iTaskPriority)
{
    SetTaskSecondary(NULL, iTaskPriority);
}

void CTaskManagerSA::SetTaskSecondary(CTaskSA* pTaskSecondary, const int iType)
{
    DEBUG_TRACE("void CTaskManagerSA::SetTaskSecondary(CTask* pTaskSecondary, const int iType)");

    CTaskSAInterface* pTaskInterface = pTaskSecondary ? pTaskSecondary->GetInterface() : nullptr;

    // CTaskManager::SetTaskSecondary
    ((void(__thiscall*)(void*, CTaskSAInterface*, int))FUNC_SetTaskSecondary)(GetInterface(), pTaskInterface, iType);
}

/**
 * \todo Convert to our tasks not gta's (same above too)
 */
CTask* CTaskManagerSA::GetTaskSecondary(const int iType)
{
    DEBUG_TRACE("CTask* CTaskManagerSA::GetTaskSecondary(const int iType)");
    if (iType < TASK_SECONDARY_MAX)
        return m_pTaskManagementSystem->GetTask(this->GetInterface()->m_tasksSecondary[iType]);
    else
        return NULL;
}

bool CTaskManagerSA::HasTaskSecondary(const CTask* pTaskSecondary)
{
    DEBUG_TRACE("bool CTaskManagerSA::HasTaskSecondary(const CTask* pTaskSecondary)");
    // was not called as thiscall and pushes CTask* where it should be CTaskSAInterface*
    // CTaskManager::HasTaskSecondary
    return ((bool(__thiscall*)(void*, const CTask*))FUNC_HasTaskSecondary)(GetInterface(), pTaskSecondary);
}

void CTaskManagerSA::ClearTaskEventResponse()
{
    DEBUG_TRACE("void CTaskManagerSA::ClearTaskEventResponse()");

    // CTaskManager::ClearTaskEventResponse
    ((void(__thiscall*)(void*))FUNC_ClearTaskEventResponse)(GetInterface());
}

void CTaskManagerSA::Flush(const int iPriority)
{
    DEBUG_TRACE("void CTaskManagerSA::Flush(bool bImmediately)");
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
