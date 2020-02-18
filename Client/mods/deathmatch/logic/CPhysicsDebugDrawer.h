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
#include <bitset>

class CPhysicsDebugDrawer : public btIDebugDraw
{
    std::bitset<32>     m_debugMode = 0;
    CGraphicsInterface* m_pGraphics;
    SColorARGB          color = SColorARGB(255, 255, 0, 0);
    float               m_fLineWidth = 2.0f;

public:
    CPhysicsDebugDrawer(CGraphicsInterface* pGraphics) : m_pGraphics(pGraphics){};
    ~CPhysicsDebugDrawer(){};

    void  SetDebugLineWidth(float fWidth) { m_fLineWidth = fWidth; }
    float GetDebugLineWidth() const { return m_fLineWidth; }

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

    void drawSphere(const btVector3& p, btScalar radius, const btVector3& color){};

    void drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha);

    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

    void reportErrorWarning(const char* warningString){};

    void draw3dText(const btVector3& location, const char* textString);

    void setDebugMode(byte bit, bool bEnabled) { m_debugMode.set(bit, bEnabled); };
    void setDebugMode(int debugMode) { m_debugMode = debugMode; };

    void reset() { m_debugMode = 0; };

    int getDebugMode() const { return m_debugMode.to_ulong(); }
    int getDebugMode(byte bit) const { return m_debugMode.test(bit); }
};
