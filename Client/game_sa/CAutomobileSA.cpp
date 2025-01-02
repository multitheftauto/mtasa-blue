/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAutomobileSA.cpp
 *  PURPOSE:     Automobile vehicle entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAutomobileSA.h"
#include "CGameSA.h"

extern CGameSA* pGame;

CAutomobileSA::CAutomobileSA(CAutomobileSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}

void CAutomobileSAInterface::SetPanelDamage(std::uint8_t panelId, bool breakGlass, bool spawnFlyingComponent)
{
    int nodeId = CDamageManagerSA::GetCarNodeIndexFromPanel(panelId);
    eCarNodes node = static_cast<eCarNodes>(nodeId);

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

            if (node != eCarNodes::WINDSCREEN && node != eCarNodes::WING_LF && node != eCarNodes::WING_RF)
            {
                // Get free bouncing panel
                CBouncingPanelSAInterface* panelInterface = nullptr;
                for (auto& panel : m_panels)
                {
                    if (panel.m_nFrameId == static_cast<std::uint16_t>(-1))
                    {
                        panelInterface = &panel;
                        break;
                    }
                }

                panelInterface->SetPanel(nodeId, 1, GetRandomNumberInRange(-0.2f, -0.5f));
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
                    ((void(__cdecl*)(CAutomobileSAInterface*))0x71C2B0)(this); // Call CGlass::CarWindscreenShatters
            }

            if (spawnFlyingComponent && (panelId != WINDSCREEN_PANEL || (panelId == WINDSCREEN_PANEL && !breakGlass)))
                SpawnFlyingComponent(node, eCarComponentCollisionTypes::COL_NODE_PANEL);

            SetComponentVisibility(frame, 0); // ATOMIC_IS_NOT_PRESENT
            break;
        }
    }
}
