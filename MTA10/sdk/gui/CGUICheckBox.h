/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUICheckBox.h
*  PURPOSE:     Check box widget interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUICHECKBOX_H
#define __CGUICHECKBOX_H

#include "CGUIElement.h"
#include "CGUICallback.h"

class CGUICheckBox : public CGUIElement
{
public:
    virtual                     ~CGUICheckBox       ( void ) {};

	virtual void				SetSelected			( bool bSelected ) = 0;
	virtual bool				GetSelected			( void ) = 0;
};

#endif
