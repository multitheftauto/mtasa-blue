/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFontSA.cpp
 *  PURPOSE:     Font class layer
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CFontSA.h"

void CFontSA::PrintChar(float x, float y, char character)
{
    // Call CFont::PrintChar
    ((void(_cdecl*)(float, float, char))0x718A10)(x, y, character);
}

void CFontSA::PrintString(float x, float y, const char* text)
{
    // Call CFont::PrintString
    ((void(__cdecl*)(float, float, const char*))0x71A700)(x, y, text);
}

void CFontSA::PrintStringFromBottom(float x, float y, const char* text)
{
    // Call CFont::PrintStringFromBottom
    ((void(__cdecl*)(float, float, const char*))0x71A820)(x, y, text);
}

void CFontSA::SetScale(float w, float h)
{
    // Call CFont::SetScale
    ((void(__cdecl*)(float, float))0x719380)(w, h);
}

void CFontSA::SetScale(const CVector2D& scale)
{
    SetScale(scale.fX, scale.fY);
}

void CFontSA::SetScaleForCurrentLanguage(float w, float h)
{
    // Call CFont::SetScaleForCurrentLanguage
    ((void(__cdecl*)(float, float))0x7193A0)(w, h);
}

void CFontSA::SetSlantRefPoint(float x, float y)
{
    // Call CFont::SetSlantRefPoint
    ((void(__cdecl*)(float, float))0x719400)(x, y);
}

void CFontSA::SetSlant(float slant)
{
    // Call CFont::SetSlant
    ((void(__cdecl*)(float))0x719420)(slant);
}

void CFontSA::SetColor(const RwColor& color)
{
    // Call CFont::SetColor
    ((void(__cdecl*)(RwColor))0x719430)(color);
}

void CFontSA::SetDropColor(const RwColor& color)
{
    // Call CFont::SetDropColor
    ((void(__cdecl*)(RwColor))0x719510)(color);
}

void CFontSA::SetFontStyle(const eFontStyle& style)
{
    // Call CFont::SetFontStyle
    ((void(__cdecl*)(eFontStyle))0x719490)(style);
}

void CFontSA::SetWrapX(float wrapx)
{
    // Call CFont::SetWrapx
    ((void(__cdecl*)(float))0x7194D0)(wrapx);
}

void CFontSA::SetRightJustifyWrap(float wrap)
{
    // Call CFont::SetRightJustifyWrap
    ((void(__cdecl*)(float))0x7194F0)(wrap);
}

void CFontSA::SetCentreSize(float size)
{
    // Call CFont::SetCentreSize
    ((void(__cdecl*)(float))0x7194E0)(size);
}

void CFontSA::SetDropShadowPosition(std::int16_t offset)
{
    // Call CFont::SetDropShadowPosition
    ((void(__cdecl*)(std::int16_t))0x719570)(offset);
}

void CFontSA::SetEdge(std::int16_t edgeSize)
{
    // Call CFont::SetEdge
    ((void(__cdecl*)(std::int16_t))0x719590)(edgeSize);
}

void CFontSA::SetProportional(bool enable)
{
    // Call CFont::SetProportional
    ((void(__cdecl*)(bool))0x7195B0)(enable);
}

void CFontSA::SetBackground(bool enable, bool includeWrap)
{
    // Call CFont::SetBackground
    ((void(__cdecl*)(bool, bool))0x7195C0)(enable, includeWrap);
}

void CFontSA::SetBackgroundColor(const RwColor& color)
{
    // Call CFont::SetBackgroundColor
    ((void(__cdecl*)(RwColor))0x7195E0)(color);
}

void CFontSA::SetJustify(bool enable)
{
    // Call CFont::SetJustify
    ((void(__cdecl*)(bool))0x719600)(enable);
}

void CFontSA::SetOrientation(const eFontAlignment& alignment)
{
    // Call CFont::SetOrientation
    ((void(__cdecl*)(eFontAlignment))0x719610)(alignment);
}

float CFontSA::GetStringWidth(const char* string, bool spaces, bool scriptValues)
{
    // Call CFont::GetStringWidth
    return ((float(__cdecl*)(const char*, bool, bool))0x71A0E0)(string, spaces, scriptValues);
}

std::int16_t CFontSA::GetNumberLines(float x, float y, const char* text)
{
    // Call CFont::GetNumberLines
    return ((std::int16_t(__cdecl*)(float, float, const char*))0x71A5E0)(x, y, text);
}

float CFontSA::GetFontHeight(float scaleY)
{
    return scaleY * 16.0f + scaleY * 2.0f;
}

eFontAlignment CFontSA::GetOrientation()
{
    bool centerAlign = *reinterpret_cast<bool*>(VAR_CFont_CenterAlign);
    bool rightAlign = *reinterpret_cast<bool*>(VAR_CFont_RightAlign);

    if (centerAlign)
        return eFontAlignment::ALIGN_CENTER;
    else if (rightAlign)
        return eFontAlignment::ALIGN_RIGHT;

    return eFontAlignment::ALIGN_LEFT;
}

eFontStyle CFontSA::GetFontStyle()
{
    std::uint8_t style = *reinterpret_cast<std::uint8_t*>(VAR_CFont_FontStyle);

    switch (style)
    {
        case 0:
            return *reinterpret_cast<eFontStyle*>(VAR_CFont_TextureID);
        case 1:
            return eFontStyle::FONT_PRICEDOWN;
        case 2:
            return eFontStyle::FONT_MENU;
        default:
            break;
    }

    return eFontStyle::FONT_MENU;
}

using SharedUtil::CalcMTASAPath;
using SharedUtil::PathJoin;

static bool HOOK_CTxdStore_LoadTxd_Fonts(int slot, const char* path)
{
    // Check if custom or high-definition font TXD is available in MTA data directory
    static SString mtaFontsPath = CalcMTASAPath(PathJoin("MTA", "data", "fonts.txd"));
    if (FileExists(mtaFontsPath))
        path = *mtaFontsPath;

    return reinterpret_cast<bool (*)(int, const char*)>(0x7320B0)(slot, path);
}

static void HOOK_CFont_Initialise_Post()
{
    // Call original readFontsDat (0x7187C0)
    ((void (*)())0x7187C0)();

    // Automatically apply crisp high-definition bilinear filtering to font rasters
    CFontSA::ApplyHDTextureFiltering();
}

void CFontSA::StaticSetHooks()
{
    // Hook CTxdStore::LoadTxd call inside CFont::Initialise at 0x5BA6A4 to support HD font TXDs
    HookInstallCall(0x5BA6A4, reinterpret_cast<DWORD>(HOOK_CTxdStore_LoadTxd_Fonts));

    // Hook CFont::Initialise (0x5BA690) at call to readFontsDat (0x5BA6E5)
    HookInstallCall(0x5BA6E5, reinterpret_cast<DWORD>(HOOK_CFont_Initialise_Post));
}

void CFontSA::ApplyHDTextureFiltering()
{
    // CFont::Sprite[0] (0xC71AD0) is font2 (Pricedown, Subtitles, Challenges)
    // CFont::Sprite[1] (0xC71AD4) is font1 (Menu, Secondary fonts)
    RwTexture** fontTexture2 = reinterpret_cast<RwTexture**>(0xC71AD0);
    RwTexture** fontTexture1 = reinterpret_cast<RwTexture**>(0xC71AD4);

    if (fontTexture2 && *fontTexture2)
    {
        // Enforce high-quality linear filtering (0x02 = rwFILTERLINEAR) on the font raster
        (*fontTexture2)->flags = ((*fontTexture2)->flags & ~0xFF) | 0x02;
    }

    if (fontTexture1 && *fontTexture1)
    {
        // Enforce high-quality linear filtering (0x02 = rwFILTERLINEAR) on the font raster
        (*fontTexture1)->flags = ((*fontTexture1)->flags & ~0xFF) | 0x02;
    }
}
