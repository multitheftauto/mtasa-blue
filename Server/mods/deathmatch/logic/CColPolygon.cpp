/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColPolygon.cpp
 *  PURPOSE:     Polygon-shaped collision entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColPolygon.h"
#include "CVector2D.h"
#include "CSpatialDatabase.h"

CColPolygon::CColPolygon(CColManager* pManager, CElement* pParent, const CVector& vecPosition) : CColShape(pManager, pParent)
{
    m_vecPosition = vecPosition;

    // That's only to speed up things by not checking the polygon collision,
    // if the point is not even in the bounds
    m_fRadius = 0.0f;
}

CElement* CColPolygon::Clone(bool* bAddEntity, CResource* pResource)
{
    CColPolygon* pColPolygon = new CColPolygon(m_pManager, GetParentEntity(), m_vecPosition);
    pColPolygon->m_Points = m_Points;
    pColPolygon->m_fRadius = m_fRadius;
    pColPolygon->SizeChanged();
    return pColPolygon;
}

bool CColPolygon::DoHitDetection(const CVector& vecNowPosition)
{
    if (!IsInBounds(vecNowPosition))
        return false;

    if (vecNowPosition.fZ > m_fCeil || vecNowPosition.fZ < m_fFloor)
        return false;

    bool collides = false;

    int j = m_Points.size() - 1;

    float x = vecNowPosition.fX;
    float y = vecNowPosition.fY;

    for (unsigned int i = 0; i < m_Points.size(); i++)
    {
        CVector2D vecPi = m_Points[i];
        CVector2D vecPj = m_Points[j];

        if ((vecPi.fY < y && vecPj.fY >= y) || (vecPj.fY < y && vecPi.fY >= y))
        {
            if (vecPi.fX + (y - vecPi.fY) / (vecPj.fY - vecPi.fY) * (vecPj.fX - vecPi.fX) < x)
            {
                collides = !collides;
            }
        }

        j = i;
    }

    return collides;
}

bool CColPolygon::ReadSpecialData(const int iLine)
{
    int iTemp;
    if (GetCustomDataInt("dimension", iTemp, true))
        m_usDimension = static_cast<unsigned short>(iTemp);

    return true;
}

void CColPolygon::SetPosition(const CVector& vecPosition)
{
    CVector vecDifference = m_vecPosition - vecPosition;

    for (unsigned int i = 0; i < m_Points.size(); i++)
    {
        m_Points[i].fX -= vecDifference.fX;
        m_Points[i].fY -= vecDifference.fY;
    }

    m_vecPosition = vecPosition;

    UpdateSpatialData();
    // Add queued collider refresh for v1.1
}

bool CColPolygon::AddPoint(CVector2D vecPoint)
{
    m_Points.push_back(vecPoint);
    CalculateRadius(vecPoint);
    return true;
}

bool CColPolygon::AddPoint(CVector2D vecPoint, unsigned int uiPointIndex)
{
    if (uiPointIndex > m_Points.size())
        return false;

    m_Points.insert(m_Points.begin() + uiPointIndex, vecPoint);
    CalculateRadius(vecPoint);

    return true;
}

bool CColPolygon::RemovePoint(unsigned int uiPointIndex)
{
    if (m_Points.size() <= 3)
        return false;

    m_Points.erase(m_Points.begin() + uiPointIndex);

    CalculateRadius();
    return true;
}

bool CColPolygon::SetPointPosition(unsigned int uiPointIndex, const CVector2D& vecPoint)
{
    if (uiPointIndex >= m_Points.size())
        return false;

    m_Points[uiPointIndex] = vecPoint;

    CalculateRadius();
    return true;
}

void CColPolygon::CalculateRadius()
{
    m_fRadius = 0.0f;
    for (auto vecPoint : m_Points)
    {
        CVector2D vecDistance = vecPoint - m_vecPosition;
        float     fDistance = vecDistance.Length();

        if (fDistance > m_fRadius)
            m_fRadius = fDistance;
    }

    SizeChanged();
}

void CColPolygon::CalculateRadius(const CVector2D& vecPoint)
{
    CVector2D vecDistance = vecPoint - m_vecPosition;
    float     fDistance = vecDistance.Length();

    if (fDistance > m_fRadius)
    {
        m_fRadius = fDistance;
        SizeChanged();
    }
}

bool CColPolygon::IsInBounds(CVector vecPoint)
{
    float fDistanceX = vecPoint.fX - m_vecPosition.fX;
    float fDistanceY = vecPoint.fY - m_vecPosition.fY;

    float fDist = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY);

    return fDist <= m_fRadius;
}

CSphere CColPolygon::GetWorldBoundingSphere()
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX;
    sphere.vecPosition.fY = m_vecPosition.fY;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius = m_fRadius;
    return sphere;
}

bool CColPolygon::SetHeight(float fFloor, float fCeil)
{
    if (m_fFloor == fFloor && m_fCeil == fCeil)
        return false;

    m_fFloor = fFloor;
    m_fCeil = fCeil;

    return true;
}
