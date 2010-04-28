/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskSecondarySA.cpp
*  PURPOSE:     Secondary game tasks
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// ##############################################################################
// ## Name:    CTaskSimpleDuck                                    
// ## Purpose: Make the ped duck
// ##############################################################################

CTaskSimpleDuckSA::CTaskSimpleDuckSA ( eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck, short nUseShotsWhizzingEvents )
{
    DWORD dwFunc = FUNC_CTaskSimpleDuck__Constructor;
    this->CreateTaskInterface(sizeof(CTaskSimpleDuckSAInterface));
    DWORD dwThisInterface = (DWORD)this->GetInterface();
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
