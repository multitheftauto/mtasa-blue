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
#include "CPhysicsDebugDrawer.h"

void CPhysicsDebugDrawer::Clear()
{
    m_vecLines.clear();
}

void CPhysicsDebugDrawer::DrawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
    const CVector& vecFrom = from;
    const CVector& vecTo = to;
    const float    distance = DistanceBetweenPoints3D((vecFrom + vecTo) / 2, m_vecCameraPosition);
    if (distance < m_fDrawDistance) // 50, 500 - 10%
    {
        float alpha = 255 - distance / m_fDrawDistance * 255; 
        m_vecLines.emplace_back(vecFrom, vecTo, SColorARGB(Min(alpha, 255.0f), lineColor.x() * 255.0f, lineColor.y() * 255.0f, lineColor.z() * 255.0f));
    }
}

void CPhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
    DrawLine(from, to, fromColor);
}

void CPhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
    DrawLine(from, to, lineColor);
}

void CPhysicsDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
    DrawLine(PointOnB, PointOnB + normalOnB * distance, color);
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
    DrawLine(a, b, lineColor);
    DrawLine(b, c, lineColor);
    DrawLine(c, a, lineColor);
}
