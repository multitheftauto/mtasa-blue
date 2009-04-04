/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIMemo_Impl.cpp
*  PURPOSE:     Multi-line edit box widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define CGUIMEMO_NAME "CGUI/MultiLineEditbox"

CGUIMemo_Impl::CGUIMemo_Impl ( CGUI_Impl* pGUI, CGUIElement* pParent, const char* szEdit )
{
	m_pManager = pGUI;
	m_pTextChanged = NULL;
	m_pData = NULL;

    // Get an unique identifier for CEGUI
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUIMEMO_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );
	
	// Store the pointer to this CGUI element in the CEGUI element
	m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );

    //m_pWindow->setRect ( CEGUI::Relative, CEGUI::Rect (0.00f, 0.00f, 0.40f, 0.40f ) );

    // Register our events
	m_pWindow->subscribeEvent ( CEGUI::MultiLineEditbox::EventTextChanged, CEGUI::Event::Subscriber ( &CGUIMemo_Impl::Event_TextChanged, this ) );
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


CGUIMemo_Impl::~CGUIMemo_Impl ( void )
{
	if ( m_pTextChanged != NULL )
		delete m_pTextChanged;

    m_pManager->RemoveFromRedrawQueue ( reinterpret_cast < CGUIElement* > ( ( m_pWindow )->getUserData () ) );

    // Destroy the control
    m_pWindow->destroy ();

	// Destroy the properties list
	EmptyProperties ();
}


void CGUIMemo_Impl::SetReadOnly ( bool bReadOnly )
{
    reinterpret_cast < CEGUI::MultiLineEditbox* > ( m_pWindow ) -> setReadOnly ( bReadOnly );
}


bool CGUIMemo_Impl::IsReadOnly ( void )
{
    return reinterpret_cast < CEGUI::MultiLineEditbox* > ( m_pWindow ) -> isReadOnly ();
}


void CGUIMemo_Impl::EnsureCaratIsVisible ( void )
{
	CEGUI::MultiLineEditbox* wndMemo = reinterpret_cast < CEGUI::MultiLineEditbox* > ( m_pWindow );
	wndMemo->setCaratIndex ( ( wndMemo->getText ( ) ).length ( ) );
}


unsigned int CGUIMemo_Impl::GetCaratIndex ( void )
{
    return static_cast < unsigned int > ( reinterpret_cast < CEGUI::MultiLineEditbox* > ( m_pWindow )->getCaratIndex () );
}


void CGUIMemo_Impl::SetCaratIndex ( unsigned int uiIndex )
{
    reinterpret_cast < CEGUI::MultiLineEditbox* > ( m_pWindow )->setCaratIndex ( uiIndex );
}


float CGUIMemo_Impl::GetVerticalScrollPosition ( void )
{
    CEGUI::Scrollbar* pScrollbar = reinterpret_cast < CEGUI::MultiLineEditbox* > ( m_pWindow )->d_vertScrollbar;
    if ( pScrollbar )
    {
        return pScrollbar->getScrollPosition ();
    }

    return 0.0f;
}


void CGUIMemo_Impl::SetVerticalScrollPosition ( float fPosition )
{
    CEGUI::Scrollbar* pScrollbar = reinterpret_cast < CEGUI::MultiLineEditbox* > ( m_pWindow )->d_vertScrollbar;
    if ( pScrollbar )
    {
        pScrollbar->setScrollPosition ( fPosition );
    }
}


float CGUIMemo_Impl::GetScrollbarDocumentSize ( void )
{
    CEGUI::Scrollbar* pScrollbar = reinterpret_cast < CEGUI::MultiLineEditbox* > ( m_pWindow )->d_vertScrollbar;
    if ( pScrollbar )
    {
        return pScrollbar->getDocumentSize ();
    }

    return 1.0f;
}


float CGUIMemo_Impl::GetScrollbarPageSize ( void )
{
    CEGUI::Scrollbar* pScrollbar = reinterpret_cast < CEGUI::MultiLineEditbox* > ( m_pWindow )->d_vertScrollbar;
    if ( pScrollbar )
    {
        return pScrollbar->getPageSize ();
    }

    return 1.0f;
}


void CGUIMemo_Impl::SetTextChangedHandler ( const GUI_CALLBACK & Callback )
{
    m_pTextChanged = new GUI_CALLBACK ( Callback );
}


bool CGUIMemo_Impl::Event_TextChanged ( const CEGUI::EventArgs& e )
{
	if ( m_pTextChanged )
		(*m_pTextChanged) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}
