/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIWindow.h
 *  PURPOSE:     Window widget interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"

class CGUIWindow : public CGUIElement
{
public:
    virtual ~CGUIWindow(){};

    virtual void SetMovable(bool bMovable) = 0;
    virtual bool IsMovable() = 0;
    virtual void SetSizingEnabled(bool bResizeEnabled) = 0;
    virtual bool IsSizingEnabled() = 0;
    virtual void SetFrameEnabled(bool bFrameEnabled) = 0;
    virtual bool IsFrameEnabled() = 0;
    virtual void SetCloseButtonEnabled(bool bCloseButtonEnabled) = 0;
    virtual bool IsCloseButtonEnabled() = 0;
    virtual void SetTitlebarEnabled(bool bTitlebarEnabled) = 0;
    virtual bool IsTitlebarEnabled() = 0;

    virtual void SetCloseClickHandler(GUI_CALLBACK Callback) = 0;
};
