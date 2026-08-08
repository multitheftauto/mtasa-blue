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
    const std::uint32_t modelId = vehicle->m_nModelIndex;
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
        pushad
        mov     ecx, edi
        call    IsDozerOrClone
        test    al, al
        popad
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
        pushad
        mov     ecx, edi
        call    IsDozerOrClone
        test    al, al
        popad
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

        pushad
        mov     ecx, edi
        call    IsDozerOrClone
        test    al, al
        popad
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
        pushad
        mov     ecx, esi
        call    IsDozerOrClone
        test    al, al
        popad
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
        // ecx already holds the vehicle pointer here, nothing to move into it.
        pushad
        call    IsDozerOrClone
        test    al, al
        popad
        jz      notDozer

        jmp     CONTINUE_CAutomobile__MovingCollisionSpeed_DozerExtraB

        notDozer:
        jmp     SKIP_CAutomobile__MovingCollisionSpeed_DozerExtraB
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::PreRender, swinging the misc_a component out
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
        pushad
        mov     ecx, esi
        call    IsDozerOrClone
        test    al, al
        popad
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
        pushad
        mov     ecx, esi
        call    IsDozerOrClone
        test    al, al
        popad
        jz      notDozer

        jmp     CONTINUE_CAutomobile__ProcessControl_DozerDispatch

        notDozer:
        jmp     SKIP_CAutomobile__ProcessControl_DozerDispatch
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
    EZHookInstall(CAutomobile__ProcessControl_DozerAngleReset);
    EZHookInstall(CAutomobile__ProcessControl_DozerMiscGate);
    EZHookInstall(CAutomobile__MovingCollisionSpeed_Dozer);
    EZHookInstall(CAutomobile__MovingCollisionSpeed_DozerExtraA);
    EZHookInstall(CAutomobile__MovingCollisionSpeed_DozerExtraB);
    EZHookInstall(CAutomobile__PreRender_DozerSwing);
    EZHookInstall(CAutomobile__ProcessControl_DozerDispatch);
}
