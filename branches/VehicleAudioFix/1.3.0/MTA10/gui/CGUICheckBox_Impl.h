/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUICheckBox_Impl.h
*  PURPOSE:     Checkbox widget class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUICHECKBOX_IMPL_H
#define __CGUICHECKBOX_IMPL_H

#include <gui/CGUICheckBox.h>
#include "CGUIElement_Impl.h"

class CGUICheckBox_Impl : public CGUICheckBox, public CGUIElement_Impl
{
public:
                            CGUICheckBox_Impl        ( class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "", bool bChecked = false );
                            ~CGUICheckBox_Impl       ( void );

    void                    SetSelected              ( bool bChecked );
    bool                    GetSelected              ( void );

    eCGUIType               GetType                  ( void ) { return CGUI_CHECKBOX; };

    #include "CGUIElement_Inc.h"
};

#endif
