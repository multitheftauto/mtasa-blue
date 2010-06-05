/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTextDisplayBackgroundEffect.cpp
*  PURPOSE:     Client text display background effect class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

CClientTextDisplayBackgroundEffect::CClientTextDisplayBackgroundEffect ( void )
{
    m_ulColor = 0;
}


CClientTextDisplayBackgroundEffect::CClientTextDisplayBackgroundEffect ( unsigned long ulColor )
{
    m_ulColor = ulColor;
}


void CClientTextDisplayBackgroundEffect::DoPulse ( void )
{
    // Check that the text display exists and that it's visible
    if ( m_pTextDisplay && m_pTextDisplay->IsVisible () )
    {
        // Got a caption?
        const char* szCaption = m_pTextDisplay->GetCaptionPointer ();
        if ( szCaption )
        {
            // Calculate the width and the height of the text
            float fScale = m_pTextDisplay->GetScale ();
            int iWidth = g_pCore->GetGraphics ()->GetTextExtent ( szCaption, fScale ) + 4;
            int iHeight = g_pCore->GetGraphics ()->GetTextHeight ( fScale ) + 1;

            // Grab the text position and convert it to pixels
            CVector vecPosition = m_pTextDisplay->GetPosition ();
            vecPosition.fX *= static_cast < float > ( g_pCore->GetGraphics ()->GetViewportWidth () );
            vecPosition.fY *= static_cast < float > ( g_pCore->GetGraphics ()->GetViewportHeight () );

            // Center?
            if ( m_pTextDisplay->GetFormat () == DT_CENTER )
            {
                vecPosition.fX -= iWidth / 2;
            }
            else
            {
                vecPosition.fX -= 2;
            }

            // Grab the text alpha level and patch it with our color so we can fade with it
            float fTextPercent = static_cast < float > ( m_pTextDisplay->GetColorAlpha () ) / 255.0f;
            float fOurAlpha = static_cast < float > ( ( 0xFF000000 & m_ulColor ) >> 24 );
            float fAlpha = fTextPercent * fOurAlpha;
            unsigned long ulColor = ( static_cast < unsigned long > ( fAlpha ) << 24 ) | ( m_ulColor & 0x00FFFFFF );

            // Draw the polygon
            g_pGame->GetHud ()->Draw2DPolygon ( vecPosition.fX, vecPosition.fY,
                                                vecPosition.fX + iWidth, vecPosition.fY,
                                                vecPosition.fX, vecPosition.fY + iHeight,
                                                vecPosition.fX + iWidth, vecPosition.fY + iHeight,
                                                ulColor );
        }
    }
}
