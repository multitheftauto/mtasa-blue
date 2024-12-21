/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/Task.h
 *  PURPOSE:     Task interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "TaskTypes.h"

class CEvent;
class CPed;
class CTaskSAInterface;

#define NO_TASK_TYPE            9999

class CTask
{
public:
    virtual ~CTask(){};

    virtual CTask*                  Clone() = 0;
    virtual void                    SetParent(CTask* pParent) = 0;
    virtual CTask*                  GetParent() = 0;
    virtual CTask*                  GetSubTask() = 0;
    virtual bool                    IsSimpleTask() = 0;
    virtual int                     GetTaskType() = 0;            // returns NO_TASK_TYPE if no task type exists
    virtual void                    StopTimer(const CEvent* pEvent) = 0;
    virtual bool                    MakeAbortable(CPed* pPed, const int iPriority, const CEvent* pEvent) = 0;
    virtual const char*             GetTaskName() = 0;
    virtual class CTaskSAInterface* GetInterface() noexcept = 0;
    virtual const class CTaskSAInterface* GetInterface() const noexcept = 0;
    virtual bool                    IsValid() = 0;
    virtual void                    SetAsPedTask(CPed* pPed, const int iTaskPriority, const bool bForceNewTask = false) = 0;
    virtual void                    SetAsSecondaryPedTask(CPed* pPed, const int iType) = 0;

    virtual void Destroy() = 0;
    virtual void DestroyJustThis() = 0;
};

class CTaskSimple : public virtual CTask
{
public:
    virtual ~CTaskSimple(){};

    virtual bool ProcessPed(CPed* pPed) = 0;
    virtual bool SetPedPosition(CPed* pPed) = 0;
};

class CTaskComplex : public virtual CTask
{
public:
    virtual ~CTaskComplex(){};

    virtual void   SetSubTask(CTask* pSubTask) = 0;
    virtual CTask* CreateNextSubTask(CPed* pPed) = 0;
    virtual CTask* CreateFirstSubTask(CPed* pPed) = 0;
    virtual CTask* ControlSubTask(CPed* pPed) = 0;
};
