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

CClientColRectangle::CClientColRectangle ( CClientManager* pManager, ElementID ID, const CVector& vecPosition, const CVector2D& vecSize ) : CClientColShape ( pManager, ID )
{
    m_pManager = pManager;
    m_vecPosition = vecPosition;
    m_vecSize = vecSize;

    SetTypeName ( "colrectangle" );
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