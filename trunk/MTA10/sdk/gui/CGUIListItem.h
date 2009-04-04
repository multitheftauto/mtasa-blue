/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUIListItem.h
*  PURPOSE:     List widget item interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUILISTITEM_H
#define __CGUILISTITEM_H

#include "CGUIStaticImage.h"
#include <string>

class CGUIListItem
{
public:
    virtual                         ~CGUIListItem           ( void ) {};

	virtual void					SetFont					( const char *szFontName ) = 0;
    virtual void                    SetText                 ( const char *Text ) = 0;
    virtual std::string             GetText                 ( void ) = 0;

	virtual void*					GetData					( void ) = 0;
	virtual void					SetData					( void* pData ) = 0;

	virtual void					SetDisabled				( bool bDisabled ) = 0;
	virtual void					SetImage				( CGUIStaticImage* Image ) = 0;

    virtual bool                    GetSelectedState        ( void ) = 0;
    virtual void                    SetSelectedState        ( bool bState ) = 0;
};

#endif
