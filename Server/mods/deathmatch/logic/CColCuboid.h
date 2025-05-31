/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColCuboid.h
 *  PURPOSE:     Cube-shaped collision entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CColShape.h"

class CColCuboid : public CColShape
{
public:
    CColCuboid(CColManager* pManager, CElement* pParent, const CVector& vecPosition, const CVector& vecSize);
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    virtual CSphere GetWorldBoundingSphere();

    eColShapeType GetShapeType() { return COLSHAPE_CUBOID; }

    bool DoHitDetection(const CVector& vecNowPosition);

    const CVector& GetSize() { return m_vecSize; };
    void           SetSize(const CVector& vecSize)
    {
        m_vecSize = vecSize;
        SizeChanged();
    };

protected:
    bool ReadSpecialData(const int iLine) override;

    CVector m_vecSize;
};
