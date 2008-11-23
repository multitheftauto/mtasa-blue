/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIScrollPane_Impl.cpp
*  PURPOSE:     Scroll pane widget class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define CGUISCROLLPANE_NAME "CGUI/ScrollablePane"

CGUIScrollPane_Impl::CGUIScrollPane_Impl ( CGUI_Impl* pGUI, CGUIElement* pParent )
{
	m_pManager = pGUI;
	m_pData = NULL;

	// Initialize
	m_pGUI = pGUI;

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUISCROLLPANE_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );
    m_pWindow->setRect ( CEGUI::Relative, CEGUI::Rect (0.9f, 0.9f, 0.9f, 0.9f) );

	// Store the pointer to this CGUI element in the CEGUI element
	m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );
    AddEvents ();

    // If a parent is specified, add it to it's children list, if not, add it as a child to the m_pManager
    if ( pParent )
    {
        SetParent ( pParent );
    }
    else
    {
        pGUI->AddChild ( this );
		SetParent ( NULL );
    }
	SetHorizontalScrollStepSize ( 100.0f );
	SetVerticalScrollStepSize ( 100.0f );
}

CGUIScrollPane_Impl::~CGUIScrollPane_Impl ( void )
{
    m_pManager->RemoveFromRedrawQueue ( reinterpret_cast < CGUIElement* > ( ( m_pWindow )->getUserData () ) );

    // Destroy the control
    m_pWindow->destroy ();

	// Destroy the properties list
	EmptyProperties ();
}


void CGUIScrollPane_Impl::SetHorizontalScrollBar ( bool bEnabled )
{
	reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> setShowHorzScrollbar ( bEnabled );
}


void CGUIScrollPane_Impl::SetVerticalScrollBar ( bool bEnabled )
{
	reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> setShowVertScrollbar ( bEnabled );
}


void CGUIScrollPane_Impl::SetHorizontalScrollPosition ( float fPosition )
{
	try {
		reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> setHorizontalScrollPosition ( fPosition );
	} catch ( CEGUI::Exception ) {}
}


void CGUIScrollPane_Impl::SetVerticalScrollPosition ( float fPosition )
{
	try {
		reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> setVerticalScrollPosition ( fPosition );
	} catch ( CEGUI::Exception ) {}
}


float CGUIScrollPane_Impl::GetHorizontalScrollPosition ( void )
{
	try {
		return reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> getHorizontalScrollPosition ();
	} catch ( CEGUI::Exception ) { return 0; }
}


float CGUIScrollPane_Impl::GetVerticalScrollPosition ( void )
{
	try {
		return reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> getVerticalScrollPosition ();
	} catch ( CEGUI::Exception ) { return 0; }
}


void CGUIScrollPane_Impl::SetHorizontalScrollStepSize ( float fPosition )
{
	try {
		reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> setHorizontalStepSize ( fPosition );
	} catch ( CEGUI::Exception ) {}
}


void CGUIScrollPane_Impl::SetVerticalScrollStepSize ( float fPosition )
{
	try {
		reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> setVerticalStepSize ( fPosition );
	} catch ( CEGUI::Exception ) {}
}


float CGUIScrollPane_Impl::GetHorizontalScrollStepSize ( void )
{
	try {
		return reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> getHorizontalStepSize ();
	} catch ( CEGUI::Exception ) { return 0; }
}


float CGUIScrollPane_Impl::GetVerticalScrollStepSize ( void )
{
	try {
		return reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> getVerticalStepSize ();
	} catch ( CEGUI::Exception ) { return 0; }
}