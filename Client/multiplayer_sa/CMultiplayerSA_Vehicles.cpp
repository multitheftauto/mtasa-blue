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
#include <enums/VehicleType.h>

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
// The Cement Truck's drum on custom vehicle models
//
// Same split again: ProcessControl decides whether to turn the drum and whether to reach the code
// that works out the force it applies, and PreRender decides whether to draw it turned. A clone
// carrying its own model ID matches none of the checks, so the drum just sits still.
//
// The force calculation site is shared with the fire truck's ladder, which keeps its own separate
// comparison right after this one; only the Cement Truck's own comparison is touched here.
//
// Every hook below asks the same question, whether this vehicle should behave as a Cement Truck,
// and lets the model it was cloned from count. Everything else keeps the model index it already had.
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool __fastcall IsCementTruckOrClone(CVehicleSAInterface* vehicle)
{
    const std::uint32_t modelId = static_cast<std::uint32_t>(vehicle->m_nModelIndex);
    if (modelId == static_cast<std::uint32_t>(VehicleType::VT_CEMENT))
        return true;

    CModelInfo* modelInfo = pGameInterface->GetModelInfo(modelId);
    return modelInfo && modelInfo->GetParentID() == static_cast<unsigned int>(VehicleType::VT_CEMENT);
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::ProcessControl, resetting the drum's previous angle for this tick
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A149F | 66 81 F9 0C 02 | cmp     cx, 0x20C
// >>> 0x6A14A4 | 74 0E          | je      0x6A14B4
//     0x6A14A6 | 66 81 F9 50 02 | cmp     cx, 0x250
#define HOOKPOS_CAutomobile__ProcessControl_CementAngleReset  0x6A149F
#define HOOKSIZE_CAutomobile__ProcessControl_CementAngleReset 7
static const DWORD CONTINUE_CAutomobile__ProcessControl_CementAngleReset = 0x6A14B4;
static const DWORD SKIP_CAutomobile__ProcessControl_CementAngleReset = 0x6A14A6;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_CementAngleReset()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        mov     ecx, edi
        call    IsCementTruckOrClone
        test    al, al
        pop     ecx
        jz      notCement

        jmp     CONTINUE_CAutomobile__ProcessControl_CementAngleReset

        notCement:
        jmp     SKIP_CAutomobile__ProcessControl_CementAngleReset
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::ProcessControl, letting the drum into the block that turns it this tick
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A1502 | 66 81 F9 0C 02 | cmp     cx, 0x20C
// >>> 0x6A1507 | 74 53          | je      0x6A155C
//     0x6A1509 | 66 81 F9 50 02 | cmp     cx, 0x250
#define HOOKPOS_CAutomobile__ProcessControl_CementMiscGate  0x6A1502
#define HOOKSIZE_CAutomobile__ProcessControl_CementMiscGate 7
static const DWORD CONTINUE_CAutomobile__ProcessControl_CementMiscGate = 0x6A155C;
static const DWORD SKIP_CAutomobile__ProcessControl_CementMiscGate = 0x6A1509;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_CementMiscGate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        mov     ecx, edi
        call    IsCementTruckOrClone
        test    al, al
        pop     ecx
        jz      notCement

        jmp     CONTINUE_CAutomobile__ProcessControl_CementMiscGate

        notCement:
        jmp     SKIP_CAutomobile__ProcessControl_CementMiscGate
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// The force the drum applies to whatever it pushes
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A1560 | 66 3D 0C 02       | cmp     ax, 0x20C
// >>> 0x6A1564 | 0F 84 0B 08 00 00 | je      0x6A1D75
//     0x6A156A | 66 3D 20 02       | cmp     ax, 0x220
#define HOOKPOS_CAutomobile__MovingCollisionSpeed_Cement  0x6A1560
#define HOOKSIZE_CAutomobile__MovingCollisionSpeed_Cement 10
static const DWORD CONTINUE_CAutomobile__MovingCollisionSpeed_Cement = 0x6A1D75;
static const DWORD SKIP_CAutomobile__MovingCollisionSpeed_Cement = 0x6A156A;

static void __declspec(naked) HOOK_CAutomobile__MovingCollisionSpeed_Cement()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        mov     ecx, edi
        call    IsCementTruckOrClone
        test    al, al
        pop     eax
        jz      notCement

        jmp     CONTINUE_CAutomobile__MovingCollisionSpeed_Cement

        notCement:
        jmp     SKIP_CAutomobile__MovingCollisionSpeed_Cement
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::PreRender, turning the drum
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6AC43B | 66 3D 0C 02       | cmp     ax, 0x20C
// >>> 0x6AC43F | 0F 85 94 00 00 00 | jne     0x6AC4D9
//     0x6AC445 | 8A 56 36          | mov     dl, byte ptr [esi + 0x36]
#define HOOKPOS_CAutomobile__PreRender_CementTurn  0x6AC43B
#define HOOKSIZE_CAutomobile__PreRender_CementTurn 10
static const DWORD CONTINUE_CAutomobile__PreRender_CementTurn = 0x6AC445;
static const DWORD SKIP_CAutomobile__PreRender_CementTurn = 0x6AC4D9;

static void __declspec(naked) HOOK_CAutomobile__PreRender_CementTurn()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        mov     ecx, esi
        call    IsCementTruckOrClone
        test    al, al
        pop     eax
        jz      notCement

        jmp     CONTINUE_CAutomobile__PreRender_CementTurn

        notCement:
        jmp     SKIP_CAutomobile__PreRender_CementTurn
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::ProcessControl, the model switch that reaches UpdateMovingCollision at all
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6B1FA1 | 66 3D 0C 02 | cmp     ax, 0x20C
// >>> 0x6B1FA5 | 74 68       | je      0x6B200F
//     0x6B1FA7 | 66 3D 50 02 | cmp     ax, 0x250
#define HOOKPOS_CAutomobile__ProcessControl_CementDispatch  0x6B1FA1
#define HOOKSIZE_CAutomobile__ProcessControl_CementDispatch 6
static const DWORD CONTINUE_CAutomobile__ProcessControl_CementDispatch = 0x6B200F;
static const DWORD SKIP_CAutomobile__ProcessControl_CementDispatch = 0x6B1FA7;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_CementDispatch()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        mov     ecx, esi
        call    IsCementTruckOrClone
        test    al, al
        pop     eax
        jz      notCement

        jmp     CONTINUE_CAutomobile__ProcessControl_CementDispatch

        notCement:
        jmp     SKIP_CAutomobile__ProcessControl_CementDispatch
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// The Dozer's blade on custom vehicle models
//
// CAutomobile::ProcessControl and CAutomobile::PreRender both decide whether to move the blade
// (component misc_a) through their own separate model index checks, and the force the blade applies
// to whatever it pushes is worked out the same way in a third place. A model created by
// engineRequestModel carries an ID of its own, so a cloned Dozer matches none of them and the blade
// just sits still.
//
// Every hook below asks the same question, whether this vehicle should behave as a Dozer, and lets
// the model it was cloned from count. Everything else keeps the model index it already had.
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool __fastcall IsDozerOrClone(CVehicleSAInterface* vehicle)
{
    const std::uint32_t modelId = static_cast<std::uint32_t>(vehicle->m_nModelIndex);
    if (modelId == static_cast<std::uint32_t>(VehicleType::VT_DOZER))
        return true;

    CModelInfo* modelInfo = pGameInterface->GetModelInfo(modelId);
    return modelInfo && modelInfo->GetParentID() == static_cast<unsigned int>(VehicleType::VT_DOZER);
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::ProcessControl, resetting the blade's previous angle for this tick
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A1491 | 66 81 F9 E6 01 | cmp     cx, 0x1E6
// >>> 0x6A1496 | 74 1C          | je      0x6A14B4
//     0x6A1498 | 66 81 F9 96 01 | cmp     cx, 0x196
#define HOOKPOS_CAutomobile__ProcessControl_DozerAngleReset  0x6A1491
#define HOOKSIZE_CAutomobile__ProcessControl_DozerAngleReset 7
static const DWORD CONTINUE_CAutomobile__ProcessControl_DozerAngleReset = 0x6A14B4;
static const DWORD SKIP_CAutomobile__ProcessControl_DozerAngleReset = 0x6A1498;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_DozerAngleReset()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        mov     ecx, edi
        call    IsDozerOrClone
        test    al, al
        pop     ecx
        jz      notDozer

        jmp     CONTINUE_CAutomobile__ProcessControl_DozerAngleReset

        notDozer:
        jmp     SKIP_CAutomobile__ProcessControl_DozerAngleReset
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::ProcessControl, letting the blade into the block that moves it this tick
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A14F4 | 66 81 F9 E6 01 | cmp     cx, 0x1E6
// >>> 0x6A14F9 | 74 61          | je      0x6A155C
//     0x6A14FB | 66 81 F9 96 01 | cmp     cx, 0x196
#define HOOKPOS_CAutomobile__ProcessControl_DozerMiscGate  0x6A14F4
#define HOOKSIZE_CAutomobile__ProcessControl_DozerMiscGate 7
static const DWORD CONTINUE_CAutomobile__ProcessControl_DozerMiscGate = 0x6A155C;
static const DWORD SKIP_CAutomobile__ProcessControl_DozerMiscGate = 0x6A14FB;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_DozerMiscGate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        mov     ecx, edi
        call    IsDozerOrClone
        test    al, al
        pop     ecx
        jz      notDozer

        jmp     CONTINUE_CAutomobile__ProcessControl_DozerMiscGate

        notDozer:
        jmp     SKIP_CAutomobile__ProcessControl_DozerMiscGate
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// The force the blade applies to whatever it pushes, part one
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A173F | 66 3D E6 01          | cmp     ax, 0x1E6
// >>> 0x6A1743 | 89 9C 24 F8 00 00 00 | mov     dword ptr [esp + 0xF8], ebx
// >>> 0x6A174A | 75 50                | jne     0x6A179C
#define HOOKPOS_CAutomobile__MovingCollisionSpeed_Dozer  0x6A173F
#define HOOKSIZE_CAutomobile__MovingCollisionSpeed_Dozer 13
static const DWORD CONTINUE_CAutomobile__MovingCollisionSpeed_Dozer = 0x6A174C;
static const DWORD SKIP_CAutomobile__MovingCollisionSpeed_Dozer = 0x6A179C;

static void __declspec(naked) HOOK_CAutomobile__MovingCollisionSpeed_Dozer()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // ebx is already zero here (xor ebx, ebx a few instructions up), and this reset happens for
        // every model that reaches this point, not only a matched one, so it stays unconditional.
        mov     dword ptr [esp + 0xF8], ebx

        push    eax
        mov     ecx, edi
        call    IsDozerOrClone
        test    al, al
        pop     eax
        jz      notDozer

        jmp     CONTINUE_CAutomobile__MovingCollisionSpeed_Dozer

        notDozer:
        jmp     SKIP_CAutomobile__MovingCollisionSpeed_Dozer
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// The force the blade applies to whatever it pushes, part two
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A1FB6 | 66 81 F9 E6 01 | cmp     cx, 0x1E6
// >>> 0x6A1FBB | 75 14          | jne     0x6A1FD1
//     0x6A1FBD | 8B 86 98 06 00 00 | mov  eax, [esi + 0x698]
#define HOOKPOS_CAutomobile__MovingCollisionSpeed_DozerExtraA  0x6A1FB6
#define HOOKSIZE_CAutomobile__MovingCollisionSpeed_DozerExtraA 7
static const DWORD CONTINUE_CAutomobile__MovingCollisionSpeed_DozerExtraA = 0x6A1FBD;
static const DWORD SKIP_CAutomobile__MovingCollisionSpeed_DozerExtraA = 0x6A1FD1;

static void __declspec(naked) HOOK_CAutomobile__MovingCollisionSpeed_DozerExtraA()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        mov     ecx, esi
        call    IsDozerOrClone
        test    al, al
        pop     ecx
        jz      notDozer

        jmp     CONTINUE_CAutomobile__MovingCollisionSpeed_DozerExtraA

        notDozer:
        jmp     SKIP_CAutomobile__MovingCollisionSpeed_DozerExtraA
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// The force the blade applies to whatever it pushes, part three
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A21A4 | 66 3D E6 01 | cmp     ax, 0x1E6
// >>> 0x6A21A8 | 75 1E       | jne     0x6A21C8
//     0x6A21AA | 8B B1 98 06 00 00 | mov ptr esi, [ecx + 0x698]
#define HOOKPOS_CAutomobile__MovingCollisionSpeed_DozerExtraB  0x6A21A4
#define HOOKSIZE_CAutomobile__MovingCollisionSpeed_DozerExtraB 6
static const DWORD CONTINUE_CAutomobile__MovingCollisionSpeed_DozerExtraB = 0x6A21AA;
static const DWORD SKIP_CAutomobile__MovingCollisionSpeed_DozerExtraB = 0x6A21C8;

static void __declspec(naked) HOOK_CAutomobile__MovingCollisionSpeed_DozerExtraB()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        push    ecx
        call    IsDozerOrClone
        test    al, al
        pop     ecx
        pop     eax
        jz      notDozer

        jmp     CONTINUE_CAutomobile__MovingCollisionSpeed_DozerExtraB

        notDozer:
        jmp     SKIP_CAutomobile__MovingCollisionSpeed_DozerExtraB
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::PreRender, swinging the misc_a component out (Dozer blade)
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6AC40C | 66 3D E6 01 | cmp     ax, 0x1E6
// >>> 0x6AC410 | 75 29       | jne     0x6AC43B
//     0x6AC412 | 0F B7 86 6C 08 00 00 | movzx eax, word ptr [esi + 0x86C]
#define HOOKPOS_CAutomobile__PreRender_DozerSwing  0x6AC40C
#define HOOKSIZE_CAutomobile__PreRender_DozerSwing 6
static const DWORD CONTINUE_CAutomobile__PreRender_DozerSwing = 0x6AC412;
static const DWORD SKIP_CAutomobile__PreRender_DozerSwing = 0x6AC43B;

static void __declspec(naked) HOOK_CAutomobile__PreRender_DozerSwing()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        mov     ecx, esi
        call    IsDozerOrClone
        test    al, al
        pop     eax
        jz      notDozer

        jmp     CONTINUE_CAutomobile__PreRender_DozerSwing

        notDozer:
        jmp     SKIP_CAutomobile__PreRender_DozerSwing
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::ProcessControl, the model switch that reaches UpdateMovingCollision at all
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6B1F95 | 66 3D E6 01 | cmp     ax, 0x1E6
// >>> 0x6B1F99 | 74 74       | je      0x6B200F
//     0x6B1F9B | 66 3D 96 01 | cmp     ax, 0x196
#define HOOKPOS_CAutomobile__ProcessControl_DozerDispatch  0x6B1F95
#define HOOKSIZE_CAutomobile__ProcessControl_DozerDispatch 6
static const DWORD CONTINUE_CAutomobile__ProcessControl_DozerDispatch = 0x6B200F;
static const DWORD SKIP_CAutomobile__ProcessControl_DozerDispatch = 0x6B1F9B;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_DozerDispatch()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        mov     ecx, esi
        call    IsDozerOrClone
        test    al, al
        pop     eax
        jz      notDozer

        jmp     CONTINUE_CAutomobile__ProcessControl_DozerDispatch

        notDozer:
        jmp     SKIP_CAutomobile__ProcessControl_DozerDispatch
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Pop up headlights on custom vehicle models
//
// The ZR-350 is the one car whose headlights swing out before they light up, and the game drives it
// from two model index checks. CAutomobile::PreRender turns the misc_a component through
// CVehicle::SetComponentRotation to deploy them, and CVehicle::DoVehicleLights waits for that
// deployment to finish before it lets the headlights come on. A model created by engineRequestModel
// carries an ID of its own, so a cloned ZR-350 misses both and its headlights stay shut.
//
// Both hooks answer the same question, whether this vehicle should behave as a ZR-350, and let the
// model it was cloned from count. Everything else keeps the model index it already had.
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool __fastcall HasPopUpHeadlights(CVehicleSAInterface* vehicle)
{
    const std::uint32_t modelId = static_cast<std::uint32_t>(vehicle->m_nModelIndex);
    if (modelId == static_cast<std::uint32_t>(VehicleType::VT_ZR350))
        return true;

    CModelInfo* modelInfo = pGameInterface->GetModelInfo(modelId);
    return modelInfo && modelInfo->GetParentID() == static_cast<unsigned int>(VehicleType::VT_ZR350);
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::PreRender, swinging the misc_a component out (ZR-350 headlights)
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6ACA8D | 66 3D DD 01       | cmp     ax, 0x1DD
// >>> 0x6ACA91 | 0F 85 30 01 00 00 | jne     0x6ACBC7
//     0x6ACA97 | 8B 86 58 09 00 00 | mov     eax, [esi + 0x958]
#define HOOKPOS_CAutomobile__PreRender_PopUpLights  0x6ACA8D
#define HOOKSIZE_CAutomobile__PreRender_PopUpLights 10
static const DWORD CONTINUE_CAutomobile__PreRender_PopUpLights = 0x6ACA97;
static const DWORD SKIP_CAutomobile__PreRender_PopUpLights = 0x6ACBC7;

static void __declspec(naked) HOOK_CAutomobile__PreRender_PopUpLights()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    esi
        mov     ecx, esi
        call    HasPopUpHeadlights
        test    al, al          // pop leaves the flags alone, so this survives it
        pop     esi
        jz      notPopUpLights

        jmp     CONTINUE_CAutomobile__PreRender_PopUpLights

        notPopUpLights:
        jmp     SKIP_CAutomobile__PreRender_PopUpLights
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CVehicle::DoVehicleLights, holding the headlights back until they are out
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6E1C17 | 66 81 7E 22 DD 01 | cmp     word ptr [esi + 0x22], 0x1DD
// >>> 0x6E1C1D | 0F 85 AB 00 00 00 | jne     0x6E1CCE
//     0x6E1C23 | 8A 56 36          | mov     dl, byte ptr [esi + 0x36]
#define HOOKPOS_CVehicle__DoVehicleLights_PopUpLights  0x6E1C17
#define HOOKSIZE_CVehicle__DoVehicleLights_PopUpLights 12
static const DWORD CONTINUE_CVehicle__DoVehicleLights_PopUpLights = 0x6E1C23;
static const DWORD SKIP_CVehicle__DoVehicleLights_PopUpLights = 0x6E1CCE;

static void __declspec(naked) HOOK_CVehicle__DoVehicleLights_PopUpLights()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    esi
        push    ecx
        mov     ecx, esi
        call    HasPopUpHeadlights
        test    al, al
        pop     ecx
        pop     esi
        jz      notPopUpLights

        jmp     CONTINUE_CVehicle__DoVehicleLights_PopUpLights

        notPopUpLights:
        jmp     SKIP_CVehicle__DoVehicleLights_PopUpLights
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::GetTowBarPos
//
// This hook lets trailers attach to trucks created with engineRequestModel.
//
// CVehicle::UpdateTractorLink asks the truck where its tow bar sits every frame, and the answer
// comes from a switch over the model index; the trucks built to pull a trailer return their misc_a
// dummy, everything else gets a generic point off the front bumper. A cloned truck carries a model
// ID of its own, so it matches no case and the trailer hangs off the wrong end.
//
// Redirecting just the model index that switch reads, to the ID the model was cloned from, is
// enough. The branch it then takes reads nothing but the vehicle's own misc_a dummy, so the clone
// gets its tow bar straight out of its own DFF.
//
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6AF250 | 66 8B 41 22 | mov     ax, word ptr [ecx + 0x22]
// >>> 0x6AF254 | 83 EC 0C    | sub     esp, 0xC
//     0x6AF257 | 66 3D 0D 02 | cmp     ax, 0x20D
#define HOOKPOS_CAutomobile__GetTowBarPos  0x6AF250
#define HOOKSIZE_CAutomobile__GetTowBarPos 7
static const DWORD CONTINUE_CAutomobile__GetTowBarPos = 0x6AF257;

// Models that answer with their misc_a dummy, so a clone of one still tows.
//
// The tow truck and the tractor are left out; they derive the bar from their hoist angle, and the
// rest of their behaviour stays gated on the stock model index, so a clone would only half work.
// The trailers in that same switch, artict3 and the two baggage boxes, are vehicles.ide type
// trailer and so become CTrailer, which overrides this function and never reaches it.
static constexpr bool HasTowBarDummy(VehicleType model)
{
    switch (model)
    {
        case VehicleType::VT_LINERUN:
        case VehicleType::VT_BAGGAGE:
        case VehicleType::VT_PETRO:
        case VehicleType::VT_RDTRAIN:
        case VehicleType::VT_UTILITY:
        case VehicleType::VT_TUG:
            return true;
        default:
            return false;
    }
}

static std::uint32_t __fastcall GetTowBarModelId(CVehicleSAInterface* vehicle)
{
    const std::uint32_t modelId = static_cast<std::uint32_t>(vehicle->m_nModelIndex);

    CModelInfo* modelInfo = pGameInterface->GetModelInfo(modelId);
    if (!modelInfo)
        return modelId;

    // Only engineRequestModel models carry a parent, so stock vehicles keep their index and take
    // the branch they always did.
    const std::uint32_t parentId = modelInfo->GetParentID();
    if (parentId == 0 || !HasTowBarDummy(static_cast<VehicleType>(parentId)))
        return modelId;

    return parentId;
}

static void __declspec(naked) HOOK_CAutomobile__GetTowBarPos()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        call    GetTowBarModelId        // vehicle in ecx; model index comes back in ax
        pop     ecx

        sub     esp, 0x0C
        jmp     CONTINUE_CAutomobile__GetTowBarPos
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// The Dumper's tipping ramp on custom vehicle models
//
// Built on CMonsterTruck, not plain CAutomobile: ProcessControl decides whether the ramp moves,
// UpdateMovingCollision moves it and picks the rotation formula, GetMovingCollisionOffset answers
// how far it's tipped, and CMonsterTruck::PreRender does the actual swing. All gate on model index
// alone, so a clone matches none of them. IsSubMonsterTruck() (a runtime flag, not model index)
// already recognises a clone and is left untouched.
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool __fastcall IsDumperOrClone(CVehicleSAInterface* vehicle)
{
    const std::uint32_t modelId = static_cast<std::uint32_t>(vehicle->m_nModelIndex);
    if (modelId == static_cast<std::uint32_t>(VehicleType::VT_DUMPER))
        return true;

    CModelInfo* modelInfo = pGameInterface->GetModelInfo(modelId);
    return modelInfo && modelInfo->GetParentID() == static_cast<unsigned int>(VehicleType::VT_DUMPER);
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::ProcessControl, the switch that reaches UpdateMovingCollision at all
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6B1F9B | 66 3D 96 01 | cmp     ax, 0x196
// >>> 0x6B1F9F | 74 6E       | je      0x6B200F
//     0x6B1FA1 | 66 3D 0C 02 | cmp     ax, 0x20C
#define HOOKPOS_CAutomobile__ProcessControl_DumperDispatch  0x6B1F9B
#define HOOKSIZE_CAutomobile__ProcessControl_DumperDispatch 6
static const DWORD CONTINUE_CAutomobile__ProcessControl_DumperDispatch = 0x6B200F;
static const DWORD SKIP_CAutomobile__ProcessControl_DumperDispatch = 0x6B1FA1;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_DumperDispatch()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // ax (model id) is read again on skip
        push    eax
        mov     ecx, esi
        call    IsDumperOrClone
        test    al, al
        pop     eax
        jz      notDumper

        jmp     CONTINUE_CAutomobile__ProcessControl_DumperDispatch

        notDumper:
        jmp     SKIP_CAutomobile__ProcessControl_DumperDispatch
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::UpdateMovingCollision, resetting the ramp's previous angle for this tick
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A1498 | 66 81 F9 96 01 | cmp     cx, 0x196
// >>> 0x6A149D | 74 15          | je      0x6A14B4
//     0x6A149F | 66 81 F9 0C 02 | cmp     cx, 0x20C
#define HOOKPOS_CAutomobile__UpdateMovingCollision_DumperAngleReset  0x6A1498
#define HOOKSIZE_CAutomobile__UpdateMovingCollision_DumperAngleReset 7
static const DWORD CONTINUE_CAutomobile__UpdateMovingCollision_DumperAngleReset = 0x6A14B4;
static const DWORD SKIP_CAutomobile__UpdateMovingCollision_DumperAngleReset = 0x6A149F;

static void __declspec(naked) HOOK_CAutomobile__UpdateMovingCollision_DumperAngleReset()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // cx (model id) is read again on skip
        push    ecx
        mov     ecx, edi
        call    IsDumperOrClone
        test    al, al
        pop     ecx
        jz      notDumper

        jmp     CONTINUE_CAutomobile__UpdateMovingCollision_DumperAngleReset

        notDumper:
        jmp     SKIP_CAutomobile__UpdateMovingCollision_DumperAngleReset
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::UpdateMovingCollision, letting the ramp into the block that moves it this tick
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A14FB | 66 81 F9 96 01 | cmp     cx, 0x196
// >>> 0x6A1500 | 74 5A          | je      0x6A155C
//     0x6A1502 | 66 81 F9 0C 02 | cmp     cx, 0x20C
#define HOOKPOS_CAutomobile__UpdateMovingCollision_DumperMiscGate  0x6A14FB
#define HOOKSIZE_CAutomobile__UpdateMovingCollision_DumperMiscGate 7
static const DWORD CONTINUE_CAutomobile__UpdateMovingCollision_DumperMiscGate = 0x6A155C;
static const DWORD SKIP_CAutomobile__UpdateMovingCollision_DumperMiscGate = 0x6A1502;

static void __declspec(naked) HOOK_CAutomobile__UpdateMovingCollision_DumperMiscGate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // cx (model id) is read again on skip
        push    ecx
        mov     ecx, edi
        call    IsDumperOrClone
        test    al, al
        pop     ecx
        jz      notDumper

        jmp     CONTINUE_CAutomobile__UpdateMovingCollision_DumperMiscGate

        notDumper:
        jmp     SKIP_CAutomobile__UpdateMovingCollision_DumperMiscGate
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::UpdateMovingCollision, picking the ramp's own rotation formula
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A179C | 66 3D 96 01 | cmp     ax, 0x196
// >>> 0x6A17A0 | 75 4D       | jne     0x6A17EF
//     0x6A17A2 | 83 BF 94 05 00 00 01 | cmp dword ptr [edi + 0x594], 1
#define HOOKPOS_CAutomobile__UpdateMovingCollision_DumperRotation  0x6A179C
#define HOOKSIZE_CAutomobile__UpdateMovingCollision_DumperRotation 6
static const DWORD CONTINUE_CAutomobile__UpdateMovingCollision_DumperRotation = 0x6A17A2;
static const DWORD SKIP_CAutomobile__UpdateMovingCollision_DumperRotation = 0x6A17EF;

static void __declspec(naked) HOOK_CAutomobile__UpdateMovingCollision_DumperRotation()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // ax (model id) is read again on skip
        push    eax
        mov     ecx, edi
        call    IsDumperOrClone
        test    al, al
        pop     eax
        jz      notDumper

        jmp     CONTINUE_CAutomobile__UpdateMovingCollision_DumperRotation

        notDumper:
        jmp     SKIP_CAutomobile__UpdateMovingCollision_DumperRotation
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::GetMovingCollisionOffset, the ramp's case in the model switch
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A2166 | 66 3D 96 01 | cmp     ax, 0x196
// >>> 0x6A216A | 75 1E       | jne     0x6A218A
//     0x6A216C | 8B B1 A0 06 00 00 | mov esi, [ecx + 0x6A0]
#define HOOKPOS_CAutomobile__GetMovingCollisionOffset_Dumper  0x6A2166
#define HOOKSIZE_CAutomobile__GetMovingCollisionOffset_Dumper 6
static const DWORD CONTINUE_CAutomobile__GetMovingCollisionOffset_Dumper = 0x6A216C;
static const DWORD SKIP_CAutomobile__GetMovingCollisionOffset_Dumper = 0x6A218A;

static void __declspec(naked) HOOK_CAutomobile__GetMovingCollisionOffset_Dumper()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // ecx is the vehicle (already the call arg); ax and dx are both read again past here
        push    eax
        push    ecx
        push    edx
        call    IsDumperOrClone
        test    al, al
        pop     edx
        pop     ecx
        pop     eax
        jz      notDumper

        jmp     CONTINUE_CAutomobile__GetMovingCollisionOffset_Dumper

        notDumper:
        jmp     SKIP_CAutomobile__GetMovingCollisionOffset_Dumper
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CMonsterTruck::PreRender, swinging the misc_c component out
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C7F30 | 66 81 7E 22 96 01 | cmp     word ptr [esi + 0x22], 0x196
// >>> 0x6C7F36 | 75 2F             | jne     0x6C7F67
//     0x6C7F38 | 8B 86 A0 06 00 00 | mov     eax, [esi + 0x6A0]
#define HOOKPOS_CMonsterTruck__PreRender_DumperSwing  0x6C7F30
#define HOOKSIZE_CMonsterTruck__PreRender_DumperSwing 8
static const DWORD CONTINUE_CMonsterTruck__PreRender_DumperSwing = 0x6C7F38;
static const DWORD SKIP_CMonsterTruck__PreRender_DumperSwing = 0x6C7F67;

static void __declspec(naked) HOOK_CMonsterTruck__PreRender_DumperSwing()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // esi is callee-saved; nothing past here reads eax/ecx/edx from before the call
        mov     ecx, esi
        call    IsDumperOrClone
        test    al, al
        jz      notDumper

        jmp     CONTINUE_CMonsterTruck__PreRender_DumperSwing

        notDumper:
        jmp     SKIP_CMonsterTruck__PreRender_DumperSwing
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// Aircraft on custom vehicle models
//
// Raw model-index comparisons across the flight model, weapon code, audio and cameras don't
// recognise a clone's own id. The three resolvers below map it back to the base aircraft:
// ResolveAircraftWeaponModelId for individual armed aircraft, ResolveVehicleParentModelId for
// whole vehicle classes, and ResolveJetAudioModelId for the jet engine sound.
//////////////////////////////////////////////////////////////////////////////////////////
static std::uint32_t __fastcall ResolveAircraftWeaponModelId(std::uint32_t modelId)
{
    // Standard models leave m_dwParentID uninitialised, so answer the vanilla range here instead of reading it.
    if (modelId >= 400 && modelId <= 611)
        return modelId;

    CModelInfo* modelInfo = pGameInterface->GetModelInfo(modelId);
    if (!modelInfo)
        return modelId;

    switch (static_cast<VehicleType>(modelInfo->GetParentID()))
    {
        case VehicleType::VT_SEASPAR:
        case VehicleType::VT_SPARROW:
        case VehicleType::VT_HUNTER:
        case VehicleType::VT_HYDRA:
        case VehicleType::VT_RUSTLER:
        case VehicleType::VT_RCBARON:
        case VehicleType::VT_RCTIGER:
        case VehicleType::VT_TORNADO:
        case VehicleType::VT_CARGOBOB:
        case VehicleType::VT_MAVERICK:
        case VehicleType::VT_POLMAV:
            return static_cast<std::uint32_t>(modelInfo->GetParentID());
        default:
            return modelId;
    }
}

static std::uint32_t __fastcall ResolveVehicleParentModelId(std::uint32_t modelId)
{
    // Standard models leave m_dwParentID uninitialised, so answer the vanilla range here instead of reading it.
    if (modelId >= 400 && modelId <= 611)
        return modelId;

    CModelInfo* modelInfo = pGameInterface->GetModelInfo(modelId);
    if (!modelInfo)
        return modelId;

    const std::uint32_t parentId = static_cast<std::uint32_t>(modelInfo->GetParentID());
    return (parentId >= 400 && parentId <= 611) ? parentId : modelId;
}

static std::uint32_t __fastcall ResolveJetAudioModelId(std::uint32_t modelId)
{
    // Standard models leave m_dwParentID uninitialised, so answer the vanilla range here instead of reading it.
    if (modelId >= 400 && modelId <= 611)
        return modelId;

    CModelInfo* modelInfo = pGameInterface->GetModelInfo(modelId);
    if (!modelInfo)
        return modelId;

    switch (static_cast<VehicleType>(modelInfo->GetParentID()))
    {
        case VehicleType::VT_SHAMAL:
        case VehicleType::VT_HYDRA:
        case VehicleType::VT_AT400:
        case VehicleType::VT_ANDROM:
            return static_cast<std::uint32_t>(modelInfo->GetParentID());
        default:
            return modelId;
    }
}

static bool __fastcall IsAndromadaOrClone(CVehicleSAInterface* vehicle)
{
    const std::uint32_t modelId = static_cast<std::uint32_t>(vehicle->m_nModelIndex);
    if (modelId == static_cast<std::uint32_t>(VehicleType::VT_ANDROM))
        return true;

    // Same reasoning as the resolvers above: no other standard model can be a clone.
    if (modelId >= 400 && modelId <= 611)
        return false;

    CModelInfo* modelInfo = pGameInterface->GetModelInfo(modelId);
    return modelInfo && modelInfo->GetParentID() == static_cast<unsigned int>(VehicleType::VT_ANDROM);
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D3F30 | 0F BF 41 22    | movsx   eax, word ptr [ecx + 0x22]
// >>> 0x6D3F34 | 05 57 FE FF FF | add     eax, 0xFFFFFE57
//     0x6D3F39 | 3D 97 00 00 00 | cmp     eax, 0x97
#define HOOKPOS_CVehicle__GetPlaneNumGuns_ModelId  0x6D3F30
#define HOOKSIZE_CVehicle__GetPlaneNumGuns_ModelId 9
static const DWORD CONTINUE_CVehicle__GetPlaneNumGuns_ModelId = 0x6D3F39;

static void __declspec(naked) HOOK_CVehicle__GetPlaneNumGuns_ModelId()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        push    edx
        movzx   ecx, word ptr [ecx + 0x22]
        call    ResolveAircraftWeaponModelId
        pop     edx
        pop     ecx
        add     eax, 0xFFFFFE57
        jmp     CONTINUE_CVehicle__GetPlaneNumGuns_ModelId
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D3E00 | 0F BF 41 22    | movsx   eax, word ptr [ecx + 0x22]
// >>> 0x6D3E04 | 05 57 FE FF FF | add     eax, 0xFFFFFE57
//     0x6D3E09 | 3D 97 00 00 00 | cmp     eax, 0x97
#define HOOKPOS_CVehicle__GetPlaneGunsAutoAimAngle_ModelId  0x6D3E00
#define HOOKSIZE_CVehicle__GetPlaneGunsAutoAimAngle_ModelId 9
static const DWORD CONTINUE_CVehicle__GetPlaneGunsAutoAimAngle_ModelId = 0x6D3E09;

static void __declspec(naked) HOOK_CVehicle__GetPlaneGunsAutoAimAngle_ModelId()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [ecx + 0x22]
        call    ResolveAircraftWeaponModelId
        add     eax, 0xFFFFFE57
        jmp     CONTINUE_CVehicle__GetPlaneGunsAutoAimAngle_ModelId
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D4125 | 0F BF 41 22    | movsx   eax, word ptr [ecx + 0x22]
// >>> 0x6D4129 | 05 57 FE FF FF | add     eax, 0xFFFFFE57
//     0x6D412E | 3D 97 00 00 00 | cmp     eax, 0x97
#define HOOKPOS_CVehicle__GetPlaneGunsRateOfFire_ModelId  0x6D4125
#define HOOKSIZE_CVehicle__GetPlaneGunsRateOfFire_ModelId 9
static const DWORD CONTINUE_CVehicle__GetPlaneGunsRateOfFire_ModelId = 0x6D412E;

static void __declspec(naked) HOOK_CVehicle__GetPlaneGunsRateOfFire_ModelId()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [ecx + 0x22]
        call    ResolveAircraftWeaponModelId
        add     eax, 0xFFFFFE57
        jmp     CONTINUE_CVehicle__GetPlaneGunsRateOfFire_ModelId
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D45D5 | 0F BF 41 22    | movsx   eax, word ptr [ecx + 0x22]
// >>> 0x6D45D9 | 05 57 FE FF FF | add     eax, 0xFFFFFE57
//     0x6D45DE | 3D 5F 00 00 00 | cmp     eax, 0x5F
#define HOOKPOS_CVehicle__GetPlaneOrdnanceRateOfFire_ModelId  0x6D45D5
#define HOOKSIZE_CVehicle__GetPlaneOrdnanceRateOfFire_ModelId 9
static const DWORD CONTINUE_CVehicle__GetPlaneOrdnanceRateOfFire_ModelId = 0x6D45DE;

static void __declspec(naked) HOOK_CVehicle__GetPlaneOrdnanceRateOfFire_ModelId()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [ecx + 0x22]
        call    ResolveAircraftWeaponModelId
        add     eax, 0xFFFFFE57
        jmp     CONTINUE_CVehicle__GetPlaneOrdnanceRateOfFire_ModelId
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D42FE | 8D 81 57 FE FF FF | lea     eax, [ecx + 0xFFFFFE57]
//     0x6D4304 | 3D 97 00 00 00    | cmp     eax, 0x97
#define HOOKPOS_CVehicle__GetPlaneGunsPosition_ModelId  0x6D42FE
#define HOOKSIZE_CVehicle__GetPlaneGunsPosition_ModelId 6
static const DWORD CONTINUE_CVehicle__GetPlaneGunsPosition_ModelId = 0x6D4304;

static void __declspec(naked) HOOK_CVehicle__GetPlaneGunsPosition_ModelId()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    edx
        call    ResolveAircraftWeaponModelId        ; ecx already holds the model id
        pop     edx
        add     eax, 0xFFFFFE57
        jmp     CONTINUE_CVehicle__GetPlaneGunsPosition_ModelId
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D474B | 8D 87 57 FE FF FF | lea     eax, [edi + 0xFFFFFE57]
//     0x6D4751 | 3D 97 00 00 00    | cmp     eax, 0x97
#define HOOKPOS_CVehicle__GetPlaneOrdnancePosition_ModelId  0x6D474B
#define HOOKSIZE_CVehicle__GetPlaneOrdnancePosition_ModelId 6
static const DWORD CONTINUE_CVehicle__GetPlaneOrdnancePosition_ModelId = 0x6D4751;

static void __declspec(naked) HOOK_CVehicle__GetPlaneOrdnancePosition_ModelId()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        push    edx
        mov     ecx, edi
        call    ResolveAircraftWeaponModelId
        pop     edx
        pop     ecx
        add     eax, 0xFFFFFE57
        jmp     CONTINUE_CVehicle__GetPlaneOrdnancePosition_ModelId
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D4900 | 0F BF 41 22    | movsx   eax, word ptr [ecx + 0x22]
// >>> 0x6D4904 | 05 57 FE FF FF | add     eax, 0xFFFFFE57
//     0x6D4909 | 3D 97 00 00 00 | cmp     eax, 0x97
#define HOOKPOS_CVehicle__SelectPlaneWeapon_ModelId  0x6D4900
#define HOOKSIZE_CVehicle__SelectPlaneWeapon_ModelId 9
static const DWORD CONTINUE_CVehicle__SelectPlaneWeapon_ModelId = 0x6D4909;

static void __declspec(naked) HOOK_CVehicle__SelectPlaneWeapon_ModelId()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        push    edx
        movzx   ecx, word ptr [ecx + 0x22]
        call    ResolveAircraftWeaponModelId
        pop     edx
        pop     ecx
        add     eax, 0xFFFFFE57
        jmp     CONTINUE_CVehicle__SelectPlaneWeapon_ModelId
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D4D5E | 0F BF 46 22    | movsx   eax, word ptr [esi + 0x22]
// >>> 0x6D4D62 | 05 57 FE FF FF | add     eax, 0xFFFFFE57
//     0x6D4D67 | 3D 97 00 00 00 | cmp     eax, 0x97
#define HOOKPOS_CVehicle__FirePlaneGuns_ModelId  0x6D4D5E
#define HOOKSIZE_CVehicle__FirePlaneGuns_ModelId 9
static const DWORD CONTINUE_CVehicle__FirePlaneGuns_ModelId = 0x6D4D67;

static void __declspec(naked) HOOK_CVehicle__FirePlaneGuns_ModelId()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveAircraftWeaponModelId
        add     eax, 0xFFFFFE57
        jmp     CONTINUE_CVehicle__FirePlaneGuns_ModelId
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D514F | 0F BF 46 22 | movsx   eax, word ptr [esi + 0x22]
// >>> 0x6D5153 | 2D A9 01 00 00 | sub     eax, 0x1A9
//     0x6D5158 | 74 0E          | jz      0x6D5168
#define HOOKPOS_CVehicle__FireUnguidedMissile_ModelId  0x6D514F
#define HOOKSIZE_CVehicle__FireUnguidedMissile_ModelId 9
static const DWORD CONTINUE_CVehicle__FireUnguidedMissile_ModelId = 0x6D5158;

static void __declspec(naked) HOOK_CVehicle__FireUnguidedMissile_ModelId()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveAircraftWeaponModelId
        sub     eax, 0x1A9
        jmp     CONTINUE_CVehicle__FireUnguidedMissile_ModelId
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6E3457 | 0F BF 46 22    | movsx   eax, word ptr [esi + 0x22]
// >>> 0x6E345B | 05 57 FE FF FF | add     eax, 0xFFFFFE57
//     0x6E3460 | 83 C4 08       | add     esp, 0x8
#define HOOKPOS_CVehicle__GetPlaneWeaponFiringStatus_ModelId  0x6E3457
#define HOOKSIZE_CVehicle__GetPlaneWeaponFiringStatus_ModelId 9
static const DWORD CONTINUE_CVehicle__GetPlaneWeaponFiringStatus_ModelId = 0x6E3460;

static void __declspec(naked) HOOK_CVehicle__GetPlaneWeaponFiringStatus_ModelId()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveAircraftWeaponModelId
        add     eax, 0xFFFFFE57
        jmp     CONTINUE_CVehicle__GetPlaneWeaponFiringStatus_ModelId
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6E39B8 | 66 81 7E 22 08 02 | cmp     word ptr [esi + 0x22], 0x208
//     0x6E39BE | B1 01             | mov     cl, 0x1
#define HOOKPOS_CVehicle__ProcessWeapons_HydraCheck  0x6E39B8
#define HOOKSIZE_CVehicle__ProcessWeapons_HydraCheck 6
static const DWORD CONTINUE_CVehicle__ProcessWeapons_HydraCheck = 0x6E39BE;

static void __declspec(naked) HOOK_CVehicle__ProcessWeapons_HydraCheck()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveAircraftWeaponModelId
        cmp     eax, 0x208
        jmp     CONTINUE_CVehicle__ProcessWeapons_HydraCheck
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CHud::DrawCrossHairs, Hydra/Hunter gunsight crosshair for custom models; one resolve replaces
// both compares and the second FindPlayerVehicle call.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x58E09F | 66 81 78 22 08 02 | cmp     word ptr [eax + 0x22], 0x208
// >>> 0x58E0A5 | 74 14             | je      0x58E0BB
// >>> 0x58E0A7 | 6A 00             | push    0
// >>> 0x58E0A9 | 6A FF             | push    -1
// >>> 0x58E0AB | E8 20 00 FE FF    | call    FindPlayerVehicle
// >>> 0x58E0B0 | 83 C4 08          | add     esp, 8
// >>> 0x58E0B3 | 66 81 78 22 A9 01 | cmp     word ptr [eax + 0x22], 0x1A9
//     0x58E0B9 | 75 05             | jne     0x58E0C0
#define HOOKPOS_CHud__DrawCrossHairs_ModelCheck  0x58E09F
#define HOOKSIZE_CHud__DrawCrossHairs_ModelCheck 26
static const DWORD CONTINUE_CHud__DrawCrossHairs_ModelCheck = 0x58E0B9;

static void __declspec(naked) HOOK_CHud__DrawCrossHairs_ModelCheck()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [eax + 0x22]
        call    ResolveAircraftWeaponModelId
        cmp     eax, 0x208
        je      matched
        cmp     eax, 0x1A9

        matched:
        jmp     CONTINUE_CHud__DrawCrossHairs_ModelCheck
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x527058 | 66 81 78 22 08 02 | cmp     word ptr [eax + 0x22], 0x208
//     0x52705E | 74 0A             | je      0x52706A
#define HOOKPOS_CCam__Process_HydraLockOnCheck  0x527058
#define HOOKSIZE_CCam__Process_HydraLockOnCheck 6
static const DWORD CONTINUE_CCam__Process_HydraLockOnCheck = 0x52705E;

static void __declspec(naked) HOOK_CCam__Process_HydraLockOnCheck()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [eax + 0x22]
        call    ResolveAircraftWeaponModelId
        cmp     eax, 0x208
        jmp     CONTINUE_CCam__Process_HydraLockOnCheck
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A53B5 | E8 66 F0 D5 FF    | call    0x404420 (GetModelIndex)
// >>> 0x6A53BA | 3D 08 02 00 00    | cmp     eax, 0x208
//     0x6A53BF | 0F 84 B2 00 00 00 | je      0x6A5477
#define HOOKPOS_CAutomobile__ProcessCarWheelPair_HydraSteerExempt  0x6A53BA
#define HOOKSIZE_CAutomobile__ProcessCarWheelPair_HydraSteerExempt 5
static const DWORD CONTINUE_CAutomobile__ProcessCarWheelPair_HydraSteerExempt = 0x6A53BF;

static void __declspec(naked) HOOK_CAutomobile__ProcessCarWheelPair_HydraSteerExempt()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     ecx, eax
        call    ResolveAircraftWeaponModelId
        cmp     eax, 0x208
        jmp     CONTINUE_CAutomobile__ProcessCarWheelPair_HydraSteerExempt
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C41D9 | 81 FF A9 01 00 00 | cmp     edi, 0x1A9
//     0x6C41DF | 89 5C 24 18       | mov     dword ptr [esp + 0x18], ebx
#define HOOKPOS_CHeli__Constructor_HunterDoor  0x6C41D9
#define HOOKSIZE_CHeli__Constructor_HunterDoor 6
static const DWORD CONTINUE_CHeli__Constructor_HunterDoor = 0x6C41DF;

static void __declspec(naked) HOOK_CHeli__Constructor_HunterDoor()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        push    ecx
        push    edx
        mov     ecx, edi
        call    ResolveAircraftWeaponModelId
        cmp     eax, 0x1A9
        pop     edx
        pop     ecx
        pop     eax
        jmp     CONTINUE_CHeli__Constructor_HunterDoor
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CPlane::Constructor, resolves the model once before the door/panel/wheel-visibility switch,
// covering the Hydra, Rustler, Cropduster, Shamal, Nevada, Vortex and Stunt Plane cases below.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C8F10 | 81 FF 08 02 00 00 | cmp     edi, 0x208
//     0x6C8F16 | 66 C7 86 A6 04 00 00 FF 00 | mov     word ptr [esi + 0x4A6], 0xFF
#define HOOKPOS_CPlane__Constructor_ModelSwitch  0x6C8F10
#define HOOKSIZE_CPlane__Constructor_ModelSwitch 6
static const DWORD CONTINUE_CPlane__Constructor_ModelSwitch = 0x6C8F16;

static void __declspec(naked) HOOK_CPlane__Constructor_ModelSwitch()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        push    edx
        mov     ecx, edi
        call    ResolveVehicleParentModelId
        mov     edi, eax
        pop     edx
        pop     ecx
        cmp     edi, 0x208
        jmp     CONTINUE_CPlane__Constructor_ModelSwitch
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x406AF1 | 66 81 7E 22 08 02 | cmp     word ptr [esi + 0x22], 0x208
//     0x406AF7 | E9 FD 49 2C 00    | jmp     0x6CB4F9
#define HOOKPOS_CPlane__ProcessControlInputs_HydraNozzleTurn  0x406AF1
#define HOOKSIZE_CPlane__ProcessControlInputs_HydraNozzleTurn 6
static const DWORD CONTINUE_CPlane__ProcessControlInputs_HydraNozzleTurn = 0x406AF7;

static void __declspec(naked) HOOK_CPlane__ProcessControlInputs_HydraNozzleTurn()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveAircraftWeaponModelId
        cmp     eax, 0x208
        jmp     CONTINUE_CPlane__ProcessControlInputs_HydraNozzleTurn
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x501C73 | 0F BF 42 22    | movsx   eax, word ptr [edx + 0x22]
// >>> 0x501C77 | 05 F9 FD FF FF | add     eax, 0xFFFFFDF9
//     0x501C7C | 83 F8 49       | cmp     eax, 0x49
#define HOOKPOS_CAEVehicleAudioEntity__ProcessAircraft_JetClass  0x501C73
#define HOOKSIZE_CAEVehicleAudioEntity__ProcessAircraft_JetClass 9
static const DWORD CONTINUE_CAEVehicleAudioEntity__ProcessAircraft_JetClass = 0x501C7C;

static void __declspec(naked) HOOK_CAEVehicleAudioEntity__ProcessAircraft_JetClass()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        push    edx
        movzx   ecx, word ptr [edx + 0x22]
        call    ResolveJetAudioModelId
        pop     edx
        pop     ecx
        add     eax, 0xFFFFFDF9
        jmp     CONTINUE_CAEVehicleAudioEntity__ProcessAircraft_JetClass
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x4FF980 | 0F BF 40 22    | movsx   eax, word ptr [eax + 0x22]
// >>> 0x4FF984 | 05 F9 FD FF FF | add     eax, 0xFFFFFDF9
//     0x4FF989 | 83 F8 49       | cmp     eax, 0x49
#define HOOKPOS_CAEVehicleAudioEntity__ProcessGenericJet_ModelSelect  0x4FF980
#define HOOKSIZE_CAEVehicleAudioEntity__ProcessGenericJet_ModelSelect 9
static const DWORD CONTINUE_CAEVehicleAudioEntity__ProcessGenericJet_ModelSelect = 0x4FF989;

static void __declspec(naked) HOOK_CAEVehicleAudioEntity__ProcessGenericJet_ModelSelect()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [eax + 0x22]
        call    ResolveJetAudioModelId
        add     eax, 0xFFFFFDF9
        jmp     CONTINUE_CAEVehicleAudioEntity__ProcessGenericJet_ModelSelect
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x501AB9 | 0F BF 40 22    | movsx   eax, word ptr [eax + 0x22]
// >>> 0x501ABD | 05 4D FE FF FF | add     eax, 0xFFFFFE4D
//     0x501AC2 | 3D 9C 00 00 00 | cmp     eax, 0x9C
#define HOOKPOS_CAEVehicleAudioEntity__ProcessSpecialVehicle_ModelSwitch  0x501AB9
#define HOOKSIZE_CAEVehicleAudioEntity__ProcessSpecialVehicle_ModelSwitch 9
static const DWORD CONTINUE_CAEVehicleAudioEntity__ProcessSpecialVehicle_ModelSwitch = 0x501AC2;

static void __declspec(naked) HOOK_CAEVehicleAudioEntity__ProcessSpecialVehicle_ModelSwitch()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        movzx   ecx, word ptr [eax + 0x22]
        call    ResolveVehicleParentModelId
        pop     ecx
        add     eax, 0xFFFFFE4D
        jmp     CONTINUE_CAEVehicleAudioEntity__ProcessSpecialVehicle_ModelSwitch
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A8DE2 | 66 8B 46 22 | mov     ax, word ptr [esi + 0x22]
// >>> 0x6A8DE6 | 66 3D BF 01 | cmp     ax, 0x1BF
//     0x6A8DEA | 0F 84 AE 00 00 00 | je      0x6A8E9E
#define HOOKPOS_CAutomobile__ProcessBuoyancy_AmphibiousRotorExempt  0x6A8DE2
#define HOOKSIZE_CAutomobile__ProcessBuoyancy_AmphibiousRotorExempt 8
static const DWORD CONTINUE_CAutomobile__ProcessBuoyancy_AmphibiousRotorExempt = 0x6A8DEA;

static void __declspec(naked) HOOK_CAutomobile__ProcessBuoyancy_AmphibiousRotorExempt()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // ax is left holding the resolved id for the untouched Leviathan compare at 0x6A8DF0.
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x1BF
        jmp     CONTINUE_CAutomobile__ProcessBuoyancy_AmphibiousRotorExempt
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A8F18 | 66 8B 4E 22       | mov     cx, word ptr [esi + 0x22]
// >>> 0x6A8F1C | 66 81 F9 BF 01    | cmp     cx, 0x1BF
//     0x6A8F21 | 74 07             | je      0x6A8F2A
#define HOOKPOS_CAutomobile__ProcessBuoyancy_AmphibiousSafeFloat  0x6A8F18
#define HOOKSIZE_CAutomobile__ProcessBuoyancy_AmphibiousSafeFloat 9
static const DWORD CONTINUE_CAutomobile__ProcessBuoyancy_AmphibiousSafeFloat = 0x6A8F21;

static void __declspec(naked) HOOK_CAutomobile__ProcessBuoyancy_AmphibiousSafeFloat()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // cx is left holding the resolved id for the Leviathan and Vortex compares below.
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        mov     ecx, eax
        cmp     cx, 0x1BF
        jmp     CONTINUE_CAutomobile__ProcessBuoyancy_AmphibiousSafeFloat
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A8D4E | 66 81 7E 22 1B 02 | cmp     word ptr [esi + 0x22], 0x21B
//     0x6A8D54 | D9 5C 24 14       | fstp    dword ptr [esp + 0x14]
#define HOOKPOS_CAutomobile__ProcessBuoyancy_VortexSpeedDampingExempt  0x6A8D4E
#define HOOKSIZE_CAutomobile__ProcessBuoyancy_VortexSpeedDampingExempt 6
static const DWORD CONTINUE_CAutomobile__ProcessBuoyancy_VortexSpeedDampingExempt = 0x6A8D54;

static void __declspec(naked) HOOK_CAutomobile__ProcessBuoyancy_VortexSpeedDampingExempt()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Don't touch the x87 stack; st(0) feeds the fstp at CONTINUE.
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x21B
        jmp     CONTINUE_CAutomobile__ProcessBuoyancy_VortexSpeedDampingExempt
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6CB176 | 66 39 6E 22 | cmp     word ptr [esi + 0x22], bp
// >>> 0x6CB17A | 57          | push    edi
//     0x6CB17B | 75 77       | jne     0x6CB1F4
#define HOOKPOS_CPlane__ProcessControlInputs_VortexSteerAxis  0x6CB176
#define HOOKSIZE_CPlane__ProcessControlInputs_VortexSteerAxis 5
static const DWORD CONTINUE_CPlane__ProcessControlInputs_VortexSteerAxis = 0x6CB17B;

static void __declspec(naked) HOOK_CPlane__ProcessControlInputs_VortexSteerAxis()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x21B
        push    edi
        jmp     CONTINUE_CPlane__ProcessControlInputs_VortexSteerAxis
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6CB281 | 66 39 6E 22       | cmp     word ptr [esi + 0x22], bp
// >>> 0x6CB285 | 8B 96 84 03 00 00 | mov     edx, dword ptr [esi + 0x384]
//     0x6CB28B | D9 82 A0 00 00 00 | fld     dword ptr [edx + 0xA0]
#define HOOKPOS_CPlane__ProcessControlInputs_VortexSteerAngleSource  0x6CB281
#define HOOKSIZE_CPlane__ProcessControlInputs_VortexSteerAngleSource 10
static const DWORD CONTINUE_CPlane__ProcessControlInputs_VortexSteerAngleSource = 0x6CB28B;

static void __declspec(naked) HOOK_CPlane__ProcessControlInputs_VortexSteerAngleSource()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x21B
        mov     edx, dword ptr [esi + 0x384]
        jmp     CONTINUE_CPlane__ProcessControlInputs_VortexSteerAngleSource
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x524624 | 66 8B 47 22 | mov     ax, word ptr [edi + 0x22]
// >>> 0x524628 | 66 3D B9 01 | cmp     ax, 0x1B9
//     0x52462C | 0F 84 8F 00 00 00 | je      0x5246C1
#define HOOKPOS_CCam__ProcessFollowCarSA_DistanceCategoryModel  0x524624
#define HOOKSIZE_CCam__ProcessFollowCarSA_DistanceCategoryModel 8
static const DWORD CONTINUE_CCam__ProcessFollowCarSA_DistanceCategoryModel = 0x52462C;

static void __declspec(naked) HOOK_CCam__ProcessFollowCarSA_DistanceCategoryModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [edi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x1B9
        jmp     CONTINUE_CCam__ProcessFollowCarSA_DistanceCategoryModel
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6B217D | 66 81 7E 22 CC 01 | cmp     word ptr [esi + 0x22], 0x1CC
//     0x6B2183 | 75 4A             | jne     0x6B21CF
#define HOOKPOS_CAutomobile__ProcessControl_SkimmerBoatControl  0x6B217D
#define HOOKSIZE_CAutomobile__ProcessControl_SkimmerBoatControl 6
static const DWORD CONTINUE_CAutomobile__ProcessControl_SkimmerBoatControl = 0x6B2183;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_SkimmerBoatControl()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CAutomobile__ProcessControl_SkimmerBoatControl
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C7172 | 66 81 7E 22 F1 01 | cmp     word ptr [esi + 0x22], 0x1F1
//     0x6C7178 | 75 17             | jne     0x6C7191
#define HOOKPOS_CHeli__ProcessControl_SearchLightModel  0x6C7172
#define HOOKSIZE_CHeli__ProcessControl_SearchLightModel 6
static const DWORD CONTINUE_CHeli__ProcessControl_SearchLightModel = 0x6C7178;

static void __declspec(naked) HOOK_CHeli__ProcessControl_SearchLightModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1F1
        jmp     CONTINUE_CHeli__ProcessControl_SearchLightModel
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6B2239 | 66 81 7E 22 D0 01 | cmp     word ptr [esi + 0x22], 0x1D0
//     0x6B223F | 75 0A             | jne     0x6B224B
#define HOOKPOS_CAutomobile__ProcessControl_RCBaronFakePhysicsGate  0x6B2239
#define HOOKSIZE_CAutomobile__ProcessControl_RCBaronFakePhysicsGate 6
static const DWORD CONTINUE_CAutomobile__ProcessControl_RCBaronFakePhysicsGate = 0x6B223F;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_RCBaronFakePhysicsGate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1D0
        jmp     CONTINUE_CAutomobile__ProcessControl_RCBaronFakePhysicsGate
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6B2B93 | 66 81 7E 22 D0 01 | cmp     word ptr [esi + 0x22], 0x1D0
//     0x6B2B99 | 74 0A             | je      0x6B2BA5
#define HOOKPOS_CAutomobile__ProcessControl_RCBaronNormalGate  0x6B2B93
#define HOOKSIZE_CAutomobile__ProcessControl_RCBaronNormalGate 6
static const DWORD CONTINUE_CAutomobile__ProcessControl_RCBaronNormalGate = 0x6B2B99;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_RCBaronNormalGate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1D0
        jmp     CONTINUE_CAutomobile__ProcessControl_RCBaronNormalGate
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6CB977 | 66 81 7E 22 D0 01 | cmp     word ptr [esi + 0x22], 0x1D0
//     0x6CB97D | 0F 84 67 07 00 00 | je      0x6CC0EA
#define HOOKPOS_CAutomobile__ProcessFlyingCarStuff_RCBaronFastPathGate  0x6CB977
#define HOOKSIZE_CAutomobile__ProcessFlyingCarStuff_RCBaronFastPathGate 6
static const DWORD CONTINUE_CAutomobile__ProcessFlyingCarStuff_RCBaronFastPathGate = 0x6CB97D;

static void __declspec(naked) HOOK_CAutomobile__ProcessFlyingCarStuff_RCBaronFastPathGate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x1D0
        jmp     CONTINUE_CAutomobile__ProcessFlyingCarStuff_RCBaronFastPathGate
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6CC20F | 66 81 7E 22 D0 01 | cmp     word ptr [esi + 0x22], 0x1D0
//     0x6CC215 | 0F 95 C0          | setne   al
#define HOOKPOS_CAutomobile__ProcessFlyingCarStuff_RCBaronControlScaleSelect  0x6CC20F
#define HOOKSIZE_CAutomobile__ProcessFlyingCarStuff_RCBaronControlScaleSelect 6
static const DWORD CONTINUE_CAutomobile__ProcessFlyingCarStuff_RCBaronControlScaleSelect = 0x6CC215;

static void __declspec(naked) HOOK_CAutomobile__ProcessFlyingCarStuff_RCBaronControlScaleSelect()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x1D0
        pop     eax
        jmp     CONTINUE_CAutomobile__ProcessFlyingCarStuff_RCBaronControlScaleSelect
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6CC318 | 66 8B 4E 22       | mov     cx, word ptr [esi + 0x22]
//     0x6CC31C | 66 81 F9 D0 01    | cmp     cx, 0x1D0
#define HOOKPOS_CAutomobile__ProcessFlyingCarStuff_RCBaronBehaviorDispatch  0x6CC318
#define HOOKSIZE_CAutomobile__ProcessFlyingCarStuff_RCBaronBehaviorDispatch 9
static const DWORD CONTINUE_CAutomobile__ProcessFlyingCarStuff_RCBaronBehaviorDispatch = 0x6CC321;

static void __declspec(naked) HOOK_CAutomobile__ProcessFlyingCarStuff_RCBaronBehaviorDispatch()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        mov     cx, ax
        cmp     cx, 0x1D0
        pop     eax
        jmp     CONTINUE_CAutomobile__ProcessFlyingCarStuff_RCBaronBehaviorDispatch
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D6A7B | 0F BF 4E 22          | movzx   ecx, word ptr [esi + 0x22]
// >>> 0x6D6A7F | 88 86 88 04 00 00    | mov     byte ptr [esi + 0x488], al
//     0x6D6A85 | 8D 81 47 FE FF FF    | lea     eax, [ecx - 0x1B9]
#define HOOKPOS_CVehicle__SetModelIndex_RCVehicleFlag  0x6D6A7B
#define HOOKSIZE_CVehicle__SetModelIndex_RCVehicleFlag 10
static const DWORD CONTINUE_CVehicle__SetModelIndex_RCVehicleFlag = 0x6D6A85;

static void __declspec(naked) HOOK_CVehicle__SetModelIndex_RCVehicleFlag()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     byte ptr [esi + 0x488], al
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        mov     ecx, eax
        jmp     CONTINUE_CVehicle__SetModelIndex_RCVehicleFlag
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C926D | 66 8B 46 22 | mov     ax, word ptr [esi + 0x22]
// >>> 0x6C9271 | 66 3D 00 02 | cmp     ax, 0x200
//     0x6C9275 | 74 06       | je      0x6C927D
#define HOOKPOS_CPlane__ProcessControl_SmokeEjectorModel  0x6C926D
#define HOOKSIZE_CPlane__ProcessControl_SmokeEjectorModel 8
static const DWORD CONTINUE_CPlane__ProcessControl_SmokeEjectorModel = 0x6C9275;

static void __declspec(naked) HOOK_CPlane__ProcessControl_SmokeEjectorModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x200
        jmp     CONTINUE_CPlane__ProcessControl_SmokeEjectorModel
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6CA945 | 66 8B 46 22 | mov     ax, word ptr [esi + 0x22]
// >>> 0x6CA949 | 66 3D 00 02 | cmp     ax, 0x200
//     0x6CA94D | 0F 85 88 00 00 00 | jne     0x6CA9DB
#define HOOKPOS_CPlane__ProcessControl_SmokeParticleModel  0x6CA945
#define HOOKSIZE_CPlane__ProcessControl_SmokeParticleModel 8
static const DWORD CONTINUE_CPlane__ProcessControl_SmokeParticleModel = 0x6CA94D;

static void __declspec(naked) HOOK_CPlane__ProcessControl_SmokeParticleModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x200
        jmp     CONTINUE_CPlane__ProcessControl_SmokeParticleModel
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C94FB | 66 81 7E 22 1B 02 | cmp     word ptr [esi + 0x22], 0x21B
//     0x6C9501 | 75 0A             | jne     0x6C950D
#define HOOKPOS_CPlane__ProcessControl_VortexTrailModel  0x6C94FB
#define HOOKSIZE_CPlane__ProcessControl_VortexTrailModel 6
static const DWORD CONTINUE_CPlane__ProcessControl_VortexTrailModel = 0x6C9501;

static void __declspec(naked) HOOK_CPlane__ProcessControl_VortexTrailModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x21B
        jmp     CONTINUE_CPlane__ProcessControl_VortexTrailModel
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6B1E26 | 66 8B 4E 22       | mov     cx, word ptr [esi + 0x22]
// >>> 0x6B1E2A | 66 81 F9 BF 01    | cmp     cx, 0x1BF
//     0x6B1E2F | 74 07             | je      0x6B1E38
#define HOOKPOS_CAutomobile__ProcessControl_AmphibiousPhysicsKeepAlive  0x6B1E26
#define HOOKSIZE_CAutomobile__ProcessControl_AmphibiousPhysicsKeepAlive 9
static const DWORD CONTINUE_CAutomobile__ProcessControl_AmphibiousPhysicsKeepAlive = 0x6B1E2F;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_AmphibiousPhysicsKeepAlive()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        mov     ecx, eax
        cmp     cx, 0x1BF
        jmp     CONTINUE_CAutomobile__ProcessControl_AmphibiousPhysicsKeepAlive
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C9101 | 66 81 7E 22 08 02 | cmp     word ptr [esi + 0x22], 0x208
//     0x6C9107 | 89 9E F8 09 00 00 | mov     [esi + 0x9F8], ebx
#define HOOKPOS_CPlane__Constructor_HydraNozzlePrime  0x6C9101
#define HOOKSIZE_CPlane__Constructor_HydraNozzlePrime 6
static const DWORD CONTINUE_CPlane__Constructor_HydraNozzlePrime = 0x6C9107;

static void __declspec(naked) HOOK_CPlane__Constructor_HydraNozzlePrime()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x208
        jmp     CONTINUE_CPlane__Constructor_HydraNozzlePrime
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D8FDA | 66 81 FB 08 02 | cmp     bx, 0x208
//     0x6D8FDF | 8B 7E 14       | mov     edi, [esi + 0x14]
#define HOOKPOS_CVehicle__FlyingControl_HydraPlaneThrust  0x6D8FDA
#define HOOKSIZE_CVehicle__FlyingControl_HydraPlaneThrust 5
static const DWORD CONTINUE_CVehicle__FlyingControl_HydraPlaneThrust = 0x6D8FDF;

static void __declspec(naked) HOOK_CVehicle__FlyingControl_HydraPlaneThrust()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x208
        jmp     CONTINUE_CVehicle__FlyingControl_HydraPlaneThrust
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D9C04 | 66 81 7E 22 08 02 | cmp     word ptr [esi + 0x22], 0x208
//     0x6D9C0A | 0F 85 85 00 00 00 | jne     0x6D9C95
#define HOOKPOS_CVehicle__FlyingControl_HydraHoverThrust  0x6D9C04
#define HOOKSIZE_CVehicle__FlyingControl_HydraHoverThrust 6
static const DWORD CONTINUE_CVehicle__FlyingControl_HydraHoverThrust = 0x6D9C0A;

static void __declspec(naked) HOOK_CVehicle__FlyingControl_HydraHoverThrust()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x208
        jmp     CONTINUE_CVehicle__FlyingControl_HydraHoverThrust
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C9D7E | 0F BF 46 22    | movsx   eax, word ptr [esi + 0x22]
// >>> 0x6C9D82 | 05 24 FE FF FF | add     eax, 0xFFFFFE24
//     0x6C9D87 | 83 CD FF       | or      ebp, 0xFFFFFFFF
#define HOOKPOS_CPlane__PreRender_ComponentSwitch  0x6C9D7E
#define HOOKSIZE_CPlane__PreRender_ComponentSwitch 9
static const DWORD CONTINUE_CPlane__PreRender_ComponentSwitch = 0x6C9D87;

static void __declspec(naked) HOOK_CPlane__PreRender_ComponentSwitch()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        add     eax, 0xFFFFFE24
        jmp     CONTINUE_CPlane__PreRender_ComponentSwitch
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C968E | 66 81 7E 22 08 02 | cmp     word ptr [esi + 0x22], 0x208
//     0x6C9694 | 75 05             | jne     0x6C969B
#define HOOKPOS_CPlane__PreRender_HydraRotationMode  0x6C968E
#define HOOKSIZE_CPlane__PreRender_HydraRotationMode 6
static const DWORD CONTINUE_CPlane__PreRender_HydraRotationMode = 0x6C9694;

static void __declspec(naked) HOOK_CPlane__PreRender_HydraRotationMode()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x208
        jmp     CONTINUE_CPlane__PreRender_HydraRotationMode
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C9EE3 | 66 8B 46 22 | mov     ax, word ptr [esi + 0x22]
// >>> 0x6C9EE7 | 66 3D 50 02 | cmp     ax, 0x250
//     0x6C9EEB | 75 2F       | jne     0x6C9F1C
#define HOOKPOS_CPlane__PreRender_RampAndNozzleModel  0x6C9EE3
#define HOOKSIZE_CPlane__PreRender_RampAndNozzleModel 8
static const DWORD CONTINUE_CPlane__PreRender_RampAndNozzleModel = 0x6C9EEB;

static void __declspec(naked) HOOK_CPlane__PreRender_RampAndNozzleModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x250
        jmp     CONTINUE_CPlane__PreRender_RampAndNozzleModel
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A14A6 | 66 81 F9 50 02 | cmp     cx, 0x250
// >>> 0x6A14AB | 74 07          | je      0x6A14B4
//     0x6A14AD | 66 81 F9 12 02 | cmp     cx, 0x212
#define HOOKPOS_CAutomobile__ProcessControl_AndromAngleReset  0x6A14A6
#define HOOKSIZE_CAutomobile__ProcessControl_AndromAngleReset 7
static const DWORD CONTINUE_CAutomobile__ProcessControl_AndromAngleReset = 0x6A14B4;
static const DWORD SKIP_CAutomobile__ProcessControl_AndromAngleReset = 0x6A14AD;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_AndromAngleReset()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        mov     ecx, edi
        call    IsAndromadaOrClone
        test    al, al
        pop     ecx
        jz      notAndrom

        jmp     CONTINUE_CAutomobile__ProcessControl_AndromAngleReset

        notAndrom:
        jmp     SKIP_CAutomobile__ProcessControl_AndromAngleReset
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A1509 | 66 81 F9 50 02 | cmp     cx, 0x250
// >>> 0x6A150E | 74 4C          | je      0x6A155C
//     0x6A1510 | 66 81 F9 20 02 | cmp     cx, 0x220
#define HOOKPOS_CAutomobile__ProcessControl_AndromMiscGate  0x6A1509
#define HOOKSIZE_CAutomobile__ProcessControl_AndromMiscGate 7
static const DWORD CONTINUE_CAutomobile__ProcessControl_AndromMiscGate = 0x6A155C;
static const DWORD SKIP_CAutomobile__ProcessControl_AndromMiscGate = 0x6A1510;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_AndromMiscGate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        mov     ecx, edi
        call    IsAndromadaOrClone
        test    al, al
        pop     ecx
        jz      notAndrom

        jmp     CONTINUE_CAutomobile__ProcessControl_AndromMiscGate

        notAndrom:
        jmp     SKIP_CAutomobile__ProcessControl_AndromMiscGate
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A17EF | 66 3D 50 02 | cmp     ax, 0x250
// >>> 0x6A17F3 | 75 50       | jne     0x6A1845
//     0x6A17F5 | 8B 87 A8 06 00 00 | mov eax, dword ptr [edi + 0x6A8]
#define HOOKPOS_CAutomobile__MovingCollisionSpeed_AndromA  0x6A17EF
#define HOOKSIZE_CAutomobile__MovingCollisionSpeed_AndromA 6
static const DWORD CONTINUE_CAutomobile__MovingCollisionSpeed_AndromA = 0x6A17F5;
static const DWORD SKIP_CAutomobile__MovingCollisionSpeed_AndromA = 0x6A1845;

static void __declspec(naked) HOOK_CAutomobile__MovingCollisionSpeed_AndromA()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        mov     ecx, edi
        call    IsAndromadaOrClone
        test    al, al
        pop     eax
        jz      notAndrom

        jmp     CONTINUE_CAutomobile__MovingCollisionSpeed_AndromA

        notAndrom:
        jmp     SKIP_CAutomobile__MovingCollisionSpeed_AndromA
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A1FD1 | 66 81 F9 50 02 | cmp     cx, 0x250
// >>> 0x6A1FD6 | 75 2A          | jne     0x6A2002
//     0x6A1FD8 | 8B B6 A8 06 00 00 | mov esi, dword ptr [esi + 0x6A8]
#define HOOKPOS_CAutomobile__MovingCollisionSpeed_AndromB  0x6A1FD1
#define HOOKSIZE_CAutomobile__MovingCollisionSpeed_AndromB 7
static const DWORD CONTINUE_CAutomobile__MovingCollisionSpeed_AndromB = 0x6A1FD8;
static const DWORD SKIP_CAutomobile__MovingCollisionSpeed_AndromB = 0x6A2002;

static void __declspec(naked) HOOK_CAutomobile__MovingCollisionSpeed_AndromB()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        mov     ecx, esi
        call    IsAndromadaOrClone
        test    al, al
        pop     ecx
        jz      notAndrom

        jmp     CONTINUE_CAutomobile__MovingCollisionSpeed_AndromB

        notAndrom:
        jmp     SKIP_CAutomobile__MovingCollisionSpeed_AndromB
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A21C8 | 66 3D 50 02 | cmp     ax, 0x250
// >>> 0x6A21CC | 75 1E       | jne     0x6A21EC
//     0x6A21CE | 8B B1 A8 06 00 00 | mov esi, dword ptr [ecx + 0x6A8]
#define HOOKPOS_CAutomobile__MovingCollisionSpeed_AndromC  0x6A21C8
#define HOOKSIZE_CAutomobile__MovingCollisionSpeed_AndromC 6
static const DWORD CONTINUE_CAutomobile__MovingCollisionSpeed_AndromC = 0x6A21CE;
static const DWORD SKIP_CAutomobile__MovingCollisionSpeed_AndromC = 0x6A21EC;

static void __declspec(naked) HOOK_CAutomobile__MovingCollisionSpeed_AndromC()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        push    ecx
        push    edx
        call    IsAndromadaOrClone
        test    al, al
        pop     edx
        pop     ecx
        pop     eax
        jz      notAndrom

        jmp     CONTINUE_CAutomobile__MovingCollisionSpeed_AndromC

        notAndrom:
        jmp     SKIP_CAutomobile__MovingCollisionSpeed_AndromC
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6B1FA7 | 66 3D 50 02 | cmp     ax, 0x250
// >>> 0x6B1FAB | 74 62       | je      0x6B200F
//     0x6B1FAD | 66 3D 12 02 | cmp     ax, 0x212
#define HOOKPOS_CAutomobile__ProcessControl_AndromDispatch  0x6B1FA7
#define HOOKSIZE_CAutomobile__ProcessControl_AndromDispatch 6
static const DWORD CONTINUE_CAutomobile__ProcessControl_AndromDispatch = 0x6B200F;
static const DWORD SKIP_CAutomobile__ProcessControl_AndromDispatch = 0x6B1FAD;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_AndromDispatch()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        push    ecx
        mov     ecx, esi
        call    IsAndromadaOrClone
        test    al, al
        pop     ecx
        pop     eax
        jz      notAndrom

        jmp     CONTINUE_CAutomobile__ProcessControl_AndromDispatch

        notAndrom:
        jmp     SKIP_CAutomobile__ProcessControl_AndromDispatch
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// Skimmer, Vortex, Sea Sparrow/Sparrow/RC Tiger weapon accuracy, amphibious ped exit, AT-400
// and Andromada gaps on custom models, spanning boat physics, boat camera, hover flight,
// ped enter/exit, weapon spread and vehicle shadows.
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
// CVehicle::ProcessBoatControl, cutting the Skimmer's buoyancy force to 3% while capsized so it rights itself like the boat model
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6DBF0A | 66 81 7E 22 CC 01 | cmp     word ptr [esi + 0x22], 0x1CC
//     0x6DBF10 | 8B 15 4C C9 C1 00 | mov     edx, dword ptr [0xC1C94C]
#define HOOKPOS_CVehicle__ProcessBoatControl_SkimmerCapsizedBuoyancy  0x6DBF0A
#define HOOKSIZE_CVehicle__ProcessBoatControl_SkimmerCapsizedBuoyancy 6
static const DWORD CONTINUE_CVehicle__ProcessBoatControl_SkimmerCapsizedBuoyancy = 0x6DBF10;

static void __declspec(naked) HOOK_CVehicle__ProcessBoatControl_SkimmerCapsizedBuoyancy()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CVehicle__ProcessBoatControl_SkimmerCapsizedBuoyancy
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CVehicle::ProcessBoatControl, damping the turn/move forces right after the Skimmer lands nose-first on water and resetting its m_f2ndSteerAngle timer
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6DC00B | 66 81 7E 22 CC 01 | cmp     word ptr [esi + 0x22], 0x1CC
//     0x6DC011 | 0F 85 CB 01 00 00 | jne     0x6DC1E2
#define HOOKPOS_CVehicle__ProcessBoatControl_SkimmerWaterLandingDamping  0x6DC00B
#define HOOKSIZE_CVehicle__ProcessBoatControl_SkimmerWaterLandingDamping 6
static const DWORD CONTINUE_CVehicle__ProcessBoatControl_SkimmerWaterLandingDamping = 0x6DC011;

static void __declspec(naked) HOOK_CVehicle__ProcessBoatControl_SkimmerWaterLandingDamping()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CVehicle__ProcessBoatControl_SkimmerWaterLandingDamping
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CVehicle::ProcessBoatControl, giving the Skimmer aquaplane thrust of (gas + 1) instead of plain boat throttle so it can plane and take off from water
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6DC21B | 66 81 7E 22 CC 01 | cmp     word ptr [esi + 0x22], 0x1CC
//     0x6DC221 | D9 46 4C          | fld     dword ptr [esi + 0x4C]
#define HOOKPOS_CVehicle__ProcessBoatControl_SkimmerAquaplaneThrust  0x6DC21B
#define HOOKSIZE_CVehicle__ProcessBoatControl_SkimmerAquaplaneThrust 6
static const DWORD CONTINUE_CVehicle__ProcessBoatControl_SkimmerAquaplaneThrust = 0x6DC221;

static void __declspec(naked) HOOK_CVehicle__ProcessBoatControl_SkimmerAquaplaneThrust()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // ecx (a physics-matrix pointer) is reused past CONTINUE at 0x6DC2B1/BE/C9.
        push    ecx
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        pop     ecx
        cmp     eax, 0x1CC
        jmp     CONTINUE_CVehicle__ProcessBoatControl_SkimmerAquaplaneThrust
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CVehicle::ProcessBoatControl, forcing the Skimmer to always be treated as slowing down for the rudder/steer force block
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6DC621 | 66 81 7E 22 CC 01 | cmp     word ptr [esi + 0x22], 0x1CC
//     0x6DC627 | 0F 84 13 03 00 00 | je      0x6DC940
#define HOOKPOS_CVehicle__ProcessBoatControl_SkimmerSlowingDownGate  0x6DC621
#define HOOKSIZE_CVehicle__ProcessBoatControl_SkimmerSlowingDownGate 6
static const DWORD CONTINUE_CVehicle__ProcessBoatControl_SkimmerSlowingDownGate = 0x6DC627;

static void __declspec(naked) HOOK_CVehicle__ProcessBoatControl_SkimmerSlowingDownGate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CVehicle__ProcessBoatControl_SkimmerSlowingDownGate
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CVehicle::ProcessBoatControl, skipping the boat turn-resistance calculation while the Skimmer's water-landing timer is active
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6DCD63 | 66 81 7E 22 CC 01 | cmp     word ptr [esi + 0x22], 0x1CC
//     0x6DCD69 | 75 17             | jne     0x6DCD82
#define HOOKPOS_CVehicle__ProcessBoatControl_SkimmerTurnResistanceSkip  0x6DCD63
#define HOOKSIZE_CVehicle__ProcessBoatControl_SkimmerTurnResistanceSkip 6
static const DWORD CONTINUE_CVehicle__ProcessBoatControl_SkimmerTurnResistanceSkip = 0x6DCD69;

static void __declspec(naked) HOOK_CVehicle__ProcessBoatControl_SkimmerTurnResistanceSkip()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CVehicle__ProcessBoatControl_SkimmerTurnResistanceSkip
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CVehicle::ApplyBoatWaterResistance, multiplying the Skimmer's hull water-drag speed multiplier by 30 so it doesn't glide frictionless across the surface
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D274C | 66 81 7E 22 CC 01 | cmp     word ptr [esi + 0x22], 0x1CC
//     0x6D2752 | 8B 86 84 03 00 00 | mov     eax, dword ptr [esi + 0x384]
#define HOOKPOS_CVehicle__ApplyBoatWaterResistance_SkimmerHullDrag  0x6D274C
#define HOOKSIZE_CVehicle__ApplyBoatWaterResistance_SkimmerHullDrag 6
static const DWORD CONTINUE_CVehicle__ApplyBoatWaterResistance_SkimmerHullDrag = 0x6D2752;

static void __declspec(naked) HOOK_CVehicle__ApplyBoatWaterResistance_SkimmerHullDrag()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CVehicle__ApplyBoatWaterResistance_SkimmerHullDrag
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CBuoyancy::PreCalcSetup, selecting the buoyancy volume/centre constants for the Leviathan and
// the Skimmer; both are tested off the same resolved ax, so this one hook covers both models.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C2D33 | 66 8B 47 22 | mov     ax, word ptr [edi + 0x22]
// >>> 0x6C2D37 | 66 3D A1 01 | cmp     ax, 0x1A1
//     0x6C2D3B | 75 23       | jne     0x6C2D60
#define HOOKPOS_CBuoyancy__PreCalcSetup_LeviathanSkimmerModel  0x6C2D33
#define HOOKSIZE_CBuoyancy__PreCalcSetup_LeviathanSkimmerModel 8
static const DWORD CONTINUE_CBuoyancy__PreCalcSetup_LeviathanSkimmerModel = 0x6C2D3B;

static void __declspec(naked) HOOK_CBuoyancy__PreCalcSetup_LeviathanSkimmerModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        push    edx
        movzx   ecx, word ptr [edi + 0x22]
        call    ResolveVehicleParentModelId
        pop     edx
        pop     ecx
        cmp     ax, 0x1A1
        jmp     CONTINUE_CBuoyancy__PreCalcSetup_LeviathanSkimmerModel
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CPlane::ProcessControl, forcing all four wheels to WHEEL_STATUS_MISSING every frame because the Skimmer has floats instead of wheels
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C92EC | 66 81 7E 22 CC 01 | cmp     word ptr [esi + 0x22], 0x1CC
//     0x6C92F2 | 75 1D             | jne     0x6C9311
#define HOOKPOS_CPlane__ProcessControl_SkimmerWheelStatus  0x6C92EC
#define HOOKSIZE_CPlane__ProcessControl_SkimmerWheelStatus 6
static const DWORD CONTINUE_CPlane__ProcessControl_SkimmerWheelStatus = 0x6C92F2;

static void __declspec(naked) HOOK_CPlane__ProcessControl_SkimmerWheelStatus()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CPlane__ProcessControl_SkimmerWheelStatus
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CPlane::PreRender, gating the call into CVehicle::DoBoatSplashes that drives the Skimmer's water spray FX
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6CAA93 | 66 81 7E 22 CC 01 | cmp     word ptr [esi + 0x22], 0x1CC
//     0x6CAA99 | 75 16             | jne     0x6CAAB1
#define HOOKPOS_CPlane__PreRender_SkimmerBoatSplashesCall  0x6CAA93
#define HOOKSIZE_CPlane__PreRender_SkimmerBoatSplashesCall 6
static const DWORD CONTINUE_CPlane__PreRender_SkimmerBoatSplashesCall = 0x6CAA99;

static void __declspec(naked) HOOK_CPlane__PreRender_SkimmerBoatSplashesCall()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CPlane__PreRender_SkimmerBoatSplashesCall
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CVehicle::DoBoatSplashes, selecting the Skimmer-specific splash intensity multiplier and clamp
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6DD218 | BF CC 01 00 00 | mov     edi, 0x1CC
// >>> 0x6DD21D | 66 39 7E 22    | cmp     word ptr [esi + 0x22], di
//     0x6DD221 | D9 FA          | fsqrt
#define HOOKPOS_CVehicle__DoBoatSplashes_SkimmerIntensityModel  0x6DD218
#define HOOKSIZE_CVehicle__DoBoatSplashes_SkimmerIntensityModel 9
static const DWORD CONTINUE_CVehicle__DoBoatSplashes_SkimmerIntensityModel = 0x6DD221;

static void __declspec(naked) HOOK_CVehicle__DoBoatSplashes_SkimmerIntensityModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // edi keeps the literal target; only the vehicle's model is resolved
        mov     edi, 0x1CC
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, di
        jmp     CONTINUE_CVehicle__DoBoatSplashes_SkimmerIntensityModel
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CWaterLevel::RenderBoatWakes, driving the Skimmer's wake trail on water for a clone.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6EDA0C | 66 81 7E 22 CC 01 | cmp     word ptr [esi + 0x22], 0x1CC
//     0x6EDA12 | D9 40 0C          | fld     dword ptr [eax + 0xC]
#define HOOKPOS_CWaterLevel__RenderBoatWakes_SkimmerModel  0x6EDA0C
#define HOOKSIZE_CWaterLevel__RenderBoatWakes_SkimmerModel 6
static const DWORD CONTINUE_CWaterLevel__RenderBoatWakes_SkimmerModel = 0x6EDA12;

static void __declspec(naked) HOOK_CWaterLevel__RenderBoatWakes_SkimmerModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        pop     eax
        jmp     CONTINUE_CWaterLevel__RenderBoatWakes_SkimmerModel
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CBoat::Render, selecting the Skimmer's propeller-spin special case for a clone at render time.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6F0234 | 66 81 7E 22 CC 01 | cmp     word ptr [esi + 0x22], 0x1CC
//     0x6F023A | 0F 84 EC 06 00 00 | je      0x6F092C
#define HOOKPOS_CBoat__Render_SkimmerPropellerSpin  0x6F0234
#define HOOKSIZE_CBoat__Render_SkimmerPropellerSpin 6
static const DWORD CONTINUE_CBoat__Render_SkimmerPropellerSpin = 0x6F023A;

static void __declspec(naked) HOOK_CBoat__Render_SkimmerPropellerSpin()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CBoat__Render_SkimmerPropellerSpin
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCamera::CamControl, entering the on-water camera-control branch for the Skimmer.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x528294 | 66 81 79 22 CC 01 | cmp     word ptr [ecx + 0x22], 0x1CC
//     0x52829A | 75 4A             | jne     0x5282E6
#define HOOKPOS_CCamera__CamControl_SkimmerCameraMode  0x528294
#define HOOKSIZE_CCamera__CamControl_SkimmerCameraMode 6
static const DWORD CONTINUE_CCamera__CamControl_SkimmerCameraMode = 0x52829A;

static void __declspec(naked) HOOK_CCamera__CamControl_SkimmerCameraMode()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [ecx + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CCamera__CamControl_SkimmerCameraMode
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCam::TryToStartNewCamMode, Skimmer gate 1 of 5.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x51E5AC | 66 81 78 22 CC 01 | cmp     word ptr [eax + 0x22], 0x1CC
//     0x51E5B2 | 0F 85 34 0B 00 00 | jne     0x51F0EC
#define HOOKPOS_CCam__TryToStartNewCamMode_SkimmerCheckA  0x51E5AC
#define HOOKSIZE_CCam__TryToStartNewCamMode_SkimmerCheckA 6
static const DWORD CONTINUE_CCam__TryToStartNewCamMode_SkimmerCheckA = 0x51E5B2;

static void __declspec(naked) HOOK_CCam__TryToStartNewCamMode_SkimmerCheckA()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [eax + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CCam__TryToStartNewCamMode_SkimmerCheckA
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCam::TryToStartNewCamMode, Skimmer gate 2 of 5.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x51E773 | 66 81 79 22 CC 01 | cmp     word ptr [ecx + 0x22], 0x1CC
//     0x51E779 | 0F 85 6D 09 00 00 | jne     0x51F0EC
#define HOOKPOS_CCam__TryToStartNewCamMode_SkimmerCheckB  0x51E773
#define HOOKSIZE_CCam__TryToStartNewCamMode_SkimmerCheckB 6
static const DWORD CONTINUE_CCam__TryToStartNewCamMode_SkimmerCheckB = 0x51E779;

static void __declspec(naked) HOOK_CCam__TryToStartNewCamMode_SkimmerCheckB()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        movzx   ecx, word ptr [ecx + 0x22]
        call    ResolveVehicleParentModelId
        pop     ecx
        cmp     eax, 0x1CC
        jmp     CONTINUE_CCam__TryToStartNewCamMode_SkimmerCheckB
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCam::TryToStartNewCamMode, Skimmer gate 3 of 5.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x51E937 | 66 81 78 22 CC 01 | cmp     word ptr [eax + 0x22], 0x1CC
//     0x51E93D | 0F 85 A9 07 00 00 | jne     0x51F0EC
#define HOOKPOS_CCam__TryToStartNewCamMode_SkimmerCheckC  0x51E937
#define HOOKSIZE_CCam__TryToStartNewCamMode_SkimmerCheckC 6
static const DWORD CONTINUE_CCam__TryToStartNewCamMode_SkimmerCheckC = 0x51E93D;

static void __declspec(naked) HOOK_CCam__TryToStartNewCamMode_SkimmerCheckC()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [eax + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CCam__TryToStartNewCamMode_SkimmerCheckC
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCam::TryToStartNewCamMode, Skimmer gate 4 of 5.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x51EF39 | 66 81 7A 22 CC 01 | cmp     word ptr [edx + 0x22], 0x1CC
//     0x51EF3F | 0F 85 A7 01 00 00 | jne     0x51F0EC
#define HOOKPOS_CCam__TryToStartNewCamMode_SkimmerCheckD  0x51EF39
#define HOOKSIZE_CCam__TryToStartNewCamMode_SkimmerCheckD 6
static const DWORD CONTINUE_CCam__TryToStartNewCamMode_SkimmerCheckD = 0x51EF3F;

static void __declspec(naked) HOOK_CCam__TryToStartNewCamMode_SkimmerCheckD()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [edx + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CCam__TryToStartNewCamMode_SkimmerCheckD
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCam::TryToStartNewCamMode, Skimmer gate 5 of 5.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x51F15B | 66 81 79 22 CC 01 | cmp     word ptr [ecx + 0x22], 0x1CC
//     0x51F161 | 75 89             | jne     0x51F0EC
#define HOOKPOS_CCam__TryToStartNewCamMode_SkimmerCheckE  0x51F15B
#define HOOKSIZE_CCam__TryToStartNewCamMode_SkimmerCheckE 6
static const DWORD CONTINUE_CCam__TryToStartNewCamMode_SkimmerCheckE = 0x51F161;

static void __declspec(naked) HOOK_CCam__TryToStartNewCamMode_SkimmerCheckE()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [ecx + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CCam__TryToStartNewCamMode_SkimmerCheckE
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCam::Process (boat/fixed cam framing region), Skimmer gate 1 of 5.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x51D864 | 66 81 7A 22 CC 01 | cmp     word ptr [edx + 0x22], 0x1CC
//     0x51D86A | 0F 85 68 0C 00 00 | jne     0x51E4D8
#define HOOKPOS_CCam__Process_SkimmerFramingA  0x51D864
#define HOOKSIZE_CCam__Process_SkimmerFramingA 6
static const DWORD CONTINUE_CCam__Process_SkimmerFramingA = 0x51D86A;

static void __declspec(naked) HOOK_CCam__Process_SkimmerFramingA()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [edx + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CCam__Process_SkimmerFramingA
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCam::Process (boat/fixed cam framing region), Skimmer gate 2 of 5.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x51D92B | 66 81 7A 22 CC 01 | cmp     word ptr [edx + 0x22], 0x1CC
//     0x51D931 | 0F 85 A1 0B 00 00 | jne     0x51E4D8
#define HOOKPOS_CCam__Process_SkimmerFramingB  0x51D92B
#define HOOKSIZE_CCam__Process_SkimmerFramingB 6
static const DWORD CONTINUE_CCam__Process_SkimmerFramingB = 0x51D931;

static void __declspec(naked) HOOK_CCam__Process_SkimmerFramingB()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [edx + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CCam__Process_SkimmerFramingB
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCam::Process (boat/fixed cam framing region), Skimmer gate 3 of 5.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x51DA60 | 66 81 79 22 CC 01 | cmp     word ptr [ecx + 0x22], 0x1CC
//     0x51DA66 | 0F 85 6C 0A 00 00 | jne     0x51E4D8
#define HOOKPOS_CCam__Process_SkimmerFramingC  0x51DA60
#define HOOKSIZE_CCam__Process_SkimmerFramingC 6
static const DWORD CONTINUE_CCam__Process_SkimmerFramingC = 0x51DA66;

static void __declspec(naked) HOOK_CCam__Process_SkimmerFramingC()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [ecx + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CCam__Process_SkimmerFramingC
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCam::Process (boat/fixed cam framing region), Skimmer gate 4 of 5.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x51DCFC | 66 81 78 22 CC 01 | cmp     word ptr [eax + 0x22], 0x1CC
//     0x51DD02 | 0F 85 D0 07 00 00 | jne     0x51E4D8
#define HOOKPOS_CCam__Process_SkimmerFramingD  0x51DCFC
#define HOOKSIZE_CCam__Process_SkimmerFramingD 6
static const DWORD CONTINUE_CCam__Process_SkimmerFramingD = 0x51DD02;

static void __declspec(naked) HOOK_CCam__Process_SkimmerFramingD()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [eax + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CCam__Process_SkimmerFramingD
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCam::Process (boat/fixed cam framing region), Skimmer gate 5 of 5.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x51DE84 | 66 81 78 22 CC 01 | cmp     word ptr [eax + 0x22], 0x1CC
//     0x51DE8A | 0F 85 48 06 00 00 | jne     0x51E4D8
#define HOOKPOS_CCam__Process_SkimmerFramingE  0x51DE84
#define HOOKSIZE_CCam__Process_SkimmerFramingE 6
static const DWORD CONTINUE_CCam__Process_SkimmerFramingE = 0x51DE8A;

static void __declspec(naked) HOOK_CCam__Process_SkimmerFramingE()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [eax + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x1CC
        jmp     CONTINUE_CCam__Process_SkimmerFramingE
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CRenderer::SetupEntityVisibility, first-person culling exception list for a clone's hull.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x554374 | 66 3D CC 01 | cmp     ax, 0x1CC
// >>> 0x554378 | 75 12       | jne     0x55438C
//     0x55437A | 89 35 D4 45 B7 00 | mov     dword ptr [0xB745D4], esi
#define HOOKPOS_CRenderer__SetupEntityVisibility_SkimmerCulling  0x554374
#define HOOKSIZE_CRenderer__SetupEntityVisibility_SkimmerCulling 6
static const DWORD CONTINUE_CRenderer__SetupEntityVisibility_SkimmerCulling = 0x55437A;
static const DWORD SKIP_CRenderer__SetupEntityVisibility_SkimmerCulling = 0x55438C;

static void __declspec(naked) HOOK_CRenderer__SetupEntityVisibility_SkimmerCulling()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, ax
        call    ResolveVehicleParentModelId
        cmp     ax, 0x1CC
        jne     notSkimmer

        jmp     CONTINUE_CRenderer__SetupEntityVisibility_SkimmerCulling

        notSkimmer:
        jmp     SKIP_CRenderer__SetupEntityVisibility_SkimmerCulling
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6B284B | 66 81 7E 22 1B 02 | cmp     word ptr [esi + 0x22], 0x21b
//     0x6B2851 | 75 21             | jne     0x6b2874
#define HOOKPOS_CAutomobile__ProcessControl_VortexTractionBoost  0x6B284B
#define HOOKSIZE_CAutomobile__ProcessControl_VortexTractionBoost 6
static const DWORD CONTINUE_CAutomobile__ProcessControl_VortexTractionBoost = 0x6B2851;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_VortexTractionBoost()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x21B
        jmp     CONTINUE_CAutomobile__ProcessControl_VortexTractionBoost
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6B356A | 66 81 7E 22 1B 02 | cmp     word ptr [esi + 0x22], 0x21b
//     0x6B3570 | 0F 84 B2 00 00 00 | je      0x6b3628
#define HOOKPOS_CAutomobile__ProcessControl_VortexWheelSettleExempt  0x6B356A
#define HOOKSIZE_CAutomobile__ProcessControl_VortexWheelSettleExempt 6
static const DWORD CONTINUE_CAutomobile__ProcessControl_VortexWheelSettleExempt = 0x6B3570;

static void __declspec(naked) HOOK_CAutomobile__ProcessControl_VortexWheelSettleExempt()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x21B
        jmp     CONTINUE_CAutomobile__ProcessControl_VortexWheelSettleExempt
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6AFFEA | 66 81 7E 22 1B 02 | cmp     word ptr [esi + 0x22], 0x21b
//     0x6AFFF0 | 75 1C             | jne     0x6b000e
#define HOOKPOS_CAutomobile__ProcessSuspension_VortexSpringForceScale  0x6AFFEA
#define HOOKSIZE_CAutomobile__ProcessSuspension_VortexSpringForceScale 6
static const DWORD CONTINUE_CAutomobile__ProcessSuspension_VortexSpringForceScale = 0x6AFFF0;

static void __declspec(naked) HOOK_CAutomobile__ProcessSuspension_VortexSpringForceScale()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x21B
        jmp     CONTINUE_CAutomobile__ProcessSuspension_VortexSpringForceScale
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6B0017 | 66 81 7E 22 1B 02 | cmp     word ptr [esi + 0x22], 0x21b
//     0x6B001D | 74 35             | je      0x6b0054
#define HOOKPOS_CAutomobile__ProcessSuspension_VortexGroundSpringExempt  0x6B0017
#define HOOKSIZE_CAutomobile__ProcessSuspension_VortexGroundSpringExempt 6
static const DWORD CONTINUE_CAutomobile__ProcessSuspension_VortexGroundSpringExempt = 0x6B001D;

static void __declspec(naked) HOOK_CAutomobile__ProcessSuspension_VortexGroundSpringExempt()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x21B
        jmp     CONTINUE_CAutomobile__ProcessSuspension_VortexGroundSpringExempt
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CVehicle::FlyingControl, resolves bx once for both the Vortex and Skimmer ground-rest compares
// that share this load (a separate, independent load feeds the VortexThrustFormula hook below).
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D8E18 | 66 8B 5E 22       | mov     bx, word ptr [esi + 0x22]
// >>> 0x6D8E1C | 66 81 FB 1B 02    | cmp     bx, 0x21b
//     0x6D8E21 | 74 4C             | je      0x6d8e6f
#define HOOKPOS_CVehicle__FlyingControl_VortexSkimmerGroundRestGate  0x6D8E18
#define HOOKSIZE_CVehicle__FlyingControl_VortexSkimmerGroundRestGate 9
static const DWORD CONTINUE_CVehicle__FlyingControl_VortexSkimmerGroundRestGate = 0x6D8E21;

static void __declspec(naked) HOOK_CVehicle__FlyingControl_VortexSkimmerGroundRestGate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // eax holds 1.0f, consumed by four untouched dword compares at 0x6D8E23/2B/33/3B.
        push    eax
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        mov     bx, ax
        pop     eax
        cmp     bx, 0x21B
        jmp     CONTINUE_CVehicle__FlyingControl_VortexSkimmerGroundRestGate
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CVehicle::FlyingControl, resolves the model id feeding the Vortex thrust-formula compare
// (an independent load from the ground-rest gate hook above, not the same shared value).
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D8F09 | 66 81 FB 1B 02    | cmp     bx, 0x21b
//     0x6D8F0E | 8B 8E 88 03 00 00 | mov     ecx, dword ptr [esi + 0x388]
#define HOOKPOS_CVehicle__FlyingControl_VortexThrustFormula  0x6D8F09
#define HOOKSIZE_CVehicle__FlyingControl_VortexThrustFormula 5
static const DWORD CONTINUE_CVehicle__FlyingControl_VortexThrustFormula = 0x6D8F0E;

static void __declspec(naked) HOOK_CVehicle__FlyingControl_VortexThrustFormula()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x21B
        jmp     CONTINUE_CVehicle__FlyingControl_VortexThrustFormula
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CVehicle::FlyingControl, the Vortex's pitch/turn damping clamp
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6D9233 | 66 81 7E 22 1B 02 | cmp     word ptr [esi + 0x22], 0x21b
//     0x6D9239 | D8 4F 08          | fmul    dword ptr [edi + 8]
#define HOOKPOS_CVehicle__FlyingControl_VortexTurnDampingClamp  0x6D9233
#define HOOKSIZE_CVehicle__FlyingControl_VortexTurnDampingClamp 6
static const DWORD CONTINUE_CVehicle__FlyingControl_VortexTurnDampingClamp = 0x6D9239;

static void __declspec(naked) HOOK_CVehicle__FlyingControl_VortexTurnDampingClamp()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // eax holds a pointer from call 0x542ce0, read again past CONTINUE via fld [eax]/[eax+4].
        push    eax
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        mov     edx, eax
        pop     eax
        cmp     edx, 0x21B
        jmp     CONTINUE_CVehicle__FlyingControl_VortexTurnDampingClamp
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CPlane::Constructor, exempts a Vortex clone from the bDontCollideWithFlyers flag set on every
// other plane model.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C8E54 | 66 81 7E 22 1B 02 | cmp     word ptr [esi + 0x22], 0x21b
//     0x6C8E5A | C7 06 48 19 87 00 | mov     dword ptr [esi], 0x871948
#define HOOKPOS_CPlane__Constructor_VortexFlyerCollisionExempt  0x6C8E54
#define HOOKSIZE_CPlane__Constructor_VortexFlyerCollisionExempt 6
static const DWORD CONTINUE_CPlane__Constructor_VortexFlyerCollisionExempt = 0x6C8E5A;

static void __declspec(naked) HOOK_CPlane__Constructor_VortexFlyerCollisionExempt()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x21B
        jmp     CONTINUE_CPlane__Constructor_VortexFlyerCollisionExempt
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CPlane::VehicleDamage, the Vortex's car-style damage branch instead of the plane crash branch
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6CC4C4 | 66 81 7E 22 1B 02          | cmp     word ptr [esi + 0x22], 0x21b
//     0x6CC4CA | C7 44 24 10 FA 7E AA 3E    | mov     dword ptr [esp + 0x10], 0x3eaa7efa
#define HOOKPOS_CPlane__VehicleDamage_VortexCarStyleDamage  0x6CC4C4
#define HOOKSIZE_CPlane__VehicleDamage_VortexCarStyleDamage 6
static const DWORD CONTINUE_CPlane__VehicleDamage_VortexCarStyleDamage = 0x6CC4CA;

static void __declspec(naked) HOOK_CPlane__VehicleDamage_VortexCarStyleDamage()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // eax/ecx hold globals stored to [esp+0xC]/[esp+8] right after CONTINUE.
        push    eax
        push    ecx
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        mov     edx, eax
        pop     ecx
        pop     eax
        cmp     edx, 0x21B
        jmp     CONTINUE_CPlane__VehicleDamage_VortexCarStyleDamage
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C97E6 | 66 81 7E 22 1B 02 | cmp     word ptr [esi + 0x22], 0x21b
//     0x6C97EC | 75 35             | jne     0x6c9823
#define HOOKPOS_CPlane__PreRender_VortexFanFXMatrix  0x6C97E6
#define HOOKSIZE_CPlane__PreRender_VortexFanFXMatrix 6
static const DWORD CONTINUE_CPlane__PreRender_VortexFanFXMatrix = 0x6C97EC;

static void __declspec(naked) HOOK_CPlane__PreRender_VortexFanFXMatrix()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     eax, 0x21B
        jmp     CONTINUE_CPlane__PreRender_VortexFanFXMatrix
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAEVehicleAudioEntity::Initialise, resolves before the rebase-then-table-jump so every model
// in the [0x1C0..0x247] byte table gets the correct case for a clone, not just Vortex.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x4F7814 | 0F BF 42 22    | movsx   eax, word ptr [edx + 0x22]
// >>> 0x4F7818 | 05 40 FE FF FF | add     eax, 0xfffffe40
//     0x4F781D | 3D 87 00 00 00 | cmp     eax, 0x87
#define HOOKPOS_CAEVehicleAudioEntity__Initialise_SpecialVehicleModelSwitch  0x4F7814
#define HOOKSIZE_CAEVehicleAudioEntity__Initialise_SpecialVehicleModelSwitch 9
static const DWORD CONTINUE_CAEVehicleAudioEntity__Initialise_SpecialVehicleModelSwitch = 0x4F781D;

static void __declspec(naked) HOOK_CAEVehicleAudioEntity__Initialise_SpecialVehicleModelSwitch()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [edx + 0x22]
        call    ResolveVehicleParentModelId
        add     eax, 0xFFFFFE40
        jmp     CONTINUE_CAEVehicleAudioEntity__Initialise_SpecialVehicleModelSwitch
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CPlane::PreRender, resolves the model id used to pick the AT-400/Andromada/Vortex jumbo
// shadow variant in CShadows::StoreShadowForVehicle.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6CA70E | 66 8B 46 22 | mov     ax, word ptr [esi + 0x22]
// >>> 0x6CA712 | D9 5C 24 34 | fstp    dword ptr [esp + 0x34]
//     0x6CA716 | 66 3D 1B 02 | cmp     ax, 0x21B
#define HOOKPOS_CPlane__PreRender_ShadowTypeModel  0x6CA70E
#define HOOKSIZE_CPlane__PreRender_ShadowTypeModel 8
static const DWORD CONTINUE_CPlane__PreRender_ShadowTypeModel = 0x6CA716;

static void __declspec(naked) HOOK_CPlane__PreRender_ShadowTypeModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        fstp    dword ptr [esp + 0x38]
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        pop     ecx
        // CONTINUE re-does the compare on ax, so none is needed here
        jmp     CONTINUE_CPlane__PreRender_ShadowTypeModel
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCarEnterExit::IsRoomForPedToLeaveCar, restores the AT-400's taller stair-door clearance
// exemption for a clone.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x650870 | 66 81 7E 22 41 02 | cmp     word ptr [esi + 0x22], 0x241
//     0x650876 | 74 0C             | je      0x650884
#define HOOKPOS_CCarEnterExit__IsRoomForPedToLeaveCar_AT400DoorException  0x650870
#define HOOKSIZE_CCarEnterExit__IsRoomForPedToLeaveCar_AT400DoorException 6
static const DWORD CONTINUE_CCarEnterExit__IsRoomForPedToLeaveCar_AT400DoorException = 0x650876;

static void __declspec(naked) HOOK_CCarEnterExit__IsRoomForPedToLeaveCar_AT400DoorException()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // eax is read again on the not-taken path at 0x650878 (cmp eax, [esi + 0xFC]).
        push    eax
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x241
        pop     eax
        jmp     CONTINUE_CCarEnterExit__IsRoomForPedToLeaveCar_AT400DoorException
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCarEnterExit::IsRoomForPedToLeaveCar, resolves the model id feeding the Skimmer/Vortex/
// SeaSparrow/Leviathan amphibious water-exit check.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6508ED | 66 8B 76 22       | mov     si, word ptr [esi + 0x22]
// >>> 0x6508F1 | 66 81 FE CC 01    | cmp     si, 0x1CC
//     0x6508F6 | 74 5A             | je      0x650952
#define HOOKPOS_CCarEnterExit__IsRoomForPedToLeaveCar_AmphibiousExitChain  0x6508ED
#define HOOKSIZE_CCarEnterExit__IsRoomForPedToLeaveCar_AmphibiousExitChain 9
static const DWORD CONTINUE_CCarEnterExit__IsRoomForPedToLeaveCar_AmphibiousExitChain = 0x6508F6;

static void __declspec(naked) HOOK_CCarEnterExit__IsRoomForPedToLeaveCar_AmphibiousExitChain()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        // si is left holding the resolved id for the compares that follow
        mov     si, ax
        cmp     si, 0x1CC
        jmp     CONTINUE_CCarEnterExit__IsRoomForPedToLeaveCar_AmphibiousExitChain
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CCarEnterExit::IsPathToDoorBlockedByVehicleCollisionModel, restores the AT-400's door-path
// collision-model exemption for a clone.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x651233 | 66 81 7F 22 41 02 | cmp     word ptr [edi + 0x22], 0x241
//     0x651239 | 0F 84 48 01 00 00 | je      0x651387
#define HOOKPOS_CCarEnterExit__IsPathToDoorBlockedByVehicleCollisionModel_AT400DoorException  0x651233
#define HOOKSIZE_CCarEnterExit__IsPathToDoorBlockedByVehicleCollisionModel_AT400DoorException 6
static const DWORD CONTINUE_CCarEnterExit__IsPathToDoorBlockedByVehicleCollisionModel_AT400DoorException = 0x651239;

static void __declspec(naked) HOOK_CCarEnterExit__IsPathToDoorBlockedByVehicleCollisionModel_AT400DoorException()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [edi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x241
        jmp     CONTINUE_CCarEnterExit__IsPathToDoorBlockedByVehicleCollisionModel_AT400DoorException
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CTaskUtilityLineUpPedWithCar::ProcessPed, AT-400 stair door line-up offset; needs three
// separate hooks, one per case handler of the same anim-id switch.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6515DD | 66 81 7E 22 41 02 | cmp     word ptr [esi + 0x22], 0x241
//     0x6515E3 | 0F 85 9C 02 00 00 | jne     0x651885
#define HOOKPOS_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetA  0x6515DD
#define HOOKSIZE_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetA 6
static const DWORD CONTINUE_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetA = 0x6515E3;

static void __declspec(naked) HOOK_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetA()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x241
        jmp     CONTINUE_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetA
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// Second of the three repeats (see AT400StairOffsetA above for the full explanation).
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x651634 | 66 81 7E 22 41 02 | cmp     word ptr [esi + 0x22], 0x241
//     0x65163A | 0F 85 45 02 00 00 | jne     0x651885
#define HOOKPOS_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetB  0x651634
#define HOOKSIZE_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetB 6
static const DWORD CONTINUE_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetB = 0x65163A;

static void __declspec(naked) HOOK_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetB()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x241
        jmp     CONTINUE_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetB
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// Third of the three repeats (see AT400StairOffsetA above for the full explanation).
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x651722 | 66 81 7E 22 41 02 | cmp     word ptr [esi + 0x22], 0x241
//     0x651728 | 0F 85 57 01 00 00 | jne     0x651885
#define HOOKPOS_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetC  0x651722
#define HOOKSIZE_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetC 6
static const DWORD CONTINUE_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetC = 0x651728;

static void __declspec(naked) HOOK_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetC()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x241
        jmp     CONTINUE_CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetC
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CTaskComplexEnterCar::CreateFirstSubTask, enter-from-water task variant for the
// amphibious family (Skimmer/Vortex/Sea Sparrow/Leviathan).
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x643BE5 | 66 8B 48 22       | mov     cx, word ptr [eax + 0x22]
// >>> 0x643BE9 | 66 81 F9 CC 01    | cmp     cx, 0x1CC
//     0x643BEE | 74 15             | je      0x643C05
#define HOOKPOS_CTaskComplexEnterCar__CreateFirstSubTask_AmphibiousWaterEntryChain  0x643BE5
#define HOOKSIZE_CTaskComplexEnterCar__CreateFirstSubTask_AmphibiousWaterEntryChain 9
static const DWORD CONTINUE_CTaskComplexEnterCar__CreateFirstSubTask_AmphibiousWaterEntryChain = 0x643BEE;

static void __declspec(naked) HOOK_CTaskComplexEnterCar__CreateFirstSubTask_AmphibiousWaterEntryChain()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        movzx   ecx, word ptr [eax + 0x22]
        call    ResolveVehicleParentModelId
        // cx is left holding the resolved id for the compares further down
        mov     cx, ax
        cmp     cx, 0x1CC
        pop     eax
        jmp     CONTINUE_CTaskComplexEnterCar__CreateFirstSubTask_AmphibiousWaterEntryChain
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CPlayerInfo::FindClosestCarSectorList, AT-400 closest-enterable-car distance test.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x56F5F9 | 66 81 7E 22 41 02 | cmp     word ptr [esi + 0x22], 0x241
//     0x56F5FF | D9 00             | fld     dword ptr [eax]
#define HOOKPOS_CPlayerInfo__FindClosestCarSectorList_AT400DistanceException  0x56F5F9
#define HOOKSIZE_CPlayerInfo__FindClosestCarSectorList_AT400DistanceException 6
static const DWORD CONTINUE_CPlayerInfo__FindClosestCarSectorList_AT400DistanceException = 0x56F5FF;

static void __declspec(naked) HOOK_CPlayerInfo__FindClosestCarSectorList_AT400DistanceException()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        movzx   ecx, word ptr [esi + 0x22]
        call    ResolveVehicleParentModelId
        cmp     ax, 0x241
        pop     eax
        jmp     CONTINUE_CPlayerInfo__FindClosestCarSectorList_AT400DistanceException
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CPlayerInfo::Process, water-exit handling (IsRoomForPedToLeaveCar + follow-up task) for
// the amphibious family (Skimmer/Vortex/Sea Sparrow/Leviathan).
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x5707FD | 66 8B 43 22 | mov     ax, word ptr [ebx + 0x22]
// >>> 0x570801 | 66 3D CC 01 | cmp     ax, 0x1CC
//     0x570805 | 74 12       | je      0x570819
#define HOOKPOS_CPlayerInfo__Process_AmphibiousWaterExitChain  0x5707FD
#define HOOKSIZE_CPlayerInfo__Process_AmphibiousWaterExitChain 8
static const DWORD CONTINUE_CPlayerInfo__Process_AmphibiousWaterExitChain = 0x570805;

static void __declspec(naked) HOOK_CPlayerInfo__Process_AmphibiousWaterExitChain()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [ebx + 0x22]
        call    ResolveVehicleParentModelId
        // ax is left holding the resolved id for the compares that follow
        cmp     ax, 0x1CC
        jmp     CONTINUE_CPlayerInfo__Process_AmphibiousWaterExitChain
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CWeapon::FireInstantHit, tighter 0.1 spread (vs 0.3 default) for Sea Sparrow-mounted guns;
// Sparrow/RC Tiger clones aren't covered since the resolver doesn't map them.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x7408E3 | 66 8B 47 22 | mov     ax, word ptr [edi + 0x22]
// >>> 0x7408E7 | 66 3D BF 01 | cmp     ax, 0x1BF
//     0x7408EB | 74 14       | je      0x740901
#define HOOKPOS_CWeapon__FireInstantHit_MountedGunSpreadChain  0x7408E3
#define HOOKSIZE_CWeapon__FireInstantHit_MountedGunSpreadChain 8
static const DWORD CONTINUE_CWeapon__FireInstantHit_MountedGunSpreadChain = 0x7408EB;

static void __declspec(naked) HOOK_CWeapon__FireInstantHit_MountedGunSpreadChain()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [edi + 0x22]
        call    ResolveAircraftWeaponModelId
        // ax is left holding the resolved id for the compares that follow
        cmp     ax, 0x1BF
        jmp     CONTINUE_CWeapon__FireInstantHit_MountedGunSpreadChain
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CShadows::StoreShadowForVehicle, resolves the model id once at the shared load feeding
// both heli/plane shadow jump tables, fixing every clone they cover, not just this task's.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x70BF09 | 0F BF 5F 22 | movsx   ebx, word ptr [edi + 0x22]
// >>> 0x70BF0D | DD D8       | fstp    st(0)
//     0x70BF0F | 8B CF       | mov     ecx, edi
#define HOOKPOS_CShadows__StoreShadowForVehicle_ModelDispatchResolve  0x70BF09
#define HOOKSIZE_CShadows__StoreShadowForVehicle_ModelDispatchResolve 6
static const DWORD CONTINUE_CShadows__StoreShadowForVehicle_ModelDispatchResolve = 0x70BF0F;

static void __declspec(naked) HOOK_CShadows__StoreShadowForVehicle_ModelDispatchResolve()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        movzx   ecx, word ptr [edi + 0x22]
        call    ResolveVehicleParentModelId
        mov     ebx, eax
        // call must not touch the x87 stack; this fstp is the only allowed pop
        fstp    st(0)
        jmp     CONTINUE_CShadows__StoreShadowForVehicle_ModelDispatchResolve
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAEVehicleAudioEntity::ProcessMovingParts plays the Andromada's cargo-door creak, gated
// on model ID twice (outer OR-chain, then a separate dispatch table); needs two hooks.
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
// Outer gate: patches the Andromada (0x250) leg of the six-way OR-chain that decides
// whether moving-parts audio runs at all.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x4FB287 | 66 3D 50 02 | cmp     ax, 0x250
// >>> 0x4FB28B | 74 0A       | je      0x4FB297
//     0x4FB28D | 66 3D 12 02 | cmp     ax, 0x212
#define HOOKPOS_CAEVehicleAudioEntity__ProcessMovingParts_AndromGate  0x4FB287
#define HOOKSIZE_CAEVehicleAudioEntity__ProcessMovingParts_AndromGate 6
static const DWORD CONTINUE_CAEVehicleAudioEntity__ProcessMovingParts_AndromGate = 0x4FB297;
static const DWORD SKIP_CAEVehicleAudioEntity__ProcessMovingParts_AndromGate = 0x4FB28D;

static void __declspec(naked) HOOK_CAEVehicleAudioEntity__ProcessMovingParts_AndromGate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        push    ecx
        push    edx
        call    IsAndromadaOrClone
        test    al, al
        pop     edx
        pop     ecx
        pop     eax
        jz      notAndrom

        jmp     CONTINUE_CAEVehicleAudioEntity__ProcessMovingParts_AndromGate

        notAndrom:
        jmp     SKIP_CAEVehicleAudioEntity__ProcessMovingParts_AndromGate
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// Inner dispatch: forces jump-table case index 4 (the Andromada creak block) since a
// clone's model id falls outside the range-checked table and would otherwise miss it.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x4FB347 | 05 6A FE FF FF | add     eax, 0xFFFFFE6A
//     0x4FB34C | 3D BA 00 00 00 | cmp     eax, 0xBA
#define HOOKPOS_CAEVehicleAudioEntity__ProcessMovingParts_AndromCreakCase  0x4FB347
#define HOOKSIZE_CAEVehicleAudioEntity__ProcessMovingParts_AndromCreakCase 5
static const DWORD CONTINUE_CAEVehicleAudioEntity__ProcessMovingParts_AndromCreakCase = 0x4FB35E;
static const DWORD SKIP_CAEVehicleAudioEntity__ProcessMovingParts_AndromCreakCase = 0x4FB34C;

static void __declspec(naked) HOOK_CAEVehicleAudioEntity__ProcessMovingParts_AndromCreakCase()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    eax
        mov     ecx, edx
        call    IsAndromadaOrClone
        test    al, al
        pop     eax
        jz      notAndrom

        // force case index 4: the Andromada creak block
        mov     eax, 4
        jmp     CONTINUE_CAEVehicleAudioEntity__ProcessMovingParts_AndromCreakCase

        notAndrom:
        add     eax, 0xFFFFFE6A
        jmp     SKIP_CAEVehicleAudioEntity__ProcessMovingParts_AndromCreakCase
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CPlane::PreRender, Andromada ramp/door visual transform block.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C95B8 | 66 81 7E 22 50 02 | cmp     word ptr [esi + 0x22], 0x250
// >>> 0x6C95BE | 0F 85 91 00 00 00 | jne     0x6C9655
//     0x6C95C4 | D9 47 9C          | fld     dword ptr [edi - 0x64]
#define HOOKPOS_CPlane__PreRender_AndromRampBlock  0x6C95B8
#define HOOKSIZE_CPlane__PreRender_AndromRampBlock 12
static const DWORD CONTINUE_CPlane__PreRender_AndromRampBlock = 0x6C95C4;
static const DWORD SKIP_CPlane__PreRender_AndromRampBlock = 0x6C9655;

static void __declspec(naked) HOOK_CPlane__PreRender_AndromRampBlock()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     ecx, esi
        call    IsAndromadaOrClone
        test    al, al
        jz      notAndrom

        jmp     CONTINUE_CPlane__PreRender_AndromRampBlock

        notAndrom:
        jmp     SKIP_CPlane__PreRender_AndromRampBlock
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
    EZHookInstall(CAutomobile__ProcessControl_CementAngleReset);
    EZHookInstall(CAutomobile__ProcessControl_CementMiscGate);
    EZHookInstall(CAutomobile__MovingCollisionSpeed_Cement);
    EZHookInstall(CAutomobile__PreRender_CementTurn);
    EZHookInstall(CAutomobile__ProcessControl_CementDispatch);
    EZHookInstall(CAutomobile__ProcessControl_DozerAngleReset);
    EZHookInstall(CAutomobile__ProcessControl_DozerMiscGate);
    EZHookInstall(CAutomobile__MovingCollisionSpeed_Dozer);
    EZHookInstall(CAutomobile__MovingCollisionSpeed_DozerExtraA);
    EZHookInstall(CAutomobile__MovingCollisionSpeed_DozerExtraB);
    EZHookInstall(CAutomobile__PreRender_DozerSwing);
    EZHookInstall(CAutomobile__ProcessControl_DozerDispatch);
    EZHookInstall(CAutomobile__PreRender_PopUpLights);
    EZHookInstall(CVehicle__DoVehicleLights_PopUpLights);
    EZHookInstall(CAutomobile__GetTowBarPos);
    EZHookInstall(CAutomobile__ProcessControl_DumperDispatch);
    EZHookInstall(CAutomobile__UpdateMovingCollision_DumperAngleReset);
    EZHookInstall(CAutomobile__UpdateMovingCollision_DumperMiscGate);
    EZHookInstall(CAutomobile__UpdateMovingCollision_DumperRotation);
    EZHookInstall(CAutomobile__GetMovingCollisionOffset_Dumper);
    EZHookInstall(CMonsterTruck__PreRender_DumperSwing);
    EZHookInstall(CVehicle__GetPlaneNumGuns_ModelId);
    EZHookInstall(CVehicle__GetPlaneGunsAutoAimAngle_ModelId);
    EZHookInstall(CVehicle__GetPlaneGunsRateOfFire_ModelId);
    EZHookInstall(CVehicle__GetPlaneOrdnanceRateOfFire_ModelId);
    EZHookInstall(CVehicle__GetPlaneGunsPosition_ModelId);
    EZHookInstall(CVehicle__GetPlaneOrdnancePosition_ModelId);
    EZHookInstall(CVehicle__SelectPlaneWeapon_ModelId);
    EZHookInstall(CVehicle__FirePlaneGuns_ModelId);
    EZHookInstall(CVehicle__FireUnguidedMissile_ModelId);
    EZHookInstall(CVehicle__GetPlaneWeaponFiringStatus_ModelId);
    EZHookInstall(CVehicle__ProcessWeapons_HydraCheck);
    EZHookInstall(CHud__DrawCrossHairs_ModelCheck);
    EZHookInstall(CCam__Process_HydraLockOnCheck);
    EZHookInstall(CAutomobile__ProcessCarWheelPair_HydraSteerExempt);
    EZHookInstall(CHeli__Constructor_HunterDoor);
    EZHookInstall(CPlane__Constructor_ModelSwitch);
    EZHookInstall(CPlane__ProcessControlInputs_HydraNozzleTurn);
    EZHookInstall(CAEVehicleAudioEntity__ProcessAircraft_JetClass);
    EZHookInstall(CAEVehicleAudioEntity__ProcessGenericJet_ModelSelect);
    EZHookInstall(CAEVehicleAudioEntity__ProcessSpecialVehicle_ModelSwitch);
    EZHookInstall(CAutomobile__ProcessBuoyancy_AmphibiousRotorExempt);
    EZHookInstall(CAutomobile__ProcessBuoyancy_AmphibiousSafeFloat);
    EZHookInstall(CAutomobile__ProcessBuoyancy_VortexSpeedDampingExempt);
    EZHookInstall(CPlane__ProcessControlInputs_VortexSteerAxis);
    EZHookInstall(CPlane__ProcessControlInputs_VortexSteerAngleSource);
    EZHookInstall(CCam__ProcessFollowCarSA_DistanceCategoryModel);
    EZHookInstall(CAutomobile__ProcessControl_SkimmerBoatControl);
    EZHookInstall(CHeli__ProcessControl_SearchLightModel);
    EZHookInstall(CAutomobile__ProcessControl_RCBaronFakePhysicsGate);
    EZHookInstall(CAutomobile__ProcessControl_RCBaronNormalGate);
    EZHookInstall(CAutomobile__ProcessFlyingCarStuff_RCBaronFastPathGate);
    EZHookInstall(CAutomobile__ProcessFlyingCarStuff_RCBaronControlScaleSelect);
    EZHookInstall(CAutomobile__ProcessFlyingCarStuff_RCBaronBehaviorDispatch);
    EZHookInstall(CVehicle__SetModelIndex_RCVehicleFlag);
    EZHookInstall(CPlane__ProcessControl_SmokeEjectorModel);
    EZHookInstall(CPlane__ProcessControl_SmokeParticleModel);
    EZHookInstall(CPlane__ProcessControl_VortexTrailModel);
    EZHookInstall(CAutomobile__ProcessControl_AmphibiousPhysicsKeepAlive);
    EZHookInstall(CPlane__Constructor_HydraNozzlePrime);
    EZHookInstall(CVehicle__FlyingControl_HydraPlaneThrust);
    EZHookInstall(CVehicle__FlyingControl_HydraHoverThrust);
    EZHookInstall(CPlane__PreRender_ComponentSwitch);
    EZHookInstall(CPlane__PreRender_HydraRotationMode);
    EZHookInstall(CPlane__PreRender_RampAndNozzleModel);
    EZHookInstall(CAutomobile__ProcessControl_AndromAngleReset);
    EZHookInstall(CAutomobile__ProcessControl_AndromMiscGate);
    EZHookInstall(CAutomobile__MovingCollisionSpeed_AndromA);
    EZHookInstall(CAutomobile__MovingCollisionSpeed_AndromB);
    EZHookInstall(CAutomobile__MovingCollisionSpeed_AndromC);
    EZHookInstall(CAutomobile__ProcessControl_AndromDispatch);
    EZHookInstall(CVehicle__ProcessBoatControl_SkimmerCapsizedBuoyancy);
    EZHookInstall(CVehicle__ProcessBoatControl_SkimmerWaterLandingDamping);
    EZHookInstall(CVehicle__ProcessBoatControl_SkimmerAquaplaneThrust);
    EZHookInstall(CVehicle__ProcessBoatControl_SkimmerSlowingDownGate);
    EZHookInstall(CVehicle__ProcessBoatControl_SkimmerTurnResistanceSkip);
    EZHookInstall(CVehicle__ApplyBoatWaterResistance_SkimmerHullDrag);
    EZHookInstall(CBuoyancy__PreCalcSetup_LeviathanSkimmerModel);
    EZHookInstall(CPlane__ProcessControl_SkimmerWheelStatus);
    EZHookInstall(CPlane__PreRender_SkimmerBoatSplashesCall);
    EZHookInstall(CVehicle__DoBoatSplashes_SkimmerIntensityModel);
    EZHookInstall(CWaterLevel__RenderBoatWakes_SkimmerModel);
    EZHookInstall(CBoat__Render_SkimmerPropellerSpin);
    EZHookInstall(CCamera__CamControl_SkimmerCameraMode);
    EZHookInstall(CCam__TryToStartNewCamMode_SkimmerCheckA);
    EZHookInstall(CCam__TryToStartNewCamMode_SkimmerCheckB);
    EZHookInstall(CCam__TryToStartNewCamMode_SkimmerCheckC);
    EZHookInstall(CCam__TryToStartNewCamMode_SkimmerCheckD);
    EZHookInstall(CCam__TryToStartNewCamMode_SkimmerCheckE);
    EZHookInstall(CCam__Process_SkimmerFramingA);
    EZHookInstall(CCam__Process_SkimmerFramingB);
    EZHookInstall(CCam__Process_SkimmerFramingC);
    EZHookInstall(CCam__Process_SkimmerFramingD);
    EZHookInstall(CCam__Process_SkimmerFramingE);
    EZHookInstall(CRenderer__SetupEntityVisibility_SkimmerCulling);
    EZHookInstall(CAutomobile__ProcessControl_VortexTractionBoost);
    EZHookInstall(CAutomobile__ProcessControl_VortexWheelSettleExempt);
    EZHookInstall(CAutomobile__ProcessSuspension_VortexSpringForceScale);
    EZHookInstall(CAutomobile__ProcessSuspension_VortexGroundSpringExempt);
    EZHookInstall(CVehicle__FlyingControl_VortexSkimmerGroundRestGate);
    EZHookInstall(CVehicle__FlyingControl_VortexThrustFormula);
    EZHookInstall(CVehicle__FlyingControl_VortexTurnDampingClamp);
    EZHookInstall(CPlane__Constructor_VortexFlyerCollisionExempt);
    EZHookInstall(CPlane__VehicleDamage_VortexCarStyleDamage);
    EZHookInstall(CPlane__PreRender_VortexFanFXMatrix);
    EZHookInstall(CAEVehicleAudioEntity__Initialise_SpecialVehicleModelSwitch);
    EZHookInstall(CPlane__PreRender_ShadowTypeModel);
    EZHookInstall(CCarEnterExit__IsRoomForPedToLeaveCar_AT400DoorException);
    EZHookInstall(CCarEnterExit__IsRoomForPedToLeaveCar_AmphibiousExitChain);
    EZHookInstall(CCarEnterExit__IsPathToDoorBlockedByVehicleCollisionModel_AT400DoorException);
    EZHookInstall(CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetA);
    EZHookInstall(CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetB);
    EZHookInstall(CTaskUtilityLineUpPedWithCar__ProcessPed_AT400StairOffsetC);
    EZHookInstall(CTaskComplexEnterCar__CreateFirstSubTask_AmphibiousWaterEntryChain);
    EZHookInstall(CPlayerInfo__FindClosestCarSectorList_AT400DistanceException);
    EZHookInstall(CPlayerInfo__Process_AmphibiousWaterExitChain);
    EZHookInstall(CWeapon__FireInstantHit_MountedGunSpreadChain);
    EZHookInstall(CShadows__StoreShadowForVehicle_ModelDispatchResolve);
    EZHookInstall(CAEVehicleAudioEntity__ProcessMovingParts_AndromGate);
    EZHookInstall(CAEVehicleAudioEntity__ProcessMovingParts_AndromCreakCase);
    EZHookInstall(CPlane__PreRender_AndromRampBlock);
}
