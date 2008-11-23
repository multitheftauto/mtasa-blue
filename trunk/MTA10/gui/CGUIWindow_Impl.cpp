/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIWindow_Impl.cpp
*  PURPOSE:     Window widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define CGUIWINDOW_NAME "CGUI/FrameWindow"

CGUIWindow_Impl::CGUIWindow_Impl ( CGUI_Impl* pGUI, CGUIElement* pParent, const char* szCaption )
{
	m_pManager = pGUI;
	m_pOnCloseClick = NULL;
	m_pOnKeyDown = NULL;
	m_pData = NULL;

    // Get an unique identifier for CEGUI
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUIWINDOW_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );
    m_pWindow->setRect ( CEGUI::Relative, CEGUI::Rect (0.10f, 0.10f, 0.60f, 0.90f) );
	m_pWindow->setAlpha ( 0.8f );
	
	// Store the pointer to this CGUI element in the CEGUI element
	m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );

	// Set fixed minimum size to 96x48
	m_pWindow->setMetricsMode ( CEGUI::Absolute );
	m_pWindow->setMinimumSize ( CEGUI::Size ( 96.0f, 48.0f ) );
 
	// Some window specific style options
	reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> setTitlebarFont ( "default-bold-small" );
	char *szUpperCaption = new char [ strlen ( szCaption ) + 1 ];
	strcpy ( szUpperCaption, szCaption );
	_strupr ( szUpperCaption );
	m_pWindow->setText ( szUpperCaption );
	delete [] szUpperCaption;

    // Register our events
    m_pWindow->subscribeEvent ( CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber ( CGUIWindow_Impl::Event_OnCloseClick, this ) );
    m_pWindow->subscribeEvent ( CEGUI::FrameWindow::EventKeyDown, CEGUI::Event::Subscriber ( CGUIWindow_Impl::Event_OnKeyDown, this ) );
    AddEvents ();

	// Disable rolling up, because we don't need it and it causes a freeze
	reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> setRollupEnabled ( false );

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


CGUIWindow_Impl::~CGUIWindow_Impl ( void )
{
	if ( m_pOnCloseClick != NULL )
		delete m_pOnCloseClick;
	if ( m_pOnKeyDown != NULL )
		delete m_pOnKeyDown;

    m_pManager->RemoveFromRedrawQueue ( reinterpret_cast < CGUIElement* > ( ( m_pWindow )->getUserData () ) );

    // Destroy the control
    m_pWindow->destroy ();

	// Destroy the properties list
	EmptyProperties ();
}


void CGUIWindow_Impl::SetMovable ( bool bMovable )
{
    reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> setDragMovingEnabled ( bMovable );
}


bool CGUIWindow_Impl::IsMovable ( void )
{
    return reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> isDragMovingEnabled ();
}


void CGUIWindow_Impl::SetSizingEnabled ( bool bResizeEnabled )
{
    reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> setSizingEnabled ( bResizeEnabled );
}


bool CGUIWindow_Impl::IsSizingEnabled ( void )
{
    return reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> isSizingEnabled ();
}


void CGUIWindow_Impl::SetFrameEnabled ( bool bFrameEnabled )
{
    reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> setFrameEnabled ( bFrameEnabled );
}


bool CGUIWindow_Impl::IsFrameEnabled ( void )
{
    return reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> isFrameEnabled ();
}


void CGUIWindow_Impl::SetCloseButtonEnabled ( bool bCloseButtonEnabled )
{
    reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> setCloseButtonEnabled ( bCloseButtonEnabled );
}


bool CGUIWindow_Impl::IsCloseButtonEnabled ( void )
{
    return reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> isCloseButtonEnabled ();
}


void CGUIWindow_Impl::SetTitlebarEnabled ( bool bTitlebarEnabled )
{
    reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> setTitleBarEnabled ( bTitlebarEnabled );
}


bool CGUIWindow_Impl::IsTitlebarEnabled ( void )
{
    return reinterpret_cast < CEGUI::FrameWindow* > ( m_pWindow ) -> isTitleBarEnabled ();
}


void CGUIWindow_Impl::SetCloseClickHandler ( GUI_CALLBACK Callback )
{
    m_pOnCloseClick = new GUI_CALLBACK ( Callback );
}


void CGUIWindow_Impl::SetKeyDownHandler ( GUI_CALLBACK Callback )
{
    m_pOnKeyDown = new GUI_CALLBACK ( Callback );
}


bool CGUIWindow_Impl::Event_OnCloseClick ( const CEGUI::EventArgs& e )
{
	if ( m_pOnCloseClick )
		(*m_pOnCloseClick) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}


bool CGUIWindow_Impl::Event_OnKeyDown ( const CEGUI::EventArgs& e )
{
	if ( m_pOnKeyDown )
		(*m_pOnKeyDown) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}
