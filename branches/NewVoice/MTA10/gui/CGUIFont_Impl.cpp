/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIFont_Impl.cpp
*  PURPOSE:     Font type class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CGUIFont_Impl::CGUIFont_Impl ( CGUI_Impl* pGUI, const char* szFontName, const char* szFontFile, unsigned int uSize, unsigned int uFlags, bool bAutoScale )
{
    // Store the fontmanager and create a font with the given attributes
    m_pFontManager = pGUI->GetFontManager ();
    m_pFont = NULL;
    while ( !m_pFont )
    {
        try
        {
            m_pFont = m_pFontManager->createFont ( szFontName, szFontFile, uSize, uFlags, bAutoScale, 1024, 768 );
        }
        catch ( CEGUI::RendererException )
        {
            // Reduce size until it can fit into a texture
            if ( --uSize == 1 )
                throw;
        }
    }

    // Define our glyphs
    defineFontGlyphs( " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" );

    // Set default attributes
    SetNativeResolution ( 1024, 768 );
    SetAutoScalingEnabled ( bAutoScale );
}


CGUIFont_Impl::~CGUIFont_Impl ( void )
{
    m_pFontManager->destroyFont ( m_pFont );
}


void CGUIFont_Impl::SetAntiAliasingEnabled ( bool bAntialiased )
{
    m_pFont->setAntiAliased ( bAntialiased );
}


void CGUIFont_Impl::DrawTextString ( const char *szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX, float fScaleY )
{
    CEGUI::TextFormatting fmt;

    if ( ulFormat == DT_CENTER )
        fmt = CEGUI::Centred;
    else if ( ulFormat == DT_RIGHT )
        fmt = CEGUI::RightAligned;
    else
        fmt = CEGUI::LeftAligned;

    m_pFont->drawText ( szText ? CEGUI::String ( szText ) : CEGUI::String (), CEGUI::Rect ( DrawArea.fX1, DrawArea.fY1, DrawArea.fX2, DrawArea.fY2 ), fZ, CEGUI::Rect ( ClipRect.fX1, ClipRect.fY1, ClipRect.fX2, ClipRect.fY2 ), fmt, CEGUI::ColourRect ( CEGUI::colour ( ( CEGUI::argb_t ) ulColor ) ), fScaleX, fScaleY );
}

bool CGUIFont_Impl::IsAntiAliasingEnabled ( void )
{
    return m_pFont->isAntiAliased ();
}


void CGUIFont_Impl::SetAutoScalingEnabled ( bool bAutoScaled )
{
    m_pFont->setAutoScalingEnabled ( bAutoScaled );
}


bool CGUIFont_Impl::IsAutoScalingEnabled ( void )
{
    return m_pFont->isAutoScaled ();
}


void CGUIFont_Impl::SetNativeResolution ( int iX, int iY )
{
    m_pFont->setNativeResolution ( CEGUI::Size ( static_cast < float > ( iX ), static_cast < float > ( iY ) ) );
}


float CGUIFont_Impl::GetCharacterWidth ( int iChar, float fScale )
{
    char szBuf [2];
    szBuf [0] = iChar;
    szBuf [1] = 0;

    return m_pFont->getTextExtent ( szBuf, fScale );
}


float CGUIFont_Impl::GetFontHeight ( float fScale )
{
    float fHeight = m_pFont->getFontHeight ( fScale );  // average height.. not the maximum height for long characters such as 'g' or 'j'
    fHeight += 2.0f;                                    // so hack it

    return fHeight;
}


float CGUIFont_Impl::GetTextExtent ( const char* szText, float fScale )
{
    return m_pFont->getTextExtent ( CGUI_Impl::GetUTFString(szText), fScale );
}


CEGUI::Font* CGUIFont_Impl::GetFont ( void )
{
    return m_pFont;
}

void CGUIFont_Impl::defineFontGlyphs (unsigned int uExtraGlyphs[])
{
        CEGUI::String glyphSet; // (?) we needs temporary (CEGUI::String) string to define our glyphs correctly
        
        for ( unsigned int g = 0; uExtraGlyphs[g] >= 32; ++g ) // (?) adding extra glyphs codes to temp string
	    {
            glyphSet += (CEGUI::utf32) uExtraGlyphs[g];
	    }
        
        m_pFont->defineFontGlyphs( glyphSet ); // (?) defining font's glyphs
}

void CGUIFont_Impl::defineFontGlyphs (const char *szExtraGlyphs)
{
    m_pFont->defineFontGlyphs( (CEGUI::utf8*)szExtraGlyphs );
}

bool CGUIFont_Impl::isGlyphBeingUsed (unsigned long ulGlyph)
{
    return m_pFont->isGlyphBeingUsed ( ulGlyph );
}