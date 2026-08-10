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
    const std::uint32_t modelId = vehicle->m_nModelIndex;
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
//
// CMultiplayerSA::InitHooks_Vehicles
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Vehicles()
{
    EZHookInstall(CDamageManager__ProgressDoorDamage);
    EZHookInstall(CAutomobile__GetTowBarPos);
    EZHookInstall(CAutomobile__ProcessControl_DumperDispatch);
    EZHookInstall(CAutomobile__UpdateMovingCollision_DumperAngleReset);
    EZHookInstall(CAutomobile__UpdateMovingCollision_DumperMiscGate);
    EZHookInstall(CAutomobile__UpdateMovingCollision_DumperRotation);
    EZHookInstall(CAutomobile__GetMovingCollisionOffset_Dumper);
    EZHookInstall(CMonsterTruck__PreRender_DumperSwing);
}
