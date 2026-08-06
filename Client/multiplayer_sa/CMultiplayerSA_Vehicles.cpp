/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Vehicles.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

static bool __fastcall AreVehicleDoorsUndamageable(CVehicleSAInterface* vehicle)
{
    SClientEntity<CVehicleSA>* pair = pGameInterface->GetPools()->GetVehicle((DWORD*)vehicle);

    if (!pair)
        return false;

    return pair->pEntity->AreDoorsUndamageable();
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CDamageManager::ProgressDoorDamage
//
// This hook checks if our CVehicleSA instance prevents door damage
//
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C2320 | 53             | push    ebx
//     0x6C2321 | 56             | push    esi
//     0x6C2322 | 0F B6 74 24 0C | movzx   esi, [esp + doorId]
//     0x6C2327 | 85 F6          | test    esi, esi
#define HOOKPOS_CDamageManager__ProgressDoorDamage  0x6C2320
#define HOOKSIZE_CDamageManager__ProgressDoorDamage 7
static DWORD CONTINUE_CDamageManager__ProgressDoorDamage = 0x6C2327;

static void __declspec(naked) HOOK_CDamageManager__ProgressDoorDamage()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        mov     ecx, [esp + 08h]        // CAutomobileSAInterface*
        call    AreVehicleDoorsUndamageable
        test    al, al
        jz      continueGameCodeLocation

        popad
        mov     al, 0
        retn    8

        continueGameCodeLocation:
        popad
        push    ebx
        push    esi
        movzx   esi, [esp + 0Ch]
        jmp     CONTINUE_CDamageManager__ProgressDoorDamage
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CBike::ProcessControl
//
// Both divide the steer angle by the steering lock in radians to get a steer ratio. The steer
// angle is itself the steering lock times the input, so a lock of zero makes this 0 / 0, a NaN
// rather than an infinity. Cars survive the same division because an infinity trips their
// "greater than 1" clamp, while a NaN compares false against every clamp. It reaches the rider
// lean angle, and CBike::CalculateLeanMatrix builds the bike and rider matrices from it, which
// is what stretches the ped until respawn.
//
// A lock of zero means the bike cannot steer, so the ratio is zero. These produce it directly.
//
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6BAB93 | D9 87 A0 00 00 00 | fld     dword ptr [edi + 0A0h]  ; m_fSteeringLock
//     0x6BAB99 | D8 0D EC 95 85 00 | fmul    dword ptr ds:[08595ECh] ; degrees to radians
//     0x6BAB9F | DE F9             | fdivp   st(1), st(0)
#define HOOKPOS_CBike__ProcessControl_SteerRatio  0x6BAB93
#define HOOKSIZE_CBike__ProcessControl_SteerRatio 14
static DWORD CONTINUE_CBike__ProcessControl_SteerRatio = 0x6BABA1;

// This one divides a value already on the FPU stack, so zero has to replace it.
static void __declspec(naked) HOOK_CBike__ProcessControl_SteerRatio()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        mov     eax, [edi + 0A0h]
        test    eax, 7FFFFFFFh              // zero, ignoring the sign bit
        jz      noSteeringLock
        pop     eax

        fld     dword ptr [edi + 0A0h]
        fmul    dword ptr ds:[08595ECh]
        fdivp   st(1), st(0)
        jmp     CONTINUE_CBike__ProcessControl_SteerRatio

        noSteeringLock:
        pop     eax
        fstp    st(0)
        fldz
        jmp     CONTINUE_CBike__ProcessControl_SteerRatio
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6BBA49 | D9 81 A0 00 00 00 | fld     dword ptr [ecx + 0A0h]  ; m_fSteeringLock
//     0x6BBA4F | D8 0D EC 95 85 00 | fmul    dword ptr ds:[08595ECh] ; degrees to radians
//     0x6BBA55 | D8 BE 94 04 00 00 | fdivr   dword ptr [esi + 494h]  ; m_fSteerAngle
#define HOOKPOS_CBike__ProcessControl_LeanRatio  0x6BBA49
#define HOOKSIZE_CBike__ProcessControl_LeanRatio 18
static DWORD CONTINUE_CBike__ProcessControl_LeanRatio = 0x6BBA5B;

// This one pushes a new value onto the FPU stack, so zero is simply pushed instead.
static void __declspec(naked) HOOK_CBike__ProcessControl_LeanRatio()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        mov     eax, [ecx + 0A0h]
        test    eax, 7FFFFFFFh
        jz      noSteeringLock
        pop     eax

        fld     dword ptr [ecx + 0A0h]
        fmul    dword ptr ds:[08595ECh]
        fdivr   dword ptr [esi + 494h]
        jmp     CONTINUE_CBike__ProcessControl_LeanRatio

        noSteeringLock:
        pop     eax
        fldz
        jmp     CONTINUE_CBike__ProcessControl_LeanRatio
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Vehicles
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Vehicles()
{
    EZHookInstall(CDamageManager__ProgressDoorDamage);
    EZHookInstall(CBike__ProcessControl_SteerRatio);
    EZHookInstall(CBike__ProcessControl_LeanRatio);
}
