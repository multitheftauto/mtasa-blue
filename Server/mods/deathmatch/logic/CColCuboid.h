/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColCuboid.h
 *  PURPOSE:     Cube-shaped collision entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CColShape.h"

class CColCuboid : public CColShape
{
public:
    CColCuboid(CColManager* pManager, CElement* pParent, const CVector& vecPosition, const CVector& vecSize);

    virtual CSphere GetWorldBoundingSphere(void);

    eColShapeType GetShapeType(void) { return COLSHAPE_CUBOID; }

    bool DoHitDetection(const CVector& vecNowPosition);

    const CVector& GetSize(void) { return m_vecSize; };
    void           SetSize(const CVector& vecSize)
    {
        m_vecSize = vecSize;
        SizeChanged();
    };

protected:
    bool ReadSpecialData(const int iLine) override;

    CVector m_vecSize;
};
