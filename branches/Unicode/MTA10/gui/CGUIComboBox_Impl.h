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

#ifndef __CGUICOMBOBOX_IMPL_H
#define __CGUICOMBOBOX_IMPL_H

#include "CGUIElement_Impl.h"
#include "CGUIListItem_Impl.h"

class CGUIComboBox_Impl : public CGUIComboBox, public CGUIElement_Impl
{
public:
                                CGUIComboBox_Impl           ( class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "" );
                                ~CGUIComboBox_Impl          ( void );

    eCGUIType                   GetType                     ( void ) { return CGUI_COMBOBOX; };

    CGUIListItem*               AddItem                     ( const char* szText );
    bool                        RemoveItem                  ( int index );
    CGUIListItem*               GetSelectedItem             ( void );
    int                         GetSelectedItemIndex        ( void );
    int                         GetItemIndex                ( CGUIListItem* pItem );
    const char*                 GetItemText                 ( int index );
    bool                        SetItemText                 ( int index, const char* szText );
    bool                        SetSelectedItemByIndex      ( int index );
    void                        Clear                       ( void );

    void                        SetReadOnly                 ( bool bReadonly );

    void                        SetSelectionHandler         ( GUI_CALLBACK Callback );

    #include "CGUIElement_Inc.h"

protected:

    google::dense_hash_map < CEGUI::ListboxItem*, CGUIListItem_Impl* > m_Items;

    bool                                Event_OnSelectionAccepted  ( const CEGUI::EventArgs& e );
    CGUIListItem_Impl*                  GetListItem               ( CEGUI::ListboxItem* pItem );
    std::string                         storedCaption;
    GUI_CALLBACK                        m_OnSelectChange;
};

#endif
