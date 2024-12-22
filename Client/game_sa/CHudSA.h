/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHudSA.h
 *  PURPOSE:     Header file for HUD display class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CHud.h>
#include <CVector.h>
#include <game/RenderWare.h>
#include "CFontSA.h"

#define FUNC_Draw                   0x58FAE0

#define VAR_DisableClock            0xBAA400

// X
#define VAR_AspectRatioMultX        0x859520
// Y
#define VAR_AspectRatioMult         0x859524

#define VAR_CameraCrosshairScale    0x866C74

#define FUNC_DrawAmmo               0x5893B0
#define FUNC_DrawWeaponIcon         0x58D7D0
#define FUNC_RenderHealthBar        0x589270
#define FUNC_RenderBreathBar        0x589190
#define FUNC_RenderArmorBar         0x5890A0

#define FUNC_DrawVitalStats         0x589650
#define FUNC_DrawVehicleName        0x58AEA0
#define FUNC_DrawHelpText           0x58B6E0
#define FUNC_DrawAreaName           0x58AA50
#define FUNC_DrawRadar              0x58A330
#define FUNC_DrawRadarPlanB         0x58A335
#define FUNC_DrawRadioName          0x4E9E50
#define FUNC_DrawWantedLevel        0x58D9A0
#define FUNC_DrawCrosshair          0x58E020

#define FUNC_CStats_GetFatAndMuscleModifier 0x559AF0
#define FUNC_CSprite2d_DrawBarChart         0x728640

#define CODE_ShowMoney              0x58F47D

#define VAR_CTheScripts_bDrawCrossHair 0xA44490
#define VAR_RSGlobal                   0xC17040
#define VAR_ItemToFlash                0xBAB1DC

struct SHudComponent
{
    bool          bIsPartOfAll;
    eHudComponent type;
    bool          bSaveOriginalBytes;
    DWORD         uiDataAddr;
    DWORD         uiDataSize;
    DWORD         origData;
    DWORD         disabledData;
};

enum class eHudColour
{
    RED,
    GREEN,
    DARK_BLUE,
    LIGHT_BLUE,
    LIGHT_GRAY,
    BLACK,
    GOLD,
    PURPLE,
    DARK_GRAY,
    DARK_RED,
    DARK_GREEN,
    CREAM,
    NIGHT_BLUE,
    BLUE,
    YELLOW,
};

struct RsGlobal
{
    const char*  appName;
    std::int32_t maximumWidth;
    std::int32_t maximumHeight;
    std::int32_t frameLimit;
    bool         quit;
    void*        ps;
    std::uint8_t keyboard[12]; // RsInputDevice
    std::uint8_t mouse[12]; // RsInputDevice
    std::uint8_t pad[12]; // RsInputDevice
};

struct SComponentPlacement
{
    // Original position & size
    float x{0.0f}, y{0.0f}; // for getter function only
    float width{0.0f}, height{0.0f};

    // Custom position & size
    float customX{0.0f}, customY{0.0f};
    float customWidth{0.0f}, customHeight{0.0f};

    bool  useCustomPosition{false};
    bool  useCustomSize{false};
    bool  setDefaultXY{false};
};

struct SHudComponentData
{
    SComponentPlacement placement{};
    RwColor             fillColor{};
    RwColor             fillColor_Second{0,0,0,255};

    // Bar
    bool drawBlackBorder{true};
    bool drawPercentage{false};

    // Text
    RwColor             dropColor{};
    eFontAlignment      alignment{};
    eFontStyle          style{};
    std::int16_t        textOutline{0};
    std::int16_t        textShadow{0};
    bool                proportional{false};

    SHudComponentData(
        RwColor fill = {}, 
        RwColor fillSecond = {0, 0, 0, 255}, 
        bool blackBorder = true, 
        bool percentage = false, 
        RwColor drop = {}, 
        eFontAlignment align = eFontAlignment::ALIGN_LEFT, 
        eFontStyle fontStyle = eFontStyle::FONT_PRICEDOWN,
        std::int16_t outline = 0, 
        std::int16_t shadow = 0, 
        bool prop = false) : fillColor(fill),
          fillColor_Second(fillSecond), 
          drawBlackBorder(blackBorder), 
          drawPercentage(percentage), 
          dropColor(drop), 
          alignment(align), 
          style(fontStyle), 
          textOutline(outline), 
          textShadow(shadow), 
          proportional(prop) {}
};

struct ComponentProperties
{
    SHudComponentData hpBar;
    SHudComponentData breathBar;
    SHudComponentData armorBar;

    SHudComponentData clock;
    SHudComponentData money;
};

class CHudSA : public CHud
{
public:
    CHudSA();
    void Disable(bool bDisabled);
    bool IsDisabled();
    void SetComponentVisible(eHudComponent component, bool bVisible);
    bool IsComponentVisible(eHudComponent component);
    void AdjustComponents(float fAspectRatio);
    void ResetComponentAdjustment();
    bool IsCrosshairVisible();

    bool IsComponentBar(const eHudComponent& component) const noexcept override;
    bool IsComponentText(const eHudComponent& component) const noexcept override;

    void SetComponentPlacementPosition(SComponentPlacement& placement, const CVector2D& position) noexcept;
    void SetComponentPlacementSize(SComponentPlacement& placement, const CVector2D& size) noexcept;

    void SetComponentPosition(const eHudComponent& component, const CVector2D& position) noexcept override;
    void SetComponentSize(const eHudComponent& component, const CVector2D& size) noexcept override;

    void ResetComponentPlacement(const eHudComponent& component, bool resetSize) noexcept override;

    void SetComponentColor(const eHudComponent& component, std::uint32_t color, bool secondColor = false) noexcept override;
    void ResetComponentColor(const eHudComponent& component, bool secondColor = false) noexcept override;

    void SetComponentDrawBlackBorder(const eHudComponent& component, bool draw) noexcept override { GetHudComponentRef(component).drawBlackBorder = draw; }
    void SetComponentDrawPercentage(const eHudComponent& component, bool draw) noexcept override { GetHudComponentRef(component).drawPercentage = draw; }
    void SetHealthBarBlinkingValue(float minHealth) noexcept override { blinkingBarHPValue = minHealth; }

    void SetComponentFontDropColor(const eHudComponent& component, std::uint32_t color) noexcept override;
    void SetComponentFontOutline(const eHudComponent& component, float outline) noexcept override { GetHudComponentRef(component).textOutline = static_cast<std::int16_t>(outline); }
    void SetComponentFontShadow(const eHudComponent& component, float shadow) noexcept override { GetHudComponentRef(component).textShadow = static_cast<std::int16_t>(shadow); }
    void SetComponentFontStyle(const eHudComponent& component, const eFontStyle& style) noexcept override { GetHudComponentRef(component).style = style; }
    void SetComponentFontAlignment(const eHudComponent& component, const eFontAlignment& alignment) noexcept override { GetHudComponentRef(component).alignment = alignment; }
    void SetComponentFontProportional(const eHudComponent& component, bool proportional) noexcept override { GetHudComponentRef(component).proportional = proportional; }

    void ResetComponentFontOutline(const eHudComponent& component) noexcept override { ResetComponentFontData(component, eHudComponentProperty::TEXT_OUTLINE); }
    void ResetComponentFontShadow(const eHudComponent& component) noexcept override { ResetComponentFontData(component, eHudComponentProperty::TEXT_SHADOW); }
    void ResetComponentFontStyle(const eHudComponent& component) noexcept override { ResetComponentFontData(component, eHudComponentProperty::TEXT_STYLE); }
    void ResetComponentFontAlignment(const eHudComponent& component) noexcept override { ResetComponentFontData(component, eHudComponentProperty::TEXT_ALIGNMENT); }
    void ResetComponentFontProportional(const eHudComponent& component) noexcept override { ResetComponentFontData(component, eHudComponentProperty::TEXT_PROPORTIONAL); }

    static RsGlobal* GetRSGlobal() noexcept { return rsGlobal; }
    static RwColor   GetHUDColour(const eHudColour& colour) noexcept;

    static void StaticSetHooks();

private:
    void InitComponentList();
    void UpdateStreetchCalculations();
    void ResetComponent(SComponentPlacement& placement, bool resetSize) noexcept;
    void ResetComponentFontData(const eHudComponent& component, const eHudComponentProperty& property) noexcept;
    
    SHudComponentData& GetHudComponentRef(const eHudComponent& component) const noexcept;

    static void RenderHealthBar(int x, int y);
    static void RenderBreathBar(int x, int y);
    static void RenderArmorBar(int x, int y);

    static void RenderText(float x, float y, const char* text, SHudComponentData& properties, bool useSecondColor = false);
    static void RenderClock(float x, float y, const char* strTime);
    static void RenderMoney(float x, float y, const char* strMoney);

private:
    std::map<eHudComponent, SHudComponent> m_HudComponentMap;

    float* m_pfAspectRatioMultiplicator;
    float* m_pfCameraCrosshairScale;
    float  m_fSniperCrosshairScale;

    static RsGlobal* rsGlobal;
    static std::int16_t* itemToFlash;

    static float calcStreetchX;
    static float calcStreetchY;
    static float blinkingBarHPValue;
};
