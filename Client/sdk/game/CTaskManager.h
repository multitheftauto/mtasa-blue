/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CTaskManager.h
 *  PURPOSE:     Task manager interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CTask;

enum
{
    TASK_PRIORITY_PHYSICAL_RESPONSE = 0,
    TASK_PRIORITY_EVENT_RESPONSE_TEMP,
    TASK_PRIORITY_EVENT_RESPONSE_NONTEMP,
    TASK_PRIORITY_PRIMARY,
    TASK_PRIORITY_DEFAULT,
    TASK_PRIORITY_MAX
};

enum eSecondaryTaskType
{
    TASK_SECONDARY_ATTACK = 0,            // want duck to be after attack
    TASK_SECONDARY_DUCK,                  // because attack controls ducking movement
    TASK_SECONDARY_SAY,
    TASK_SECONDARY_FACIAL_COMPLEX,
    TASK_SECONDARY_PARTIAL_ANIM,
    TASK_SECONDARY_IK,
    TASK_SECONDARY_MAX
};

enum
{
    ABORT_PRIORITY_LEISURE = 0,
    ABORT_PRIORITY_URGENT,
    ABORT_PRIORITY_IMMEDIATE
};

enum taskType
{
    PRIMARY_TASK = 0,
    SECONDARY_TASK
};


class CTaskManager
{
public:
    virtual void RemoveTask(const int iTaskPriority) = 0;
    // virtual void SetTask(CTaskSA* pTaskPrimary, const int iTaskPriority, const bool bForceNewTask = false)=0;
    virtual CTask* GetTask(const int iTaskPriority) = 0;
    virtual CTask* GetActiveTask() = 0;
    virtual CTask* GetSimplestActiveTask() = 0;
    virtual CTask* GetSimplestTask(const int iPriority) = 0;
    virtual CTask* FindActiveTaskByType(const int iTaskType) = 0;
    virtual CTask* FindTaskByType(const int iPriority, const int iTaskType) = 0;
    virtual void   RemoveTaskSecondary(const int iTaskPriority) = 0;
    virtual bool   RemoveTaskSecondary(const int taskPriority, const int taskType) = 0;
    // virtual void SetTaskSecondary(CTask* pTaskSecondary, const int iType)=0;
    virtual CTask* GetTaskSecondary(const int iType) = 0;
    virtual bool   HasTaskSecondary(const CTask* pTaskSecondary) = 0;
    virtual void   ClearTaskEventResponse() = 0;
    virtual void   Flush(const int iPriority) = 0;
};
