/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUITabPanel_Impl::CGUITabPanel_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative)
    : CGUIElement_Impl(pGUI, pParent, pos, size, relative)
{
    m_type = CGUIType::TABPANEL;
}

void CGUITabPanel_Impl::Begin()
{
    CGUIElement_Impl::Begin();
}

void CGUITabPanel_Impl::End()
{
    CGUIElement_Impl::End();
}
