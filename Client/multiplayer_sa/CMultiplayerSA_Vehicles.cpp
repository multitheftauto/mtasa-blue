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
    const std::uint32_t modelId = vehicle->m_nModelIndex;
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
        pushad
        mov     ecx, edi
        call    IsCementTruckOrClone
        test    al, al
        popad
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
        pushad
        mov     ecx, edi
        call    IsCementTruckOrClone
        test    al, al
        popad
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
        pushad
        mov     ecx, edi
        call    IsCementTruckOrClone
        test    al, al
        popad
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
        pushad
        mov     ecx, esi
        call    IsCementTruckOrClone
        test    al, al
        popad
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
        pushad
        mov     ecx, esi
        call    IsCementTruckOrClone
        test    al, al
        popad
        jz      notCement

        jmp     CONTINUE_CAutomobile__ProcessControl_CementDispatch

        notCement:
        jmp     SKIP_CAutomobile__ProcessControl_CementDispatch
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
    EZHookInstall(CAutomobile__ProcessControl_CementAngleReset);
    EZHookInstall(CAutomobile__ProcessControl_CementMiscGate);
    EZHookInstall(CAutomobile__MovingCollisionSpeed_Cement);
    EZHookInstall(CAutomobile__PreRender_CementTurn);
    EZHookInstall(CAutomobile__ProcessControl_CementDispatch);
}
