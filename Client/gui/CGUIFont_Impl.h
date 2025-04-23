/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIFont_Impl.h
 *  PURPOSE:     Font type class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIFont.h>
#include <CEGUI.h>

class CGUIFont_Impl : public CGUIFont
{
public:
    CGUIFont_Impl(class CGUI_Impl* pManager, const char* szFontName, const char* szFontFile, unsigned int uSize = 8, unsigned int uFlags = 0,
                  bool bAutoScale = false);
    ~CGUIFont_Impl();

    void SetAntiAliasingEnabled(bool bAntialiased);
    bool IsAntiAliasingEnabled();
    void SetAutoScalingEnabled(bool bAutoScaled);
    bool IsAutoScalingEnabled();

    void SetNativeResolution(int iX, int iY);

    float GetCharacterWidth(int iChar, float fScale = 1.0f);
    float GetFontHeight(float fScale = 1.0f);
    float GetTextExtent(const char* szText, float fScale = 1.0f);

    CEGUI::Font* GetFont();

    void DrawTextString(const char* szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX,
                        float fScaleY);

private:
    CEGUI::Font*        m_pFont;
    CEGUI::FontManager* m_pFontManager;
};
