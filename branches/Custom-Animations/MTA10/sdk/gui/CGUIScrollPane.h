/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUIScrollPane.h
*  PURPOSE:     Scroll pane widget interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUISCROLLPANE_H
#define __CGUISCROLLPANE_H

#include "CGUIElement.h"

class CGUIScrollPane : public CGUIElement
{
public:
    virtual                         ~CGUIScrollPane         ( void ) {};

    virtual void                    SetHorizontalScrollBar  ( bool bEnabled ) = 0;
    virtual void                    SetVerticalScrollBar    ( bool bEnabled ) = 0;

    virtual void                    SetHorizontalScrollPosition ( float fPosition ) = 0;
    virtual float                   GetHorizontalScrollPosition ( void ) = 0;
    virtual void                    SetVerticalScrollPosition   ( float fPosition ) = 0;
    virtual float                   GetVerticalScrollPosition   ( void ) = 0;

    virtual void                    SetHorizontalScrollStepSize ( float fPosition ) = 0;
    virtual float                   GetHorizontalScrollStepSize ( void ) = 0;

    virtual void                    SetVerticalScrollStepSize   ( float fPosition ) = 0;
    virtual float                   GetVerticalScrollStepSize   ( void ) = 0;
};

#endif
