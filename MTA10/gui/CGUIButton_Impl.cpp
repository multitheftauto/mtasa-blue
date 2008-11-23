/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIButton_Impl.cpp
*  PURPOSE:     Button widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define CGUIBUTTON_NAME "CGUI/Button"

CGUIButton_Impl::CGUIButton_Impl ( CGUI_Impl* pGUI, CGUIElement* pParent, const char* szCaption )
{
	m_pManager = pGUI;
	m_pOnClick = NULL;
	m_pData = NULL;

	// Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUIBUTTON_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );
    m_pWindow->setText ( szCaption );
	m_pWindow->setSize ( CEGUI::Absolute, CEGUI::Size ( 128.0f, 24.0f ) );
	m_pWindow->setVisible ( true );

	// Store the pointer to this CGUI element in the CEGUI element
	m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );

	// Subscribe the clicked event
    m_pWindow->subscribeEvent ( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber ( CGUIButton_Impl::Event_OnClick, this ) );
    AddEvents ();

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if ( pParent )
    {
        SetParent ( pParent );
    }
    else
    {
        pGUI->AddChild ( this );
		SetParent ( NULL );
    }
}


CGUIButton_Impl::~CGUIButton_Impl ( void )
{
	if ( m_pOnClick != NULL )
		delete m_pOnClick;

    m_pManager->RemoveFromRedrawQueue ( reinterpret_cast < CGUIElement* > ( ( m_pWindow )->getUserData () ) );

    // Destroy the control
    m_pWindow->destroy ();

	// Destroy the properties list
	EmptyProperties ();
}


void CGUIButton_Impl::Click ( void )
{
}


void CGUIButton_Impl::SetOnClickHandler ( const GUI_CALLBACK & Callback )
{
    m_pOnClick = new GUI_CALLBACK ( Callback );
}


bool CGUIButton_Impl::Event_OnClick ( const CEGUI::EventArgs& e )
{
    (*m_pOnClick) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}

