/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTextDisplayFadeEffect.cpp
*  PURPOSE:     Client text display fade effect class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

CClientTextDisplayFadeEffect::CClientTextDisplayFadeEffect ( unsigned long ulTime, unsigned long ulFlags )
{
    // Init
    m_ulStart = CClientTime::GetTime ();
    m_ulTime = ulTime;

    // If we haven't got any flags, fade in and out
    if ( ulFlags == 0 )
    {
        m_bFadeIn = true;
        m_bFadeOut = true;
    }
    else
    {
        m_bFadeIn = ( ulFlags & TEXTDISPLAY_FADE_IN ) ? true : false;
        m_bFadeOut = ( ulFlags & TEXTDISPLAY_FADE_OUT ) ? true : false;
    }
}


void CClientTextDisplayFadeEffect::DoPulse ( void )
{
    // Grab the current time
    unsigned long ulCurrentTime = CClientTime::GetTime ();

    // Default to 100% alpha
    m_pTextDisplay->SetColorAlpha ( 0xFF );

    // If we're supposed to fade in
    if ( m_bFadeIn )
    {
        // If we're within m_ulTime from when we started, process the fadein
        if ( m_ulStart + m_ulTime > ulCurrentTime )
        {
            unsigned long ulTimePassed = ulCurrentTime - m_ulStart;
            float fPercentageOfTime = static_cast < float > ( ulTimePassed ) / static_cast < float > ( m_ulTime );
            unsigned char ucAlpha = static_cast < unsigned char > ( 255.0f * fPercentageOfTime );
            m_pTextDisplay->SetColorAlpha ( ucAlpha );
        }
    }

    // If we're supposed to fade out
    if ( m_bFadeOut )
    {
        // If we're within m_ulTime from the end, process the fadeout
        if ( m_pTextDisplay->GetExpirationTime () - m_ulTime < ulCurrentTime )
        {
            unsigned long ulTimeLeft = m_pTextDisplay->GetExpirationTime () - ulCurrentTime;
            float fPercentageOfTime = static_cast < float > ( ulTimeLeft ) / static_cast < float > ( m_ulTime );
            unsigned char ucAlpha = static_cast < unsigned char > ( 255.0f * fPercentageOfTime );
            m_pTextDisplay->SetColorAlpha ( ucAlpha );
        }
    }
}
