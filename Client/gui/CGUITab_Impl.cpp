/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUITab_Impl::CGUITab_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, std::string text)
    : CGUIElement_Impl(pGUI, pParent, {}, {}, false)
{
    m_type = CGUIType::TAB;

    SetParent(pParent);
}

void CGUITab_Impl::Begin()
{
    CGUIElement_Impl::Begin();
}

void CGUITab_Impl::End()
{
    CGUIElement_Impl::End();
}
