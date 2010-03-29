/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskSA.cpp
*  PURPOSE:     Base game task
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

DWORD dwTasksAlive = 0;
DWORD dwTasksCreatedTotal = 0;

CTaskSA::CTaskSA()
{
    DEBUG_TRACE("CTaskSA::CTaskSA()");
    Parent = 0;
    TaskInterface = 0;
    dwTasksCreatedTotal ++;
    dwTasksAlive++;
    m_bBeingDestroyed = false;
}

CTaskSA::~CTaskSA()
{
    DEBUG_TRACE("CTaskSA::~CTaskSA()");
    dwTasksAlive--;
}


// alocate memory for the task (ammount nSize)
void CTaskSA::CreateTaskInterface(size_t nSize)
{
    DEBUG_TRACE("void CTaskSA::CreateTaskInterface(size_t nSize)");

    // Safety margin. I see GTA allocate more than we do for some tasks. We could create
    // crashes by not allocating enough. Better to potentially waste 12 bytes.
    nSize = nSize + 12;

    DWORD dwFunc = FUNC_CTask__Operator_New;
    DWORD dwReturn = 0;
    _asm
    {
        push    nSize
        call    dwFunc
        add     esp, 4
        mov     dwReturn, eax
    }

    TaskInterface = (CTaskSAInterface*)dwReturn;
    Parent = 0;
}


CTask * CTaskSA::Clone() 
{
    DEBUG_TRACE("CTask * CTaskSA::Clone() ");
    DWORD dwThisInterface = (DWORD)this->GetInterface();
    DWORD dwFunc = this->GetInterface()->VTBL->Clone;
    DWORD dwReturn = 0;
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     dwReturn, eax
    }
    return (CTask *)dwReturn;
}

void CTaskSA::SetParent(CTask* pParent) 
{
    UCTask unionTask;
    unionTask.pTask = pParent;
    this->GetInterface()->m_pParent = unionTask.pTaskSA->GetInterface(); 
    this->Parent = unionTask.pTaskSA; 
}

CTask * CTaskSA::GetSubTask() 
{
    static CTaskManagementSystemSA * s_pTaskManagementSystem = (CTaskManagementSystemSA *)pGame->GetTaskManagementSystem();

    DEBUG_TRACE("CTask * CTaskSA::GetSubTask()");
    DWORD dwThisInterface = (DWORD)this->GetInterface();
    DWORD dwFunc = this->GetInterface()->VTBL->GetSubTask;
    DWORD dwReturn = 0;
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     dwReturn, eax
    }
    return s_pTaskManagementSystem->GetTask ( (CTaskSAInterface *)dwReturn );
}

bool CTaskSA::IsSimpleTask()
{
    DEBUG_TRACE("bool CTaskSA::IsSimpleTask()");
    DWORD dwThisInterface = (DWORD)this->GetInterface();
    DWORD dwFunc = this->GetInterface()->VTBL->IsSimpleTask;
    bool bReturn = 0;
    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

int CTaskSA::GetTaskType() 
{
    DEBUG_TRACE("int CTaskSA::GetTaskType()");
    CTaskSAInterface * pTaskInterface = this->GetInterface();
   
    DWORD dwFunc = pTaskInterface->VTBL->GetTaskType;
    int iReturn = 9999;

    if ( dwFunc && dwFunc != 0x82263A ) // some functions have no task type 0x82263A is purecal (assert?)
    {
        _asm
        {
            mov     ecx, pTaskInterface
            call    dwFunc
            mov     iReturn, eax
        }
    }
    return iReturn;
}

/**
 * \todo Handle pEvent correctly to convert it
 */
void CTaskSA::StopTimer(const CEvent* pEvent) 
{
    DEBUG_TRACE("void CTaskSA::StopTimer(const CEvent* pEvent)");
    DWORD dwThisInterface = (DWORD)this->GetInterface();
    DWORD dwFunc = this->GetInterface()->VTBL->StopTimer;
    if ( dwFunc != 0x82263A  && dwFunc )
    {
        _asm
        {
            mov     ecx, dwThisInterface
            push    pEvent
            call    dwFunc
        }
    }
}

/**
 * \todo Handle pEvent correctly to convert it
 */
bool CTaskSA::MakeAbortable(CPed* pPed, const int iPriority, const CEvent* pEvent) 
{
    DEBUG_TRACE("bool CTaskSA::MakeAbortable(CPed* pPed, const int iPriority, const CEvent* pEvent)");

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( !pPedSA ) return false;

    DWORD dwPedInterface = (DWORD) pPedSA->GetInterface ();
    DWORD dwThisInterface = (DWORD) this->GetInterface ();
    DWORD dwFunc = this->GetInterface ()->VTBL->MakeAbortable;
    bool bReturn = 0;
    if ( dwFunc != 0x82263A  && dwFunc ) // 82263A = purecall
    {
        _asm
        {
            mov     ecx, dwThisInterface
            push    pEvent
            push    iPriority
            push    dwPedInterface
            call    dwFunc
            mov     bReturn, al
        }
    }
    return bReturn;
}

char * CTaskSA::GetTaskName()
{
    DEBUG_TRACE("char * CTaskSA::GetTaskName()");
    int iTaskType = GetTaskType();
    if ( iTaskType != NO_TASK_TYPE )
        return TaskNames[iTaskType].szName;
    else
        return reinterpret_cast < char* > ( &sNoTaskName );
}

void CTaskSA::Destroy()
{
    DEBUG_TRACE("void CTaskSA::Destroy()");

    if ( m_bBeingDestroyed ) // we want to make sure we don't delete this twice or we get crashes :)
        return;              // our hook in CTaskManagementSystem will try to delete this otherwise
    m_bBeingDestroyed = true;

    DWORD dwThisInterface = (DWORD)this->GetInterface();
    DWORD dwFunc = this->GetInterface()->VTBL->DeletingDestructor;
    if ( dwFunc )
    {
        _asm
        {
            mov     ecx, dwThisInterface
            push    1           // delete the task too
            call    dwFunc
        }
    }

    /*dwFunc = FUNC_CTask__Operator_Delete;
    DWORD thisInterface = (DWORD)this->GetInterface();
    if ( thisInterface )
    {
        _asm
        {
            push    thisInterface
            call    dwFunc
            add     esp, 4
        }
    }*/

    delete this;
}

void CTaskSA::DestroyJustThis()
{
    DEBUG_TRACE("void CTaskSA::DestroyJustThis()");

    if ( m_bBeingDestroyed ) // we want to make sure we don't delete this twice or we get crashes :)
        return;              // our hook in CTaskManagementSystem will try to delete this otherwise
    m_bBeingDestroyed = true;

    delete this;
}

void CTaskSA::SetAsPedTask ( CPed * pPed, const int iTaskPriority, const bool bForceNewTask )
{
    ((CTaskManagerSA*)pPed->GetPedIntelligence()->GetTaskManager())->SetTask ( this, iTaskPriority, bForceNewTask );
}

void CTaskSA::SetAsSecondaryPedTask ( CPed * pPed, const int iType )
{
    ((CTaskManagerSA*)pPed->GetPedIntelligence()->GetTaskManager())->SetTaskSecondary ( this, iType );
}


// ####################################################################
// ## CTaskSimple Functions
// ####################################################################

bool CTaskSimpleSA::ProcessPed ( CPed* pPed )
{
    DEBUG_TRACE("bool CTaskSimpleSA::ProcessPed(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( !pPedSA ) return false;

    DWORD dwPedInterface = (DWORD)pPedSA->GetInterface ();
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwFunc = ((TaskSimpleVTBL *)this->GetInterface ()->VTBL)->ProcessPed;
    bool bReturn = 0;
    if ( dwFunc != 0x82263A && dwFunc )
    {
        _asm
        {
            mov     ecx, dwThisInterface
            push    dwPedInterface
            call    dwFunc
            mov     bReturn, al
        }
    }
    return bReturn;
}

bool CTaskSimpleSA::SetPedPosition ( CPed* pPed )
{
    DEBUG_TRACE("bool CTaskSimpleSA::SetPedPosition(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( !pPedSA ) return false;

    DWORD dwPedInterface = (DWORD)pPedSA->GetInterface ();
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwFunc = ((TaskSimpleVTBL *)this->GetInterface ()->VTBL)->SetPedPosition;
    bool bReturn = 0;
    if ( dwFunc != 0x82263A && dwFunc )
    {
        _asm
        {
            mov     ecx, dwThisInterface
            push    dwPedInterface
            call    dwFunc
            mov     bReturn, al
        }
    }
    return bReturn;
}
// ####################################################################
// ## CTaskComplex Functions
// ####################################################################
/**
 * \todo Implement subtask related stuff without adding variables (so sizeof(CTask) == sizeof(CTaskComplex))
 */
/*
CTaskComplexSA::CTaskComplexSA()
{
    DEBUG_TRACE("CTaskComplexSA::CTaskComplexSA()");
//  this->m_pSubTask = 0;
}
*/


void CTaskComplexSA::SetSubTask(CTask* pSubTask)
{
    DEBUG_TRACE("void CTaskComplexSA::SetSubTask(CTask* pSubTask)");
/*  if(this->m_pSubTask)
        delete this->m_pSubTask;
    else
        this->m_pSubTask = pSubTask;*/

    DWORD dwTaskInterface = (DWORD)pSubTask->GetInterface();
    DWORD dwThisInterface = (DWORD)this->GetInterface();
    DWORD dwFunc = ((TaskComplexVTBL *)this->GetInterface()->VTBL)->SetSubTask;
    if ( dwFunc != 0x82263A  && dwFunc )
    {
        _asm
        {
            mov     ecx, dwThisInterface
            push    dwTaskInterface
            call    dwFunc
        }
    }
}

CTask * CTaskComplexSA::CreateNextSubTask ( CPed* pPed )
{
    DEBUG_TRACE("CTask * CTaskComplexSA::CreateNextSubTask(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( !pPedSA ) return NULL;

    DWORD dwPedInterface = (DWORD)pPedSA->GetInterface ();
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwFunc = ((TaskComplexVTBL *)this->GetInterface ()->VTBL)->CreateNextSubTask;
    DWORD dwReturn = 0;
    if ( dwFunc != 0x82263A && dwFunc )
    {
        _asm
        {
            mov     ecx, dwThisInterface
            push    dwPedInterface
            call    dwFunc
            mov     dwReturn, eax
        }
    }
    return ((CTaskManagementSystemSA*)pGame->GetTaskManagementSystem())->GetTask ( ( CTaskSAInterface * ) dwReturn );
}

CTask * CTaskComplexSA::CreateFirstSubTask ( CPed* pPed )
{
    DEBUG_TRACE("CTask * CTaskComplexSA::CreateFirstSubTask(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( !pPedSA ) return NULL;

    DWORD dwPedInterface = (DWORD)pPedSA->GetInterface ();
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwFunc = ((TaskComplexVTBL *)this->GetInterface ()->VTBL)->CreateFirstSubTask;
    DWORD dwReturn = 0;
    if ( dwFunc != 0x82263A && dwFunc )
    {
        _asm
        {
            mov     ecx, dwThisInterface
            push    dwPedInterface
            call    dwFunc
            mov     dwReturn, eax
        }
    }
    return ((CTaskManagementSystemSA*)pGame->GetTaskManagementSystem())->GetTask ( ( CTaskSAInterface * ) dwReturn );
}

CTask * CTaskComplexSA::ControlSubTask ( CPed* pPed )
{
    DEBUG_TRACE("CTask * CTaskComplexSA::ControlSubTask(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( !pPedSA ) return NULL;

    DWORD dwPedInterface = (DWORD)pPedSA->GetInterface ();
    DWORD dwThisInterface = (DWORD)this->GetInterface ();
    DWORD dwFunc = ((TaskComplexVTBL *)this->GetInterface ()->VTBL)->ControlSubTask;
    DWORD dwReturn = 0;
    if ( dwFunc != 0x82263A && dwFunc )
    {
        _asm
        {
            mov     ecx, dwThisInterface
            push    dwPedInterface
            call    dwFunc
            mov     dwReturn, eax
        }
    }
    return ((CTaskManagementSystemSA*)pGame->GetTaskManagementSystem())->GetTask ( ( CTaskSAInterface * ) dwReturn );
}

