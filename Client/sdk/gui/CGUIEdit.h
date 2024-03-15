/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIEdit.h
 *  PURPOSE:     Edit box widget interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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

    virtual void         SetMaxLength(std::uint32_t uiMaxLength) = 0;
    virtual std::uint32_t GetMaxLength() = 0;

    virtual void         SetSelection(std::uint32_t uiStart, std::uint32_t uiEnd) = 0;
    virtual std::uint32_t GetSelectionStart() = 0;
    virtual std::uint32_t GetSelectionEnd() = 0;
    virtual std::uint32_t GetSelectionLength() = 0;

    virtual void         SetCaretIndex(std::uint32_t uiIndex) = 0;
    virtual void         SetCaretAtStart() = 0;
    virtual void         SetCaretAtEnd() = 0;
    virtual std::uint32_t GetCaretIndex() = 0;

    virtual void SetTextAcceptedHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetTextChangedHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetRenderingEndedHandler(GUI_CALLBACK Callback) = 0;
    virtual void SetRenderingStartedHandler(GUI_CALLBACK Callback) = 0;
};
