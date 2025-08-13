/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPadSA.cpp
 *  PURPOSE:     Controller pad input logic
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPadSA.h"

CControllerState* CPadSA::GetCurrentControllerState(CControllerState* ControllerState)
{
    MemCpyFast(ControllerState, &internalInterface->NewState, sizeof(CControllerState));
    return ControllerState;
}

CControllerState* CPadSA::GetLastControllerState(CControllerState* ControllerState)
{
    MemCpyFast(ControllerState, &internalInterface->OldState, sizeof(CControllerState));
    return ControllerState;
}

void CPadSA::SetCurrentControllerState(CControllerState* ControllerState)
{
    MemCpyFast(&internalInterface->NewState, ControllerState, sizeof(CControllerState));
}

void CPadSA::SetLastControllerState(CControllerState* ControllerState)
{
    MemCpyFast(&internalInterface->OldState, ControllerState, sizeof(CControllerState));
}

void CPadSA::Store()
{
    MemCpyFast(&StoredPad, internalInterface, sizeof(CPadSAInterface));
}

void CPadSA::Restore()
{
    MemCpyFast(internalInterface, &StoredPad, sizeof(CPadSAInterface));
}

bool CPadSA::IsEnabled()
{
    bool bEnabled = *(BYTE*)FUNC_CPad_UpdatePads == 0x56;
    return bEnabled;
}

void CPadSA::Disable(bool bDisable)
{
    if (bDisable)
        MemPut<BYTE>(FUNC_CPad_UpdatePads, 0xC3);
    else
        MemPut<BYTE>(FUNC_CPad_UpdatePads, 0x56);

    // this->internalInterface->DisablePlayerControls = bDisable;
}

void CPadSA::Clear()
{
    CControllerState cs;            // create a null controller (class is inited to null)
    SetCurrentControllerState(&cs);
    SetLastControllerState(&cs);
}

void CPadSA::SetHornHistoryValue(bool value)
{
    internalInterface->iCurrHornHistory++;
    if (internalInterface->iCurrHornHistory >= MAX_HORN_HISTORY)
        internalInterface->iCurrHornHistory = 0;
    internalInterface->bHornHistory[internalInterface->iCurrHornHistory] = value;
}

void CPadSA::SetLastTimeTouched(DWORD dwTime)
{
    internalInterface->LastTimeTouched = dwTime;
}
