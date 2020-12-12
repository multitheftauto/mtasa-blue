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

void CPhysicsDebugDrawer::Clear()
{
    m_vecLines.clear();
}

void CPhysicsDebugDrawer::DrawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
    m_vecLines.emplace_back(reinterpret_cast<const CVector&>(from), reinterpret_cast<const CVector&>(to),
                            SColorARGB(255, lineColor.x() * 255.0f, lineColor.y() * 255.0f, lineColor.z() * 255.0f));
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
    CVector vecScreen;
    //if (CStaticFunctionDefinitions::GetScreenFromWorldPosition((CVector) reinterpret_cast<const CVector&>(location), vecScreen, 50.0f, false))
    //{
    //    m_pGraphics->DrawString(vecScreen.fX, vecScreen.fY, SColorARGB(255, 255, 255, 255), m_fLineWidth, textString);
    //}
}

void CPhysicsDebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& lineColor, btScalar alpha)
{
    DrawLine(a, b, lineColor);
    DrawLine(b, c, lineColor);
    DrawLine(c, a, lineColor);
}
