/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPointLights.cpp
*  PURPOSE:     PointLights entity class
*  DEVELOPERS:  Alexander Romanov <lexr128@gmail.com>
*
*****************************************************************************/

#include <StdInc.h>

CClientPointLights::CClientPointLights ( class CClientManager* pManager, ElementID ID ) : ClassInit ( this ), CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    m_pPointLightsManager = pManager->GetPointLightsManager ();
    m_iMode = 0;
    m_fRadius = 3.0f;
    m_Color = SColorRGBA ( 255, 0, 0, 0 );
    m_vecDirection = CVector ( );
    m_bCreatesShadow = false;
    m_pAffected = NULL;
    m_bStreamedIn = true;

    SetTypeName ( "light" );

    // Make sure we're visible/invisible according to our dimension
    RelateDimension ( m_pPointLightsManager->GetDimension () );

    // Add us to the manager's list
    m_pPointLightsManager->AddToList ( this );
}


CClientPointLights::~CClientPointLights ( void )
{
    // Remove us from the manager's list
    Unlink ();
}


void CClientPointLights::Unlink ( void )
{
    m_pPointLightsManager->RemoveFromList ( this );
}


void CClientPointLights::DoPulse ( void )
{
    // Update our position/rotation if we're attached
    DoAttaching ();

    // Suppose to show?
    if ( m_bStreamedIn )
    {
        g_pGame->GetPointLights ()->AddLight ( m_iMode, m_vecPosition, m_vecDirection, m_fRadius, m_Color, 0, m_bCreatesShadow, 0 );
    }
}


void CClientPointLights::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
    RelateDimension ( m_pPointLightsManager->GetDimension () );
}


void CClientPointLights::RelateDimension ( unsigned short usDimension )
{
    m_bStreamedIn = ( usDimension == m_usDimension );
}