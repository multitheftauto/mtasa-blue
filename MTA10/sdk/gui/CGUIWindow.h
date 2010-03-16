/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUIWindow.h
*  PURPOSE:     Window widget interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUIWINDOW_H
#define __CGUIWINDOW_H

#include "CGUIElement.h"

class CGUIWindow : public CGUIElement
{
public:
    virtual                     ~CGUIWindow             ( void ) {};

    virtual void                SetMovable              ( bool bMovable ) = 0;
    virtual bool                IsMovable               ( void ) = 0;
    virtual void                SetSizingEnabled        ( bool bResizeEnabled ) = 0;
    virtual bool                IsSizingEnabled         ( void ) = 0;
    virtual void                SetFrameEnabled         ( bool bFrameEnabled ) = 0;
    virtual bool                IsFrameEnabled          ( void ) = 0;
    virtual void                SetCloseButtonEnabled   ( bool bCloseButtonEnabled ) = 0;
    virtual bool                IsCloseButtonEnabled    ( void ) = 0;
    virtual void                SetTitlebarEnabled      ( bool bTitlebarEnabled ) = 0;
    virtual bool                IsTitlebarEnabled       ( void ) = 0;

    virtual void                SetCloseClickHandler    ( GUI_CALLBACK Callback ) = 0;
	virtual void                SetKeyDownHandler		( GUI_CALLBACK Callback ) = 0;
    virtual void                SetEnterKeyHandler      ( GUI_CALLBACK Callback ) = 0;
};

#endif
