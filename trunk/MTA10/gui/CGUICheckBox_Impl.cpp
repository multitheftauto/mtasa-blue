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

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUICHECKBOX_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );

    m_pWindow->setText ( CGUI_Impl::GetUTFString(szCaption) );

    m_pWindow->setSize ( CEGUI::Absolute, CEGUI::Size ( 128.0f, 16.0f ) );
    m_pWindow->setVisible ( true );

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );

    // Register our events
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
    DestroyElement ();
}


void CGUICheckBox_Impl::SetSelected ( bool bChecked )
{
    reinterpret_cast < CEGUI::Checkbox * > ( m_pWindow ) -> setSelected ( !bChecked );
}


bool CGUICheckBox_Impl::GetSelected ( void )
{
    return !( reinterpret_cast < CEGUI::Checkbox * > ( m_pWindow ) -> isSelected () );
}
