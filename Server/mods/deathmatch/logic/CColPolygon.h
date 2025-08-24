/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColPolygon.h
 *  PURPOSE:     Polygon-shaped collision entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CColShape.h"
#include "CVector2D.h"
#include <list>

class CColPolygon : public CColShape
{
public:
    CColPolygon(CColManager* pManager, CElement* pParent, const CVector& vecPosition);
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    virtual CSphere GetWorldBoundingSphere();

    eColShapeType GetShapeType() { return COLSHAPE_POLYGON; }

    bool DoHitDetection(const CVector& vecNowPosition);

    void SetPosition(const CVector& vecPosition);

    bool AddPoint(CVector2D vecPoint);
    bool AddPoint(CVector2D vecPoint, unsigned int uiPointIndex);
    bool SetPointPosition(unsigned int uiPointIndex, const CVector2D& vecPoint);
    bool RemovePoint(unsigned int uiPointIndex);

    unsigned int                           CountPoints() const { return static_cast<unsigned int>(m_Points.size()); };
    std::vector<CVector2D>::const_iterator IterBegin() { return m_Points.begin(); };
    std::vector<CVector2D>::const_iterator IterEnd() { return m_Points.end(); };

    bool SetHeight(float fFloor, float fCeil);
    void GetHeight(float& fFloor, float& fCeil) const
    {
        fFloor = m_fFloor;
        fCeil = m_fCeil;
    };

protected:
    std::vector<CVector2D> m_Points;

    bool ReadSpecialData(const int iLine) override;

    bool IsInBounds(CVector vecPoint);
    void CalculateRadius();
    void CalculateRadius(const CVector2D& vecPoint);

    float m_fRadius = 0.0f;
    float m_fFloor = std::numeric_limits<float>::lowest();
    float m_fCeil = std::numeric_limits<float>::max();
};
