/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/Task.h
*  PURPOSE:		Task interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_TASK
#define __CGAME_TASK

#include "TaskTypes.h"

// these will end up in an enum once we have it
//#define TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER	811
//#define TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER	812
//#define TASK_SIMPLE_SET_PED_OUT					816
#define TASK_SIMPLE_DUCK                415

#include "CPed.h"

class CEvent;

#define NO_TASK_TYPE            9999

class CTask
{
public:
    virtual                     ~CTask ( void ) {};

	virtual CTask * Clone()=0;
	virtual void SetParent(CTask* pParent)=0;
	virtual CTask * GetParent()=0;
	virtual CTask * GetSubTask()=0;
	virtual bool IsSimpleTask()=0;
	virtual int GetTaskType()=0; // returns NO_TASK_TYPE if no task type exists
	virtual void StopTimer(const CEvent* pEvent)=0;
	virtual bool MakeAbortable(CPed* pPed, const int iPriority, const CEvent* pEvent)=0;
    virtual char * GetTaskName()=0;
    virtual class CTaskSAInterface* GetInterface ( void )=0;
    virtual void SetAsPedTask ( CPed * pPed, const int iTaskPriority, const bool bForceNewTask = false )=0;
    virtual void SetAsSecondaryPedTask ( CPed * pPed, const int iType )=0;

    virtual void Destroy()=0;
    virtual void DestroyJustThis()=0;
};

class CTaskSimple : public virtual CTask
{
public:
    virtual      ~CTaskSimple ( void ) {};

	virtual bool ProcessPed(CPed* pPed)=0;
	virtual bool SetPedPosition(CPed *pPed)=0;
};

class CTaskComplex : public virtual CTask
{
public:
    virtual      ~CTaskComplex ( void ) {};

	virtual void SetSubTask(CTask* pSubTask)=0;
	virtual CTask * CreateNextSubTask(CPed* pPed)=0;
	virtual CTask * CreateFirstSubTask(CPed* pPed)=0;
	virtual CTask * ControlSubTask(CPed* pPed)=0;
};

#endif