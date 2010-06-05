/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIMessageBox_Impl.cpp
*  PURPOSE:     Message box class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "../core/CFilePathTranslator.h"

#define MESSAGEBOX_WIDTH	    400.0f
#define MESSAGEBOX_HEIGHT	    150.0f
#define MESSAGEBOX_SPACER       20
#define MESSAGEBOX_ICON_SIZE    42

CGUIMessageBox_Impl::CGUIMessageBox_Impl ( CGUI_Impl* pGUI, const char* szTitle, const char* szCaption, unsigned int uiFlags )
{
    // Initialize
    m_bAutoDestroy = true;      // TODO: If any buttons..
	m_pIcon = NULL;
	m_pButton = NULL;
	m_pLabelCaption = NULL;

    // Create window
	CVector2D ScreenSize = pGUI->GetResolution ();
	m_pWindow = pGUI->CreateWnd ( NULL, szTitle );
    m_pWindow->SetAlwaysOnTop ( true );
    m_pWindow->SetCloseButtonEnabled ( false );
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetPosition ( CVector2D ( ScreenSize.fX*0.5f - MESSAGEBOX_WIDTH*0.5f, ScreenSize.fY*0.5f - MESSAGEBOX_HEIGHT*0.5f ) );
    m_pWindow->SetSize ( CVector2D ( MESSAGEBOX_WIDTH, MESSAGEBOX_HEIGHT ) );		// relative 0.35, 0.225

    // Create the icon
    m_pIcon = pGUI->CreateStaticImage ( m_pWindow );
    m_pIcon->SetFrameEnabled ( false );
    m_pIcon->SetPosition ( CVector2D ( MESSAGEBOX_SPACER, MESSAGEBOX_HEIGHT / 2 - MESSAGEBOX_ICON_SIZE / 2 ) );
    m_pIcon->SetSize ( CVector2D ( MESSAGEBOX_ICON_SIZE, MESSAGEBOX_ICON_SIZE ) );

	if ( uiFlags & MB_ICON_INFO )
	    m_pIcon->LoadFromFile ( CGUI_ICON_MESSAGEBOX_INFO );
	else if ( uiFlags & MB_ICON_QUESTION )
	    m_pIcon->LoadFromFile ( CGUI_ICON_MESSAGEBOX_QUESTION );
	else if ( uiFlags & MB_ICON_WARNING )
	    m_pIcon->LoadFromFile ( CGUI_ICON_MESSAGEBOX_WARNING );
	else if ( uiFlags & MB_ICON_ERROR )
	    m_pIcon->LoadFromFile ( CGUI_ICON_MESSAGEBOX_ERROR );

    // Create caption label
    m_pLabelCaption = pGUI->CreateLabel ( m_pWindow, szCaption );
    m_pLabelCaption->SetPosition ( CVector2D ( MESSAGEBOX_ICON_SIZE + MESSAGEBOX_SPACER * 2, MESSAGEBOX_SPACER ) );
    m_pLabelCaption->SetSize ( CVector2D ( MESSAGEBOX_WIDTH - MESSAGEBOX_ICON_SIZE - MESSAGEBOX_SPACER * 3, MESSAGEBOX_HEIGHT - MESSAGEBOX_SPACER * 2 ) );
    m_pLabelCaption->SetHorizontalAlign ( CGUI_ALIGN_LEFT_WORDWRAP );
    m_pLabelCaption->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );

    // Create buttons
	if ( uiFlags & MB_BUTTON_OK ) {
		m_pButton = pGUI->CreateButton ( m_pWindow, "OK" );
	} else if ( uiFlags & MB_BUTTON_CANCEL ) {
		m_pButton = pGUI->CreateButton ( m_pWindow, "Cancel" );
	} else if ( uiFlags & MB_BUTTON_YES ) {
		m_pButton = pGUI->CreateButton ( m_pWindow, "Yes" );
	}

	if ( m_pButton ) {
		m_pButton->SetPosition ( CVector2D ( ( MESSAGEBOX_WIDTH - 84 ) / 2, MESSAGEBOX_HEIGHT * 0.76f ) );
		m_pButton->SetSize ( CVector2D ( 84.0f, 24.0f ) );

		SetClickHandler ( GUI_CALLBACK ( &CGUIMessageBox_Impl::OnResponseHandler, this ) );
    }

	// Set the KeyDown handler
	m_pWindow->SetKeyDownHandler ( GUI_CALLBACK ( &CGUIMessageBox_Impl::OnKeyDownHandler, this ) );

	// And finally set the focus to our window
	m_pWindow->Activate ();
}


CGUIMessageBox_Impl::~CGUIMessageBox_Impl ( void )
{
	if ( m_pButton )
		delete m_pButton;

	if ( m_pLabelCaption )
		delete m_pLabelCaption;

	if ( m_pIcon )
		delete m_pIcon;
	
    if ( m_pWindow )
	    delete m_pWindow;
}


void CGUIMessageBox_Impl::SetClickHandler ( GUI_CALLBACK ResponseHandler )
{
	m_pButton->SetClickHandler ( ResponseHandler );
}


void CGUIMessageBox_Impl::SetAutoDestroy ( bool bAutoDestroy )
{
    m_bAutoDestroy = bAutoDestroy;
}


bool CGUIMessageBox_Impl::GetAutoDestroy ( void )
{
    return m_bAutoDestroy;
}


void CGUIMessageBox_Impl::SetVisible ( bool bVisible )
{
    m_pWindow->SetVisible ( true );
}


bool CGUIMessageBox_Impl::IsVisible ( void )
{
    return m_pWindow->IsVisible ();
}


bool CGUIMessageBox_Impl::OnKeyDownHandler ( CGUIElement* pElement )
{
	// Cast it to a set of keyboard arguments
	/*
	const CEGUI::KeyEventArgs& KeyboardArgs = reinterpret_cast < const CEGUI::KeyEventArgs& > ( Args );

	switch ( KeyboardArgs.scancode )
	{
		// Return key
		case CEGUI::Key::Return:
		{
			// Fire the clicking event on the main button
            if ( m_pButton )
            {
			    m_pButton->Click ();
            }

			break;
		}
	}
	*/

	return true;
}


bool CGUIMessageBox_Impl::OnResponseHandler ( CGUIElement* pElement )
{
    // Hide our main form
    m_pWindow->SetVisible ( false );

    // Delete this class
    if ( m_bAutoDestroy )
    {
        delete this;
        return true;
    }

    return true;
}


CGUIWindow* CGUIMessageBox_Impl::GetWindow ( void )
{
	return m_pWindow;
}
