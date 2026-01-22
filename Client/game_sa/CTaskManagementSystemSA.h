/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTaskManagementSystemSA.h
 *  PURPOSE:     Header file for task management system class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CTaskSA;
class CTaskSAInterface;

#define FUNC_CTask_Operator_Delete          0x61A5B0

class CTaskManagementSystemSA
{
public:
    CTaskManagementSystemSA();
    ~CTaskManagementSystemSA();

    CTaskSA* AddTask(CTaskSA* pTask);
    void     RemoveTask(CTaskSAInterface* pTaskInterface);
    CTaskSA* GetTask(CTaskSAInterface* pTaskInterface);
    CTaskSA* CreateAppropriateTask(CTaskSAInterface* pTaskInterface, int iTaskType);

private:
    struct STaskListItem
    {
        CTaskSAInterface* taskInterface;
        CTaskSA*          pTaskSA;
    };

    std::list<STaskListItem*> m_TaskList;
};
