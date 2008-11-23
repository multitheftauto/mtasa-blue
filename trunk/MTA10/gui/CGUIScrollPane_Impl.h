/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIScrollPane_Impl.h
*  PURPOSE:     Scroll pane widget class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUISCROLLPANE_IMPL_H
#define __CGUISCROLLPANE_IMPL_H

#include <gui/CGUIScrollPane.h>
#include "CGUIElement_Impl.h"

class CGUIScrollPane_Impl : public CGUIScrollPane, public CGUIElement_Impl
{
public:
                            CGUIScrollPane_Impl			( class CGUI_Impl* pGUI, CGUIElement* pParent = NULL );
                            ~CGUIScrollPane_Impl		( void );

	void					SetHorizontalScrollBar		( bool bEnabled );
	void					SetVerticalScrollBar		( bool bEnabled );

	void					SetHorizontalScrollPosition	( float fPosition );
	float					GetHorizontalScrollPosition	( void );

	void					SetVerticalScrollPosition	( float fPosition );
	float					GetVerticalScrollPosition	( void );

	void					SetHorizontalScrollStepSize	( float fPosition );
	float					GetHorizontalScrollStepSize	( void );

	void					SetVerticalScrollStepSize	( float fPosition );
	float					GetVerticalScrollStepSize	( void );

	eCGUIType				GetType					( void ) { return CGUI_SCROLLPANE; };

    #include "CGUIElement_Inc.h"

private:
    class CGUI_Impl*        m_pGUI;
};

#endif
