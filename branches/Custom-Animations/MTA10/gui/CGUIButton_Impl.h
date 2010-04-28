/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIButton_Impl.h
*  PURPOSE:     Button widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUIBUTTON_IMPL_H
#define __CGUIBUTTON_IMPL_H

#include <gui/CGUIButton.h>
#include "CGUIElement_Impl.h"

class CGUIButton_Impl : public CGUIButton, public CGUIElement_Impl
{
public:
                                CGUIButton_Impl             ( class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "" );
                                ~CGUIButton_Impl            ( void );

    eCGUIType                   GetType                     ( void ) { return CGUI_BUTTON; };

    #include "CGUIElement_Inc.h"
};

#endif
