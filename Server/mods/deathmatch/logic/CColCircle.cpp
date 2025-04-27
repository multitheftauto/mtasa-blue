/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColCircle.cpp
 *  PURPOSE:     Circle-shaped collision entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColCircle.h"
#include "CColManager.h"
#include "CVector2D.h"
#include "CSpatialDatabase.h"
#include "Utils.h"

CColCircle::CColCircle(CColManager* pManager, CElement* pParent, const CVector2D& vecPosition, float fRadius, bool bIsPartnered)
    : CColShape(pManager, pParent, bIsPartnered)
{
    m_vecPosition.fX = vecPosition.fX;
    m_vecPosition.fY = vecPosition.fY;
    m_vecPosition.fZ = SPATIAL_2D_Z;
    m_fRadius = fRadius;
    UpdateSpatialData();
}

CElement* CColCircle::Clone(bool* bAddEntity, CResource* pResource)
{
    CColCircle* pColCircle = new CColCircle(m_pManager, GetParentEntity(), m_vecPosition, m_fRadius);
    return pColCircle;
}

bool CColCircle::DoHitDetection(const CVector& vecNowPosition)
{
    // Do a simple distance check between now position and our position
    return IsPointNearPoint2D(vecNowPosition, m_vecPosition, m_fRadius);
}

bool CColCircle::ReadSpecialData(const int iLine)
{
    int iTemp;
    if (GetCustomDataInt("dimension", iTemp, true))
        m_usDimension = static_cast<unsigned short>(iTemp);

    GetCustomDataFloat("radius", m_fRadius, true);

    return true;
}

CSphere CColCircle::GetWorldBoundingSphere()
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX;
    sphere.vecPosition.fY = m_vecPosition.fY;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius = m_fRadius;
    return sphere;
}
