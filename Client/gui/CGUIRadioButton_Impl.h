/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIRadioButton.h>
#include "CGUIElement_Impl.h"

class CGUIRadioButton_Impl : public CGUIRadioButton, public CGUIElement_Impl
{
public:
    CGUIRadioButton_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative);

    void Begin();
    void End();

#include "CGUIElement_Inc.h"
private:
};
