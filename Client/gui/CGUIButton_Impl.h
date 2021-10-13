/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIButton.h>
#include "CGUIElement_Impl.h"

class CGUIButton_Impl : public CGUIButton, public CGUIElement_Impl
{
public:
    CGUIButton_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative, std::string text);

    void Begin();
    void End();

#include "CGUIElement_Inc.h"
private:
};
