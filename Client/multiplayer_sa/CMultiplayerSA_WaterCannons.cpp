/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_WaterCannons.cpp
 *  PURPOSE:     Script-owned water cannons, kept apart from the vehicle-owned ones
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include <algorithm>
#include <cstdint>
#include <map>
#include <vector>

// Native CWaterCannon (per-instance) methods. The Firetruck/SWAT tank's own water cannons live in
// a single fixed-size, byte-limited array (CWaterCannons::aCannons, see CGameSA.h) that every
// vehicle shares; a script spawning many custom cannons into that same array could starve a real
// vehicle of a slot. Calling these directly on our own, separately allocated instances instead
// keeps the two pools from ever competing for space.
#define FUNC_CWaterCannon_Constructor        0x728B10
#define FUNC_CWaterCannon_Destructor         0x728B30
#define FUNC_CWaterCannon_Init               0x728B40
#define FUNC_CWaterCannon_UpdateNewInput     0x728C20
#define FUNC_CWaterCannon_UpdateOncePerFrame 0x72A280
#define FUNC_CWaterCannon_Render             0x728DA0
#define SIZE_CWaterCannon                    0x3CC

// The jet's audio entity is embedded at this offset; its per-frame Service is what the stock
// CWaterCannons::Update calls for the vehicle-owned cannons, and Initialise wires its owner entity
#define OFFSET_CWaterCannon_Audio                 0x32C
#define FUNC_CAEWaterCannonAudioEntity_Initialise 0x503060
#define FUNC_CAEWaterCannonAudioEntity_Service    0x5030D0

// The jet-sound gate in Service is "section in use AND owner vehicle (cannon+0) != 0". Our
// vehicle-less cannons have no owner, so this je would mute them; NOP it to drop just the owner
// half of the gate. Firetruck cannons always have an owner, so their behaviour is unchanged.
#define PATCH_CWaterCannon_SoundGateOwnerCheck 0x503127

// Not a native constraint, since these live entirely outside the vehicles' own fixed-size array;
// just a sanity ceiling so a runaway script can't grow this list without bound. Update_OncePerFrame
// runs a world-wide ped scan for every entry every few frames, so this is also a soft cost limit.
static constexpr size_t MAX_CUSTOM_WATER_CANNONS = 256;

struct SWaterCannonColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

static std::vector<void*>                 customWaterCannons;
static std::map<void*, SWaterCannonColor> customWaterCannonColors;

// Which of our cannons is being rendered right now, so the shared native Render colour code knows
// whose colour to use; null while the game renders the vehicle-owned cannons, which keep the
// native colour. Set around each of our own Render calls, never fished out of registers
static void* g_pRenderingCustomCannon = nullptr;

void* CMultiplayerSA::CreateCustomWaterCannon()
{
    if (customWaterCannons.size() >= MAX_CUSTOM_WATER_CANNONS)
        return nullptr;

    void* pCannon = malloc(SIZE_CWaterCannon);
    memset(pCannon, 0, SIZE_CWaterCannon);
    ((void(__thiscall*)(void*))FUNC_CWaterCannon_Constructor)(pCannon);
    ((void(__thiscall*)(void*))FUNC_CWaterCannon_Init)(pCannon);

    // Point the embedded audio entity at this cannon and initialise it, so its per-frame Service
    // has a valid owner to read from and passes its sound-bank checks; a vehicle-less cannon whose
    // audio was never wired up this way stays silent
    ((void(__thiscall*)(void*, void*))FUNC_CAEWaterCannonAudioEntity_Initialise)(static_cast<char*>(pCannon) + OFFSET_CWaterCannon_Audio, pCannon);

    customWaterCannons.push_back(pCannon);
    return pCannon;
}

void CMultiplayerSA::DestroyCustomWaterCannon(void* pCannon)
{
    if (!pCannon)
        return;

    const auto iter = std::find(customWaterCannons.begin(), customWaterCannons.end(), pCannon);
    if (iter == customWaterCannons.end())
        return;

    customWaterCannons.erase(iter);
    customWaterCannonColors.erase(pCannon);

    ((void(__thiscall*)(void*))FUNC_CWaterCannon_Destructor)(pCannon);
    free(pCannon);
}

void CMultiplayerSA::UpdateCustomWaterCannon(void* pCannon, const CVector& vecStart, const CVector& vecVelocity)
{
    if (!pCannon)
        return;

    ((void(__thiscall*)(void*, const CVector*, const CVector*))FUNC_CWaterCannon_UpdateNewInput)(pCannon, &vecStart, &vecVelocity);
}

void CMultiplayerSA::SetCustomWaterCannonColor(void* pCannon, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha)
{
    if (!pCannon)
        return;

    customWaterCannonColors[pCannon] = {ucRed, ucGreen, ucBlue, ucAlpha};
}

void CMultiplayerSA::ResetCustomWaterCannonColor(void* pCannon)
{
    if (!pCannon)
        return;

    customWaterCannonColors.erase(pCannon);
}

static void UpdateAllCustomWaterCannons()
{
    std::int16_t index = 0;
    for (void* pCannon : customWaterCannons)
    {
        // Service the audio BEFORE the per-frame update, the same order CWaterCannons::Update uses.
        // UpdateOncePerFrame advances m_nSectionsCount onto a freshly cleared section, so servicing
        // after it would see that section unused and restart the jet sound every frame (a machine
        // gun stutter).
        ((void(__thiscall*)(void*))FUNC_CAEWaterCannonAudioEntity_Service)(static_cast<char*>(pCannon) + OFFSET_CWaterCannon_Audio);

        ((void(__thiscall*)(void*, std::int16_t))FUNC_CWaterCannon_UpdateOncePerFrame)(pCannon, index++);
    }
}

static void RenderAllCustomWaterCannons()
{
    for (void* pCannon : customWaterCannons)
    {
        g_pRenderingCustomCannon = pCannon;
        ((void(__thiscall*)(void*))FUNC_CWaterCannon_Render)(pCannon);
    }
    g_pRenderingCustomCannon = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// The single call site for CWaterCannons::Update, so our own custom cannons age, animate and
// extinguish fires alongside the vehicle-owned ones every frame, without touching their array.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x53C086 | E8 35 63 1E 00 | call    0x0072A3C0
//     0x53C08B | E8 10 70 03 00 | call    0x005720A0
static const DWORD FUNC_CWaterCannons_Update = 0x0072A3C0;
#define HOOKPOS_CWaterCannons_UpdateCustomDispatch  0x53C086
#define HOOKSIZE_CWaterCannons_UpdateCustomDispatch 5
static const DWORD CONTINUE_CWaterCannons_UpdateCustomDispatch = 0x53C08B;

static void __declspec(naked) HOOK_CWaterCannons_UpdateCustomDispatch()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // this call sits in a flat chain of independent, argument-less subsystem update calls;
        // none of them read a register left behind by the previous one, so nothing needs saving
        call    UpdateAllCustomWaterCannons

        call    FUNC_CWaterCannons_Update
        jmp     CONTINUE_CWaterCannons_UpdateCustomDispatch
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// The single call site for CWaterCannons::Render, same reasoning as the Update dispatch above.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x53E1A5 | E8 86 D7 1F 00 | call    0x00729B30
//     0x53E1AA | E8 B1 95 12 00 | call    0x006E7760
static const DWORD FUNC_CWaterCannons_Render = 0x00729B30;
#define HOOKPOS_CWaterCannons_RenderCustomDispatch  0x53E1A5
#define HOOKSIZE_CWaterCannons_RenderCustomDispatch 5
static const DWORD CONTINUE_CWaterCannons_RenderCustomDispatch = 0x53E1AA;

static void __declspec(naked) HOOK_CWaterCannons_RenderCustomDispatch()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        call    RenderAllCustomWaterCannons

        call    FUNC_CWaterCannons_Render
        jmp     CONTINUE_CWaterCannons_RenderCustomDispatch
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
// CWaterCannon::Render packs a hardcoded RGBA (R=200 G=200 B=255, alpha fades along the jet)
// into every vertex; override it per instance so setWaterCannonColor works. The block runs for
// every cannon, and eax holds the jet's fade alpha (0..64) on entry. The cannon being rendered
// is taken from g_pRenderingCustomCannon (set around our own Render calls), so a null one (a
// vehicle-owned cannon) or one without a custom colour keeps the native light blue.
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x7290FF | C1 E0 18       | shl eax, 0x18
//     0x729102 | 8B C8          | mov ecx, eax
//     0x729104 | 81 C9 FF C8 C8 00 | or ecx, 0x00C8C8FF   (11 bytes total, rejoin at 0x72910A)
static DWORD __cdecl ComputeCustomWaterCannonColor(int iFadeAlpha)
{
    const DWORD dwFade = static_cast<DWORD>(iFadeAlpha & 0xFF) << 24;

    if (!g_pRenderingCustomCannon)
        return dwFade | 0x00C8C8FF;  // native default (R200 G200 B255)

    const auto iter = customWaterCannonColors.find(g_pRenderingCustomCannon);
    if (iter == customWaterCannonColors.end())
        return dwFade | 0x00C8C8FF;

    // Keep the jet's own fade, scaled by the chosen alpha, and swap in the chosen RGB
    const SWaterCannonColor& color = iter->second;
    const DWORD              dwAlpha = static_cast<DWORD>((iFadeAlpha * color.a / 255) & 0xFF) << 24;
    return dwAlpha | (static_cast<DWORD>(color.r) << 16) | (static_cast<DWORD>(color.g) << 8) | static_cast<DWORD>(color.b);
}

#define HOOKPOS_CWaterCannon_RenderColor  0x7290FF
#define HOOKSIZE_CWaterCannon_RenderColor 11
static const DWORD RETURN_CWaterCannon_RenderColor = 0x72910A;

static void __declspec(naked) HOOK_CWaterCannon_RenderColor()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    edx                 // the lookup call clobbers edx, which is live past the rejoin
        push    eax                 // arg: fade alpha
        call    ComputeCustomWaterCannonColor
        add     esp, 4
        pop     edx
        mov     ecx, eax            // the vertex store loop at the rejoin expects the colour in ecx
        mov     eax, RETURN_CWaterCannon_RenderColor
        jmp     eax
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_WaterCannons
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_WaterCannons()
{
    EZHookInstall(CWaterCannons_UpdateCustomDispatch);
    EZHookInstall(CWaterCannons_RenderCustomDispatch);
    EZHookInstall(CWaterCannon_RenderColor);

    MemSet(reinterpret_cast<void*>(PATCH_CWaterCannon_SoundGateOwnerCheck), 0x90, 2);
}
