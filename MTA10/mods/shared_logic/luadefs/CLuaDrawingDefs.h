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
#include "CLuaDefs.h"

class CLuaDrawingDefs : public CLuaDefs
{
public:
    static void LoadFunctions ( void );
    static void AddClass ( lua_State* luaVM );

    LUA_DECLARE ( DxDrawLine );
    LUA_DECLARE ( DxDrawLine3D );
    LUA_DECLARE ( DxDrawMaterialLine3D );
    LUA_DECLARE ( DxDrawMaterialSectionLine3D );
    LUA_DECLARE ( DxDrawText );
    LUA_DECLARE ( DxDrawRectangle );
    LUA_DECLARE ( DxDrawImage );
    LUA_DECLARE ( DxDrawImageSection );
    LUA_DECLARE_OOP ( DxGetTextWidth );
    LUA_DECLARE_OOP ( DxGetFontHeight );
    LUA_DECLARE ( DxCreateFont );
    LUA_DECLARE ( DxCreateTexture );
    LUA_DECLARE ( DxCreateShader );
    LUA_DECLARE ( DxCreateRenderTarget );
    LUA_DECLARE ( DxCreateScreenSource );
    LUA_DECLARE ( DxGetMaterialSize );
    LUA_DECLARE ( DxSetShaderValue );
    LUA_DECLARE ( DxSetShaderTessellation );
    LUA_DECLARE ( DxSetShaderTransform );
    LUA_DECLARE ( DxSetRenderTarget );
    LUA_DECLARE ( DxUpdateScreenSource );
    LUA_DECLARE ( DxGetStatus );
    LUA_DECLARE ( DxSetTestMode );
    LUA_DECLARE ( DxGetTexturePixels );
    LUA_DECLARE ( DxSetTexturePixels );
    LUA_DECLARE ( DxGetPixelsSize );
    LUA_DECLARE ( DxGetPixelsFormat );
    LUA_DECLARE ( DxConvertPixels );
    LUA_DECLARE ( DxGetPixelColor );
    LUA_DECLARE ( DxSetPixelColor );
    LUA_DECLARE ( DxSetBlendMode );
    LUA_DECLARE ( DxGetBlendMode );
    LUA_DECLARE ( DxSetAspectRatioAdjustmentEnabled );
    LUA_DECLARE ( DxIsAspectRatioAdjustmentEnabled );
    LUA_DECLARE ( DxSetTextureEdge );

private:
    static void AddDxMaterialClass ( lua_State* luaVM );
    static void AddDxTextureClass ( lua_State* luaVM );
    static void AddDxFontClass ( lua_State* luaVM );
    static void AddDxShaderClass ( lua_State* luaVM );
    static void AddDxScreenSourceClass ( lua_State* luaVM );
    static void AddDxRenderTargetClass ( lua_State* luaVM );
};
