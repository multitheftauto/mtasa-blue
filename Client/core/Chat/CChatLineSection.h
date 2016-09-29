/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

class CChatLineSection
{
public:
    friend class CChatLine;

    CChatLineSection();
    CChatLineSection(const CChatLineSection& other);

    CChatLineSection&           operator =              (const CChatLineSection& other);

    void                        Draw(const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, const CRect2D& RenderBounds);
    float                       GetWidth(void);
    inline const char*          GetText(void) { return m_strText.c_str(); }
    void                        SetText(const char* szText) { m_strText = szText; }
    inline void                 GetColor(CColor& color) { color = m_Color; }
    inline void                 SetColor(const CColor& color) { m_Color = color; }

protected:
    std::string                 m_strText;
    CColor                      m_Color;
    float                       m_fCachedWidth;
    unsigned int                m_uiCachedLength;
};
