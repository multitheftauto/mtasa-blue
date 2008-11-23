/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIWindow_Impl.h
*  PURPOSE:     Window widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUIWINDOW_IMPL_H
#define __CGUIWINDOW_IMPL_H

#include <gui/CGUIWindow.h>
#include "CGUIElement_Impl.h"

class CGUIWindow_Impl : public CGUIWindow, public CGUIElement_Impl
{
public:
                        CGUIWindow_Impl         ( class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "" );
                        ~CGUIWindow_Impl        ( void );

    void                SetMovable              ( bool bMovable );
    bool                IsMovable               ( void );
    void                SetSizingEnabled        ( bool bResizeEnabled );
    bool                IsSizingEnabled         ( void );
    void                SetFrameEnabled         ( bool bFrameEnabled );
    bool                IsFrameEnabled          ( void );
    void                SetCloseButtonEnabled   ( bool bCloseButtonEnabled );
    bool                IsCloseButtonEnabled    ( void );
    void                SetTitlebarEnabled      ( bool bTitlebarEnabled );
    bool                IsTitlebarEnabled       ( void );

    void                SetCloseClickHandler    ( GUI_CALLBACK Callback );
	void                SetKeyDownHandler		( GUI_CALLBACK Callback );

	eCGUIType			GetType					( void ) { return CGUI_WINDOW; };

    #include "CGUIElement_Inc.h"

protected:
    bool                Event_OnCloseClick      ( const CEGUI::EventArgs& e );
    bool                Event_OnKeyDown         ( const CEGUI::EventArgs& e );

    GUI_CALLBACK*         m_pOnCloseClick;
    GUI_CALLBACK*         m_pOnKeyDown;
};

#endif
