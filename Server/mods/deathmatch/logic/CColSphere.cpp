/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColSphere.cpp
 *  PURPOSE:     Sphere-shaped collision entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColSphere.h"
#include "Utils.h"

CColSphere::CColSphere(CColManager* pManager, CElement* pParent, const CVector& vecPosition, float fRadius, bool bIsPartnered)
    : CColShape(pManager, pParent, bIsPartnered)
{
    m_vecPosition = vecPosition;
    m_fRadius = fRadius;
    UpdateSpatialData();
}

CElement* CColSphere::Clone(bool* bAddEntity, CResource* pResource)
{
    CColSphere* pColSphere = new CColSphere(m_pManager, GetParentEntity(), m_vecPosition, m_fRadius, IsPartnered());
    return pColSphere;
}

bool CColSphere::DoHitDetection(const CVector& vecNowPosition)
{
    // Do a simple distance check between now position and our position
    return IsPointNearPoint3D(vecNowPosition, m_vecPosition, m_fRadius);
}

bool CColSphere::ReadSpecialData(const int iLine)
{
    int iTemp;
    if (GetCustomDataInt("dimension", iTemp, true))
        m_usDimension = static_cast<unsigned short>(iTemp);

    GetCustomDataFloat("radius", m_fRadius, true);

    return true;
}

CSphere CColSphere::GetWorldBoundingSphere()
{
    return CSphere(m_vecPosition, m_fRadius);
}
