/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUIScrollBar.h
*  PURPOSE:     Scroll bar widget interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUISCROLLBAR_H
#define __CGUISCROLLBAR_H

#include "CGUIElement.h"
#include "CGUICallback.h"

class CGUIScrollBar : public CGUIElement
{
public:
    virtual                     ~CGUIScrollBar		( void ) {};

    virtual void                SetOnScrollHandler	( const GUI_CALLBACK & Callback ) = 0;

	virtual void				SetScrollPosition	( float fPosition ) = 0;
	virtual float				GetScrollPosition	( void ) = 0;
};

#endif
