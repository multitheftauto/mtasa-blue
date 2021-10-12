/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUILabel_Impl::CGUILabel_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative)
    : CGUIElement_Impl(pGUI, pParent, pos, size, relative)
{
    m_type = CGUIType::LABEL;
}

void CGUILabel_Impl::Begin()
{
    CGUIElement_Impl::Begin();

    if (m_textAlign.first != CGUITextAlignHorizontal::LEFT)
    {
        float textPosition = ImGui::GetCursorPosX();

        float containerWidth = ImGui::GetWindowSize().x;
        float textWidth = ImGui::CalcTextSize(m_text.c_str()).x;

        float textGap = (containerWidth - textWidth);

        switch (m_textAlign.first)
        {
            case CGUITextAlignHorizontal::CENTER:
                textPosition += (textGap * 0.5f);
                break;

            case CGUITextAlignHorizontal::RIGHT:
                textPosition += textGap;
                break;
        };

        ImGui::SetCursorPosX(textPosition);
    }

    if (m_textAlign.second != CGUITextAlignVertical::TOP)
    {
        float textPosition = ImGui::GetCursorPosY();

        float containerHeight = ImGui::GetWindowSize().y;
        float textHeight = ImGui::CalcTextSize(m_text.c_str()).y;

        float textGap = (containerHeight - textHeight);

        switch (m_textAlign.second)
        {
            case CGUITextAlignVertical::MIDDLE:
                textPosition += (textGap * 0.5f);
                break;

            case CGUITextAlignVertical::BOTTOM:
                textPosition += textGap;
                break;
        }

        ImGui::SetCursorPosY(textPosition);
    }

    ImGui::TextColored(ImVec4(m_color.R / 255, m_color.G / 255, m_color.B / 255, m_color.A / 255), m_text.c_str());
}

void CGUILabel_Impl::End()
{
    CGUIElement_Impl::End();
}

CVector2D CGUILabel_Impl::GetTextSize()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    Begin();

    ImVec2 size = ImGui::CalcTextSize(m_text.c_str());

    End();

    ImGui::EndFrame();

    return CVector2D(size.x, size.y);
}

float CGUILabel_Impl::GetTextExtent()
{
    return GetTextSize().fX;
}

CColor CGUILabel_Impl::GetTextColor()
{
    return m_color;
}

void CGUILabel_Impl::SetTextColor(CColor color)
{
    m_color = color;
}

void CGUILabel_Impl::SetTextColor(int r, int g, int b, int a)
{
    m_color = CColor(Clamp(0, r, 255), Clamp(0, g, 255), Clamp(0, b, 255), Clamp(0, a, 255));
}

CGUITextAlignHorizontal CGUILabel_Impl::GetTextHorizontalAlign()
{
    return m_textAlign.first;
}

CGUITextAlignVertical CGUILabel_Impl::GetTextVerticalAlign()
{
    return m_textAlign.second;
}

void CGUILabel_Impl::SetTextHorizontalAlign(CGUITextAlignHorizontal align)
{
    m_textAlign.first = align;
}

void CGUILabel_Impl::SetTextVerticalAlign(CGUITextAlignVertical align)
{
    m_textAlign.second = align;
}
