/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColRectangle.cpp
*  PURPOSE:     Rectangle-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#include <StdInc.h>

CClientColRectangle::CClientColRectangle ( CClientManager* pManager, ElementID ID, const CVector2D& vecPosition, const CVector2D& vecSize ) : ClassInit ( this ), CClientColShape ( pManager, ID )
{
    m_pManager = pManager;
    m_vecPosition.fX = vecPosition.fX;
    m_vecPosition.fY = vecPosition.fY;
    m_vecPosition.fZ = SPATIAL_2D_Z;
    m_vecSize = vecSize;

    UpdateSpatialData ();
}


bool CClientColRectangle::DoHitDetection ( const CVector& vecNowPosition, float fRadius )
{
    // FIXME: What about radius?

    // See if the now position is within our cube
    return ( vecNowPosition.fX >= m_vecPosition.fX &&
             vecNowPosition.fX <= m_vecPosition.fX + m_vecSize.fX &&
             vecNowPosition.fY >= m_vecPosition.fY &&
             vecNowPosition.fY <= m_vecPosition.fY + m_vecSize.fY );
}


CSphere CClientColRectangle::GetWorldBoundingSphere ( void )
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX + m_vecSize.fX * 0.5f;
    sphere.vecPosition.fY = m_vecPosition.fY + m_vecSize.fY * 0.5f;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius        = Max ( m_vecSize.fX, m_vecSize.fY ) * 0.5f;
    return sphere;
}


//
// Draw wireframe rectangle
//
void CClientColRectangle::DebugRender ( const CVector& vecPosition, float fDrawRadius )
{
    CVector vecBase ( m_vecPosition.fX, m_vecPosition.fY, vecPosition.fZ - fDrawRadius );
    CVector vecOrigin ( m_vecPosition.fX + m_vecSize.fX * 0.5f, m_vecPosition.fY + m_vecSize.fY * 0.5f, vecPosition.fZ );
    CVector vecSize ( m_vecSize.fX, m_vecSize.fY, fDrawRadius * 2.f );

    SColorARGB color ( 128, 255, 0, 255 );
    float fLineWidth = 4.f + pow ( Max ( m_vecSize.fX, m_vecSize.fY ) * 0.5f, 0.5f );
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics ();

    // Don't draw a few end slices to show it goes on for ever
    int iSkipEndSlicesZ = 4;

    // Calc required detail level
    uint uiNumSlicesX = Max ( 2, Round ( sqrt ( vecSize.fX ) * 1.5f ) );
    uint uiNumSlicesY = Max ( 2, Round ( sqrt ( vecSize.fY ) * 1.5f ) );
    uint uiNumSlicesZ = Max ( 2 + iSkipEndSlicesZ * 2, Round ( sqrt ( vecSize.fZ ) * 2.0f ) );

    // Draw Slices Z
    {
        static const CVector cornerPoints[] = { CVector( 0, 0, 1 ), CVector( 1, 0, 1 ), CVector( 1, 1, 1 ), CVector( 0, 1, 1 ) };

        CVector vecMult = vecSize;
        CVector vecAdd = vecBase + CVector ( 0, 0, 4 ); // Extra bit so a slice is on the same Z coord as the camera

        for ( uint s = iSkipEndSlicesZ ; s < uiNumSlicesZ - iSkipEndSlicesZ ; s++ )
        {
            vecMult.fZ = vecSize.fZ * ( s / (float)( uiNumSlicesZ - 1 ) );
            for ( uint i = 0 ; i < NUMELMS( cornerPoints ) ; i++ )
            {
                const CVector& vecBegin = cornerPoints [ i ] * vecMult + vecAdd;
                const CVector& vecEnd = cornerPoints [ ( i + 1 ) % 4 ] * vecMult + vecAdd;
                pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
            }
        }
    }

    // Draw Slices Y
    {
        static const CVector cornerPoints[] = { CVector( 0, 1, 0 ), CVector( 1, 1, 0 ), CVector( 1, 1, 1 ), CVector( 0, 1, 1 ) };

        CVector vecMult = vecSize;
        CVector vecAdd = vecBase;

        for ( uint s = 0 ; s < uiNumSlicesY ; s++ )
        {
            vecMult.fY = vecSize.fY * ( s / (float)( uiNumSlicesY - 1 ) );
            for ( uint i = 0 ; i < NUMELMS( cornerPoints ) ; i++ )
            {
                if ( !(i & 1) )
                    continue;   // No end cap
                const CVector& vecBegin = cornerPoints [ i ] * vecMult + vecAdd;
                const CVector& vecEnd = cornerPoints [ ( i + 1 ) % 4 ] * vecMult + vecAdd;
                pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
            }
        }
    }

    // Draw Slices X
    {
        static const CVector cornerPoints[] = { CVector( 1, 0, 0 ), CVector( 1, 1, 0 ), CVector( 1, 1, 1 ), CVector( 1, 0, 1 ) };

        CVector vecMult = vecSize;
        CVector vecAdd = vecBase;

        for ( uint s = 0 ; s < uiNumSlicesX ; s++ )
        {
            vecMult.fX = vecSize.fX * ( s / (float)( uiNumSlicesX - 1 ) );
            for ( uint i = 0 ; i < NUMELMS( cornerPoints ) ; i++ )
            {
                if ( !(i & 1) )
                    continue;   // No end cap
                const CVector& vecBegin = cornerPoints [ i ] * vecMult + vecAdd;
                const CVector& vecEnd = cornerPoints [ ( i + 1 ) % 4 ] * vecMult + vecAdd;
                pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
            }
        }
    }
}
