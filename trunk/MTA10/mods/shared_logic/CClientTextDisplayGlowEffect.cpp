/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTextDisplayGlowEffect.cpp
*  PURPOSE:     Client text display glow effect class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

CClientTextDisplayGlowEffect::CClientTextDisplayGlowEffect ( unsigned long ulPulseTime, unsigned char ucLowestAlpha, unsigned char ucHighestAlpha )
{
    // Init
    m_ulPulseTime = ulPulseTime;
    m_ucLowestAlpha = ucLowestAlpha;
    m_ucHighestAlpha = ucHighestAlpha;

    m_bPulsingOut = true;
    m_ulLastPulse = 0;
}


void CClientTextDisplayGlowEffect::DoPulse ( void )
{
    // Time for the next pulse?
    unsigned long ulCurrentTime = CClientTime::GetTime ();
    if ( ulCurrentTime >= m_ulLastPulse + m_ulPulseTime )
    {
        m_ulLastPulse = ulCurrentTime;
        m_bPulsingOut = true;
    }

    // 
    if ( m_bPulsingOut )
    {
        // Time to pulse in again?
        if ( ulCurrentTime > m_ulLastPulse + m_ulPulseTime / 2 )
        {
            m_bPulsingOut = false;
        }
        else
        {
            unsigned long ulTimePassed = ulCurrentTime - m_ulLastPulse;
            float fDeltaAlpha = static_cast < float > ( m_ucHighestAlpha - m_ucLowestAlpha );
            float fPercentageOfTime = static_cast < float > ( ulTimePassed ) / static_cast < float > ( m_ulPulseTime / 2 );
            unsigned char ucAlpha = static_cast < unsigned char > ( fDeltaAlpha * fPercentageOfTime );
            m_pTextDisplay->SetColorAlpha ( m_ucHighestAlpha - ucAlpha );
        }
    }

    // Are we pulsing in again?
    if ( !m_bPulsingOut )
    {
        unsigned long ulTimePassed = ulCurrentTime - m_ulLastPulse + m_ulPulseTime / 2;
        float fDeltaAlpha = static_cast < float > ( m_ucHighestAlpha - m_ucLowestAlpha );
        float fPercentageOfTime = static_cast < float > ( ulTimePassed ) / static_cast < float > ( m_ulPulseTime / 2 );
        unsigned char ucAlpha = static_cast < unsigned char > ( fDeltaAlpha * fPercentageOfTime );
        m_pTextDisplay->SetColorAlpha ( m_ucLowestAlpha + ucAlpha );
    }
}
