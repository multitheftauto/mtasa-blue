/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHudSA.cpp
 *  PURPOSE:     HUD display
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CHudSA.h"
#include "CGameSA.h"
#include "CCameraSA.h"
#include "CPlayerInfoSA.h"
#include "TaskAttackSA.h"

extern CGameSA* pGame;

char szVehicleName[50] = {'\0'};
char szZoneName[50] = {'\0'};

CHudSA::CHudSA()
{
    InitComponentList();

    // Set the default values
    m_fSniperCrosshairScale = 210.0f;

    m_pfCameraCrosshairScale = (float*)VAR_CameraCrosshairScale;
    MemPut<float>(m_pfCameraCrosshairScale, 192.0f);
    m_pfAspectRatioMultiplicator = (float*)VAR_AspectRatioMult;
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

bool CHudSA::IsCrosshairVisible()
{
    bool specialAiming = false;
    bool simpleAiming = false;

    // Get camera view mode
    CCamera* camera = pGame->GetCamera();
    eCamMode cameraViewMode = static_cast<eCamMode>(camera->GetCam(camera->GetActiveCam())->GetMode());

    // Get player
    CPed* playerPed = pGame->GetPedContext();
    CWeapon* weapon = nullptr;
    eWeaponType weaponType;

    // Get player current weapon
    if (playerPed)
    {
        weapon = playerPed->GetWeapon(playerPed->GetCurrentWeaponSlot());
        if (weapon)
            weaponType = weapon->GetType();
    }

    // Special aiming
    if (cameraViewMode == MODE_SNIPER || cameraViewMode == MODE_1STPERSON || cameraViewMode == MODE_ROCKETLAUNCHER || cameraViewMode == MODE_ROCKETLAUNCHER_HS || cameraViewMode == MODE_M16_1STPERSON || cameraViewMode == MODE_HELICANNON_1STPERSON || cameraViewMode == MODE_CAMERA)
    {
        if (weapon && cameraViewMode != MODE_1STPERSON && pGame->GetWeaponInfo(weaponType, WEAPONSKILL_STD)->GetFireType() != FIRETYPE_MELEE)
            specialAiming = true;
    }

    // Simple aiming
    if (cameraViewMode == MODE_M16_1STPERSON_RUNABOUT || cameraViewMode == MODE_ROCKETLAUNCHER_RUNABOUT || cameraViewMode == MODE_ROCKETLAUNCHER_RUNABOUT_HS || cameraViewMode == MODE_SNIPER_RUNABOUT)
        simpleAiming = true;

    if ((playerPed && weapon) && !playerPed->GetTargetedObject() && playerPed->GetPedInterface()->pPlayerData->m_bFreeAiming)
    {
        CTaskSimpleUseGun* taskUseGun = playerPed->GetPedIntelligence()->GetTaskUseGun();
        if ((!taskUseGun || !taskUseGun->GetSkipAim()) && (cameraViewMode == MODE_AIMWEAPON || cameraViewMode == MODE_AIMWEAPON_FROMCAR || cameraViewMode == MODE_AIMWEAPON_ATTACHED))
        {
            if (playerPed->GetPedState() != PED_ENTER_CAR && playerPed->GetPedState() != PED_CARJACK)
            {
                if ((weaponType >= WEAPONTYPE_PISTOL && weaponType <= WEAPONTYPE_M4) || weaponType == WEAPONTYPE_TEC9 || weaponType == WEAPONTYPE_COUNTRYRIFLE || weaponType == WEAPONTYPE_MINIGUN || weaponType == WEAPONTYPE_FLAMETHROWER)
                    simpleAiming = cameraViewMode != MODE_AIMWEAPON || camera->GetTransitionState() == 0;
            }
        }
    }

    // Check CTheScripts::bDrawCrossHair
    std::uint8_t crossHairType = *reinterpret_cast<std::uint8_t*>(VAR_CTheScripts_bDrawCrossHair);
    return specialAiming || simpleAiming || crossHairType > 0;
}
