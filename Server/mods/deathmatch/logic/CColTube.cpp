/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColTube.cpp
 *  PURPOSE:     Tube-shaped collision entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColTube.h"
#include "Utils.h"

CColTube::CColTube(CColManager* pManager, CElement* pParent, const CVector& vecPosition, float fRadius, float fHeight) : CColShape(pManager, pParent)
{
    m_vecPosition = vecPosition;
    m_fRadius = fRadius;
    m_fHeight = fHeight;
    UpdateSpatialData();
}

CElement* CColTube::Clone(bool* bAddEntity, CResource* pResource)
{
    CColTube* pColTube = new CColTube(m_pManager, GetParentEntity(), m_vecPosition, m_fRadius, m_fHeight);
    return pColTube;
}

bool CColTube::DoHitDetection(const CVector& vecNowPosition)
{
    // FIXME: What about radius in height?

    // First see if we're within the circle. Then see if we're within its height
    return (IsPointNearPoint2D(vecNowPosition, m_vecPosition, m_fRadius) && vecNowPosition.fZ >= m_vecPosition.fZ &&
            vecNowPosition.fZ <= m_vecPosition.fZ + m_fHeight);
}

bool CColTube::ReadSpecialData(const int iLine)
{
    int iTemp;
    if (GetCustomDataInt("dimension", iTemp, true))
        m_usDimension = static_cast<unsigned short>(iTemp);

    GetCustomDataFloat("radius", m_fRadius, true);
    GetCustomDataFloat("height", m_fHeight, true);

    return true;
}

CSphere CColTube::GetWorldBoundingSphere()
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX;
    sphere.vecPosition.fY = m_vecPosition.fY;
    sphere.vecPosition.fZ = m_vecPosition.fZ + m_fHeight * 0.5f;
    sphere.fRadius = std::max(m_fRadius, m_fHeight * 0.5f);
    return sphere;
}
