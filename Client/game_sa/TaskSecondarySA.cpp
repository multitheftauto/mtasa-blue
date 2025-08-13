/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskSecondarySA.cpp
 *  PURPOSE:     Secondary game tasks
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "TaskSecondarySA.h"

// ##############################################################################
// ## Name:    CTaskSimpleDuck
// ## Purpose: Make the ped duck
// ##############################################################################

CTaskSimpleDuckSA::CTaskSimpleDuckSA(eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck, short nUseShotsWhizzingEvents)
{
    DWORD dwFunc = FUNC_CTaskSimpleDuck__Constructor;
    CreateTaskInterface(sizeof(CTaskSimpleDuckSAInterface));
    if (!IsValid())
        return;
    DWORD dwThisInterface = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThisInterface
        push    ebx
        mov     bx, nUseShotsWhizzingEvents
        push    ebx
        mov     bx, nLengthOfDuck
        push    ebx
        push    nDuckControl
        call    dwFunc
        pop     ebx
    }
}
