/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUITab_Impl.h
*  PURPOSE:     Tab widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUITAB_IMPL_H
#define __CGUITAB_IMPL_H

#include <gui/CGUITab.h>
#include "CGUIElement_Impl.h"

class CGUITab_Impl : public CGUITab, public CGUIElement_Impl, public CGUITabList
{
public:
                            CGUITab_Impl            ( class CGUI_Impl* pManager, CGUIElement_Impl* pParent, const char* szCaption );
                            ~CGUITab_Impl           ( void );

    void                    SetCaption              ( const char* szCaption );

    eCGUIType               GetType                 ( void ) { return CGUI_TAB; };

    #include "CGUIElement_Inc.h"
};

#endif
