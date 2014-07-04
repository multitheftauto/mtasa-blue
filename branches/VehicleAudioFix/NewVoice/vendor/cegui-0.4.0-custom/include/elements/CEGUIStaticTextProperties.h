/************************************************************************
	filename: 	CEGUIStaticTextProperties.h
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Interface for properties for the StaticText class.
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
#ifndef _CEGUIStaticTextProperties_h_
#define _CEGUIStaticTextProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of StaticTextProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the StaticText class
*/
namespace StaticTextProperties
{
/*!
\brief
	Property to access the text colours for the StaticText widget.

	\par Usage:
		- Name: TextColours
		- Format: "tl:[aarrggbb] tr:[aarrggbb] bl:[aarrggbb] br:[aarrggbb]".

	\par Where:
		- tl:[aarrggbb] is the top-left colour value specified as ARGB (hex).
		- tr:[aarrggbb] is the top-right colour value specified as ARGB (hex).
		- bl:[aarrggbb] is the bottom-left colour value specified as ARGB (hex).
		- br:[aarrggbb] is the bottom-right colour value specified as ARGB (hex).
*/
class TextColours : public Property
{
public:
	TextColours() : Property(
		"TextColours",
		"Property to get/set the text colours for the StaticText widget.  Value is \"tl:[aarrggbb] tr:[aarrggbb] bl:[aarrggbb] br:[aarrggbb]\".",
		"tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the horizontal formatting mode setting.

	\par Usage:
		- Name: HorzFormatting
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
class HorzFormatting : public Property
{
public:
	HorzFormatting() : Property(
		"HorzFormatting",
		"Property to get/set the horizontal formatting mode.  Value is one of the HorzFormatting strings.",
		"LeftAligned")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the vertical formatting mode setting.

	\par Usage:
		- Name: VertFormatting
		- Format: "[text]".

	\par Where [text] is one of:
		- "TopAligned"
		- "BottomAligned"
		- "VertCentred"
*/
class VertFormatting : public Property
{
public:
	VertFormatting() : Property(
		"VertFormatting",
		"Property to get/set the vertical formatting mode.  Value is one of the VertFormatting strings.",
		"VertCentred")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the setting for enabling the vertical scroll bar.

	\par Usage:
		- Name: VertScrollbar
		- Format: "[text]"

		\par Where [Text] is:
		- "True" to indicate the scroll bar is enabled and will be shown when needed.
		- "False" to indicate the scroll bar is disabled and will never be shown
*/
class VertScrollbar : public Property
{
public:
	VertScrollbar() : Property(
		"VertScrollbar",
		"Property to get/set the setting for the vertical scroll bar.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the setting for enabling the horizontal scroll bar.

	\par Usage:
		- Name: HorzScrollbar
		- Format: "[text]"

		\par Where [Text] is:
		- "True" to indicate the scroll bar is enabled and will be shown when needed.
		- "False" to indicate the scroll bar is disabled and will never be shown
*/
class HorzScrollbar : public Property
{
public:
	HorzScrollbar() : Property(
		"HorzScrollbar",
		"Property to get/set the setting for the horizontal scroll bar.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

} // End of  StaticTextProperties namespace section

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIStaticTextProperties_h_
