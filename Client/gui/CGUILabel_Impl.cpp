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

    ImGui::Text(m_text.c_str());
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
