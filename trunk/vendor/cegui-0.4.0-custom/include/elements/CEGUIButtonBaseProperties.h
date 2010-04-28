/************************************************************************
	filename: 	CEGUIButtonBaseProperties.h
	created:	9/7/2004
	author:		Paul D Turner
	
	purpose:	Interface to properties for button base class
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
#ifndef _CEGUIButtonBaseProperties_h_
#define _CEGUIButtonBaseProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{
// Start of ButtonBaseProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the ButtonBase class
*/
namespace ButtonBaseProperties
{
/*!
\brief
	Property to access the normal text colour.

	This property offers access to the colour value to be used for rendering the button caption text for the normal state.

	\par Usage:
		- Name: NormalTextColour
		- Format: "aarrggbb".

	\par Where:
		- aarrggbb is the ARGB colour value to be used.
*/
class NormalTextColour : public Property
{
public:
	NormalTextColour() : Property(
		"NormalTextColour",
		"Property to get/set the colour to use when rendering label text for normal state.  Value is \"aarrggbb\" (hex).",
		"FFFFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the hover / highlight text colour.

	This property offers access to the colour value to be used for rendering the button caption text for the hover or highlight state.

	\par Usage:
		- Name: HoverTextColour
		- Format: "aarrggbb".

	\par Where:
		- aarrggbb is the ARGB colour value to be used.
*/
class HoverTextColour : public Property
{
public:
	HoverTextColour() : Property(
		"HoverTextColour",
		"Property to get/set the colour to use when rendering label text for hover/highlight state.  Value is \"aarrggbb\" (hex).",
		"FFFFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the pushed text colour.

	This property offers access to the colour value to be used for rendering the button caption text for the pushed state.

	\par Usage:
		- Name: PushedTextColour
		- Format: "aarrggbb".

	\par Where:
		- aarrggbb is the ARGB colour value to be used.
*/
class PushedTextColour : public Property
{
public:
	PushedTextColour() : Property(
		"PushedTextColour", 
		"Property to get/set the colour to use when rendering label text for pushed state.  Value is \"aarrggbb\" (hex).",
		"FFFFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the disabled text colour.

	This property offers access to the colour value to be used for rendering the button caption text for the disabled state.

	\par Usage:
		- Name: DisabledTextColour
		- Format: "aarrggbb".

	\par Where:
		- aarrggbb is the ARGB colour value to be used.
*/
class DisabledTextColour : public Property
{
public:
	DisabledTextColour() : Property(
		"DisabledTextColour",
		"Property to get/set the colour to use when rendering label text for disabled state.  Value is \"aarrggbb\" (hex).",
		"FF7F7F7F")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


} // End of  ButtonBaseProperties namespace section


} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIButtonBaseProperties_h_
