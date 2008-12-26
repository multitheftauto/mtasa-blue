/*********************************************************
*
*  Multi Theft Auto :: Codename "Blue" (version 1.1)
*  Copyright © 2003-2006 MTA.  All Rights Reserved.
*
*  Grand Theft Auto is © 1997-2005 Rockstar North
*
*  THE FOLLOWING SOURCES ARE CONSIDIERED HIGHLY CONFIDENTIAL
*  AND ARE TO BE VIEWED ONLY BY MEMBERS OF THE MULTI THEFT
*  AUTO STAFF.  ANY VIOLATION OF THESE TERMS CAN LEAD TO
*  CRIMINAL PROSECUTION AND/OR LARGE FINES UNDER UNITED
*  STATES LAW.
*
**********************************************************/

#ifndef __CGUICOMBOBOX_IMPL_H
#define __CGUICOMBOBOX_IMPL_H

#include "CGUIElement_Impl.h"
#include "CGUIListItem_Impl.h"
#include <list>

class CGUIComboBox_Impl : public CGUIComboBox, public CGUIElement_Impl
{
public:
                                CGUIComboBox_Impl           ( class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "" );
                                ~CGUIComboBox_Impl          ( void );

    void                        Click                       ( void );

    void                        SetOnClickHandler           ( const GUI_CALLBACK & Callback );

	eCGUIType					GetType						( void ) { return CGUI_COMBOBOX; };

    CGUIListItem*               AddItem                     ( const char* szText );
    CGUIListItem*               GetSelectedItem             ( void );
    void                        Clear                       ( void );

    void                        SetReadOnly                 ( bool bReadonly );

    #include "CGUIElement_Inc.h"

protected:

    std::list < CGUIListItem_Impl* >    m_Items;

    CGUIListItem_Impl*          GetListItem                 ( CEGUI::ListboxItem* pItem );

    bool                        Event_OnClick               ( const CEGUI::EventArgs& e );

    GUI_CALLBACK*				m_pOnClick;
};

#endif