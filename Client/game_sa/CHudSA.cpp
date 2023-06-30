/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CHudSA.cpp
 *  PURPOSE:     HUD display
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CHudSA.h"

#define FUNC_DrawRadar      0x58A330
#define FUNC_DrawRadarPlanB 0x58A335

char szVehicleName[50] = {'\0'};
char szZoneName[50] = {'\0'};

CHudSA::CHudSA()
{
    InitComponentList();

    // Set the default values
    m_fSniperCrosshairScale = 210.0f;

    m_pfCameraCrosshairScale = (float*)0x866C74;
    MemPut<float>(m_pfCameraCrosshairScale, 192.0f);
    m_pfAspectRatioMultiplicator = (float*)0x859524;
    MemPut<float>(m_pfAspectRatioMultiplicator, 0.002232143f);

    // Patch xrefs to 0x863B34, because this variable seems to be shared (2 other functions without any context access to it; probably a compiler optimization)
    MemPut<DWORD>(0x58E7D4 + 2, (DWORD)&m_fSniperCrosshairScale);
    MemPut<DWORD>(0x58E7EA + 2, (DWORD)&m_fSniperCrosshairScale);
    MemPut<DWORD>(0x53E3ED + 2, (DWORD)&m_fSniperCrosshairScale);
    MemPut<DWORD>(0x53E41A + 2, (DWORD)&m_fSniperCrosshairScale);
    MemPut<DWORD>(0x53E488 + 2, (DWORD)&m_fSniperCrosshairScale);
    MemPut<DWORD>(0x53E4BF + 2, (DWORD)&m_fSniperCrosshairScale);
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
        {1, HUD_AMMO,         1, 0x5893B0,       1, 0xCC,   0xC3},
        {1, HUD_WEAPON,       1, 0x58D7D0,       1, 0xCC,   0xC3},
        {1, HUD_HEALTH,       1, 0x589270,       1, 0xCC,   0xC3},
        {1, HUD_BREATH,       1, 0x589190,       1, 0xCC,   0xC3},
        {1, HUD_ARMOUR,       1, 0x5890A0,       1, 0xCC,   0xC3},
        {1, HUD_MONEY,        1, 0x58F47D,       2, 0xCCCC, 0xE990},
        {1, HUD_VEHICLE_NAME, 1, 0x58AEA0,       1, 0xCC,   0xC3},
        {1, HUD_AREA_NAME,    1, 0x58AA50,       1, 0xCC,   0xC3},
        {1, HUD_RADAR,        1, FUNC_DrawRadar, 1, 0xCC,   0xC3},
        {1, HUD_CLOCK,        0, 0xBAA400,       1, 1,      0},
        {1, HUD_RADIO,        1, 0x4E9E50,       1, 0xCC,   0xC3},
        {1, HUD_WANTED,       1, 0x58D9A0,       1, 0xCC,   0xC3},
        {1, HUD_CROSSHAIR,    1, 0x58E020,       1, 0xCC,   0xC3},
        {1, HUD_VITAL_STATS,  1, 0x589650,       1, 0xCC,   0xC3},
        {0, HUD_HELP_TEXT,    1, 0x58B6E0,       1, 0xCC,   0xC3},
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

//
// CHudSA::AdjustComponents
//
void CHudSA::AdjustComponents(float fAspectRatio)
{
    // Fix for #7400 (HUD elements do not scale correctly for widescreen)
    // 0x859524: GTA multiplies all HUD and menu transformation variables by this floating point value. It is equal to 1/448, so just translate it to 16/10 /
    // 16/9
    MemPut<float>(m_pfAspectRatioMultiplicator, 0.002232143f / (4.0f / 3.0f) * fAspectRatio);

    // Set the sniper crosshair scale (fix for #7659)
    m_fSniperCrosshairScale = 210.0f * (4.0f / 3.0f) / fAspectRatio;

    // Set the camera crosshair scale (same display flaw as in #7659)
    MemPut<float>(m_pfCameraCrosshairScale, 192.0f * (4.0f / 3.0f) / fAspectRatio);
}

//
// CHudSA::ResetComponentAdjustment
//
void CHudSA::ResetComponentAdjustment()
{
    // Restore default values (4:3 aspect ratio)
    MemPut<float>(m_pfAspectRatioMultiplicator, 0.002232143f);
    MemPut<float>(m_pfCameraCrosshairScale, 192.0f);
    m_fSniperCrosshairScale = 210.0f;
}
