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

class CChatLine
{
public:
    CChatLine(void);

    virtual const char*         Format(const char* szText, float fWidth, CColor& color, bool bColorCoded);
    virtual void                Draw(const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, const CRect2D& RenderBounds);
    virtual float               GetWidth(void);
    bool                        IsActive(void) { return m_bActive; }
    void                        SetActive(bool bActive) { m_bActive = bActive; }

    inline unsigned long        GetCreationTime(void) { return m_ulCreationTime; }
    void                        UpdateCreationTime(void);

protected:

    bool                                m_bActive;
    std::vector < CChatLineSection >    m_Sections;
    unsigned long                       m_ulCreationTime;
};
