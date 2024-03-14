/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPlayerTasks.cpp
 *  PURPOSE:     Player tasks class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerTasks.h"

CPlayerTasks::CPlayerTasks()
{
    memset(&m_PriorityTasks[0], 0, sizeof(m_PriorityTasks));
    memset(&m_SecondaryTasks[0], 0, sizeof(m_SecondaryTasks));
}

CPlayerTasks::~CPlayerTasks()
{
    std::uint32_t i;
    for (i = 0; i < TASK_PRIORITY_MAX; i++)
        delete m_PriorityTasks[i];
    for (i = 0; i < TASK_SECONDARY_MAX; i++)
        delete m_SecondaryTasks[i];
}

CPlayerTask* CPlayerTasks::GetPriorityTask(std::uint32_t uiTaskType)
{
    if (uiTaskType < TASK_PRIORITY_MAX)
    {
        return m_PriorityTasks[uiTaskType];
    }

    return NULL;
}

bool CPlayerTasks::SetPriorityTask(std::uint32_t uiTaskType, CPlayerTask* pTask)
{
    if (uiTaskType < TASK_PRIORITY_MAX)
    {
        CPlayerTask* pCurrentTask = m_PriorityTasks[uiTaskType];
        if (pCurrentTask)
        {
            delete pCurrentTask;
        }

        m_PriorityTasks[uiTaskType] = pTask;

        return true;
    }

    return false;
}

CPlayerTask* CPlayerTasks::GetSecondaryTask(std::uint32_t uiTaskType)
{
    if (uiTaskType < TASK_SECONDARY_MAX)
    {
        return m_SecondaryTasks[uiTaskType];
    }

    return NULL;
}

bool CPlayerTasks::SetSecondaryTask(std::uint32_t uiTaskType, CPlayerTask* pTask)
{
    if (uiTaskType < TASK_SECONDARY_MAX)
    {
        CPlayerTask* pCurrentTask = m_SecondaryTasks[uiTaskType];
        if (pCurrentTask)
        {
            delete pCurrentTask;
        }

        m_SecondaryTasks[uiTaskType] = pTask;

        return true;
    }

    return false;
}
