/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPlayerTasks.h
 *  PURPOSE:     Player tasks class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPlayerTasks;

#pragma once

#include <string.h>
#include "TaskNames.h"

class CPlayerTask;

enum
{
    TASK_PRIORITY_PHYSICAL_RESPONSE = 0,
    TASK_PRIORITY_EVENT_RESPONSE_TEMP,
    TASK_PRIORITY_EVENT_RESPONSE_NONTEMP,
    TASK_PRIORITY_PRIMARY,
    TASK_PRIORITY_DEFAULT,
    TASK_PRIORITY_MAX
};

enum
{
    TASK_SECONDARY_ATTACK,
    TASK_SECONDARY_DUCK,
    TASK_SECONDARY_SAY,
    TASK_SECONDARY_FACIAL_COMPLEX,
    TASK_SECONDARY_PARTIAL_ANIM,
    TASK_SECONDARY_IK,
    TASK_SECONDARY_MAX
};

class CPlayerTasks
{
public:
    CPlayerTasks();
    ~CPlayerTasks();

    CPlayerTask* GetPriorityTask(unsigned int uiTaskType);
    bool         SetPriorityTask(unsigned int uiTaskType, CPlayerTask* pTask);

    CPlayerTask* GetSecondaryTask(unsigned int uiTaskType);
    bool         SetSecondaryTask(unsigned int uiTaskType, CPlayerTask* pTask);

protected:
    SFixedArray<CPlayerTask*, TASK_PRIORITY_MAX>  m_PriorityTasks;
    SFixedArray<CPlayerTask*, TASK_SECONDARY_MAX> m_SecondaryTasks;
};

class CPlayerTask
{
public:
    CPlayerTask()
    {
        m_usID = 0xFFFF;
        m_pSubTask = NULL;
    }
    CPlayerTask(unsigned short usID)
    {
        m_usID = usID;
        m_pSubTask = NULL;
    }
    ~CPlayerTask() { delete m_pSubTask; }

    unsigned short GetID() { return m_usID; }
    char*          GetName() { return (m_usID < 1802) ? TaskNames[m_usID].szName : sNoTaskName; }
    CPlayerTask*   GetSubTask() { return m_pSubTask; }
    void           SetSubTask(CPlayerTask* pTask) { m_pSubTask = pTask; }

protected:
    unsigned short m_usID;
    CPlayerTask*   m_pSubTask;
};
