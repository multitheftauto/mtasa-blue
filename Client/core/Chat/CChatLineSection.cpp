/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  DEVELOPERS:  Jax <>
*               arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include <StdInc.h>

extern CChat * g_pChat;

CChatLineSection::CChatLineSection()
{
    m_fCachedWidth = -1.0f;
    m_uiCachedLength = 0;
}

CChatLineSection::CChatLineSection(const CChatLineSection& other)
{
    *this = other;
}

CChatLineSection& CChatLineSection::operator = (const CChatLineSection& other)
{
    m_strText = other.m_strText;
    m_Color = other.m_Color;
    m_fCachedWidth = other.m_fCachedWidth;
    m_uiCachedLength = other.m_uiCachedLength;
    return *this;
}

void CChatLineSection::Draw(const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, const CRect2D& RenderBounds)
{
    if (!m_strText.empty() && ucAlpha > 0)
    {
        if (bShadow)
        {
            CRect2D drawShadowAt(vecPosition.fX + 1.0f, vecPosition.fY + 1.0f, vecPosition.fX + 1000.0f, vecPosition.fY + 1000.0f);
            CChat::DrawTextString(m_strText.c_str(), drawShadowAt, 0.0f, drawShadowAt, 0, COLOR_ARGB(ucAlpha, 0, 0, 0), g_pChat->m_vecScale.fX, g_pChat->m_vecScale.fY, RenderBounds);
        }
        CRect2D drawAt(vecPosition.fX, vecPosition.fY, vecPosition.fX + 1000.0f, vecPosition.fY + 1000.0f);
        CChat::DrawTextString(m_strText.c_str(), drawAt, 0.0f, drawAt, 0, COLOR_ARGB(ucAlpha, m_Color.R, m_Color.G, m_Color.B), g_pChat->m_vecScale.fX, g_pChat->m_vecScale.fY, RenderBounds);
    }
}


float CChatLineSection::GetWidth()
{
    if (m_fCachedWidth < 0.0f || m_strText.size() != m_uiCachedLength)
    {
        m_fCachedWidth = CChat::GetTextExtent(m_strText.c_str(), g_pChat->m_vecScale.fX) / Max(0.01f, g_pChat->m_vecScale.fX);
        m_uiCachedLength = m_strText.size();
    }
    return m_fCachedWidth * g_pChat->m_vecScale.fX;
}
