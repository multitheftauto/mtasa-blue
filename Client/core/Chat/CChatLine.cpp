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
#include "StdInc.h"
#include <game/CGame.h>

extern CCore* g_pCore;
extern CChat * g_pChat;

CChatLine::CChatLine(void)
{
    m_bActive = false;
    UpdateCreationTime();
}

void CChatLine::UpdateCreationTime()
{
    m_ulCreationTime = GetTickCount32();
}


//
// Calculate the equivalent ansi string pointer of szPosition.
//
const char* CalcAnsiPtr(const char* szStringAnsi, const wchar_t* szPosition)
{
    int iOrigSize = strlen(szStringAnsi);
    int iEndSize = UTF16ToMbUTF8(szPosition).length();
    int iOffset = iOrigSize - iEndSize;
    return szStringAnsi + iOffset;
}


const char* CChatLine::Format(const char* szStringAnsi, float fWidth, CColor& color, bool bColorCoded)
{
    std::wstring wString = MbUTF8ToUTF16(szStringAnsi);
    const wchar_t* szString = wString.c_str();

    float fPrevSectionsWidth = 0.0f;
    m_Sections.clear();

    const wchar_t* szSectionStart = szString;
    const wchar_t* szSectionEnd = szString;
    const wchar_t* szLastWrapPoint = szString;
    bool bLastSection = false;
    while (!bLastSection)      // iterate over sections
    {
        m_Sections.resize(m_Sections.size() + 1);
        CChatLineSection& section = *(m_Sections.end() - 1);
        section.SetColor(color);

        if (m_Sections.size() > 1 && bColorCoded)      // If we've processed sections before
            szSectionEnd += 7;                           // skip the color code

        szSectionStart = szSectionEnd;
        szLastWrapPoint = szSectionStart;
        unsigned int uiSeekPos = 0;
        std::wstring strSectionStart = szSectionStart;

        while (true)      // find end of this section
        {
            float fSectionWidth = CChat::GetTextExtent(UTF16ToMbUTF8(strSectionStart.substr(0, uiSeekPos)).c_str(), g_pChat->m_vecScale.fX);

            if (*szSectionEnd == '\0' || *szSectionEnd == '\n' || fPrevSectionsWidth + fSectionWidth > fWidth)
            {
                bLastSection = true;
                break;
            }
            if (bColorCoded && IsColorCode(UTF16ToMbUTF8(szSectionEnd).c_str()))
            {
                unsigned long ulColor = 0;
                sscanf(UTF16ToMbUTF8(szSectionEnd).c_str() + 1, "%06x", &ulColor);
                color = ulColor;
                fPrevSectionsWidth += fSectionWidth;
                break;
            }
            if (isspace((unsigned char)*szSectionEnd) || ispunct((unsigned char)*szSectionEnd))
            {
                szLastWrapPoint = szSectionEnd;
            }
            szSectionEnd++;
            uiSeekPos++;
        }
        section.m_strText = UTF16ToMbUTF8(strSectionStart.substr(0, uiSeekPos));
    }

    if (*szSectionEnd == '\0')
    {
        return NULL;
    }
    else if (*szSectionEnd == '\n')
    {
        return CalcAnsiPtr(szStringAnsi, szSectionEnd + 1);
    }
    else
    {
        // Do word wrap
        if (szLastWrapPoint == szSectionStart)
        {
            // Wrapping point coincides with the start of a section.
            if (szLastWrapPoint == szString)
            {
                // The line consists of one huge word. Leave the one section we created as it
                // is (with the huge word cut off) and return szRemaining as the rest of the word
                return CalcAnsiPtr(szStringAnsi, szSectionEnd);
            }
            else
            {
                // There's more than one section, remove the last one (where our wrap point is)
                m_Sections.pop_back();
            }
        }
        else
        {
            // Wrapping point is in the middle of a section, truncate
            CChatLineSection& last = *(m_Sections.end() - 1);
            std::wstring wstrTemp = MbUTF8ToUTF16(last.m_strText);
            wstrTemp.resize(szLastWrapPoint - szSectionStart);
            last.m_strText = UTF16ToMbUTF8(wstrTemp);
        }
        return CalcAnsiPtr(szStringAnsi, szLastWrapPoint);
    }
}


void CChatLine::Draw(const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, const CRect2D& RenderBounds)
{
    float fCurrentX = vecPosition.fX;
    for (auto& section : m_Sections)
    {
        section.Draw(CVector2D(fCurrentX, vecPosition.fY), ucAlpha, bShadow, RenderBounds);
        fCurrentX += section.GetWidth();
    }
}


float CChatLine::GetWidth()
{
    float fWidth = 0.0f;
    for (auto& section : m_Sections)
    {
        fWidth += section.GetWidth();
    }
    return fWidth;
}
