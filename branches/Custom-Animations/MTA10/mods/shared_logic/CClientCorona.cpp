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
    CClientEntityRefManager::AddEntityRefs ( ENTITY_REF_DEBUG ( this, "CClientCorona" ), &m_pThis, NULL );

    // Init
    m_pThis = pThis;
    m_bStreamedIn = false;
    m_bVisible = true;
    m_Color = SColorRGBA ( 255, 0, 0, 255 );
    m_fSize = 4.0f;
    m_pCoronas = g_pGame->GetCoronas ();

    // Pick an unique identifier
    static unsigned long ulIdentifier = 0xFFFFFFFF;
    m_ulIdentifier = --ulIdentifier;
}


CClientCorona::~CClientCorona ( void )
{
    // Disable our corona
    CRegisteredCorona* pCorona = m_pCoronas->CreateCorona ( m_ulIdentifier, &m_Matrix.vPos );
    if ( pCorona )
    {
        pCorona->Disable ();
    }
    CClientEntityRefManager::RemoveEntityRefs ( 0, &m_pThis, NULL );
}


bool CClientCorona::IsHit ( const CVector& vecPosition ) const
{
    return IsPointNearPoint3D ( m_Matrix.vPos, vecPosition, m_fSize + 4 );
}


void CClientCorona::StreamIn ( void )
{
    // We're now streamed in
    m_bStreamedIn = true;
}


void CClientCorona::StreamOut ( void )
{
    // Disable the corona
    CRegisteredCorona* pCorona = m_pCoronas->CreateCorona ( m_ulIdentifier, &m_Matrix.vPos );
    if ( pCorona )
    {
        pCorona->Disable();
    }

    // No longer streamed in
    m_bStreamedIn = false;
}


void CClientCorona::DoPulse ( void )
{
    CRegisteredCorona* pCorona = m_pCoronas->CreateCorona ( m_ulIdentifier, &m_Matrix.vPos );
    if ( !pCorona )
        return;

    if ( IsStreamedIn () && m_bVisible )
    {
        SColor color = GetColor ();
        if ( m_pThis->GetInterior () != g_pGame->GetWorld ()->GetCurrentArea () )
            color.A = 0;
        pCorona->SetColor ( color.R, color.G, color.B, color.A );
        pCorona->SetSize ( m_fSize );
    }
    else
    {
        pCorona->Disable ();
    }
}