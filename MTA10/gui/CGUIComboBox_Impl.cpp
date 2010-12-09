/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIComboBox_Impl.h
*  PURPOSE:     Combobox widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_Toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace google;

#define CGUICOMBOBOX_NAME "CGUI/Combobox"

CGUIComboBox_Impl::CGUIComboBox_Impl ( CGUI_Impl* pGUI, CGUIElement* pParent, const char* szCaption )
{
    m_pManager = pGUI;
    m_Items.set_deleted_key ( (CEGUI::ListboxItem *)0xFFFFFFFF );
    m_Items.set_empty_key ( (CEGUI::ListboxItem *)0x00000000 );

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUICOMBOBOX_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );

    CEGUI::String strText;
    strText.assign( (CEGUI::utf8*) szCaption ); // assign as UTF8 string
    m_pWindow->setText ( strText );

    m_pWindow->setSize ( CEGUI::Absolute, CEGUI::Size ( 128.0f, 24.0f ) );
    m_pWindow->setVisible ( true );

    // This needs a better alternative, so changing comboBox will change this - Jyrno42
    storedCaption = strText;

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );

    //Add out changed event
    m_pWindow->subscribeEvent ( CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber ( &CGUIComboBox_Impl::Event_OnSelectionAccepted, this ) );
    
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
    Clear ();
    DestroyElement ();
}


CGUIListItem* CGUIComboBox_Impl::AddItem ( const char* szText )
{
    CGUIListItem_Impl* pNewItem = new CGUIListItem_Impl ( szText, CGUIListItem_Impl::Type::TextItem, NULL );
    CEGUI::ListboxItem* pListboxItem = pNewItem->GetListItem ();
    reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> addItem ( pListboxItem );
    m_Items [ pNewItem->GetListItem () ] = pNewItem;
    return pNewItem;
}

bool CGUIComboBox_Impl::RemoveItem ( int index )
{
    try
    {
        CEGUI::ListboxItem* pItem = reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) ->getListboxItemFromIndex ( index );
        if( pItem->isSelected( ) ) // if this is currently selected, let's update the editbox.
        {
            m_pWindow->setText ( storedCaption );
        }
        reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> removeItem ( pItem );
        return true;
    }
    catch(...)
    {
        return false;
    }
    return false;
}

CGUIListItem* CGUIComboBox_Impl::GetSelectedItem ( void )
{
    return GetListItem ( reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> getSelectedItem () );
}

int CGUIComboBox_Impl::GetSelectedItemIndex( void )
{
    CEGUI::ListboxItem* pItem = reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> getSelectedItem ();
    dense_hash_map < CEGUI::ListboxItem*, CGUIListItem_Impl* >::iterator it;
    it = m_Items.find ( pItem );
    if ( it == m_Items.end () )
        return -1;
    
    try
    {
        return reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) ->getItemIndex( it->first );
    }
    catch(...)
    {
        return -1;
    }
}

int CGUIComboBox_Impl::GetItemIndex( CGUIListItem* pItem )
{
    dense_hash_map < CEGUI::ListboxItem*, CGUIListItem_Impl* >::iterator it;
    bool found;
    
    for ( it = m_Items.begin (); it != m_Items.end (); it++ )
    {
        if( it->second == pItem )
        {
            found = true;
            break;
        }
    }
    if ( found )
    {    
        try
        {
            return reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> getItemIndex( it->first );
        }
        catch(...)
        {
            return -1;
        }
    }
    
    return -1;
}

const char* CGUIComboBox_Impl::GetItemText ( int index )
{
    try
    {
        CEGUI::ListboxItem* pItem = reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) ->getListboxItemFromIndex ( index );
        return pItem->getText( ).c_str( );
    }
    catch(...)
    {
        return NULL;
    }
    return NULL;
}

bool CGUIComboBox_Impl::SetItemText ( int index, const char* szText )
{
    try
    {
        CEGUI::ListboxItem* pItem = reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) ->getListboxItemFromIndex ( index );
        CEGUI::String strText;
        strText.assign( (CEGUI::utf8*) szText ); // assign as UTF8 string
        pItem->setText( strText, NULL );
        if( pItem->isSelected( ) ) // if this is currently selected, let's update the editbox.
        {
            strText.assign( (CEGUI::utf8*) szText );
            m_pWindow->setText ( strText );
        }
        return true;
    }
    catch(...)
    {
        return false;
    }
    return false;
}

bool CGUIComboBox_Impl::SetSelectedItemByIndex ( int index )
{
    try
    {
        reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> clearAllSelections( );

        if( index == 0 )
        {
            m_pWindow->setText ( storedCaption );
        }
        else
        {
            CEGUI::ListboxItem* pItem = reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) ->getListboxItemFromIndex ( index );
            if( pItem != NULL )
            {
                pItem->setSelected( true );
                m_pWindow->setText ( pItem->getText( ) );
                return true;
            }
        }
    }
    catch(...)
    {
        return false;
    }
    return false;
}


void CGUIComboBox_Impl::Clear ( void )
{
    reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> getDropList () -> resetList ();

    dense_hash_map < CEGUI::ListboxItem*, CGUIListItem_Impl* >::iterator it;
    for ( it = m_Items.begin (); it != m_Items.end (); it++ )
    {
        delete it->second;
    }

    m_Items.clear ();
    m_pWindow->setText ( storedCaption );
}

void CGUIComboBox_Impl::SetReadOnly ( bool bReadonly )
{
    reinterpret_cast < CEGUI::Combobox* > ( m_pWindow ) -> setReadOnly ( bReadonly );
}


CGUIListItem_Impl* CGUIComboBox_Impl::GetListItem ( CEGUI::ListboxItem* pItem )
{
    dense_hash_map < CEGUI::ListboxItem*, CGUIListItem_Impl* >::iterator it;
    it = m_Items.find ( pItem );
    if ( it == m_Items.end () )
        return NULL;

    return it->second;
}


void CGUIComboBox_Impl::SetSelectionHandler ( GUI_CALLBACK Callback  )
{
    m_OnSelectChange = Callback;
}


bool CGUIComboBox_Impl::Event_OnSelectionAccepted ( const CEGUI::EventArgs& e )
{
    if ( m_OnSelectChange )
        m_OnSelectChange ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}

