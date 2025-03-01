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

#include "StdInc.h"
#include "CLuaDefs.h"
#include "lua/CLuaFunctionParser.h"

#include <SharedUtil.SysInfo.h>
#include <SharedUtil.SysInfo.hpp>

#define MIN_CLIENT_REQ_DXSETRENDERTARGET_CALL_RESTRICTIONS "1.3.0-9.04431"
extern bool g_bAllowAspectRatioAdjustment;

void CLuaDrawingDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"dxDrawLine", DxDrawLine},
        {"dxDrawMaterialLine3D", DxDrawMaterialLine3D},
        {"dxDrawMaterialSectionLine3D", DxDrawMaterialSectionLine3D},
        {"dxDrawLine3D", DxDrawLine3D},
        {"dxDrawText", DxDrawText},
        {"dxDrawRectangle", DxDrawRectangle},
        {"dxDrawCircle", DxDrawCircle},
        {"dxDrawImage", DxDrawImage},
        {"dxDrawImageSection", DxDrawImageSection},
        {"dxDrawPrimitive", DxDrawPrimitive},
        {"dxDrawPrimitive3D", DxDrawPrimitive3D},
        {"dxDrawMaterialPrimitive", DxDrawMaterialPrimitive},
        {"dxDrawMaterialPrimitive3D", DxDrawMaterialPrimitive3D},
        {"dxDrawWiredSphere", ArgumentParser<DxDrawWiredSphere>},
        {"dxDrawModel3D", ArgumentParser<DxDrawModel3D>},
        {"dxGetTextWidth", DxGetTextWidth},
        {"dxGetTextSize", ArgumentParser<DxGetTextSize>},
        {"dxGetFontHeight", DxGetFontHeight},
        {"dxCreateFont", DxCreateFont},
        {"dxCreateTexture", DxCreateTexture},
        {"dxCreateShader", DxCreateShader},
        {"dxCreateRenderTarget", DxCreateRenderTarget},
        {"dxCreateScreenSource", DxCreateScreenSource},
        {"dxGetMaterialSize", DxGetMaterialSize},
        {"dxSetShaderValue", DxSetShaderValue},
        {"dxSetShaderTessellation", DxSetShaderTessellation},
        {"dxSetShaderTransform", DxSetShaderTransform},
        {"dxSetRenderTarget", DxSetRenderTarget},
        {"dxUpdateScreenSource", DxUpdateScreenSource},
        {"dxGetStatus", DxGetStatus},
        {"dxSetTestMode", DxSetTestMode},
        {"dxGetTexturePixels", DxGetTexturePixels},
        {"dxSetTexturePixels", DxSetTexturePixels},
        {"dxGetPixelsSize", DxGetPixelsSize},
        {"dxGetPixelsFormat", DxGetPixelsFormat},
        {"dxConvertPixels", DxConvertPixels},
        {"dxGetPixelColor", DxGetPixelColor},
        {"dxSetPixelColor", DxSetPixelColor},
        {"dxSetBlendMode", DxSetBlendMode},
        {"dxGetBlendMode", DxGetBlendMode},
        {"dxSetAspectRatioAdjustmentEnabled", DxSetAspectRatioAdjustmentEnabled},
        {"dxIsAspectRatioAdjustmentEnabled", DxIsAspectRatioAdjustmentEnabled},
        {"dxSetTextureEdge", DxSetTextureEdge},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaDrawingDefs::AddClass(lua_State* luaVM)
{
    AddDxMaterialClass(luaVM);
    AddDxTextureClass(luaVM);
    AddDxFontClass(luaVM);
    AddDxShaderClass(luaVM);
    AddDxScreenSourceClass(luaVM);
    AddDxRenderTargetClass(luaVM);
}

void CLuaDrawingDefs::AddDxMaterialClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "getSize", "dxGetMaterialSize");

    lua_registerclass(luaVM, "DxMaterial", "Element");
}

void CLuaDrawingDefs::AddDxTextureClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "dxCreateTexture");

    lua_classfunction(luaVM, "setEdge", "dxSetTextureEdge");
    lua_classfunction(luaVM, "setPixels", "dxSetTexturePixels");
    lua_classfunction(luaVM, "getPixels", "dxGetTexturePixels");

    lua_registerclass(luaVM, "DxTexture", "DxMaterial");
}

void CLuaDrawingDefs::AddDxFontClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "dxCreateFont");
    lua_classfunction(luaVM, "destroy", "destroyElement");

    lua_classfunction(luaVM, "getHeight", OOP_DxGetFontHeight);
    lua_classfunction(luaVM, "getTextWidth", OOP_DxGetTextWidth);
    lua_classfunction(luaVM, "getTextSize", ArgumentParser<OOP_DxGetTextSize>);

    lua_registerclass(luaVM, "DxFont");
}

void CLuaDrawingDefs::AddDxShaderClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "dxCreateShader");
    lua_classfunction(luaVM, "applyToWorldTexture", "engineApplyShaderToWorldTexture");
    lua_classfunction(luaVM, "removeFromWorldTexture", "engineRemoveShaderFromWorldTexture");

    lua_classfunction(luaVM, "setValue", "dxSetShaderValue");
    lua_classfunction(luaVM, "setTessellation", "dxSetShaderTessellation");
    lua_classfunction(luaVM, "setTransform", "dxSetShaderTransform");

    // lua_classvariable ( luaVM, "value", CLuaOOPDefs::SetShaderValue, NULL); // .value["param"] = value
    lua_classvariable(luaVM, "tessellation", "dxSetShaderTessellation", NULL);

    lua_registerclass(luaVM, "DxShader", "DxMaterial");
}

void CLuaDrawingDefs::AddDxScreenSourceClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "dxCreateScreenSource");
    lua_classfunction(luaVM, "update", "dxUpdateScreenSource");

    lua_registerclass(luaVM, "DxScreenSource", "DxTexture");
}

void CLuaDrawingDefs::AddDxRenderTargetClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "dxCreateRenderTarget");
    lua_classfunction(luaVM, "setAsTarget", "dxSetRenderTarget");

    lua_registerclass(luaVM, "DxRenderTarget", "DxTexture");
}

int CLuaDrawingDefs::DxDrawLine(lua_State* luaVM)
{
    //  bool dxDrawLine ( int startX, int startY, int endX, int endY, int color, [float width=1, bool postGUI=false] )
    CVector2D vecStart;
    CVector2D vecEnd;
    SColor    color;
    float     fWidth;
    bool      bPostGUI;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecStart);
    argStream.ReadVector2D(vecEnd);
    argStream.ReadColor(color, 0xFFFFFFFF);
    argStream.ReadNumber(fWidth, 1);
    argStream.ReadBool(bPostGUI, false);

    if (!argStream.HasErrors())
    {
        g_pCore->GetGraphics()->DrawLineQueued(vecStart.fX, vecStart.fY, vecEnd.fX, vecEnd.fY, fWidth, color, bPostGUI);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawLine3D(lua_State* luaVM)
{
    //  bool dxDrawLine3D ( float startX, float startY, float startZ, float endX, float endY, float endZ, int color[, int width, bool postGUI ] )
    CVector vecBegin;
    CVector vecEnd;
    SColor  color;
    float   fWidth;
    eRenderStage renderStage{eRenderStage::POST_FX};

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecBegin);
    argStream.ReadVector3D(vecEnd);
    argStream.ReadColor(color, 0xFFFFFFFF);
    argStream.ReadNumber(fWidth, 1);
    if (argStream.NextIsBool())
        renderStage = argStream.ReadBool() ? eRenderStage::POST_GUI : eRenderStage::POST_FX;
    else
        argStream.ReadIfNextIsEnumString(renderStage, eRenderStage::POST_FX);

    if (!argStream.HasErrors())
    {
        g_pCore->GetGraphics()->DrawLine3DQueued(vecBegin, vecEnd, fWidth, color, renderStage);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawMaterialLine3D(lua_State* luaVM)
{
    //  bool dxDrawMaterialLine3D ( float startX, float startY, float startZ, float endX, float endY, float endZ, [bool flipUV,] element material, int width [,
    //  int color = white,
    //                          float faceX, float faceY, float faceZ ] )
    CVector          vecBegin;
    CVector          vecEnd;
    bool             bFlipUV;
    CClientMaterial* pMaterial;
    float            fWidth;
    SColor           color;
    CVector          vecFaceToward;
    bool             bUseFaceToward = false;
    eRenderStage renderStage{eRenderStage::POST_FX};

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecBegin);
    argStream.ReadVector3D(vecEnd);
    argStream.ReadIfNextIsBool(bFlipUV, false);
    argStream.ReadUserData(pMaterial);
    argStream.ReadNumber(fWidth);
    argStream.ReadColor(color, 0xFFFFFFFF);
    if (argStream.NextIsBool())
        renderStage = argStream.ReadBool() ? eRenderStage::POST_GUI : eRenderStage::POST_FX;
    else
        argStream.ReadIfNextIsEnumString(renderStage, eRenderStage::POST_FX);

    if (argStream.NextIsVector3D())
    {
        argStream.ReadVector3D(vecFaceToward);
        bUseFaceToward = true;
    }

    if (!argStream.HasErrors())
    {
        g_pCore->GetGraphics()->DrawMaterialLine3DQueued(vecBegin, vecEnd, fWidth, color, pMaterial->GetMaterialItem(), 0, 0, 1, 1, true, bFlipUV,
                                                         bUseFaceToward, vecFaceToward, renderStage);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawMaterialSectionLine3D(lua_State* luaVM)
{
    //  bool dxDrawMaterialSectionLine3D ( float startX, float startY, float startZ, float endX, float endY, float endZ, float u, float v, float usize, float
    //  vsize,
    //                                  [bool flipUV,] element material, int width, [int color = white, float faceX, float faceY, float faceZ ] )
    CVector          vecBegin;
    CVector          vecEnd;
    CVector2D        vecSectionPos;
    CVector2D        vecSectionSize;
    bool             bFlipUV;
    CClientMaterial* pMaterial;
    float            fWidth;
    SColor           color;
    CVector          vecFaceToward;
    bool             bUseFaceToward = false;
    eRenderStage renderStage{eRenderStage::POST_FX};

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecBegin);
    argStream.ReadVector3D(vecEnd);
    argStream.ReadVector2D(vecSectionPos);
    argStream.ReadVector2D(vecSectionSize);
    argStream.ReadIfNextIsBool(bFlipUV, false);
    argStream.ReadUserData(pMaterial);
    argStream.ReadNumber(fWidth);
    argStream.ReadColor(color, 0xFFFFFFFF);
    if (argStream.NextIsBool())
        renderStage = argStream.ReadBool() ? eRenderStage::POST_GUI : eRenderStage::POST_FX;
    else
        argStream.ReadIfNextIsEnumString(renderStage, eRenderStage::POST_FX);

    if (argStream.NextIsVector3D())
    {
        argStream.ReadVector3D(vecFaceToward);
        bUseFaceToward = true;
    }

    if (!argStream.HasErrors())
    {
        g_pCore->GetGraphics()->DrawMaterialLine3DQueued(vecBegin, vecEnd, fWidth, color, pMaterial->GetMaterialItem(), vecSectionPos.fX, vecSectionPos.fY,
                                                         vecSectionSize.fX, vecSectionSize.fY, false, bFlipUV, bUseFaceToward, vecFaceToward, renderStage);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawText(lua_State* luaVM)
{
    //  bool dxDrawText ( string text, float left, float top [, float right=left, float bottom=top, int color=white, float scale=1, mixed font="default",
    //      string alignX="left", string alignY="top", bool clip=false, bool wordBreak=false, bool postGUI=false, bool colorCoded=false, bool
    //      subPixelPositioning=false, float rotation=0, float rotationCenterX=(left+right)/2, float rotationCenterY=(top+bottom)/2] )
    SString            strText;
    CVector2D          vecTopLeft;
    CVector2D          vecBottomRight;
    SColor             color;
    float              fScaleX;
    float              fScaleY;
    eFontType          fontType;
    CClientDxFont*     pDxFontElement;
    eDXHorizontalAlign alignX;
    eDXVerticalAlign   alignY;
    bool               bClip;
    bool               bWordBreak;
    bool               bPostGUI;
    bool               bColorCoded;
    bool               bSubPixelPositioning;
    float              fRotation;
    CVector2D          vecRotationOrigin;
    float              fLineHeight;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strText);
    argStream.ReadVector2D(vecTopLeft);
    if (argStream.NextIsUserDataOfType<CLuaVector2D>())
        argStream.ReadVector2D(vecBottomRight);
    else
    {
        argStream.ReadNumber(vecBottomRight.fX, vecTopLeft.fX);
        argStream.ReadNumber(vecBottomRight.fY, vecTopLeft.fY);
    }
    argStream.ReadColor(color, 0xFFFFFFFF);
    if (argStream.NextIsUserDataOfType<CLuaVector2D>())
    {
        CVector2D vecScale;
        argStream.ReadVector2D(vecScale);
        fScaleX = vecScale.fX;
        fScaleY = vecScale.fY;
    }
    else
    {
        argStream.ReadNumber(fScaleX, 1);
        if (argStream.NextIsNumber())
            argStream.ReadNumber(fScaleY);
        else
            fScaleY = fScaleX;
    }
    MixedReadDxFontString(argStream, fontType, FONT_DEFAULT, pDxFontElement);
    argStream.ReadEnumString(alignX, DX_ALIGN_LEFT);
    argStream.ReadEnumString(alignY, DX_ALIGN_TOP);
    argStream.ReadBool(bClip, false);
    argStream.ReadBool(bWordBreak, false);
    argStream.ReadBool(bPostGUI, false);
    argStream.ReadBool(bColorCoded, false);
    argStream.ReadBool(bSubPixelPositioning, false);
    argStream.ReadNumber(fRotation, 0);
    argStream.ReadVector2D(vecRotationOrigin, CVector2D((vecTopLeft.fX + vecBottomRight.fX) * 0.5f, (vecTopLeft.fY + vecBottomRight.fY) * 0.5f));
    argStream.ReadNumber(fLineHeight, 0);

    if (!argStream.HasErrors())
    {
        // Get DX font
        ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont(fontType, pDxFontElement);

        // Make format flag
        ulong ulFormat = alignX | alignY;
        // if ( ulFormat & DT_BOTTOM ) ulFormat |= DT_SINGLELINE;        MS says we should do this. Nobody tells me what to do.
        if (bWordBreak)
            ulFormat |= DT_WORDBREAK;
        if (!bClip)
            ulFormat |= DT_NOCLIP;

        g_pCore->GetGraphics()->DrawStringQueued(vecTopLeft.fX, vecTopLeft.fY, vecBottomRight.fX, vecBottomRight.fY, color, strText, fScaleX, fScaleY, ulFormat,
                                                 pD3DXFont, bPostGUI, bColorCoded, bSubPixelPositioning, fRotation, vecRotationOrigin.fX, vecRotationOrigin.fY,
                                                 fLineHeight);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawRectangle(lua_State* luaVM)
{
    //  bool dxDrawRectangle ( float startX, float startY, float width, float height [, int color = white, bool postGUI = false, bool subPixelPositioning=false]
    //  )
    CVector2D vecPosition;
    CVector2D vecSize;
    SColor    color;
    bool      bPostGUI;
    bool      bSubPixelPositioning;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadVector2D(vecSize);
    argStream.ReadColor(color, 0xFFFFFFFF);
    argStream.ReadBool(bPostGUI, false);
    argStream.ReadBool(bSubPixelPositioning, false);

    if (!argStream.HasErrors())
    {
        g_pCore->GetGraphics()->DrawRectQueued(vecPosition.fX, vecPosition.fY, vecSize.fX, vecSize.fY, color, bPostGUI, bSubPixelPositioning);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawCircle(lua_State* luaVM)
{
    CVector2D vecPosition;
    float     fRadius;
    float     fStartAngle;
    float     fStopAngle;
    SColor    color;
    SColor    colorCenter;
    short     siSegments;
    float     fRatio;
    bool      bPostGUI;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadNumber(fRadius);
    argStream.ReadNumber(fStartAngle, 0);
    argStream.ReadNumber(fStopAngle, 360);
    argStream.ReadColor(color, 0xFFFFFFFF);
    argStream.ReadColor(colorCenter, color);
    argStream.ReadNumber(siSegments, 32);
    argStream.ReadNumber(fRatio, 1);
    argStream.ReadBool(bPostGUI, false);

    if (!argStream.HasErrors())
    {
        const short siMinimumSegments = 3;
        const short siMaximumSegments = 1024;
        if (siSegments >= siMinimumSegments && siSegments <= siMaximumSegments)
        {
            const float fMinimumRatio = 0;
            const float fMaximumRatio = 100;
            if (fRatio > fMinimumRatio && fRatio <= fMaximumRatio)
            {
                if (fRadius > 0 && fStartAngle != fStopAngle)
                {
                    if (fStopAngle < fStartAngle)
                        std::swap(fStopAngle, fStartAngle);

                    // Clamp the angle, so we never draw more than 360 degrees
                    if (fStartAngle + 360.0f < fStopAngle)
                        fStopAngle = fStartAngle + 360.0f;

                    g_pCore->GetGraphics()->DrawCircleQueued(vecPosition.fX, vecPosition.fY, fRadius, fStartAngle, fStopAngle, color, colorCenter, siSegments,
                                                             fRatio, bPostGUI);
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
            else
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }
        else
        {
            lua_pushboolean(luaVM, false);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawImage(lua_State* luaVM)
{
    //  bool dxDrawImage ( float posX, float posY, float width, float height, string filepath [, float rotation = 0, float rotationCenterOffsetX = 0,
    //      float rotationCenterOffsetY = 0, int color = white, bool postGUI = false ] )

    CVector2D        vecPosition;
    CVector2D        vecSize;
    CClientMaterial* pMaterialElement;
    float            fRotation;
    CVector2D        vecRotationCenter;
    SColor           color;
    bool             bPostGUI;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadVector2D(vecSize);
    MixedReadMaterialString(argStream, pMaterialElement);
    argStream.ReadNumber(fRotation, 0);
    argStream.ReadVector2D(vecRotationCenter, CVector2D());
    argStream.ReadColor(color, 0xffffffff);
    argStream.ReadBool(bPostGUI, false);

    if (!argStream.HasErrors())
    {
        if (pMaterialElement)
        {
            g_pCore->GetGraphics()->DrawTextureQueued(vecPosition.fX, vecPosition.fY, vecSize.fX, vecSize.fY, 0, 0, 1, 1, true,
                                                      pMaterialElement->GetMaterialItem(), fRotation, vecRotationCenter.fX, vecRotationCenter.fY, color,
                                                      bPostGUI);
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawImageSection(lua_State* luaVM)
{
    //  bool dxDrawImageSection ( float posX, float posY, float width, float height, float u, float v, float usize, float vsize, string filepath,
    //      [ float rotation = 0, float rotationCenterOffsetX = 0, float rotationCenterOffsetY = 0, int color = white, bool postGUI = false ] )
    CVector2D        vecPosition;
    CVector2D        vecSize;
    CVector2D        vecSectionPosition;
    CVector2D        vecSectionSize;
    CClientMaterial* pMaterialElement;
    float            fRotation;
    CVector2D        vecRotationCenter;
    SColor           color;
    bool             bPostGUI;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadVector2D(vecSize);
    argStream.ReadVector2D(vecSectionPosition);
    argStream.ReadVector2D(vecSectionSize);
    MixedReadMaterialString(argStream, pMaterialElement);
    argStream.ReadNumber(fRotation, 0);
    argStream.ReadVector2D(vecRotationCenter, CVector2D());
    argStream.ReadColor(color, 0xffffffff);
    argStream.ReadBool(bPostGUI, false);

    if (!argStream.HasErrors())
    {
        if (pMaterialElement)
        {
            g_pCore->GetGraphics()->DrawTextureQueued(vecPosition.fX, vecPosition.fY, vecSize.fX, vecSize.fY, vecSectionPosition.fX, vecSectionPosition.fY,
                                                      vecSectionSize.fX, vecSectionSize.fY, false, pMaterialElement->GetMaterialItem(), fRotation,
                                                      vecRotationCenter.fX, vecRotationCenter.fY, color, bPostGUI);
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawPrimitive3D(lua_State* luaVM)
{
    // bool DxDrawPrimitive3D (string primitiveType, bool postGUI, table vertice1, ...)
    D3DPRIMITIVETYPE ePrimitiveType;
    auto             pVecVertices = new std::vector<PrimitiveVertice>();
    eRenderStage renderStage{eRenderStage::POST_FX};
    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(ePrimitiveType);
    if (argStream.NextIsBool())
        renderStage = argStream.ReadBool() ? eRenderStage::POST_GUI : eRenderStage::POST_FX;
    else
        argStream.ReadEnumString(renderStage, eRenderStage::POST_FX);

    std::vector<double> vecTableContent;

    while (argStream.NextIsTable())
    {
        vecTableContent.clear();

        argStream.ReadNumberTable(vecTableContent);
        switch (vecTableContent.size())
        {
            case Primitive3DVerticeSizes::VERT_XYZ:
                pVecVertices->push_back(PrimitiveVertice{static_cast<float>(vecTableContent[0]), static_cast<float>(vecTableContent[1]),
                                                         static_cast<float>(vecTableContent[2]), (DWORD)0xFFFFFFFF});
                break;
            case Primitive3DVerticeSizes::VERT_XYZ_COLOR:
                pVecVertices->push_back(PrimitiveVertice{static_cast<float>(vecTableContent[0]), static_cast<float>(vecTableContent[1]),
                                                         static_cast<float>(vecTableContent[2]), static_cast<DWORD>(static_cast<int64_t>(vecTableContent[3]))});
                break;
            default:
                argStream.SetCustomError(SString("Expected table with 3 or 4 numbers, got %i numbers", vecTableContent.size()).c_str());
                break;
        }
    }

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (g_pCore->GetGraphics()->IsValidPrimitiveSize(pVecVertices->size(), ePrimitiveType))
    {
        g_pCore->GetGraphics()->DrawPrimitive3DQueued(pVecVertices, ePrimitiveType, renderStage);
        lua_pushboolean(luaVM, true);
        return 1;
    }

    // Failed
    delete pVecVertices;
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawMaterialPrimitive3D(lua_State* luaVM)
{
    // bool DxDrawMaterialPrimitive3D (string primitiveType, dxMaterial material, bool postGUI, table vertice1, ...)
    D3DPRIMITIVETYPE ePrimitiveType;
    auto             pVecVertices = new std::vector<PrimitiveMaterialVertice>();
    CClientMaterial* pMaterialElement;
    eRenderStage renderStage{eRenderStage::POST_FX};

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(ePrimitiveType);
    MixedReadMaterialString(argStream, pMaterialElement);
    if (argStream.NextIsBool())
        renderStage = argStream.ReadBool() ? eRenderStage::POST_GUI : eRenderStage::POST_FX;
    else
        argStream.ReadEnumString(renderStage, eRenderStage::POST_FX);

    std::vector<double> vecTableContent;

    while (argStream.NextIsTable())
    {
        vecTableContent.clear();

        argStream.ReadNumberTable(vecTableContent);
        switch (vecTableContent.size())
        {
            case Primitive3DVerticeSizes::VERT_XYZ_UV:
                pVecVertices->push_back(PrimitiveMaterialVertice{static_cast<float>(vecTableContent[0]), static_cast<float>(vecTableContent[1]),
                                                                 static_cast<float>(vecTableContent[2]), (DWORD)0xFFFFFFFF,
                                                                 static_cast<float>(vecTableContent[3]), static_cast<float>(vecTableContent[4])});
                break;
            case Primitive3DVerticeSizes::VERT_XYZ_COLOR_UV:
                pVecVertices->push_back(PrimitiveMaterialVertice{static_cast<float>(vecTableContent[0]), static_cast<float>(vecTableContent[1]),
                                                                 static_cast<float>(vecTableContent[2]),
                                                                 static_cast<DWORD>(static_cast<int64_t>(vecTableContent[3])),
                                                                 static_cast<float>(vecTableContent[4]), static_cast<float>(vecTableContent[5])});
                break;
            default:
                argStream.SetCustomError(SString("Expected table with 5 or 6 numbers, got %i numbers", vecTableContent.size()).c_str());
                break;
        }
    }

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (g_pCore->GetGraphics()->IsValidPrimitiveSize(pVecVertices->size(), ePrimitiveType))
    {
        g_pCore->GetGraphics()->DrawMaterialPrimitive3DQueued(pVecVertices, ePrimitiveType, pMaterialElement->GetMaterialItem(), renderStage);
        lua_pushboolean(luaVM, true);
        return 1;
    }

    // Failed
    delete pVecVertices;
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawPrimitive(lua_State* luaVM)
{
    // bool dxDrawPrimitive (string primitiveType, bool postGUI, table vertice1, ...)
    D3DPRIMITIVETYPE ePrimitiveType;
    auto             pVecVertices = new std::vector<PrimitiveVertice>();
    bool             bPostGUI;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(ePrimitiveType);
    argStream.ReadBool(bPostGUI);

    std::vector<double> vecTableContent;

    while (argStream.NextIsTable())
    {
        vecTableContent.clear();

        argStream.ReadNumberTable(vecTableContent);
        switch (vecTableContent.size())
        {
            case PrimitiveVerticeSizes::VERT_XY:
                pVecVertices->push_back(PrimitiveVertice{static_cast<float>(vecTableContent[0]), static_cast<float>(vecTableContent[1]), 0, (DWORD)0xFFFFFFFF});
                break;
            case PrimitiveVerticeSizes::VERT_XY_COLOR:
                pVecVertices->push_back(PrimitiveVertice{static_cast<float>(vecTableContent[0]), static_cast<float>(vecTableContent[1]), 0,
                                                         static_cast<DWORD>(static_cast<int64_t>(vecTableContent[2]))});
                break;
            default:
                argStream.SetCustomError(SString("Expected table with 2 or 3 numbers, got %i numbers", vecTableContent.size()).c_str());
                break;
        }
    }

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (g_pCore->GetGraphics()->IsValidPrimitiveSize(pVecVertices->size(), ePrimitiveType))
    {
        g_pCore->GetGraphics()->DrawPrimitiveQueued(pVecVertices, ePrimitiveType, bPostGUI);
        lua_pushboolean(luaVM, true);
        return 1;
    }

    // Failed
    delete pVecVertices;
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxDrawMaterialPrimitive(lua_State* luaVM)
{
    // bool dxDrawPrimitive (string primitiveType, dxMaterial material, bool postGUI, table vertice1, ...)
    D3DPRIMITIVETYPE ePrimitiveType;
    auto             pVecVertices = new std::vector<PrimitiveMaterialVertice>();
    CClientMaterial* pMaterialElement;
    bool             bPostGUI;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(ePrimitiveType);
    MixedReadMaterialString(argStream, pMaterialElement);
    argStream.ReadBool(bPostGUI);

    std::vector<double> vecTableContent;

    while (argStream.NextIsTable())
    {
        vecTableContent.clear();

        argStream.ReadNumberTable(vecTableContent);
        switch (vecTableContent.size())
        {
            case PrimitiveVerticeSizes::VERT_XY_UV:
                pVecVertices->push_back(PrimitiveMaterialVertice{static_cast<float>(vecTableContent[0]), static_cast<float>(vecTableContent[1]), 0,
                                                                 (DWORD)0xFFFFFFFF, static_cast<float>(vecTableContent[2]),
                                                                 static_cast<float>(vecTableContent[3])});
                break;
            case PrimitiveVerticeSizes::VERT_XY_COLOR_UV:
                pVecVertices->push_back(PrimitiveMaterialVertice{static_cast<float>(vecTableContent[0]), static_cast<float>(vecTableContent[1]), 0,
                                                                 static_cast<DWORD>(static_cast<int64_t>(vecTableContent[2])),
                                                                 static_cast<float>(vecTableContent[3]), static_cast<float>(vecTableContent[4])});
                break;
            default:
                argStream.SetCustomError(SString("Expected table with 4 or 5 numbers, got %i numbers", vecTableContent.size()).c_str());
                break;
        }
    }

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (g_pCore->GetGraphics()->IsValidPrimitiveSize(pVecVertices->size(), ePrimitiveType))
    {
        g_pCore->GetGraphics()->DrawMaterialPrimitiveQueued(pVecVertices, ePrimitiveType, pMaterialElement->GetMaterialItem(), bPostGUI);
        lua_pushboolean(luaVM, true);
        return 1;
    }

    // Failed
    delete pVecVertices;
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxGetTextWidth(lua_State* luaVM)
{
    //  float dxGetTextWidth ( string text, [float scale=1, mixed font="default", bool colorCoded=false] )
    SString        strText;
    float          fScale;
    eFontType      fontType;
    CClientDxFont* pDxFontElement;
    bool           bColorCoded;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strText);
    argStream.ReadNumber(fScale, 1);
    MixedReadDxFontString(argStream, fontType, FONT_DEFAULT, pDxFontElement);
    argStream.ReadBool(bColorCoded, false);

    if (!argStream.HasErrors())
    {
        ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont(fontType, pDxFontElement);

        // Retrieve the longest line's extent
        std::stringstream ssText(strText);
        std::string       sLineText;
        float             fWidth = 0.0f, fLineExtent = 0.0f;

        while (std::getline(ssText, sLineText))
        {
            fLineExtent = g_pCore->GetGraphics()->GetDXTextExtent(sLineText.c_str(), fScale, pD3DXFont, bColorCoded);
            if (fLineExtent > fWidth)
                fWidth = fLineExtent;
        }

        // Success
        lua_pushnumber(luaVM, fWidth);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::OOP_DxGetTextWidth(lua_State* luaVM)
{
    //  float dxGetTextWidth ( string text, [float scale=1, mixed font="default", bool colorCoded=false] )
    SString        strText;
    float          fScale;
    CClientDxFont* pDxFontElement;
    bool           bColorCoded;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pDxFontElement);
    argStream.ReadString(strText);
    argStream.ReadNumber(fScale, 1);
    argStream.ReadBool(bColorCoded, false);

    if (!argStream.HasErrors())
    {
        ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont(FONT_DEFAULT, pDxFontElement);

        // Retrieve the longest line's extent
        std::stringstream ssText(strText);
        std::string       sLineText;
        float             fWidth = 0.0f, fLineExtent = 0.0f;

        while (std::getline(ssText, sLineText))
        {
            fLineExtent = g_pCore->GetGraphics()->GetDXTextExtent(sLineText.c_str(), fScale, pD3DXFont, bColorCoded);
            if (fLineExtent > fWidth)
                fWidth = fLineExtent;
        }

        // Success
        lua_pushnumber(luaVM, fWidth);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

CVector2D CLuaDrawingDefs::OOP_DxGetTextSize(std::variant<CClientDxFont*, eFontType> variantFont, const std::string text, const std::optional<float> optWidth,
                                             const std::optional<std::variant<CVector2D, float>> optScaleXY, const std::optional<bool> optWordBreak,
                                             const std::optional<bool> optColorCoded)
{
    // float, float dxGetTextSize ( string text, [float width=0, float scaleXY=1.0, float=scaleY=1.0, mixed font="default",
    // bool wordBreak=false, bool colorCoded=false] )
    CGraphicsInterface* const graphics = g_pCore->GetGraphics();

    // resolve scale (use X as Y value, if optScaleY is empty)
    CVector2D scale(1.0f, 1.0f);
    if (optScaleXY.has_value())
    {
        std::variant<CVector2D, float> scaleXY = optScaleXY.value();
        if (std::holds_alternative<float>(scaleXY))
        {
            scale.fX = std::get<float>(scaleXY);
            scale.fY = scale.fX;
        }
        else
        {
            scale = std::get<CVector2D>(scaleXY);
        }
    }

    CVector2D vecSize;
    graphics->GetDXTextSize(vecSize, text.c_str(), optWidth.value_or(0.0f), scale.fX, scale.fY, CStaticFunctionDefinitions::ResolveD3DXFont(variantFont),
                            optWordBreak.value_or(false), optColorCoded.value_or(false));

    return vecSize;
}

int CLuaDrawingDefs::DxGetFontHeight(lua_State* luaVM)
{
    //  int dxGetFontHeight ( [float scale=1, mixed font="default"] )
    float          fScale;
    eFontType      fontType;
    CClientDxFont* pDxFontElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fScale, 1);
    MixedReadDxFontString(argStream, fontType, FONT_DEFAULT, pDxFontElement);

    if (!argStream.HasErrors())
    {
        ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont(fontType, pDxFontElement);

        float fHeight = g_pCore->GetGraphics()->GetDXFontHeight(fScale, pD3DXFont);
        // Success
        lua_pushnumber(luaVM, fHeight);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::OOP_DxGetFontHeight(lua_State* luaVM)
{
    //  int dxGetFontHeight ( [float scale=1, mixed font="default"] )
    float          fScale;
    CClientDxFont* pDxFontElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pDxFontElement);
    argStream.ReadNumber(fScale, 1);

    if (!argStream.HasErrors())
    {
        ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont(FONT_DEFAULT, pDxFontElement);

        float fHeight = g_pCore->GetGraphics()->GetDXFontHeight(fScale, pD3DXFont);
        // Success
        lua_pushnumber(luaVM, fHeight);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxCreateTexture(lua_State* luaVM)
{
    //  element dxCreateTexture( string filepath [, string textureFormat = "argb", bool mipmaps = true, string textureEdge = "wrap" ] )
    //  element dxCreateTexture( string pixels [, string textureFormat = "argb", bool mipmaps = true, string textureEdge = "wrap" ] )
    //  element dxCreateTexture( int width, int height [, string textureFormat = "argb", string textureEdge = "wrap", string textureType = "2d", int depth ] )
    SString         strFilePath;
    CPixels         pixels;
    int             width = 0;
    int             height = 0;
    ERenderFormat   renderFormat;
    bool            bMipMaps = true;
    ETextureAddress textureAddress;
    ETextureType    textureType = TTYPE_TEXTURE;
    int             depth = 1;

    CScriptArgReader argStream(luaVM);
    if (!argStream.NextIsNumber())
    {
        argStream.ReadCharStringRef(pixels.externalData);
        if (!g_pCore->GetGraphics()->GetPixelsManager()->IsPixels(pixels))
        {
            // element dxCreateTexture( string filepath [, string textureFormat = "argb", bool mipmaps = true, string textureEdge = "wrap" ] )
            pixels = CPixels();
            argStream = CScriptArgReader(luaVM);
            argStream.ReadString(strFilePath);
            argStream.ReadEnumString(renderFormat, RFORMAT_UNKNOWN);
            argStream.ReadBool(bMipMaps, true);
            argStream.ReadEnumString(textureAddress, TADDRESS_WRAP);
        }
        else
        {
            // element dxCreateTexture( string pixels [, string textureFormat = "argb", bool mipmaps = true, string textureEdge = "wrap" ] )
            argStream.ReadEnumString(renderFormat, RFORMAT_UNKNOWN);
            argStream.ReadBool(bMipMaps, true);
            argStream.ReadEnumString(textureAddress, TADDRESS_WRAP);
        }
    }
    else
    {
        // element dxCreateTexture( int width, int height [, string textureFormat = "argb", string textureEdge = "wrap", string textureType = "2d", int depth ]
        // )
        argStream.ReadNumber(width);
        argStream.ReadNumber(height);
        argStream.ReadEnumString(renderFormat, RFORMAT_UNKNOWN);
        argStream.ReadEnumString(textureAddress, TADDRESS_WRAP);
        argStream.ReadEnumString(textureType, TTYPE_TEXTURE);
        if (textureType == TTYPE_VOLUMETEXTURE)
            argStream.ReadNumber(depth);
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pParentResource = pLuaMain->GetResource();

            if (!strFilePath.empty())
            {
                // From file
                CResource* pFileResource = pParentResource;
                SString    strPath, strMetaPath;
                if (CResourceManager::ParseResourcePathInput(strFilePath, pFileResource, &strPath, &strMetaPath))
                {
                    if (FileExists(strPath))
                    {
                        CClientTexture* pTexture = g_pClientGame->GetManager()->GetRenderElementManager()->CreateTexture(
                            strPath, NULL, bMipMaps, RDEFAULT, RDEFAULT, renderFormat, textureAddress);
                        if (pTexture)
                        {
                            // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's
                            // ElementGroup? **
                            pTexture->SetParent(pParentResource->GetResourceDynamicEntity());
                        }
                        lua_pushelement(luaVM, pTexture);
                        return 1;
                    }
                    else
                        argStream.SetCustomError(strFilePath, "File not found");
                }
                else
                    argStream.SetCustomError(strFilePath, "Bad file path");
            }
            else if (pixels.GetSize())
            {
                // From pixels
                CClientTexture* pTexture = g_pClientGame->GetManager()->GetRenderElementManager()->CreateTexture("", &pixels, bMipMaps, RDEFAULT, RDEFAULT,
                                                                                                                 renderFormat, textureAddress);
                if (pTexture)
                {
                    pTexture->SetParent(pParentResource->GetResourceDynamicEntity());
                }
                lua_pushelement(luaVM, pTexture);
                return 1;
            }
            else
            {
                // Blank sized
                CClientTexture* pTexture = g_pClientGame->GetManager()->GetRenderElementManager()->CreateTexture("", NULL, false, width, height, renderFormat,
                                                                                                                 textureAddress, textureType, depth);
                if (pTexture)
                {
                    pTexture->SetParent(pParentResource->GetResourceDynamicEntity());
                }
                lua_pushelement(luaVM, pTexture);
                return 1;
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxCreateShader(lua_State* luaVM)
{
    //  element dxCreateShader( string filepath / string raw_data [, float priority = 0, float maxdistance = 0, bool layered = false, string elementTypes =
    //  "world,vehicle,object,other" ] )
    SString                      strFile;
    EffectMacroList              macros;
    float                        fPriority;
    float                        fMaxDistance;
    bool                         bLayered;
    std::vector<EEntityTypeMask> elementTypeList;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strFile);
    if (argStream.NextIsTable())
        argStream.ReadPairTable(macros);
    argStream.ReadNumber(fPriority, 0.0f);
    argStream.ReadNumber(fMaxDistance, 0.0f);
    argStream.ReadBool(bLayered, false);
    argStream.ReadEnumStringList(elementTypeList, "world,vehicle,object,other");

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    if (strFile.empty())
    {
        m_pScriptDebugging->LogCustom(luaVM, "expected non-empty string at argument 1");
        lua_pushnil(luaVM);
        return 1;
    }

    CLuaMain* const pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

    if (!pLuaMain)
    {
        lua_pushnil(luaVM);
        return 1;
    }

    int iEntityTypeMaskResult = 0;

    for (EEntityTypeMask elementType : elementTypeList)
        iEntityTypeMaskResult |= elementType;

    CResource* pParentResource = pLuaMain->GetResource();
    CResource* pFileResource = pParentResource;
    SString    strPath, strMetaPath;

    bool bIsRawData = false;

    const bool bValidFilePath = CResourceManager::ParseResourcePathInput(strFile, pFileResource, &strPath, &strMetaPath);

    if (!bValidFilePath || (strFile[0] != '@' && strFile[0] != ':'))
    {
        bIsRawData = strFile.find("\n") != std::string::npos;

        if (!bIsRawData)
        {
            bIsRawData = (strFile.find("technique ") != std::string::npos) && (strFile.find("pass ") != std::string::npos) &&
                         (strFile.find('{') != std::string::npos) && (strFile.find('}') != std::string::npos);
        }
    }

    SString strRootPath;

    if (bIsRawData)
    {
        strPath = strFile;
        pFileResource = pParentResource;
        strRootPath = pFileResource->GetResourceDirectoryPath(ACCESS_PUBLIC, strMetaPath);
    }
    else
    {
        if (!pFileResource || !FileExists(strPath))
        {
            argStream.SetCustomError(strFile, "file doesn't exist");
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
            lua_pushboolean(luaVM, false);
            return 1;
        }

        strRootPath = strPath.Left(strPath.length() - strMetaPath.length());
    }

    SString        strStatus;
    CClientShader* pShader = g_pClientGame->GetManager()->GetRenderElementManager()->CreateShader(strPath, strRootPath, bIsRawData, strStatus, fPriority,
                                                                                                  fMaxDistance, bLayered, false, iEntityTypeMaskResult, macros);

    if (pShader)
    {
        pShader->SetParent(pParentResource->GetResourceDynamicEntity());
        lua_pushelement(luaVM, pShader);
        lua_pushstring(luaVM, strStatus);
        return 2;
    }

    // Replace any path in the error message with our own one
    SString strRootPathWithoutResource = strRootPath.Left(strRootPath.TrimEnd("\\").length() - SStringX(pFileResource->GetName()).length());
    strStatus = strStatus.ReplaceI(strRootPathWithoutResource, "");
    argStream.SetCustomError(bIsRawData ? "raw data" : strFile, strStatus);
    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxCreateRenderTarget(lua_State* luaVM)
{
    //  element dxCreateRenderTarget( int sizeX, int sizeY [, int withAlphaChannel = false ] )
    //  element dxCreateRenderTarget( int sizeX, int sizeY, SurfaceFormat surfaceFormat )
    CVector2D vecSize;
    bool      bWithAlphaChannel = false;
    bool      bHasSurfaceFormat = false;
    _D3DFORMAT surfaceFormat;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecSize);
    if (argStream.NextIsString())
    {
        argStream.ReadEnumString(surfaceFormat);
        bHasSurfaceFormat = true;
    }
    else
        argStream.ReadBool(bWithAlphaChannel, false);

    if (!argStream.HasErrors())
    {
        CLuaMain*  pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        CResource* pParentResource = pLuaMain ? pLuaMain->GetResource() : NULL;
        if (pParentResource)
        {
            CClientRenderTarget* pRenderTarget = g_pClientGame->GetManager()->GetRenderElementManager()->CreateRenderTarget(
                (uint)vecSize.fX, (uint)vecSize.fY, bHasSurfaceFormat, bWithAlphaChannel, surfaceFormat);
            if (pRenderTarget)
            {
                // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                pRenderTarget->SetParent(pParentResource->GetResourceDynamicEntity());

                lua_pushelement(luaVM, pRenderTarget);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxCreateScreenSource(lua_State* luaVM)
{
    //  element dxCreateScreenSource( int sizeX, int sizeY )
    CVector2D vecSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecSize);

    if (!argStream.HasErrors())
    {
        CLuaMain*  pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        CResource* pParentResource = pLuaMain ? pLuaMain->GetResource() : NULL;
        if (pParentResource)
        {
            CClientScreenSource* pScreenSource = g_pClientGame->GetManager()->GetRenderElementManager()->CreateScreenSource((uint)vecSize.fX, (uint)vecSize.fY);
            if (pScreenSource)
            {
                // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                pScreenSource->SetParent(pParentResource->GetResourceDynamicEntity());
            }
            lua_pushelement(luaVM, pScreenSource);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxGetMaterialSize(lua_State* luaVM)
{
    //  int, int [, int] dxGetMaterialSize( element material )
    CClientMaterial* pMaterial;
    SString          strName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pMaterial);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pMaterial->GetMaterialItem()->m_uiSizeX);
        lua_pushnumber(luaVM, pMaterial->GetMaterialItem()->m_uiSizeY);
        if (CFileTextureItem* pTextureItem = DynamicCast<CFileTextureItem>(pMaterial->GetMaterialItem()))
        {
            if (pTextureItem->m_TextureType == TTYPE_VOLUMETEXTURE)
            {
                lua_pushnumber(luaVM, pTextureItem->m_uiVolumeDepth);
                return 3;
            }
        }
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxSetShaderValue(lua_State* luaVM)
{
    //  bool dxSetShaderValue( element shader, string name, mixed value )
    CClientShader* pShader;
    SString        strName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pShader);
    argStream.ReadString(strName);

    if (!argStream.HasErrors())
    {
        // Try each mixed type in turn
        if (argStream.NextIsUserDataOfType<CLuaVector2D>())
        {
            CVector2D vecValue;
            argStream.ReadVector2D(vecValue);

            bool bResult = pShader->GetShaderItem()->SetValue(strName, &vecValue.fX, 2);
            lua_pushboolean(luaVM, bResult);
            return 1;
        }
        else if (argStream.NextIsUserDataOfType<CLuaVector3D>())
        {
            CVector vecValue;
            argStream.ReadVector3D(vecValue);

            bool bResult = pShader->GetShaderItem()->SetValue(strName, &vecValue.fX, 3);
            lua_pushboolean(luaVM, bResult);
            return 1;
        }
        else if (argStream.NextIsUserDataOfType<CLuaVector4D>())
        {
            CVector4D vecValue;
            argStream.ReadVector4D(vecValue);

            bool bResult = pShader->GetShaderItem()->SetValue(strName, &vecValue.fX, 4);
            lua_pushboolean(luaVM, bResult);
            return 1;
        }
        else if (argStream.NextIsUserDataOfType<CLuaMatrix>())
        {
            CMatrix matValue;
            argStream.ReadMatrix(matValue);
            float fBuffer[16];
            matValue.GetBuffer(fBuffer);

            bool bResult = pShader->GetShaderItem()->SetValue(strName, fBuffer, 16);
            lua_pushboolean(luaVM, bResult);
            return 1;
        }
        else if (argStream.NextIsUserData())
        {
            // Texture
            CClientTexture* pTexture;
            argStream.ReadUserData(pTexture);
            if (pTexture)
            {
                bool bResult = pShader->GetShaderItem()->SetValue(strName, pTexture->GetTextureItem());
                lua_pushboolean(luaVM, bResult);
                return 1;
            }
        }
        else if (argStream.NextIsBool())
        {
            // bool
            bool bValue;
            argStream.ReadBool(bValue);
            bool bResult = pShader->GetShaderItem()->SetValue(strName, bValue);
            lua_pushboolean(luaVM, bResult);
            return 1;
        }
        else if (argStream.NextCouldBeNumber())
        {
            // float(s)
            float fBuffer[16]{};
            uint  i;
            for (i = 0; i < NUMELMS(fBuffer);)
            {
                argStream.ReadNumber(fBuffer[i++]);
                if (!argStream.NextCouldBeNumber())
                    break;
            }
            bool bResult = pShader->GetShaderItem()->SetValue(strName, fBuffer, i);
            lua_pushboolean(luaVM, bResult);
            return 1;
        }
        else if (argStream.NextIsTable())
        {
            // table (of floats)
            float fBuffer[16]{};
            uint  i = 0;

            lua_pushnil(luaVM);            // Loop through our table, beginning at the first key
            while (lua_next(luaVM, argStream.m_iIndex) != 0 && i < NUMELMS(fBuffer))
            {
                fBuffer[i++] = static_cast<float>(lua_tonumber(luaVM, -1));            // Ignore the index at -2, and just read the value
                lua_pop(luaVM, 1);                                                     // Remove the item and keep the key for the next iteration
            }
            bool bResult = pShader->GetShaderItem()->SetValue(strName, fBuffer, i);
            lua_pushboolean(luaVM, bResult);
            return 1;
        }
        argStream.SetCustomError("Expected number, bool, table or texture at argument 3");
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxSetShaderTessellation(lua_State* luaVM)
{
    //  bool dxSetShaderTessellation( element shader, int tessellationX, int tessellationY )
    //  bool shader:setShaderTessellation( element shader, Vector2 tessellation )
    CClientShader* pShader;
    CVector2D      vecTessellation;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pShader);
    argStream.ReadVector2D(vecTessellation);

    if (!argStream.HasErrors())
    {
        pShader->GetShaderItem()->SetTessellation((uint)vecTessellation.fX, (uint)vecTessellation.fY);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxSetShaderTransform(lua_State* luaVM)
{
    //  bool dxSetShaderTransform( element shader, lots )
    CClientShader*   pShader;
    SShaderTransform transform;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pShader);

    argStream.ReadVector3D(transform.vecRot, CVector());
    argStream.ReadVector3D(transform.vecRotCenOffset, CVector());
    argStream.ReadBool(transform.bRotCenOffsetOriginIsScreen, false);
    argStream.ReadVector2D(transform.vecPersCenOffset, CVector2D());
    argStream.ReadBool(transform.bPersCenOffsetOriginIsScreen, false);

    if (!argStream.HasErrors())
    {
        pShader->GetShaderItem()->SetTransform(transform);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxSetRenderTarget(lua_State* luaVM)
{
    //  bool setRenderTaget( element renderTarget [, bool clear = false ] )
    CClientRenderTarget* pRenderTarget;
    bool                 bClear;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pRenderTarget, NULL);
    argStream.ReadBool(bClear, false);

    // Check version ok for this function to be called now
    if (!g_pCore->GetGraphics()->GetRenderItemManager()->IsSetRenderTargetEnabledOldVer())
        MinClientReqCheck(argStream, MIN_CLIENT_REQ_DXSETRENDERTARGET_CALL_RESTRICTIONS, "function is being called outside certain events");

    if (!argStream.HasErrors())
    {
        bool bResult;
        if (pRenderTarget)
            bResult = g_pCore->GetGraphics()->GetRenderItemManager()->SetRenderTarget(pRenderTarget->GetRenderTargetItem(), bClear);
        else
            bResult = g_pCore->GetGraphics()->GetRenderItemManager()->RestoreDefaultRenderTarget();

        lua_pushboolean(luaVM, bResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxUpdateScreenSource(lua_State* luaVM)
{
    //  bool dxUpdateScreenSource( element screenSource [, bool resampleNow] )
    CClientScreenSource* pScreenSource;
    bool                 bResampleNow;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pScreenSource);
    argStream.ReadBool(bResampleNow, false);

    if (!argStream.HasErrors())
    {
        g_pCore->GetGraphics()->GetRenderItemManager()->UpdateScreenSource(pScreenSource->GetScreenSourceItem(), bResampleNow);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxCreateFont(lua_State* luaVM)
{
    //  element dxCreateFont( string filepath [, int size=9, bool bold=false ] )
    SString      strFilePath;
    int          iSize;
    bool         bBold;
    eFontQuality ulFontQuality;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strFilePath);
    argStream.ReadNumber(iSize, 9);
    argStream.ReadBool(bBold, false);
    argStream.ReadEnumString(ulFontQuality, FONT_QUALITY_PROOF);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pParentResource = pLuaMain->GetResource();
            CResource* pFileResource = pParentResource;
            SString    strPath;
            if (CResourceManager::ParseResourcePathInput(strFilePath, pFileResource, &strPath))
            {
                if (FileExists(strPath))
                {
                    CClientDxFont* pDxFont = g_pClientGame->GetManager()->GetRenderElementManager()->CreateDxFont(strPath, iSize, bBold, ulFontQuality);

                    if (pDxFont)
                    {
                        // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's
                        // ElementGroup? **
                        pDxFont->SetParent(pParentResource->GetResourceDynamicEntity());
                        lua_pushelement(luaVM, pDxFont);
                        return 1;
                    }

                    argStream.SetCustomError(strFilePath, "Error creating font");
                }
                else
                    argStream.SetCustomError(strFilePath, "File not found");
            }
            else
                argStream.SetCustomError(strFilePath, "Bad file path");
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxSetTestMode(lua_State* luaVM)
{
    //  bool dxSetTestMode( string testMode )
    eDxTestMode testMode;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(testMode, DX_TEST_MODE_NONE);

    if (!argStream.HasErrors())
    {
        g_pCore->GetGraphics()->GetRenderItemManager()->SetTestMode(testMode);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxGetStatus(lua_State* luaVM)
{
    //  table dxGetStatus()

    CScriptArgReader argStream(luaVM);

    if (!argStream.HasErrors())
    {
        SDxStatus dxStatus;
        g_pCore->GetGraphics()->GetRenderItemManager()->GetDxStatus(dxStatus);

        lua_createtable(luaVM, 0, 24);

        lua_pushstring(luaVM, "TestMode");
        lua_pushstring(luaVM, EnumToString(dxStatus.testMode));
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "VideoCardName");
        lua_pushstring(luaVM, dxStatus.videoCard.strName);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "VideoCardRAM");
        lua_pushnumber(luaVM, dxStatus.videoCard.iInstalledMemoryKB / 1024);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "VideoCardPSVersion");
        lua_pushstring(luaVM, dxStatus.videoCard.strPSVersion);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "VideoCardMaxAnisotropy");
        lua_pushnumber(luaVM, dxStatus.videoCard.iMaxAnisotropy);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "VideoCardNumRenderTargets");
        lua_pushnumber(luaVM, dxStatus.videoCard.iNumSimultaneousRTs);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "VideoMemoryFreeForMTA");
        lua_pushnumber(luaVM, dxStatus.videoMemoryKB.iFreeForMTA / 1024);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "VideoMemoryUsedByFonts");
        lua_pushnumber(luaVM, dxStatus.videoMemoryKB.iUsedByFonts / 1024);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "VideoMemoryUsedByTextures");
        lua_pushnumber(luaVM, dxStatus.videoMemoryKB.iUsedByTextures / 1024);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "VideoMemoryUsedByRenderTargets");
        lua_pushnumber(luaVM, dxStatus.videoMemoryKB.iUsedByRenderTargets / 1024);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingWindowed");
        lua_pushboolean(luaVM, dxStatus.settings.bWindowed);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingFullScreenStyle");
        lua_pushnumber(luaVM, dxStatus.settings.iFullScreenStyle);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingFXQuality");
        lua_pushnumber(luaVM, dxStatus.settings.iFXQuality);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingDrawDistance");
        lua_pushnumber(luaVM, dxStatus.settings.iDrawDistance);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingVolumetricShadows");
        lua_pushboolean(luaVM, dxStatus.settings.bVolumetricShadows);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingStreamingVideoMemoryForGTA");
        lua_pushnumber(luaVM, dxStatus.settings.iStreamingMemory);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "AllowScreenUpload");
        lua_pushboolean(luaVM, dxStatus.settings.bAllowScreenUpload);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "DepthBufferFormat");
        lua_pushstring(luaVM, EnumToString(dxStatus.videoCard.depthBufferFormat));
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "UsingDepthBuffer");
        lua_pushboolean(luaVM, dxStatus.state.iNumShadersUsingReadableDepthBuffer > 0);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "Setting32BitColor");
        lua_pushboolean(luaVM, dxStatus.settings.b32BitColor);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingGrassEffect");
        lua_pushboolean(luaVM, dxStatus.settings.bGrassEffect);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingHeatHaze");
        lua_pushboolean(luaVM, dxStatus.settings.bHeatHaze);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingAnisotropicFiltering");
        lua_pushnumber(luaVM, dxStatus.settings.iAnisotropicFiltering);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingAntiAliasing");
        lua_pushnumber(luaVM, dxStatus.settings.iAntiAliasing);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingAspectRatio");
        lua_pushstring(luaVM, EnumToString(dxStatus.settings.aspectRatio));
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingHUDMatchAspectRatio");
        lua_pushboolean(luaVM, dxStatus.settings.bHUDMatchAspectRatio);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingFOV");
        lua_pushnumber(luaVM, dxStatus.settings.fFieldOfView);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingHighDetailVehicles");
        lua_pushboolean(luaVM, dxStatus.settings.bHighDetailVehicles);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingHighDetailPeds");
        lua_pushboolean(luaVM, dxStatus.settings.bHighDetailPeds);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingBlur");
        lua_pushboolean(luaVM, dxStatus.settings.bBlur);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingCoronaReflections");
        lua_pushboolean(luaVM, dxStatus.settings.bCoronaReflections);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "SettingDynamicPedShadows");
        lua_pushboolean(luaVM, dxStatus.settings.bDynamicPedShadows);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "TotalPhysicalMemory");
        lua_pushnumber(luaVM, static_cast<lua_Number>(SharedUtil::GetWMITotalPhysicalMemory()) / 1024.0 / 1024.0);
        lua_settable(luaVM, -3);

        lua::Push(luaVM, "SettingDebugMode");
        lua::Push(luaVM, [] {
            switch (g_pCore->GetDiagnosticDebug())
            {
                case EDiagnosticDebug::GRAPHICS_6734:
                    return "#6734 Graphics";
                case EDiagnosticDebug::D3D_6732:
                    return "#6732 D3D";
                case EDiagnosticDebug::LOG_TIMING_0000:
                    return "#0000 Log timing";
                case EDiagnosticDebug::JOYSTICK_0000:
                    return "#0000 Joystick";
                case EDiagnosticDebug::LUA_TRACE_0000:
                    return "#0000 Lua trace";
                case EDiagnosticDebug::RESIZE_ALWAYS_0000:
                    return "#0000 Resize always";
                case EDiagnosticDebug::RESIZE_NEVER_0000:
                    return "#0000 Resize never";
                default:
                    return "Default";
            }
        }());
        lua_settable(luaVM, -3);

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxGetTexturePixels(lua_State* luaVM)
{
    //  string dxGetTexturePixels( [ int surfaceIndex, ] element texture [, string pixelsFormat = "plain" [, string textureFormat = "argb"] [, bool mipmaps = true]]
    //                             [, int x, int y, int width, int height ] )
    CClientTexture*   pTexture;
    CVector2D         vecPosition;
    CVector2D         vecSize;
    int               surfaceIndex = 0;
    EPixelsFormatType pixelsFormat = EPixelsFormat::PLAIN;
    ERenderFormat     textureFormat = RFORMAT_UNKNOWN;
    bool              bMipMaps = true;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsNumber())
        argStream.ReadNumber(surfaceIndex);
    argStream.ReadUserData(pTexture);

    if (argStream.NextIsEnumString(pixelsFormat))
    {
        argStream.ReadEnumString(pixelsFormat, EPixelsFormat::PLAIN);
        argStream.ReadIfNextIsEnumString(textureFormat, RFORMAT_UNKNOWN);
        argStream.ReadIfNextIsBool(bMipMaps, true);
    }

    argStream.ReadVector2D(vecPosition, CVector2D());
    argStream.ReadVector2D(vecSize, CVector2D());

    if (!argStream.HasErrors())
    {
        CVector2D vecEndPosition = vecPosition + vecSize;
        RECT      rc = {(int)vecPosition.fX, (int)vecPosition.fY, (int)vecEndPosition.fX, (int)vecEndPosition.fY};
        CPixels   pixels;

        // TODO: "height ? &rc : NULL" - height will always be set to 0 or another number! Why does this exist?
        if (g_pCore->GetGraphics()->GetPixelsManager()->GetTexturePixels(pTexture->GetTextureItem()->m_pD3DTexture, pixels, pixelsFormat, textureFormat,
                                                                         bMipMaps, vecSize.fY == 0 ? NULL : &rc, surfaceIndex))
        {
            lua_pushlstring(luaVM, pixels.GetData(), pixels.GetSize());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxSetTexturePixels(lua_State* luaVM)
{
    //  bool dxSetTexturePixels( [ int sufaceIndex, ] element texture, string pixels [, int x, int y, int width, int height ] )
    CClientTexture* pTexture;
    CPixels         pixels;
    CVector2D       vecPosition;
    CVector2D       vecSize;
    int             surfaceIndex = 0;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsNumber())
        argStream.ReadNumber(surfaceIndex);
    argStream.ReadUserData(pTexture);
    argStream.ReadCharStringRef(pixels.externalData);
    argStream.ReadVector2D(vecPosition, CVector2D());
    argStream.ReadVector2D(vecSize, CVector2D());

    if (!argStream.HasErrors())
    {
        CVector2D vecEndPosition = vecPosition + vecSize;
        RECT      rc = {(int)vecPosition.fX, (int)vecPosition.fY, (int)vecEndPosition.fX, (int)vecEndPosition.fY};
        if (g_pCore->GetGraphics()->GetPixelsManager()->SetTexturePixels(pTexture->GetTextureItem()->m_pD3DTexture, pixels, vecSize.fY == 0 ? NULL : &rc,
                                                                         surfaceIndex))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxGetPixelsSize(lua_State* luaVM)
{
    //  int x,y dxGetPixelsSize( string pixels )
    CPixels pixels;

    CScriptArgReader argStream(luaVM);
    argStream.ReadCharStringRef(pixels.externalData);

    if (!argStream.HasErrors())
    {
        uint uiSizeX;
        uint uiSizeY;
        if (g_pCore->GetGraphics()->GetPixelsManager()->GetPixelsSize(pixels, uiSizeX, uiSizeY))
        {
            lua_pushinteger(luaVM, uiSizeX);
            lua_pushinteger(luaVM, uiSizeY);
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxGetPixelsFormat(lua_State* luaVM)
{
    //  string dxGetPixelsFormat( string pixels )
    CPixels pixels;

    CScriptArgReader argStream(luaVM);
    argStream.ReadCharStringRef(pixels.externalData);

    if (!argStream.HasErrors())
    {
        EPixelsFormatType format = g_pCore->GetGraphics()->GetPixelsManager()->GetPixelsFormat(pixels);
        if (format != EPixelsFormat::UNKNOWN)
        {
            lua_pushstring(luaVM, EnumToString(format));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxConvertPixels(lua_State* luaVM)
{
    //  string dxConvertPixels( string pixels, string pixelFormat [, int quality] )
    CPixels           pixels;
    EPixelsFormatType format;
    int               quality;

    CScriptArgReader argStream(luaVM);
    argStream.ReadCharStringRef(pixels.externalData);
    argStream.ReadEnumString(format);
    argStream.ReadNumber(quality, 80);

    if (!argStream.HasErrors())
    {
        CPixels newPixels;
        if (g_pCore->GetGraphics()->GetPixelsManager()->ChangePixelsFormat(pixels, newPixels, format, quality))
        {
            lua_pushlstring(luaVM, newPixels.GetData(), newPixels.GetSize());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxGetPixelColor(lua_State* luaVM)
{
    //  int r,g,b,a dxGetPixelColor( string pixels, int x, int y )
    CPixels   pixels;
    CVector2D vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadCharStringRef(pixels.externalData);
    argStream.ReadVector2D(vecPosition);

    if (!argStream.HasErrors())
    {
        SColor color;
        if (g_pCore->GetGraphics()->GetPixelsManager()->GetPixelColor(pixels, (int)vecPosition.fX, (int)vecPosition.fY, color))
        {
            lua_pushnumber(luaVM, color.R);
            lua_pushnumber(luaVM, color.G);
            lua_pushnumber(luaVM, color.B);
            lua_pushnumber(luaVM, color.A);
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxSetPixelColor(lua_State* luaVM)
{
    //  bool dxSetPixelColor( string pixels, int x, int y, int r, int g, int b [, int a] )
    CPixels   pixels;
    CVector2D vecPosition;
    SColor    color;

    CScriptArgReader argStream(luaVM);
    argStream.ReadCharStringRef(pixels.externalData);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadNumber(color.R);
    argStream.ReadNumber(color.G);
    argStream.ReadNumber(color.B);
    argStream.ReadNumber(color.A, 255);

    if (!argStream.HasErrors())
    {
        if (g_pCore->GetGraphics()->GetPixelsManager()->SetPixelColor(pixels, (int)vecPosition.fX, (int)vecPosition.fY, color))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // error: bad arguments
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxSetBlendMode(lua_State* luaVM)
{
    //  bool dxSetBlendMode ( string blendMode )
    EBlendModeType blendMode;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(blendMode, EBlendMode::BLEND);

    if (!argStream.HasErrors())
    {
        g_pCore->GetGraphics()->SetBlendMode(blendMode);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxGetBlendMode(lua_State* luaVM)
{
    //  string dxGetBlendMode ()
    EBlendModeType blendMode = g_pCore->GetGraphics()->GetBlendMode();
    lua_pushstring(luaVM, EnumToString(blendMode));
    return 1;
}

int CLuaDrawingDefs::DxSetAspectRatioAdjustmentEnabled(lua_State* luaVM)
{
    //  bool dxSetAspectRatioAdjustmentEnabled( bool enabled, float sourceRatio = 4/3 )
    bool  bEnabled;
    float fSourceRatio;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bEnabled);
    argStream.ReadNumber(fSourceRatio, 4 / 3.f);

    if (!g_bAllowAspectRatioAdjustment)
        argStream.SetCustomError("Function can only be used inside certain events");

    if (!argStream.HasErrors())
    {
        g_pCore->GetGraphics()->SetAspectRatioAdjustmentEnabled(bEnabled, fSourceRatio);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaDrawingDefs::DxIsAspectRatioAdjustmentEnabled(lua_State* luaVM)
{
    //  bool, float dxIsAspectRatioAdjustmentEnabled()
    bool  bEnabled = g_pCore->GetGraphics()->IsAspectRatioAdjustmentEnabled();
    float fSourceRatio = g_pCore->GetGraphics()->GetAspectRatioAdjustmentSourceRatio();
    lua_pushboolean(luaVM, bEnabled);
    lua_pushnumber(luaVM, fSourceRatio);
    return 2;
}

int CLuaDrawingDefs::DxSetTextureEdge(lua_State* luaVM)
{
    //  bool dxSetTextureEdge ( texture theTexture, string textureEdge [, int border-color )
    CClientTexture* pTexture;
    ETextureAddress textureAddress;
    SColor          borderColor;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pTexture);
    argStream.ReadEnumString(textureAddress);
    argStream.ReadColor(borderColor, 0);

    if (!argStream.HasErrors())
    {
        pTexture->GetMaterialItem()->m_TextureAddress = textureAddress;
        pTexture->GetMaterialItem()->m_uiBorderColor = borderColor;
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaDrawingDefs::DxDrawWiredSphere(lua_State* const luaVM, const CVector position, const float radius, std::optional<SColor> color,
                                        const std::optional<float> lineWidth, const std::optional<unsigned int> iterations)
{
    // Greater than 4, crash the game
    if (iterations.has_value() && (*iterations == 0 || *iterations > 4))
        throw std::invalid_argument("Iterations must be between 1 and 4");

    if (!color)
        color = SColorARGB(64, 255, 0, 0);

    g_pCore->GetGraphics()->DrawWiredSphere(position, radius, color.value(), lineWidth.value_or(1), iterations.value_or(1));
    return true;
}

bool CLuaDrawingDefs::DxDrawModel3D(std::uint32_t modelID, CVector position, CVector rotation, const std::optional<CVector> scale, const std::optional<float> lighting)
{
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(modelID);
    if (!pModelInfo)
        throw std::invalid_argument("Invalid model ID");

    if (auto modelType = pModelInfo->GetModelType();
        modelType == eModelInfoType::UNKNOWN || modelType == eModelInfoType::VEHICLE || modelType == eModelInfoType::PED)
    {
        throw std::invalid_argument("Invalid model type");
    }

    ConvertDegreesToRadians(rotation);

    return g_pClientGame->GetModelRenderer()->EnqueueModel(pModelInfo,
        CMatrix{position, rotation, scale.value_or(CVector{1.0f, 1.0f, 1.0f})}, lighting.value_or(0.0f));
}
