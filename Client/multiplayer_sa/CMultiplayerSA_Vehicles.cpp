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
// CVehicleModelInfo::SetEditableMaterials / ResetEditableMaterials
//
// Every material repainted for a vehicle is saved in a restore list that is replayed after
// rendering. GTA's list at 0xB4DBE8 holds 256 entries without a bounds check and is directly
// followed by ms_lightsOn and ms_currentCol, so vehicle models with enough editable materials
// overwrote the colour slot indices mid-pass and the remaining materials were painted with
// random colours. Use a larger list and skip atomics whose materials would no longer fit.
//
//////////////////////////////////////////////////////////////////////////////////////////
// SetEditableMaterials
//     0x4C843A | 68 E0 83 4C 00             | push    offset SetEditableMaterialsAtomicCB
//     0x4C8440 | C7 44 24 0C E8 DB B4 00    | mov     [esp+0Ch], offset gRestoreEntries
// ResetEditableMaterials
//     0x4C8460 | 8B 0D E8 DB B4 00          | mov     ecx, [gRestoreEntries]
//     0x4C8468 | B8 E8 DB B4 00             | mov     eax, offset gRestoreEntries
#define FUNC_SetEditableMaterialsAtomicCB 0x4C83E0

namespace
{
    struct SMaterialRestoreEntry
    {
        void* pAddress;
        DWORD dwValue;
    };

    // Alpha pass, then either remap texture + colour or lights colour + texture + lighting
    constexpr std::size_t MAX_RESTORE_ENTRIES_PER_MATERIAL = 4;

    SMaterialRestoreEntry materialRestoreEntries[4096];
}  // namespace

static RpAtomic* __cdecl SetEditableMaterialsAtomicCB(RpAtomic* atomic, void* data)
{
    const SMaterialRestoreEntry* nextEntry = *static_cast<SMaterialRestoreEntry**>(data);
    const std::size_t            entriesLeft = static_cast<std::size_t>(std::end(materialRestoreEntries) - nextEntry);
    const RpGeometry*            geometry = atomic->geometry;

    // Keep one entry free for the terminator written by SetEditableMaterials
    if (!geometry || entriesLeft <= static_cast<std::size_t>(geometry->materials.entries) * MAX_RESTORE_ENTRIES_PER_MATERIAL)
        return atomic;

    return reinterpret_cast<RpAtomic*(__cdecl*)(RpAtomic*, void*)>(FUNC_SetEditableMaterialsAtomicCB)(atomic, data);
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

    MemPut<DWORD>(0x4C843A + 1, (DWORD)&SetEditableMaterialsAtomicCB);
    MemPut<DWORD>(0x4C8440 + 4, (DWORD)materialRestoreEntries);
    MemPut<DWORD>(0x4C8460 + 2, (DWORD)materialRestoreEntries);
    MemPut<DWORD>(0x4C8468 + 1, (DWORD)materialRestoreEntries);
}
