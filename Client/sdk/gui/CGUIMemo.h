/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIMemo.h
 *  PURPOSE:     Multi-line edit box widget interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"
#include "CGUICallback.h"

class CGUIMemo : public CGUIElement
{
public:
    virtual ~CGUIMemo(){};

    virtual void SetReadOnly(bool bReadOnly) = 0;
    virtual bool IsReadOnly() = 0;

    virtual std::uint32_t GetCaretIndex() = 0;
    virtual void         SetCaretIndex(std::uint32_t uiIndex) = 0;

    virtual float GetVerticalScrollPosition() = 0;
    virtual void  SetVerticalScrollPosition(float fPosition) = 0;
    virtual float GetMaxVerticalScrollPosition() = 0;
    virtual float GetScrollbarDocumentSize() = 0;
    virtual float GetScrollbarPageSize() = 0;

    virtual void         SetSelection(std::uint32_t uiStart, std::uint32_t uiEnd) = 0;
    virtual std::uint32_t GetSelectionStart() = 0;
    virtual std::uint32_t GetSelectionEnd() = 0;
    virtual std::uint32_t GetSelectionLength() = 0;

    virtual void EnsureCaratIsVisible() = 0;

    virtual void SetTextChangedHandler(const GUI_CALLBACK& Callback) = 0;
};
