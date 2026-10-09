/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Peds.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

#include "CRemoteDataSA.h"

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPed::DoFootLanded
//
// Whenever a pedestrian's foot hits the ground
//
//////////////////////////////////////////////////////////////////////////////////////////
static PedStepHandler* pPedStepHandler = nullptr;

void CMultiplayerSA::SetPedStepHandler(PedStepHandler* pHandler)
{
    pPedStepHandler = pHandler;
}

static void __cdecl DoFootLanded(CPedSAInterface* pPedSAInterface, short footId, char unknown1)
{
    if (pPedStepHandler)
        pPedStepHandler(pPedSAInterface, footId == LANDED_PED_LEFT_FOOT ? true : false);
}

#define HOOKPOS_CPed_DoFootLanded  0x5E5380
#define HOOKSIZE_CPed_DoFootLanded 6
static const DWORD CONTINUE_CPed_DoFootLanded = 0x5E5386;

static void __declspec(naked) HOOK_CPed_DoFootLanded()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    [esp + 32 + 4]
        push    [esp + 32 + 8]
        push    ecx
        call    DoFootLanded
        add     esp, 12
        popad

        // Continue
        sub     esp, 30h
        push    esi
        mov     esi, ecx
        jmp     CONTINUE_CPed_DoFootLanded
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPed::IsPlayer
//
// Check if ped is a player ped
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CPed_IsPlayer  0x5DF8F0
#define HOOKSIZE_CPed_IsPlayer 6
static constexpr std::uintptr_t RETURN_CPed_IsPlayer = 0x5DF8F6;

static void __declspec(naked) HOOK_CPed_IsPlayer()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     eax, dword ptr [ecx + 598h]
        jmp     RETURN_CPed_IsPlayer
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPed::GetWeaponSkill
//
// Synchronize and return the weapon skill stat (Poor, Standard, Pro) for remote players
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool GetRemotePedWeaponSkill(CPedSAInterface* pedInterface, eWeaponType weaponType, eWeaponSkill& outSkill)
{
    SClientEntity<CPedSA>* pedClientEntity = pGameInterface->GetPools()->GetPed((DWORD*)pedInterface);
    CPed*                  ped = pedClientEntity ? pedClientEntity->pEntity : nullptr;
    if (!ped)
        return false;

    CPed* localPlayerPed = pGameInterface->GetPools()->GetPedFromRef(1);
    if (ped == localPlayerPed)
        return false;

    if (weaponType < WEAPONTYPE_PISTOL || weaponType > WEAPONTYPE_TEC9)
        return false;

    auto* playerPed = dynamic_cast<CPlayerPed*>(ped);
    if (!playerPed)
        return false;

    CRemoteDataStorageSA* remoteData = CRemoteDataSA::GetRemoteDataStorage(playerPed);
    if (!remoteData)
        return false;

    float        stat = remoteData->m_stats.StatTypesFloat[pGameInterface->GetStats()->GetSkillStatIndex(weaponType)];
    CWeaponInfo* poor = pGameInterface->GetWeaponInfo(weaponType, WEAPONSKILL_POOR);
    CWeaponInfo* std = pGameInterface->GetWeaponInfo(weaponType, WEAPONSKILL_STD);
    CWeaponInfo* pro = pGameInterface->GetWeaponInfo(weaponType, WEAPONSKILL_PRO);

    if (stat >= pro->GetRequiredStatLevel())
        outSkill = WEAPONSKILL_PRO;
    else if (stat >= std->GetRequiredStatLevel())
        outSkill = WEAPONSKILL_STD;
    else
        outSkill = WEAPONSKILL_POOR;

    return true;
}

#define HOOKPOS_CPed_GetWeaponSkill  0x5E3B60
#define HOOKSIZE_CPed_GetWeaponSkill 8
static constexpr std::uintptr_t RETURN_CPed_GetWeaponSkill = 0x5E3B68;

static void __declspec(naked) HOOK_CPed_GetWeaponSkill()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // ecx = this (CPedSAInterface), [esp+4] = weaponType
    // clang-format off
    __asm
    {
        sub     esp, 4                  // allocate space for outSkill (eWeaponSkill)
        lea     eax, [esp]              // &outSkill
        push    eax
        push    dword ptr [esp + 8 + 4] // weaponType (arg1)
        push    ecx                     // pedInterface (this)
        call    GetRemotePedWeaponSkill
        add     esp, 12
        test    al, al
        jz      use_original_logic

        // Custom skill calculated for remote player
        mov     al, byte ptr [esp]      // load outSkill
        add     esp, 4                  // deallocate local stack space
        retn    4

    use_original_logic:
        add     esp, 4                  // deallocate local stack space
        push    esi
        mov     esi, [esp + 8]
        cmp     esi, 16h
        jmp     RETURN_CPed_GetWeaponSkill
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPed::AddGogglesModel
//
// Only apply the full-screen goggles visual effect if the ped putting them on is the local player
//
//////////////////////////////////////////////////////////////////////////////////////////
static bool CPed_AddGogglesModelCheck(void* pedInterface)
{
    SClientEntity<CPedSA>* pedClientEntity = pGameInterface->GetPools()->GetPed((DWORD*)pedInterface);
    CPed*                  ped = pedClientEntity ? pedClientEntity->pEntity : nullptr;
    return ped == pGameInterface->GetPools()->GetPedFromRef(1);
}

#define HOOKPOS_CPed_AddGogglesModel  0x5E3ACB
#define HOOKSIZE_CPed_AddGogglesModel 6
static constexpr std::uintptr_t RETURN_CPed_AddGogglesModel = 0x5E3AD4;

static void __declspec(naked) HOOK_CPed_AddGogglesModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    esi
        call    CPed_AddGogglesModelCheck
        add     esp, 4

        test    al, al
        jz      skip
        mov     eax, [esp + 10h]
        mov     [esi + 500h], eax
        mov     byte ptr [eax], 1

    skip:
        jmp     RETURN_CPed_AddGogglesModel
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTaskSimpleJetPack::RenderJetPack / CAEPedAudioEntity::UpdateJetPack
//
// Hide and mute a ped's jetpack whenever the ped itself is hidden due to being in a different
// interior
//
//////////////////////////////////////////////////////////////////////////////////////////

// CTaskSimpleJetPack::RenderJetPack draws the jetpack from its own clump, separate from the ped's;
// interior-based alpha hiding never reaches it. This hook skips the render and its thruster FX
// whenever the ped itself is hidden.
#define HOOKPOS_CTaskSimpleJetPack_RenderJetPack 0x67F6A0
DWORD RETURN_CTaskSimpleJetPack_RenderJetPack = 0x67F6AC;
DWORD SKIP_CTaskSimpleJetPack_RenderJetPack = 0x67FA11;

// CAEPedAudioEntity::UpdateJetPack recomputes the engine sound's volume from its own ramp state
// every tick; this hook releases the sound channels while the ped is hidden and recreates them
// once visible again, so a hidden jetpack neither plays nor keeps its channels mixing for nothing.
#define HOOKPOS_CAEPedAudioEntity_UpdateJetPack 0x4E0EE0
DWORD RETURN_CAEPedAudioEntity_UpdateJetPack = 0x4E0EE6;
DWORD SKIP_CAEPedAudioEntity_UpdateJetPack = 0x4E1112;

// CWorld::m_CurrentArea, read directly rather than through CWorldSA to keep this hook self
// contained (same value CClientPed already compares a ped's own m_areaCode against to hide it).
static constexpr std::uintptr_t CWorld_CurrentArea = 0xB72914;

CPedSAInterface* pJetpackHookPedInterface;

bool CTaskSimpleJetPack_ShouldHide()
{
    return pJetpackHookPedInterface && pJetpackHookPedInterface->m_areaCode != *reinterpret_cast<std::uint8_t*>(CWorld_CurrentArea);
}

static void __declspec(naked) HOOK_CTaskSimpleJetPack_RenderJetPack()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     eax, [esp+4]
        mov     pJetpackHookPedInterface, eax

        // Replicate the bytes this hook overwrites so the rest of the function keeps working
        sub     esp, 1Ch
        push    ebp
        mov     ebp, ecx
        mov     eax, [ebp+40h]
        push    esi
        xor     esi, esi
    }
    // clang-format on

    if (CTaskSimpleJetPack_ShouldHide())
    {
        // Same cleanup the game runs when the jetpack is dropped; kills both thruster FX and
        // skips the clump render entirely. EBP still holds this, callee-saved across the call
        // above, and that's all this path needs.
        // clang-format off
        __asm
        {
            jmp     SKIP_CTaskSimpleJetPack_RenderJetPack
        }
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            // EAX isn't preserved across the call above, so reload the clump pointer it held
            mov     eax, [ebp+40h]
            jmp     RETURN_CTaskSimpleJetPack_RenderJetPack
        }
        // clang-format on
    }
}

DWORD FUNC_CAEPedAudioEntity_TurnOnJetPack = 0x4E28A0;
DWORD FUNC_CAEPedAudioEntity_TurnOffJetPack = 0x4E2A70;

CPedSoundEntitySAInterface* pJetpackAudioHookInterface;

bool CAEPedAudioEntity_IsJetpackHidden()
{
    return pJetpackAudioHookInterface && pJetpackAudioHookInterface->ped &&
           pJetpackAudioHookInterface->ped->m_areaCode != *reinterpret_cast<std::uint8_t*>(CWorld_CurrentArea);
}

static void __declspec(naked) HOOK_CAEPedAudioEntity_UpdateJetPack()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     pJetpackAudioHookInterface, ecx

        // Replicate the byte this hook overwrites so the rest of the function keeps working
        mov     al, [ecx+98h]
    }
    // clang-format on

    if (CAEPedAudioEntity_IsJetpackHidden())
    {
        // Releases the sound channels, so a hidden jetpack costs nothing to mix. TurnOffJetPack()
        // also clears jetpackSoundPlaying, which TurnOnJetPack() below requires to be false before
        // it will recreate them; skipping that would leave the jetpack silent for good once hidden,
        // even after becoming visible again. Repeating this every tick while hidden is harmless,
        // TurnOffJetPack() only touches anything if the channels aren't already gone. Jumping to
        // the game's own "nothing to update" exit then skips this tick's now pointless ramp calc.
        // clang-format off
        __asm
        {
            mov     ecx, pJetpackAudioHookInterface
            cmp     dword ptr [ecx+9Ch], 0
            jz      alreadyStopped
            call    FUNC_CAEPedAudioEntity_TurnOffJetPack
        alreadyStopped:
            jmp     SKIP_CAEPedAudioEntity_UpdateJetPack
        }
        // clang-format on
    }
    else
    {
        // clang-format off
        __asm
        {
            // Coming back into view with the channels released (we stopped them earlier): bring
            // them back the same way the game originally started them, ramping up from silence
            // instead of popping straight to full volume.
            mov     ecx, pJetpackAudioHookInterface
            cmp     dword ptr [ecx+9Ch], 0
            jnz     alreadyPlaying
            call    FUNC_CAEPedAudioEntity_TurnOnJetPack
        alreadyPlaying:
            // EAX/ECX/EDX aren't preserved across a call, so reload what the resumed code needs
            mov     ecx, pJetpackAudioHookInterface
            mov     al, [ecx+98h]
            jmp     RETURN_CAEPedAudioEntity_UpdateJetPack
        }
        // clang-format on
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Peds
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Peds()
{
    EZHookInstall(CPed_DoFootLanded);
    EZHookInstall(CPed_IsPlayer);
    EZHookInstall(CPed_GetWeaponSkill);
    EZHookInstall(CPed_AddGogglesModel);

    HookInstall(HOOKPOS_CTaskSimpleJetPack_RenderJetPack, (DWORD)HOOK_CTaskSimpleJetPack_RenderJetPack, 12);
    HookInstall(HOOKPOS_CAEPedAudioEntity_UpdateJetPack, (DWORD)HOOK_CAEPedAudioEntity_UpdateJetPack, 6);
}
