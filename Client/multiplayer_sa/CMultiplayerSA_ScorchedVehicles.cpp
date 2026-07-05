/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_ScorchedVehicles.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

namespace
{
    constexpr float SCORCHED_VEHICLE_LIGHTING_PS2 = 0.0f;

    bool ms_bPs2ScorchedVehiclesEnabled = false;

    using WorldReplaceNormalLightsWithScorched_t = void(__cdecl*)(RpWorld* world, float lighting);

    void __cdecl WorldReplaceNormalLightsWithScorched_MTA(RpWorld* world, float lighting)
    {
        if (ms_bPs2ScorchedVehiclesEnabled)
            lighting = SCORCHED_VEHICLE_LIGHTING_PS2;

        reinterpret_cast<WorldReplaceNormalLightsWithScorched_t>(0x7357E0)(world, lighting);
    }
}  // namespace

bool CMultiplayerSA::IsPs2ScorchedVehiclesEnabled() const noexcept
{
    return ms_bPs2ScorchedVehiclesEnabled;
}

void CMultiplayerSA::SetPs2ScorchedVehiclesEnabled(bool enabled)
{
    ms_bPs2ScorchedVehiclesEnabled = enabled;
}

void CMultiplayerSA::InitHooks_ScorchedVehicles()
{
    HookInstallCall(0x553E6A, reinterpret_cast<DWORD>(WorldReplaceNormalLightsWithScorched_MTA));  // SetupLightingForEntity
    HookInstallCall(0x554FC2, reinterpret_cast<DWORD>(WorldReplaceNormalLightsWithScorched_MTA));  // CObject::SetupLighting
}
