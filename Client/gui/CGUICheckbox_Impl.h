/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUICheckbox.h>
#include "CGUIElement_Impl.h"

class CGUICheckbox_Impl : public CGUICheckbox, public CGUIElement_Impl
{
public:
    CGUICheckbox_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative);

    void Begin();
    void End();

#include "CGUIElement_Inc.h"
private:
};
