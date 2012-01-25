/************************************************************************
	filename: 	CEGUIMenuItemProperties.h
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
#ifndef _CEGUIMenuItemProperties_h_
#define _CEGUIMenuItemProperties_h_

#include "CEGUIProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of MenuItemProperties namespace section
namespace MenuItemProperties
{

/*!
\brief
	Property to access the hover colour of the item.

	\par Usage:
		- Name: HoverColour
		- Format: "[aarrggbb]".

	\par Where:
		- [aarrggbb] represents the hover colour of the item.
*/
class HoverColour : public Property
{
public:
	HoverColour() : Property(
		"HoverColour",
		"Property to get/set the hover colour of the item.  Value is a colour.",
		"00FFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the pushed colour of the item.

	\par Usage:
		- Name: PushedColour
		- Format: "[aarrggbb]".

	\par Where:
		- [aarrggbb] represents the pushed colour of the item.
*/
class PushedColour : public Property
{
public:
	PushedColour() : Property(
		"PushedColour",
		"Property to get/set the pushed colour of the item.  Value is a colour.",
		"00FFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the opened colour of the item.

	\par Usage:
		- Name: OpenedColour
		- Format: "[aarrggbb]".

	\par Where:
		- [aarrggbb] represents the opened colour of the item.
*/
class OpenedColour : public Property
{
public:
	OpenedColour() : Property(
		"OpenedColour",
		"Property to get/set the opened colour of the item.  Value is a colour.",
		"00EFEFEF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the normal text colour of the item.

	\par Usage:
		- Name: NormalTextColour
		- Format: "[aarrggbb]".

	\par Where:
		- [aarrggbb] represents the normal text colour of the item.
*/
class NormalTextColour : public Property
{
public:
	NormalTextColour() : Property(
		"NormalTextColour",
		"Property to get/set the normal text colour of the item.  Value is a colour.",
		"00FFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the disabled text colour of the item.

	\par Usage:
		- Name: DisabledTextColour
		- Format: "[aarrggbb]".

	\par Where:
		- [aarrggbb] represents the disabled text colour of the item.
*/
class DisabledTextColour : public Property
{
public:
	DisabledTextColour() : Property(
		"DisabledTextColour",
		"Property to get/set the disabled text colour of the item.  Value is a colour.",
		"007F7F7F")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


} // End of  MenuItemProperties namespace section
} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIMenuItemProperties_h_
