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

// Not a native constraint, since these live entirely outside the vehicles' own fixed-size array;
// just a sanity ceiling so a runaway script can't grow this list without bound. Update_OncePerFrame
// runs a world-wide ped scan for every entry every few frames, so this is also a soft cost limit.
static constexpr size_t MAX_CUSTOM_WATER_CANNONS = 256;

static std::vector<void*> customWaterCannons;

void* CMultiplayerSA::CreateCustomWaterCannon()
{
    if (customWaterCannons.size() >= MAX_CUSTOM_WATER_CANNONS)
        return nullptr;

    void* pCannon = malloc(SIZE_CWaterCannon);
    memset(pCannon, 0, SIZE_CWaterCannon);
    ((void(__thiscall*)(void*))FUNC_CWaterCannon_Constructor)(pCannon);
    ((void(__thiscall*)(void*))FUNC_CWaterCannon_Init)(pCannon);

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

    ((void(__thiscall*)(void*))FUNC_CWaterCannon_Destructor)(pCannon);
    free(pCannon);
}

void CMultiplayerSA::UpdateCustomWaterCannon(void* pCannon, const CVector& vecStart, const CVector& vecVelocity)
{
    if (!pCannon)
        return;

    ((void(__thiscall*)(void*, const CVector*, const CVector*))FUNC_CWaterCannon_UpdateNewInput)(pCannon, &vecStart, &vecVelocity);
}

static void UpdateAllCustomWaterCannons()
{
    std::int16_t index = 0;
    for (void* pCannon : customWaterCannons)
        ((void(__thiscall*)(void*, std::int16_t))FUNC_CWaterCannon_UpdateOncePerFrame)(pCannon, index++);
}

static void RenderAllCustomWaterCannons()
{
    for (void* pCannon : customWaterCannons)
        ((void(__thiscall*)(void*))FUNC_CWaterCannon_Render)(pCannon);
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
//
// CMultiplayerSA::InitHooks_WaterCannons
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_WaterCannons()
{
    EZHookInstall(CWaterCannons_UpdateCustomDispatch);
    EZHookInstall(CWaterCannons_RenderCustomDispatch);
}
