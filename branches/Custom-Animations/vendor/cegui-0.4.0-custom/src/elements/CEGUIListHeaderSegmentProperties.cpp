/************************************************************************
	filename: 	CEGUIListHeaderSegmentProperties.cpp
	created:	11/7/2004
	author:		Paul D Turner
	
	purpose:	Implements properties for the header segment class
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
#include "elements/CEGUIListHeaderSegmentProperties.h"
#include "elements/CEGUIListHeaderSegment.h"
#include "CEGUIPropertyHelper.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of ListHeaderSegmentProperties namespace section
namespace ListHeaderSegmentProperties
{
String	Sizable::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const ListHeaderSegment*>(receiver)->isSizingEnabled());
}


void	Sizable::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<ListHeaderSegment*>(receiver)->setSizingEnabled(PropertyHelper::stringToBool(value));
}


String	Clickable::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const ListHeaderSegment*>(receiver)->isClickable());
}


void	Clickable::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<ListHeaderSegment*>(receiver)->setClickable(PropertyHelper::stringToBool(value));
}


String	Dragable::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const ListHeaderSegment*>(receiver)->isDragMovingEnabled());
}


void	Dragable::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<ListHeaderSegment*>(receiver)->setDragMovingEnabled(PropertyHelper::stringToBool(value));
}


String	SortDirection::get(const PropertyReceiver* receiver) const
{
	switch(static_cast<const ListHeaderSegment*>(receiver)->getSortDirection())
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

	static_cast<ListHeaderSegment*>(receiver)->setSortDirection(dir);
}


} // End of  ListHeaderSegmentProperties namespace section

} // End of  CEGUI namespace section
