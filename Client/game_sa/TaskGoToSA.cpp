/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskGoToSA.cpp
 *  PURPOSE:     Go-to game tasks
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "TaskGoToSA.h"

// ##############################################################################
// ## Name:    CTaskComplexWander
// ## Purpose: Generic task that makes peds wander around. Can't be used
// ##          directly, use a superclass of this instead.
// ##############################################################################

int CTaskComplexWanderSA::GetWanderType()
{
    CTaskSAInterface* pTaskInterface = GetInterface();
    DWORD             dwFunc = ((TaskComplexWanderVTBL*)pTaskInterface->VTBL)->GetWanderType;
    int               iReturn = NO_WANDER_TYPE;

    if (dwFunc && dwFunc != 0x82263A)  // some tasks have no wander type 0x82263A is purecal (assert?)
    {
        // clang-format off
        iReturn = gta_thiscall_address<decltype(iReturn)>(dwFunc, pTaskInterface);
        // clang-format on
    }
    return iReturn;
}

CNodeAddress* CTaskComplexWanderSA::GetNextNode()
{
    return &((CTaskComplexWanderSAInterface*)GetInterface())->m_NextNode;
}

CNodeAddress* CTaskComplexWanderSA::GetLastNode()
{
    return &((CTaskComplexWanderSAInterface*)GetInterface())->m_LastNode;
}

// ##############################################################################
// ## Name:    CTaskComplexWanderStandard
// ## Purpose: Standard class used for making normal peds wander around
// ##############################################################################

CTaskComplexWanderStandardSA::CTaskComplexWanderStandardSA(const int iMoveState, const unsigned char iDir, const bool bWanderSensibly)
{
    CreateTaskInterface(sizeof(CTaskComplexWanderStandardSAInterface));
    if (!IsValid())
        return;
    DWORD dwFunc = FUNC_CTaskComplexWanderStandard__Constructor;
    DWORD dwThisInterface = (DWORD)GetInterface();
    // clang-format off
    gta_thiscall_address(dwFunc, dwThisInterface, iMoveState, iDir, bWanderSensibly);
    // clang-format on
}
