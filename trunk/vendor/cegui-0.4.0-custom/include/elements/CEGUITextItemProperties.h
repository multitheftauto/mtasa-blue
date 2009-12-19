/************************************************************************
	filename: 	CEGUITextItemProperties.h
	created:	8/4/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
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
#ifndef _CEGUITextItemProperties_h_
#define _CEGUITextItemProperties_h_

#include "CEGUIProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of TextItemProperties namespace section
namespace TextItemProperties
{

/*!
\brief
	Property to access the X offset of the text on the item.

	\par Usage:
		- Name: TextXOffset
		- Format: "[float]".

	\par Where:
		- [float] represents the X offset of the text on the item.
*/
class TextXOffset : public Property
{
public:
	TextXOffset() : Property(
		"TextXOffset",
		"Property to get/set the X offset of the text on the item.  Value is a float.",
		"0.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the text colour of the item.

	\par Usage:
		- Name: TextColour
		- Format: "[aarrggbb]".

	\par Where:
		- [aarrggbb] represents the text colour of the item.
*/
class TextColour : public Property
{
public:
	TextColour() : Property(
		"TextColour",
		"Property to get/set the text colour of the item.  Value is a colour.",
		"00FFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the horizontal formatting mode setting.

	\par Usage:
		- Name: TextFormatting
		- Format: "[text]".

	\par Where [text] is one of:
		- "LeftAligned"
		- "RightAligned"
		- "HorzCentred"
		- "HorzJustified"
		- "WordWrapLeftAligned"
		- "WordWrapRightAligned"
		- "WordWrapCentred"
		- "WordWrapJustified"
*/
class TextFormatting : public Property
{
public:
	TextFormatting() : Property(
		"TextFormatting",
		"Property to get/set the horizontal formatting mode.  Value is one of the HorzFormatting strings.",
		"LeftAligned")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

} // End of  TextItemProperties namespace section
} // End of  CEGUI namespace section

#endif	// end of guard _CEGUITextItemProperties_h_
