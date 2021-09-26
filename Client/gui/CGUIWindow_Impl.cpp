/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUIWindow_Impl::CGUIWindow_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, std::string title)
{
    m_pManager = pGUI;

    m_initialPosition = pos;
    m_initialSize = size;

    m_title = title;
}

void CGUIWindow_Impl::Begin()
{
    CGUIElement_Impl::Begin();
}

void CGUIWindow_Impl::End()
{
    CGUIElement_Impl::End();
}
