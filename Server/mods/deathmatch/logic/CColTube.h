/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColTube.h
 *  PURPOSE:     Tube-shaped collision entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CColShape.h"

class CColTube : public CColShape
{
public:
    CColTube(CColManager* pManager, CElement* pParent, const CVector& vecPosition, float fRadius, float fHeight);

    virtual CSphere GetWorldBoundingSphere(void);

    eColShapeType GetShapeType(void) { return COLSHAPE_TUBE; }
    bool          DoHitDetection(const CVector& vecNowPosition);

    float GetRadius(void) { return m_fRadius; };
    void  SetRadius(float fRadius)
    {
        m_fRadius = fRadius;
        SizeChanged();
    };
    float GetHeight(void) { return m_fHeight; };
    void  SetHeight(float fHeight)
    {
        m_fHeight = fHeight;
        SizeChanged();
    };

protected:
    bool ReadSpecialData(const int iLine) override;

    float m_fRadius;
    float m_fHeight;
};
