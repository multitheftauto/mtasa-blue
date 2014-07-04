/************************************************************************
	filename: 	CEGUIThumbProperties.h
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Interface for Thumb properties
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
#ifndef _CEGUIThumbProperties_h_
#define _CEGUIThumbProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of ThumbProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the Thumb class
*/
namespace ThumbProperties
{
/*!
\brief
	Property to access the state of the "hot-tracked" setting for the thumb.

	\par Usage:
		- Name: HotTracked
		- Format: "[text]".

	\par Where [Text] is:
		- "True" to indicate the thumb will send notifications as it is dragged.
		- "False" to indicate the thumb will only notify once, when it is released.
*/
class HotTracked : public Property
{
public:
	HotTracked() : Property(
		"HotTracked",
		"Property to get/set the state of the state of the 'hot-tracked' setting for the thumb.  Value is either \"True\" or \"False\".",
		"True")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the state the setting to free the thumb vertically.

	\par Usage:
		- Name: VertFree
		- Format: "[text]".

	\par Where [Text] is:
		- "True" to indicate the thumb will be free (movable) vertically.
		- "False" to indicate the thumb will be fixed vertically.
*/
class VertFree : public Property
{
public:
	VertFree() : Property(
		"VertFree",
		"Property to get/set the state the setting to free the thumb vertically.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the state the setting to free the thumb horizontally.

	\par Usage:
		- Name: HorzFree
		- Format: "[text]".

	\par Where [Text] is:
		- "True" to indicate the thumb will be free (movable) horizontally.
		- "False" to indicate the thumb will be fixed horizontally.
*/
class HorzFree : public Property
{
public:
	HorzFree() : Property(
		"HorzFree", 
		"Property to get/set the state the setting to free the thumb horizontally.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the vertical movement range for the thumb.

	\par Usage:
		- Name: VertRange
		- Format: "min:[float] max:[float]".

	\par Where:
		- min:[float] specifies the minimum vertical setting (position) for the thumb, specified using the active metrics system for the window.
		- max:[float] specifies the maximum vertical setting (position) for the thumb, specified using the active metrics system for the window.
*/
class VertRange : public Property
{
public:
	VertRange() : Property(
		"VertRange",
		"Property to get/set the vertical movement range for the thumb.  Value is \"min:[float] max:[float]\".",
		"min:0.000000 max:1.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the horizontal movement range for the thumb.

	\par Usage:
		- Name: HorzRange
		- Format: "min:[float] max:[float]".

	\par Where:
		- min:[float] specifies the minimum horizontal setting (position) for the thumb, specified using the active metrics system for the window.
		- max:[float] specifies the maximum horizontal setting (position) for the thumb, specified using the active metrics system for the window.
*/
class HorzRange : public Property
{
public:
	HorzRange() : Property(
		"HorzRange",
		"Property to get/set the horizontal movement range for the thumb.  Value is \"min:[float] max:[float]\".",
		"min:0.000000 max:1.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


} // End of  ThumbProperties namespace section

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIThumbProperties_h_
