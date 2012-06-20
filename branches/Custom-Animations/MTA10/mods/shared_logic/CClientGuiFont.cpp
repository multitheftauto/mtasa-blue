/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGuiFont.cpp
*  PURPOSE:     Custom font bucket
*  DEVELOPERS:  qwerty
*
*****************************************************************************/

#include <StdInc.h>


////////////////////////////////////////////////////////////////
//
// CClientGuiFont::CClientGuiFont
//
//
//
////////////////////////////////////////////////////////////////
CClientGuiFont::CClientGuiFont ( CClientManager* pManager, ElementID ID, CGuiFontItem* pFontItem ) : ClassInit ( this ), CClientRenderElement ( pManager, ID )
{
    SetTypeName ( "gui-font" );
    m_pRenderItem = pFontItem;
}


////////////////////////////////////////////////////////////////
//
// CClientGuiFont::~CClientGuiFont
//
//
//
////////////////////////////////////////////////////////////////
CClientGuiFont::~CClientGuiFont ( void )
{
    Unlink ();
}


////////////////////////////////////////////////////////////////
//
// CClientGuiFont::Unlink
//
// Remove from manager lists
//
////////////////////////////////////////////////////////////////
void CClientGuiFont::Unlink ( void )
{
    // Make sure GUI elements are not using us
    while ( m_GUIElementUserList.size () )
        (*m_GUIElementUserList.begin ())->SetFont ( "", NULL );

    CClientRenderElement::Unlink ();
}


////////////////////////////////////////////////////////////////
//
// CClientGuiFont::GetGUIFontName
//
// Get name CEGUI uses for this custom font
//
////////////////////////////////////////////////////////////////
const SString& CClientGuiFont::GetCEGUIFontName ( void )
{
    return GetGuiFontItem ()->m_strCEGUIFontName;
}


////////////////////////////////////////////////////////////////
//
// CClientGuiFont::NotifyGUIElementAttach
//
// Keep track of GUI elements using this font
//
////////////////////////////////////////////////////////////////
void CClientGuiFont::NotifyGUIElementAttach ( CClientGUIElement* pGUIElement )
{
    assert ( !MapContains ( m_GUIElementUserList, pGUIElement ) );
    MapInsert ( m_GUIElementUserList, pGUIElement );
}


////////////////////////////////////////////////////////////////
//
// CClientGuiFont::NotifyGUIElementDetach
//
// Keep track of GUI elements using this font
//
////////////////////////////////////////////////////////////////
void CClientGuiFont::NotifyGUIElementDetach ( CClientGUIElement* pGUIElement )
{
    assert ( MapContains ( m_GUIElementUserList, pGUIElement ) );
    MapRemove ( m_GUIElementUserList, pGUIElement );
}
