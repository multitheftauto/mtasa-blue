/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPhysics.cpp
 *  PURPOSE:     Physics debug drawer
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPhysicsDebugDrawer.h"

void CPhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(from), reinterpret_cast<const CVector&>(to), fLineWidth, color, false);
}

void CPhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(from), reinterpret_cast<const CVector&>(to), fLineWidth,
                                  SColorARGB(255, lineColor.x() * 255.0f, lineColor.y() * 255.0f, lineColor.z() * 255.0f), false);
}

void CPhysicsDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
    const CVector point = reinterpret_cast<const CVector&>(PointOnB);
    const CVector normal = reinterpret_cast<const CVector&>(normalOnB);
    m_pGraphics->DrawLine3DQueued(point, point + normal * distance, fLineWidth, SColorARGB(255, color.x() * 255.0f, color.y() * 255.0f, color.z() * 255.0f),
                                  false);
}

void CPhysicsDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
    CVector vecScreen;
    if (CStaticFunctionDefinitions::GetScreenFromWorldPosition((CVector) reinterpret_cast<const CVector&>(location), vecScreen, 50.0f, false))
    {
        m_pGraphics->DrawString(vecScreen.fX, vecScreen.fY, SColorARGB(255, 255, 255, 255), fLineWidth, textString);
    }
}

void CPhysicsDebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& lineColor, btScalar alpha)
{
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(a), reinterpret_cast<const CVector&>(b), fLineWidth, color, false);
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(b), reinterpret_cast<const CVector&>(c), fLineWidth, color, false);
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(a), reinterpret_cast<const CVector&>(c), fLineWidth, color, false);
}
