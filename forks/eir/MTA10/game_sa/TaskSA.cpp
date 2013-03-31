/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskSA.cpp
*  PURPOSE:     Base game task
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void* CTaskSAInterface::operator new( size_t )
{
    return (*ppTaskPool)->Allocate();
}

void CTaskSAInterface::operator delete( void *ptr )
{
    (*ppTaskPool)->Free( (CTaskSAInterface*)ptr );
}

CTaskSAInterface* CTaskSAInterface::Clone( void )
{
    return NULL;
}

CTaskSAInterface* CTaskSAInterface::GetSubTask( void )
{
    return NULL;
}

bool CTaskSAInterface::IsSimpleTask( void ) const
{
    return false;
}

int CTaskSAInterface::GetTaskType( void ) const
{
    return -1;
}

void CTaskSAInterface::StopTimer( CEventSAInterface *evt )
{

}

bool CTaskSAInterface::MakeAbortable( CPedSAInterface *ped, int priority, CEventSAInterface *evt )
{
    return false;
}


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


CTask * CTaskSA::Clone( void ) 
{
    DEBUG_TRACE("CTask * CTaskSA::Clone() ");

    assert( 0 );

    // Who made this function anyway? Crash.
    return (CTask *)GetInterface()->Clone();
}

void CTaskSA::SetParent( CTask* pParent ) 
{
    UCTask unionTask;
    unionTask.pTask = pParent;
    this->GetInterface()->m_pParent = unionTask.pTaskSA->GetInterface(); 
    this->Parent = unionTask.pTaskSA; 
}

CTask * CTaskSA::GetSubTask( void ) 
{
    static CTaskManagementSystemSA * s_pTaskManagementSystem = (CTaskManagementSystemSA *)pGame->GetTaskManagementSystem();

    DEBUG_TRACE("CTask * CTaskSA::GetSubTask()");

    return s_pTaskManagementSystem->GetTask ( GetInterface()->GetSubTask() );
}

bool CTaskSA::IsSimpleTask( void )
{
    DEBUG_TRACE("bool CTaskSA::IsSimpleTask()");

    return GetInterface()->IsSimpleTask();
}

int CTaskSA::GetTaskType( void ) 
{
    DEBUG_TRACE("int CTaskSA::GetTaskType()");
    
    return GetInterface()->GetTaskType();
}

/**
 * \todo Handle pEvent correctly to convert it
 */
void CTaskSA::StopTimer(const CEvent* pEvent) 
{
    DEBUG_TRACE("void CTaskSA::StopTimer(const CEvent* pEvent)");
    
    // This function ain't gonna work either. Find a way to convert to the game interface!
    GetInterface()->StopTimer( (CEventSAInterface*)pEvent );
}

/**
 * \todo Handle pEvent correctly to convert it
 */
bool CTaskSA::MakeAbortable(CPed* pPed, const int iPriority, const CEvent* pEvent) 
{
    DEBUG_TRACE("bool CTaskSA::MakeAbortable(CPed* pPed, const int iPriority, const CEvent* pEvent)");

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( !pPedSA ) return false;

    return GetInterface()->MakeAbortable( (CPedSAInterface*)pPedSA->GetInterface(), iPriority, (CEventSAInterface*)pEvent );
}

const char* CTaskSA::GetTaskName()
{
    DEBUG_TRACE("char * CTaskSA::GetTaskName()");
    int iTaskType = GetTaskType();
    if ( iTaskType != NO_TASK_TYPE )
        if ( TaskNames[iTaskType].szName )
            return TaskNames[iTaskType].szName;
        else
            return "";

    return sNoTaskName;
}

void CTaskSA::Destroy()
{
    DEBUG_TRACE("void CTaskSA::Destroy()");

    if ( m_bBeingDestroyed ) // we want to make sure we don't delete this twice or we get crashes :)
        return;              // our hook in CTaskManagementSystem will try to delete this otherwise
    m_bBeingDestroyed = true;
    
    delete GetInterface();

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

    return GetInterface()->ProcessPed( (CPedSAInterface*)pPedSA->GetInterface() );
}

bool CTaskSimpleSA::SetPedPosition ( CPed* pPed )
{
    DEBUG_TRACE("bool CTaskSimpleSA::SetPedPosition(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( !pPedSA ) return false;

    return GetInterface()->SetPedPosition( (CPedSAInterface*)pPedSA->GetInterface() );
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

    GetInterface()->SetSubTask( pSubTask->GetInterface() );
}

CTask * CTaskComplexSA::CreateNextSubTask ( CPed* pPed )
{
    DEBUG_TRACE("CTask * CTaskComplexSA::CreateNextSubTask(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( !pPedSA ) return NULL;

    return pGame->GetTaskManagementSystem()->GetTask( GetInterface()->CreateNextSubTask( (CPedSAInterface*)pPedSA->GetInterface() ) );
}

CTask * CTaskComplexSA::CreateFirstSubTask ( CPed* pPed )
{
    DEBUG_TRACE("CTask * CTaskComplexSA::CreateFirstSubTask(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( !pPedSA ) return NULL;

    return pGame->GetTaskManagementSystem()->GetTask( GetInterface()->CreateFirstSubTask( (CPedSAInterface*)pPedSA->GetInterface() ) );
}

CTask * CTaskComplexSA::ControlSubTask ( CPed* pPed )
{
    DEBUG_TRACE("CTask * CTaskComplexSA::ControlSubTask(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );
    if ( !pPedSA ) return NULL;

    return ((CTaskManagementSystemSA*)pGame->GetTaskManagementSystem())->GetTask ( GetInterface()->ControlSubTask( (CPedSAInterface*)pPedSA->GetInterface() ) );
}

