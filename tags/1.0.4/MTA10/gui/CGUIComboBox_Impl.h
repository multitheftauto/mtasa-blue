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
    CGUIListItem*               GetSelectedItem             ( void );
    void                        Clear                       ( void );

    void                        SetReadOnly                 ( bool bReadonly );

    #include "CGUIElement_Inc.h"

protected:

    google::dense_hash_map < CEGUI::ListboxItem*, CGUIListItem_Impl* > m_Items;

    CGUIListItem_Impl*          GetListItem                 ( CEGUI::ListboxItem* pItem );
};

#endif
