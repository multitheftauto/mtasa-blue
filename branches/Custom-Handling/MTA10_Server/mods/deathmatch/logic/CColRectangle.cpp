/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CColRectangle.cpp
*  PURPOSE:     Rectangle-shaped collision entity class
*  DEVELOPERS:  Kevin Whiteside <>
*               Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CColRectangle::CColRectangle ( CColManager* pManager, CElement* pParent, const CVector& vecPosition, const CVector2D& vecSize, CXMLNode* pNode ) : CColShape ( pManager, pParent, pNode )
{
    m_vecPosition = vecPosition;
    m_vecSize = vecSize;

    UpdateSpatialData ();
}


bool CColRectangle::DoHitDetection ( const CVector& vecLastPosition, const CVector& vecNowPosition, float fRadius )
{
    // FIXME: What about radius?

    // See if the now position is within our cube
    return ( vecNowPosition.fX >= m_vecPosition.fX &&
             vecNowPosition.fX <= m_vecPosition.fX + m_vecSize.fX &&
             vecNowPosition.fY >= m_vecPosition.fY &&
             vecNowPosition.fY <= m_vecPosition.fY + m_vecSize.fY );
}


bool CColRectangle::ReadSpecialData ( void )
{
    int iTemp;
    if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    float fWidth = 1.0f, fDepth = 1.0f;
    GetCustomDataFloat ( "width", fWidth, true );
    GetCustomDataFloat ( "depth", fDepth, true );

    m_vecSize.fX = fWidth;
    m_vecSize.fY = fDepth;

    return true;
}


CSphere CColRectangle::GetWorldBoundingSphere ( void )
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX + m_vecSize.fX * 0.5f;
    sphere.vecPosition.fY = m_vecPosition.fY + m_vecSize.fY * 0.5f;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius        = Max ( m_vecSize.fX, m_vecSize.fY ) * 0.5f;
    return sphere;
}
