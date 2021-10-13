/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUIButton_Impl::CGUIButton_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative, std::string text)
    : CGUIElement_Impl(pGUI, pParent, pos, size, relative)
{
    m_type = CGUIType::BUTTON;

    SetText(text);
    SetAutoSizingEnabled(true);
}

void CGUIButton_Impl::Begin()
{
    CGUIElement_Impl::Begin();

    if (ImGui::Button(m_text.c_str(), ImVec2(m_size.fX, m_size.fY)))
    {
        // clicked
    }
}

void CGUIButton_Impl::End()
{
    CGUIElement_Impl::End();
}
