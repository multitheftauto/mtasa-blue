/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColSphere.cpp
 *  PURPOSE:     Sphere-shaped collision entity class
 *
 *****************************************************************************/

#include <StdInc.h>

CClientColSphere::CClientColSphere(CClientManager* pManager, ElementID ID, const CVector& vecPosition, float fRadius)
    : ClassInit(this), CClientColShape(pManager, ID)
{
    m_vecPosition = vecPosition;
    m_fRadius = fRadius;
    UpdateSpatialData();
}

bool CClientColSphere::DoHitDetection(const CVector& vecNowPosition, float fRadius)
{
    // Do a simple distance check between now position and our position
    return IsPointNearPoint3D(vecNowPosition, m_vecPosition, fRadius + m_fRadius);
}

CSphere CClientColSphere::GetWorldBoundingSphere()
{
    return CSphere(m_vecPosition, m_fRadius);
}

//
// Draw wireframe sphere
//
void CClientColSphere::DebugRender(const CVector& vecPosition, float fDrawRadius)
{
    SColorARGB          color(64, 255, 0, 0);
    float               fLineWidth = 4.f + pow(m_fRadius, 0.5f);
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics();

    uint iterations = Clamp(1, Round(pow(m_fRadius, 0.25f) * 0.75f), 4);

    pGraphics->DrawWiredSphere(m_vecPosition, m_fRadius, color, fLineWidth, iterations);
}
