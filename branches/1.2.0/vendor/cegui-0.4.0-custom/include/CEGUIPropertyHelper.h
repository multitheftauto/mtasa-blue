/************************************************************************
	filename: 	CEGUIPropertyHelper.h
	created:	6/7/2004
	author:		Paul D Turner
	
	purpose:	Interface to the PropertyHelper class
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#ifndef _CEGUIPropertyHelper_h_
#define _CEGUIPropertyHelper_h_

#include "CEGUIWindow.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Helper class used to convert various data types to and from the format expected in Propery strings
*/
class CEGUIEXPORT PropertyHelper
{
public:
	static	float	stringToFloat(const String& str);
	static	uint	stringToUint(const String& str);
	static	int		stringToInt(const String& str);
	static	bool	stringToBool(const String& str);
	static	Size	stringToSize(const String& str);
	static	Point	stringToPoint(const String& str);
	static	Rect	stringToRect(const String& str);
	static	MetricsMode	stringToMetricsMode(const String& str);
	static const Image*	stringToImage(const String& str);
	static	colour	stringToColour(const String& str);
	static	ColourRect	stringToColourRect(const String& str);
	static	UDim	stringToUDim(const String& str);
	static	UVector2	stringToUVector2(const String& str);
	static	URect	stringToURect(const String& str);

	static String	floatToString(float val);
	static String	uintToString(uint val);
	static String	intToString(int val);
	static String	boolToString(bool val);
	static String	sizeToString(const Size& val);
	static String	pointToString(const Point& val);
	static String	rectToString(const Rect& val);
	static String	metricsModeToString(MetricsMode val);
	static String	imageToString(const Image* const val);
	static String	colourToString(const colour& val);
	static String	colourRectToString(const ColourRect& val);
	static String	udimToString(const UDim& val);
	static String	uvector2ToString(const UVector2& val);
	static String	urectToString(const URect& val);
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIPropertyHelper_h_
