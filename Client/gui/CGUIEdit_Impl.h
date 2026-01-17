/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIEdit_Impl.h
 *  PURPOSE:     Edit box widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIEdit.h>
#include "CGUIElement_Impl.h"

class CGUIEdit_Impl : public CGUIEdit, public CGUIElement_Impl, public CGUITabListItem
{
public:
    CGUIEdit_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szText = "");
    ~CGUIEdit_Impl();

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly();
    void SetMasked(bool bMasked);
    bool IsMasked();

    void         SetMaxLength(unsigned int uiMaxLength);
    unsigned int GetMaxLength();

    void         SetSelection(unsigned int uiStart, unsigned int uiEnd);
    unsigned int GetSelectionStart();
    unsigned int GetSelectionEnd();
    unsigned int GetSelectionLength();

    void         SetCaretIndex(unsigned int uiIndex);
    void         SetCaretAtStart();
    void         SetCaretAtEnd();
    unsigned int GetCaretIndex();

    void SetTextAcceptedHandler(GUI_CALLBACK Callback);
    void SetTextChangedHandler(GUI_CALLBACK Callback);
    void SetRenderingEndedHandler(GUI_CALLBACK Callback);
    void SetRenderingStartedHandler(GUI_CALLBACK Callback);

    bool ActivateOnTab();

    eCGUIType GetType() { return CGUI_EDIT; };

    #include "CGUIElement_Inc.h"

protected:
    bool Event_OnTextChanged(const CEGUI::EventArgs& e);
    bool Event_OnKeyDown(const CEGUI::EventArgs& e);
    bool Event_OnRenderingEnded(const CEGUI::EventArgs& e);
    bool Event_OnRenderingStarted(const CEGUI::EventArgs& e);

    GUI_CALLBACK m_OnTextAccepted;
    GUI_CALLBACK m_OnTextChanged;
    GUI_CALLBACK m_OnRenderingEnded;
    GUI_CALLBACK m_OnRenderingStarted;
};
