/************************************************************************
	filename: 	CEGUIPopupMenuProperties.h
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
#ifndef _CEGUIPopupMenuProperties_h_
#define _CEGUIPopupMenuProperties_h_

#include "CEGUIProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of ItemListBaseProperties namespace section
namespace PopupMenuProperties
{

/*!
\brief
	Property to access the fade in time in seconds of the menu bar.

	\par Usage:
		- Name: FadeInTime
		- Format: "[float]".

	\par Where:
		- [float] represents the fade in time in seconds of the menu bar.
*/
class FadeInTime : public Property
{
public:
	FadeInTime() : Property(
		"FadeInTime",
		"Property to get/set the fade in time in seconds of the menu bar.  Value is a float.",
		"0.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the fade out time in seconds of the menu bar.

	\par Usage:
		- Name: FadeOutTime
		- Format: "[float]".

	\par Where:
		- [float] represents the fade out time in seconds of the menu bar.
*/
class FadeOutTime : public Property
{
public:
	FadeOutTime() : Property(
		"FadeOutTime",
		"Property to get/set the fade out time in seconds of the menu bar.  Value is a float.",
		"0.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

} // End of  PopupMenuProperties namespace section
} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIPopupMenuProperties_h_
