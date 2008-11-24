/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIEdit_Impl.cpp
*  PURPOSE:     Edit box widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define CGUIEDIT_NAME "CGUI/Editbox"

CGUIEdit_Impl::CGUIEdit_Impl ( CGUI_Impl* pGUI, CGUIElement* pParent, const char* szEdit )
{
	m_pManager = pGUI;
	m_pOnTextAccepted = NULL;
	m_pOnTextChanged = NULL;
	m_pData = NULL;

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the edit and set default settings
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUIEDIT_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );
    m_pWindow->setRect ( CEGUI::Relative, CEGUI::Rect (0.00f, 0.00f, 0.30f, 0.20f ) );

	// Store the pointer to this CGUI element in the CEGUI element
	m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );

    // Register our event
    m_pWindow->subscribeEvent ( CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber ( &CGUIEdit_Impl::Event_OnTextAccepted, this ) );
    m_pWindow->subscribeEvent ( CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber ( &CGUIEdit_Impl::Event_OnTextChanged, this ) );
    AddEvents ();

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if ( pParent )
    {
        SetParent ( reinterpret_cast < CGUIElement_Impl* > ( pParent ) );
    }
    else
    {
        pGUI->AddChild ( this );
		SetParent ( NULL );
    }
}


CGUIEdit_Impl::~CGUIEdit_Impl ( void )
{
	if ( m_pOnTextAccepted != NULL )
		delete m_pOnTextAccepted;
	if ( m_pOnTextChanged != NULL )
		delete m_pOnTextChanged;

    m_pManager->RemoveFromRedrawQueue ( reinterpret_cast < CGUIElement* > ( ( m_pWindow )->getUserData () ) );

    // Destroy the control
    m_pWindow->destroy ();

	// Destroy the properties list
	EmptyProperties ();
}


void CGUIEdit_Impl::SetReadOnly ( bool bReadOnly )
{
    reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> setReadOnly ( bReadOnly );
}


bool CGUIEdit_Impl::IsReadOnly ( void )
{
    return reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> isReadOnly ();
}


void CGUIEdit_Impl::SetMasked ( bool bMasked )
{
    reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> setTextMasked ( bMasked );
}


bool CGUIEdit_Impl::IsMasked ( void )
{
    return reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> isTextMasked ();
}


void CGUIEdit_Impl::SetMaxLength ( unsigned int uiMaxLength )
{
    reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> setMaxTextLength ( uiMaxLength );
}


unsigned int CGUIEdit_Impl::GetMaxLength ( void )
{
    return static_cast < unsigned int > ( reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> getMaxTextLength () );
}


void CGUIEdit_Impl::SetSelection ( unsigned int uiStart, unsigned int uiEnd )
{
    reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> setSelection ( uiStart, uiEnd );
}


unsigned int CGUIEdit_Impl::GetSelectionStart ( void )
{
    return static_cast < unsigned int > ( reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> getSelectionStartIndex () );
}


unsigned int CGUIEdit_Impl::GetSelectionEnd ( void )
{
    return static_cast < unsigned int > ( reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> getSelectionEndIndex () );
}


unsigned int CGUIEdit_Impl::GetSelectionLength ( void )
{
    return static_cast < unsigned int > ( reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> getSelectionLength () );
}


void CGUIEdit_Impl::SetCaratIndex ( unsigned int uiIndex )
{
    return reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> setCaratIndex ( uiIndex );
}


void CGUIEdit_Impl::SetCaratAtStart ( void )
{
    reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) ->setCaratIndex ( 0 );
}


void CGUIEdit_Impl::SetCaratAtEnd ( void )
{
    reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) ->setCaratIndex ( GetText ().length () );
}


unsigned int CGUIEdit_Impl::GetCaratIndex ( void )
{
    return static_cast < unsigned int > ( reinterpret_cast < CEGUI::Editbox* > ( m_pWindow ) -> getCaratIndex () );
}


void CGUIEdit_Impl::SetTextAcceptedHandler ( GUI_CALLBACK Callback )
{
    m_pOnTextAccepted = new GUI_CALLBACK ( Callback );
}


void CGUIEdit_Impl::SetTextChangedHandler ( GUI_CALLBACK Callback )
{
    m_pOnTextChanged = new GUI_CALLBACK ( Callback );
}


bool CGUIEdit_Impl::Event_OnTextAccepted ( const CEGUI::EventArgs& e )
{
	if ( m_pOnTextAccepted )
		(*m_pOnTextAccepted) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}


bool CGUIEdit_Impl::Event_OnTextChanged ( const CEGUI::EventArgs& e )
{
	if ( m_pOnTextChanged )
		(*m_pOnTextChanged) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}
