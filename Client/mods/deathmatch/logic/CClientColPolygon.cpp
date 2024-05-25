/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColPolygon.cpp
 *  PURPOSE:     Polygon-shaped collision entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CClientColPolygon::CClientColPolygon(CClientManager* pManager, ElementID ID, const CVector2D& vecPosition) : ClassInit(this), CClientColShape(pManager, ID)
{
    m_pManager = pManager;
    m_vecPosition.fX = vecPosition.fX;
    m_vecPosition.fY = vecPosition.fY;
    m_vecPosition.fZ = SPATIAL_2D_Z;

    // That's only to speed up things by not checking the polygon collision,
    // if the point is not even in the bounds
    m_fRadius = 0.0f;
}

bool CClientColPolygon::DoHitDetection(const CVector& vecNowPosition, float fRadius)
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

        if (vecPi.fY < y && vecPj.fY >= y || vecPj.fY < y && vecPi.fY >= y)
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

void CClientColPolygon::SetPosition(const CVector& vecPosition)
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

bool CClientColPolygon::AddPoint(CVector2D vecPoint)
{
    m_Points.push_back(vecPoint);
    CalculateRadius(vecPoint);
    return true;
}

bool CClientColPolygon::AddPoint(CVector2D vecPoint, unsigned int uiPointIndex)
{
    if (uiPointIndex > m_Points.size())
        return false;

    m_Points.insert(m_Points.begin() + uiPointIndex, vecPoint);
    CalculateRadius(vecPoint);

    return true;
}

bool CClientColPolygon::RemovePoint(unsigned int uiPointIndex)
{
    if (m_Points.size() <= 3)
        return false;

    if (uiPointIndex >= m_Points.size())
        return false;

    m_Points.erase(m_Points.begin() + uiPointIndex);

    CalculateRadius();
    return true;
}

bool CClientColPolygon::SetPointPosition(unsigned int uiPointIndex, const CVector2D& vecPoint)
{
    if (uiPointIndex >= m_Points.size())
        return false;

    m_Points[uiPointIndex] = vecPoint;

    CalculateRadius();
    return true;
}

void CClientColPolygon::CalculateRadius()
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

void CClientColPolygon::CalculateRadius(const CVector2D& vecPoint)
{
    CVector2D vecDistance = vecPoint - m_vecPosition;
    float     fDistance = vecDistance.Length();

    if (fDistance > m_fRadius)
    {
        m_fRadius = fDistance;
        SizeChanged();
    }
}

bool CClientColPolygon::IsInBounds(CVector vecPoint)
{
    float fDistanceX = vecPoint.fX - m_vecPosition.fX;
    float fDistanceY = vecPoint.fY - m_vecPosition.fY;

    float fDist = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY);

    return fDist <= m_fRadius;
}

CSphere CClientColPolygon::GetWorldBoundingSphere()
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX;
    sphere.vecPosition.fY = m_vecPosition.fY;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius = m_fRadius;
    return sphere;
}

bool CClientColPolygon::SetHeight(float fFloor, float fCeil)
{
    if (m_fFloor == fFloor && m_fCeil == fCeil)
        return false;

    m_fFloor = fFloor;
    m_fCeil = fCeil;

    return true;
}

//
// Draw wireframe polygon
//
void CClientColPolygon::DebugRender(const CVector& vecPosition, float fDrawRadius)
{
    const uint uiNumPoints = m_Points.size();

    SColorARGB          color(128, 255, 255, 0);
    float               fLineWidth = 4.f + pow(m_fRadius, 0.5f);
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics();

    // Don't draw a few end slices to show it goes on for ever
    int iSkipEndSlices = 4;

    // Calc required detail level
    uint uiNumSlices = std::max(2 + iSkipEndSlices * 2, Round(sqrt(fDrawRadius * 2.0f) * 2.0f));

    // Draw Slices
    {
        for (uint s = iSkipEndSlices; s < uiNumSlices - iSkipEndSlices; s++)
        {
            float fZ = vecPosition.fZ - fDrawRadius + fDrawRadius * 2.0f * (s / (float)(uiNumSlices - 1));
            fZ += 4;            // Extra bit so a slice is on the same Z coord as the camera
            if (m_fFloor <= fZ && fZ <= m_fCeil)
            {
                for (uint i = 0; i < uiNumPoints; i++)
                {
                    const CVector2D& vecPointBegin = m_Points[i];
                    const CVector2D& vecPointEnd = m_Points[(i + 1) % uiNumPoints];

                    CVector vecBegin(vecPointBegin.fX, vecPointBegin.fY, fZ);
                    CVector vecEnd(vecPointEnd.fX, vecPointEnd.fY, fZ);
                    pGraphics->DrawLine3DQueued(vecBegin, vecEnd, fLineWidth, color, eRenderStage::POST_FX);
                }
            }
        }
    }

    // Draw lines from bottom to top
    {
        for (uint i = 0; i < uiNumPoints; i++)
        {
            const CVector2D& vecPoint = m_Points[i];

            CVector vecBegin(vecPoint.fX, vecPoint.fY, std::max(vecPosition.fZ - fDrawRadius, m_fFloor));
            CVector vecEnd(vecPoint.fX, vecPoint.fY, std::min(vecPosition.fZ + fDrawRadius, m_fCeil));
            pGraphics->DrawLine3DQueued(vecBegin, vecEnd, fLineWidth, color, eRenderStage::POST_FX);
        }
    }

    // Draw top and bottom
    for (uint i = 0; i < uiNumPoints; i++)
    {
        const CVector2D& vecPointBegin = m_Points[i];
        const CVector2D& vecPointEnd = m_Points[(i + 1) % uiNumPoints];

        CVector vecFloorBegin(vecPointBegin.fX, vecPointBegin.fY, m_fFloor);
        CVector vecFloorEnd(vecPointEnd.fX, vecPointEnd.fY, m_fFloor);
        pGraphics->DrawLine3DQueued(vecFloorBegin, vecFloorEnd, fLineWidth, color, eRenderStage::POST_FX);

        CVector vecCeilBegin(vecPointBegin.fX, vecPointBegin.fY, m_fCeil);
        CVector vecCeilEnd(vecPointEnd.fX, vecPointEnd.fY, m_fCeil);
        pGraphics->DrawLine3DQueued(vecCeilBegin, vecCeilEnd, fLineWidth, color, eRenderStage::POST_FX);
    }
}
