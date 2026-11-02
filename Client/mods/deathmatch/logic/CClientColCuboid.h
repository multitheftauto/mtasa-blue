/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColCircle.h
 *  PURPOSE:     Cuboid-shaped collision entity class
 *
 *****************************************************************************/

#pragma once

class CClientColCuboid final : public CClientColShape
{
    DECLARE_CLASS(CClientColCuboid, CClientColShape)
public:
    CClientColCuboid(CClientManager* pManager, ElementID ID, const CVector& vecPosition, const CVector& vecSize,
                     const CVector& vecRotation = CVector());

    virtual CSphere GetWorldBoundingSphere();
    virtual void    DebugRender(const CVector& vecPosition, float fDrawRadius);

    eColShapeType GetShapeType() { return COLSHAPE_CUBOID; }

    bool DoHitDetection(const CVector& vecNowPosition, float fRadius);

    const CVector& GetSize() { return m_vecSize; };
    void           SetSize(const CVector& vecSize)
    {
        m_vecSize = vecSize;
        SizeChanged();
    }

    void GetRotationRadians(CVector& vecRotation) const override { vecRotation = m_vecRotation; }
    void SetRotationRadians(const CVector& vecRotation) override
    {
        m_vecRotation = vecRotation;
        SizeChanged();
    }

protected:
    CVector m_vecSize;
    CVector m_vecRotation;    // Radians, ZXY Euler order
};
