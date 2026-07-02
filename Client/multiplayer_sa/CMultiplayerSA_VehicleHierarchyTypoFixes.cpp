/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_VehicleHierarchyTypoFixes.cpp
 *  PURPOSE:     Optional CVehicleModelInfo hierarchy compare hook (matches SilentPatch SA)
 *
 *  GTA matches IDE/handling component names to frame names via _stricmp inside
 *  CVehicleModelInfo setup. Stock models use inconsistent spellings; SilentPatch
 *  wraps that call to accept both canonical and typo names. We expose the same
 *  behaviour only when enabled so servers can keep fully vanilla visuals.
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CMultiplayerSA.h"

#include <utility>

namespace
{
    // Off by default (matches stock GTA SA without SilentPatch).
    bool g_bVehicleHierarchyTypoFixesEnabled = false;

    int(__cdecl* g_pfnOrgStricmp)(const char*, const char*) = nullptr;

    // Pairs are { correct name, name as in shipping .dff }. Must be sorted by the second string.
    static constexpr std::pair<const char*, const char*> typosAndFixes[] = {
        {"boat_moving_hi", "boat_moving"},
        {"elevator_r", "elevator"},
        {"misc_a", "misca"},
        {"misc_b", "miscb"},
        {"taillights", "tailights"},
        {"taillights2", "tailights2"},
        {"transmission_f", "transmision_f"},
        {"transmission_r", "transmision_r"},
        {"wheel_lm_dummy", "wheel_lm"},
    };

    int __cdecl Hooked_VehicleHierarchy_Stricmp(const char* dataName, const char* nodeName)
    {
        if (!g_bVehicleHierarchyTypoFixesEnabled)
            return g_pfnOrgStricmp(dataName, nodeName);

        const int origComp = g_pfnOrgStricmp(dataName, nodeName);
        if (origComp == 0)
            return 0;

        for (const auto& typo : typosAndFixes)
        {
            const int nodeComp = _stricmp(typo.second, nodeName);
            if (nodeComp > 0)
                break;

            if (nodeComp == 0 && _stricmp(typo.first, dataName) == 0)
                return 0;
        }

        return origComp;
    }
}  // namespace

void InitVehicleHierarchyTypoFixesHook()
{
    // SA 1.0 US: indirect call to _stricmp while CVehicleModelInfo binds handling/IDE component names to clump frames.
    // Same site hooked by SilentPatch (`InterceptCall(0x4C5311, ...)`).
    constexpr DWORD CALLSITE = 0x4C5311;
    HookCheckOriginalByte(CALLSITE, 0xE8);
    const DWORD rel = *reinterpret_cast<DWORD*>(CALLSITE + 1);
    g_pfnOrgStricmp = reinterpret_cast<decltype(g_pfnOrgStricmp)>(CALLSITE + 5 + rel);
    HookInstallCall(CALLSITE, reinterpret_cast<DWORD>(&Hooked_VehicleHierarchy_Stricmp));
}

void CMultiplayerSA::SetVehicleHierarchyTypoFixesEnabled(bool bEnabled)
{
    g_bVehicleHierarchyTypoFixesEnabled = bEnabled;
}

bool CMultiplayerSA::GetVehicleHierarchyTypoFixesEnabled() const
{
    return g_bVehicleHierarchyTypoFixesEnabled;
}
