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
public:
    CClientDisplay(unsigned long ulID);
    virtual ~CClientDisplay() = default;

    unsigned long        GetID() const { return m_ulID; }
    virtual eDisplayType GetType() = 0;

    virtual const CVector& GetPosition() { return m_vecPosition; };
    virtual void           SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; };

    virtual SColor GetColor() const { return m_Color; }
    virtual void   SetColor(const SColor& color) { m_Color = color; }
    virtual void   SetColorAlpha(unsigned char ucAlpha);

    virtual bool IsVisible() { return m_bVisible; };
    virtual void SetVisible(bool bVisible) { m_bVisible = bVisible; };

    virtual void Render() = 0;

protected:
    unsigned long m_ulID;
    bool          m_bVisible;
    CVector       m_vecPosition;
    SColor        m_Color;
};
