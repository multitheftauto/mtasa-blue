/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTextDisplay.cpp
 *  PURPOSE:     Client text display base class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

float CClientTextDisplay::m_fGlobalScale = 1.0f;

CClientTextDisplay::CClientTextDisplay(int ID) : CClientDisplay(ID)
{
    // Init
    m_fScale = 1;
    m_ulFormat = 0;
    m_bVisible = true;
}

void CClientTextDisplay::SetCaption(const char* szCaption)
{
    if (szCaption)
        m_strCaption = szCaption;
    else
        m_strCaption.clear();
}

void CClientTextDisplay::SetPosition(const CVector& vecPosition)
{
    m_vecPosition = vecPosition;
}

void CClientTextDisplay::Render()
{
    // If we're visible
    if (m_bVisible && !m_strCaption.empty())
    {
        // Render
        CVector2D    vecResolution = g_pCore->GetGUI()->GetResolution();
        unsigned int uiX = unsigned int(m_vecPosition.fX * vecResolution.fX);
        unsigned int uiY = unsigned int(m_vecPosition.fY * vecResolution.fY);
        if (m_ulFormat & DT_BOTTOM)
            m_ulFormat |= DT_SINGLELINE;

        unsigned int uiShadowOffset = std::max(1, (int)(m_fScale * m_fGlobalScale));
        SColorRGBA   rgbaShadowColor(0, 0, 0, m_Color.A * m_ucShadowAlpha / 255);

        if (rgbaShadowColor.A > 0)
            g_pCore->GetGraphics()->DrawString(uiX + uiShadowOffset, uiY + uiShadowOffset, uiX + uiShadowOffset, uiY + uiShadowOffset, rgbaShadowColor,
                                               m_strCaption, m_fScale * m_fGlobalScale, m_fScale * m_fGlobalScale, m_ulFormat | DT_NOCLIP);
        g_pCore->GetGraphics()->DrawString(uiX, uiY, uiX, uiY, m_Color, m_strCaption, m_fScale * m_fGlobalScale, m_fScale * m_fGlobalScale,
                                           m_ulFormat | DT_NOCLIP);
    }
}

void CClientTextDisplay::SetColor(const SColor color)
{
    m_Color = color;
}

void CClientTextDisplay::SetColorAlpha(unsigned char ucAlpha)
{
    m_Color.A = ucAlpha;
}

void CClientTextDisplay::SetShadowAlpha(unsigned char ucShadowAlpha)
{
    m_ucShadowAlpha = ucShadowAlpha;
}

void CClientTextDisplay::SetScale(float fScale)
{
    m_fScale = fScale;
}

void CClientTextDisplay::SetFormat(unsigned long ulFormat)
{
    m_ulFormat = ulFormat;
}

void CClientTextDisplay::SetVisible(bool bVisible)
{
    CClientDisplay::SetVisible(bVisible);
}
