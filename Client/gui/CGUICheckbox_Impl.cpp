/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUICheckbox_Impl::CGUICheckbox_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative)
    : CGUIElement_Impl(pGUI, pParent, pos, size, relative)
{
    m_type = CGUIType::CHECKBOX;
}

void CGUICheckbox_Impl::Begin()
{
    CGUIElement_Impl::Begin();
}

void CGUICheckbox_Impl::End()
{
    CGUIElement_Impl::End();
}
