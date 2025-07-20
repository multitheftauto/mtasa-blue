/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIProgressBar_Impl.h
 *  PURPOSE:     Progress bar widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIProgressBar.h>
#include "CGUIElement_Impl.h"

class CGUIProgressBar_Impl : public CGUIProgressBar, public CGUIElement_Impl
{
public:
    CGUIProgressBar_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL);
    ~CGUIProgressBar_Impl();

    void  SetProgress(float fProgress);
    float GetProgress();

    eCGUIType GetType() { return CGUI_PROGRESSBAR; };

    #include "CGUIElement_Inc.h"

private:
};
