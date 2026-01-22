/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIEdit.h
 *  PURPOSE:     Edit box widget interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"
#include "CGUICallback.h"

class CGUIEdit : public CGUIElement
{
public:
    virtual ~CGUIEdit(){};

    virtual void SetReadOnly(bool bReadOnly) = 0;
    virtual bool IsReadOnly() = 0;
    virtual void SetMasked(bool bMasked) = 0;
    virtual bool IsMasked() = 0;

    virtual void         SetMaxLength(unsigned int uiMaxLength) = 0;
    virtual unsigned int GetMaxLength() = 0;

    virtual void         SetSelection(unsigned int uiStart, unsigned int uiEnd) = 0;
    virtual unsigned int GetSelectionStart() = 0;
    virtual unsigned int GetSelectionEnd() = 0;
    virtual unsigned int GetSelectionLength() = 0;

    virtual void         SetCaretIndex(unsigned int uiIndex) = 0;
    virtual void         SetCaretAtStart() = 0;
    virtual void         SetCaretAtEnd() = 0;
    virtual unsigned int GetCaretIndex() = 0;

    virtual void SetTextAcceptedHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetTextChangedHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetRenderingEndedHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetRenderingStartedHandler(GUI_CALLBACK Callback) = 0;
};
