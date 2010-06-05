/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUISize.h
*  PURPOSE:     Widget size interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUI_CGUISIZE
#define __CGUI_CGUISIZE

class CGUISize
{
    public:
	            CGUISize        ( void ) {}
	            CGUISize        ( float width, float height ) : fWidth ( width ), fHeight (height ) {}

	bool        operator==      ( const CGUISize& other ) const;
	bool        operator!=      ( const CGUISize& other ) const;

	float       fWidth;
    float       fHeight;
};

#endif
