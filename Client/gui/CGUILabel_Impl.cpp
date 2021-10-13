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

    ImGui::TextColored(ImVec4(m_textColor.R / 255, m_textColor.G / 255, m_textColor.B / 255, m_textColor.A / 255), m_text.c_str());
}

void CGUILabel_Impl::End()
{
    CGUIElement_Impl::End();
}
