/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarMarker.cpp
*  PURPOSE:     Radar marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

#include "StdInc.h"

using SharedUtil::CalcMTASAPath;

CClientRadarMarker::CClientRadarMarker ( CClientManager* pManager, ElementID ID, short sOrdering, unsigned short usVisibleDistance ) : ClassInit ( this ), CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    m_pRadarMarkerManager = pManager->GetRadarMarkerManager ();

    SetTypeName ( "blip" );

    m_bIsVisible = true;
    m_pMarker = NULL;
    m_usScale = 2;
    m_ulSprite = 0;
    m_Color = SColorRGBA ( 0, 0, 255, 255 );
    m_sOrdering = sOrdering;
    m_usVisibleDistance = usVisibleDistance;

    // Add us to the radar marker manager list
    m_pRadarMarkerManager->AddToList ( this );

    // Try to create it
    Create ();
}


CClientRadarMarker::~CClientRadarMarker ( void )
{
    // Try to destroy it
    Destroy ();

    // Remove us from the marker manager
    Unlink ();
}


void CClientRadarMarker::Unlink ( void )
{
    m_pRadarMarkerManager->RemoveFromList ( this );
}


void CClientRadarMarker::DoPulse ( void )
{    
    // Are we attached to something?
    if ( m_pAttachedToEntity )
    {
        CVector vecPosition;
        m_pAttachedToEntity->GetPosition ( vecPosition );        
        vecPosition += m_vecAttachedPosition;
        SetPosition ( vecPosition );

        // TODO: move to ::SetElementDimension
        if ( m_usDimension != m_pAttachedToEntity->GetDimension () )
        {
            SetDimension ( m_pAttachedToEntity->GetDimension () );
        }
    }

    // Are we in a visible distance?
    if ( IsInVisibleDistance () )
    {
        // Are we not created?
        if ( !m_pMarker )
        {
            // Are we visible and in the right dimension?
            if ( m_bIsVisible && m_usDimension == m_pManager->GetRadarMarkerManager ()->GetDimension () )
            {
                // Recreate our marker
                CreateMarker ();
            }
        }
    }
    else
    {
        // Are we created?
        if ( m_pMarker )
        {
            // Destroy our marker
            DestroyMarker ();
        }
    }
}


void CClientRadarMarker::SetPosition ( const CVector& vecPosition )
{
    if ( m_pMarker )
    {
        m_pMarker->SetPosition ( const_cast < CVector* > ( &vecPosition ) );
    }

    m_vecPosition = vecPosition;
}


void CClientRadarMarker::GetPosition ( CVector& vecPosition ) const
{
    if ( m_pMarker )
    {
        vecPosition = *m_pMarker->GetPosition ();
    }
    else
    {
        vecPosition = m_vecPosition;
    }
}


void CClientRadarMarker::SetScale ( unsigned short usScale )
{
    m_usScale = usScale;

    if ( m_pMarker )
    {
        m_pMarker->SetScale ( usScale );
    }
}


void CClientRadarMarker::SetColor ( const SColor color )
{
    // Store the color
    m_Color = color;

    // Set the color
    if ( m_pMarker )
    {
        // Supposed to be black? Make it almost black as entirely black
        // might make us hit some reserved colors. Which'd be unexpected.
        if ( m_Color.R == 0 &&
             m_Color.G == 0 &&
             m_Color.B == 0 )
        {
            m_pMarker->SetColor ( SColorRGBA ( 1, 1, 1, m_Color.A ) );
        }
        else
        {
            m_pMarker->SetColor ( m_Color );
        }
    }
}


void CClientRadarMarker::SetSprite ( unsigned long ulSprite )
{
    m_ulSprite = ulSprite;

    if ( m_pMarker )
    {
        m_pMarker->SetSprite ( static_cast < eMarkerSprite > ( ulSprite ) );
    }
}


void CClientRadarMarker::SetVisible ( bool bVisible )
{
    // Are we in the right dimension?
    if ( m_usDimension == m_pManager->GetRadarMarkerManager ()->GetDimension () )
    {
        // Visible now but weren't last time?
        if ( bVisible && !m_bIsVisible )
        {
            Create ();
        }

        // Invisible now but weren't last time? Destroy it
        else if ( !bVisible && m_bIsVisible )
        {
            Destroy ();
        }
    }

    // Update visibility
    m_bIsVisible = bVisible;
}


bool CClientRadarMarker::Create ( void )
{
    m_pManager->GetRadarMarkerManager ()->m_bOrderOnPulse = true;

    return true;
}


void CClientRadarMarker::InternalCreate ( void )
{
    // Not already got a marker?
    if ( !m_pMarker )
    {
        CreateMarker ();

        // Create the marker on the full size map
        SetSprite ( m_ulSprite );
    }
}


void CClientRadarMarker::Destroy ( void )
{
    DestroyMarker ();
}


void CClientRadarMarker::CreateMarker ( void )
{
    // Not already got a marker?
    if ( !m_pMarker )
    {
        if ( IsInVisibleDistance () )
        {
            // Create the marker
            m_pMarker = g_pGame->GetRadar ()->CreateMarker ( &m_vecPosition );
            if ( m_pMarker )
            {
                SColor color = m_Color;

                // Supposed to be black? Make it almost black as entirely black
                // might make us hit some reserved colors. Which'd be unexpected.
                if ( color.R == 0 &&
                     color.G == 0 &&
                     color.B == 0 )
                {
                    color = SColorRGBA ( 1, 1, 1, color.A );
                }

                // Set the properties
                m_pMarker->SetPosition ( &m_vecPosition );
                m_pMarker->SetScale ( m_usScale );
                m_pMarker->SetColor ( color );
                m_pMarker->SetSprite ( static_cast < eMarkerSprite > ( m_ulSprite ) );
            }
        }
    }
}


void CClientRadarMarker::DestroyMarker ( void )
{
    if ( m_pMarker )
    {
        m_pMarker->Remove ();
        m_pMarker = NULL;
    }
}


void CClientRadarMarker::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
    RelateDimension ( m_pManager->GetRadarMarkerManager ()->GetDimension () );
}


void CClientRadarMarker::RelateDimension ( unsigned short usDimension )
{
    // Are we visible?
    if ( m_bIsVisible )
    {
        if ( usDimension == m_usDimension )
        {
            Create ();
        }
        else
        {
            Destroy ();
        }
    }
}


void CClientRadarMarker::SetOrdering ( short sOrdering )
{
    if ( sOrdering != m_sOrdering )
    {
        m_sOrdering = sOrdering;
        m_pManager->GetRadarMarkerManager ()->m_bOrderOnPulse = true;
    }
}


bool CClientRadarMarker::IsInVisibleDistance ( void )
{
    float fDistance = DistanceBetweenPoints3D ( m_vecPosition, m_pRadarMarkerManager->m_vecCameraPosition );
    return ( fDistance <= m_usVisibleDistance );
}
