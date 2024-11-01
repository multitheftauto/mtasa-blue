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
    const char* appName;
    std::int32_t maximumWidth;
    std::int32_t maximumHeight;
    std::int32_t frameLimit;
    bool        quit;
    void*       ps;
    std::uint8_t keyboard[12]; // RsInputDevice
    std::uint8_t mouse[12]; // RsInputDevice
    std::uint8_t pad[12]; // RsInputDevice
};

struct ComponentPlacement
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

struct HudBar
{
    ComponentPlacement placement;
    RwColor fillColor{};
    bool    drawBlackBorder{true};
    bool    drawPercentage{false};
};

struct HealthBar
{
    HudBar  bar;
    float   blinkingBarHP{10.0f};
};

struct BreathBar
{
    HudBar bar;
};

struct ArmorBar
{
    HudBar bar;
};

struct ComponentProperties
{
    HealthBar hpBar;
    BreathBar breathBar;
    ArmorBar  armorBar;
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

    void SetComponentPlacementPosition(ComponentPlacement& placement, const CVector2D& position);
    void SetComponentPlacementSize(ComponentPlacement& placement, const CVector2D& size);

    void SetComponentPosition(const eHudComponent& component, const CVector2D& position) noexcept override;
    void SetComponentSize(const eHudComponent& component, const CVector2D& size) noexcept override;

    void ResetComponentPlacement(const eHudComponent& component, bool resetSize) noexcept override;

    void SetComponentBarColor(const eHudComponent& component, float color) noexcept override;
    void SetComponentDrawBlackBorder(const eHudComponent& component, bool draw) noexcept override;
    void SetComponentDrawPercentage(const eHudComponent& component, bool draw) noexcept override;
    void SetHealthBarBlinkingValue(float minHealth) noexcept override;

    static RsGlobal* GetRSGlobal() noexcept { return rsGlobal; }
    static RwColor   GetHUDColour(const eHudColour& colour);

    static void StaticSetHooks();

protected:
    void InitComponentList();
    void UpdateStreetchCalculations();
    void ResetComponent(ComponentPlacement& placement, bool resetSize) noexcept;

    static void RenderHealthBar(int x, int y);
    static void RenderBreathBar(int x, int y);
    static void RenderArmorBar(int x, int y);

    static HudBar& GetHudBarRef(const eHudComponent& component) noexcept;

    std::map<eHudComponent, SHudComponent> m_HudComponentMap;

    float* m_pfAspectRatioMultiplicator;
    float* m_pfCameraCrosshairScale;
    float  m_fSniperCrosshairScale;

    static RsGlobal* rsGlobal;
    static std::int16_t* itemToFlash;

    static float calcStreetchX;
    static float calcStreetchY;
};
