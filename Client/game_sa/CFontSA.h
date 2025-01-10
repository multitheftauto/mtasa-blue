/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFontSA.h
 *  PURPOSE:     Header file for font class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include <CVector2D.h>
#include <game/RenderWare.h>
#include <game/CHud.h>

#define VAR_CFont_Scale 0xC71A64
#define VAR_CFont_Color 0xC71A60

#define VAR_CFont_Slant         0xC71A6C
#define VAR_CFont_SlantRefPoint 0xC71A70

#define VAR_CFont_Justify          0xC71A78
#define VAR_CFont_RightJustifyWrap 0xC71A90

#define VAR_CFont_CenterAlign 0xC71A79
#define VAR_CFont_RightAlign  0xC71A7A

#define VAR_CFont_Background            0xC71A7B
#define VAR_CFont_BackgroundIncludeWrap 0xC71A7C
#define VAR_CFont_BackgroundColor       0xC71A84

#define VAR_CFont_Proportional 0xC71A7D
#define VAR_CFont_Wrapx        0xC71A88
#define VAR_CFont_CentreSize   0xC71A8C

#define VAR_CFont_FontStyle 0xC71A95
#define VAR_CFont_TextureID 0xC71A94
#define VAR_CFont_Shadow    0xC71A96

#define VAR_CFont_DropColor 0xC71A97
#define VAR_CFont_Edge      0xC71A9B

class CFontSA
{
public:
    static void PrintChar(float x, float y, char character);
    static void PrintString(float x, float y, const char* text);
    static void PrintStringFromBottom(float x, float y, const char* text);

    static void SetScale(float w, float h);
    static void SetScale(const CVector2D& scale);
    static void SetScaleForCurrentLanguage(float w, float h);

    static void SetSlantRefPoint(float x, float y);
    static void SetSlant(float slant);

    static void SetColor(const RwColor& color);
    static void SetDropColor(const RwColor& color);

    static void SetFontStyle(const eFontStyle& style);
    static void SetCentreSize(float size);

    static void SetWrapX(float wrapx);
    static void SetRightJustifyWrap(float wrap);

    static void SetDropShadowPosition(std::int16_t offset);
    static void SetEdge(std::int16_t edgeSize);            // outline

    static void SetProportional(bool enable);

    static void SetBackground(bool enable, bool includeWrap);
    static void SetBackgroundColor(const RwColor& color);

    static void SetJustify(bool enable);
    static void SetOrientation(const eFontAlignment& alignment);

    static float        GetStringWidth(const char* string, bool spaces, bool scriptValues = false);
    static std::int16_t GetNumberLines(float x, float y, const char* text);
    static float        GetFontHeight(float scaleY);

    static CVector2D GetScale() { return *reinterpret_cast<CVector2D*>(VAR_CFont_Scale); }
    static RwColor   GetColor() { return *reinterpret_cast<RwColor*>(VAR_CFont_Color); }
    static RwColor   GetDropColor() { return *reinterpret_cast<RwColor*>(VAR_CFont_DropColor); }

    static float     GetSlant() { return *reinterpret_cast<float*>(VAR_CFont_Slant); }
    static CVector2D GetSlantRefPoint() { return *reinterpret_cast<CVector2D*>(VAR_CFont_SlantRefPoint); }

    static bool           IsFontJustify() { return *reinterpret_cast<bool*>(VAR_CFont_Justify); }
    static eFontAlignment GetOrientation();

    static bool    IsBackgroundEnabled() { return *reinterpret_cast<bool*>(VAR_CFont_Background); }
    static bool    IsBackgroundWrapIncluded() { return *reinterpret_cast<bool*>(VAR_CFont_BackgroundIncludeWrap); }
    static RwColor GetBackgroundColor() { return *reinterpret_cast<RwColor*>(VAR_CFont_BackgroundColor); }

    static bool IsProportional() { return *reinterpret_cast<bool*>(VAR_CFont_Proportional); }

    static float GetWrapX() { return *reinterpret_cast<float*>(VAR_CFont_Wrapx); }
    static float GetCentreSize() { return *reinterpret_cast<float*>(VAR_CFont_CentreSize); }
    static float GetRightJustifyWrap() { return *reinterpret_cast<float*>(VAR_CFont_RightJustifyWrap); }

    static eFontStyle GetFontStyle();

    static float GetEdge() { return static_cast<float>(*reinterpret_cast<std::uint8_t*>(VAR_CFont_Edge)); }
    static float GetDropdownShadow() { return static_cast<float>(*reinterpret_cast<std::uint8_t*>(VAR_CFont_Shadow)); }

    static bool GetProportional() { return *reinterpret_cast<bool*>(VAR_CFont_Proportional); }
};
