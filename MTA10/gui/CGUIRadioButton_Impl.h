/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIRadioButton_Impl.h
*  PURPOSE:     Radio button widget class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUIRADIOBUTTON_IMPL_H
#define __CGUIRADIOBUTTON_IMPL_H

#include <gui/CGUIRadioButton.h>
#include "CGUIElement_Impl.h"

class CGUIRadioButton_Impl : public CGUIRadioButton, public CGUIElement_Impl
{
public:
                            CGUIRadioButton_Impl        ( class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "" );
                            ~CGUIRadioButton_Impl       ( void );

	void					SetSelected					( bool bChecked );
	bool					GetSelected					( void );

	eCGUIType				GetType						( void ) { return CGUI_RADIOBUTTON; };

    #include "CGUIElement_Inc.h"

private:
};

#endif
