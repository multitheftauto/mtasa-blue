/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskGoToSA.cpp
*  PURPOSE:     Go-to game tasks
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// ##############################################################################
// ## Name:    CTaskComplexWander                                    
// ## Purpose: Generic task that makes peds wander around. Can't be used 
// ##          directly, use a superclass of this instead.
// ##############################################################################

int CTaskComplexWanderSA::GetWanderType()
{
    DEBUG_TRACE("int CTaskComplexWander::GetWanderType()");
    CTaskSAInterface * pTaskInterface = this->GetInterface();
    DWORD dwFunc = ((TaskComplexWanderVTBL *)pTaskInterface->VTBL)->GetWanderType;
    int iReturn = NO_WANDER_TYPE;

    if ( dwFunc && dwFunc != 0x82263A ) // some tasks have no wander type 0x82263A is purecal (assert?)
    {
        _asm
        {
            mov     ecx, pTaskInterface
            call    dwFunc
            mov     iReturn, eax
        }
    }
    return iReturn;
}

CNodeAddress * CTaskComplexWanderSA::GetNextNode()
{
    return &((CTaskComplexWanderSAInterface *)this->GetInterface())->m_NextNode;
}

CNodeAddress * CTaskComplexWanderSA::GetLastNode()
{
    return &((CTaskComplexWanderSAInterface *)this->GetInterface())->m_LastNode;
}


// ##############################################################################
// ## Name:    CTaskComplexWanderStandard                                    
// ## Purpose: Standard class used for making normal peds wander around
// ##############################################################################

CTaskComplexWanderStandardSA::CTaskComplexWanderStandardSA(const int iMoveState, const unsigned char iDir, const bool bWanderSensibly)
{
    DEBUG_TRACE("CTaskComplexWanderStandardSA::CTaskComplexWanderStandardSA(const int iMoveState, const unsigned char iDir, const bool bWanderSensibly)");
    this->CreateTaskInterface(sizeof(CTaskComplexWanderStandardSAInterface));
    DWORD dwFunc = FUNC_CTaskComplexWanderStandard__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    bWanderSensibly
        push    iDir
        push    iMoveState
        call    dwFunc
    }
}
