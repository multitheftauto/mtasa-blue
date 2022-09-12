/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CBulletPhysics.cpp
 *  PURPOSE:     Physics debug drawer
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBulletPhysicsCommon.h"
#include "CBulletPhysics.h"
#include <Utils.h>


void CPhysicsDebugDrawer::SetLineWidth(float fWidth)
{
    m_fLineWidth = fWidth;
}

void CPhysicsDebugDrawer::SetDrawDistance(float fDistance)
{
    m_fDrawDistance = fDistance;
}

void CPhysicsDebugDrawer::SetCameraPosition(CVector& vecPosition)
{
    m_vecCameraPosition = vecPosition;
}

void CPhysicsDebugDrawer::Clear()
{
    m_vecLines.clear();
}

void CPhysicsDebugDrawer::DrawLineInternal(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
    const CVector vecFrom = CPhysicsSharedLogic::ConvertVector(from);
    const CVector vecTo = CPhysicsSharedLogic::ConvertVector(to);
    const float    distance = DistanceBetweenPoints3D((vecFrom + vecTo) / 2, m_vecCameraPosition);
    if (distance < m_fDrawDistance)
    {
        float alpha = 255.0f - distance / m_fDrawDistance * 255.0f;
        m_vecLines.emplace_back(vecFrom, vecTo, SColorARGB(Clamp(0.0f, alpha, 255.0f), lineColor.x() * 254.0f, lineColor.y() * 254.0f, lineColor.z() * 254.0f));
    }
}

void CPhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
    DrawLineInternal(from, to, fromColor);
}

void CPhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
    DrawLineInternal(from, to, lineColor);
}

void CPhysicsDebugDrawer::drawContactPoint(const btVector3& btPointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
    DrawLineInternal(btPointOnB, btPointOnB + normalOnB * distance, color);
}

void CPhysicsDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
    // Unused
}

void CPhysicsDebugDrawer::reportErrorWarning(const char* warningString)
{
    // Probably unused
}

void CPhysicsDebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& lineColor, btScalar alpha)
{
    DrawLineInternal(a, b, lineColor);
    DrawLineInternal(b, c, lineColor);
    DrawLineInternal(c, a, lineColor);
}
