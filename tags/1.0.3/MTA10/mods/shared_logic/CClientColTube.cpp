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

CClientColTube::CClientColTube ( CClientManager* pManager, ElementID ID ) : CClientColShape ( pManager, ID )
{
    m_pManager = pManager;
    m_fRadius = 1.0f;
    m_fHeight = 1.0f;

    SetTypeName ( "coltube" );
}


CClientColTube::CClientColTube ( CClientManager* pManager, ElementID ID, const CVector& vecPosition, float fRadius, float fHeight ) : CClientColShape ( pManager, ID )
{
    m_vecPosition = vecPosition;
    m_fRadius = fRadius;
    m_fHeight = fHeight;
}


bool CClientColTube::DoHitDetection ( const CVector& vecNowPosition, float fRadius )
{
    // FIXME: What about radius in height?

    // First see if we're within the circle. Then see if we're within its height
    return ( IsPointNearPoint2D ( vecNowPosition, m_vecPosition, fRadius + m_fRadius ) &&
             vecNowPosition.fZ >= m_vecPosition.fZ &&
             vecNowPosition.fZ <= m_vecPosition.fZ + m_fHeight );
}
