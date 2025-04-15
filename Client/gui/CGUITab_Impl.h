/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUITab_Impl.h
 *  PURPOSE:     Tab widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUITab.h>
#include "CGUIElement_Impl.h"

class CGUITab_Impl : public CGUITab, public CGUIElement_Impl, public CGUITabList
{
public:
    CGUITab_Impl(class CGUI_Impl* pManager, CGUIElement_Impl* pParent, const char* szCaption);
    ~CGUITab_Impl();

    void SetCaption(const char* szCaption);

    eCGUIType GetType() { return CGUI_TAB; };

#define SETVISIBLE_HACK
#define SETENABLED_HACK
    #include "CGUIElement_Inc.h"
#undef SETENABLED_HACK
#undef SETVISIBLE_HACK

    void SetVisible(bool bVisible);
    bool IsVisible();
    void SetEnabled(bool bEnabled);
    bool IsEnabled();
};
