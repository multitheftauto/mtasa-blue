/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColPolygon.h
 *  PURPOSE:     Polygon-shaped collision entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CColShape.h"

#include <list>

class CColPolygon : public CColShape
{
public:
    CColPolygon(CColManager* pManager, CElement* pParent, const CVector& vecPosition);

    virtual CSphere GetWorldBoundingSphere(void);

    eColShapeType GetShapeType(void) { return COLSHAPE_POLYGON; }

    bool DoHitDetection(const CVector& vecNowPosition);

    void SetPosition(const CVector& vecPosition);

    void AddPoint(CVector2D vecPoint);

    unsigned int                           CountPoints(void) const { return static_cast<unsigned int>(m_Points.size()); };
    std::vector<CVector2D>::const_iterator IterBegin(void) { return m_Points.begin(); };
    std::vector<CVector2D>::const_iterator IterEnd(void) { return m_Points.end(); };

    void SetHeight(float fFloor, float fCeil);
    void GetHeight(float &fFloor, float &fCeil) { fFloor = m_fFloor; fCeil = m_fCeil; };

protected:
    std::vector<CVector2D> m_Points;

    bool ReadSpecialData(const int iLine) override;

    bool IsInBounds(CVector vecPoint);

    float m_fRadius;
    float m_fFloor = std::numeric_limits<float>::min();
    float m_fCeil = std::numeric_limits<float>::max();

};
