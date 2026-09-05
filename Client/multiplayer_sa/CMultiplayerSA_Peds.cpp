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
#include <game/RenderWare.h>

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
// CTaskSimpleJetPack::RenderJetPack / CAEPedAudioEntity::UpdateJetPack
//
// Hide and mute a ped's jetpack whenever the ped itself is hidden (other interior or alpha 0).
//
//////////////////////////////////////////////////////////////////////////////////////////

// CTaskSimpleJetPack::RenderJetPack draws the jetpack from its own clump, separate from the ped's;
// interior hiding and setPedAlpha never reach it. Skip the render (and thruster FX) when the ped
// is hidden, and copy the ped clump alpha onto the jetpack so me.alpha = 0 does not leave a
// floating pack (#5225). Thruster FxSystem has no colour alpha, so scale spawn rate by ped alpha
// instead — otherwise translucent peds still show full-bright thruster flames.
#define HOOKPOS_CTaskSimpleJetPack_RenderJetPack 0x67F6A0
DWORD RETURN_CTaskSimpleJetPack_RenderJetPack = 0x67F6AC;
DWORD SKIP_CTaskSimpleJetPack_RenderJetPack = 0x67FA11;

static constexpr DWORD FUNC_CVisibilityPlugins_SetClumpAlpha = 0x732B00;
static constexpr DWORD FUNC_CVisibilityPlugins_GetClumpAlpha = 0x732B20;
static constexpr DWORD FUNC_RpClumpForAllAtomics = 0x749B70;
// rpGEOMETRYMODULATEMATERIALCOLOR — material colour (including alpha) is otherwise ignored.
static constexpr unsigned int RpGeometryModulateMaterialColor = 0x00000040;
// CTaskSimpleJetPack::m_ThrusterFX[2] (left/right), see gta-reversed TaskSimpleJetPack.h
static constexpr unsigned int OFFSET_Task_ThrusterFX = 0x64;
// FxSystem_c::m_nRateMult — 1000 == density 1.0 (matches CFxSystemSA::SetEffectDensity)
static constexpr unsigned int OFFSET_FxSystem_RateMult = 0x5E;
static constexpr short        FX_RATE_MULT_FULL = 1000;

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
RpClump*         pJetPackClumpForAlpha;
void*            pJetPackTaskForAlpha;

static int GetPedClumpAlpha(CPedSAInterface* pPed)
{
    if (!pPed || !pPed->m_pRwObject)
        return 255;

    return ((int(__cdecl*)(RpClump*))FUNC_CVisibilityPlugins_GetClumpAlpha)(pPed->m_pRwObject);
}

static bool IsPedJetpackHidden(CPedSAInterface* pPed)
{
    if (!pPed)
        return false;

    if (pPed->m_areaCode != *reinterpret_cast<std::uint8_t*>(CWorld_CurrentArea))
        return true;

    // StreamedInPulse writes ped alpha onto the ped clump. Alpha 0 should hide the jetpack
    // the same way a different interior does, including thruster FX.
    return GetPedClumpAlpha(pPed) == 0;
}

bool CTaskSimpleJetPack_ShouldHide()
{
    return IsPedJetpackHidden(pJetpackHookPedInterface);
}

static RpAtomic* SetJetpackAtomicAlpha(RpAtomic* pAtomic, void* pData)
{
    RpGeometry* pGeometry = pAtomic->geometry;
    if (!pGeometry)
        return pAtomic;

    const auto ucAlpha = static_cast<unsigned char>(reinterpret_cast<std::uintptr_t>(pData));
    pGeometry->flags |= RpGeometryModulateMaterialColor;

    for (int i = 0; i < pGeometry->materials.entries; ++i)
    {
        if (RpMaterial* pMaterial = pGeometry->materials.materials[i])
            pMaterial->color.a = ucAlpha;
    }

    return pAtomic;
}

// FxSystem_c has no per-system colour alpha; rate mult is the only knob that fades thrusters with the ped.
static void ApplyThrusterFxAlpha(void* pTask, int iAlpha)
{
    if (!pTask)
        return;

    auto**      ppThrusterFX = reinterpret_cast<void**>(static_cast<char*>(pTask) + OFFSET_Task_ThrusterFX);
    const short sRateMult = static_cast<short>((Clamp(0, iAlpha, 255) / 255.0f) * FX_RATE_MULT_FULL);

    for (int i = 0; i < 2; ++i)
    {
        if (void* pFx = ppThrusterFX[i])
            *reinterpret_cast<short*>(static_cast<char*>(pFx) + OFFSET_FxSystem_RateMult) = sRateMult;
    }
}

// Kill both thruster systems (CTaskSimpleJetPack::StopJetPackEffect).
static constexpr DWORD FUNC_CTaskSimpleJetPack_StopJetPackEffect = 0x67BA10;

static void KillThrusterFx(void* pTask)
{
    if (!pTask)
        return;

    ((void(__thiscall*)(void*))FUNC_CTaskSimpleJetPack_StopJetPackEffect)(pTask);
}

static int s_iLastThrusterAlpha = 255;

static void CTaskSimpleJetPack_ApplyPedAlpha()
{
    if (!pJetPackClumpForAlpha || !pJetpackHookPedInterface)
        return;

    const int iAlpha = GetPedClumpAlpha(pJetpackHookPedInterface);
    ((void(__cdecl*)(RpClump*, int))FUNC_CVisibilityPlugins_SetClumpAlpha)(pJetPackClumpForAlpha, iAlpha);
    // RpClumpRender uses material alpha, not CVisibilityPlugins clump alpha, for this object clump.
    ((RpClump * (__cdecl*)(RpClump*, RpAtomic * (__cdecl*)(RpAtomic*, void*), void*)) FUNC_RpClumpForAllAtomics)(
        pJetPackClumpForAlpha, SetJetpackAtomicAlpha, reinterpret_cast<void*>(static_cast<std::uintptr_t>(iAlpha)));

    // Already-spawned thruster particles keep their old brightness when alpha drops, so kill them
    // on change. DoJetPackEffect recreates the systems later this frame; next RenderJetPack scales
    // their spawn rate. Continuous frames only touch rate mult.
    if (iAlpha != s_iLastThrusterAlpha)
    {
        KillThrusterFx(pJetPackTaskForAlpha);
        s_iLastThrusterAlpha = iAlpha;
    }
    ApplyThrusterFxAlpha(pJetPackTaskForAlpha, iAlpha);
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
            mov     eax, [ebp+40h]
            mov     pJetPackClumpForAlpha, eax
            mov     pJetPackTaskForAlpha, ebp
        }
        // clang-format on

        CTaskSimpleJetPack_ApplyPedAlpha();

        // clang-format off
        __asm
        {
            // EAX isn't preserved across the calls above, so reload the clump pointer it held
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
    return IsPedJetpackHidden(pJetpackAudioHookInterface ? pJetpackAudioHookInterface->ped : nullptr);
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

    HookInstall(HOOKPOS_CTaskSimpleJetPack_RenderJetPack, (DWORD)HOOK_CTaskSimpleJetPack_RenderJetPack, 12);
    HookInstall(HOOKPOS_CAEPedAudioEntity_UpdateJetPack, (DWORD)HOOK_CAEPedAudioEntity_UpdateJetPack, 6);
}
