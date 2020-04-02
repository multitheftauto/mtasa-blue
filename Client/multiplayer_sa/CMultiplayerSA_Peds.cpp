/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Peds.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPed::DoFootLanded
//
// Whenever a pedestrian's foot hits the ground
//
//////////////////////////////////////////////////////////////////////////////////////////
static PedStepHandler* pPedStepHandler = nullptr;

void CMultiplayerSA::SetPedStepHandler(PedStepHandler* pHandler)
{
    pPedStepHandler = pHandler;
}

static void __cdecl DoFootLanded(CPedSAInterface* pPedSAInterface, short footId, char unknown1)
{
    if (pPedStepHandler)
        pPedStepHandler(pPedSAInterface, footId == LANDED_PED_LEFT_FOOT ? true : false);
}

#define HOOKPOS_CPed_DoFootLanded               0x5E5380
#define HOOKSIZE_CPed_DoFootLanded              6
static const DWORD CONTINUE_CPed_DoFootLanded = 0x5E5386;

void _declspec(naked) HOOK_CPed_DoFootLanded()
{
    _asm
    {
        pushad
        push    [esp + 32 + 4]
        push    [esp + 32 + 8]
        push    ecx
        call    DoFootLanded
        add     esp, 12
        popad

        // Continue
        sub     esp, 30h
        push    esi
        mov     esi, ecx
        jmp     CONTINUE_CPed_DoFootLanded
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Peds
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Peds()
{
    EZHookInstall(CPed_DoFootLanded);
}
