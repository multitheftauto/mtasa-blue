/************************************************************************
	filename: 	CEGUIMultiColumnListProperties.cpp
	created:	11/7/2004
	author:		Paul D Turner
	
	purpose:	Implements multi-column list properties.
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
#include "elements/CEGUIMultiColumnListProperties.h"
#include "elements/CEGUIMultiColumnList.h"
#include "CEGUIPropertyHelper.h"
#include "CEGUILogger.h"

// Start of CEGUI namespace section
namespace CEGUI
{

// Start of MultiColumnListProperties namespace section
namespace MultiColumnListProperties
{
String	ColumnsSizable::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const MultiColumnList*>(receiver)->isUserColumnSizingEnabled());
}


void	ColumnsSizable::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiColumnList*>(receiver)->setUserColumnSizingEnabled(PropertyHelper::stringToBool(value));
}


String	ColumnsMovable::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const MultiColumnList*>(receiver)->isUserColumnDraggingEnabled());
}


void	ColumnsMovable::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiColumnList*>(receiver)->setUserColumnDraggingEnabled(PropertyHelper::stringToBool(value));
}


String	SortSettingEnabled::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const MultiColumnList*>(receiver)->isUserSortControlEnabled());
}


void	SortSettingEnabled::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiColumnList*>(receiver)->setUserSortControlEnabled(PropertyHelper::stringToBool(value));
}


String	SortColumnID::get(const PropertyReceiver* receiver) const
{
	const MultiColumnList* mcl = static_cast<const MultiColumnList*>(receiver);
	return PropertyHelper::uintToString(mcl->getColumnID(mcl->getSortColumn()));
}


void	SortColumnID::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiColumnList*>(receiver)->setSortColumnByID(PropertyHelper::stringToUint(value));
}


String	SortDirection::get(const PropertyReceiver* receiver) const
{
	switch(static_cast<const MultiColumnList*>(receiver)->getSortDirection())
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

	static_cast<MultiColumnList*>(receiver)->setSortDirection(dir);
}


String	NominatedSelectionColumnID::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<const MultiColumnList*>(receiver)->getNominatedSelectionColumnID());
}


void	NominatedSelectionColumnID::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiColumnList*>(receiver)->setNominatedSelectionColumn(PropertyHelper::stringToUint(value));
}


String	NominatedSelectionRow::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<const MultiColumnList*>(receiver)->getNominatedSelectionRow());
}


void	NominatedSelectionRow::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiColumnList*>(receiver)->setNominatedSelectionRow(PropertyHelper::stringToUint(value));
}


String	ForceVertScrollbar::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const MultiColumnList*>(receiver)->isVertScrollbarAlwaysShown());
}


void	ForceVertScrollbar::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiColumnList*>(receiver)->setShowVertScrollbar(PropertyHelper::stringToBool(value));
}


String	ForceHorzScrollbar::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const MultiColumnList*>(receiver)->isHorzScrollbarAlwaysShown());
}


void	ForceHorzScrollbar::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiColumnList*>(receiver)->setShowHorzScrollbar(PropertyHelper::stringToBool(value));
}


String	SelectionMode::get(const PropertyReceiver* receiver) const
{
	switch(static_cast<const MultiColumnList*>(receiver)->getSelectionMode())
	{
	case MultiColumnList::RowMultiple:
		return String((utf8*)"RowMultiple");
		break;

	case MultiColumnList::ColumnSingle:
		return String((utf8*)"ColumnSingle");
		break;

	case MultiColumnList::ColumnMultiple:
		return String((utf8*)"ColumnMultiple");
		break;

	case MultiColumnList::CellSingle:
		return String((utf8*)"CellSingle");
		break;

	case MultiColumnList::CellMultiple:
		return String((utf8*)"CellMultiple");
		break;

	case MultiColumnList::NominatedColumnSingle:
		return String((utf8*)"NominatedColumnSingle");
		break;

	case MultiColumnList::NominatedColumnMultiple:
		return String((utf8*)"NominatedColumnMultiple");
		break;

	case MultiColumnList::NominatedRowSingle:
		return String((utf8*)"NominatedRowSingle");
		break;

	case MultiColumnList::NominatedRowMultiple:
		return String((utf8*)"NominatedRowMultiple");
		break;

	default:
		return String((utf8*)"RowSingle");
		break;
	}
}


void	SelectionMode::set(PropertyReceiver* receiver, const String& value)
{
	MultiColumnList::SelectionMode mode;

	if (value == (utf8*)"RowMultiple")
	{
		mode = MultiColumnList::RowMultiple;
	}
	else if (value == (utf8*)"ColumnSingle")
	{
		mode = MultiColumnList::ColumnSingle;
	}
	else if (value == (utf8*)"ColumnMultiple")
	{
		mode = MultiColumnList::ColumnMultiple;
	}
	else if (value == (utf8*)"CellSingle")
	{
		mode = MultiColumnList::CellSingle;
	}
	else if (value == (utf8*)"CellMultiple")
	{
		mode = MultiColumnList::CellMultiple;
	}
	else if (value == (utf8*)"NominatedColumnSingle")
	{
		mode = MultiColumnList::NominatedColumnSingle;
	}
	else if (value == (utf8*)"NominatedColumnMultiple")
	{
		mode = MultiColumnList::NominatedColumnMultiple;
	}
	else if (value == (utf8*)"NominatedRowSingle")
	{
		mode = MultiColumnList::NominatedRowSingle;
	}
	else if (value == (utf8*)"NominatedRowMultiple")
	{
		mode = MultiColumnList::NominatedRowMultiple;
	}
	else
	{
		mode = MultiColumnList::RowSingle;
	}

	static_cast<MultiColumnList*>(receiver)->setSelectionMode(mode);
}


String ColumnHeader::get(const PropertyReceiver* receiver) const
{
	return String("");
}


void ColumnHeader::set(PropertyReceiver* receiver, const String& value)
{
	// extract data from the value string

	size_t wstart = value.find("width:");
	size_t idstart = value.find("id:");

	String caption(value.substr(0, wstart));
	caption = caption.substr(caption.find_first_of(":") + 1);

	String width(value.substr(wstart, idstart));
	width = width.substr(width.find_first_of(":") + 1);

	String id(value.substr(idstart));
	id = id.substr(id.find_first_of(":") + 1);

	static_cast<MultiColumnList*>(receiver)->addColumn(
		caption, PropertyHelper::stringToUint(id), PropertyHelper::stringToFloat(width));
}


String RowCount::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<const MultiColumnList*>(receiver)->getRowCount());
}


void RowCount::set(PropertyReceiver* receiver, const String& value)
{
	// property is read only.
	Logger::getSingleton().logEvent(
		"Attempt to set read only property 'RowCount' on MultiColumnListbox '" + 
		static_cast<const MultiColumnList*>(receiver)->getName() + "'.", Errors);
}


} // End of  MultiColumnListProperties namespace section

} // End of  CEGUI namespace section
