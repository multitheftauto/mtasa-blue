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

static ComponentProperties componentProperties;

RsGlobal* CHudSA::rsGlobal = reinterpret_cast<RsGlobal*>(VAR_RSGlobal);
std::int16_t* CHudSA::itemToFlash = reinterpret_cast<std::int16_t*>(VAR_ItemToFlash);

float CHudSA::calcStreetchX = 0.0f;
float CHudSA::calcStreetchY = 0.0f;

constexpr RwColor COLOR_BLACK = RwColor{0, 0, 0, 0};

CHudSA::CHudSA()
{
    InitComponentList();

    // Set the default values
    m_fSniperCrosshairScale = 210.0f;

    m_pfCameraCrosshairScale = (float*)VAR_CameraCrosshairScale;
    MemPut<float>(m_pfCameraCrosshairScale, 192.0f);
    m_pfAspectRatioMultiplicator = (float*)VAR_AspectRatioMult;
    MemPut<float>(m_pfAspectRatioMultiplicator, 0.002232143f);

    UpdateStreetchCalculations();

    // Patch xrefs to 0x863B34, because this variable seems to be shared (2 other functions without any context access to it; probably a compiler optimization)
    MemPut<DWORD>(0x58E7D4 + 2, (DWORD)&m_fSniperCrosshairScale);
    MemPut<DWORD>(0x58E7EA + 2, (DWORD)&m_fSniperCrosshairScale);
    MemPut<DWORD>(0x53E3ED + 2, (DWORD)&m_fSniperCrosshairScale);
    MemPut<DWORD>(0x53E41A + 2, (DWORD)&m_fSniperCrosshairScale);
    MemPut<DWORD>(0x53E488 + 2, (DWORD)&m_fSniperCrosshairScale);
    MemPut<DWORD>(0x53E4BF + 2, (DWORD)&m_fSniperCrosshairScale);

    // Initalize default colors
    componentProperties.hpBar.bar.fillColor = CHudSA::GetHUDColour(eHudColour::RED);
    componentProperties.breathBar.bar.fillColor = CHudSA::GetHUDColour(eHudColour::LIGHT_BLUE);
    componentProperties.armorBar.bar.fillColor = CHudSA::GetHUDColour(eHudColour::LIGHT_GRAY);
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
        {1, HUD_HEALTH, 1, FUNC_RenderHealthBar, 1, 0xCC, 0xC3},
        {1, HUD_BREATH, 1, FUNC_RenderBreathBar, 1, 0xCC, 0xC3},
        {1, HUD_ARMOUR, 1, FUNC_RenderArmorBar, 1, 0xCC, 0xC3},
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

void CHudSA::UpdateStreetchCalculations()
{
    calcStreetchX = rsGlobal->maximumWidth * (*reinterpret_cast<float*>(VAR_AspectRatioMultX));
    calcStreetchY = rsGlobal->maximumHeight * (*m_pfAspectRatioMultiplicator);

    ComponentPlacement& hpPlacement = componentProperties.hpBar.bar.placement;
    hpPlacement.height = calcStreetchY * 9.0f;
    hpPlacement.width = calcStreetchX * 109.0f;
    hpPlacement.setDefaultXY = false;

    ComponentPlacement& breathPlacement = componentProperties.breathBar.bar.placement;
    breathPlacement.height = calcStreetchY * 9.0f;
    breathPlacement.width = calcStreetchX * 62.0f;
    breathPlacement.setDefaultXY = false;

    ComponentPlacement& armorPlacement = componentProperties.armorBar.bar.placement;
    armorPlacement.height = calcStreetchY * 9.0f;
    armorPlacement.width = calcStreetchX * 62.0f;
    armorPlacement.setDefaultXY = false;
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

    UpdateStreetchCalculations();
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

    UpdateStreetchCalculations();
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

RwColor CHudSA::GetHUDColour(const eHudColour& colour)
{
    switch (colour)
    {
        case eHudColour::RED:
            return {180, 25, 29, 255};
        case eHudColour::GREEN:
            return {54, 104, 44, 255};
        case eHudColour::DARK_BLUE:
            return {50, 60, 127, 255};
        case eHudColour::LIGHT_BLUE:
            return {172, 203, 241, 255};
        case eHudColour::LIGHT_GRAY:
            return {225, 225, 225, 255};
        case eHudColour::BLACK:
            return {0, 0, 0, 255};
        case eHudColour::GOLD:
            return {144, 98, 16, 255};
        case eHudColour::PURPLE:
            return {168, 110, 252, 255};
        case eHudColour::DARK_GRAY:
            return {150, 150, 150, 255};
        case eHudColour::DARK_RED:
            return {104, 15, 17, 255};
        case eHudColour::DARK_GREEN:
            return {38, 71, 31, 255};
        case eHudColour::CREAM:
            return {226, 192, 99, 255};
        case eHudColour::NIGHT_BLUE:
            return {74, 90, 107, 255};
        case eHudColour::BLUE:
            return {20, 25, 200, 255};
        case eHudColour::YELLOW:
            return {255, 255, 0, 255};
        default:
            return {0, 0, 0, 255};
    }
}

HudBar& CHudSA::GetHudBarRef(const eHudComponent& component) noexcept
{
    switch (component)
    {
        case HUD_HEALTH:
            return componentProperties.hpBar.bar;
        case HUD_BREATH:
            return componentProperties.breathBar.bar;
        case HUD_ARMOUR:
            return componentProperties.armorBar.bar;
    }
}

void CHudSA::SetComponentPlacementPosition(ComponentPlacement& placement, const CVector2D& position)
{
    placement.customX = position.fX;
    placement.customY = position.fY;
    placement.useCustomPosition = true;
}

void CHudSA::SetComponentPlacementSize(ComponentPlacement& placement, const CVector2D& size)
{
    placement.customWidth = size.fX;
    placement.customHeight = size.fY;
    placement.useCustomSize = true;
}

void CHudSA::SetComponentPosition(const eHudComponent& component, const CVector2D& position) noexcept
{ 
    switch (component)
    {
        case HUD_BREATH:
            SetComponentPlacementPosition(componentProperties.breathBar.bar.placement, position);
            break;
        case HUD_HEALTH:
            SetComponentPlacementPosition(componentProperties.hpBar.bar.placement, position);
            break;
        case HUD_ARMOUR:
            SetComponentPlacementPosition(componentProperties.armorBar.bar.placement, position);
            break;
    }
}

void CHudSA::SetComponentSize(const eHudComponent& component, const CVector2D& size) noexcept
{
    switch (component)
    {
        case HUD_BREATH:
            SetComponentPlacementSize(componentProperties.breathBar.bar.placement, size);
            break;
        case HUD_HEALTH:
            SetComponentPlacementSize(componentProperties.hpBar.bar.placement, size);
            break;
        case HUD_ARMOUR:
            SetComponentPlacementSize(componentProperties.armorBar.bar.placement, size);
            break;
    }
}

void CHudSA::ResetComponent(ComponentPlacement& placement, bool resetSize) noexcept
{
    if (resetSize)
    {
        placement.useCustomSize = false;
        placement.customHeight = 0.0f;
        placement.customWidth = 0.0f;
    }
    else
    {
        placement.useCustomPosition = false;
        placement.customX = 0.0f;
        placement.customY = 0.0f;
    }
}

void CHudSA::ResetComponentPlacement(const eHudComponent& component, bool resetSize) noexcept
{
    switch (component)
    {
        case HUD_ALL:
        {
            for (const auto& cmp : m_HudComponentMap)
            {
                if (cmp.first == HUD_ALL)
                    continue;

                ResetComponentPlacement(cmp.first, resetSize);
            }

            break;
        }
        case HUD_HEALTH:
            ResetComponent(componentProperties.hpBar.bar.placement, resetSize);
            break;
        case HUD_BREATH:
            ResetComponent(componentProperties.breathBar.bar.placement, resetSize);
            break;
        case HUD_ARMOUR:
            ResetComponent(componentProperties.armorBar.bar.placement, resetSize);
            break;
    }
}

void CHudSA::SetComponentBarColor(const eHudComponent& component, float color) noexcept
{ 
    SColor newColor = TOCOLOR2SCOLOR(static_cast<std::uint32_t>(color));
    GetHudBarRef(component).fillColor = RwColor{newColor.R, newColor.G, newColor.B, newColor.A};
}

void CHudSA::SetComponentDrawBlackBorder(const eHudComponent& component, bool draw) noexcept
{
    GetHudBarRef(component).drawBlackBorder = draw;
}

void CHudSA::SetComponentDrawPercentage(const eHudComponent& component, bool draw) noexcept
{
    GetHudBarRef(component).drawPercentage = draw;
}

void CHudSA::SetHealthBarBlinkingValue(float minHealth) noexcept
{
    componentProperties.hpBar.blinkingBarHP = minHealth;
}

void CHudSA::RenderHealthBar(int x, int y)
{
    // Flash each 8 frames
    bool isValidFrame = (pGame->GetSystemFrameCounter() & 8) == 0;
    if (*itemToFlash == 4 && isValidFrame) // 4 = HEALTH_BAR
        return;

    CPed* playerPed = pGame->GetPedContext();
    if (!playerPed || (playerPed->GetHealth() <= componentProperties.hpBar.blinkingBarHP && isValidFrame))
        return;

    // Save default position once
    if (!componentProperties.hpBar.bar.placement.setDefaultXY)
    {
        componentProperties.hpBar.bar.placement.x = x;
        componentProperties.hpBar.bar.placement.y = y;
        componentProperties.hpBar.bar.placement.setDefaultXY = true;
    }

    // Get player max health
    float maxHealth = static_cast<float>(pGame->GetPlayerInfo()->GetMaxHealth());

    // Use custom position/size?
    bool useCustomPosition = componentProperties.hpBar.bar.placement.useCustomPosition;
    bool useCustomSize = componentProperties.hpBar.bar.placement.useCustomSize;

    // Calc bar width depending on MAX_HEALTH stat
    double statModifier = ((double(__cdecl*)(int))FUNC_CStats_GetFatAndMuscleModifier)(10);
    float  totalWidth = ((useCustomSize ? componentProperties.hpBar.bar.placement.customWidth : componentProperties.hpBar.bar.placement.width) * maxHealth) / statModifier;

    // call CSprite2d::DrawBarChart
    ((void(__cdecl*)(float, float, std::uint16_t, std::uint32_t, float, bool, bool, bool, RwColor, RwColor))FUNC_CSprite2d_DrawBarChart)(useCustomPosition ? componentProperties.hpBar.bar.placement.customX : (useCustomSize ? componentProperties.hpBar.bar.placement.customWidth : componentProperties.hpBar.bar.placement.width) - totalWidth + x, useCustomPosition ? componentProperties.hpBar.bar.placement.customY : y, static_cast<std::uint16_t>(totalWidth), static_cast<std::uint32_t>(useCustomSize ? componentProperties.hpBar.bar.placement.customHeight : componentProperties.hpBar.bar.placement.height), playerPed->GetHealth() * 100.0f / maxHealth, false, componentProperties.hpBar.bar.drawPercentage, componentProperties.hpBar.bar.drawBlackBorder, componentProperties.hpBar.bar.fillColor, COLOR_BLACK);
}

void CHudSA::RenderBreathBar(int x, int y)
{
    // Flash each 8 frames
    if (*itemToFlash == 10 && (pGame->GetSystemFrameCounter() & 8) == 0) // 10 = BREATH_BAR
        return;

    CPed* playerPed = pGame->GetPedContext();
    if (!playerPed)
        return;

    // Save default position once
    if (!componentProperties.breathBar.bar.placement.setDefaultXY)
    {
        componentProperties.breathBar.bar.placement.x = x;
        componentProperties.breathBar.bar.placement.y = y;
        componentProperties.breathBar.bar.placement.setDefaultXY = true;
    }

    // Calc bar width depending on AIR_IN_LUNG stat
    double statModifier = ((double(__cdecl*)(int))FUNC_CStats_GetFatAndMuscleModifier)(8);

    // Use custom position/size?
    bool useCustomPosition = componentProperties.breathBar.bar.placement.useCustomPosition;
    bool useCustomSize = componentProperties.breathBar.bar.placement.useCustomSize;

    // call CSprite2d::DrawBarChart
    ((void(__cdecl*)(float, float, std::uint16_t, std::uint32_t, float, bool, bool, bool, RwColor, RwColor))FUNC_CSprite2d_DrawBarChart)(useCustomPosition ? componentProperties.breathBar.bar.placement.customX : x, useCustomPosition ? componentProperties.breathBar.bar.placement.customY : y, static_cast<std::uint16_t>(useCustomSize ? componentProperties.breathBar.bar.placement.customWidth : componentProperties.breathBar.bar.placement.width), static_cast<std::uint32_t>(useCustomSize ? componentProperties.breathBar.bar.placement.customHeight : componentProperties.breathBar.bar.placement.height), playerPed->GetOxygenLevel() / statModifier * 100.0f, false, componentProperties.breathBar.bar.drawPercentage, componentProperties.breathBar.bar.drawBlackBorder, componentProperties.breathBar.bar.fillColor, COLOR_BLACK);
}

void CHudSA::RenderArmorBar(int x, int y)
{
    // Flash each 8 frames
    if (*itemToFlash == 3 && (pGame->GetSystemFrameCounter() & 8) == 0) // 3 = ARMOR_BAR
        return;

    CPed* playerPed = pGame->GetPedContext();
    if (!playerPed || playerPed->GetArmor() < 1.0f)
        return;

    // Save default position once
    if (!componentProperties.armorBar.bar.placement.setDefaultXY)
    {
        componentProperties.armorBar.bar.placement.x = x;
        componentProperties.armorBar.bar.placement.y = y;
        componentProperties.armorBar.bar.placement.setDefaultXY = true;
    }

    // Use custom position/size?
    bool useCustomPosition = componentProperties.hpBar.bar.placement.useCustomPosition;
    bool useCustomSize = componentProperties.hpBar.bar.placement.useCustomSize;

    // call CSprite2d::DrawBarChart
    ((void(__cdecl*)(float, float, std::uint16_t, std::uint32_t, float, bool, bool, bool, RwColor, RwColor))FUNC_CSprite2d_DrawBarChart)(useCustomPosition ? componentProperties.armorBar.bar.placement.customX : x, useCustomPosition ? componentProperties.armorBar.bar.placement.customY : y, static_cast<std::uint16_t>(useCustomSize ? componentProperties.armorBar.bar.placement.customWidth : componentProperties.armorBar.bar.placement.width), static_cast<std::uint32_t>(useCustomSize ? componentProperties.armorBar.bar.placement.customHeight : componentProperties.armorBar.bar.placement.height), playerPed->GetArmor() / static_cast<float>(pGame->GetPlayerInfo()->GetMaxArmor()) * 100.0f, false, componentProperties.armorBar.bar.drawPercentage, componentProperties.armorBar.bar.drawBlackBorder, componentProperties.armorBar.bar.fillColor, COLOR_BLACK);
}

static void _declspec(naked) HOOK_RenderHudBar()
{
    _asm
    {
        mov eax, [esp]

        push [esp+0Ch] // y
        push [esp+0Ch] // x

        // Health bar
        cmp eax, 0058EE9Fh
        jz renderHealthBar

        cmp eax, 0058EF12h
        jz renderHealthBar

        // Breath bar
        cmp eax, 0058F136h
        jz renderBreathBar

        cmp eax, 0058F1B2h
        jz renderBreathBar

        // Armor bar
        cmp eax, 0058EF70h
        jz renderArmorBar

        cmp eax, 0058EFE3h
        jz renderArmorBar

        jmp skip

        renderHealthBar:
        call CHudSA::RenderHealthBar
        jmp skip

        renderBreathBar:
        call CHudSA::RenderBreathBar
        jmp skip

        renderArmorBar:
        call CHudSA::RenderArmorBar
        jmp skip

        skip:
        add esp, 8
        retn
    }
}

void CHudSA::StaticSetHooks()
{
    HookInstall(FUNC_RenderHealthBar, &HOOK_RenderHudBar, 11);
    HookInstall(FUNC_RenderBreathBar, &HOOK_RenderHudBar, 11);
    HookInstall(FUNC_RenderArmorBar, &HOOK_RenderHudBar, 11);
}
