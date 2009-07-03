/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCorona.cpp
*  PURPOSE:     Corona marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include <StdInc.h>

CClientCorona::CClientCorona ( CClientMarker * pThis )
{
    // Init
    m_pThis = pThis;
    m_bStreamedIn = false;
    m_bVisible = true;
    m_rgbaColor = COLOR_RGBA ( 255, 0, 0, 255 );
    m_fSize = 4.0f;
    m_pCoronas = g_pGame->GetCoronas ();

    // Pick an unique identifier
    static unsigned long ulIdentifier = 0xFFFFFFFF;
    m_ulIdentifier = --ulIdentifier;
}


CClientCorona::~CClientCorona ( void )
{
    // Disable our corona
    CRegisteredCorona* pCorona = m_pCoronas->CreateCorona ( m_ulIdentifier, &m_vecPosition );
    if ( pCorona )
    {
        pCorona->Disable ();
    }
}


bool CClientCorona::IsHit ( const CVector& vecPosition ) const
{
    return IsPointNearPoint3D ( m_vecPosition, vecPosition, m_fSize + 4 );
}


void CClientCorona::SetPosition ( const CVector& vecPosition )
{
    m_vecPosition = vecPosition;
}


void CClientCorona::GetColor ( unsigned char& Red, unsigned char& Green, unsigned char& Blue, unsigned char& Alpha ) const
{
    Red   = m_rgbaColor.R;
    Green = m_rgbaColor.G;
    Blue  = m_rgbaColor.B;
    Alpha = m_rgbaColor.A;
}


void CClientCorona::SetColor ( unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha )
{
    SetColor ( SColorARGB ( Alpha, Red, Green, Blue ) );
}


void CClientCorona::SetColor ( unsigned long ulColor )
{
    // Different from our current color?
    if ( m_rgbaColor != ulColor )
    {
        // Set it
        m_rgbaColor = ulColor;
    }
}


void CClientCorona::StreamIn ( void )
{
    // We're now streamed in
    m_bStreamedIn = true;
}


void CClientCorona::StreamOut ( void )
{
    // Disable the corona
    CRegisteredCorona* pCorona = m_pCoronas->CreateCorona ( m_ulIdentifier, &m_vecPosition );
    if ( pCorona )
    {
        pCorona->Disable();
    }

    // No longer streamed in
    m_bStreamedIn = false;
}


void CClientCorona::DoPulse ( void )
{
    if ( IsStreamedIn () && m_bVisible && m_pThis->GetInterior () == g_pGame->GetWorld ()->GetCurrentArea () )
    {
        // Draw it and set the properties
        CRegisteredCorona* pCorona = m_pCoronas->CreateCorona ( m_ulIdentifier, &m_vecPosition );
        if ( pCorona )
        {
            unsigned char R, G, B, A;
            GetColor ( R, G, B, A );
            pCorona->SetColor ( R, G, B, A );
            pCorona->SetSize ( m_fSize );
        }
    }
}