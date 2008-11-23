/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIMessageBox_Impl.h
*  PURPOSE:     Message box class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUIMESSAGEBOX_IMPL_H
#define __CGUIMESSAGEBOX_IMPL_H

#include <gui/CGUIMessageBox.h>
#include "CGUIButton_Impl.h"
#include "CGUILabel_Impl.h"
#include "CGUIStaticImage_Impl.h"
#include "CGUIWindow_Impl.h"

class CGUIMessageBox_Impl : public CGUIMessageBox
{
public:
							CGUIMessageBox_Impl			( class CGUI_Impl* pGUI, const char* szTitle, const char* szCaption, unsigned int uiFlags );
							~CGUIMessageBox_Impl		( void );

    void					SetAutoDestroy              ( bool bAutoDestroy );
    bool					GetAutoDestroy              ( void );

    void					SetVisible                  ( bool bVisible );
    bool					IsVisible                   ( void );

	void					SetOnClickHandler			( GUI_CALLBACK Callback );

	CGUIWindow*				GetWindow					( void );
private:
    bool					OnResponseHandler           ( CGUIElement* pElement );
	bool					OnKeyDownHandler			( CGUIElement* pElement );

    CGUIWindow*				m_pWindow;
    CGUIStaticImage*		m_pIcon;
    CGUILabel*				m_pLabelCaption;
    CGUIButton*				m_pButton;

    bool					m_bAutoDestroy;
};

#endif
