/************************************************************************
	filename: 	CEGUIListboxProperties.h
	created:	11/7/2004
	author:		Paul D Turner
	
	purpose:	Interface to Listbox property classes
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
#ifndef _CEGUIListboxProperties_h_
#define _CEGUIListboxProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of ListboxProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the Listbox class
*/
namespace ListboxProperties
{
/*!
\brief
	Property to access the sort setting of the list box.

	\par Usage:
		- Name: Sort
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate the list items should be sorted.
		- "False" to indicate the list items should not be sorted.
*/
class Sort : public Property
{
public:
	Sort() : Property(
		"Sort",
		"Property to get/set the sort setting of the list box.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the multi-select setting of the list box.

	\par Usage:
		- Name: MultiSelect
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate that multiple items may be selected.
		- "False" to indicate that only a single item may be selected.
*/
class MultiSelect : public Property
{
public:
	MultiSelect() : Property(
		"MultiSelect",
		"Property to get/set the multi-select setting of the list box.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the 'always show' setting for the vertical scroll bar of the list box.

	\par Usage:
		- Name: ForceVertScrollbar
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate that the vertical scroll bar will always be shown.
		- "False" to indicate that the vertical scroll bar will only be shown when it is needed.
*/
class ForceVertScrollbar : public Property
{
public:
	ForceVertScrollbar() : Property(
		"ForceVertScrollbar",
		"Property to get/set the 'always show' setting for the vertical scroll bar of the list box.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the 'always show' setting for the horizontal scroll bar of the list box.

	\par Usage:
		- Name: ForceHorzScrollbar
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate that the horizontal scroll bar will always be shown.
		- "False" to indicate that the horizontal scroll bar will only be shown when it is needed.
*/
class ForceHorzScrollbar : public Property
{
public:
	ForceHorzScrollbar() : Property(
		"ForceHorzScrollbar",
		"Property to get/set the 'always show' setting for the horizontal scroll bar of the list box.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
Property to access the show item tooltips setting of the list box.

\par Usage:
- Name: ItemTooltips
- Format: "[text]"

\par Where [Text] is:
- "True" to indicate that the tooltip of the list box will be set by the item below the mouse pointer
- "False" to indicate that the list box has a static tooltip.
*/
class ItemTooltips : public Property
{
public:
	ItemTooltips() : Property(
		"ItemTooltips",
		"Property to access the show item tooltips setting of the list box.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

} // End of  ListboxProperties namespace section

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIListboxProperties_h_
