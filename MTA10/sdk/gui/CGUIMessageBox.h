/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUIMessageBox.h
*  PURPOSE:     Message box interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUIMESSAGEBOX_H
#define __CGUIMESSAGEBOX_H

#include "CGUIElement.h"
#include "CGUIWindow.h"
#include "CGUICallback.h"

class CGUIMessageBox
{
public:
    virtual                     ~CGUIMessageBox         ( void ) {};

    virtual void                SetAutoDestroy          ( bool bAutoDestroy ) = 0;
    virtual bool                GetAutoDestroy          ( void ) = 0;

    virtual void                SetVisible              ( bool bVisible ) = 0;
    virtual bool                IsVisible               ( void ) = 0;

	virtual void				SetClickHandler         ( GUI_CALLBACK Callback ) = 0;

	virtual CGUIWindow*			GetWindow				( void ) = 0;
};

#endif
