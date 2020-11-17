/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIFont_Impl.cpp
 *  PURPOSE:     Font type class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using namespace GUINew;

CGUIFont_Impl::CGUIFont_Impl(CGUI_Impl* pGUI, const char* szFontName, const char* szFontFile, unsigned int uSize, unsigned int uFlags, bool bAutoScale, bool isWinFont)
{
    // Store the fontmanager and create a font with the given attributes
    m_pFontManager = pGUI->GetFontManager();
    m_pFont = NULL;

    // Store the renderer's geometry buffer
    m_pGeometryBuffer = pGUI->GetGeometryBuffer();

    CEGUI::AutoScaledMode scaleMode = bAutoScale ? CEGUI::AutoScaledMode::ASM_Both : CEGUI::AutoScaledMode::ASM_Disabled;

    while (!m_pFont)
    {
        try
        {
            m_pFont = &m_pFontManager->createFreeTypeFont(szFontName, uSize, true, szFontFile, isWinFont ? "absolute" : "mta_fonts", scaleMode, CEGUI::Sizef(1024, 768));
        }
        catch (CEGUI::RendererException)
        {
            // Reduce size until it can fit into a texture
            if (--uSize == 1)
                throw;
        }
    }

    // Define our glyphs (no longer required)
    //m_pFont->setInitialFontGlyphs(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");

    // Set default attributes
    SetNativeResolution(1024, 768);
    SetAutoScalingEnabled(bAutoScale);
}

CGUIFont_Impl::~CGUIFont_Impl()
{
    m_pFontManager->destroy(*m_pFont);
}

void CGUIFont_Impl::DrawTextString(const char* szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor,
                                   float fScaleX, float fScaleY)
{
    m_pFont->drawText(*m_pGeometryBuffer, szText ? CGUI_Impl::GetUTFString(szText) : CEGUI::String(),
        CEGUI::Vector2(DrawArea.fX1, DrawArea.fY1),
        &CEGUI::Rectf(CEGUI::Vector2(ClipRect.fX1, ClipRect.fY1), CEGUI::Vector2(ClipRect.fX2, ClipRect.fY2)),
        CEGUI::ColourRect((CEGUI::argb_t)ulColor),
        0, fScaleX, fScaleY);
}

void CGUIFont_Impl::SetAntiAliasingEnabled(bool bAntialiased)
{
    //m_pFont->setAntiAliased(bAntialiased);
}

bool CGUIFont_Impl::IsAntiAliasingEnabled()
{
    //return m_pFont->isAntiAliased();
    return false;
}

void CGUIFont_Impl::SetAutoScalingEnabled(bool bAutoScaled)
{
    CEGUI::AutoScaledMode scaleMode = bAutoScaled ? CEGUI::AutoScaledMode::ASM_Both : CEGUI::AutoScaledMode::ASM_Disabled;
    m_pFont->setAutoScaled(scaleMode);
}

bool CGUIFont_Impl::IsAutoScalingEnabled()
{
    CEGUI::AutoScaledMode scaledMode = m_pFont->getAutoScaled();
    return CEGUI::AutoScaledMode::ASM_Disabled ? false : true;
}

void CGUIFont_Impl::SetNativeResolution(int iX, int iY)
{
    m_pFont->setNativeResolution(CEGUI::Size(static_cast<float>(iX), static_cast<float>(iY)));
}

float CGUIFont_Impl::GetCharacterWidth(int iChar, float fScale)
{
    char szBuf[2];
    szBuf[0] = iChar;
    szBuf[1] = 0;

    return m_pFont->getTextExtent(szBuf, fScale);
}

float CGUIFont_Impl::GetFontHeight(float fScale)
{
    float fHeight = m_pFont->getFontHeight(fScale);            // average height.. not the maximum height for long characters such as 'g' or 'j'
    fHeight += 2.0f;                                           // so hack it

    return fHeight;
}

float CGUIFont_Impl::GetTextExtent(const char* szText, float fScale)
{
    return m_pFont->getTextExtent(CGUI_Impl::GetUTFString(szText), fScale);
}

CEGUI::Font* CGUIFont_Impl::GetFont()
{
    return m_pFont;
}
