/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIRadioButton_Impl.h
 *  PURPOSE:     Radio button widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIRadioButton.h>
#include "CGUIElement_Impl.h"

class CGUIRadioButton_Impl : public CGUIRadioButton, public CGUIElement_Impl
{
public:
    CGUIRadioButton_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "");
    ~CGUIRadioButton_Impl();

    void SetSelected(bool bChecked);
    bool GetSelected();

    eCGUIType GetType() { return CGUI_RADIOBUTTON; };

    #include "CGUIElement_Inc.h"

private:
};
