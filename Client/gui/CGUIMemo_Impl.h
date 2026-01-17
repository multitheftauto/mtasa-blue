/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIMemo_Impl.h
 *  PURPOSE:     Multi-line edit box widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIMemo.h>
#include "CGUIElement_Impl.h"

class CGUIMemo_Impl : public CGUIMemo, public CGUIElement_Impl, public CGUITabListItem
{
public:
    CGUIMemo_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szText = "");
    ~CGUIMemo_Impl();

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly();

    size_t GetCaretIndex();
    void   SetCaretIndex(size_t Index);

    float GetVerticalScrollPosition();
    void  SetVerticalScrollPosition(float fPosition);
    float GetMaxVerticalScrollPosition();
    float GetScrollbarDocumentSize();
    float GetScrollbarPageSize();

    void         SetSelection(unsigned int uiStart, unsigned int uiEnd);
    unsigned int GetSelectionStart();
    unsigned int GetSelectionEnd();
    unsigned int GetSelectionLength();

    void EnsureCaratIsVisible();

    bool ActivateOnTab();

    void SetTextChangedHandler(const GUI_CALLBACK& Callback);

    eCGUIType GetType() { return CGUI_MEMO; };

    #include "CGUIElement_Inc.h"

private:
    bool Event_TextChanged(const CEGUI::EventArgs& e);
    bool Event_OnKeyDown(const CEGUI::EventArgs& e);

    GUI_CALLBACK m_TextChanged;
};
