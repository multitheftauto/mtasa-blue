/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUICheckBox_Impl.h
 *  PURPOSE:     Checkbox widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUICheckBox.h>
#include "CGUIElement_Impl.h"

class CGUICheckBox_Impl : public CGUICheckBox, public CGUIElement_Impl
{
public:
    CGUICheckBox_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "", bool bChecked = false);
    ~CGUICheckBox_Impl();

    void SetSelected(bool bChecked);
    bool GetSelected();

    eCGUIType GetType() { return CGUI_CHECKBOX; };

    #include "CGUIElement_Inc.h"
};
