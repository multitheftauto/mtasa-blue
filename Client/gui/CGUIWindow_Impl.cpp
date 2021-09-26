/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUIWindow_Impl::CGUIWindow_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative, std::string title) : CGUIElement_Impl(pGUI, pParent, pos, size, relative)
{
    m_type = "window";
    m_title = title;

    SetFrameEnabled(true);
}

void CGUIWindow_Impl::Begin()
{
    CGUIElement_Impl::Begin();
}

void CGUIWindow_Impl::End()
{
    CGUIElement_Impl::End();
}
