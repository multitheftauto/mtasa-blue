/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColRectangle.h
 *  PURPOSE:     Rectangle-shaped collision entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CColShape.h"
#include <CVector2D.h>

class CColRectangle : public CColShape
{
public:
    CColRectangle(CColManager* pManager, CElement* pParent, const CVector2D& vecPosition, const CVector2D& vecSize);
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    virtual CSphere GetWorldBoundingSphere();

    eColShapeType GetShapeType() { return COLSHAPE_RECTANGLE; }

    bool DoHitDetection(const CVector& vecNowPosition);

    const CVector2D& GetSize() { return m_vecSize; };
    void             SetSize(const CVector2D& vecSize)
    {
        m_vecSize = vecSize;
        SizeChanged();
    };

protected:
    bool ReadSpecialData(const int iLine) override;

    CVector2D m_vecSize;
};
