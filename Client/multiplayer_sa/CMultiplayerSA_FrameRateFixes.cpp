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

static bool bWouldBeNewFrame = false;
static unsigned int nLastFrameTime = 0;

constexpr float kOriginalTimeStep = 50.0f / 30.0f;

#define HOOKPOS_CTaskSimpleUseGun__SetMoveAnim 0x61E4F2
#define HOOKSIZE_CTaskSimpleUseGun__SetMoveAnim 0x6
const unsigned int RETURN_CTaskSimpleUseGun__SetMoveAnim = 0x61E4F8;

void _declspec(naked) HOOK_CTaskSimpleUseGun__SetMoveAnim()
{
    _asm {
        fld ds:[0xB7CB5C]           // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fmul ds:[0x858B1C]          // 0.1f
        fxch
        fcom
        fxch
        fstp st(0)
        jmp RETURN_CTaskSimpleUseGun__SetMoveAnim
    }
}

#define HOOKPOS_CTimer__Update 0x561C5D
#define HOOKSIZE_CTimer__Update 0xE
void _declspec(naked) HOOK_CTimer__Update()
{
    _asm {
        add esp, 0x4

        mov bWouldBeNewFrame, 0
        mov eax, nLastFrameTime
        add eax, 33                 // 33 = 1000 / 30
        mov ecx, ds:[0xB7CB84]      // CTimer::m_snTimeInMilliseconds
        cmp ecx, eax
        jb skip

        mov bWouldBeNewFrame, 1
        mov nLastFrameTime, ecx
        mov eax, ds:[0xB7CB4C]      // CTimer::m_FrameCounter
        inc eax
        mov ds:[0xB7CB4C], eax      // CTimer::m_FrameCounter

    skip:
        add esp, 0xC
        ret
    }
}

#define HOOKPOS_BreakObject_c__Update  0x59E420
#define HOOKSIZE_BreakObject_c__Update 0xB
const unsigned int RETURN_BreakObject_c__Update = 0x59E42B;

void _declspec(naked) HOOK_BreakObject_c__Update()
{
    _asm {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        mov edx, [edi+eax+0x70]
        lea eax, [edi+eax+0x70]
        dec edx
        mov [eax], edx

    skip:
        jmp RETURN_BreakObject_c__Update
    }
}

#define HOOKPOS_CProjectileInfo__Update 0x738C63
#define HOOKSIZE_CProjectileInfo__Update 0x5
const unsigned int RETURN_CProjectileInfo__Update = 0x738C68;
const unsigned int RETURN_CProjectileInfo__Update_SKIP = 0x738F22;
void _declspec(naked) HOOK_CProjectileInfo__Update()
{
    _asm {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        mov eax, [ebx]
        cmp eax, 0x13

        jmp RETURN_CProjectileInfo__Update
    skip:
        jmp RETURN_CProjectileInfo__Update_SKIP
    }
}

#define HOOKPOS_CVehicle__AddWheelDirtAndWater 0x6D2D50
#define HOOKSIZE_CVehicle__AddWheelDirtAndWater 0x6
const unsigned int RETURN_CVehicle__AddWheelDirtAndWater = 0x6D2D56;
void _declspec(naked) HOOK_CVehicle__AddWheelDirtAndWater()
{
    _asm {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        mov eax, [esp+0x8]
        test eax, eax

        jmp RETURN_CVehicle__AddWheelDirtAndWater
    skip:
        xor eax, eax
        retn 0x10
    }
}

#define HOOKPOS_CPlane__PreRender 0x6CA937
#define HOOKSIZE_CPlane__PreRender 0x6
const unsigned int RETURN_CPlane__PreRender = 0x6CA93D;
const unsigned int RETURN_CPlane__PreRender_SKIP = 0x6CAA93;

void _declspec(naked) HOOK_CPlane__PreRender()
{
    _asm {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        mov al, [esi+0xA00]

        jmp RETURN_CPlane__PreRender
    skip:
        jmp RETURN_CPlane__PreRender_SKIP
    }
}

#define HOOKPOS_CWaterCannon__Update_OncePerFrame 0x72A29B
#define HOOKSIZE_CWaterCannon__Update_OncePerFrame 0x5
static const unsigned int RETURN_CWaterCannon__Update_OncePerFrame = 0x72A2A0;
static const unsigned int RETURN_CWaterCannon__Update_OncePerFrame_SKIP = 0x72A2BB;
static void _declspec(naked) HOOK_CWaterCannon__Update_OncePerFrame()
{
    _asm {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        movsx eax, [edi+0x4]
        inc eax

        jmp RETURN_CWaterCannon__Update_OncePerFrame
    skip:
        jmp RETURN_CWaterCannon__Update_OncePerFrame_SKIP
    }
}

#define HOOKPOS_CWaterCannon__Update_OncePerFrame_PushPedFix 0x72A37B
#define HOOKSIZE_CWaterCannon__Update_OncePerFrame_PushPedFix 0x6
static const unsigned int RETURN_CWaterCannon__Update_OncePerFrame_PushPedFix = 0x72A381;
static const unsigned int RETURN_CWaterCannon__Update_OncePerFrame_PushPedFix_SKIP = 0x72A38E;
static void _declspec(naked) HOOK_CWaterCannon__Update_OncePerFrame_PushPedFix()
{
    _asm {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        mov ecx, ds:[0xB7CB4C]

        jmp RETURN_CWaterCannon__Update_OncePerFrame_PushPedFix
    skip:
        jmp RETURN_CWaterCannon__Update_OncePerFrame_PushPedFix_SKIP
    }
}

#define HOOKPOS_CWaterCannon__Render_FxFix 0x729430
#define HOOKSIZE_CWaterCannon__Render_FxFix 0x7
static const unsigned int RETURN_CWaterCannon__Render_FxFix = 0x729440;
static const unsigned int RETURN_CWaterCannon__Render_FxFix_SKIP = 0x7294EE;
static void _declspec(naked) HOOK_CWaterCannon__Render_FxFix()
{
    _asm {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        fstp [esp+0x84]

        jmp RETURN_CWaterCannon__Render_FxFix
    skip:
        jmp RETURN_CWaterCannon__Render_FxFix_SKIP
    }
}

void CMultiplayerSA::InitHooks_FrameRateFixes()
{
    EZHookInstall(CTaskSimpleUseGun__SetMoveAnim);
    EZHookInstall(CTimer__Update);
    EZHookInstall(BreakObject_c__Update);
    EZHookInstall(CProjectileInfo__Update);
    EZHookInstall(CVehicle__AddWheelDirtAndWater);
    EZHookInstall(CPlane__PreRender);
    EZHookInstall(CWaterCannon__Update_OncePerFrame);
    EZHookInstall(CWaterCannon__Update_OncePerFrame_PushPedFix);
    EZHookInstall(CWaterCannon__Render_FxFix);
}
