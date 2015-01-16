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

#ifndef __CGUIMEMO_H
#define __CGUIMEMO_H

#include "CGUIElement.h"
#include "CGUICallback.h"

class CGUIMemo : public CGUIElement
{
public:
    virtual                 ~CGUIMemo               ( void ) {};

    virtual void            SetReadOnly             ( bool bReadOnly ) = 0;
    virtual bool            IsReadOnly              ( void ) = 0;

    virtual unsigned int    GetCaretIndex           ( void ) = 0;
    virtual void            SetCaretIndex           ( unsigned int uiIndex ) = 0;

    virtual float           GetVerticalScrollPosition   ( void ) = 0;
    virtual void            SetVerticalScrollPosition   ( float fPosition ) = 0;
    virtual float           GetScrollbarDocumentSize    ( void ) = 0;
    virtual float           GetScrollbarPageSize        ( void ) = 0;

    virtual void            EnsureCaratIsVisible    ( void ) = 0;

    virtual void            SetTextChangedHandler   ( const GUI_CALLBACK & Callback ) = 0;
};

#endif
