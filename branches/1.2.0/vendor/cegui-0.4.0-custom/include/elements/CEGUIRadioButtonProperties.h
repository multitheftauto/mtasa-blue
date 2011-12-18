/************************************************************************
	filename: 	CEGUIRadioButtonProperties.h
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Interface to properties for RadioButton class
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
#ifndef _CEGUIRadioButtonProperties_h_
#define _CEGUIRadioButtonProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of RadioButtonProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the RadioButton class
*/
namespace RadioButtonProperties
{
/*!
\brief
	Property to access the selected state of the RadioButton.

	\par Usage:
		- Name: Selected
		- Format: "[text]".

	\par Where [Text] is:
		- "True" to indicate the radio button is selected.
		- "False" to indicate the radio button is not selected.
*/
class Selected : public Property
{
public:
	Selected() : Property(
		"Selected",
		"Property to get/set the selected state of the RadioButton.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the radio button group ID.

	\par Usage:
		- Name: GroupID
		- Format: "[uint]".

	\par Where:
		- [uint] is any unsigned integer value.
*/
class GroupID : public Property
{
public:
	GroupID() : Property(
		"GroupID",
		"Property to get/set the radio button group ID.  Value is an unsigned integer number.",
		"0")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


} // End of  RadioButtonProperties namespace section

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIRadioButtonProperties_h_
