/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIScrollPane.h
 *  PURPOSE:     Scroll pane widget interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"

class CGUIScrollPane : public CGUIElement
{
public:
    virtual ~CGUIScrollPane(){};

    virtual void SetHorizontalScrollBar(bool bEnabled) = 0;
    virtual void SetVerticalScrollBar(bool bEnabled) = 0;

    virtual void  SetHorizontalScrollPosition(float fPosition) = 0;
    virtual float GetHorizontalScrollPosition() = 0;
    virtual void  SetVerticalScrollPosition(float fPosition) = 0;
    virtual float GetVerticalScrollPosition() = 0;

    virtual void  SetHorizontalScrollStepSize(float fPosition) = 0;
    virtual float GetHorizontalScrollStepSize() = 0;

    virtual void  SetVerticalScrollStepSize(float fPosition) = 0;
    virtual float GetVerticalScrollStepSize() = 0;
};
