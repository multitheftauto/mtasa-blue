/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHudSAPlacement.h
 *  PURPOSE:     Geometry and colour helpers for the placement of the HUD
 *               components the game draws itself (the radar and the crosshair)
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace HudPlacement
{
    constexpr float RADAR_LEFT = 36.0f;
    constexpr float RADAR_RIGHT = 138.0f;
    constexpr float RADAR_BOTTOM = 24.0f;
    constexpr float RADAR_TOP = 108.0f;

    struct SRect
    {
        float x{0.0f};
        float y{0.0f};
        float width{0.0f};
        float height{0.0f};
    };

    struct SVertexTransform
    {
        bool  bActive{false};
        float fScaleX{1.0f};
        float fScaleY{1.0f};
        float fOffsetX{0.0f};
        float fOffsetY{0.0f};
    };

    inline SRect GetDefaultRect(float fStretchX, float fStretchY, float fScreenHeight) noexcept
    {
        SRect rect;
        rect.x = RADAR_LEFT * fStretchX;
        rect.y = fScreenHeight - RADAR_TOP * fStretchY;
        rect.width = (RADAR_RIGHT - RADAR_LEFT) * fStretchX;
        rect.height = (RADAR_TOP - RADAR_BOTTOM) * fStretchY;
        return rect;
    }

    inline SRect GetCrosshairDefaultRect(float fScreenWidth, float fScreenHeight) noexcept
    {
        SRect rect;
        rect.x = fScreenWidth * 0.5f;
        rect.y = fScreenHeight * 0.5f;
        rect.width = fScreenWidth;
        rect.height = fScreenHeight;
        return rect;
    }

    inline SVertexTransform BuildTransform(const SRect& defaultRect, bool bUseCustomPosition, bool bUseCustomSize, const SRect& customRect) noexcept
    {
        SVertexTransform transform;

        if ((!bUseCustomPosition && !bUseCustomSize) || defaultRect.width <= 0.0f || defaultRect.height <= 0.0f)
            return transform;

        const float fWidth = bUseCustomSize ? customRect.width : defaultRect.width;
        const float fHeight = bUseCustomSize ? customRect.height : defaultRect.height;
        if (fWidth <= 0.0f || fHeight <= 0.0f)
            return transform;

        const float fX = bUseCustomPosition ? customRect.x : defaultRect.x;
        const float fY = bUseCustomPosition ? customRect.y : defaultRect.y;

        transform.bActive = true;
        transform.fScaleX = fWidth / defaultRect.width;
        transform.fScaleY = fHeight / defaultRect.height;
        transform.fOffsetX = fX - defaultRect.x * transform.fScaleX;
        transform.fOffsetY = fY - defaultRect.y * transform.fScaleY;
        return transform;
    }

    inline void ApplyToVertex(const SVertexTransform& transform, float& fX, float& fY) noexcept
    {
        fX = fX * transform.fScaleX + transform.fOffsetX;
        fY = fY * transform.fScaleY + transform.fOffsetY;
    }

    struct SRgba
    {
        std::uint8_t r{255};
        std::uint8_t g{255};
        std::uint8_t b{255};
        std::uint8_t a{255};
    };

    struct SVertexPlacement
    {
        SVertexTransform transform;
        SRgba            color;
        bool             bUseColor{false};
    };

    inline std::uint32_t ModulateArgb(std::uint32_t uiArgb, const SRgba& tint) noexcept
    {
        const auto ucMultiply = [](std::uint32_t uiChannel, std::uint8_t ucTint) { return (uiChannel * ucTint + 127) / 255; };

        const std::uint32_t uiAlpha = ucMultiply((uiArgb >> 24) & 0xFF, tint.a);
        const std::uint32_t uiRed = ucMultiply((uiArgb >> 16) & 0xFF, tint.r);
        const std::uint32_t uiGreen = ucMultiply((uiArgb >> 8) & 0xFF, tint.g);
        const std::uint32_t uiBlue = ucMultiply(uiArgb & 0xFF, tint.b);

        return (uiAlpha << 24) | (uiRed << 16) | (uiGreen << 8) | uiBlue;
    }
}
