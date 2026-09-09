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
#include <algorithm>
#include <cmath>

static bool         bWouldBeNewFrame = false;
static unsigned int nLastFrameTime = 0;

constexpr float kOriginalTimeStep = 50.0f / 30.0f;

// Fixes player movement issue while aiming and walking on high FPS.
// Only rescales the compare threshold; m_MoveCmd's reset is NOPed separately in InitHooks_FrameRateFixes.
#define HOOKPOS_CTaskSimpleUseGun__SetMoveAnim  0x61E4F2
#define HOOKSIZE_CTaskSimpleUseGun__SetMoveAnim 0x6
const unsigned int            RETURN_CTaskSimpleUseGun__SetMoveAnim = 0x61E4F8;
static void __declspec(naked) HOOK_CTaskSimpleUseGun__SetMoveAnim()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        fld ds:[0xB7CB5C]           // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fmul ds:[0x858B1C]          // 0.1f
        fxch
        fcom
        fxch
        fstp st(0)
        jmp RETURN_CTaskSimpleUseGun__SetMoveAnim
    }
    // clang-format on
}

// Fixes excessively fast camera shaking with setCameraShakeLevel on high FPS.
#define HOOKPOS_CCamera__Process  0x52C723
#define HOOKSIZE_CCamera__Process 0x12
static const unsigned int     RETURN_CCamera__Process = 0x52C735;
static void __declspec(naked) HOOK_CCamera__Process()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        fld ds:[0x858C80]           // 5.0f
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fadd ds:[0xB6EC30]
        fstp ds:[0xB6EC30]
        jmp RETURN_CCamera__Process
    }
    // clang-format on
}

// Fixes helicopters accelerating excessively during takeoff at high FPS.
#define HOOKPOS_CHeli__ProcessFlyingCarStuff  0x6C4F13
#define HOOKSIZE_CHeli__ProcessFlyingCarStuff 0x2A
static const unsigned int     RETURN_CHeli__ProcessFlyingCarStuff = 0x6C4F3D;
static void __declspec(naked) HOOK_CHeli__ProcessFlyingCarStuff()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov ax, [esi+0x22]
        cmp ax, 465
        jz is_rc_heli
        cmp ax, 501
        jz is_rc_heli

        fld ds:[0x858CDC]           // 0.001f
        jmp end

    is_rc_heli:
        fld ds:[0x859CD8]           // 0.003f

    end:
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fadd [esi+0x84C]
        jmp RETURN_CHeli__ProcessFlyingCarStuff
    }
    // clang-format on
}

// Fixes excessively fast movement of fog on high FPS.
#define HOOKPOS_CClouds__MovingFog_Update  0x716BA6
#define HOOKSIZE_CClouds__MovingFog_Update 0x16
static const unsigned int     RETURN_CClouds__MovingFog_Update = 0x716BBC;
static void __declspec(naked) HOOK_CClouds__MovingFog_Update()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        fmul [edi*4+0xC6E394]       // CClouds::ms_mf.fSpeedFactor
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fadd [esi]
        fstp [esi]
        fld [esp+0x18]
        fmul [edi*4+0xC6E394]       // CClouds::ms_mf.fSpeedFactor
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        jmp RETURN_CClouds__MovingFog_Update
    }
    // clang-format on
}

// Fixes glass shards spinning and moving at excessive speeds on high FPS.
#define HOOKPOS_CFallingGlassPane__Update_A  0x71AABF
#define HOOKSIZE_CFallingGlassPane__Update_A 0x6
static const unsigned int     RETURN_CFallingGlassPane__Update_A = 0x71AAC5;
static void __declspec(naked) HOOK_CFallingGlassPane__Update_A()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        fld [esp+0x28]
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fstp [esp+0x28]
        fld [esp+0x24]
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fstp [esp+0x24]
        fld [esp+0x20]
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fadd [esi]
        jmp RETURN_CFallingGlassPane__Update_A
    }
    // clang-format on
}

// Fixes glass shards spinning and moving at excessive speeds on high FPS.
#define HOOKPOS_CFallingGlassPane__Update_B  0x71AAEA
#define HOOKSIZE_CFallingGlassPane__Update_B 0x6
static const unsigned int     RETURN_CFallingGlassPane__Update_B = 0x71AAF0;
static void __declspec(naked) HOOK_CFallingGlassPane__Update_B()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        fld [eax]
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fstp [eax]
        fld [eax+0x4]
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fstp [eax+0x4]
        fld [eax+0x8]
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fstp [eax+0x8]
        mov ecx, [eax]
        mov [esp+0x2C], ecx
        jmp RETURN_CFallingGlassPane__Update_B
    }
    // clang-format on
}

// Fixes glass shards spinning and moving at excessive speeds on high FPS.
#define HOOKPOS_CFallingGlassPane__Update_C  0x71AB29
#define HOOKSIZE_CFallingGlassPane__Update_C 0x6
static const unsigned int     RETURN_CFallingGlassPane__Update_C = 0x71AB2F;
static void __declspec(naked) HOOK_CFallingGlassPane__Update_C()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        fld [eax]
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fstp [eax]
        fld [eax+0x4]
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fstp [eax+0x4]
        fld [eax+0x8]
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fstp [eax+0x8]
        mov edx, [eax]
        mov [esp+0x38], edx
        jmp RETURN_CFallingGlassPane__Update_C
    }
    // clang-format on
}

// Ensure that CTimer::CurrentFrame is updated only every 33+ milliseconds.
#define HOOKPOS_CTimer__Update  0x561C5D
#define HOOKSIZE_CTimer__Update 0xE
static void __declspec(naked) HOOK_CTimer__Update()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
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
    // clang-format on
}

// Fixes premature despawning of broken breakable objects on high FPS.
#define HOOKPOS_BreakObject_c__Update  0x59E420
#define HOOKSIZE_BreakObject_c__Update 0xB
static const unsigned int     RETURN_BreakObject_c__Update = 0x59E42B;
static void __declspec(naked) HOOK_BreakObject_c__Update()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
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
    // clang-format on
}

// Fixes limited reach of the water cannon on high FPS.
#define HOOKPOS_CWaterCannon__Update_OncePerFrame  0x72A29B
#define HOOKSIZE_CWaterCannon__Update_OncePerFrame 0x5
static const unsigned int     RETURN_CWaterCannon__Update_OncePerFrame = 0x72A2A0;
static const unsigned int     RETURN_CWaterCannon__Update_OncePerFrame_SKIP = 0x72A2BB;
static void __declspec(naked) HOOK_CWaterCannon__Update_OncePerFrame()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        movsx eax, [edi+0x4]
        inc eax

        jmp RETURN_CWaterCannon__Update_OncePerFrame
    skip:
        jmp RETURN_CWaterCannon__Update_OncePerFrame_SKIP
    }
    // clang-format on
}

// Fixes money animation issues on high FPS.
#define HOOKPOS_CPlayerInfo__Process  0x5700F5
#define HOOKSIZE_CPlayerInfo__Process 0x6
static const unsigned int     RETURN_CPlayerInfo__Process = 0x5700FB;
static const unsigned int     RETURN_CPlayerInfo__Process_SKIP = 0x57015B;
static void __declspec(naked) HOOK_CPlayerInfo__Process()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        mov edx, [esi+0xBC]

        jmp RETURN_CPlayerInfo__Process
    skip:
        jmp RETURN_CPlayerInfo__Process_SKIP
    }
    // clang-format on
}

// Fixes excessive effects spawning from rocket launchers on high FPS.
#define HOOKPOS_CProjectileInfo__Update  0x738C63
#define HOOKSIZE_CProjectileInfo__Update 0x5
static const unsigned int     RETURN_CProjectileInfo__Update = 0x738C68;
static const unsigned int     RETURN_CProjectileInfo__Update_SKIP = 0x738F22;
static void __declspec(naked) HOOK_CProjectileInfo__Update()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        mov eax, [ebx]
        cmp eax, 0x13

        jmp RETURN_CProjectileInfo__Update
    skip:
        jmp RETURN_CProjectileInfo__Update_SKIP
    }
    // clang-format on
}

// Fixes excessive surface effects spawning from wheels on high FPS.
#define HOOKPOS_CVehicle__AddWheelDirtAndWater  0x6D2D50
#define HOOKSIZE_CVehicle__AddWheelDirtAndWater 0x6
static const unsigned int     RETURN_CVehicle__AddWheelDirtAndWater = 0x6D2D56;
static void __declspec(naked) HOOK_CVehicle__AddWheelDirtAndWater()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
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
    // clang-format on
}

// Fixes excessive smoke trail particle spawning from stuntplanes and cropdusters on high FPS.
#define HOOKPOS_CPlane__PreRender  0x6CA937
#define HOOKSIZE_CPlane__PreRender 0x6
static const unsigned int     RETURN_CPlane__PreRender = 0x6CA93D;
static const unsigned int     RETURN_CPlane__PreRender_SKIP = 0x6CAA93;
static void __declspec(naked) HOOK_CPlane__PreRender()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        mov al, [esi+0xA00]

        jmp RETURN_CPlane__PreRender
    skip:
        jmp RETURN_CPlane__PreRender_SKIP
    }
    // clang-format on
}

// Fixes increased frequency of water cannon pushing peds on high FPS.
#define HOOKPOS_CWaterCannon__Update_OncePerFrame_PushPedFix  0x72A37B
#define HOOKSIZE_CWaterCannon__Update_OncePerFrame_PushPedFix 0x6
static const unsigned int     RETURN_CWaterCannon__Update_OncePerFrame_PushPedFix = 0x72A381;
static const unsigned int     RETURN_CWaterCannon__Update_OncePerFrame_PushPedFix_SKIP = 0x72A38E;
static void __declspec(naked) HOOK_CWaterCannon__Update_OncePerFrame_PushPedFix()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        mov ecx, ds:[0xB7CB4C]

        jmp RETURN_CWaterCannon__Update_OncePerFrame_PushPedFix
    skip:
        jmp RETURN_CWaterCannon__Update_OncePerFrame_PushPedFix_SKIP
    }
    // clang-format on
}

// Fixes excessive particle spawning from water cannons on high FPS.
#define HOOKPOS_CWaterCannon__Render_FxFix  0x729437
#define HOOKSIZE_CWaterCannon__Render_FxFix 0x5
static const unsigned int     RETURN_CWaterCannon__Render_FxFix = 0x729440;
static const unsigned int     RETURN_CWaterCannon__Render_FxFix_SKIP = 0x7294EE;
static void __declspec(naked) HOOK_CWaterCannon__Render_FxFix()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        jmp RETURN_CWaterCannon__Render_FxFix
    skip:
        jmp RETURN_CWaterCannon__Render_FxFix_SKIP
    }
    // clang-format on
}

// Fixes excessive particle spawning with setPedHeadless on high FPS.
#define HOOKPOS_CPed__PreRenderAfterTest  0x5E7181
#define HOOKSIZE_CPed__PreRenderAfterTest 0x6
static const unsigned int     RETURN_CPed__PreRenderAfterTest = 0x5E7187;
static const unsigned int     RETURN_CPed__PreRenderAfterTest_SKIP = 0x5E722D;
static void __declspec(naked) HOOK_CPed__PreRenderAfterTest()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        mov eax, [ebp+0x46C]

        jmp RETURN_CPed__PreRenderAfterTest
    skip:
        jmp RETURN_CPed__PreRenderAfterTest_SKIP
    }
    // clang-format on
}

// Fixes excessive particle spawning from boats on high FPS.
#define HOOKPOS_cBuoyancy__AddSplashParticles  0x6C34E0
#define HOOKSIZE_cBuoyancy__AddSplashParticles 0x6
static const unsigned int     RETURN_cBuoyancy__AddSplashParticles = 0x6C34E6;
static void __declspec(naked) HOOK_cBuoyancy__AddSplashParticles()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        sub esp, 0xE0

        jmp RETURN_cBuoyancy__AddSplashParticles
    skip:
        retn 0x2C
    }
    // clang-format on
}

// Fixes excessive weather particle spawning on high FPS.
#define HOOKPOS_CWeather__AddRain  0x72AAA8
#define HOOKSIZE_CWeather__AddRain 0x6
static const unsigned int     RETURN_CWeather__AddRain = 0x72AAAE;
static void __declspec(naked) HOOK_CWeather__AddRain()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        fld ds:[0xC812F0]

        jmp RETURN_CWeather__AddRain

    skip:
        add esp, 0x84
        ret
    }
    // clang-format on
}

// Fixes excessive damage particle spawning from airplanes on high FPS.
#define HOOKPOS_CPlane__ProcessFlyingCarStuff  0x6CBE4B
#define HOOKSIZE_CPlane__ProcessFlyingCarStuff 0x6
static const unsigned int     RETURN_CPlane__ProcessFlyingCarStuff = 0x6CBE51;
static const unsigned int     RETURN_CPlane__ProcessFlyingCarStuff_SKIP = 0x6CC0D9;
static void __declspec(naked) HOOK_CPlane__ProcessFlyingCarStuff()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        mov edx, ds:[0xB6F03C]

        jmp RETURN_CPlane__ProcessFlyingCarStuff
    skip:
        jmp RETURN_CPlane__ProcessFlyingCarStuff_SKIP
    }
    // clang-format on
}

// Fixes excessive spawning of sand and water particles from vehicles on high FPS.
#define HOOKPOS_CAutomobile__UpdateWheelMatrix  0x6AA78A
#define HOOKSIZE_CAutomobile__UpdateWheelMatrix 0x5
static const unsigned int     RETURN_CAutomobile__UpdateWheelMatrix = 0x6AA78F;
static const unsigned int     RETURN_CAutomobile__UpdateWheelMatrix_SKIP = 0x6AAAD0;
static void __declspec(naked) HOOK_CAutomobile__UpdateWheelMatrix()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        push 0x3D4CCCCD

        jmp RETURN_CAutomobile__UpdateWheelMatrix
    skip:
        jmp RETURN_CAutomobile__UpdateWheelMatrix_SKIP
    }
    // clang-format on
}

// Fixes excessive particle spawning from boats on high FPS.
#define HOOKPOS_CVehicle__DoBoatSplashes  0x6DD130
#define HOOKSIZE_CVehicle__DoBoatSplashes 0x6
static const unsigned int     RETURN_CVehicle__DoBoatSplashes = 0x6DD136;
static void __declspec(naked) HOOK_CVehicle__DoBoatSplashes()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        sub esp, 0x80

        jmp RETURN_CVehicle__DoBoatSplashes
    skip:
        retn 4
    }
    // clang-format on
}

// Fixes excessive rain particle spawning on vehicles on high FPS.
#define HOOKPOS_CVehicle__AddWaterSplashParticles  0x6DDF60
#define HOOKSIZE_CVehicle__AddWaterSplashParticles 0x6
static const unsigned int     RETURN_CVehicle__AddWaterSplashParticles = 0x6DDF66;
static void __declspec(naked) HOOK_CVehicle__AddWaterSplashParticles()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        sub esp, 0xC4

        jmp RETURN_CVehicle__AddWaterSplashParticles
    skip:
        ret
    }
    // clang-format on
}

// Fixes excessive particle spawning from airplanes when damaged on high FPS.
#define HOOKPOS_CPlane__ProcessControl  0x6C939A
#define HOOKSIZE_CPlane__ProcessControl 0x5
static const unsigned int     RETURN_CPlane__ProcessControl = 0x6C939F;
static const unsigned int     RETURN_CPlane__ProcessControl_SKIP = 0x6C9463;
static void __declspec(naked) HOOK_CPlane__ProcessControl()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        lea ecx, [esp+0x3C]
        push ecx

        jmp RETURN_CPlane__ProcessControl
    skip:
        jmp RETURN_CPlane__ProcessControl_SKIP
    }
    // clang-format on
}

// Fixes excessive exhaust particle spawning from vehicles on high FPS.
#define HOOKPOS_CVehicle__AddExhaustParticles  0x6DE240
#define HOOKSIZE_CVehicle__AddExhaustParticles 0x6
static const unsigned int     RETURN_CVehicle__AddExhaustParticles = 0x6DE246;
static void __declspec(naked) HOOK_CVehicle__AddExhaustParticles()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        mov eax, fs:0x0

        jmp RETURN_CVehicle__AddExhaustParticles
    skip:
        ret
    }
    // clang-format on
}

// Fixes excessive particle spawning while swimming on high FPS.
#define HOOKPOS_CTaskSimpleSwim__ProcessEffects  0x68AD3B
#define HOOKSIZE_CTaskSimpleSwim__ProcessEffects 0x6
static const unsigned int     RETURN_CTaskSimpleSwim__ProcessEffects = 0x68AD41;
static const unsigned int     RETURN_CTaskSimpleSwim__ProcessEffects_SKIP = 0x68AFDB;
static void __declspec(naked) HOOK_CTaskSimpleSwim__ProcessEffects()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        mov eax, [edi+0x14]
        add eax, 0x10

        jmp RETURN_CTaskSimpleSwim__ProcessEffects
    skip:
        jmp RETURN_CTaskSimpleSwim__ProcessEffects_SKIP
    }
    // clang-format on
}

// Fixes excessive particle spawning while swimming on high FPS.
#define HOOKPOS_CTaskSimpleSwim__ProcessEffectsBubbleFix  0x68AC31
#define HOOKSIZE_CTaskSimpleSwim__ProcessEffectsBubbleFix 0x7
static const unsigned int     RETURN_CTaskSimpleSwim__ProcessEffectsBubbleFix = 0x68AC38;
static const unsigned int     RETURN_CTaskSimpleSwim__ProcessEffectsBubbleFix_SKIP = 0x68AD36;
static void __declspec(naked) HOOK_CTaskSimpleSwim__ProcessEffectsBubbleFix()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        mov ecx, edi
        mov esi, 5

        jmp RETURN_CTaskSimpleSwim__ProcessEffectsBubbleFix
    skip:
        jmp RETURN_CTaskSimpleSwim__ProcessEffectsBubbleFix_SKIP
    }
    // clang-format on
}

// Fixes boat water resistance and deceleration scaling on high FPS.
#define HOOKPOS_CVehicle__ApplyBoatWaterResistance  0x6D2771
#define HOOKSIZE_CVehicle__ApplyBoatWaterResistance 6
static const unsigned int     RETURN_CVehicle__ApplyBoatWaterResistance = 0x6D2777;
static void __declspec(naked) HOOK_CVehicle__ApplyBoatWaterResistance()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        fmul    ds:[0x871DDC]           // Original constant used in code
        fmul    ds:[0xB7CB5C]           // Multiply by current timestep
        fdiv    kOriginalTimeStep       // Divide by desired 30 FPS timestep
        jmp     RETURN_CVehicle__ApplyBoatWaterResistance
    }
    // clang-format on
}

// Fixes ped swimming resistance and speed on high FPS.
#define HOOKPOS_CTaskSimpleSwim__ProcessSwimmingResistance  0x68A4EF
#define HOOKSIZE_CTaskSimpleSwim__ProcessSwimmingResistance 6
static const unsigned int     RETURN_CTaskSimpleSwim__ProcessSwimmingResistance = 0x68A50E;
static void __declspec(naked) HOOK_CTaskSimpleSwim__ProcessSwimmingResistance()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        fsub    st, st(1)

        fld     dword ptr [esp + 16]
        lea     eax, [esi + 44h]
        mov     ecx, eax
        fmul    st, st(1)

        fdiv    ds:[0xB7CB5C]
        fmul    kOriginalTimeStep

        fstp    dword ptr [esp + 28]

        fld     dword ptr [esp + 20]
        fmul    st, st(1)

        fdiv    ds:[0xB7CB5C]
        fmul    kOriginalTimeStep

        fstp    dword ptr [esp + 32]
        fmul    dword ptr [esp + 24]

        fdiv    ds:[0xB7CB5C]
        fmul    kOriginalTimeStep

        jmp     RETURN_CTaskSimpleSwim__ProcessSwimmingResistance
    }
    // clang-format on
}

// Fixes invisible weapon particles (extinguisher, spraycan, flamethrower) at high FPS
#define HOOKPOS_CWeapon_Update  0x73DC3D
#define HOOKSIZE_CWeapon_Update 5
static constexpr std::uintptr_t RETURN_CWeapon_Update = 0x073DC42;
static void __declspec(naked)   HOOK_CWeapon_Update()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Temp fix for camera
        cmp [esi], 0x2B // CWeapon::m_eWeaponType
        je skip

        // timeStep / kOriginalTimeStep
        fld ds:[0xB7CB5C] // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep

        mov eax, [esi+10h] // m_timeToNextShootInMS
        mov ebx, ds:[0xB7CB84] // CTimer::m_snTimeInMilliseconds

        sub eax, ebx // m_timeToNextShootInMS - CTimer::m_snTimeInMilliseconds

        push eax
        fild dword ptr [esp]
        add esp, 4

        fmul st(0), st(1) // (m_timeToNextShootInMS - CTimer::m_snTimeInMilliseconds) * (timeStep / kOriginalTimeStep)
        fadd st(0), ebx // + m_snTimeInMilliseconds
        fistp [esi+10h]
        fstp st(0)

        mov eax, ebx

        xor ebx, ebx
        jmp RETURN_CWeapon_Update

        skip:
        mov eax, ds:[0xB7CB84]
        jmp RETURN_CWeapon_Update
    }
    // clang-format on
}

// Governs pedestrian push velocity on unoccupied vehicles during collisions.
// In GTA:SA, CPhysical::ApplyCollision adds an impulse to velocity on every contact frame.
// At high framerates, these impulses occur far more frequently than at 30 FPS, overpowering
// tire friction and causing the vehicle to accelerate unnaturally fast.
// This caps push velocity using momentum conservation to maintain consistent vehicle weight across framerates.
static void GovernPedPushVehicleVelocity(CPhysicalSAInterface* vehicle, CPhysicalSAInterface* ped, const CVector& initialLinearVelocity,
                                         const CVector& initialAngularVelocity, const CVector& pedLinearVelocity, CVector& currentLinearVelocity,
                                         CVector& currentAngularVelocity)
{
    if (!vehicle || !ped)
        return;

    // Only govern collisions on unoccupied vehicles (no driver present)
    const auto* vehicleInterface = reinterpret_cast<const CVehicleSAInterface*>(vehicle);
    if (vehicleInterface->pDriver != nullptr)
        return;

    // Prevent vehicle tilting or flipping from push contact
    currentAngularVelocity.fX = initialAngularVelocity.fX;
    currentAngularVelocity.fY = initialAngularVelocity.fY;

    // Keep vertical velocity unaffected by push contact
    currentLinearVelocity.fZ = initialLinearVelocity.fZ;

    const CVector deltaVelocity = currentLinearVelocity - initialLinearVelocity;
    const float   deltaMagnitudeSquared = (deltaVelocity.fX * deltaVelocity.fX) + (deltaVelocity.fY * deltaVelocity.fY);
    if (deltaMagnitudeSquared <= 0.000001f)
        return;

    const float deltaMagnitude = std::sqrt(deltaMagnitudeSquared);
    const float pushDirectionX = deltaVelocity.fX / deltaMagnitude;
    const float pushDirectionY = deltaVelocity.fY / deltaMagnitude;

    const float pedForwardSpeed = (pedLinearVelocity.fX * pushDirectionX) + (pedLinearVelocity.fY * pushDirectionY);
    if (pedForwardSpeed <= 0.0f)
    {
        currentLinearVelocity = initialLinearVelocity;
        currentAngularVelocity = initialAngularVelocity;
        return;
    }

    const float     timeStep = *reinterpret_cast<const float*>(0xB7CB5C);
    constexpr float baselineTimeStep = 1.0f;
    const float     timeStepRatio = std::clamp(timeStep / baselineTimeStep, 0.001f, 1.0f);

    constexpr float playerPushMassMultiplier = 10.0f;
    const float     effectivePedMass = ped->m_fMass * playerPushMassMultiplier;
    const float     vehicleMass = vehicle->m_fMass;
    const float     maximumPushVelocity = pedForwardSpeed * (effectivePedMass / (effectivePedMass + vehicleMass));

    float forwardX = 0.0f;
    float forwardY = 1.0f;
    float rightX = 1.0f;
    float rightY = 0.0f;

    if (vehicle->matrix != nullptr)
    {
        forwardX = vehicle->matrix->vFront.fX;
        forwardY = vehicle->matrix->vFront.fY;
        rightX = vehicle->matrix->vRight.fX;
        rightY = vehicle->matrix->vRight.fY;
    }
    else
    {
        const float heading = vehicle->m_transform.m_heading;
        forwardX = -std::sin(heading);
        forwardY = std::cos(heading);
        rightX = std::cos(heading);
        rightY = std::sin(heading);
    }

    const float forwardLen = std::sqrt((forwardX * forwardX) + (forwardY * forwardY));
    if (forwardLen > 0.0001f)
    {
        forwardX /= forwardLen;
        forwardY /= forwardLen;
    }

    const float rightLen = std::sqrt((rightX * rightX) + (rightY * rightY));
    if (rightLen > 0.0001f)
    {
        rightX /= rightLen;
        rightY /= rightLen;
    }

    const float initialForwardSpeed = (initialLinearVelocity.fX * forwardX) + (initialLinearVelocity.fY * forwardY);
    const float initialLateralSpeed = (initialLinearVelocity.fX * rightX) + (initialLinearVelocity.fY * rightY);

    float currentForwardSpeed = (currentLinearVelocity.fX * forwardX) + (currentLinearVelocity.fY * forwardY);
    float currentLateralSpeed = (currentLinearVelocity.fX * rightX) + (currentLinearVelocity.fY * rightY);

    const float deltaForwardSpeed = currentForwardSpeed - initialForwardSpeed;
    currentForwardSpeed = initialForwardSpeed + (deltaForwardSpeed * timeStepRatio);

    constexpr float wakeUpThreshold = 0.008f;
    if (std::abs(initialForwardSpeed) < 0.001f && std::abs(currentForwardSpeed) < wakeUpThreshold && std::abs(deltaForwardSpeed) > 0.001f)
    {
        currentForwardSpeed = (deltaForwardSpeed > 0.0f) ? wakeUpThreshold : -wakeUpThreshold;
    }

    const float allowedForwardSpeedMax = std::max(initialForwardSpeed, maximumPushVelocity);
    const float allowedForwardSpeedMin = std::min(initialForwardSpeed, -maximumPushVelocity);
    currentForwardSpeed = std::clamp(currentForwardSpeed, allowedForwardSpeedMin, allowedForwardSpeedMax);

    const float deltaLateralSpeed = currentLateralSpeed - initialLateralSpeed;
    currentLateralSpeed = initialLateralSpeed + (deltaLateralSpeed * timeStepRatio);

    constexpr float lateralScrubRatio = 0.10f;
    const float     maximumLateralVelocity = maximumPushVelocity * lateralScrubRatio;
    const float     allowedLateralSpeedMax = std::max(initialLateralSpeed, maximumLateralVelocity);
    const float     allowedLateralSpeedMin = std::min(initialLateralSpeed, -maximumLateralVelocity);
    currentLateralSpeed = std::clamp(currentLateralSpeed, allowedLateralSpeedMin, allowedLateralSpeedMax);

    currentLinearVelocity.fX = (currentForwardSpeed * forwardX) + (currentLateralSpeed * rightX);
    currentLinearVelocity.fY = (currentForwardSpeed * forwardY) + (currentLateralSpeed * rightY);

    // Scale collision yaw impulse so rotational torque delivered per second is invariant across framerates
    const float deltaAngularZ = currentAngularVelocity.fZ - initialAngularVelocity.fZ;
    currentAngularVelocity.fZ = initialAngularVelocity.fZ + (deltaAngularZ * timeStepRatio);

    // Physical angular velocity limit derived from conservation of angular momentum at the vehicle corner
    constexpr float cornerLeverArm = 2.2f;
    const float     vehicleTurnMass = (vehicle->m_fTurnMass > 0.0f) ? vehicle->m_fTurnMass : (vehicleMass * 2.5f);
    const float     maximumAngularVelocity =
        (cornerLeverArm * effectivePedMass * pedForwardSpeed) / (vehicleTurnMass + (effectivePedMass * cornerLeverArm * cornerLeverArm));

    const float allowedYawSpeedMax = std::max(initialAngularVelocity.fZ, maximumAngularVelocity);
    const float allowedYawSpeedMin = std::min(initialAngularVelocity.fZ, -maximumAngularVelocity);
    currentAngularVelocity.fZ = std::clamp(currentAngularVelocity.fZ, allowedYawSpeedMin, allowedYawSpeedMax);
}

#define CALL_CPhysical__ApplyCollision_1    0x54BDB2
#define CALL_CPhysical__ApplyCollision_2    0x54BF78
#define CALL_CPhysical__ApplyCollision_3    0x54C23A
#define CALL_CPhysical__ApplyCollision_4    0x54C435
#define CALL_CPhysical__ApplyCollision_5    0x54D17E
#define CALL_CPhysical__ApplyCollision_6    0x54D27E
#define CALL_CPhysical__ApplyCollision_7    0x54D3FE
#define CALL_CPhysical__ApplyCollision_8    0x54D4D2
#define CALL_CPhysical__ApplyCollisionAlt_1 0x54C9FA
#define CALL_CPhysical__ApplyCollisionAlt_2 0x54CAC2

static bool __fastcall HOOK_CPhysical__ApplyCollision(CPhysicalSAInterface* thisEntity, void* /*edx*/, CEntitySAInterface* collidedEntity,
                                                      CColPointSAInterface* colPoint, float* thisDamageIntensity, float* collidedDamageIntensity)
{
    const CVector initialThisLinearVelocity = thisEntity ? thisEntity->m_vecLinearVelocity : CVector{};
    const CVector initialThisAngularVelocity = thisEntity ? thisEntity->m_vecAngularVelocity : CVector{};

    auto*   collidedPhysical = reinterpret_cast<CPhysicalSAInterface*>(collidedEntity);
    CVector initialCollidedLinearVelocity{};
    CVector initialCollidedAngularVelocity{};
    if (collidedPhysical)
    {
        initialCollidedLinearVelocity = collidedPhysical->m_vecLinearVelocity;
        initialCollidedAngularVelocity = collidedPhysical->m_vecAngularVelocity;
    }

    using ApplyCollisionFn = bool(__thiscall*)(CPhysicalSAInterface*, CEntitySAInterface*, CColPointSAInterface*, float*, float*);
    const auto originalApplyCollision = reinterpret_cast<ApplyCollisionFn>(0x548680);

    const bool result = originalApplyCollision(thisEntity, collidedEntity, colPoint, thisDamageIntensity, collidedDamageIntensity);

    if (result && thisEntity && collidedEntity)
    {
        // Entity types: 2 = Vehicle, 3 = Ped (from CEntitySAInterface::nType bitfield)
        const uint8 thisType = thisEntity->nType;
        const uint8 collidedType = collidedEntity->nType;

        if (thisType == 2 && collidedType == 3 && collidedPhysical)
        {
            GovernPedPushVehicleVelocity(thisEntity, collidedPhysical, initialThisLinearVelocity, initialThisAngularVelocity, initialCollidedLinearVelocity,
                                         thisEntity->m_vecLinearVelocity, thisEntity->m_vecAngularVelocity);
        }
        else if (thisType == 3 && collidedType == 2 && collidedPhysical)
        {
            GovernPedPushVehicleVelocity(collidedPhysical, thisEntity, initialCollidedLinearVelocity, initialCollidedAngularVelocity, initialThisLinearVelocity,
                                         collidedPhysical->m_vecLinearVelocity, collidedPhysical->m_vecAngularVelocity);
        }
    }

    return result;
}

static bool __fastcall HOOK_CPhysical__ApplyCollisionAlt(CPhysicalSAInterface* thisEntity, void* /*edx*/, CPhysicalSAInterface* collidedEntity,
                                                         CColPointSAInterface* colPoint, float* damageIntensity, CVector* outLinearVelocity,
                                                         CVector* outAngularVelocity)
{
    const CVector initialLinearVelocity = outLinearVelocity ? *outLinearVelocity : CVector{};
    const CVector initialAngularVelocity = outAngularVelocity ? *outAngularVelocity : CVector{};

    const CVector collidedLinearVelocity = collidedEntity ? collidedEntity->m_vecLinearVelocity : CVector{};

    using ApplyCollisionAltFn = bool(__thiscall*)(CPhysicalSAInterface*, CPhysicalSAInterface*, CColPointSAInterface*, float*, CVector*, CVector*);
    const auto originalApplyCollisionAlt = reinterpret_cast<ApplyCollisionAltFn>(0x544D50);

    const bool result = originalApplyCollisionAlt(thisEntity, collidedEntity, colPoint, damageIntensity, outLinearVelocity, outAngularVelocity);

    if (result && thisEntity && collidedEntity && outLinearVelocity && outAngularVelocity)
    {
        // Entity types: 2 = Vehicle, 3 = Ped
        const uint8 thisType = thisEntity->nType;
        const uint8 collidedType = collidedEntity->nType;

        if (thisType == 2 && collidedType == 3)
        {
            GovernPedPushVehicleVelocity(thisEntity, collidedEntity, initialLinearVelocity, initialAngularVelocity, collidedLinearVelocity, *outLinearVelocity,
                                         *outAngularVelocity);
        }
    }

    return result;
}

// In GTA:SA (30 FPS, timeStep = 1.66667f), vehicles sleep after 10 still frames (~333 ms).
// At high framerates (e.g. 240 FPS), 10 frames elapse in only ~41 ms, deactivating unoccupied vehicles
// before suspension springs can lift the chassis and causing bottomed-out suspension on spawn.
// Scaling the threshold dynamically by timeStep maintains an invariant settling window (~400 ms).
static uint8 __cdecl CalculateVehicleSleepFrameThreshold() noexcept
{
    const float timeStep = *reinterpret_cast<const float*>(0xB7CB5C);
    if (timeStep <= 0.0001f)
        return 10;

    constexpr float baselineNumerator = 20.0f;
    const float     rawThreshold = baselineNumerator / timeStep;
    return static_cast<uint8>(std::clamp(std::round(rawThreshold), 10.0f, 240.0f));
}

// Fixes bottomed-out vehicle suspension when spawning or dropping at high framerates.
// CAutomobile::ProcessControl
#define HOOKPOS_CAutomobile__ProcessControl_SleepThreshold  0x6B1D34
#define HOOKSIZE_CAutomobile__ProcessControl_SleepThreshold 10
static const unsigned int     RETURN_CAutomobile__ProcessControl_SleepThreshold = 0x6B1D3E;
static void __declspec(naked) HOOK_CAutomobile__ProcessControl_SleepThreshold()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push ecx
        push edx
        call CalculateVehicleSleepFrameThreshold
        mov bl, al
        pop edx
        pop ecx

        mov al, dl
        cmp al, bl
        mov [esi+0xB8], dl
        jmp RETURN_CAutomobile__ProcessControl_SleepThreshold
    }
    // clang-format on
}

// Fixes bottomed-out bike suspension when spawning or dropping at high framerates.
// CBike::ProcessControl
#define HOOKPOS_CBike__ProcessControl_SleepThreshold  0x6B997C
#define HOOKSIZE_CBike__ProcessControl_SleepThreshold 8
static const unsigned int     RETURN_CBike__ProcessControl_SleepThreshold = 0x6B9984;
static void __declspec(naked) HOOK_CBike__ProcessControl_SleepThreshold()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push ecx
        push edx
        call CalculateVehicleSleepFrameThreshold
        pop edx
        pop ecx

        cmp cl, al
        mov [esi+0xB8], cl
        jmp RETURN_CBike__ProcessControl_SleepThreshold
    }
    // clang-format on
}

// CBike::ProcessControl sleep counter clamp
#define HOOKPOS_CBike__ProcessControl_SleepClamp  0x6B99C5
#define HOOKSIZE_CBike__ProcessControl_SleepClamp 16
static const unsigned int     RETURN_CBike__ProcessControl_SleepClamp = 0x6B99D5;
static void __declspec(naked) HOOK_CBike__ProcessControl_SleepClamp()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push ecx
        push edx
        call CalculateVehicleSleepFrameThreshold
        pop edx
        pop ecx

        cmp [esi+0xB8], al
        jbe clamp_done
        mov [esi+0xB8], al

    clamp_done:
        jmp RETURN_CBike__ProcessControl_SleepClamp
    }
    // clang-format on
}

#define HOOKPOS_CPhysical__ApplyAirResistance  0x544D29
#define HOOKSIZE_CPhysical__ApplyAirResistance 5
static const unsigned int     RETURN_CPhysical__ApplyAirResistance = 0x544D4D;
static void __declspec(naked) HOOK_CPhysical__ApplyAirResistance()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        fld ds:[0x862CD0]            // 0.99000001f
        fld ds:[0xB7CB5C]            // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep            // 1.666f
        mov eax, 0x822130            // powf
        call eax

        fld st(0)
        fmul [esi+0x50]
        fstp [esi+0x50]

        fld st(0)
        fmul [esi+0x54]
        fstp [esi+0x54]

        fmul [esi+0x58]
        fstp [esi+0x58]
        jmp RETURN_CPhysical__ApplyAirResistance
    }
    // clang-format on
}

template <unsigned int returnAddress>
static void __declspec(naked) HOOK_VehicleRapidStopFix()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    static unsigned int RETURN_VehicleRapidStopFix = returnAddress;
    // clang-format off
    __asm
    {
        fld ds:[0xC2B9CC]            // mod_HandlingManager.m_fWheelFriction
        fmul ds:[0xB7CB5C]            // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep            // 1.666f
        jmp RETURN_VehicleRapidStopFix
    }
    // clang-format on
}

void CMultiplayerSA::SetRapidVehicleStopFixEnabled(bool enabled)
{
    if (m_isRapidVehicleStopFixEnabled == enabled)
        return;

    if (enabled)
    {
        EZHookInstall(CPhysical__ApplyAirResistance);

        // CVehicle::ProcessWheel
        HookInstall(0x6D6E69, (DWORD)HOOK_VehicleRapidStopFix<0x6D6E6F>, 6);
        HookInstall(0x6D6EA8, (DWORD)HOOK_VehicleRapidStopFix<0x6D6EAE>, 6);

        // CVehicle::ProcessBikeWheel
        HookInstall(0x6D767F, (DWORD)HOOK_VehicleRapidStopFix<0x6D7685>, 6);
        HookInstall(0x6D76AB, (DWORD)HOOK_VehicleRapidStopFix<0x6D76B1>, 6);
        HookInstall(0x6D76CD, (DWORD)HOOK_VehicleRapidStopFix<0x6D76D3>, 6);
    }
    else
    {
        MemCpy((void*)HOOKPOS_CPhysical__ApplyAirResistance, "\xD9\x46\x50\xD8\x0D", 5);

        MemCpy((void*)0x6D6E69, "\xD9\x05\xCC\xB9\xC2\x00", 6);
        MemCpy((void*)0x6D6EA8, "\xD9\x05\xCC\xB9\xC2\x00", 6);

        MemCpy((void*)0x6D767F, "\xD9\x05\xCC\xB9\xC2\x00", 6);
        MemCpy((void*)0x6D76AB, "\xD9\x05\xCC\xB9\xC2\x00", 6);
        MemCpy((void*)0x6D76CD, "\xD9\x05\xCC\xB9\xC2\x00", 6);
    }

    m_isRapidVehicleStopFixEnabled = enabled;
}

void CMultiplayerSA::InitHooks_FrameRateFixes()
{
    EZHookInstall(CTaskSimpleUseGun__SetMoveAnim);
    EZHookInstall(CCamera__Process);
    EZHookInstall(CHeli__ProcessFlyingCarStuff);
    EZHookInstall(CClouds__MovingFog_Update);
    EZHookInstall(CFallingGlassPane__Update_A);
    EZHookInstall(CFallingGlassPane__Update_B);
    EZHookInstall(CFallingGlassPane__Update_C);

    // Fixes camera jitter while aiming and walking at high FPS.
    // CTaskSimpleUseGun::SetMoveAnim
    MemSet((void*)0x61E5E4, 0x90, 0x6);

    // Fixes slow camera movement towards the back of the vehicle on high FPS.
    // CCam::Process_FollowCar_SA
    MemSet((void*)0x524FD7, 0x90, 0x1B);

    // Fixes slow boat movement on high FPS.
    // CVehicle::ProcessBoatControl
    MemPut(0x6DC23F, &kOriginalTimeStep);

    // Fixes climbing over certain objects killing player on high FPS or low game speed.
    // GitHub Issue #602
    MemPut(0x6811E9, &kOriginalTimeStep);
    MemPut(0x68128A, &kOriginalTimeStep);
    MemPut(0x68131B, &kOriginalTimeStep);

    // CTimer::m_FrameCounter fixes
    EZHookInstall(CTimer__Update);

    EZHookInstall(BreakObject_c__Update);
    EZHookInstall(CWaterCannon__Update_OncePerFrame);
    EZHookInstall(CPlayerInfo__Process);

    EZHookInstall(CProjectileInfo__Update);
    EZHookInstall(CVehicle__AddWheelDirtAndWater);
    EZHookInstall(CPlane__PreRender);
    EZHookInstall(CWaterCannon__Update_OncePerFrame_PushPedFix);
    EZHookInstall(CWaterCannon__Render_FxFix);
    EZHookInstall(CPed__PreRenderAfterTest);
    EZHookInstall(cBuoyancy__AddSplashParticles);
    EZHookInstall(CWeather__AddRain);
    EZHookInstall(CPlane__ProcessFlyingCarStuff);
    EZHookInstall(CAutomobile__UpdateWheelMatrix);
    EZHookInstall(CVehicle__DoBoatSplashes);
    EZHookInstall(CVehicle__AddWaterSplashParticles);
    EZHookInstall(CVehicle__ApplyBoatWaterResistance);
    EZHookInstall(CPlane__ProcessControl);
    EZHookInstall(CVehicle__AddExhaustParticles);
    EZHookInstall(CTaskSimpleSwim__ProcessEffects);
    EZHookInstall(CTaskSimpleSwim__ProcessEffectsBubbleFix);
    EZHookInstall(CTaskSimpleSwim__ProcessSwimmingResistance);

    EZHookInstall(CWeapon_Update);
    EZHookInstall(CAutomobile__ProcessControl_SleepThreshold);
    EZHookInstall(CBike__ProcessControl_SleepThreshold);
    EZHookInstall(CBike__ProcessControl_SleepClamp);
    HookInstallCall(CALL_CPhysical__ApplyCollision_1, (DWORD)HOOK_CPhysical__ApplyCollision);
    HookInstallCall(CALL_CPhysical__ApplyCollision_2, (DWORD)HOOK_CPhysical__ApplyCollision);
    HookInstallCall(CALL_CPhysical__ApplyCollision_3, (DWORD)HOOK_CPhysical__ApplyCollision);
    HookInstallCall(CALL_CPhysical__ApplyCollision_4, (DWORD)HOOK_CPhysical__ApplyCollision);
    HookInstallCall(CALL_CPhysical__ApplyCollision_5, (DWORD)HOOK_CPhysical__ApplyCollision);
    HookInstallCall(CALL_CPhysical__ApplyCollision_6, (DWORD)HOOK_CPhysical__ApplyCollision);
    HookInstallCall(CALL_CPhysical__ApplyCollision_7, (DWORD)HOOK_CPhysical__ApplyCollision);
    HookInstallCall(CALL_CPhysical__ApplyCollision_8, (DWORD)HOOK_CPhysical__ApplyCollision);
    HookInstallCall(CALL_CPhysical__ApplyCollisionAlt_1, (DWORD)HOOK_CPhysical__ApplyCollisionAlt);
    HookInstallCall(CALL_CPhysical__ApplyCollisionAlt_2, (DWORD)HOOK_CPhysical__ApplyCollisionAlt);
}
