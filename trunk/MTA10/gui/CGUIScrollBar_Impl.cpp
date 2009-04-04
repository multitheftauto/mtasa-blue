/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIScrollBar_Impl.cpp
*  PURPOSE:     Scroll bar widget class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define CGUISCROLLBAR_HORIZONTAL_NAME	"CGUI/HorizontalScrollbar"
#define CGUISCROLLBAR_VERTICAL_NAME		"CGUI/VerticalScrollbar"

CGUIScrollBar_Impl::CGUIScrollBar_Impl ( CGUI_Impl* pGUI, bool bHorizontal, CGUIElement* pParent )
{
	m_pManager = pGUI;
	m_pOnScroll = NULL;
	m_pData = NULL;

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the window and set default settings
	m_pWindow = pGUI->GetWindowManager ()->createWindow ( bHorizontal ? CGUISCROLLBAR_HORIZONTAL_NAME : CGUISCROLLBAR_VERTICAL_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );

	// Store the pointer to this CGUI element in the CEGUI element
	m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );
	
    // Register our events
	m_pWindow->subscribeEvent ( CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber ( &CGUIScrollBar_Impl::Event_OnScroll, this ) );
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


CGUIScrollBar_Impl::~CGUIScrollBar_Impl ( void )
{
    m_pManager->RemoveFromRedrawQueue ( reinterpret_cast < CGUIElement* > ( ( m_pWindow )->getUserData () ) );

    // Destroy the control
    m_pWindow->destroy ();

	// Destroy the properties list
	EmptyProperties ();
}


void CGUIScrollBar_Impl::SetScrollPosition ( float fPosition )
{
	reinterpret_cast < CEGUI::Scrollbar* > ( m_pWindow ) -> setScrollPosition ( fPosition );
}


float CGUIScrollBar_Impl::GetScrollPosition ( void )
{
	return reinterpret_cast < CEGUI::Scrollbar* > ( m_pWindow ) -> getScrollPosition ();
}


void CGUIScrollBar_Impl::SetOnScrollHandler ( const GUI_CALLBACK & Callback )
{
    m_pOnScroll = new GUI_CALLBACK ( Callback );
}


bool CGUIScrollBar_Impl::Event_OnScroll ( const CEGUI::EventArgs& e )
{
	if ( m_pOnScroll )
		(*m_pOnScroll) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}
