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

#ifndef __CGUIEDIT_H
#define __CGUIEDIT_H

#include "CGUIElement.h"
#include "CGUICallback.h"

class CGUIEdit : public CGUIElement
{
public:
    virtual                     ~CGUIEdit               ( void ) {};

    virtual void                SetReadOnly             ( bool bReadOnly ) = 0;
    virtual bool                IsReadOnly              ( void ) = 0;
    virtual void                SetMasked               ( bool bMasked ) = 0;
    virtual bool                IsMasked                ( void ) = 0;

    virtual void                SetMaxLength            ( unsigned int uiMaxLength ) = 0;
    virtual unsigned int        GetMaxLength            ( void ) = 0;

    virtual void                SetSelection            ( unsigned int uiStart, unsigned int uiEnd ) = 0;
    virtual unsigned int        GetSelectionStart       ( void ) = 0;
    virtual unsigned int        GetSelectionEnd         ( void ) = 0;
    virtual unsigned int        GetSelectionLength      ( void ) = 0;

    virtual void                SetCaratIndex           ( unsigned int uiIndex ) = 0;
    virtual void                SetCaratAtStart         ( void ) = 0;
    virtual void                SetCaratAtEnd           ( void ) = 0;
    virtual unsigned int        GetCaratIndex           ( void ) = 0;

    virtual void                SetTextAcceptedHandler  ( GUI_CALLBACK Callback ) = 0;
    virtual void                SetTextChangedHandler   ( GUI_CALLBACK Callback ) = 0;
};

#endif
