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
    CClientDisplay(class CClientDisplayManager* pDisplayManager, std::uint32_t ulID);
    virtual ~CClientDisplay();

    std::uint32_t        GetID() { return m_ulID; }
    virtual eDisplayType GetType() = 0;

    std::uint32_t GetExpirationTime() { return m_ulExpirationTime; };
    void          SetExpirationTime(std::uint32_t ulTime) { m_ulExpirationTime = ulTime; };
    std::uint32_t GetTimeTillExpiration() { return m_ulExpirationTime - CClientTime::GetTime(); };
    void          SetTimeTillExpiration(std::uint32_t ulMs) { m_ulExpirationTime = CClientTime::GetTime() + ulMs; };

    virtual const CVector& GetPosition() { return m_vecPosition; };
    virtual void           SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; };

    virtual SColor GetColor() const { return m_Color; }
    virtual void   SetColor(const SColor& color) { m_Color = color; }
    virtual void   SetColorAlpha(std::uint8_t ucAlpha);

    virtual bool IsVisible() { return m_bVisible; };
    virtual void SetVisible(bool bVisible) { m_bVisible = bVisible; };

    virtual void Render() = 0;

protected:
    bool IsExpired() { return (m_ulExpirationTime != 0 && (CClientTime::GetTime() > m_ulExpirationTime)); };

    CClientDisplayManager* m_pDisplayManager;

    std::uint32_t m_ulID;
    std::uint32_t m_ulExpirationTime;
    bool          m_bVisible;
    CVector       m_vecPosition;
    SColor        m_Color;
};
