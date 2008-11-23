/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/TaskSA.h
*  PURPOSE:		Base game task
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
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

#define FUNC_CTask__Operator_New			0x61A5A0
#define FUNC_CTask__Operator_Delete			0x61A5B0

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


#endif