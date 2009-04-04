/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUIButton.h
*  PURPOSE:     Button widget interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUIBUTTON_H
#define __CGUIBUTTON_H

#include "CGUIElement.h"
#include "CGUICallback.h"

class CGUIButton : public CGUIElement
{
public:
    virtual                     ~CGUIButton         ( void ) {};

    virtual void                Click               ( void ) = 0;

    virtual void                SetOnClickHandler   ( const GUI_CALLBACK & Callback ) = 0;
};

#endif
