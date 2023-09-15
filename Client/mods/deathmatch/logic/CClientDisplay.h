/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientDisplay.h
 *  PURPOSE:     Text display class
 *
 *****************************************************************************/

class CClientDisplay;

#pragma once

#include "CClientCommon.h"

enum eDisplayType
{
    DISPLAY_TEXT,
    DISPLAY_VECTORGRAPHIC,
};

class CClientDisplay
{
    friend class CClientDisplayManager;

public:
    CClientDisplay(class CClientDisplayManager* pDisplayManager, unsigned long ulID);
    virtual ~CClientDisplay();

    unsigned long        GetID() { return m_ulID; }
    virtual eDisplayType GetType() = 0;

    unsigned long GetExpirationTime() { return m_ulExpirationTime; };
    void          SetExpirationTime(unsigned long ulTime) { m_ulExpirationTime = ulTime; };
    unsigned long GetTimeTillExpiration() { return m_ulExpirationTime - CClientTime::GetTime(); };
    void          SetTimeTillExpiration(unsigned long ulMs) { m_ulExpirationTime = CClientTime::GetTime() + ulMs; };

    virtual const CVector& GetPosition() { return m_vecPosition; };
    virtual void           SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; };

    virtual SColor GetColor() const { return m_Color; }
    virtual void   SetColor(const SColor& color) { m_Color = color; }
    virtual void   SetColorAlpha(unsigned char ucAlpha);

    virtual bool IsVisible() { return m_bVisible; };
    virtual void SetVisible(bool bVisible) { m_bVisible = bVisible; };

    virtual void Render() = 0;

protected:
    bool IsExpired() { return (m_ulExpirationTime != 0 && (CClientTime::GetTime() > m_ulExpirationTime)); };

    CClientDisplayManager* m_pDisplayManager;

    unsigned long m_ulID;
    unsigned long m_ulExpirationTime;
    bool          m_bVisible;
    CVector       m_vecPosition;
    SColor        m_Color;
};
