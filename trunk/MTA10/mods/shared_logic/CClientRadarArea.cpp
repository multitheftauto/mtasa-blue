/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarArea.cpp
*  PURPOSE:     Radar area entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include <StdInc.h>

CClientRadarArea::CClientRadarArea ( class CClientManager* pManager, ElementID ID ) : CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    m_pRadarAreaManager = pManager->GetRadarAreaManager ();
    m_Color = SColorRGBA ( 255, 255, 255, 255 );
    m_bFlashing = false;
    m_ulFlashCycleStart = 0;
    m_bStreamedIn = true;

    SetTypeName ( "radararea" );

    // Make sure we're visible/invisible according to our dimension
    RelateDimension ( m_pRadarAreaManager->GetDimension () );

    // Add us to the manager's list
    m_pRadarAreaManager->AddToList ( this );
}


CClientRadarArea::~CClientRadarArea ( void )
{
    // Remove us from the manager's list
    Unlink ();
}


void CClientRadarArea::Unlink ( void )
{
    m_pRadarAreaManager->RemoveFromList ( this );
}


void CClientRadarArea::DoPulse ( void )
{
    DoPulse ( true );
}


void CClientRadarArea::DoPulse ( bool bRender )
{
    #define RADAR_FLASH_CYCLETIME 1000

    // Suppose to show?
    if ( m_bStreamedIn )
    {
        // If it's flashing, calculate a new alpha
        SColor color = m_Color;
     
        if ( m_bFlashing )
        {
            // Time to start a new cycle?
            unsigned long ulCurrentTime = CClientTime::GetTime ();
            if ( m_ulFlashCycleStart == 0 )
            {
                m_ulFlashCycleStart = ulCurrentTime;
            }
            // Time to end the cycle and start a new?
            else if ( ulCurrentTime >= m_ulFlashCycleStart + RADAR_FLASH_CYCLETIME )
            {
                m_ulFlashCycleStart = ulCurrentTime;
            }

            // Calculate the alpha based on the last cycle time and the cycle intervals
            float fAlphaFactor;

            // We're in the fade in part of the cycle?
            if ( ulCurrentTime >= m_ulFlashCycleStart + RADAR_FLASH_CYCLETIME / 2 )
            {
                // Calculate the alpha-factor
                fAlphaFactor = static_cast < float > ( ulCurrentTime - m_ulFlashCycleStart - RADAR_FLASH_CYCLETIME / 2 ) / ( RADAR_FLASH_CYCLETIME / 2 );
            }
            else
            {
                // Calculate the alpha-factor
                fAlphaFactor = 1.0f - static_cast < float > ( ulCurrentTime - m_ulFlashCycleStart ) / ( RADAR_FLASH_CYCLETIME / 2 );
            }

            // Multiply the alpha-factor with the alpha we're supposed to have to find what alpha to use and set it
            color.A = static_cast < unsigned char > ( fAlphaFactor * static_cast < float > ( color.A ) );
        }

        // Only render the radar area if we are told to
        if ( bRender )
        {
            // Draw it
            g_pGame->GetRadar ()->DrawAreaOnRadar ( m_vecPosition.fX + m_vecSize.fX, m_vecPosition.fY,
                                                    m_vecPosition.fX, m_vecPosition.fY + m_vecSize.fY,
                                                    color );
        }
    }
}


void CClientRadarArea::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
    RelateDimension ( m_pRadarAreaManager->GetDimension () );
}


void CClientRadarArea::RelateDimension ( unsigned short usDimension )
{
    m_bStreamedIn = ( usDimension == m_usDimension );
}
