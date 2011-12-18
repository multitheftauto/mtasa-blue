/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayerTasks.cpp
*  PURPOSE:     Player tasks class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerTasks::CPlayerTasks ( void )
{
    memset ( m_PriorityTasks, 0, sizeof ( m_PriorityTasks ) );
    memset ( m_SecondaryTasks, 0, sizeof ( m_SecondaryTasks ) );
}


CPlayerTasks::~CPlayerTasks ( void )
{
    unsigned int i;
    for ( i = 0 ; i < TASK_PRIORITY_MAX ; i++ )
        delete m_PriorityTasks [ i ];
    for ( i = 0 ; i < TASK_SECONDARY_MAX ; i++ )
        delete m_SecondaryTasks [ i ];
}


CPlayerTask* CPlayerTasks::GetPriorityTask ( unsigned int uiTaskType )
{
    if ( uiTaskType < TASK_PRIORITY_MAX )
    {
        return m_PriorityTasks [ uiTaskType ];
    }

    return NULL;
}


bool CPlayerTasks::SetPriorityTask ( unsigned int uiTaskType, CPlayerTask* pTask )
{
    if ( uiTaskType < TASK_PRIORITY_MAX )
    {
        CPlayerTask* pCurrentTask = m_PriorityTasks [ uiTaskType ];
        if ( pCurrentTask )
        {
            delete pCurrentTask;
        }

        m_PriorityTasks [ uiTaskType ] = pTask;

        return true;
    }

    return false;
}


CPlayerTask* CPlayerTasks::GetSecondaryTask ( unsigned int uiTaskType )
{
    if ( uiTaskType < TASK_SECONDARY_MAX )
    {
        return m_SecondaryTasks [ uiTaskType ];
    }

    return NULL;
}


bool CPlayerTasks::SetSecondaryTask ( unsigned int uiTaskType, CPlayerTask* pTask )
{
    if ( uiTaskType < TASK_SECONDARY_MAX )
    {
        CPlayerTask* pCurrentTask = m_SecondaryTasks [ uiTaskType ];
        if ( pCurrentTask )
        {
            delete pCurrentTask;
        }

        m_SecondaryTasks [ uiTaskType ] = pTask;

        return true;
    }

    return false;
}

