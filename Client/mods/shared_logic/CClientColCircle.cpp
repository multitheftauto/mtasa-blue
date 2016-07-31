/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColCircle.cpp
*  PURPOSE:     Circle-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

#include <StdInc.h>

using namespace std;

CClientColCircle::CClientColCircle ( CClientManager* pManager, ElementID ID, const CVector2D& vecPosition, float fRadius ) : ClassInit ( this ), CClientColShape ( pManager, ID )
{
    m_vecPosition.fX = vecPosition.fX;
    m_vecPosition.fY = vecPosition.fY;
    m_vecPosition.fZ = SPATIAL_2D_Z;
    m_fRadius = fRadius;
    UpdateSpatialData ();
}


bool CClientColCircle::DoHitDetection ( const CVector& vecNowPosition, float fRadius )
{
    // Do a simple distance check between now position and our position 
    return IsPointNearPoint2D ( vecNowPosition, m_vecPosition, fRadius + m_fRadius );
}

CSphere CClientColCircle::GetWorldBoundingSphere ( void )
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX;
    sphere.vecPosition.fY = m_vecPosition.fY;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius        = m_fRadius;
    return sphere;
}


//
// Draw wireframe circle
//
void CClientColCircle::DebugRender ( const CVector& vecPosition, float fDrawRadius )
{
    SColorARGB color ( 128, 0, 255, 255 );
    float fLineWidth = 4.f + pow ( m_fRadius, 0.5f );
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics ();

    CVector vecBase ( m_vecPosition.fX - m_fRadius, m_vecPosition.fY - m_fRadius, vecPosition.fZ - fDrawRadius );
    CVector vecOrigin  ( m_vecPosition.fX, m_vecPosition.fY, vecPosition.fZ );
    float fHeight = fDrawRadius * 2.f;

    // Don't draw a few end slices to show it goes on for ever
    int iSkipEndSlices = 4;

    // Calc required detail level
    uint uiNumPoints = Max ( 6, Round ( sqrt ( m_fRadius ) * 2.0f ) );
    uint uiNumSlices = Max ( 2 + iSkipEndSlices * 2, Round ( sqrt ( fHeight ) * 2.0f ) );

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
        CVector vecMult ( m_fRadius, m_fRadius, fHeight );
        CVector vecAdd ( vecOrigin.fX, vecOrigin.fY, vecBase.fZ + 4 ); // Extra bit so a slice is on the same Z coord as the camera

        for ( uint s = iSkipEndSlices ; s < uiNumSlices - iSkipEndSlices ; s++ )
        {
            vecMult.fZ = fHeight * ( s / (float)( uiNumSlices - 1 ) );
            for ( uint i = 0 ; i < vertexList.size () ; i++ )
            {
                CVector vecBegin = vertexList[ i ] * vecMult + vecAdd;
                CVector vecEnd = vertexList[ ( i + 1 ) % uiNumPoints ] * vecMult + vecAdd;
                pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
            }
        }
    }

    // Draw lines from bottom to top
    {
        CVector vecMultB ( m_fRadius, m_fRadius, 0 );
        CVector vecMultT ( m_fRadius, m_fRadius, fHeight );
        CVector vecAdd ( vecOrigin.fX, vecOrigin.fY, vecBase.fZ );

        for ( uint i = 0 ; i < vertexList.size () ; i++ )
        {
            CVector vecBegin = vertexList[ i ] * vecMultB + vecAdd;
            CVector vecEnd = vertexList[ i ] * vecMultT + vecAdd;
            pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
        }
    }
}
