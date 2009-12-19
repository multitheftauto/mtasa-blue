/************************************************************************
	filename: 	CEGUIListHeaderProperties.h
	created:	11/7/2004
	author:		Paul D Turner
	
	purpose:	Interface to the properties for the ListHeader class.
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
#ifndef _CEGUIListHeaderProperties_h_
#define _CEGUIListHeaderProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of ListHeaderProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the ListHeader class
*/
namespace ListHeaderProperties
{
/*!
\brief
	Property to access the setting for user sizing of the column headers.

	\par Usage:
		- Name: ColumnsSizable
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate the column headers can be sized by the user.
		- "False" to indicate the column headers can not be sized by the user.
*/
class ColumnsSizable : public Property
{
public:
	ColumnsSizable() : Property(
		"ColumnsSizable",
		"Property to get/set the setting for user sizing of the column headers.  Value is either \"True\" or \"False\".",
		"True")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the setting for user moving of the column headers.

	\par Usage:
		- Name: ColumnsMovable
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate the column headers can be moved by the user.
		- "False" to indicate the column headers can not be moved by the user.
*/
class ColumnsMovable : public Property
{
public:
	ColumnsMovable() : Property(
		"ColumnsMovable",
		"Property to get/set the setting for user moving of the column headers.  Value is either \"True\" or \"False\".",
		"True")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the setting for user modification of the sort column & direction.

	\par Usage:
		- Name: SortSettingEnabled
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate the user may modify the sort column and direction by clicking the header segments.
		- "False" to indicate the user may not modify the sort column or direction.
*/
class SortSettingEnabled : public Property
{
public:
	SortSettingEnabled() : Property(
		"SortSettingEnabled",
		"Property to get/set the setting for for user modification of the sort column & direction.  Value is either \"True\" or \"False\".",
		"True")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the sort direction setting of the list header.

	\par Usage:
		- Name: SortDirection
		- Format: "[text]"

	\par Where [Text] is one of:
		- "Ascending"
		- "Descending"
		- "None"
*/
class SortDirection : public Property
{
public:
	SortDirection() : Property(
		"SortDirection",
		"Property to get/set the sort direction setting of the header.  Value is the text of one of the SortDirection enumerated value names.",
		"None")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the current sort column (via ID code).

	\par Usage:
		- Name: SortColumnID
		- Format: "[uint]".

	\par Where:
		- [uint] is any unsigned integer value.
*/
class SortColumnID : public Property
{
public:
	SortColumnID() : Property(
		"SortColumnID",
		"Property to get/set the current sort column (via ID code).  Value is an unsigned integer number.",
		"0")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

} // End of  ListHeaderProperties namespace section

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIListHeaderProperties_h_
