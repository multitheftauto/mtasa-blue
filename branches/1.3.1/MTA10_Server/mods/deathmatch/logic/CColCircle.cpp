/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CColCircle.cpp
*  PURPOSE:     Circle-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

CColCircle::CColCircle ( CColManager* pManager, CElement* pParent, const CVector& vecPosition, float fRadius, CXMLNode* pNode, bool bIsPartnered ) : CColShape ( pManager, pParent, pNode, bIsPartnered )
{
    m_vecPosition = vecPosition;
    m_fRadius = fRadius;
    UpdateSpatialData ();
}


bool CColCircle::DoHitDetection ( const CVector& vecNowPosition )
{
    // Do a simple distance check between now position and our position 
    return IsPointNearPoint2D ( vecNowPosition, m_vecPosition, m_fRadius );
}


bool CColCircle::ReadSpecialData ( void )
{
    int iTemp;
    if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    GetCustomDataFloat ( "radius", m_fRadius, true );

    return true;
}


CSphere CColCircle::GetWorldBoundingSphere ( void )
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX;
    sphere.vecPosition.fY = m_vecPosition.fY;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius        = m_fRadius;
    return sphere;
}
