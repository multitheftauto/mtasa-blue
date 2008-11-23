/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayerTasks.h
*  PURPOSE:     Player tasks class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPlayerTasks;

#ifndef __CPlayerTasks_H
#define __CPlayerTasks_H

#include <string.h>
#include "TaskNames.h"

class CPlayerTask;

enum 
{
    TASK_PRIORITY_PHYSICAL_RESPONSE=0,
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
                                CPlayerTasks                ( void );
                                ~CPlayerTasks               ( void );

    CPlayerTask*                GetPriorityTask             ( unsigned int uiTaskType );
    bool                        SetPriorityTask             ( unsigned int uiTaskType, CPlayerTask* pTask );
    
    CPlayerTask*                GetSecondaryTask            ( unsigned int uiTaskType );
    bool                        SetSecondaryTask            ( unsigned int uiTaskType, CPlayerTask* pTask );
    
protected:
    CPlayerTask*                m_PriorityTasks [ TASK_PRIORITY_MAX ];
    CPlayerTask*                m_SecondaryTasks [ TASK_SECONDARY_MAX ];
};

class CPlayerTask
{
public:
    inline                      CPlayerTask                 ( void )                    { m_usID = 0xFFFF; m_pSubTask = NULL; }
    inline                      CPlayerTask                 ( unsigned short usID )     { m_usID = usID; m_pSubTask = NULL; }
    inline                      ~CPlayerTask                ( void )                    { delete m_pSubTask; }

    inline unsigned short       GetID                       ( void )                    { return m_usID; }
    inline char*                GetName                     ( void )                    { return ( m_usID < 1802 ) ? TaskNames [ m_usID ].szName : sNoTaskName; }
    inline CPlayerTask*         GetSubTask                  ( void )                    { return m_pSubTask; }
    inline void                 SetSubTask                  ( CPlayerTask* pTask )      { m_pSubTask = pTask; }

protected:
    unsigned short              m_usID;
    CPlayerTask*                m_pSubTask;
};

#endif
