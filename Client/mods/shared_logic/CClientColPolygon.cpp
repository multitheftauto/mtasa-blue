/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColPolygon.cpp
*  PURPOSE:     Polygon-shaped collision entity class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CClientColPolygon::CClientColPolygon ( CClientManager* pManager, ElementID ID, const CVector2D& vecPosition ) : ClassInit ( this ), CClientColShape ( pManager, ID )
{
    m_pManager = pManager;
    m_vecPosition.fX = vecPosition.fX;
    m_vecPosition.fY = vecPosition.fY;
    m_vecPosition.fZ = SPATIAL_2D_Z;

    // That's only to speed up things by not checking the polygon collision,
    // if the point is not even in the bounds
    m_fRadius = 0.0f;
}


bool CClientColPolygon::DoHitDetection  ( const CVector& vecNowPosition, float fRadius )
{
    if ( !IsInBounds ( vecNowPosition ) )
        return false;

    bool collides = false;

    int j = m_Points.size() - 1;

    float x = vecNowPosition.fX;
    float y = vecNowPosition.fY;

    for ( unsigned int i = 0; i < m_Points.size(); i++ )
    {
        CVector2D vecPi = m_Points[i];
        CVector2D vecPj = m_Points[j];

        if ( vecPi.fY < y && vecPj.fY >= y || vecPj.fY < y && vecPi.fY >= y )
        {
            if ( vecPi.fX + ( y - vecPi.fY ) / ( vecPj.fY - vecPi.fY ) * ( vecPj.fX - vecPi.fX ) < x )
            {
                collides = !collides;
            }
        }

        j = i;
    }

    return collides;
}


bool CClientColPolygon::ReadSpecialData ( void )
{
    int iTemp;
    if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    return true;
}


void CClientColPolygon::SetPosition ( const CVector& vecPosition )
{
    CVector vecDifference = m_vecPosition - vecPosition;

    for ( unsigned int i = 0; i < m_Points.size(); i++ )
    {
        m_Points[i].fX -= vecDifference.fX;
        m_Points[i].fY -= vecDifference.fY;
    }

    m_vecPosition = vecPosition;
    UpdateSpatialData ();
    // Add queued collider refresh for v1.1
}

void CClientColPolygon::AddPoint ( CVector2D vecPoint )
{
    float fDistanceX = vecPoint.fX - m_vecPosition.fX;
    float fDistanceY = vecPoint.fY - m_vecPosition.fY;

    float fDist = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY );

    if ( fDist > m_fRadius )
    {
        m_fRadius = fDist;
        SizeChanged ();
    }

    m_Points.push_back ( vecPoint );
}

bool CClientColPolygon::IsInBounds ( CVector vecPoint )
{
    float fDistanceX = vecPoint.fX - m_vecPosition.fX;
    float fDistanceY = vecPoint.fY - m_vecPosition.fY;

    float fDist = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY );

    return fDist <= m_fRadius;
}


CSphere CClientColPolygon::GetWorldBoundingSphere ( void )
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX;
    sphere.vecPosition.fY = m_vecPosition.fY;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius        = m_fRadius;
    return sphere;
}


//
// Draw wireframe polygon
//
void CClientColPolygon::DebugRender ( const CVector& vecPosition, float fDrawRadius )
{
    const uint uiNumPoints = m_Points.size();

    SColorARGB color ( 128, 255, 255, 0 );
    float fLineWidth = 4.f + pow ( m_fRadius, 0.5f );
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics ();

    // Don't draw a few end slices to show it goes on for ever
    int iSkipEndSlices = 4;

    // Calc required detail level
    uint uiNumSlices = Max ( 2 + iSkipEndSlices * 2, Round ( sqrt ( fDrawRadius * 2.0f ) * 2.0f ) );

    // Draw Slices
    {
        for ( uint s = iSkipEndSlices ; s < uiNumSlices - iSkipEndSlices ; s++ )
        {
            float fZ = vecPosition.fZ - fDrawRadius + fDrawRadius * 2.0f * ( s / (float)( uiNumSlices - 1 ) );
            fZ += 4;    // Extra bit so a slice is on the same Z coord as the camera
            for ( uint i = 0 ; i < uiNumPoints ; i++ )
            {
                const CVector2D& vecPointBegin = m_Points [ i ];
                const CVector2D& vecPointEnd = m_Points [ ( i + 1 ) % uiNumPoints ];

                CVector vecBegin ( vecPointBegin.fX, vecPointBegin.fY, fZ );
                CVector vecEnd ( vecPointEnd.fX, vecPointEnd.fY, fZ );
                pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
            }
        }
    }

    // Draw lines from bottom to top
    {
        for ( uint i = 0 ; i < uiNumPoints ; i++ )
        {
            const CVector2D& vecPoint = m_Points [ i ];

            CVector vecBegin ( vecPoint.fX, vecPoint.fY, vecPosition.fZ - fDrawRadius );
            CVector vecEnd ( vecPoint.fX, vecPoint.fY, vecPosition.fZ + fDrawRadius );
            pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
        }
    }
}
