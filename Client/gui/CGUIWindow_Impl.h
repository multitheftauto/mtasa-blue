/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIWindow.h>
#include "CGUIElement_Impl.h"

class CGUIWindow_Impl : public CGUIWindow, public CGUIElement_Impl
{
public:
    CGUIWindow_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative, std::string title);

    void Begin();
    void End();

    float GetTitlebarHeight();

#include "CGUIElement_Inc.h"
private:
    float m_titlebarHeight = -1;
};
