/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIProgressBar_Impl.h
*  PURPOSE:     Progress bar widget class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUIPROGRESSBAR_IMPL_H
#define __CGUIPROGRESSBAR_IMPL_H

#include <gui/CGUIProgressBar.h>
#include "CGUIElement_Impl.h"

class CGUIProgressBar_Impl : public CGUIProgressBar, public CGUIElement_Impl
{
public:
                            CGUIProgressBar_Impl        ( class CGUI_Impl* pGUI, CGUIElement* pParent = NULL );
                            ~CGUIProgressBar_Impl       ( void );

    void                    SetProgress                 ( float fProgress );
    float                   GetProgress                 ( void );

    eCGUIType               GetType                     ( void ) { return CGUI_PROGRESSBAR; };

    #include "CGUIElement_Inc.h"

private:
};

#endif
