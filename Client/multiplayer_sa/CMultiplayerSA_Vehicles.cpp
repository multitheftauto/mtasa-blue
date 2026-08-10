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
    const std::uint32_t modelId = vehicle->m_nModelIndex;
    if (modelId == static_cast<std::uint32_t>(VehicleType::VT_ZR350))
        return true;

    CModelInfo* modelInfo = pGameInterface->GetModelInfo(modelId);
    return modelInfo && modelInfo->GetParentID() == static_cast<unsigned int>(VehicleType::VT_ZR350);
}

//////////////////////////////////////////////////////////////////////////////////////////
// CAutomobile::PreRender, swinging the misc_a component out
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
// CMultiplayerSA::InitHooks_Vehicles
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Vehicles()
{
    EZHookInstall(CDamageManager__ProgressDoorDamage);
    EZHookInstall(CAutomobile__PreRender_PopUpLights);
    EZHookInstall(CVehicle__DoVehicleLights_PopUpLights);
    EZHookInstall(CAutomobile__GetTowBarPos);
}
