/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CTaskManagerSA.h
 *  PURPOSE:     Header file for task manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CTaskManager.h>

class CTaskSAInterface;
class CTaskManagementSystemSA;
class CTaskSA;

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
    CPed*                    m_ped;
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
    void   SetTaskSecondary(CTaskSA* pTaskSecondary, const int iType);
    CTask* GetTaskSecondary(const int iType);                        // code it
    bool   HasTaskSecondary(const CTask* pTaskSecondary);            // code it
    void   ClearTaskEventResponse();
    void   Flush(const int iPriority);
};
