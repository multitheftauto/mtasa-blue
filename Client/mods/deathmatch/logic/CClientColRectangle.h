/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColRectangle.h
 *  PURPOSE:     Rectangle-shaped collision entity class
 *
 *****************************************************************************/

#pragma once

class CClientColRectangle final : public CClientColShape
{
    DECLARE_CLASS(CClientColRectangle, CClientColShape)
public:
    CClientColRectangle(CClientManager* pManager, ElementID ID, const CVector2D& vecPosition, const CVector2D& vecSize);

    virtual CSphere GetWorldBoundingSphere();
    virtual void    DebugRender(const CVector& vecPosition, float fDrawRadius);

    eColShapeType GetShapeType() { return COLSHAPE_RECTANGLE; }

    bool DoHitDetection(const CVector& vecNowPosition, float fRadius);

    const CVector2D& GetSize() { return m_vecSize; };
    void             SetSize(const CVector2D& vecSize)
    {
        m_vecSize = vecSize;
        SizeChanged();
    };

protected:
    CVector2D m_vecSize;
};
