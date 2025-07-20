/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIComboBox_Impl.h
 *  PURPOSE:     Combobox widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement_Impl.h"
#include "CGUIListItem_Impl.h"

class CGUIComboBox_Impl : public CGUIComboBox, public CGUIElement_Impl
{
public:
    CGUIComboBox_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "");
    ~CGUIComboBox_Impl();

    eCGUIType GetType() { return CGUI_COMBOBOX; };

    CGUIListItem* AddItem(const char* szText);
    CGUIListItem* AddItem(CGUIStaticImage* pImage);
    bool          RemoveItem(int index);
    CGUIListItem* GetItemByIndex(int index);
    CGUIListItem* GetSelectedItem();
    int           GetSelectedItemIndex();
    size_t        GetItemCount();
    int           GetItemIndex(CGUIListItem* pItem);
    const char*   GetItemText(int index);
    bool          SetItemText(int index, const char* szText);
    bool          SetSelectedItemByIndex(int index);
    void          Clear();
    bool          IsOpen();

    void SetReadOnly(bool bReadonly);

    void SetSelectionHandler(GUI_CALLBACK Callback);
    void SetDropListRemoveHandler(GUI_CALLBACK Callback);

    void ShowDropList();
    void HideDropList();

    #include "CGUIElement_Inc.h"

protected:
    CFastHashMap<CEGUI::ListboxItem*, CGUIListItem_Impl*> m_Items;

    bool               Event_OnSelectionAccepted(const CEGUI::EventArgs& e);
    bool               Event_OnDropListRemoved(const CEGUI::EventArgs& e);
    CGUIListItem_Impl* GetListItem(CEGUI::ListboxItem* pItem);
    CEGUI::String      storedCaption;
    GUI_CALLBACK       m_OnSelectChange;
    GUI_CALLBACK       m_OnDropListRemoved;
};
