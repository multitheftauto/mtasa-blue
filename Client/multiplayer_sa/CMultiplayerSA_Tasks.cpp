/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Tasks.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTaskSimplePlayerOnFoot::MakeAbortable
//
// If ignorefirestate is enabled, we need this hook to avoid tweaking the aiming animation
// and the chainsaw turning off when entering fire
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool __IsIgnoreFireStateEnabled()
{
    return pGameInterface->IsIgnoreFireStateEnabled();
}

#define HOOKPOS_CTaskSimplePlayerOnFoot__MakeAbortable  0x68584D
#define HOOKSIZE_CTaskSimplePlayerOnFoot__MakeAbortable 6
static constexpr std::uintptr_t RETURN_CTaskSimplePlayerOnFoot__MakeAbortable = 0x68585F;
static constexpr std::uintptr_t SKIP_CTaskSimplePlayerOnFoot__MakeAbortable = 0x685855;
static void _declspec(naked) HOOK_CTaskSimplePlayerOnFoot__MakeAbortable()
{
    _asm
    {
        // return false and keep task alive
        call dword ptr [eax+8]
        cmp eax, 3Dh
        jl skip

        // if eventPriority == 66 (EventOnFire) && IsIgnoreFireStateEnabled()
        cmp eax, 42h
        jne continue_logic

        call __IsIgnoreFireStateEnabled
        test al, al
        jz continue_logic

        // return true but keep task alive
        pop edi
        pop esi
        pop ebx
        mov al, 1
        retn 0Ch

        continue_logic:
        jmp RETURN_CTaskSimplePlayerOnFoot__MakeAbortable

        skip:
        jmp SKIP_CTaskSimplePlayerOnFoot__MakeAbortable
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Tasks
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Tasks()
{
    EZHookInstall(CTaskSimplePlayerOnFoot__MakeAbortable);
}
