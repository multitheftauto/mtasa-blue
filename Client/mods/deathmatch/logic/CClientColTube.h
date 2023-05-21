/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColTube.h
 *  PURPOSE:     Tube-shaped collision entity class
 *
 *****************************************************************************/

#pragma once

class CClientColTube final : public CClientColShape
{
    DECLARE_CLASS(CClientColTube, CClientColShape)
public:
    CClientColTube(CClientManager* pManager, ElementID ID, const CVector& vecPosition, float fRadius, float fHeight);

    virtual CSphere GetWorldBoundingSphere();
    virtual void    DebugRender(const CVector& vecPosition, float fDrawRadius);

    eColShapeType GetShapeType() { return COLSHAPE_TUBE; }

    bool DoHitDetection(const CVector& vecNowPosition, float fRadius);

    float GetRadius() { return m_fRadius; };
    void  SetRadius(float fRadius)
    {
        m_fRadius = fRadius;
        SizeChanged();
    };
    float GetHeight() { return m_fHeight; };
    void  SetHeight(float fHeight)
    {
        m_fHeight = fHeight;
        SizeChanged();
    };

protected:
    float m_fRadius;
    float m_fHeight;
};
