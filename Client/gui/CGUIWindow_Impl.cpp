/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUIWindow_Impl::CGUIWindow_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative, std::string title)
    : CGUIElement_Impl(pGUI, pParent, pos, size, relative)
{
    m_type = CGUIType::WINDOW;
    m_text = title;

    SetFlags(ImGuiWindowFlags_NoBringToFrontOnFocus);
    SetFrameEnabled(true);
}

void CGUIWindow_Impl::Begin()
{
    CGUIElement_Impl::Begin();

    if (m_pParent)
        ImGui::Text(m_text.c_str());
}

void CGUIWindow_Impl::End()
{
    if (m_titlebarHeight == -1)
    {
        float size = ImGui::GetWindowHeight();
        float max = ImGui::GetWindowContentRegionMax().y;

        max += ImGui::GetWindowPos().y;

        m_titlebarHeight = size - max;
    }

    CGUIElement_Impl::End();
}

float CGUIWindow_Impl::GetTitlebarHeight()
{
    return m_titlebarHeight;
}
