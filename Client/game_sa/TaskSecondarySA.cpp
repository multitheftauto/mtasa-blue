/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskSecondarySA.cpp
 *  PURPOSE:     Secondary game tasks
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// ##############################################################################
// ## Name:    CTaskSimpleDuck
// ## Purpose: Make the ped duck
// ##############################################################################

CTaskSimpleDuckSA::CTaskSimpleDuckSA(eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck, short nUseShotsWhizzingEvents)
{
    DWORD dwFunc = FUNC_CTaskSimpleDuck__Constructor;
    this->CreateTaskInterface(sizeof(CTaskSimpleDuckSAInterface));
    if (!IsValid())
        return;

    // CTaskSimpleDuck::CTaskSimpleDuck
    ((void*(__thiscall*)(void*, eDuckControlTypes, unsigned short, short))FUNC_CTaskSimpleDuck__Constructor)(this->GetInterface(), nDuckControl, nLengthOfDuck,
                                                                                                             nUseShotsWhizzingEvents);
}
