/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaDrawingDefs.cpp
 *  PURPOSE:     Lua drawing definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include <optional>
#include <variant>
#include <utility>
#include <optional>

class CLuaDrawingDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(DxDrawLine);
    LUA_DECLARE(DxDrawLine3D);
    LUA_DECLARE(DxDrawMaterialLine3D);
    LUA_DECLARE(DxDrawMaterialSectionLine3D);
    LUA_DECLARE(DxDrawText);
    LUA_DECLARE(DxDrawRectangle);
    LUA_DECLARE(DxDrawCircle);
    LUA_DECLARE(DxDrawImage);
    LUA_DECLARE(DxDrawImageSection);
    LUA_DECLARE(DxDrawPrimitive);
    LUA_DECLARE(DxDrawMaterialPrimitive);
    LUA_DECLARE(DxDrawPrimitive3D);
    LUA_DECLARE(DxDrawMaterialPrimitive3D);

    static CVector2D OOP_DxGetTextSize(
        // font can be called with a std::nullopt to grab the FONT_DEFAULT, see DxGetTextSize
        std::variant<CClientDxFont*, eFontType> font, const std::string text, const std::optional<float> optWidth,
        const std::optional<std::variant<CVector2D, float>> optScaleXY, const std::optional<bool> optWordBreak, const std::optional<bool> optColorCoded);

    static inline CLuaMultiReturn<float, float> DxGetTextSize(std::string text, std::optional<float> optWidth,
                                                              std::optional<std::variant<CVector2D, float>>          optScaleXY,
                                                              std::optional<std::variant<CClientDxFont*, eFontType>> optFont, std::optional<bool> optWordBreak,
                                                              std::optional<bool> optColorCoded)
    {
        const auto size = OOP_DxGetTextSize(std::move(optFont.value_or(FONT_DEFAULT)), std::move(text), std::move(optWidth), std::move(optScaleXY),
                                            std::move(optWordBreak), std::move(optColorCoded));

        return {size.fX, size.fY};
    };

    LUA_DECLARE_OOP(DxGetTextWidth);
    LUA_DECLARE_OOP(DxGetFontHeight);
    LUA_DECLARE(DxCreateFont);
    LUA_DECLARE(DxCreateTexture);
    LUA_DECLARE(DxCreateShader);
    LUA_DECLARE(DxCreateRenderTarget);
    LUA_DECLARE(DxCreateScreenSource);
    LUA_DECLARE(DxGetMaterialSize);
    LUA_DECLARE(DxSetShaderValue);
    LUA_DECLARE(DxSetShaderTessellation);
    LUA_DECLARE(DxSetShaderTransform);
    LUA_DECLARE(DxSetRenderTarget);
    LUA_DECLARE(DxUpdateScreenSource);
    LUA_DECLARE(DxGetStatus);
    LUA_DECLARE(DxSetTestMode);
    LUA_DECLARE(DxGetTexturePixels);
    LUA_DECLARE(DxSetTexturePixels);
    LUA_DECLARE(DxGetPixelsSize);
    LUA_DECLARE(DxGetPixelsFormat);
    LUA_DECLARE(DxConvertPixels);
    LUA_DECLARE(DxGetPixelColor);
    LUA_DECLARE(DxSetPixelColor);
    LUA_DECLARE(DxSetBlendMode);
    LUA_DECLARE(DxGetBlendMode);
    LUA_DECLARE(DxSetAspectRatioAdjustmentEnabled);
    LUA_DECLARE(DxIsAspectRatioAdjustmentEnabled);
    LUA_DECLARE(DxSetTextureEdge);

    static bool DxDrawWiredSphere(lua_State* const luaVM, const CVector position, const float radius, const std::optional<SColor> color,
                                  const std::optional<float> lineWidth, const std::optional<unsigned int> iterations);

    static bool DxDrawModel3D(std::uint32_t modelID, CVector position, CVector rotation, const std::optional<CVector> scale, const std::optional<float> lighting);

private:
    static void AddDxMaterialClass(lua_State* luaVM);
    static void AddDxTextureClass(lua_State* luaVM);
    static void AddDxFontClass(lua_State* luaVM);
    static void AddDxShaderClass(lua_State* luaVM);
    static void AddDxScreenSourceClass(lua_State* luaVM);
    static void AddDxRenderTargetClass(lua_State* luaVM);
};
