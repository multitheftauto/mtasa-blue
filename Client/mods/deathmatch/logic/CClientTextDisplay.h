/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTextDisplay.h
 *  PURPOSE:     Client text display base class header
 *
 *****************************************************************************/

class CClientTextDisplay;

#pragma once

#include "CClientDisplay.h"
#include "CClientDisplayManager.h"
#include <gui/CGUI.h>

class CClientTextDisplay final : public CClientDisplay
{
public:
    CClientTextDisplay(int ID = 0xFFFFFFFF);
    ~CClientTextDisplay() = default;

    eDisplayType GetType() { return DISPLAY_TEXT; }

    void SetCaption(const char* szCaption);

    void SetPosition(const CVector& vecPosition);

    void SetColor(const SColor color);
    void SetColorAlpha(unsigned char ucAlpha);
    void SetShadowAlpha(unsigned char ucShadowAlpha);

    float GetScale() const { return m_fScale; };
    void  SetScale(float fScale);

    unsigned long GetFormat() const { return m_ulFormat; };
    void          SetFormat(unsigned long ulFormat);

    void SetVisible(bool bVisible);

    void Render();

    static void SetGlobalScale(float fScale) { m_fGlobalScale = fScale; };

private:
    SString m_strCaption;
    float   m_fScale;

    unsigned long m_ulFormat;
    unsigned char m_ucShadowAlpha{};

    static float m_fGlobalScale;
};
