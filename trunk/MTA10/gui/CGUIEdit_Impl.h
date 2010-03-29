/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIEdit_Impl.h
*  PURPOSE:     Edit box widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Marcus Bauer <mabako@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUIEDIT_IMPL_H
#define __CGUIEDIT_IMPL_H

#include <gui/CGUIEdit.h>
#include "CGUIElement_Impl.h"

class CGUIEdit_Impl : public CGUIEdit, public CGUIElement_Impl, public CGUITabListItem
{
public:
                        CGUIEdit_Impl           ( class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szText = "" );
                        ~CGUIEdit_Impl          ( void );

    void                SetReadOnly             ( bool bReadOnly );
    bool                IsReadOnly              ( void );
    void                SetMasked               ( bool bMasked );
    bool                IsMasked                ( void );

    void                SetMaxLength            ( unsigned int uiMaxLength );
    unsigned int        GetMaxLength            ( void );

    void                SetSelection            ( unsigned int uiStart, unsigned int uiEnd );
    unsigned int        GetSelectionStart       ( void );
    unsigned int        GetSelectionEnd         ( void );
    unsigned int        GetSelectionLength      ( void );

    void                SetCaratIndex           ( unsigned int uiIndex );
    void                SetCaratAtStart         ( void );
    void                SetCaratAtEnd           ( void );
    unsigned int        GetCaratIndex           ( void );

    void                SetTextAcceptedHandler  ( GUI_CALLBACK Callback );
    void                SetTextChangedHandler   ( GUI_CALLBACK Callback );

    bool                ActivateOnTab           ( void );

    eCGUIType           GetType                 ( void ) { return CGUI_EDIT; };

    #include "CGUIElement_Inc.h"

protected:
    bool                Event_OnTextChanged     ( const CEGUI::EventArgs& e );
    bool                Event_OnKeyDown         ( const CEGUI::EventArgs& e );

    GUI_CALLBACK        m_OnTextAccepted;
    GUI_CALLBACK        m_OnTextChanged;
};

#endif
