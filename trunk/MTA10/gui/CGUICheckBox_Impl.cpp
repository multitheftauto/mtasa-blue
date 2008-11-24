/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUICheckBox_Impl.cpp
*  PURPOSE:     Checkbox widget class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define CGUICHECKBOX_NAME "CGUI/Checkbox"

CGUICheckBox_Impl::CGUICheckBox_Impl ( CGUI_Impl* pGUI, CGUIElement* pParent, const char* szCaption, bool bChecked )
{
	m_pManager = pGUI;
	m_pOnClick = NULL;
	m_pData = NULL;

	// Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUICHECKBOX_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );
    m_pWindow->setText ( szCaption );
	m_pWindow->setSize ( CEGUI::Absolute, CEGUI::Size ( 128.0f, 16.0f ) );
	m_pWindow->setVisible ( true );

	// Store the pointer to this CGUI element in the CEGUI element
	m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );

    // Register our events
	m_pWindow->subscribeEvent ( CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber ( &CGUICheckBox_Impl::Event_OnClick, this ) );
    AddEvents ();

	// Set selected state
	SetSelected ( bChecked );

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


CGUICheckBox_Impl::~CGUICheckBox_Impl ( void )
{
	if ( m_pOnClick != NULL )
		delete m_pOnClick;

    m_pManager->RemoveFromRedrawQueue ( reinterpret_cast < CGUIElement* > ( ( m_pWindow )->getUserData () ) );

    // Destroy the control
    m_pWindow->destroy ();

	// Destroy the properties list
	EmptyProperties ();
}


void CGUICheckBox_Impl::Click ( void )
{
    /*
	CGUIEventArgs Args;
	Args.handled = false;

	m_pWindow->fireEvent ( CEGUI::PushButton::EventClicked, Args );
    */
}

void CGUICheckBox_Impl::SetOnClickHandler ( const GUI_CALLBACK & Callback )
{
    m_pOnClick = new GUI_CALLBACK ( Callback );
}


bool CGUICheckBox_Impl::Event_OnClick ( const CEGUI::EventArgs& e )
{
	if ( m_pOnClick )
		(*m_pOnClick) ( reinterpret_cast < CGUIElement* > ( this ) );

    return true;
}

void CGUICheckBox_Impl::SetSelected ( bool bChecked )
{
	reinterpret_cast < CEGUI::Checkbox * > ( m_pWindow ) -> setSelected ( !bChecked );
}


bool CGUICheckBox_Impl::GetSelected ( void )
{
	return !( reinterpret_cast < CEGUI::Checkbox * > ( m_pWindow ) -> isSelected () );
}
