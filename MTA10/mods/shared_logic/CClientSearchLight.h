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

class CClientSearchLight : public CClientStreamElement
{
    DECLARE_CLASS ( CClientSearchLight, CClientEntity );
    friend class CClientPointLightsManager;

public:
    CClientSearchLight ( CClientManager* pManager, ElementID ID );
    ~CClientSearchLight ();

    void Unlink ();
    eClientEntityType GetType () const { return CCLIENTSEARCHLIGHT; }

    // CClientEntity methods
    virtual void GetPosition ( CVector& vecPosition ) const override { vecPosition = m_StartPosition; } // required for streaming
    virtual void SetPosition ( const CVector& vecPosition ) override { }

    // CClientStreamElement methods
    virtual void StreamIn ( bool bInstantly ) override { if ( !IsStreamedIn () ) NotifyCreate (); }
    virtual void StreamOut () override { }

    inline const CVector& GetStartPosition () const { return m_StartPosition; }
    inline void SetStartPosition ( const CVector& startPos ) { m_StartPosition = startPos; UpdateStreamPosition ( startPos );}
    inline const CVector& GetEndPosition () const { return m_EndPosition; }
    inline void SetEndPosition ( const CVector& endPos ) { m_EndPosition = endPos; }

    inline float GetStartRadius () const { return m_StartRadius; }
    inline void SetStartRadius ( float radius ) { m_StartRadius = radius; }
    inline float GetEndRadius () const { return m_EndRadius; }
    inline void SetEndRadius ( float radius ) { m_EndRadius = radius; }

    inline bool GetRenderSpot () const { return m_bRenderSpot; }
    inline void SetRenderSpot ( bool renderSpot ) { m_bRenderSpot = renderSpot; }

protected:
    void Render ();

private:
    bool m_bStreamedIn;
    CVector m_StartPosition;
    CVector m_EndPosition;
    float m_StartRadius;
    float m_EndRadius;
    bool m_bRenderSpot;
};
