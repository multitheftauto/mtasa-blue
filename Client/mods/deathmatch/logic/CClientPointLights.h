/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPointLights.h
 *  PURPOSE:     PointLights entity class header
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"

class CClientPointLightsManager;

class CClientPointLights final : public CClientEntity
{
    DECLARE_CLASS(CClientPointLights, CClientEntity)
    friend class CClientPointLightsManager;

public:
    CClientPointLights(class CClientManager* pManager, ElementID ID);
    ~CClientPointLights();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTPOINTLIGHTS; };

    int  GetMode() { return m_iMode; };
    void SetMode(int iMode) { m_iMode = iMode; };

    void GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; };
    void SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; };

    float GetRadius() { return m_fRadius; };
    void  SetRadius(float fRadius) { m_fRadius = fRadius; };

    SColor GetColor() { return m_Color; };
    void   SetColor(SColor color) { m_Color = color; };

    CVector& GetDirection() { return m_vecDirection; };
    void     SetDirection(CVector& vecDirection) { m_vecDirection = vecDirection; };

    bool GetCreatesShadow() { return m_bCreatesShadow; };
    void SetCreatesShadow(bool bCreatesShadow) { m_bCreatesShadow = bCreatesShadow; };

    CClientEntity* GetAffected() { return m_pAffected; };
    void           SetAffected(CClientEntity* pAffected) { m_pAffected = pAffected; };

    void SetDimension(unsigned short usDimension);
    void RelateDimension(unsigned short usDimension);

protected:
    void DoPulse();

    CClientPointLightsManager* m_pPointLightsManager;

    int            m_iMode;
    CVector        m_vecPosition;
    CVector        m_vecDirection;
    float          m_fRadius;
    SColor         m_Color;
    bool           m_bCreatesShadow;
    CClientEntity* m_pAffected;

    bool m_bStreamedIn;
};
