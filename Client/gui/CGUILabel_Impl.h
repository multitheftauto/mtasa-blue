/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUILabel.h>
#include "CGUIElement_Impl.h"

class CGUILabel_Impl : public CGUILabel, public CGUIElement_Impl
{
public:
    CGUILabel_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, CVector2D pos, CVector2D size, bool relative);

    void Begin();
    void End();

    CVector2D GetTextSize();
    float     GetTextExtent();

#include "CGUIElement_Inc.h"
private:
};
