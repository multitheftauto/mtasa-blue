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
	DestroyElement ();
}
