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

class CPhysicsDebugLine
{
public:
    CPhysicsDebugLine(const CVector& from, const CVector& to, const SColorARGB& color) : from(from), to(to), color(color) {}
    CVector    from;
    CVector    to;
    SColorARGB color;
};

class CPhysicsDebugDrawer : public btIDebugDraw
{
    std::bitset<32> m_debugMode = 0;
    SColorARGB      color = SColorARGB(255, 255, 0, 0);
    float           m_fLineWidth = 2.0f;
    float           m_fDrawDistance = 100.0f;
    CVector         m_vecCameraPosition;

public:
    void  SetLineWidth(float fWidth);
    float GetLineWidth() const { return m_fLineWidth; }
    void  SetDrawDistance(float fDistance);
    float GetDrawDistance() const { return m_fDrawDistance; }
    void  SetCameraPosition(CVector& vecPosition);

    void Clear();

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);
    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
    void drawSphere(const btVector3& p, btScalar radius, const btVector3& color){};
    void drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha);
    void drawContactPoint(const btVector3& btPointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
    void reportErrorWarning(const char* warningString);
    void draw3dText(const btVector3& location, const char* textString);
    void setDebugMode(int bit, bool bEnabled) { m_debugMode.set((size_t)bit, bEnabled); };
    void setDebugMode(int debugMode) { m_debugMode = debugMode; };
    void reset() { m_debugMode = 0; };
    int  getDebugMode() const { return m_debugMode.to_ulong(); }
    bool getDebugMode(int bit) const { return m_debugMode.test((size_t)bit); }

    std::vector<CPhysicsDebugLine> m_vecLines;

private:
    void DrawLineInternal(const btVector3& from, const btVector3& to, const btVector3& lineColor);
};
