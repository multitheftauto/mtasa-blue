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
#include "CAERadioTrackManagerSA.h"

extern CGameSA* pGame;

char szVehicleName[50] = {'\0'};
char szZoneName[50] = {'\0'};

static ComponentProperties componentProperties;

RsGlobal* CHudSA::rsGlobal = reinterpret_cast<RsGlobal*>(VAR_RSGlobal);
std::int16_t* CHudSA::itemToFlash = reinterpret_cast<std::int16_t*>(VAR_ItemToFlash);

float CHudSA::calcStreetchX = 0.0f;
float CHudSA::calcStreetchY = 0.0f;
float CHudSA::blinkingBarHPValue = 10.0f;

constexpr RwColor COLOR_BLACK = RwColor{0, 0, 0, 255};

// CSprite2d::DrawBarChart
using DrawBarChartFunc = void(__cdecl*)(float, float, std::uint16_t, std::uint32_t, float, bool, bool, bool, RwColor, RwColor);
DrawBarChartFunc DrawBarChart = reinterpret_cast<DrawBarChartFunc>(FUNC_CSprite2d_DrawBarChart);

// default component properties
std::unordered_map<eHudComponent, SHudComponentData> defaultComponentProperties = {
    {HUD_HEALTH, {CHudSA::GetHUDColour(eHudColour::RED)}},
    {HUD_BREATH, {CHudSA::GetHUDColour(eHudColour::LIGHT_BLUE)}},
    {HUD_ARMOUR, {CHudSA::GetHUDColour(eHudColour::LIGHT_GRAY)}},
    {HUD_CLOCK, {CHudSA::GetHUDColour(eHudColour::LIGHT_GRAY), {}, false, false, COLOR_BLACK, eFontAlignment::ALIGN_RIGHT, eFontStyle::FONT_PRICEDOWN, 2}},
    {HUD_MONEY, {CHudSA::GetHUDColour(eHudColour::GREEN), CHudSA::GetHUDColour(eHudColour::RED), false, false, COLOR_BLACK, eFontAlignment::ALIGN_RIGHT, eFontStyle::FONT_PRICEDOWN, 2}},
    {HUD_AMMO, {CHudSA::GetHUDColour(eHudColour::LIGHT_BLUE), {}, false, false, COLOR_BLACK, eFontAlignment::ALIGN_CENTER, eFontStyle::FONT_SUBTITLES, 1, 0, true}},
    {HUD_VEHICLE_NAME, {CHudSA::GetHUDColour(eHudColour::GREEN), {}, false, false, COLOR_BLACK, eFontAlignment::ALIGN_CENTER, eFontStyle::FONT_MENU, 2, 0, true}},
    {HUD_AREA_NAME, {CHudSA::GetHUDColour(eHudColour::LIGHT_BLUE), {}, false, false, COLOR_BLACK, eFontAlignment::ALIGN_CENTER, eFontStyle::FONT_GOTHIC, 2, 0, true}},
    {HUD_RADIO, {CHudSA::GetHUDColour(eHudColour::GOLD), CHudSA::GetHUDColour(eHudColour::DARK_GRAY), false, false, COLOR_BLACK, eFontAlignment::ALIGN_CENTER, eFontStyle::FONT_MENU, 1, 0, true}},
    {HUD_WEAPON, {RwColor{255, 255, 255, 255}, RwColor{255, 255, 255, 255}}},
    {HUD_WANTED, {CHudSA::GetHUDColour(eHudColour::GOLD), RwColor{0, 0, 0, 170}, false, false, COLOR_BLACK, eFontAlignment::ALIGN_RIGHT, eFontStyle::FONT_GOTHIC, 1, 0, true}}
};

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

    // Initalize default data
    componentProperties.hpBar = MapGet(defaultComponentProperties, HUD_HEALTH);
    componentProperties.breathBar = MapGet(defaultComponentProperties, HUD_BREATH);
    componentProperties.armorBar = MapGet(defaultComponentProperties, HUD_ARMOUR);
    componentProperties.clock = MapGet(defaultComponentProperties, HUD_CLOCK);
    componentProperties.money = MapGet(defaultComponentProperties, HUD_MONEY);
    componentProperties.ammo = MapGet(defaultComponentProperties, HUD_AMMO);
    componentProperties.vehName = MapGet(defaultComponentProperties, HUD_VEHICLE_NAME);
    componentProperties.areaName = MapGet(defaultComponentProperties, HUD_AREA_NAME);
    componentProperties.radioName = MapGet(defaultComponentProperties, HUD_RADIO);
    componentProperties.weaponIcon = MapGet(defaultComponentProperties, HUD_WEAPON);
    componentProperties.wanted = MapGet(defaultComponentProperties, HUD_WANTED);
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

    SComponentPlacement& hpPlacement = componentProperties.hpBar.placement;
    hpPlacement.height = calcStreetchY * 9.0f;
    hpPlacement.width = calcStreetchX * 109.0f;
    hpPlacement.setDefaultXY = false;

    SComponentPlacement& breathPlacement = componentProperties.breathBar.placement;
    breathPlacement.height = calcStreetchY * 9.0f;
    breathPlacement.width = calcStreetchX * 62.0f;
    breathPlacement.setDefaultXY = false;

    SComponentPlacement& armorPlacement = componentProperties.armorBar.placement;
    armorPlacement.height = calcStreetchY * 9.0f;
    armorPlacement.width = calcStreetchX * 62.0f;
    armorPlacement.setDefaultXY = false;

    SComponentPlacement& clockPlacement = componentProperties.clock.placement;
    clockPlacement.height = calcStreetchY * 1.1f;
    clockPlacement.width = calcStreetchX * 0.55f;
    clockPlacement.setDefaultXY = false;

    SComponentPlacement& moneyPlacement = componentProperties.money.placement;
    moneyPlacement.height = calcStreetchY * 1.1f;
    moneyPlacement.width = calcStreetchX * 0.55f;
    moneyPlacement.setDefaultXY = false;

    SComponentPlacement& ammoPlacement = componentProperties.ammo.placement;
    ammoPlacement.height = calcStreetchY * 0.7f;
    ammoPlacement.width = calcStreetchX * 0.3f;
    ammoPlacement.setDefaultXY = false;

    SComponentPlacement& vehNamePlacement = componentProperties.vehName.placement;
    vehNamePlacement.height = calcStreetchY * 1.5f;
    vehNamePlacement.width = calcStreetchX * 1.0f;
    vehNamePlacement.setDefaultXY = false;

    SComponentPlacement& areaNamePlacement = componentProperties.areaName.placement;
    areaNamePlacement.height = calcStreetchY * 1.9f;
    areaNamePlacement.width = calcStreetchX * 1.2f;
    areaNamePlacement.setDefaultXY = false;

    SComponentPlacement& radioPlacement = componentProperties.radioName.placement;
    radioPlacement.height = calcStreetchY * 0.9f;
    radioPlacement.width = calcStreetchX * 0.6f;
    radioPlacement.setDefaultXY = false;

    SComponentPlacement& weaponPlacement = componentProperties.weaponIcon.placement;
    weaponPlacement.height = calcStreetchY * 58.0f;
    weaponPlacement.width = calcStreetchX * 47.0f;
    weaponPlacement.setDefaultXY = false;

    SComponentPlacement& wantedPlacement = componentProperties.wanted.placement;
    wantedPlacement.height = calcStreetchY * 1.21f;
    wantedPlacement.width = calcStreetchX * 0.6f;
    wantedPlacement.setDefaultXY = false;
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

bool CHudSA::IsComponentBar(const eHudComponent& component) const noexcept
{
    switch (component)
    {
        case HUD_HEALTH:
        case HUD_ARMOUR:
        case HUD_BREATH:
            return true;
    }

    return false;
}

bool CHudSA::IsComponentText(const eHudComponent& component) const noexcept
{
    switch (component)
    {
        case HUD_CLOCK:
        case HUD_MONEY:
        case HUD_AMMO:
        case HUD_AREA_NAME:
        case HUD_VEHICLE_NAME:
        case HUD_RADIO:
        case HUD_WANTED:
            return true;
    }

    return false;
}

CVector2D CHudSA::GetComponentTextSize(const eHudComponent& component) const
{
    const auto& ref = GetHudComponentRef(component);
    return CVector2D(ref.placement.stringWidth, ref.placement.stringHeight);
}

RwColor CHudSA::GetHUDColour(const eHudColour& colour) noexcept
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

void CHudSA::SetComponentPlacementPosition(SComponentPlacement& placement, const CVector2D& position) noexcept
{
    placement.customX = position.fX;
    placement.customY = position.fY;
    placement.useCustomPosition = true;
}

void CHudSA::SetComponentPlacementSize(SComponentPlacement& placement, const CVector2D& size) noexcept
{
    placement.customWidth = size.fX;
    placement.customHeight = size.fY;
    placement.useCustomSize = true;
}

void CHudSA::ResetComponent(SComponentPlacement& placement, bool resetSize) noexcept
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

void CHudSA::ResetComponentFontData(const eHudComponent& component, const eHudComponentProperty& property) noexcept
{
    if (component == HUD_ALL)
    {
        for (const auto& cmp : m_HudComponentMap)
        {
            if (cmp.first == HUD_ALL)
                continue;

            ResetComponentFontData(cmp.first, property);
        }

        return;
    }

    if (!IsComponentText(component))
        return;

    auto& ref = GetHudComponentRef(component);
    if (!MapFind(defaultComponentProperties, component))
        return;

    const auto& defaultRef = MapGet(defaultComponentProperties, component);

    switch (property)
    {
        case eHudComponentProperty::TEXT_OUTLINE:
            ref.textOutline = defaultRef.textOutline;
            break;
        case eHudComponentProperty::TEXT_SHADOW:
            ref.textShadow = defaultRef.textShadow;
            break;
        case eHudComponentProperty::TEXT_STYLE:
            ref.style = defaultRef.style;
            break;
        case eHudComponentProperty::TEXT_ALIGNMENT:
            ref.alignment = defaultRef.alignment;
            break;
        case eHudComponentProperty::TEXT_PROPORTIONAL:
            ref.proportional = defaultRef.proportional;
            break;
    }
}

SHudComponentData& CHudSA::GetHudComponentRef(const eHudComponent& component) const noexcept
{
    switch (component)
    {
        case HUD_HEALTH:
            return componentProperties.hpBar;
        case HUD_BREATH:
            return componentProperties.breathBar;
        case HUD_ARMOUR:
            return componentProperties.armorBar;
        case HUD_CLOCK:
            return componentProperties.clock;
        case HUD_MONEY:
            return componentProperties.money;
        case HUD_AMMO:
            return componentProperties.ammo;
        case HUD_VEHICLE_NAME:
            return componentProperties.vehName;
        case HUD_AREA_NAME:
            return componentProperties.areaName;
        case HUD_RADIO:
            return componentProperties.radioName;
        case HUD_WEAPON:
            return componentProperties.weaponIcon;
        case HUD_WANTED:
            return componentProperties.wanted;
    }
}

void CHudSA::ResetComponentPlacement(const eHudComponent& component, bool resetSize) noexcept
{
    if (component == HUD_ALL)
    {
        for (const auto& cmp : m_HudComponentMap)
        {
            if (cmp.first == HUD_ALL)
                continue;

            ResetComponentPlacement(cmp.first, resetSize);
        }

        return;
    }

    ResetComponent(GetHudComponentRef(component).placement, resetSize);
}

void CHudSA::SetComponentColor(const eHudComponent& component, std::uint32_t color, bool secondColor) noexcept
{ 
    SColor newColor = TOCOLOR2SCOLOR(color);
    auto&  compRef = GetHudComponentRef(component);

    if (!secondColor)
        compRef.fillColor = RwColor{newColor.R, newColor.G, newColor.B, newColor.A};
    else
        compRef.fillColorSecondary = RwColor{newColor.R, newColor.G, newColor.B, newColor.A};
}

void CHudSA::ResetComponentColor(const eHudComponent& component, bool secondColor) noexcept
{
    auto& componentData = GetHudComponentRef(component);
    if (!MapFind(defaultComponentProperties, component))
        return;

    const auto& defaultRef = MapGet(defaultComponentProperties, component);

    if (!secondColor)
        componentData.fillColor = defaultRef.fillColor;
    else
        componentData.fillColorSecondary = defaultRef.fillColorSecondary;
}

void CHudSA::SetComponentFontDropColor(const eHudComponent& component, std::uint32_t color) noexcept
{
    SColor newColor = TOCOLOR2SCOLOR(color);
    GetHudComponentRef(component).dropColor = RwColor{newColor.R, newColor.G, newColor.B, newColor.A};
}

CVector2D CHudSA::GetComponentPosition(const eHudComponent& component) const noexcept
{
    const auto& ref = GetHudComponentRef(component);

    float x = ref.placement.useCustomPosition ? ref.placement.customX : ref.placement.x;
    float y = ref.placement.useCustomPosition ? ref.placement.customY : ref.placement.y;

    return CVector2D(x, y);
}

CVector2D CHudSA::GetComponentSize(const eHudComponent& component) const noexcept
{
    const auto& ref = GetHudComponentRef(component);

    float w = ref.placement.useCustomSize ? ref.placement.customWidth : ref.placement.width;
    float h = ref.placement.useCustomSize ? ref.placement.customHeight : ref.placement.height;

    return CVector2D(w, h);
}

SColor CHudSA::GetComponentColor(const eHudComponent& component) const noexcept
{
    const auto& ref = GetHudComponentRef(component);
    return SColorRGBA(ref.fillColor.r, ref.fillColor.g, ref.fillColor.b, ref.fillColor.a);
}

SColor CHudSA::GetComponentSecondaryColor(const eHudComponent& component) const noexcept
{
    const auto& ref = GetHudComponentRef(component);
    return SColorRGBA(ref.fillColorSecondary.r, ref.fillColorSecondary.g, ref.fillColorSecondary.b, ref.fillColorSecondary.a);
}

SColor CHudSA::GetComponentFontDropColor(const eHudComponent& component) const
{
    const auto& ref = GetHudComponentRef(component);
    const RwColor& color = CFontSA::GetDropColor();
    return SColorRGBA(color.r, color.g, color.b, color.a);
}

void CHudSA::RenderHealthBar(int x, int y)
{
    // Flash each 8 frames
    bool isValidFrame = (pGame->GetSystemFrameCounter() & 8) == 0;
    if (*itemToFlash == 4 && isValidFrame) // 4 = HEALTH_BAR
        return;

    CPed* playerPed = pGame->GetPedContext();
    if (!playerPed || (playerPed->GetHealth() <= blinkingBarHPValue && isValidFrame))
        return;

    // Save default position once
    if (!componentProperties.hpBar.placement.setDefaultXY)
    {
        componentProperties.hpBar.placement.x = x;
        componentProperties.hpBar.placement.y = y;
        componentProperties.hpBar.placement.setDefaultXY = true;
    }

    // Get player max health
    float maxHealth = static_cast<float>(pGame->GetPlayerInfo()->GetMaxHealth());

    // Use custom position/size?
    bool useCustomPosition = componentProperties.hpBar.placement.useCustomPosition;
    bool useCustomSize = componentProperties.hpBar.placement.useCustomSize;

    float barWidth = useCustomSize ? componentProperties.hpBar.placement.customWidth : componentProperties.hpBar.placement.width;

    // Calc bar width depending on MAX_HEALTH stat
    double statModifier = ((double(__cdecl*)(int))FUNC_CStats_GetFatAndMuscleModifier)(10);
    float  totalWidth = (barWidth * maxHealth) / statModifier;

    float posX = useCustomPosition ? componentProperties.hpBar.placement.customX : (barWidth - totalWidth + x);
    float posY = useCustomPosition ? componentProperties.hpBar.placement.customY : y;
    std::uint32_t barHeight = static_cast<std::uint32_t>(useCustomSize ? componentProperties.hpBar.placement.customHeight : componentProperties.hpBar.placement.height);

    // call CSprite2d::DrawBarChart
    DrawBarChart(posX, posY, static_cast<std::uint16_t>(totalWidth), barHeight, playerPed->GetHealth() * 100.0f / maxHealth, false, componentProperties.hpBar.drawPercentage, componentProperties.hpBar.drawBlackBorder, componentProperties.hpBar.fillColor, COLOR_BLACK);
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
    if (!componentProperties.breathBar.placement.setDefaultXY)
    {
        componentProperties.breathBar.placement.x = x;
        componentProperties.breathBar.placement.y = y;
        componentProperties.breathBar.placement.setDefaultXY = true;
    }

    // Calc bar width depending on AIR_IN_LUNG stat
    double statModifier = ((double(__cdecl*)(int))FUNC_CStats_GetFatAndMuscleModifier)(8);

    // Use custom position/size?
    bool useCustomPosition = componentProperties.breathBar.placement.useCustomPosition;
    bool useCustomSize = componentProperties.breathBar.placement.useCustomSize;

    float posX = useCustomPosition ? componentProperties.breathBar.placement.customX : x;
    float posY = useCustomPosition ? componentProperties.breathBar.placement.customY : y;
    std::uint16_t barWidth = static_cast<std::uint16_t>(useCustomSize ? componentProperties.breathBar.placement.customWidth : componentProperties.breathBar.placement.width);
    std::uint32_t barHeight = static_cast<std::uint32_t>(useCustomSize ? componentProperties.breathBar.placement.customHeight : componentProperties.breathBar.placement.height);

    // call CSprite2d::DrawBarChart
    DrawBarChart(posX, posY, barWidth, barHeight, playerPed->GetOxygenLevel() / statModifier * 100.0f, false, componentProperties.breathBar.drawPercentage, componentProperties.breathBar.drawBlackBorder, componentProperties.breathBar.fillColor, COLOR_BLACK);
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
    if (!componentProperties.armorBar.placement.setDefaultXY)
    {
        componentProperties.armorBar.placement.x = x;
        componentProperties.armorBar.placement.y = y;
        componentProperties.armorBar.placement.setDefaultXY = true;
    }

    // Use custom position/size?
    bool useCustomPosition = componentProperties.hpBar.placement.useCustomPosition;
    bool useCustomSize = componentProperties.hpBar.placement.useCustomSize;

    float posX = useCustomPosition ? componentProperties.armorBar.placement.customX : x;
    float posY = useCustomPosition ? componentProperties.armorBar.placement.customY : y;
    std::uint16_t barWidth = static_cast<std::uint16_t>(useCustomSize ? componentProperties.armorBar.placement.customWidth : componentProperties.armorBar.placement.width);
    std::uint32_t barHeight = static_cast<std::uint32_t>(useCustomSize ? componentProperties.armorBar.placement.customHeight : componentProperties.armorBar.placement.height);

    // call CSprite2d::DrawBarChart
    DrawBarChart(posX, posY, barWidth, barHeight, playerPed->GetArmor() / static_cast<float>(pGame->GetPlayerInfo()->GetMaxArmor()) * 100.0f, false, componentProperties.armorBar.drawPercentage, componentProperties.armorBar.drawBlackBorder, componentProperties.armorBar.fillColor, COLOR_BLACK);
}

void CHudSA::RenderText(float x, float y, const char* text, SHudComponentData& properties, bool useSecondColor, bool drawFromBottom, bool scaleForLanguage)
{
    // Use custom position/size?
    bool useCustomPosition = properties.placement.useCustomPosition;
    bool useCustomSize = properties.placement.useCustomSize;

    float scaleX = useCustomSize ? properties.placement.customWidth : properties.placement.width;
    float scaleY = useCustomSize ? properties.placement.customHeight : properties.placement.height;

    if (!scaleForLanguage)
        CFontSA::SetScale(scaleX, scaleY);
    else
        CFontSA::SetScaleForCurrentLanguage(scaleX, scaleY);

    CFontSA::SetProportional(properties.proportional);

    CFontSA::SetDropShadowPosition(properties.textShadow);
    CFontSA::SetEdge(properties.textOutline);

    CFontSA::SetOrientation(properties.alignment);
    CFontSA::SetFontStyle(properties.style);

    if (useSecondColor && &properties == &componentProperties.wanted)
    {
        CFontSA::SetScale(scaleX * 1.2f, scaleY * 1.2f);
        CFontSA::SetEdge(0);
    }

    if (!properties.useCustomAlpha)
    {
        CFontSA::SetDropColor(RwColor{properties.dropColor.r, properties.dropColor.g, properties.dropColor.b, CFontSA::GetColor().a});
        CFontSA::SetColor(useSecondColor ? RwColor{properties.fillColorSecondary.r, properties.fillColorSecondary.g, properties.fillColorSecondary.b, CFontSA::GetColor().a} : RwColor{properties.fillColor.r, properties.fillColor.g, properties.fillColor.b, CFontSA::GetColor().a});
    }
    else
    {
        CFontSA::SetDropColor(properties.dropColor);
        CFontSA::SetColor(useSecondColor ? properties.fillColorSecondary : properties.fillColor);
    }

    // Save default position once
    if (!properties.placement.setDefaultXY)
    {
        properties.placement.x = x;
        properties.placement.y = y;
        properties.placement.stringWidth = CFontSA::GetStringWidth(text, true);
        properties.placement.stringHeight = CFontSA::GetFontHeight(CFontSA::GetScale().fY);

        properties.placement.setDefaultXY = true;
    }

    // Draw text
    float posX = useCustomPosition ? properties.placement.customX : x;
    float posY = useCustomPosition ? properties.placement.customY : y;

    if (!drawFromBottom)
        CFontSA::PrintString(posX, posY, text);
    else
        CFontSA::PrintStringFromBottom(posX, posY, text);
}

void CHudSA::RenderClock(float x, float y, const char* strTime)
{
    RenderText(x, y, strTime, componentProperties.clock);
}

void CHudSA::RenderMoney(float x, float y, const char* strMoney)
{
    RenderText(x, y, strMoney, componentProperties.money, pGame->GetPlayerInfo()->GetPlayerMoney() < 0);
}

void CHudSA::RenderAmmo(float x, float y, const char* strAmmo)
{
    RenderText(x, y, strAmmo, componentProperties.ammo);
}

void CHudSA::RenderVehicleName(float x, float y, const char* vehName)
{
    RenderText(x, y, vehName, componentProperties.vehName, false, false, true);
}

void CHudSA::RenderZoneName(float x, float y, const char* strArea)
{
    RenderText(x, y, strArea, componentProperties.areaName, false, true, true);
}

void CHudSA::RenderRadioName(float x, float y, const char* strRadio)
{
    RenderText(x, y, strRadio, componentProperties.radioName, pGame->GetAERadioTrackManager()->IsStationLoading());
}

void __fastcall CHudSA::RenderWeaponIcon_Sprite(void* sprite, void*, CRect* rect, RwColor* color)
{
    // Use custom position/size?
    SHudComponentData& properties = componentProperties.weaponIcon;

    bool useCustomPosition = properties.placement.useCustomPosition;
    bool useCustomSize = properties.placement.useCustomSize;

    // Save default position once
    if (!properties.placement.setDefaultXY)
    {
        properties.placement.x = rect->left;
        properties.placement.y = rect->top;
        properties.placement.setDefaultXY = true;
    }

    if (useCustomPosition)
    {
        rect->left = properties.placement.customX;
        rect->top = properties.placement.customY;
    }

    if (useCustomPosition || useCustomSize)
    {
        rect->right = rect->left + (useCustomSize ? properties.placement.customWidth : properties.placement.width);
        rect->bottom = rect->top + (useCustomSize ? properties.placement.customHeight : properties.placement.height);
    }

    color->r = properties.fillColorSecondary.r;
    color->g = properties.fillColorSecondary.g;
    color->b = properties.fillColorSecondary.b;

    if (properties.useCustomAlpha)
        color->a = properties.fillColorSecondary.a;

    // Call CSprite2d::Draw
    ((void(__thiscall*)(void*, CRect*, RwColor*))FUNC_CSprite2d_Draw)(sprite, rect, color);
}

void CHudSA::RenderWeaponIcon_XLU(CVector pos, CVector2D halfSize, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint16_t intensity, float rhw, std::uint8_t a, std::uint8_t uDir, std::uint8_t vDir)
{
    // Use custom position/size?
    SHudComponentData& properties = componentProperties.weaponIcon;

    bool useCustomPosition = properties.placement.useCustomPosition;
    bool useCustomSize = properties.placement.useCustomSize;

    // Save default position once
    if (!properties.placement.setDefaultXY)
    {
        properties.placement.x = pos.fX - halfSize.fX;
        properties.placement.y = pos.fY - halfSize.fY;
        properties.placement.setDefaultXY = true;
    }

    float x = useCustomPosition ? properties.placement.customX : properties.placement.x;
    float y = useCustomPosition ? properties.placement.customY : properties.placement.y;
    float w = useCustomSize ? properties.placement.customWidth : properties.placement.width;
    float h = useCustomSize ? properties.placement.customHeight : properties.placement.height;

    pos.fX = x + w * 0.5f;
    pos.fY = y + h * 0.5f;

    if (useCustomSize)
    {
        halfSize.fX = w * 0.5f;
        halfSize.fY = h * 0.5f;
    }

    r = properties.fillColor.r;
    g = properties.fillColor.g;
    b = properties.fillColor.b;

    if (properties.useCustomAlpha)
    {
        a = properties.fillColor.a;
        intensity = a;
    }

    // Call CSprite::RenderOneXLUSprite
    ((void(__cdecl*)(CVector, CVector2D, std::uint8_t, std::uint8_t, std::uint8_t, std::uint16_t, float, std::uint8_t, std::uint8_t, std::uint8_t))FUNC_CSprite_RenderOneXLUSprite)(pos, halfSize, r, g, b, intensity, rhw, a, uDir, vDir);
}

void CHudSA::RenderWanted(bool empty, float x, float y, const char* strLevel)
{
    RenderText(x, y, strLevel, componentProperties.wanted, empty);
}

static constexpr std::uintptr_t CONTINUE_RenderWanted = 0x58DFD8;
static void _declspec(naked) HOOK_RenderWanted()
{
    _asm
    {
        cmp ebp, edi
        jle empty

        push 0
        jmp render

        empty:
        push 1

        render:
        call CHudSA::RenderWanted
        add esp,4

        jmp CONTINUE_RenderWanted
    }
}

static void HOOK_RenderHudBar(int playerId, int x, int y)
{
    void* returnAdress = _ReturnAddress();
    if (returnAdress == (void*)0x58EE9F || returnAdress == (void*)0x58EF12)
        CHudSA::RenderHealthBar(x, y);
    else if (returnAdress == (void*)0x58F136 || returnAdress == (void*)0x58F1B2)
        CHudSA::RenderBreathBar(x, y);
    else if (returnAdress == (void*)0x58EF70 || returnAdress == (void*)0x58EFE3)
        CHudSA::RenderArmorBar(x, y);
}

void CHudSA::StaticSetHooks()
{
    HookInstall(FUNC_RenderHealthBar, &HOOK_RenderHudBar, 11);
    HookInstall(FUNC_RenderBreathBar, &HOOK_RenderHudBar, 11);
    HookInstall(FUNC_RenderArmorBar, &HOOK_RenderHudBar, 11);

    HookInstallCall(0x58EC21, (DWORD)&RenderClock);
    HookInstallCall(0x58F607, (DWORD)&RenderMoney);
    HookInstallCall(0x58962A, (DWORD)&RenderAmmo);

    HookInstallCall(0x58B156, (DWORD)&RenderVehicleName);
    HookInstallCall(0x58AE5D, (DWORD)&RenderZoneName);
    HookInstallCall(0x4E9FF1, (DWORD)&RenderRadioName);

    HookInstallCall(0x58D988, (DWORD)&RenderWeaponIcon_Sprite);
    HookInstallCall(0x58D8FD, (DWORD)&RenderWeaponIcon_XLU);

    HookInstall(0x58DFD3, &HOOK_RenderWanted);
}
