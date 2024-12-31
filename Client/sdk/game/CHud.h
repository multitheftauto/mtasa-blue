/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CHud.h
 *  PURPOSE:     HUD interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CVector2D.h"

enum eHudComponent
{
    // Order must be preserved for net comms
    HUD_AMMO,
    HUD_WEAPON,
    HUD_HEALTH,
    HUD_BREATH,
    HUD_ARMOUR,
    HUD_MONEY,
    HUD_VEHICLE_NAME,
    HUD_AREA_NAME,
    HUD_RADAR,
    HUD_CLOCK,
    HUD_RADIO,
    HUD_WANTED,
    HUD_CROSSHAIR,
    HUD_ALL,
    HUD_VITAL_STATS,
    HUD_HELP_TEXT,
};

enum class eHudComponentProperty
{
    FILL_COLOR,
    FILL_COLOR_SECONDARY,
    DRAW_BLACK_BORDER,
    DRAW_PERCENTAGE,
    BLINKING_HP_VALUE,
    POSITION,
    SIZE,
    DROP_COLOR,
    TEXT_OUTLINE,
    TEXT_SHADOW,
    TEXT_STYLE,
    TEXT_ALIGNMENT,
    TEXT_PROPORTIONAL,
    CUSTOM_ALPHA,
    TEXT_SIZE,

    ALL_PROPERTIES,
};

enum class eFontStyle : std::uint8_t
{
    FONT_GOTHIC,
    FONT_SUBTITLES,
    FONT_MENU,
    FONT_PRICEDOWN,
};

enum class eFontAlignment : std::uint8_t
{
    ALIGN_CENTER,
    ALIGN_LEFT,
    ALIGN_RIGHT,
};

class CHud
{
public:
    virtual void Disable(bool bDisabled) = 0;
    virtual bool IsDisabled() = 0;
    virtual void SetComponentVisible(eHudComponent component, bool bVisible) = 0;
    virtual bool IsComponentVisible(eHudComponent component) = 0;
    virtual void AdjustComponents(float fAspectRatio) = 0;
    virtual void ResetComponentAdjustment() = 0;
    virtual bool IsCrosshairVisible() = 0;

    // Hud properties
    virtual bool IsComponentBar(const eHudComponent& component) const noexcept = 0;
    virtual bool IsComponentText(const eHudComponent& component) const noexcept = 0;

    virtual void SetComponentPosition(const eHudComponent& component, const CVector2D& position) noexcept = 0;
    virtual void SetComponentSize(const eHudComponent& component, const CVector2D& size) noexcept = 0;
    virtual void ResetComponentPlacement(const eHudComponent& component, bool resetSize) noexcept = 0;

    virtual void SetComponentColor(const eHudComponent& component, std::uint32_t color, bool secondColor = false) noexcept = 0;
    virtual void ResetComponentColor(const eHudComponent& component, bool secondColor = false) noexcept = 0;

    virtual void SetComponentDrawBlackBorder(const eHudComponent& component, bool draw) noexcept = 0;
    virtual void SetComponentDrawPercentage(const eHudComponent& component, bool draw) noexcept = 0;
    virtual void SetHealthBarBlinkingValue(float minHealth) noexcept = 0;

    virtual void SetComponentFontDropColor(const eHudComponent& component, std::uint32_t color) noexcept = 0;
    virtual void SetComponentFontOutline(const eHudComponent& component, float outline) noexcept = 0;
    virtual void SetComponentFontShadow(const eHudComponent& component, float shadow) noexcept = 0;
    virtual void SetComponentFontStyle(const eHudComponent& component, const eFontStyle& style) noexcept = 0;
    virtual void SetComponentFontAlignment(const eHudComponent& component, const eFontAlignment& alignment) noexcept = 0;
    virtual void SetComponentFontProportional(const eHudComponent& component, bool proportional) noexcept = 0;

    virtual void SetComponentUseCustomAlpha(const eHudComponent& component, bool useCustomAlpha) noexcept = 0;

    virtual void ResetComponentFontOutline(const eHudComponent& component) noexcept = 0;
    virtual void ResetComponentFontShadow(const eHudComponent& component) noexcept = 0;
    virtual void ResetComponentFontStyle(const eHudComponent& component) noexcept = 0;
    virtual void ResetComponentFontAlignment(const eHudComponent& component) noexcept = 0;
    virtual void ResetComponentFontProportional(const eHudComponent& component) noexcept = 0;

    virtual CVector2D      GetComponentPosition(const eHudComponent& component) const noexcept = 0;
    virtual CVector2D      GetComponentSize(const eHudComponent& component) const noexcept = 0;

    virtual SColor         GetComponentColor(const eHudComponent& component) const noexcept = 0;
    virtual SColor         GetComponentSecondaryColor(const eHudComponent& component) const noexcept = 0;
    virtual SColor         GetComponentFontDropColor(const eHudComponent& component) const = 0;

    virtual bool           GetComponentDrawBlackBorder(const eHudComponent& component) const noexcept = 0;
    virtual bool           GetComponentDrawPercentage(const eHudComponent& component) const noexcept = 0;
    virtual float          GetHealthBarBlinkingValue(const eHudComponent& component) const noexcept = 0;

    virtual float          GetComponentFontOutline(const eHudComponent& component) const = 0;
    virtual float          GetComponentFontShadow(const eHudComponent& component) const = 0;
    virtual eFontStyle     GetComponentFontStyle(const eHudComponent& component) const = 0;
    virtual eFontAlignment GetComponentFontAlignment(const eHudComponent& component) const = 0;
    virtual bool           GetComponentFontProportional(const eHudComponent& component) const = 0;

    virtual bool           GetComponentUseCustomAlpha(const eHudComponent& component) const noexcept = 0;

    virtual CVector2D      GetComponentTextSize(const eHudComponent& component) const = 0;
};
