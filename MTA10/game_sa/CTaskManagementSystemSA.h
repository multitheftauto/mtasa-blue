/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTaskManagementSystemSA.h
*  PURPOSE:     Header file for task management system class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CTaskManagementSystemSA;

#ifndef __CGAMESA_TASKSYSTEM
#define __CGAMESA_TASKSYSTEM

#include "TaskSA.h"
#include "TaskCarAccessoriesSA.h"
#include "TaskCarSA.h"
#include <game/CTaskManagementSystem.h>
#include <list>

using namespace std;

#define FUNC_CTask_Operator_Delete          0x61A5B0

class CTaskManagementSystemSA : public CTaskManagementSystem
{
public:
                                CTaskManagementSystemSA             ( void );
                                ~CTaskManagementSystemSA            ( void );

    CTask *                     AddTask                             ( CTaskSA * pTask );
    void                        RemoveTask                          ( CTaskSAInterface * pTaskInterface );
    CTask *                     GetTask                             ( CTaskSAInterface * pTaskInterface );
    CTask *                     CreateAppropriateTask               ( CTaskSAInterface * pTaskInterface, int iTaskType );

private:
    struct STaskListItem
    {
	    CTaskSAInterface*   taskInterface;
        CTaskSA*            pTaskSA;
    };

    list < STaskListItem* >     m_TaskList;
};

#endif
