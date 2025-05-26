/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColRectangle.cpp
 *  PURPOSE:     Rectangle-shaped collision entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColRectangle.h"
#include "CSpatialDatabase.h"

CColRectangle::CColRectangle(CColManager* pManager, CElement* pParent, const CVector2D& vecPosition, const CVector2D& vecSize) : CColShape(pManager, pParent)
{
    m_vecPosition.fX = vecPosition.fX;
    m_vecPosition.fY = vecPosition.fY;
    m_vecPosition.fZ = SPATIAL_2D_Z;
    m_vecSize = vecSize;

    UpdateSpatialData();
}

CElement* CColRectangle::Clone(bool* bAddEntity, CResource* pResource)
{
    CColRectangle* pColRectangle = new CColRectangle(m_pManager, GetParentEntity(), m_vecPosition, m_vecSize);
    return pColRectangle;
}

bool CColRectangle::DoHitDetection(const CVector& vecNowPosition)
{
    // FIXME: What about radius?

    // See if the now position is within our cube
    return (vecNowPosition.fX >= m_vecPosition.fX && vecNowPosition.fX <= m_vecPosition.fX + m_vecSize.fX && vecNowPosition.fY >= m_vecPosition.fY &&
            vecNowPosition.fY <= m_vecPosition.fY + m_vecSize.fY);
}

bool CColRectangle::ReadSpecialData(const int iLine)
{
    int iTemp;
    if (GetCustomDataInt("dimension", iTemp, true))
        m_usDimension = static_cast<unsigned short>(iTemp);

    float fWidth = 1.0f, fDepth = 1.0f;
    GetCustomDataFloat("width", fWidth, true);
    GetCustomDataFloat("depth", fDepth, true);

    m_vecSize.fX = fWidth;
    m_vecSize.fY = fDepth;

    return true;
}

CSphere CColRectangle::GetWorldBoundingSphere()
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX + m_vecSize.fX * 0.5f;
    sphere.vecPosition.fY = m_vecPosition.fY + m_vecSize.fY * 0.5f;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius = std::max(m_vecSize.fX, m_vecSize.fY) * 0.5f;
    return sphere;
}
