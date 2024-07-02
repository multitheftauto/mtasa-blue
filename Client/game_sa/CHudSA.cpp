/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHudSA.cpp
 *  PURPOSE:     HUD display
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CHudSA.h"

CHudSA::CHudSA()
{
    InitComponentList();
}

void CHudSA::Disable(bool bDisabled)
{
    if (bDisabled)
        MemPut<BYTE>(FUNC_Draw, 0xC3);
    else
        MemPut<BYTE>(FUNC_Draw, 0x80);

    // Also disable the radar as the above code will not hide it before the local player has spawned
    if (bDisabled)
        MemPut<BYTE>(FUNC_DrawRadarPlanB, 0xC3);
    else
        MemPut<BYTE>(FUNC_DrawRadarPlanB, 0x83);
}

bool CHudSA::IsDisabled()
{
    return *(BYTE*)FUNC_Draw == 0xC3;
}

//
// CHudSA::InitComponentList
//
void CHudSA::InitComponentList()
{
    SHudComponent componentList[] = {
        {1, HUD_AMMO, 1, FUNC_DrawAmmo, 1, 0xCC, 0xC3},
        {1, HUD_WEAPON, 1, FUNC_DrawWeaponIcon, 1, 0xCC, 0xC3},
        {1, HUD_HEALTH, 1, FUNC_PrintHealthForPlayer, 1, 0xCC, 0xC3},
        {1, HUD_BREATH, 1, FUNC_PrintBreathForPlayer, 1, 0xCC, 0xC3},
        {1, HUD_ARMOUR, 1, FUNC_PrintArmourForPlayer, 1, 0xCC, 0xC3},
        {1, HUD_MONEY, 1, CODE_ShowMoney, 2, 0xCCCC, 0xE990},
        {1, HUD_VEHICLE_NAME, 1, FUNC_DrawVehicleName, 1, 0xCC, 0xC3},
        {1, HUD_AREA_NAME, 1, FUNC_DrawAreaName, 1, 0xCC, 0xC3},
        {1, HUD_RADAR, 1, FUNC_DrawRadar, 1, 0xCC, 0xC3},
        {1, HUD_CLOCK, 0, VAR_DisableClock, 1, 1, 0},
        {1, HUD_RADIO, 1, FUNC_DrawRadioName, 1, 0xCC, 0xC3},
        {1, HUD_WANTED, 1, FUNC_DrawWantedLevel, 1, 0xCC, 0xC3},
        {1, HUD_CROSSHAIR, 1, FUNC_DrawCrosshair, 1, 0xCC, 0xC3},
        {1, HUD_VITAL_STATS, 1, FUNC_DrawVitalStats, 1, 0xCC, 0xC3},
        {0, HUD_HELP_TEXT, 1, FUNC_DrawHelpText, 1, 0xCC, 0xC3},
    };

    for (uint i = 0; i < NUMELMS(componentList); i++)
    {
        const SHudComponent& component = componentList[i];
        MapSet(m_HudComponentMap, component.type, component);
    }
}

//
// CHudSA::SetComponentVisible
//
void CHudSA::SetComponentVisible(eHudComponent component, bool bVisible)
{
    // Handle ALL option
    if (component == HUD_ALL)
    {
        for (std::map<eHudComponent, SHudComponent>::iterator iter = m_HudComponentMap.begin(); iter != m_HudComponentMap.end(); ++iter)
        {
            const SHudComponent& component = iter->second;
            if (component.bIsPartOfAll)
                SetComponentVisible(component.type, bVisible);
        }
        return;
    }

    // Set visiblity of one component
    SHudComponent* pComponent = MapFind(m_HudComponentMap, component);
    if (pComponent)
    {
        // Save original bytes if requred
        if (pComponent->bSaveOriginalBytes)
        {
            pComponent->origData = *(DWORD*)pComponent->uiDataAddr;
            pComponent->bSaveOriginalBytes = false;
        }

        // Poke bytes
        uchar* pSrc = (uchar*)(bVisible ? &pComponent->origData : &pComponent->disabledData);
        uchar* pDest = (uchar*)(pComponent->uiDataAddr);
        for (uint i = 0; i < pComponent->uiDataSize; i++)
        {
            if (pComponent->type != HUD_CLOCK)
                MemPut<BYTE>(pDest + i, pSrc[i]);
            else
                MemPutFast<BYTE>(pDest + i, pSrc[i]);
        }
    }
}

//
// CHudSA::IsComponentVisible
//
bool CHudSA::IsComponentVisible(eHudComponent component)
{
    SHudComponent* pComponent = MapFind(m_HudComponentMap, component);
    if (pComponent)
    {
        // Determine if invisible by matching data with disabled values
        uchar* pSrc = (uchar*)(&pComponent->disabledData);
        uchar* pDest = (uchar*)(pComponent->uiDataAddr);
        if (memcmp(pDest, pSrc, pComponent->uiDataSize) == 0)
            return false;            // Matches disabled bytes
        return true;
    }
    return false;
}
