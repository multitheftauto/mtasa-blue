/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CWater.h
 *  PURPOSE:     Water entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CElement.h"

class CWaterManager;

class CWater final : public CElement
{
public:
    enum EWaterType
    {
        TRIANGLE,
        QUAD
    };

    CWater(CWaterManager* pWaterManager, CElement* pParent, EWaterType waterType = QUAD, bool bShallow = false);
    ~CWater();

    bool IsEntity() { return true; }

    const CVector& GetPosition();
    void           SetPosition(const CVector& vecPosition);
    float          GetLevel() const;
    void           SetLevel(float fLevel);

    void Unlink();

    bool       IsWaterShallow() const { return m_bShallow; }
    EWaterType GetWaterType() const { return m_WaterType; }
    int        GetNumVertices() const { return m_WaterType == TRIANGLE ? 3 : 4; }
    bool       GetVertex(int index, CVector& vecPosition) const;
    void       SetVertex(int index, CVector& vecPosition);

    bool Valid();

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    void RoundVertices();
    void RoundVertex(int index);
    bool IsVertexWithinWorld(int index);

    CWaterManager* m_pWaterManager;

    SFixedArray<CVector, 4> m_Vertices;
    EWaterType              m_WaterType;
    bool                    m_bShallow;
};
