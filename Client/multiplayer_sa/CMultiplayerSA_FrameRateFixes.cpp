/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_FrameRateFixes.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

const float kOriginalTimeStep = 50.0f / 30.0f;

#define HOOKPOS_CTaskSimpleUseGun__ControlGunMove 0x61E0C0
#define HOOKSIZE_CTaskSimpleUseGun__ControlGunMove 0x10
const unsigned int RETURN_CTaskSimpleUseGun__ControlGunMove = 0x61E0D0;

void _declspec(naked) HOOK_CTaskSimpleUseGun__ControlGunMove()
{
    _asm
    {
        fld ds:[0xB7CB5C]       // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep  // 1.666f
        fdivr ds:[0x858CA8]     // 0.07f
        fmul ds:[0xB7CB5C]      // CTimer::ms_fTimeStep
        mov edx, [esp + 0x4]
        jmp RETURN_CTaskSimpleUseGun__ControlGunMove
    }
}

void CMultiplayerSA::InitHooks_FrameRateFixes()
{
    EZHookInstall(CTaskSimpleUseGun__ControlGunMove);
}
