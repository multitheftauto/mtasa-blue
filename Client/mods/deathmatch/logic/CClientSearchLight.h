/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientSearchLight.h
 *  PURPOSE:     HeliLight entity class header
 *
 *****************************************************************************/
#pragma once
#include "CClientEntity.h"

class CClientPointLightsManager;

class CClientSearchLight final : public CClientStreamElement
{
    DECLARE_CLASS(CClientSearchLight, CClientEntity);
    friend class CClientPointLightsManager;

public:
    CClientSearchLight(CClientManager* pManager, ElementID ID);
    ~CClientSearchLight();

    void              Unlink();
    eClientEntityType GetType() const { return CCLIENTSEARCHLIGHT; }

    // CClientEntity methods
    virtual void GetPosition(CVector& vecPosition) const override { vecPosition = m_StartPosition; }            // required for streaming
    virtual void SetPosition(const CVector& vecPosition) override {}

    // CClientStreamElement methods
    virtual void StreamIn(bool bInstantly) override
    {
        if (!IsStreamedIn())
            NotifyCreate();
    }
    virtual void StreamOut() override {}

    const CVector& GetStartPosition() const { return m_StartPosition; }
    void           SetStartPosition(const CVector& startPos)
    {
        m_StartPosition = startPos;
        UpdateStreamPosition(startPos);
    }
    const CVector& GetEndPosition() const { return m_EndPosition; }
    void           SetEndPosition(const CVector& endPos) { m_EndPosition = endPos; }

    float GetStartRadius() const { return m_StartRadius; }
    void  SetStartRadius(float radius) { m_StartRadius = radius; }
    float GetEndRadius() const { return m_EndRadius; }
    void  SetEndRadius(float radius) { m_EndRadius = radius; }

    bool GetRenderSpot() const { return m_bRenderSpot; }
    void SetRenderSpot(bool renderSpot) { m_bRenderSpot = renderSpot; }

protected:
    void Render();

private:
    bool    m_bStreamedIn;
    CVector m_StartPosition;
    CVector m_EndPosition;
    float   m_StartRadius;
    float   m_EndRadius;
    bool    m_bRenderSpot;
};
