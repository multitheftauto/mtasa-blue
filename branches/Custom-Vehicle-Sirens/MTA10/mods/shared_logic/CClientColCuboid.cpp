/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColCircle.cpp
*  PURPOSE:     Cuboid-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#include <StdInc.h>

CClientColCuboid::CClientColCuboid ( CClientManager* pManager, ElementID ID, const CVector& vecPosition, const CVector& vecSize ) : ClassInit ( this ), CClientColShape ( pManager, ID )
{
    m_pManager = pManager;
    m_vecPosition = vecPosition;
    m_vecSize = vecSize;

    UpdateSpatialData ();
}


bool CClientColCuboid::DoHitDetection  ( const CVector& vecNowPosition, float fRadius )
{
    // FIXME: What about radius?

    // See if the now position is within our cube
    return ( vecNowPosition.fX >= m_vecPosition.fX &&
             vecNowPosition.fX <= m_vecPosition.fX + m_vecSize.fX &&
             vecNowPosition.fY >= m_vecPosition.fY &&
             vecNowPosition.fY <= m_vecPosition.fY + m_vecSize.fY &&
             vecNowPosition.fZ >= m_vecPosition.fZ &&
             vecNowPosition.fZ <= m_vecPosition.fZ + m_vecSize.fZ );
}


CSphere CClientColCuboid::GetWorldBoundingSphere ( void )
{
    CSphere sphere;
    sphere.vecPosition  = m_vecPosition + m_vecSize * 0.5f;
    sphere.fRadius      = Max ( Max ( m_vecSize.fX, m_vecSize.fY ), m_vecSize.fZ ) * 0.5f;
    return sphere;
}


//
// Draw wireframe cuboid
//
void CClientColCuboid::DebugRender ( const CVector& vecPosition, float fDrawRadius )
{
    CVector vecBase = m_vecPosition;
    CVector vecOrigin = m_vecPosition + m_vecSize * 0.5f;
    CVector vecSize = m_vecSize;

    SColorARGB color ( 128, 0, 255, 0 );
    float fLineWidth = 4.f + pow ( Max ( Max ( m_vecSize.fX, m_vecSize.fY ), m_vecSize.fZ ) * 0.5f, 0.5f );
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics ();

    // Calc required detail level
    uint uiNumSlicesX = Max ( 2, Round ( sqrt ( vecSize.fX ) * 1.5f ) );
    uint uiNumSlicesY = Max ( 2, Round ( sqrt ( vecSize.fY ) * 1.5f ) );
    uint uiNumSlicesZ = Max ( 2, Round ( sqrt ( vecSize.fZ ) * 2.0f ) );

    // Draw Slices Z
    {
        static const CVector cornerPoints[] = { CVector( 0, 0, 1 ), CVector( 1, 0, 1 ), CVector( 1, 1, 1 ), CVector( 0, 1, 1 ) };

        CVector vecMult ( vecSize.fX, vecSize.fY, vecSize.fZ );
        CVector vecAdd ( vecBase.fX, vecBase.fY, vecBase.fZ );

        for ( uint s = 0 ; s < uiNumSlicesZ ; s++ )
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

        CVector vecMult ( vecSize.fX, vecSize.fY, vecSize.fZ );
        CVector vecAdd ( vecBase.fX, vecBase.fY, vecBase.fZ );

        for ( uint s = 0 ; s < uiNumSlicesY ; s++ )
        {
            vecMult.fY = vecSize.fY * ( s / (float)( uiNumSlicesY - 1 ) );
            for ( uint i = 0 ; i < NUMELMS( cornerPoints ) ; i++ )
            {
                const CVector& vecBegin = cornerPoints [ i ] * vecMult + vecAdd;
                const CVector& vecEnd = cornerPoints [ ( i + 1 ) % 4 ] * vecMult + vecAdd;
                pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
            }
        }
    }

    // Draw Slices X
    {
        static const CVector cornerPoints[] = { CVector( 1, 0, 0 ), CVector( 1, 1, 0 ), CVector( 1, 1, 1 ), CVector( 1, 0, 1 ) };

        CVector vecMult ( vecSize.fX, vecSize.fY, vecSize.fZ );
        CVector vecAdd ( vecBase.fX, vecBase.fY, vecBase.fZ );

        for ( uint s = 0 ; s < uiNumSlicesX ; s++ )
        {
            vecMult.fX = vecSize.fX * ( s / (float)( uiNumSlicesX - 1 ) );
            for ( uint i = 0 ; i < NUMELMS( cornerPoints ) ; i++ )
            {
                const CVector& vecBegin = cornerPoints [ i ] * vecMult + vecAdd;
                const CVector& vecEnd = cornerPoints [ ( i + 1 ) % 4 ] * vecMult + vecAdd;
                pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
            }
        }
    }
}
