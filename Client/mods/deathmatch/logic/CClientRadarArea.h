/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientRadarArea.h
 *  PURPOSE:     Radar area entity class header
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"
#include <gui/CGUI.h>

class CClientRadarAreaManager;

class CClientRadarArea : public CClientEntity
{
    DECLARE_CLASS(CClientRadarArea, CClientEntity)
    friend class CClientRadarAreaManager;

public:
    CClientRadarArea(class CClientManager* pManager, ElementID ID);
    ~CClientRadarArea(void);

    void Unlink(void);

    eClientEntityType GetType(void) const { return CCLIENTRADARAREA; };

    const CVector2D& GetPosition(void) const { return m_vecPosition; };
    void             GetPosition(CVector2D& vecPosition) const { vecPosition = m_vecPosition; };
    void             GetPosition(CVector& vecPosition) const { vecPosition = CVector(m_vecPosition.fX, m_vecPosition.fY, 0.0f); };
    void             SetPosition(const CVector2D& vecPosition) { m_vecPosition = vecPosition; };
    void             SetPosition(const CVector& vecPosition) { m_vecPosition = CVector2D(vecPosition.fX, vecPosition.fY); };

    const CVector2D& GetSize(void) const { return m_vecSize; };
    void             GetSize(CVector2D& vecSize) { vecSize = m_vecSize; };
    void             SetSize(const CVector2D& vecSize) { m_vecSize = vecSize; };

    SColor GetColor(void) const { return m_Color; };
    void   SetColor(const SColor color) { m_Color = color; };

    bool IsFlashing(void) const { return m_bFlashing; };
    void SetFlashing(bool bFlashing) { m_bFlashing = bFlashing; };

    float GetAlphaFactor(void) const { return m_fAlphaFactor; };

    void SetDimension(unsigned short usDimension);
    void RelateDimension(unsigned short usDimension);

protected:
    void DoPulse(void);
    void DoPulse(bool bRender);

    CClientRadarAreaManager* m_pRadarAreaManager;

    CVector2D m_vecPosition;
    CVector2D m_vecSize;
    SColor    m_Color;

    bool          m_bStreamedIn;
    bool          m_bFlashing;
    unsigned long m_ulFlashCycleStart;
    float         m_fAlphaFactor;
};
