/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColCircle.cpp
 *  PURPOSE:     Cuboid-shaped collision entity class
 *
 *****************************************************************************/

#include <StdInc.h>

CClientColCuboid::CClientColCuboid(CClientManager* pManager, ElementID ID, const CVector& vecPosition, const CVector& vecSize,
                                   const CVector& vecRotation)
    : ClassInit(this), CClientColShape(pManager, ID)
{
    m_pManager = pManager;
    m_vecPosition = vecPosition;
    m_vecSize = vecSize;
    m_vecRotation = vecRotation;

    UpdateSpatialData();
}

bool CClientColCuboid::DoHitDetection(const CVector& vecNowPosition, float fRadius)
{
    // FIXME: What about radius?

    // Fast AABB path when there is no rotation
    if (m_vecRotation.fX == 0.0f && m_vecRotation.fY == 0.0f && m_vecRotation.fZ == 0.0f)
    {
        return (vecNowPosition.fX >= m_vecPosition.fX && vecNowPosition.fX <= m_vecPosition.fX + m_vecSize.fX &&
                vecNowPosition.fY >= m_vecPosition.fY && vecNowPosition.fY <= m_vecPosition.fY + m_vecSize.fY &&
                vecNowPosition.fZ >= m_vecPosition.fZ && vecNowPosition.fZ <= m_vecPosition.fZ + m_vecSize.fZ);
    }

    // OBB test: transform the test point into the cuboid's local space (R^T for ZXY Euler: R = Ry * Rx * Rz)
    const CVector vecCenter = m_vecPosition + m_vecSize * 0.5f;
    const CVector vecLocal  = vecNowPosition - vecCenter;

    const float rx = m_vecRotation.fX, ry = m_vecRotation.fY, rz = m_vecRotation.fZ;
    const float cx = cosf(rx), sx = sinf(rx);
    const float cy = cosf(ry), sy = sinf(ry);
    const float cz = cosf(rz), sz = sinf(rz);

    // R^T * vecLocal  (transpose of the ZXY rotation matrix)
    const float lx = (cy * cz + sy * sx * sz) * vecLocal.fX + (cx * sz) * vecLocal.fY + (-sy * cz + cy * sx * sz) * vecLocal.fZ;
    const float ly = (-cy * sz + sy * sx * cz) * vecLocal.fX + (cx * cz) * vecLocal.fY + (sy * sz + cy * sx * cz) * vecLocal.fZ;
    const float lz = (sy * cx) * vecLocal.fX + (-sx) * vecLocal.fY + (cy * cx) * vecLocal.fZ;

    const CVector vecHalf = m_vecSize * 0.5f;
    return (lx >= -vecHalf.fX && lx <= vecHalf.fX && ly >= -vecHalf.fY && ly <= vecHalf.fY && lz >= -vecHalf.fZ && lz <= vecHalf.fZ);
}

CSphere CClientColCuboid::GetWorldBoundingSphere()
{
    CSphere sphere;
    sphere.vecPosition = m_vecPosition + m_vecSize * 0.5f;
    sphere.fRadius = std::max(std::max(m_vecSize.fX, m_vecSize.fY), m_vecSize.fZ) * 0.5f;
    return sphere;
}

//
// Draw wireframe cuboid
//
void CClientColCuboid::DebugRender(const CVector& vecPosition, float fDrawRadius)
{
    CVector vecBase   = m_vecPosition;
    CVector vecOrigin = m_vecPosition + m_vecSize * 0.5f;
    CVector vecSize   = m_vecSize;

    // Precompute rotation matrix (R = Ry * Rx * Rz, ZXY Euler order)
    const bool  bHasRotation = m_vecRotation.fX != 0.0f || m_vecRotation.fY != 0.0f || m_vecRotation.fZ != 0.0f;
    const float rx = m_vecRotation.fX, ry = m_vecRotation.fY, rz = m_vecRotation.fZ;
    const float cx = cosf(rx), sx = sinf(rx);
    const float cy = cosf(ry), sy = sinf(ry);
    const float cz = cosf(rz), sz = sinf(rz);
    const float r00 = cy * cz + sy * sx * sz,  r01 = -cy * sz + sy * sx * cz, r02 = sy * cx;
    const float r10 = cx * sz,                 r11 = cx * cz,                 r12 = -sx;
    const float r20 = -sy * cz + cy * sx * sz, r21 = sy * sz + cy * sx * cz,  r22 = cy * cx;

    // Rotate a world point around vecOrigin
    auto rotatePoint = [&](const CVector& p) -> CVector {
        if (!bHasRotation)
            return p;
        const CVector local = p - vecOrigin;
        return CVector(vecOrigin.fX + r00 * local.fX + r01 * local.fY + r02 * local.fZ,
                       vecOrigin.fY + r10 * local.fX + r11 * local.fY + r12 * local.fZ,
                       vecOrigin.fZ + r20 * local.fX + r21 * local.fY + r22 * local.fZ);
    };

    SColorARGB          color(128, 0, 255, 0);
    float               fLineWidth = 4.f + pow(std::max(std::max(m_vecSize.fX, m_vecSize.fY), m_vecSize.fZ) * 0.5f, 0.5f);
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics();

    // Calc required detail level
    uint uiNumSlicesX = std::max(2, Round(sqrt(vecSize.fX) * 1.5f));
    uint uiNumSlicesY = std::max(2, Round(sqrt(vecSize.fY) * 1.5f));
    uint uiNumSlicesZ = std::max(2, Round(sqrt(vecSize.fZ) * 2.0f));

    // Draw Slices Z
    {
        static const CVector cornerPoints[] = {CVector(0, 0, 1), CVector(1, 0, 1), CVector(1, 1, 1), CVector(0, 1, 1)};

        CVector vecMult(vecSize.fX, vecSize.fY, vecSize.fZ);
        CVector vecAdd(vecBase.fX, vecBase.fY, vecBase.fZ);

        for (uint s = 0; s < uiNumSlicesZ; s++)
        {
            vecMult.fZ = vecSize.fZ * (s / (float)(uiNumSlicesZ - 1));
            for (uint i = 0; i < NUMELMS(cornerPoints); i++)
            {
                pGraphics->DrawLine3DQueued(rotatePoint(cornerPoints[i] * vecMult + vecAdd),
                                            rotatePoint(cornerPoints[(i + 1) % 4] * vecMult + vecAdd), fLineWidth, color, eRenderStage::POST_FX);
            }
        }
    }

    // Draw Slices Y
    {
        static const CVector cornerPoints[] = {CVector(0, 1, 0), CVector(1, 1, 0), CVector(1, 1, 1), CVector(0, 1, 1)};

        CVector vecMult(vecSize.fX, vecSize.fY, vecSize.fZ);
        CVector vecAdd(vecBase.fX, vecBase.fY, vecBase.fZ);

        for (uint s = 0; s < uiNumSlicesY; s++)
        {
            vecMult.fY = vecSize.fY * (s / (float)(uiNumSlicesY - 1));
            for (uint i = 0; i < NUMELMS(cornerPoints); i++)
            {
                pGraphics->DrawLine3DQueued(rotatePoint(cornerPoints[i] * vecMult + vecAdd),
                                            rotatePoint(cornerPoints[(i + 1) % 4] * vecMult + vecAdd), fLineWidth, color, eRenderStage::POST_FX);
            }
        }
    }

    // Draw Slices X
    {
        static const CVector cornerPoints[] = {CVector(1, 0, 0), CVector(1, 1, 0), CVector(1, 1, 1), CVector(1, 0, 1)};

        CVector vecMult(vecSize.fX, vecSize.fY, vecSize.fZ);
        CVector vecAdd(vecBase.fX, vecBase.fY, vecBase.fZ);

        for (uint s = 0; s < uiNumSlicesX; s++)
        {
            vecMult.fX = vecSize.fX * (s / (float)(uiNumSlicesX - 1));
            for (uint i = 0; i < NUMELMS(cornerPoints); i++)
            {
                pGraphics->DrawLine3DQueued(rotatePoint(cornerPoints[i] * vecMult + vecAdd),
                                            rotatePoint(cornerPoints[(i + 1) % 4] * vecMult + vecAdd), fLineWidth, color, eRenderStage::POST_FX);
            }
        }
    }
}
