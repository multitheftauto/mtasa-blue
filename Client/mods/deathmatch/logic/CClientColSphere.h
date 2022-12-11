/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColSphere.h
 *  PURPOSE:     Sphere-shaped collision entity class
 *
 *****************************************************************************/

#pragma once

class CClientColSphere final : public CClientColShape
{
    DECLARE_CLASS(CClientColSphere, CClientColShape)
public:
    CClientColSphere(CClientManager* pManager, ElementID ID, const CVector& vecPosition, float fRadius);

    virtual CSphere GetWorldBoundingSphere();
    virtual void    DebugRender(const CVector& vecPosition, float fDrawRadius);

    eColShapeType GetShapeType() { return COLSHAPE_SPHERE; }

    bool DoHitDetection(const CVector& vecNowPosition, float fRadius);

    float GetRadius() { return m_fRadius; };
    void  SetRadius(float fRadius)
    {
        m_fRadius = fRadius;
        SizeChanged();
    }

protected:
    float m_fRadius;
};
