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

void CChatInputLine::Draw(CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow)
{
    CRect2D RenderBounds(0, 0, 9999, 9999);

    CColor colPrefix;
    m_Prefix.GetColor(colPrefix);
    if (colPrefix.A > 0)
        m_Prefix.Draw(vecPosition, colPrefix.A, bShadow, RenderBounds);

    if (g_pChat->m_InputTextColor.A > 0 && m_Sections.size() > 0)
    {
        m_Sections[0].Draw(CVector2D(vecPosition.fX + m_Prefix.GetWidth(), vecPosition.fY),
            g_pChat->m_InputTextColor.A, bShadow, RenderBounds);

        float fLineDifference = CChat::GetFontHeight(g_pChat->m_vecScale.fY);

        for ( auto& line : m_ExtraLines)
        {
            vecPosition.fY += fLineDifference;
            line.Draw(vecPosition, g_pChat->m_InputTextColor.A, bShadow, RenderBounds);
        }
    }
}


void CChatInputLine::Clear(void)
{
    m_Sections.clear();
    m_ExtraLines.clear();
}
