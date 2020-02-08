/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CPhysicsDebugDrawer.h
 *  PURPOSE:     Physics debug drawer
 *
 *****************************************************************************/

#pragma once

#include <list>

class CPhysicsDebugDrawer : public btIDebugDraw
{
    int                 m_debugMode;
    CGraphicsInterface* m_pGraphics;
    SColorARGB          color = SColorARGB(255, 255, 0, 0);
    float               fLineWidth = 2.0f;

public:
    CPhysicsDebugDrawer(CGraphicsInterface* pGraphics) : m_pGraphics(pGraphics){};
    ~CPhysicsDebugDrawer(){};

    void SetDebugLineWidth(float fWidth) { fLineWidth = fWidth; }

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

    void drawSphere(const btVector3& p, btScalar radius, const btVector3& color){};

    void drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha);

    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

    void reportErrorWarning(const char* warningString){};

    void draw3dText(const btVector3& location, const char* textString);

    void setDebugMode(int debugMode) { m_debugMode = debugMode; };

    int getDebugMode() const { return m_debugMode; }
};
