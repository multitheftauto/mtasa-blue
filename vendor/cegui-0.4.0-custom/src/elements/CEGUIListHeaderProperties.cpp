/************************************************************************
	filename: 	CEGUIListHeaderProperties.cpp
	created:	11/7/2004
	author:		Paul D Turner
	
	purpose:	Implements the properties for the ListHeader class
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
#include "StdInc.h"
#include "elements/CEGUIListHeaderProperties.h"
#include "elements/CEGUIListHeader.h"
#include "CEGUIPropertyHelper.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of ListHeaderProperties namespace section
namespace ListHeaderProperties
{
String	SortSettingEnabled::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const ListHeader*>(receiver)->isSortingEnabled());
}


void	SortSettingEnabled::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<ListHeader*>(receiver)->setSortingEnabled(PropertyHelper::stringToBool(value));
}


String	ColumnsSizable::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const ListHeader*>(receiver)->isColumnSizingEnabled());
}


void	ColumnsSizable::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<ListHeader*>(receiver)->setColumnSizingEnabled(PropertyHelper::stringToBool(value));
}


String	ColumnsMovable::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const ListHeader*>(receiver)->isColumnDraggingEnabled());
}


void	ColumnsMovable::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<ListHeader*>(receiver)->setColumnDraggingEnabled(PropertyHelper::stringToBool(value));
}


String	SortColumnID::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<const ListHeader*>(receiver)->getSortSegment().getID());
}


void	SortColumnID::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<ListHeader*>(receiver)->setSortColumnFromID(PropertyHelper::stringToUint(value));
}


String	SortDirection::get(const PropertyReceiver* receiver) const
{
	switch(static_cast<const ListHeader*>(receiver)->getSortDirection())
	{
	case ListHeaderSegment::Ascending:
		return String((utf8*)"Ascending");
		break;

	case ListHeaderSegment::Descending:
		return String((utf8*)"Descending");
		break;

	default:
		return String((utf8*)"None");
		break;
	}

}


void	SortDirection::set(PropertyReceiver* receiver, const String& value)
{
	ListHeaderSegment::SortDirection dir;

	if (value == (utf8*)"Ascending")
	{
		dir = ListHeaderSegment::Ascending;
	}
	else if (value == (utf8*)"Descending")
	{
		dir = ListHeaderSegment::Descending;
	}
	else
	{
		dir = ListHeaderSegment::None;
	}

	static_cast<ListHeader*>(receiver)->setSortDirection(dir);
}

} // End of  ListHeaderProperties namespace section

} // End of  CEGUI namespace section
