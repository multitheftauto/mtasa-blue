/*********************************************************
*
*  Multi Theft Auto :: Codename "Blue" (version 1.1)
*  Copyright © 2003-2006 MTA.  All Rights Reserved.
*
*  Grand Theft Auto is © 1997-2005 Rockstar North
*
*  THE FOLLOWING SOURCES ARE CONSIDIERED HIGHLY CONFIDENTIAL
*  AND ARE TO BE VIEWED ONLY BY MEMBERS OF THE MULTI THEFT
*  AUTO STAFF.  ANY VIOLATION OF THESE TERMS CAN LEAD TO
*  CRIMINAL PROSECUTION AND/OR LARGE FINES UNDER UNITED
*  STATES LAW.
*
**********************************************************/

#include "StdInc.h"

#define CGUICOMBOBOX_NAME "CGUI/Combobox"

CGUIComboBox_Impl::CGUIComboBox_Impl ( CGUI_Impl* pGUI, CGUIElement* pParent, const char* szCaption )
{
	m_pManager = pGUI;
	m_pOnClick = NULL;
	m_pData = NULL;

	// Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUICOMBOBOX_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );
    m_pWindow->setText ( szCaption );
	m_pWindow->setSize ( CEGUI::Absolute, CEGUI::Size ( 128.0f, 24.0f ) );
	m_pWindow->setVisible ( true );

	// Store the pointer to this CGUI element in the CEGUI element
	m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );

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


CGUIComboBox_Impl::~CGUIComboBox_Impl ( void )
{
	if ( m_pOnClick != NULL )
		delete m_pOnClick;

    Clear ();

    m_pManager->RemoveFromRedrawQueue ( reinterpret_cast < CGUIElement* > ( ( m_pWindow )->getUserData () ) );

    // Destroy the control
    m_pWindow->destroy ();

	// Destroy the properties list
	EmptyProperties ();
}


CGUIListItem* CGUIComboBox_Impl::AddItem ( const char* szText )
{
	CGUIListItem_Impl* pNewItem = new CGUIListItem_Impl ( szText, CGUIListItem_Impl::Type::TextItem, NULL );
    CEGUI::ListboxItem* pListboxItem = pNewItem->GetListItem ();
    reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> addItem ( pListboxItem );
    m_Items.push_back ( pNewItem );
    return pNewItem;
}


CGUIListItem* CGUIComboBox_Impl::GetSelectedItem ( void )
{
    return GetListItem ( reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> getSelectedItem () );
}


void CGUIComboBox_Impl::Clear ( void )
{
    reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> getDropList () -> resetList ();
    m_Items.clear ();
}

void CGUIComboBox_Impl::SetReadOnly ( bool bReadonly )
{
    reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> setReadOnly ( bReadonly );
}


void CGUIComboBox_Impl::Click ( void )
{
}


void CGUIComboBox_Impl::SetOnClickHandler ( const GUI_CALLBACK & Callback )
{
    m_pOnClick = new GUI_CALLBACK ( Callback );
}


bool CGUIComboBox_Impl::Event_OnClick ( const CEGUI::EventArgs& e )
{
    (*m_pOnClick) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}

CGUIListItem_Impl* CGUIComboBox_Impl::GetListItem ( CEGUI::ListboxItem* pItem )
{
    // Find the item in our list
    std::list < CGUIListItem_Impl* > ::const_iterator iter = m_Items.begin ();
    for ( ; iter != m_Items.end (); iter++ )
    {
        // Matching CEGUI class?
        if ( (*iter)->GetListItem () == pItem )
        {
            return *iter;
        }
    }

    // Doesn't exist
    return NULL;
}