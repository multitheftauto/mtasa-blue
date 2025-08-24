/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIScrollBar.h
 *  PURPOSE:     Scroll bar widget interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"
#include "CGUICallback.h"

class CGUIScrollBar : public CGUIElement
{
public:
    virtual ~CGUIScrollBar(){};

    virtual void SetOnScrollHandler(const GUI_CALLBACK& Callback) = 0;

    virtual void  SetScrollPosition(float fPosition) = 0;
    virtual float GetScrollPosition() = 0;
};
