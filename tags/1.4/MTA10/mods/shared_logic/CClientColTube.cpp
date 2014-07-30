/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColTube.cpp
*  PURPOSE:     Tube-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include <StdInc.h>

CClientColTube::CClientColTube ( CClientManager* pManager, ElementID ID, const CVector& vecPosition, float fRadius, float fHeight ) : ClassInit ( this ), CClientColShape ( pManager, ID )
{
    m_vecPosition = vecPosition;
    m_fRadius = fRadius;
    m_fHeight = fHeight;
    UpdateSpatialData ();
}


bool CClientColTube::DoHitDetection ( const CVector& vecNowPosition, float fRadius )
{
    // FIXME: What about radius in height?

    // First see if we're within the circle. Then see if we're within its height
    return ( IsPointNearPoint2D ( vecNowPosition, m_vecPosition, fRadius + m_fRadius ) &&
             vecNowPosition.fZ >= m_vecPosition.fZ &&
             vecNowPosition.fZ <= m_vecPosition.fZ + m_fHeight );
}


CSphere CClientColTube::GetWorldBoundingSphere ( void )
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX;
    sphere.vecPosition.fY = m_vecPosition.fY;
    sphere.vecPosition.fZ = m_vecPosition.fZ + m_fHeight * 0.5f;
    sphere.fRadius = Max ( m_fRadius, m_fHeight * 0.5f );
    return sphere;
}


//
// Draw wireframe tube
//
void CClientColTube::DebugRender ( const CVector& vecPosition, float fDrawRadius )
{
    SColorARGB color ( 128, 0, 0, 255 );
    float fLineWidth = 4.f + pow ( m_fRadius, 0.5f );
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics ();

    // Calc required detail level
    uint uiNumPoints = Max ( 6, Round ( sqrt ( m_fRadius ) * 2.0f ) );
    uint uiNumSlices = Max ( 2, Round ( sqrt ( m_fHeight ) * 2.0f ) );

    // Get the slice outline
    std::vector < CVector > vertexList;
    for ( uint p = 0; p < uiNumPoints; p++ )
    {
        float u = p / (float)uiNumPoints;
        CVector2D vecPoint = CVector2D ( cos ( u * PI * 2 ), sin ( u * PI * 2 ) );
        vertexList.push_back ( CVector ( vecPoint.fX, vecPoint.fY, 1 ) );
    }

    // Draw Slices
    {
        CVector vecMult ( m_fRadius, m_fRadius, m_fHeight );
        CVector vecAdd = m_vecPosition;

        for ( uint s = 0 ; s < uiNumSlices; s++ )
        {
            vecMult.fZ = m_fHeight * ( s / (float)( uiNumSlices - 1 ) );
            for ( uint i = 0 ; i < vertexList.size () ; i++ )
            {
                const CVector& vecBegin = vertexList[ i ] * vecMult + vecAdd;
                const CVector& vecEnd = vertexList[ ( i + 1 ) % uiNumPoints ] * vecMult + vecAdd;
                pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
            }
        }
    }

    // Draw lines from bottom to top
    {
        CVector vecMultB ( m_fRadius, m_fRadius, 0 );
        CVector vecAdd = m_vecPosition;
        CVector vecMultT ( m_fRadius, m_fRadius, m_fHeight );

        for ( uint i = 0 ; i < vertexList.size () ; i++ )
        {
            const CVector& vecBegin = vertexList[ i ] * vecMultB + vecAdd;
            const CVector& vecEnd = vertexList[ i ] * vecMultT + vecAdd;
            pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
        }
    }

    // Draw top cap
    {
        CVector vecMult ( m_fRadius, m_fRadius, m_fHeight );
        CVector vecAdd = m_vecPosition;

        CVector vecBegin ( m_vecPosition.fX, m_vecPosition.fY, m_vecPosition.fZ + m_fHeight );
        for ( uint i = 0 ; i < vertexList.size () ; i++ )
        {
            const CVector& vecEnd = vertexList[ i ] * vecMult + vecAdd;
            pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
        }
    }

    // Draw bottom cap
    {
        CVector vecMult ( m_fRadius, m_fRadius, 0 );
        CVector vecAdd = m_vecPosition;

        CVector vecBegin = m_vecPosition;
        for ( uint i = 0 ; i < vertexList.size () ; i++ )
        {
            const CVector& vecEnd = vertexList[ i ] * vecMult + vecAdd;
            pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
        }
    }
}
