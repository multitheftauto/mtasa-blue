/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColCuboid.cpp
 *  PURPOSE:     Cube-shaped collision entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColCuboid.h"

CColCuboid::CColCuboid(CColManager* pManager, CElement* pParent, const CVector& vecPosition, const CVector& vecSize) : CColShape(pManager, pParent)
{
    m_vecPosition = vecPosition;
    m_vecSize = vecSize;

    UpdateSpatialData();
}

CElement* CColCuboid::Clone(bool* bAddEntity, CResource* pResource)
{
    CColCuboid* pColCuboid = new CColCuboid(m_pManager, GetParentEntity(), m_vecPosition, m_vecSize);
    return pColCuboid;
}

bool CColCuboid::DoHitDetection(const CVector& vecNowPosition)
{
    // FIXME: What about radius?

    // See if the now position is within our cube
    return (vecNowPosition.fX >= m_vecPosition.fX && vecNowPosition.fX <= m_vecPosition.fX + m_vecSize.fX && vecNowPosition.fY >= m_vecPosition.fY &&
            vecNowPosition.fY <= m_vecPosition.fY + m_vecSize.fY && vecNowPosition.fZ >= m_vecPosition.fZ &&
            vecNowPosition.fZ <= m_vecPosition.fZ + m_vecSize.fZ);
}

bool CColCuboid::ReadSpecialData(const int iLine)
{
    int iTemp;
    if (GetCustomDataInt("dimension", iTemp, true))
        m_usDimension = static_cast<unsigned short>(iTemp);

    float fWidth = 1.0f, fDepth = 1.0f, fHeight = 1.0f;
    GetCustomDataFloat("width", fWidth, true);
    GetCustomDataFloat("depth", fDepth, true);
    GetCustomDataFloat("height", fHeight, true);

    m_vecSize.fX = fWidth;
    m_vecSize.fY = fDepth;
    m_vecSize.fZ = fHeight;
    return true;
}

CSphere CColCuboid::GetWorldBoundingSphere()
{
    CSphere sphere;
    sphere.vecPosition = m_vecPosition + m_vecSize * 0.5f;
    sphere.fRadius = std::max(std::max(m_vecSize.fX, m_vecSize.fY), m_vecSize.fZ) * 0.5f;
    return sphere;
}
