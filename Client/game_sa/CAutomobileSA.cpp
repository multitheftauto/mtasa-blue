/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAutomobileSA.cpp
 *  PURPOSE:     Automobile vehicle entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAutomobileSA.h"
#include "CGameSA.h"

extern CGameSA* pGame;

using namespace CarNodes;
using namespace VehicleFeatures;

CAutomobileSA::CAutomobileSA(CAutomobileSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}

void CAutomobileSAInterface::SetPanelDamage(std::uint8_t panelId, bool breakGlass, bool spawnFlyingComponent)
{
    int nodeId = CDamageManagerSA::GetCarNodeIndexFromPanel(panelId);
    if (nodeId < 0)
        return;

    CarNodes::Enum node = static_cast<CarNodes::Enum>(nodeId);

    RwFrame* frame = m_aCarNodes[nodeId];
    if (!frame)
        return;

    CVehicleModelInfoSAInterface* vehicleInfo = nullptr;
    if (auto* mi = pGame->GetModelInfo(m_nModelIndex))
        vehicleInfo = static_cast<CVehicleModelInfoSAInterface*>(mi->GetInterface());

    if (!vehicleInfo || !vehicleInfo->IsComponentDamageable(nodeId))
        return;

    switch (m_damageManager.GetPanelStatus(panelId))
    {
        case DT_PANEL_DAMAGED:
        {
            if ((pHandlingData->uiModelFlags & 0x10000000) != 0) // check bouncePanels flag
                return;

            if (node != WINDSCREEN && node != WING_LF && node != WING_RF)
            {
                // Get free bouncing panel
                for (auto& panel : m_panels)
                {
                    if (panel.m_nFrameId == (std::uint16_t)0xFFFF)
                    {
                        panel.SetPanel(nodeId, 1, GetRandomNumberInRange(-0.2f, -0.5f));
                        break;
                    }
                }
            }

            SetComponentVisibility(frame, 2); // ATOMIC_IS_DAM_STATE
            break;
        }
        case DT_PANEL_OPENED:
        {
            if (panelId == WINDSCREEN_PANEL)
                m_VehicleAudioEntity.AddAudioEvent(91, 0.0f);

            SetComponentVisibility(frame, 2); // ATOMIC_IS_DAM_STATE
            break;
        }
        case DT_PANEL_OPENED_DAMAGED:
        {
            if (panelId == WINDSCREEN_PANEL)
            {
                if (breakGlass)
                    ((void(__cdecl*)(CAutomobileSAInterface*, bool))0x71C2B0)(this, false); // Call CGlass::CarWindscreenShatters
            }

            if (spawnFlyingComponent && (panelId != WINDSCREEN_PANEL || (panelId == WINDSCREEN_PANEL && !breakGlass)))
                SpawnFlyingComponent(node, eCarComponentCollisionTypes::COL_NODE_PANEL);

            SetComponentVisibility(frame, 0); // ATOMIC_IS_NOT_PRESENT
            break;
        }
    }
}

void CAutomobileSA::PreRender_End(CAutomobileSAInterface* vehicleInterface)
{
    auto* vehicle = pGame->GetPools()->GetVehicle(reinterpret_cast<DWORD*>(vehicleInterface));
    if (!vehicle || !vehicle->pEntity)
        return;

    // Support for default vehicles is still in the GTA code, so we don't need to do it again
    if (vehicleInterface->m_nModelIndex == 407 || vehicleInterface->m_nModelIndex == 601)
        return;

    // Simple turret like in fire truck
    if (vehicle->pEntity->IsSpecialFeatureEnabled(WATER_CANNON) && !vehicle->pEntity->IsSpecialFeatureEnabled(TURRET))
    {
        CVehicleSA::SetComponentRotation(vehicleInterface->m_aCarNodes[MISC_A], eComponentRotationAxis::AXIS_X, vehicleInterface->m_fDoomHorizontalRotation, true);
        CVehicleSA::SetComponentRotation(vehicleInterface->m_aCarNodes[MISC_A], eComponentRotationAxis::AXIS_Z, vehicleInterface->m_fDoomVerticalRotation, false);
    }

    // Turret like rhino or swat van
    if (vehicle->pEntity->IsSpecialFeatureEnabled(TURRET))
    {
        CVehicleSA::SetComponentRotation(vehicleInterface->m_aCarNodes[MISC_A], eComponentRotationAxis::AXIS_Z, vehicleInterface->m_fDoomVerticalRotation, true);
        CVehicleSA::SetComponentRotation(vehicleInterface->m_aCarNodes[MISC_B], eComponentRotationAxis::AXIS_X, vehicleInterface->m_fDoomHorizontalRotation, true);
    }
}

bool CAutomobileSA::HasFeatureEnabled(CAutomobileSAInterface* vehicleInterface, VehicleFeatures::Enum feature)
{
    auto* vehicle = pGame->GetPools()->GetVehicle(reinterpret_cast<DWORD*>(vehicleInterface));
    if (!vehicle || !vehicle->pEntity) // This really shouldn't happen
        return true;

    return vehicle->pEntity->IsSpecialFeatureEnabled(feature);
}

static constexpr std::uintptr_t SKIIP_FIRE_TRUCK = 0x6B1F77;
static constexpr std::uintptr_t CONTINUE_FIRE_TRUCK = 0x6B1F5B;
static void _declspec(naked) HOOK_CAutomobile_ProcessControl_FireTruckCheck()
{
    _asm
    {
        push WATER_CANNON
        push esi
        call CAutomobileSA::HasFeatureEnabled
        add esp, 8

        test al, al
        jz skip

        jmp CONTINUE_FIRE_TRUCK

        skip:
        jmp SKIIP_FIRE_TRUCK
    }
}

static constexpr std::uintptr_t FINISH_PRE_RENDER = 0x6ACC92;
static void _declspec(naked) HOOK_CAutomobile_PreRender_End()
{
    _asm
    {
        push esi
        call CAutomobileSA::PreRender_End
        add esp, 4

        lea ecx, [esp+94h]
        jmp FINISH_PRE_RENDER
    }
}

void CAutomobileSA::StaticSetHooks()
{
    HookInstall(0x6B1F4B, (DWORD)HOOK_CAutomobile_ProcessControl_FireTruckCheck, 8); // Model check in CAutomobile::ProcessControl
    HookInstall(0x6ACC8B, (DWORD)HOOK_CAutomobile_PreRender_End, 7); // The end of the CAutomobile::PreRender function
}
