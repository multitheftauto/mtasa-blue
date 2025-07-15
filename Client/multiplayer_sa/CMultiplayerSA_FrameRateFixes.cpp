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

static bool         bWouldBeNewFrame = false;
static unsigned int nLastFrameTime = 0;

constexpr float kOriginalTimeStep = 50.0f / 30.0f;

// Fixes player movement issue while aiming and walking on high FPS.
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

// Fixes excessively fast camera shaking with setCameraShakeLevel on high FPS.
#define HOOKPOS_CCamera__Process 0x52C723
#define HOOKSIZE_CCamera__Process 0x12
static const unsigned int RETURN_CCamera__Process = 0x52C735;
static void _declspec(naked) HOOK_CCamera__Process()
{
    _asm {
        fld ds:[0x858C80]           // 5.0f
        fmul ds:[0xB7CB5C]          // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fadd ds:[0xB6EC30]
        fstp ds:[0xB6EC30]
        jmp RETURN_CCamera__Process
    }
}

// Fixes helicopters accelerating excessively during takeoff at high FPS.
#define HOOKPOS_CHeli__ProcessFlyingCarStuff 0x6C4F13
#define HOOKSIZE_CHeli__ProcessFlyingCarStuff 0x2A
static const unsigned int RETURN_CHeli__ProcessFlyingCarStuff = 0x6C4F3D;
static void _declspec(naked) HOOK_CHeli__ProcessFlyingCarStuff()
{
    _asm {
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
}

// Fixes excessively fast movement of fog on high FPS.
#define HOOKPOS_CClouds__MovingFog_Update 0x716BA6
#define HOOKSIZE_CClouds__MovingFog_Update 0x16
static const unsigned int RETURN_CClouds__MovingFog_Update = 0x716BBC;
static void _declspec(naked) HOOK_CClouds__MovingFog_Update()
{
    _asm {
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
}

// Fixes glass shards spinning and moving at excessive speeds on high FPS.
#define HOOKPOS_CFallingGlassPane__Update_A 0x71AABF
#define HOOKSIZE_CFallingGlassPane__Update_A 0x6
static const unsigned int RETURN_CFallingGlassPane__Update_A = 0x71AAC5;
static void _declspec(naked) HOOK_CFallingGlassPane__Update_A()
{
    _asm {
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
}

// Fixes glass shards spinning and moving at excessive speeds on high FPS.
#define HOOKPOS_CFallingGlassPane__Update_B 0x71AAEA
#define HOOKSIZE_CFallingGlassPane__Update_B 0x6
static const unsigned int RETURN_CFallingGlassPane__Update_B = 0x71AAF0;
static void _declspec(naked) HOOK_CFallingGlassPane__Update_B()
{
    _asm {
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
}

// Fixes glass shards spinning and moving at excessive speeds on high FPS.
#define HOOKPOS_CFallingGlassPane__Update_C 0x71AB29
#define HOOKSIZE_CFallingGlassPane__Update_C 0x6
static const unsigned int RETURN_CFallingGlassPane__Update_C = 0x71AB2F;
static void _declspec(naked) HOOK_CFallingGlassPane__Update_C()
{
    _asm {
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
}

// Ensure that CTimer::CurrentFrame is updated only every 33+ milliseconds.
#define HOOKPOS_CTimer__Update 0x561C5D
#define HOOKSIZE_CTimer__Update 0xE
static void _declspec(naked) HOOK_CTimer__Update()
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

// Fixes premature despawning of broken breakable objects on high FPS.
#define HOOKPOS_BreakObject_c__Update 0x59E420
#define HOOKSIZE_BreakObject_c__Update 0xB
static const unsigned int RETURN_BreakObject_c__Update = 0x59E42B;
static void _declspec(naked) HOOK_BreakObject_c__Update()
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

// Fixes limited reach of the water cannon on high FPS.
#define HOOKPOS_CWaterCannon__Update_OncePerFrame  0x72A29B
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

// Fixes money animation issues on high FPS.
#define HOOKPOS_CPlayerInfo__Process 0x5700F5
#define HOOKSIZE_CPlayerInfo__Process 0x6
static const unsigned int RETURN_CPlayerInfo__Process = 0x5700FB;
static const unsigned int RETURN_CPlayerInfo__Process_SKIP = 0x57015B;
static void _declspec(naked) HOOK_CPlayerInfo__Process()
{
    _asm {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        mov edx, [esi+0xBC]

        jmp RETURN_CPlayerInfo__Process
    skip:
        jmp RETURN_CPlayerInfo__Process_SKIP
    }
}

// Fixes excessive effects spawning from rocket launchers on high FPS.
#define HOOKPOS_CProjectileInfo__Update 0x738C63
#define HOOKSIZE_CProjectileInfo__Update 0x5
static const unsigned int RETURN_CProjectileInfo__Update = 0x738C68;
static const unsigned int RETURN_CProjectileInfo__Update_SKIP = 0x738F22;
static void _declspec(naked) HOOK_CProjectileInfo__Update()
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

// Fixes excessive surface effects spawning from wheels on high FPS.
#define HOOKPOS_CVehicle__AddWheelDirtAndWater 0x6D2D50
#define HOOKSIZE_CVehicle__AddWheelDirtAndWater 0x6
static const unsigned int RETURN_CVehicle__AddWheelDirtAndWater = 0x6D2D56;
static void _declspec(naked) HOOK_CVehicle__AddWheelDirtAndWater()
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

// Fixes excessive smoke trail particle spawning from stuntplanes and cropdusters on high FPS.
#define HOOKPOS_CPlane__PreRender 0x6CA937
#define HOOKSIZE_CPlane__PreRender 0x6
static const unsigned int RETURN_CPlane__PreRender = 0x6CA93D;
static const unsigned int RETURN_CPlane__PreRender_SKIP = 0x6CAA93;
static void _declspec(naked) HOOK_CPlane__PreRender()
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

// Fixes increased frequency of water cannon pushing peds on high FPS.
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

// Fixes excessive particle spawning from water cannons on high FPS.
#define HOOKPOS_CWaterCannon__Render_FxFix 0x729437
#define HOOKSIZE_CWaterCannon__Render_FxFix 0x5
static const unsigned int RETURN_CWaterCannon__Render_FxFix = 0x729440;
static const unsigned int RETURN_CWaterCannon__Render_FxFix_SKIP = 0x7294EE;
static void _declspec(naked) HOOK_CWaterCannon__Render_FxFix()
{
    _asm {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        jmp RETURN_CWaterCannon__Render_FxFix
    skip:
        jmp RETURN_CWaterCannon__Render_FxFix_SKIP
    }
}

// Fixes excessive particle spawning with setPedHeadless on high FPS.
#define HOOKPOS_CPed__PreRenderAfterTest 0x5E7181
#define HOOKSIZE_CPed__PreRenderAfterTest 0x6
static const unsigned int RETURN_CPed__PreRenderAfterTest = 0x5E7187;
static const unsigned int RETURN_CPed__PreRenderAfterTest_SKIP = 0x5E722D;
static void _declspec(naked) HOOK_CPed__PreRenderAfterTest()
{
    _asm {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        mov eax, [ebp+0x46C]

        jmp RETURN_CPed__PreRenderAfterTest
    skip:
        jmp RETURN_CPed__PreRenderAfterTest_SKIP
    }
}

// Fixes excessive particle spawning from boats on high FPS.
#define HOOKPOS_cBuoyancy__AddSplashParticles 0x6C34E0
#define HOOKSIZE_cBuoyancy__AddSplashParticles 0x6
static const unsigned int RETURN_cBuoyancy__AddSplashParticles = 0x6C34E6;
static void _declspec(naked) HOOK_cBuoyancy__AddSplashParticles()
{
    _asm {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        sub esp, 0xE0

        jmp RETURN_cBuoyancy__AddSplashParticles
    skip:
        retn 0x2C
    }
}

// Fixes excessive weather particle spawning on high FPS.
#define HOOKPOS_CWeather__AddRain 0x72AAA8
#define HOOKSIZE_CWeather__AddRain 0x6
static const unsigned int RETURN_CWeather__AddRain = 0x72AAAE;
static void _declspec(naked) HOOK_CWeather__AddRain()
{
    _asm {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        fld ds:[0xC812F0]

        jmp RETURN_CWeather__AddRain

    skip:
        add esp, 0x84
        ret
    }
}

// Fixes excessive damage particle spawning from airplanes on high FPS.
#define HOOKPOS_CPlane__ProcessFlyingCarStuff 0x6CBE4B
#define HOOKSIZE_CPlane__ProcessFlyingCarStuff 0x6
static const unsigned int RETURN_CPlane__ProcessFlyingCarStuff = 0x6CBE51;
static const unsigned int RETURN_CPlane__ProcessFlyingCarStuff_SKIP = 0x6CC0D9;
static void _declspec(naked) HOOK_CPlane__ProcessFlyingCarStuff()
{
    _asm {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        mov edx, ds:[0xB6F03C]

        jmp RETURN_CPlane__ProcessFlyingCarStuff
    skip:
        jmp RETURN_CPlane__ProcessFlyingCarStuff_SKIP
    }
}

// Fixes excessive spawning of sand and water particles from vehicles on high FPS.
#define HOOKPOS_CAutomobile__UpdateWheelMatrix 0x6AA78A
#define HOOKSIZE_CAutomobile__UpdateWheelMatrix 0x5
static const unsigned int RETURN_CAutomobile__UpdateWheelMatrix = 0x6AA78F;
static const unsigned int RETURN_CAutomobile__UpdateWheelMatrix_SKIP = 0x6AAAD0;
static void _declspec(naked) HOOK_CAutomobile__UpdateWheelMatrix()
{
    _asm {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        push 0x3D4CCCCD

        jmp RETURN_CAutomobile__UpdateWheelMatrix
    skip:
        jmp RETURN_CAutomobile__UpdateWheelMatrix_SKIP
    }
}

// Fixes excessive particle spawning from boats on high FPS.
#define HOOKPOS_CVehicle__DoBoatSplashes 0x6DD130
#define HOOKSIZE_CVehicle__DoBoatSplashes 0x6
static const unsigned int RETURN_CVehicle__DoBoatSplashes = 0x6DD136;
static void _declspec(naked) HOOK_CVehicle__DoBoatSplashes()
{
    _asm {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        sub esp, 0x80

        jmp RETURN_CVehicle__DoBoatSplashes
    skip:
        retn 4
    }
}

// Fixes excessive rain particle spawning on vehicles on high FPS.
#define HOOKPOS_CVehicle__AddWaterSplashParticles 0x6DDF60
#define HOOKSIZE_CVehicle__AddWaterSplashParticles 0x6
static const unsigned int RETURN_CVehicle__AddWaterSplashParticles = 0x6DDF66;
static void _declspec(naked) HOOK_CVehicle__AddWaterSplashParticles()
{
    _asm {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        sub esp, 0xC4

        jmp RETURN_CVehicle__AddWaterSplashParticles
    skip:
        ret
    }
}

// Fixes excessive particle spawning from airplanes when damaged on high FPS.
#define HOOKPOS_CPlane__ProcessControl 0x6C939A
#define HOOKSIZE_CPlane__ProcessControl 0x5
static const unsigned int RETURN_CPlane__ProcessControl = 0x6C939F;
static const unsigned int RETURN_CPlane__ProcessControl_SKIP = 0x6C9463;
static void _declspec(naked) HOOK_CPlane__ProcessControl()
{
    _asm {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        lea ecx, [esp+0x3C]
        push ecx

        jmp RETURN_CPlane__ProcessControl
    skip:
        jmp RETURN_CPlane__ProcessControl_SKIP
    }
}

// Fixes excessive exhaust particle spawning from vehicles on high FPS.
#define HOOKPOS_CVehicle__AddExhaustParticles 0x6DE240
#define HOOKSIZE_CVehicle__AddExhaustParticles 0x6
static const unsigned int RETURN_CVehicle__AddExhaustParticles = 0x6DE246;
static void _declspec(naked) HOOK_CVehicle__AddExhaustParticles()
{
    _asm {
        movzx edx, bWouldBeNewFrame
        test edx, edx
        jz skip

        mov eax, fs:0x0

        jmp RETURN_CVehicle__AddExhaustParticles
    skip:
        ret
    }
}

// Fixes excessive particle spawning while swimming on high FPS.
#define HOOKPOS_CTaskSimpleSwim__ProcessEffects 0x68AD3B
#define HOOKSIZE_CTaskSimpleSwim__ProcessEffects 0x6
static const unsigned int RETURN_CTaskSimpleSwim__ProcessEffects = 0x68AD41;
static const unsigned int RETURN_CTaskSimpleSwim__ProcessEffects_SKIP = 0x68AFDB;
static void _declspec(naked) HOOK_CTaskSimpleSwim__ProcessEffects()
{
    _asm {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        mov eax, [edi+0x14]
        add eax, 0x10

        jmp RETURN_CTaskSimpleSwim__ProcessEffects
    skip:
        jmp RETURN_CTaskSimpleSwim__ProcessEffects_SKIP
    }
}

// Fixes excessive particle spawning while swimming on high FPS.
#define HOOKPOS_CTaskSimpleSwim__ProcessEffectsBubbleFix 0x68AC31
#define HOOKSIZE_CTaskSimpleSwim__ProcessEffectsBubbleFix 0x7
static const unsigned int RETURN_CTaskSimpleSwim__ProcessEffectsBubbleFix = 0x68AC38;
static const unsigned int RETURN_CTaskSimpleSwim__ProcessEffectsBubbleFix_SKIP = 0x68AD36;
static void _declspec(naked) HOOK_CTaskSimpleSwim__ProcessEffectsBubbleFix()
{
    _asm {
        movzx eax, bWouldBeNewFrame
        test eax, eax
        jz skip

        mov ecx, edi
        mov esi, 5

        jmp RETURN_CTaskSimpleSwim__ProcessEffectsBubbleFix
    skip:
        jmp RETURN_CTaskSimpleSwim__ProcessEffectsBubbleFix_SKIP
    }
}

// Fixes invisible weapon particles (extinguisher, spraycan, flamethrower) at high FPS
#define HOOKPOS_CWeapon_Update 0x73DC3D
#define HOOKSIZE_CWeapon_Update 5
static constexpr std::uintptr_t RETURN_CWeapon_Update = 0x073DC42;
static void __declspec(naked) HOOK_CWeapon_Update()
{
    _asm
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
}

#define HOOKPOS_CPhysical__ApplyAirResistance  0x544D29
#define HOOKSIZE_CPhysical__ApplyAirResistance 5
static const unsigned int    RETURN_CPhysical__ApplyAirResistance = 0x544D4D;
static void _declspec(naked) HOOK_CPhysical__ApplyAirResistance()
{
    _asm {
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
}

template <unsigned int returnAddress>
static void _declspec(naked) HOOK_VehicleRapidStopFix()
{
    static unsigned int RETURN_VehicleRapidStopFix = returnAddress;
    _asm {
        fld ds:[0xC2B9CC]            // mod_HandlingManager.m_fWheelFriction
        fmul ds:[0xB7CB5C]            // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep            // 1.666f
        jmp RETURN_VehicleRapidStopFix
    }
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

    // Fixes slow camera movement towards the back of the vehicle on high FPS.
    // CCam::Process_FollowCar_SA
    MemSet((void*)0x524FD7, 0x90, 0x1B);

    // Fixes slow boat movement on high FPS.
    // CVehicle::ProcessBoatControl
    MemPut(0x6DC23F, &kOriginalTimeStep);

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
    EZHookInstall(CPlane__ProcessControl);
    EZHookInstall(CVehicle__AddExhaustParticles);
    EZHookInstall(CTaskSimpleSwim__ProcessEffects);
    EZHookInstall(CTaskSimpleSwim__ProcessEffectsBubbleFix);

    EZHookInstall(CWeapon_Update);
}
