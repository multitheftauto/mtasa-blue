/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColPolygon.h
 *  PURPOSE:     Polygon-shaped collision entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CClientColPolygon : public CClientColShape
{
    DECLARE_CLASS(CClientColPolygon, CClientColShape)
public:
    CClientColPolygon(CClientManager* pManager, ElementID ID, const CVector2D& vecPosition);

    virtual CSphere GetWorldBoundingSphere();
    virtual void    DebugRender(const CVector& vecPosition, float fDrawRadius);

    eColShapeType GetShapeType() { return COLSHAPE_POLYGON; }

    bool DoHitDetection(const CVector& vecNowPosition, float fRadius);

    void SetPosition(const CVector& vecPosition);

    void AddPoint(CVector2D vecPoint);

    unsigned int                           CountPoints() const { return static_cast<unsigned int>(m_Points.size()); };
    std::vector<CVector2D>::const_iterator IterBegin() { return m_Points.begin(); };
    std::vector<CVector2D>::const_iterator IterEnd() { return m_Points.end(); };
    void SetHeight(float fFloor, float fCeil);
    void GetHeight(float &fFloor, float &fCeil) { fFloor = m_fFloor; fCeil = m_fCeil; };

protected:
    std::vector<CVector2D> m_Points;

    bool IsInBounds(CVector vecPoint);

    float m_fRadius;
    float m_fFloor = std::numeric_limits<float>::min();
    float m_fCeil = std::numeric_limits<float>::max();
};
