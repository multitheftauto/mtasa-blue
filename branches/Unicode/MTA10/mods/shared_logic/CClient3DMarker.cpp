/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClient3DMarker.cpp
*  PURPOSE:     3D marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*****************************************************************************/

#include <StdInc.h>

extern CClientGame* g_pClientGame;

CClient3DMarker::CClient3DMarker ( CClientMarker * pThis )
{
    // Init
    m_pThis = pThis;
    m_bMarkerStreamedIn = false;
    m_bVisible = true;
    m_Color = SColorRGBA ( 255, 0, 0, 128 );
    m_fSize = 4.0f;
    m_dwType = MARKER3D_CYLINDER2;
    m_pMarker = NULL;
    m_ulIdentifier = (DWORD)this;
}


CClient3DMarker::~CClient3DMarker ( void )
{

}


unsigned long CClient3DMarker::Get3DMarkerType ( void )
{
    switch ( m_dwType )
    {
        case MARKER3D_CYLINDER2:
            return CClient3DMarker::TYPE_CYLINDER;

        case MARKER3D_ARROW:
            return CClient3DMarker::TYPE_ARROW;

        default:
            return CClient3DMarker::TYPE_INVALID;
    }
}


void CClient3DMarker::Set3DMarkerType ( unsigned long ulType )
{
    switch ( ulType )
    {
        case CClient3DMarker::TYPE_CYLINDER:
            m_dwType = MARKER3D_CYLINDER2;
            break;

        case CClient3DMarker::TYPE_ARROW:
            m_dwType = MARKER3D_ARROW;
            break;

        default:
            m_dwType = MARKER3D_CYLINDER2;
            break;
    }
}


bool CClient3DMarker::IsHit ( const CVector& vecPosition ) const
{
    return IsPointNearPoint3D ( m_Matrix.vPos, vecPosition, m_fSize + 4 );
}


void CClient3DMarker::StreamIn ( void )
{
    // We're now streamed in
    m_bMarkerStreamedIn = true;
}


void CClient3DMarker::StreamOut ( void )
{
    // We're no longer streamed in
    m_bMarkerStreamedIn = false;
    m_pMarker = NULL;
}


void CClient3DMarker::DoPulse ( void )
{    
    if ( m_bMarkerStreamedIn && m_bVisible && m_pThis->GetInterior () == g_pGame->GetWorld ()->GetCurrentArea () )
    {       
        SColor color = GetColor ();
        m_pMarker = g_pGame->Get3DMarkers ()->CreateMarker ( m_ulIdentifier, static_cast < e3DMarkerType > ( m_dwType ), &m_Matrix.vPos, m_fSize, 0.2f, color.R, color.G, color.B, color.A );
        if ( m_pMarker )
        {
            // Make sure it doesn't get cleaned up
            m_pMarker->SetActive ();
            m_pMarker->SetColor ( m_Color );
            m_pMarker->SetSize ( m_fSize );
            m_pMarker->SetMatrix ( &m_Matrix );

            // This appears to do nothing
            g_pGame->GetVisibilityPlugins ()->SetClumpAlpha ( ( RpClump * ) m_pMarker->GetRwObject (), m_Color.A );
        }
    }
}
