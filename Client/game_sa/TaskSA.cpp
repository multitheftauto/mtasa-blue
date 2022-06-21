/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskSA.cpp
 *  PURPOSE:     Base game task
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
    dwTasksCreatedTotal++;
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

    // CTask::operator new
    TaskInterface = ((CTaskSAInterface * (__cdecl*)(size_t)) FUNC_CTask__Operator_New)(nSize);
    Parent = 0;
}

CTask* CTaskSA::Clone()
{
    DEBUG_TRACE("CTask * CTaskSA::Clone() ");

    // This returns an CTaskSAInterface*
    return ((CTask * (__thiscall*)(void*)) GetInterface()->VTBL->Clone)(GetInterface());
}

void CTaskSA::SetParent(CTask* pParent)
{
    UCTask unionTask;
    unionTask.pTask = pParent;
    this->GetInterface()->m_pParent = unionTask.pTaskSA->GetInterface();
    this->Parent = unionTask.pTaskSA;
}

CTask* CTaskSA::GetSubTask()
{
    static CTaskManagementSystemSA* s_pTaskManagementSystem = (CTaskManagementSystemSA*)pGame->GetTaskManagementSystem();

    DEBUG_TRACE("CTask * CTaskSA::GetSubTask()");

    CTaskSAInterface* pTask = ((CTaskSAInterface * (__thiscall*)(void*)) GetInterface()->VTBL->GetSubTask)(GetInterface());

    return s_pTaskManagementSystem->GetTask(pTask);
}

bool CTaskSA::IsSimpleTask()
{
    DEBUG_TRACE("bool CTaskSA::IsSimpleTask()");

    return ((bool(__thiscall*)(void*))GetInterface()->VTBL->IsSimpleTask)(GetInterface());
}

int CTaskSA::GetTaskType()
{
    DEBUG_TRACE("int CTaskSA::GetTaskType()");

    DWORD dwFunc = GetInterface()->VTBL->GetTaskType;

    if (dwFunc && dwFunc != 0x82263A)            // some functions have no task type 0x82263A is purecal (assert?)
        return ((int(__thiscall*)(void*))dwFunc)(GetInterface());

    return 9999;
}

/**
 * \todo Handle pEvent correctly to convert it
 */
void CTaskSA::StopTimer(const CEvent* pEvent)
{
    DEBUG_TRACE("void CTaskSA::StopTimer(const CEvent* pEvent)");
    DWORD dwFunc = GetInterface()->VTBL->StopTimer;
    if (dwFunc != 0x82263A && dwFunc)
        ((void(__thiscall*)(void*, const CEvent*))dwFunc)(GetInterface(), pEvent);
}

/**
 * \todo Handle pEvent correctly to convert it
 */
bool CTaskSA::MakeAbortable(CPed* pPed, const int iPriority, const CEvent* pEvent)
{
    DEBUG_TRACE("bool CTaskSA::MakeAbortable(CPed* pPed, const int iPriority, const CEvent* pEvent)");

    CPedSA* pPedSA = dynamic_cast<CPedSA*>(pPed);
    if (!pPedSA)
        return false;

    DWORD dwFunc = this->GetInterface()->VTBL->MakeAbortable;
    if (dwFunc != 0x82263A && dwFunc)            // 82263A = purecall
        return ((bool(__thiscall*)(void*, CPedSAInterface*, int, const CEvent*))dwFunc)(GetInterface(), pPedSA->GetPedInterface(), iPriority, pEvent);

    return false;
}

const char* CTaskSA::GetTaskName()
{
    DEBUG_TRACE("char * CTaskSA::GetTaskName()");
    int iTaskType = GetTaskType();
    if (iTaskType != NO_TASK_TYPE)
        if (TaskNames[iTaskType].szName)
            return TaskNames[iTaskType].szName;
        else
            return "";

    return sNoTaskName;
}

void CTaskSA::Destroy()
{
    DEBUG_TRACE("void CTaskSA::Destroy()");

    if (m_bBeingDestroyed)            // we want to make sure we don't delete this twice or we get crashes :)
        return;                       // our hook in CTaskManagementSystem will try to delete this otherwise
    m_bBeingDestroyed = true;

    DWORD dwFunc = GetInterface()->VTBL->DeletingDestructor;
    if (dwFunc)
        ((void(__thiscall*)(void*, bool))dwFunc)(GetInterface(), true);

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

    if (m_bBeingDestroyed)            // we want to make sure we don't delete this twice or we get crashes :)
        return;                       // our hook in CTaskManagementSystem will try to delete this otherwise
    m_bBeingDestroyed = true;

    delete this;
}

void CTaskSA::SetAsPedTask(CPed* pPed, const int iTaskPriority, const bool bForceNewTask)
{
    ((CTaskManagerSA*)pPed->GetPedIntelligence()->GetTaskManager())->SetTask(this, iTaskPriority, bForceNewTask);
}

void CTaskSA::SetAsSecondaryPedTask(CPed* pPed, const int iType)
{
    ((CTaskManagerSA*)pPed->GetPedIntelligence()->GetTaskManager())->SetTaskSecondary(this, iType);
}

// ####################################################################
// ## CTaskSimple Functions
// ####################################################################

bool CTaskSimpleSA::ProcessPed(CPed* pPed)
{
    DEBUG_TRACE("bool CTaskSimpleSA::ProcessPed(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast<CPedSA*>(pPed);
    if (!pPedSA)
        return false;

    DWORD dwFunc = ((TaskSimpleVTBL*)GetInterface()->VTBL)->ProcessPed;
    if (dwFunc != 0x82263A && dwFunc)
        return ((bool(__thiscall*)(void*, CPedSAInterface*))dwFunc)(GetInterface(), pPedSA->GetPedInterface());

    return false;
}

bool CTaskSimpleSA::SetPedPosition(CPed* pPed)
{
    DEBUG_TRACE("bool CTaskSimpleSA::SetPedPosition(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast<CPedSA*>(pPed);
    if (!pPedSA)
        return false;

    DWORD dwFunc = ((TaskSimpleVTBL*)GetInterface()->VTBL)->SetPedPosition;
    if (dwFunc != 0x82263A && dwFunc)
        return ((bool(__thiscall*)(void*, CPedSAInterface*))dwFunc)(GetInterface(), pPedSA->GetPedInterface());

    return false;
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

    DWORD dwFunc = ((TaskComplexVTBL*)this->GetInterface()->VTBL)->SetSubTask;
    if (dwFunc != 0x82263A && dwFunc)
        ((void(__thiscall*)(void*, CTaskSAInterface*))dwFunc)(GetInterface(), pSubTask->GetInterface());
}

CTask* CTaskComplexSA::CreateNextSubTask(CPed* pPed)
{
    DEBUG_TRACE("CTask * CTaskComplexSA::CreateNextSubTask(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast<CPedSA*>(pPed);
    if (!pPedSA)
        return NULL;

    DWORD             dwFunc = ((TaskComplexVTBL*)this->GetInterface()->VTBL)->CreateNextSubTask;
    CTaskSAInterface* pTaskInterface = nullptr;
    if (dwFunc != 0x82263A && dwFunc)
        pTaskInterface = ((CTaskSAInterface * (__thiscall*)(void*, CPedSAInterface*)) dwFunc)(GetInterface(), pPedSA->GetPedInterface());

    return ((CTaskManagementSystemSA*)pGame->GetTaskManagementSystem())->GetTask(pTaskInterface);
}

CTask* CTaskComplexSA::CreateFirstSubTask(CPed* pPed)
{
    DEBUG_TRACE("CTask * CTaskComplexSA::CreateFirstSubTask(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast<CPedSA*>(pPed);
    if (!pPedSA)
        return NULL;

    DWORD dwFunc = ((TaskComplexVTBL*)this->GetInterface()->VTBL)->CreateFirstSubTask;

    CTaskSAInterface* pTaskInterface = nullptr;
    if (dwFunc != 0x82263A && dwFunc)
        pTaskInterface = ((CTaskSAInterface * (__thiscall*)(void*, CPedSAInterface*)) dwFunc)(GetInterface(), pPedSA->GetPedInterface());

    return ((CTaskManagementSystemSA*)pGame->GetTaskManagementSystem())->GetTask(pTaskInterface);
}

CTask* CTaskComplexSA::ControlSubTask(CPed* pPed)
{
    DEBUG_TRACE("CTask * CTaskComplexSA::ControlSubTask(CPed* pPed)");

    CPedSA* pPedSA = dynamic_cast<CPedSA*>(pPed);
    if (!pPedSA)
        return NULL;

    DWORD dwFunc = ((TaskComplexVTBL*)this->GetInterface()->VTBL)->ControlSubTask;

    CTaskSAInterface* pTaskInterface = nullptr;
    if (dwFunc != 0x82263A && dwFunc)
        pTaskInterface = ((CTaskSAInterface * (__thiscall*)(void*, CPedSAInterface*)) dwFunc)(GetInterface(), pPedSA->GetPedInterface());

    return ((CTaskManagementSystemSA*)pGame->GetTaskManagementSystem())->GetTask(pTaskInterface);
}
