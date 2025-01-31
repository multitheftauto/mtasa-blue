/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTaskManagerSA.h
 *  PURPOSE:     Header file for task manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CTaskManager.h>

class CTaskSAInterface;
class CTaskManagementSystemSA;
class CTaskSA;

#define FUNC_SetTask                    0x681AF0
#define FUNC_GetActiveTask              0x681720
#define FUNC_GetSimplestActiveTask      0x6819D0
#define FUNC_GetSimplestTask            0x681A00
#define FUNC_FindActiveTaskByType       0x681740
#define FUNC_FindTaskByType             0x6817D0
#define FUNC_SetTaskSecondary           0x681B60
#define FUNC_ClearTaskEventResponse     0x681BD0
#define FUNC_HasTaskSecondary           0x681820

// pointer for this at +1148+4? (see 0x568A20)
class CTaskManagerSAInterface
{
public:
    CTaskSAInterface* m_tasks[TASK_PRIORITY_MAX];
    CTaskSAInterface* m_tasksSecondary[TASK_SECONDARY_MAX];

    CPedSAInterface* m_pPed;
};

class CTaskManagerSA : public CTaskManager
{
private:
    CTaskManagerSAInterface* internalInterface;
    CPed*                    ped;
    CTaskManagementSystemSA* m_pTaskManagementSystem;

public:
    CTaskManagerSA(CTaskManagerSAInterface* taskManagerInterface, CPed* ped);
    CTaskManagerSAInterface* GetInterface() { return internalInterface; }

    void   RemoveTask(const int iTaskPriority);
    void   SetTask(CTaskSA* pTaskPrimary, const int iTaskPriority, const bool bForceNewTask = false);
    CTask* GetTask(const int iTaskPriority);
    CTask* GetActiveTask();
    CTask* GetSimplestActiveTask();
    CTask* GetSimplestTask(const int iPriority);
    CTask* FindActiveTaskByType(const int iTaskType);
    CTask* FindTaskByType(const int iPriority, const int iTaskType);
    void   RemoveTaskSecondary(const int iTaskPriority);
    bool   RemoveTaskSecondary(const int taskPriority, const int taskType);
    void   SetTaskSecondary(CTaskSA* pTaskSecondary, const int iType);
    CTask* GetTaskSecondary(const int iType);                        // code it
    bool   HasTaskSecondary(const CTask* pTaskSecondary);            // code it
    void   ClearTaskEventResponse();
    void   Flush(const int iPriority);
};
