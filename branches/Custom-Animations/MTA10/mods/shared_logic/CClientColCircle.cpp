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

CClientColCircle::CClientColCircle ( CClientManager* pManager, ElementID ID, const CVector& vecPosition, float fRadius ) : CClientColShape ( pManager, ID )
{
    m_vecPosition = vecPosition;
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