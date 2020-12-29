/************************************************************************
	filename: 	CEGUIMenuBaseProperties.h
	created:	5/4/2005
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
#ifndef _CEGUIMenuBaseProperties_h_
#define _CEGUIMenuBaseProperties_h_

#include "CEGUIProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of ItemListBaseProperties namespace section
namespace MenuBaseProperties
{

/*!
\brief
	Property to access the item spacing of the menu.

	\par Usage:
		- Name: ItemSpacing
		- Format: "[float]".

	\par Where:
		- [float] represents the item spacing of the menu.
*/
class ItemSpacing : public Property
{
public:
	ItemSpacing() : Property(
		"ItemSpacing",
		"Property to get/set the item spacing of the menu.  Value is a float.",
		"10.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the horz padding value of the menu.

	\par Usage:
		- Name: HorzPadding
		- Format: "[float]".

	\par Where:
		- [float] represents the horz padding of the slider.
*/
class HorzPadding : public Property
{
public:
	HorzPadding() : Property(
		"HorzPadding",
		"Property to get/set the horizontal padding of the menu.  Value is a float.",
		"3.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the vert padding value of the menu.

\par Usage:
	- Name: VertPadding
	- Format: "[float]".

\par Where:
	- [float] represents the vert padding of the slider.
*/
class VertPadding : public Property
{
public:
	VertPadding() : Property(
		"VertPadding",
		"Property to get/set the vertical padding of the menu.  Value is a float.",
		"3.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the border width of the menu.

\par Usage:
	- Name: BorderWidth
	- Format: "[float]".

\par Where:
	- [float] represents the border width of the slider.
*/
class BorderWidth : public Property
{
public:
	BorderWidth() : Property(
		"BorderWidth",
		"Property to get/set the border width of the menu.  Value is a float.",
		"5.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the background colours of the menu.

\par Usage:
	- Name: BackgroundColours
	- Format: "tl:aarrggbb tr:aarrggbb bl:aarrggbb br:aarrggbb".

\par Where:
	- aarrggbb is the ARGB colour value to be used for each corner of the ColourRect
*/
class BackgroundColours : public Property
{
public:
	BackgroundColours() : Property(
		"BackgroundColours",
		"Property to get/set the background colours of the menu.  Value is a ColourRect.",
		"tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
    Property to access the state of the allow multiple popups setting.

    \par Usage:
	    - Name: AllowMultiplePopups
	    - Format: "[text]".

    \par Where [Text] is:
	    - "True" to indicate that auto resizing is enabled.
	    - "False" to indicate that auto resizing is disabled.
*/
class AllowMultiplePopups : public Property
{
public:
    AllowMultiplePopups() : Property(
	    "AllowMultiplePopups",
	    "Property to get/set the state of the allow multiple popups setting for the menu.  Value is either \"True\" or \"False\".",
	    "False")
    {}

    String	get(const PropertyReceiver* receiver) const;
    void	set(PropertyReceiver* receiver, const String& value);
};

} // End of  MenuBaseProperties namespace section
} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIMenuBaseProperties_h_
