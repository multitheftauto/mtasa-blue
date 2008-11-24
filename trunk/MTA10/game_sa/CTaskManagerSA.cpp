/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTaskManagerSA.cpp
*  PURPOSE:     Task manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTaskManagerSA::CTaskManagerSA(CTaskManagerSAInterface * taskManagerInterface, CPed * ped)
{
	DEBUG_TRACE("CTaskManagerSA::CTaskManagerSA(CTaskManagerSAInterface * taskManagerInterface, CPed * ped)");
	this->ped = ped;
	this->internalInterface = taskManagerInterface;
    this->m_pTaskManagementSystem = (CTaskManagementSystemSA *)(pGame->GetTaskManagementSystem());
}

void CTaskManagerSA::RemoveTask(const int iTaskPriority)
{
    if ( iTaskPriority != TASK_PRIORITY_DEFAULT ) // TASK_PRIORITY_DEFAULT removed = crash
    {
        SetTask(NULL, iTaskPriority);
    }
}

void CTaskManagerSA::SetTask(CTaskSA* pTaskPrimary, const int iTaskPriority, const bool bForceNewTask)
{
	DEBUG_TRACE("void CTaskManagerSA::SetTask(CTask* pTaskPrimary, const int iTaskPriority, const bool bForceNewTask)");
    
	DWORD dwFunc = FUNC_SetTask;
    CTaskSAInterface * taskInterface = NULL;
    if ( pTaskPrimary ) taskInterface = pTaskPrimary->GetInterface();

	DWORD dwInterface = (DWORD)this->GetInterface();
	_asm
	{
		xor		eax, eax
		movzx	eax, bForceNewTask
		push	eax
		push	iTaskPriority
		push	taskInterface
		mov		ecx, dwInterface
		call	dwFunc
	}    
}

CTask* CTaskManagerSA::GetTask(const int iTaskPriority)
{
    //_asm int 3
	DEBUG_TRACE("CTask* CTaskManagerSA::GetTask(const int iTaskPriority)");
    CTaskManagerSAInterface * pTaskManagerInterface = this->GetInterface();
    return m_pTaskManagementSystem->GetTask ( pTaskManagerInterface->m_tasks[iTaskPriority] );
}

CTask* CTaskManagerSA::GetActiveTask()
{
	DEBUG_TRACE("CTask* CTaskManagerSA::GetActiveTask()");
	DWORD dwFunc = FUNC_GetActiveTask;
	DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
        mov     ecx, dwThis
		call	dwFunc
		mov		dwReturn, eax
	}

    CTaskSAInterface * pActiveTask = (CTaskSAInterface *)dwReturn;
    if ( dwReturn )
        return m_pTaskManagementSystem->GetTask ( (CTaskSAInterface *)dwReturn );
    else
        return NULL;
}

CTask* CTaskManagerSA::GetSimplestActiveTask()
{
	DEBUG_TRACE("CTask* CTaskManagerSA::GetSimplestActiveTask()");
	DWORD dwFunc = FUNC_GetSimplestActiveTask;
	DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)this->GetInterface();

	_asm
	{
        mov     ecx, dwThis
		call	dwFunc
		mov		dwReturn, eax
	}

	if ( dwReturn )
        return m_pTaskManagementSystem->GetTask ( (CTaskSAInterface *)dwReturn );
    else
        return NULL;
}

CTask* CTaskManagerSA::GetSimplestTask(const int iPriority)
{
	DEBUG_TRACE("CTask* CTaskManagerSA::GetSimplestTask(const int iPriority)");
	DWORD dwFunc = FUNC_GetSimplestTask;
	DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
        mov     ecx, dwThis
		push	iPriority
		call	dwFunc
		mov		dwReturn, eax
	}

	if ( dwReturn )
        return m_pTaskManagementSystem->GetTask ( (CTaskSAInterface *)dwReturn );
    else
        return NULL;
}

CTask* CTaskManagerSA::FindActiveTaskByType(const int iTaskType)
{
	DEBUG_TRACE("CTask* CTaskManagerSA::FindActiveTaskByType(const int iTaskType)");
	DWORD dwFunc = FUNC_FindActiveTaskByType;
	DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
        mov     ecx, dwThis
		push	iTaskType
		call	dwFunc
		mov		dwReturn, eax
	}

	if ( dwReturn )
        return m_pTaskManagementSystem->GetTask ( (CTaskSAInterface *)dwReturn );
    else
        return NULL;
}

CTask* CTaskManagerSA::FindTaskByType(const int iPriority, const int iTaskType)
{
	DEBUG_TRACE("CTask* CTaskManagerSA::FindTaskByType(const int iPriority, const int iTaskType)");
	DWORD dwFunc = FUNC_FindTaskByType;
	DWORD dwReturn = 0;
    DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
        mov     ecx, dwThis
		push	iTaskType
		push	iPriority
		call	dwFunc
		mov		dwReturn, eax
	}

	if ( dwReturn )
        return m_pTaskManagementSystem->GetTask ( (CTaskSAInterface *)dwReturn );
    else
        return NULL;
}

void CTaskManagerSA::RemoveTaskSecondary(const int iTaskPriority)
{
    SetTaskSecondary(NULL, iTaskPriority);
}

void CTaskManagerSA::SetTaskSecondary(CTaskSA* pTaskSecondary, const int iType)
{
	DEBUG_TRACE("void CTaskManagerSA::SetTaskSecondary(CTask* pTaskSecondary, const int iType)");
	DWORD dwFunc = FUNC_SetTaskSecondary;
	CTaskSAInterface * taskInterface = NULL;
    if ( pTaskSecondary ) taskInterface = pTaskSecondary->GetInterface();
	DWORD dwInterface = (DWORD)this->GetInterface();
	_asm
	{
		push	iType
		push	taskInterface
        mov		ecx, dwInterface
		call	dwFunc
	}
}

/**
 * \todo Convert to our tasks not gta's (same above too)
 */
CTask* CTaskManagerSA::GetTaskSecondary(const int iType)
{
	DEBUG_TRACE("CTask* CTaskManagerSA::GetTaskSecondary(const int iType)");
    if ( iType < TASK_SECONDARY_MAX )
        return m_pTaskManagementSystem->GetTask ( this->GetInterface()->m_tasksSecondary[iType] );
    else
        return NULL;
}

bool CTaskManagerSA::HasTaskSecondary(const CTask* pTaskSecondary)
{
    _asm int 3
	DEBUG_TRACE("bool CTaskManagerSA::HasTaskSecondary(const CTask* pTaskSecondary)");
	DWORD dwFunc = FUNC_HasTaskSecondary;
	bool bReturn = false;
	_asm
	{
		push	pTaskSecondary
		call	dwFunc
		mov		bReturn, al
	}
	return bReturn;
}

void CTaskManagerSA::ClearTaskEventResponse()
{
	DEBUG_TRACE("void CTaskManagerSA::ClearTaskEventResponse()");
	DWORD dwFunc = FUNC_ClearTaskEventResponse;
	_asm
	{
		call	dwFunc
	}
}

void CTaskManagerSA::Flush(const int iPriority)
{
    DEBUG_TRACE("void CTaskManagerSA::Flush(bool bImmediately)");
    for ( int i = 0 ; i < TASK_PRIORITY_MAX ; i++ )
    {
        CTask * pTask = GetTask ( i );
        if ( pTask )
        {
            pTask->MakeAbortable ( ped, iPriority, NULL );
        }
    }
    for ( int i = 0 ; i < TASK_SECONDARY_MAX ; i++ )
    {
        CTask * pTask = GetTaskSecondary ( i );
        if ( pTask )
        {
            pTask->MakeAbortable ( ped, iPriority, NULL );
        }
    }
}
