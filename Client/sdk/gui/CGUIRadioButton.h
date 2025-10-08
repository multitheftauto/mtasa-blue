/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIRadioButton.h
 *  PURPOSE:     Radio button widget interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"
#include "CGUICallback.h"

class CGUIRadioButton : public CGUIElement
{
public:
    virtual ~CGUIRadioButton(){};

    virtual void SetSelected(bool bSelected) = 0;
    virtual bool GetSelected() = 0;
};
