/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskSA.h
*  PURPOSE:     Base game task
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASK
#define __CGAMESA_TASK

#include <game/Task.h>
#include "TaskNamesSA.h"

#define FUNC_CTask__Operator_New            0x61A5A0
#define FUNC_CTask__Operator_Delete         0x61A5B0

class CTaskTimer
{
public:
    DWORD dwTimeStart; // ?
    DWORD dwTimeEnd; // ?
    bool bSet;
    bool bStopped;
};

class TaskVTBL
{
public:
    DWORD DeletingDestructor;
    DWORD Clone;
    DWORD GetSubTask;
    DWORD IsSimpleTask;
    DWORD GetTaskType;
    DWORD StopTimer;
    DWORD MakeAbortable;
};

class TaskSimpleVTBL : public TaskVTBL
{
public:
    DWORD ProcessPed;
    DWORD SetPedPosition;
};

class TaskComplexVTBL : public TaskVTBL
{
public:
    DWORD SetSubTask; 
    DWORD CreateNextSubTask;
    DWORD CreateFirstSubTask;
    DWORD ControlSubTask;
};

class CTaskSAInterface
{
public:
    TaskVTBL * VTBL; // cast to either TaskSimpleVTBL or TaskComplexVTBL
    CTaskSAInterface * m_pParent;
};

class CTaskSA : public virtual CTask
{
private:
    // our variable(s)
    CTaskSAInterface * TaskInterface;
    CTaskSA * Parent; // if the task was setup through an external source, this isn't going to be correct
    bool m_bBeingDestroyed;

public:
                        CTaskSA                 ( void );
                        ~CTaskSA                ( void );

    CTask*              Clone                   ( void );
    void                SetParent               ( CTask* pParent );
    CTask*              GetParent               ( void )                        { return Parent; }
    CTask*              GetSubTask              ( void );
    bool                IsSimpleTask            ( void );
    int                 GetTaskType             ( void );
    void                StopTimer               ( const CEvent* pEvent );
    bool                MakeAbortable           ( CPed* pPed, const int iPriority, const CEvent* pEvent );
    char*               GetTaskName             ( void );

    // our function(s)
    void                SetInterface            ( CTaskSAInterface* pInterface ) { TaskInterface = pInterface; };
    CTaskSAInterface*   GetInterface            ( void )                         {return this->TaskInterface;}
    bool                IsValid                 ( void )                         { return this->GetInterface() != NULL; }

    void                CreateTaskInterface     ( size_t nSize );

    void                SetAsPedTask            ( CPed * pPed, const int iTaskPriority, const bool bForceNewTask = false );
    void                SetAsSecondaryPedTask   ( CPed * pPed, const int iType );
    void                Destroy                 ( void );
    void                DestroyJustThis         ( void );
};

union UCTask 
{
    CTask * pTask;
    CTaskSA * pTaskSA;
};

class CTaskSimpleSAInterface : public CTaskSAInterface
{
    public:
};

class CTaskSimpleSA : public virtual CTaskSA, public virtual CTaskSimple
{
public:
    CTaskSimpleSA ( ) {};

    bool ProcessPed(CPed* pPed);
    bool SetPedPosition(CPed *pPed);
};

class CTaskComplexSAInterface : public CTaskSAInterface
{
public:
    CTask* m_pSubTask;
};

class CTaskComplexSA : public virtual CTaskSA, public virtual CTaskComplex
{
public:
    CTaskComplexSA() {};

    void SetSubTask(CTask* pSubTask);
    CTask* CreateNextSubTask(CPed* pPed);
    CTask* CreateFirstSubTask(CPed* pPed);
    CTask* ControlSubTask(CPed* pPed);
};


//
// 'Safe' task news
// Will return NULL if the created task is not valid
//
template < class T >
static T* ValidNewTask ( T* pTask )
{
   if ( pTask->IsValid () )
        return pTask;
    delete pTask;
    return NULL;
}

template < class T >
static T* NewTask ( void )
{
    return ValidNewTask ( new T () );
}

template < class T, class A >
static T* NewTask ( const A& a )
{
    return ValidNewTask ( new T ( a ) );
}

template < class T, class A, class B >
static T* NewTask ( const A& a, const B& b )
{
    return ValidNewTask ( new T ( a, b ) );
}

template < class T, class A, class B, class C >
static T* NewTask ( const A& a, const B& b, const C& c )
{
    return ValidNewTask ( new T ( a, b, c ) );
}

template < class T, class A, class B, class C, class D >
static T* NewTask ( const A& a, const B& b, const C& c, const D& d )
{
    return ValidNewTask ( new T ( a, b, c, d ) );
}

template < class T, class A, class B, class C, class D, class E >
static T* NewTask ( const A& a, const B& b, const C& c, const D& d, const E& e )
{
    return ValidNewTask ( new T ( a, b, c, d, e ) );
}

template < class T, class A, class B, class C, class D, class E, class F >
static T* NewTask ( const A& a, const B& b, const C& c, const D& d, const E& e, const F& f )
{
    return ValidNewTask ( new T ( a, b, c, d, e, f ) );
}

template < class T, class A, class B, class C, class D, class E, class F, class G >
static T* NewTask ( const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g )
{
    return ValidNewTask ( new T ( a, b, c, d, e, f, g ) );
}

template < class T, class A, class B, class C, class D, class E, class F, class G, class H >
static T* NewTask ( const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h )
{
    return ValidNewTask ( new T ( a, b, c, d, e, f, g, h ) );
}

template < class T, class A, class B, class C, class D, class E, class F, class G, class H, class I >
static T* NewTask ( const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i )
{
    return ValidNewTask ( new T ( a, b, c, d, e, f, g, h, i ) );
}

template < class T, class A, class B, class C, class D, class E, class F, class G, class H, class I, class J >
static T* NewTask ( const A& a, const B& b, const C& c, const D& d, const E& e, const F& f, const G& g, const H& h, const I& i, const J& j )
{
    return ValidNewTask ( new T ( a, b, c, d, e, f, g, h, i, j ) );
}

#endif
