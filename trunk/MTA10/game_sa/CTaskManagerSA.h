/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTaskManagerSA.h
*  PURPOSE:     Header file for task manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASK_MANAGER
#define __CGAMESA_TASK_MANAGER

#include <game/CTaskManager.h>
#include "Common.h"

class CVehicle;
class CTaskSAInterface;

//#define TASK_PRIORITY_MAX		5
//#define TASK_SECONDARY_MAX		6

// functions
#define FUNC_SetTask					0x681AF0
#define FUNC_GetActiveTask				0x681720
#define FUNC_GetSimplestActiveTask		0x6819D0
//#define FUNC_GetSimplestTask			0x681970 // static
#define FUNC_GetSimplestTask			0x681A00
#define FUNC_FindActiveTaskByType		0x681740
#define FUNC_FindTaskByType				0x6817D0
#define FUNC_SetTaskSecondary			0x681B60
#define FUNC_ClearTaskEventResponse		0x681BD0
#define FUNC_HasTaskSecondary			0x681820

#include "CTaskManagementSystemSA.h"

// task headers
#include "TaskCarAccessoriesSA.h"
#include "TaskGoToSA.h"
#include "TaskCarSA.h"
#include "TaskJumpFallSA.h"
#include "TaskSecondarySA.h"
#include "TaskPhysicalResponseSA.h"
#include "TaskIKSA.h"

// pointer for this at +1148+4? (see 0x568A20)
class CTaskManagerSAInterface
{
public:
    CTaskSAInterface	* m_tasks[TASK_PRIORITY_MAX];  
    CTaskSAInterface	* m_tasksSecondary[TASK_SECONDARY_MAX];

    CPedSAInterface		* m_pPed;    
};

class CTaskManagerSA : public CTaskManager
{
private:
	CTaskManagerSAInterface * internalInterface;
	CPed * ped;
    CTaskManagementSystemSA * m_pTaskManagementSystem;
public:
	CTaskManagerSA(CTaskManagerSAInterface * taskManagerInterface, CPed * ped);
	CTaskManagerSAInterface * GetInterface() { return this->internalInterface; }

    void RemoveTask(const int iTaskPriority);
	void SetTask(CTaskSA* pTaskPrimary, const int iTaskPriority, const bool bForceNewTask = false);
    CTask* GetTask(const int iTaskPriority);
	CTask* GetActiveTask();
	CTask* GetSimplestActiveTask();
	CTask* GetSimplestTask(const int iPriority);
	CTask* FindActiveTaskByType(const int iTaskType);
	CTask* FindTaskByType(const int iPriority, const int iTaskType);
    void RemoveTaskSecondary(const int iTaskPriority);
	void SetTaskSecondary(CTaskSA* pTaskSecondary, const int iType);
	CTask* GetTaskSecondary(const int iType); // code it
	bool HasTaskSecondary(const CTask* pTaskSecondary); // code it
	void ClearTaskEventResponse();
    void Flush(const int iPriority);
};

#endif