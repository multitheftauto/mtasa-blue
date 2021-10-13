/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUITab.h>
#include "CGUIElement_Impl.h"

class CGUITab_Impl : public CGUITab, public CGUIElement_Impl
{
public:
    CGUITab_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, std::string text);

    void Begin();
    void End();

#include "CGUIElement_Inc.h"
private:
};
