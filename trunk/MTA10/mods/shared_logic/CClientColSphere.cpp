/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColSphere.cpp
*  PURPOSE:     Sphere-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include <StdInc.h>

CClientColSphere::CClientColSphere ( CClientManager* pManager, ElementID ID ) : CClientColShape ( pManager, ID )
{
    m_pManager = pManager;
    m_fRadius = 1.0f;

    SetTypeName ( "colsphere" );
}


CClientColSphere::CClientColSphere ( CClientManager* pManager, ElementID ID, const CVector& vecPosition, float fRadius ) : CClientColShape ( pManager, ID )
{
    m_vecPosition = vecPosition;
    m_fRadius = fRadius;
}


bool CClientColSphere::DoHitDetection ( const CVector& vecNowPosition, float fRadius )
{
    // Do a simple distance check between now position and our position 
    return IsPointNearPoint3D ( vecNowPosition, m_vecPosition, fRadius + m_fRadius );
}