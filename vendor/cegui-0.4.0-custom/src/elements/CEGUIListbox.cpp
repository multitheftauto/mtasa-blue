/************************************************************************
	filename: 	CEGUIListbox.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Listbox widget base class
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
#include "CEGUIExceptions.h"
#include "CEGUIWindowManager.h"
#include "elements/CEGUIListbox.h"
#include "elements/CEGUIListboxItem.h"
#include "elements/CEGUIScrollbar.h"
#include "elements/CEGUITooltip.h"

#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
const String Listbox::EventNamespace("Listbox");

/*************************************************************************
	Definition of Properties for this class
*************************************************************************/
ListboxProperties::Sort					Listbox::d_sortProperty;
ListboxProperties::MultiSelect			Listbox::d_multiSelectProperty;
ListboxProperties::ForceVertScrollbar	Listbox::d_forceVertProperty;
ListboxProperties::ForceHorzScrollbar	Listbox::d_forceHorzProperty;
ListboxProperties::ItemTooltips			Listbox::d_itemTooltipsProperty;


/*************************************************************************
	Constants
*************************************************************************/
// event names
const String Listbox::EventListContentsChanged( (utf8*)"ListItemsChanged" );
const String Listbox::EventSelectionChanged( (utf8*)"ItemSelectionChanged" );
const String Listbox::EventSortModeChanged( (utf8*)"SortModeChanged" );
const String Listbox::EventMultiselectModeChanged( (utf8*)"MuliselectModeChanged" );
const String Listbox::EventVertScrollbarModeChanged( (utf8*)"VertScrollModeChanged" );
const String Listbox::EventHorzScrollbarModeChanged( (utf8*)"HorzScrollModeChanged" );

	
/*************************************************************************
	Constructor for Listbox base class.
*************************************************************************/
Listbox::Listbox(const String& type, const String& name)
	: Window(type, name),
	d_sorted(false),
	d_multiselect(false),
	d_forceVertScroll(false),
	d_forceHorzScroll(false),
	d_itemTooltips(false),
	d_lastSelected(NULL)
{
	// add new events specific to list box.
	addListboxEvents();

	addListboxProperties();
}


/*************************************************************************
	Destructor for Listbox base class.
*************************************************************************/
Listbox::~Listbox(void)
{
	resetList_impl();
}


/*************************************************************************
	Initialise the Window based object ready for use.
*************************************************************************/
void Listbox::initialise(void)
{
	// create the component sub-widgets
	d_vertScrollbar = createVertScrollbar(getName() + "__auto_vscrollbar__");
	d_horzScrollbar = createHorzScrollbar(getName() + "__auto_hscrollbar__");

	addChildWindow(d_vertScrollbar);
	addChildWindow(d_horzScrollbar);

    d_vertScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&Listbox::handle_scrollChange, this));
    d_horzScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&Listbox::handle_scrollChange, this));

	configureScrollbars();
	performChildWindowLayout();
}


/*************************************************************************
	Return the number of selected items in the list box.	
*************************************************************************/
size_t Listbox::getSelectedCount(void) const
{
	size_t count = 0;

	for (size_t index = 0; index < d_listItems.size(); ++index)
	{
		if (d_listItems[index]->isSelected())
		{
			count++;
		}

	}

	return count;
}


/*************************************************************************
	Return a pointer to the first selected item.
*************************************************************************/
ListboxItem* Listbox::getFirstSelectedItem(void) const
{
	return getNextSelected(NULL);
}


/*************************************************************************
	Return a pointer to the next selected item after item 'start_item'
*************************************************************************/
ListboxItem* Listbox::getNextSelected(const ListboxItem* start_item) const
{
	// if start_item is NULL begin search at begining, else start at item after start_item
	size_t index = (start_item == NULL) ? 0 : (getItemIndex(start_item) + 1);

	while (index < d_listItems.size())
	{
		// return pointer to this item if it's selected.
		if (d_listItems[index]->isSelected())
		{
			return d_listItems[index];
		}
		// not selected, advance to next
		else
		{
			index++;
		}

	}

	// no more selected items.
	return NULL;
}


/*************************************************************************
	Return the item at index position 'index'.
*************************************************************************/
ListboxItem* Listbox::getListboxItemFromIndex(size_t index) const
{
	if (index < d_listItems.size())
	{
		return d_listItems[index];
	}
	else
	{
		throw InvalidRequestException((utf8*)"Listbox::getListboxItemFromIndex - the specified index is out of range for this Listbox.");
	}
}


/*************************************************************************
	Return the index of ListboxItem \a item
*************************************************************************/
size_t Listbox::getItemIndex(const ListboxItem* item) const
{
	LBItemList::const_iterator pos = std::find(d_listItems.begin(), d_listItems.end(), item);

	if (pos != d_listItems.end())
	{
		return std::distance(d_listItems.begin(), pos);
	}
	else
	{
		throw InvalidRequestException((utf8*)"Listbox::getItemIndex - the specified ListboxItem is not attached to this Listbox.");
	}

}


/*************************************************************************
	return whether the string at index position \a index is selected
*************************************************************************/
bool Listbox::isItemSelected(size_t index) const
{
	if (index < d_listItems.size())
	{
		return d_listItems[index]->isSelected();
	}
	else
	{
		throw InvalidRequestException((utf8*)"Listbox::isItemSelected - the specified index is out of range for this Listbox.");
	}

}


/*************************************************************************
	Search the list for an item with the specified text
*************************************************************************/
ListboxItem* Listbox::findItemWithText(const String& text, const ListboxItem* start_item)
{
	// if start_item is NULL begin search at begining, else start at item after start_item
	size_t index = (start_item == NULL) ? 0 : (getItemIndex(start_item) + 1);

	while (index < d_listItems.size())
	{
		// return pointer to this item if it's text matches
		if (d_listItems[index]->getText() == text)
		{
			return d_listItems[index];
		}
		// no matching text, advance to next item
		else
		{
			index++;
		}

	}

	// no items matched.
	return NULL;
}


/*************************************************************************
	Return whether the specified ListboxItem is in the List
*************************************************************************/
bool Listbox::isListboxItemInList(const ListboxItem* item) const
{
	return std::find(d_listItems.begin(), d_listItems.end(), item) != d_listItems.end();
}



/*************************************************************************
	Remove all items from the list.
*************************************************************************/
void Listbox::resetList(void)
{
	if (resetList_impl())
	{
		WindowEventArgs args(this);
		onListContentsChanged(args);
	}

}


/*************************************************************************
	Add the given ListboxItem to the list.
*************************************************************************/
void Listbox::addItem(ListboxItem* item)
{
	if (item != NULL)
	{
		// establish ownership
		item->setOwnerWindow(this);

		// if sorting is enabled, re-sort the list
		if (isSortEnabled())
		{
			d_listItems.insert(std::upper_bound(d_listItems.begin(), d_listItems.end(), item, &lbi_less), item);
		
		}
		// not sorted, just stick it on the end.
		else
		{
			d_listItems.push_back(item);
		}

		WindowEventArgs args(this);
		onListContentsChanged(args);
	}

}


/*************************************************************************
	Insert an item into the list box after a specified item already in
	the list.
*************************************************************************/
void Listbox::insertItem(ListboxItem* item, const ListboxItem* position)
{
	// if the list is sorted, it's the same as a normal add operation
	if (isSortEnabled())
	{
		addItem(item);
	}
	else if (item != NULL)
	{
		// establish ownership
		item->setOwnerWindow(this);

		// if position is NULL begin insert at begining, else insert after item 'position'
		LBItemList::iterator ins_pos;

		if (position == NULL)
		{
			ins_pos = d_listItems.begin();
		}
		else
		{
			ins_pos = std::find(d_listItems.begin(), d_listItems.end(), position);

			// throw if item 'position' is not in the list
			if (ins_pos == d_listItems.end())
			{
				throw InvalidRequestException((utf8*)"Listbox::insertItem - the specified ListboxItem for parameter 'position' is not attached to this Listbox.");
			}

		}
		
		d_listItems.insert(ins_pos, item);

		WindowEventArgs args(this);
		onListContentsChanged(args);
	}

}


/*************************************************************************
	Removes the given item from the list box.
*************************************************************************/
void Listbox::removeItem(const ListboxItem* item)
{
	if (item != NULL)
	{
		LBItemList::iterator pos = std::find(d_listItems.begin(), d_listItems.end(), item);

		// if item is in the list
		if (pos != d_listItems.end())
		{
			// disown item
			(*pos)->setOwnerWindow(NULL);

			// remove item
			d_listItems.erase(pos);

			// if item was the last selected item, reset that to NULL
			if (item == d_lastSelected)
			{
				d_lastSelected = NULL;
			}

			// if item is supposed to be deleted by us
			if (item->isAutoDeleted())
			{
				// clean up this item.
				delete item;
			}

			WindowEventArgs args(this);
			onListContentsChanged(args);
		}

	}
	
}


/*************************************************************************
	Clear the selected state for all items.
*************************************************************************/
void Listbox::clearAllSelections(void)
{
	// only fire events and update if we actually made any changes
	if (clearAllSelections_impl())
	{
		WindowEventArgs args(this);
		onSelectionChanged(args);
	}

}


/*************************************************************************
	Set whether the list should be sorted.
*************************************************************************/
void Listbox::setSortingEnabled(bool setting)
{
	// only react if the setting will change
	if (d_sorted != setting)
	{
		d_sorted = setting;

		// if we are enabling sorting, we need to sort the list
		if (d_sorted)
		{
			std::sort(d_listItems.begin(), d_listItems.end(), &lbi_greater);
		}

        WindowEventArgs args(this);
		onSortModeChanged(args);
	}

}


/*************************************************************************
	Set whether the list should allow multiple selections or just a
	single selection
*************************************************************************/
void Listbox::setMultiselectEnabled(bool setting)
{
	// only react if the setting is changed
	if (d_multiselect != setting)
	{
		d_multiselect = setting;

		// if we change to single-select, deselect all except the first selected item.
        WindowEventArgs args(this);
		if ((!d_multiselect) && (getSelectedCount() > 1))
		{
			ListboxItem* itm = getFirstSelectedItem();

			while ((itm = getNextSelected(itm)))
			{
				itm->setSelected(false);
			}

			onSelectionChanged(args);

		}

		onMultiselectModeChanged(args);
	}

}

void Listbox::setItemTooltipsEnabled(bool setting)
{
	d_itemTooltips = setting;
}



/*************************************************************************
	Set whether the vertical scroll bar should always be shown.
*************************************************************************/
void Listbox::setShowVertScrollbar(bool setting)
{
	if (d_forceVertScroll != setting)
	{
		d_forceVertScroll = setting;

		configureScrollbars();
		WindowEventArgs args(this);
		onVertScrollbarModeChanged(args);
	}

}


/*************************************************************************
	Set whether the horizontal scroll bar should always be shown.
*************************************************************************/
void Listbox::setShowHorzScrollbar(bool setting)
{
	if (d_forceHorzScroll != setting)
	{
		d_forceHorzScroll = setting;

		configureScrollbars();
		WindowEventArgs args(this);
		onHorzScrollbarModeChanged(args);
	}

}


/*************************************************************************
	Set the select state of an attached ListboxItem.
*************************************************************************/
void Listbox::setItemSelectState(ListboxItem* item, bool state)
{
	LBItemList::iterator pos = std::find(d_listItems.begin(), d_listItems.end(), item);

	if (pos != d_listItems.end())
	{
		setItemSelectState(std::distance(d_listItems.begin(), pos), state);
	}
	else
	{
		throw InvalidRequestException((utf8*)"Listbox::setItemSelectState - the specified ListboxItem is not attached to this Listbox.");
	}
}


/*************************************************************************
	Set the select state of an attached ListboxItem.	
*************************************************************************/
void Listbox::setItemSelectState(size_t item_index, bool state)
{
	if (item_index < getItemCount())
	{
		// only do this if the setting is changing
		if (d_listItems[item_index]->isSelected() != state)
		{
			// conditions apply for single-select mode
			if (state && !d_multiselect)
			{
				clearAllSelections_impl();
			}

			d_listItems[item_index]->setSelected(state);
            WindowEventArgs args(this);
			onSelectionChanged(args);
		}

	}
	else
	{
		throw InvalidRequestException((utf8*)"Listbox::setItemSelectState - the value passed in the 'item_index' parameter is out of range for this Listbox.");
	}

}


/*************************************************************************
	Causes the list box to update it's internal state after changes have
	been made to one or more attached ListboxItem objects.	
*************************************************************************/
void Listbox::handleUpdatedItemData(void)
{
	configureScrollbars();
	requestRedraw();
}


/*************************************************************************
	Perform the actual rendering for this Window.
*************************************************************************/
void Listbox::populateRenderCache()
{
    // get the derived class to render general stuff before we handle the items
    cacheListboxBaseImagery();

    //
    // Render list items
    //
    Vector3	itemPos;
    Size	itemSize;
    Rect	itemClipper, itemRect;
    float	widest = getWidestItemWidth();

    // calculate position of area we have to render into
    Rect itemsArea(getListRenderArea());

    // set up some initial positional details for items
    itemPos.d_x = itemsArea.d_left - d_horzScrollbar->getScrollPosition();
    itemPos.d_y = itemsArea.d_top - d_vertScrollbar->getScrollPosition();
    itemPos.d_z = System::getSingleton().getRenderer()->getZLayer(3) - System::getSingleton().getRenderer()->getCurrentZ();

    float alpha = getEffectiveAlpha();

    // loop through the items
    size_t itemCount = getItemCount();

    for (size_t i = 0; i < itemCount; ++i)
    {
        itemSize.d_height = d_listItems[i]->getPixelSize().d_height;

        // allow item to have full width of box if this is wider than items
        itemSize.d_width = ceguimax(itemsArea.getWidth(), widest);

        // calculate destination area for this item.
        itemRect.d_left	= itemPos.d_x;
        itemRect.d_top	= itemPos.d_y;
        itemRect.setSize(itemSize);
        itemClipper = itemRect.getIntersection(itemsArea);

        // skip this item if totally clipped
        if (itemClipper.getWidth() == 0)
        {
            itemPos.d_y += itemSize.d_height;
            continue;
        }

        // draw this item
        d_listItems[i]->draw(d_renderCache, itemRect, itemPos.d_z, alpha, &itemClipper);

        // update position ready for next item
        itemPos.d_y += itemSize.d_height;
    }

}


/*************************************************************************
	display required integrated scroll bars according to current state
	of the list box and update their values.
*************************************************************************/
void Listbox::configureScrollbars(void)
{
    Scrollbar* vertScrollbar;
    Scrollbar* horzScrollbar;

    try
    {
        vertScrollbar = static_cast<Scrollbar*>(WindowManager::getSingleton().getWindow(getName() + "__auto_vscrollbar__"));
        horzScrollbar = static_cast<Scrollbar*>(WindowManager::getSingleton().getWindow(getName() + "__auto_hscrollbar__"));
    }
    catch (UnknownObjectException)
    {
        // no scrollbars?  Can't configure then!
        return;
    }

	float totalHeight	= getTotalItemsHeight();
	float widestItem	= getWidestItemWidth();

	//
	// First show or hide the scroll bars as needed (or requested)
	//
	// show or hide vertical scroll bar as required (or as specified by option)
	if ((totalHeight > getListRenderArea().getHeight()) || d_forceVertScroll)
	{
		vertScrollbar->show();

		// show or hide horizontal scroll bar as required (or as specified by option)
		if ((widestItem > getListRenderArea().getWidth()) || d_forceHorzScroll)
		{
			horzScrollbar->show();
		}
		else
		{
			horzScrollbar->hide();
		}

	}
	else
	{
		// show or hide horizontal scroll bar as required (or as specified by option)
		if ((widestItem > getListRenderArea().getWidth()) || d_forceHorzScroll)
		{
			horzScrollbar->show();

			// show or hide vertical scroll bar as required (or as specified by option)
			if ((totalHeight > getListRenderArea().getHeight()) || d_forceVertScroll)
			{
				vertScrollbar->show();
			}
			else
			{
				vertScrollbar->hide();
			}

		}
		else
		{
			vertScrollbar->hide();
			horzScrollbar->hide();
		}

	}

	//
	// Set up scroll bar values
	//
	Rect renderArea(getListRenderArea());

	vertScrollbar->setDocumentSize(totalHeight);
	vertScrollbar->setPageSize(renderArea.getHeight());
	vertScrollbar->setStepSize(ceguimax(1.0f, renderArea.getHeight() / 10.0f));
	vertScrollbar->setScrollPosition(vertScrollbar->getScrollPosition());

	horzScrollbar->setDocumentSize(widestItem);
	horzScrollbar->setPageSize(renderArea.getWidth());
	horzScrollbar->setStepSize(ceguimax(1.0f, renderArea.getWidth() / 10.0f));
	horzScrollbar->setScrollPosition(horzScrollbar->getScrollPosition());
}


/*************************************************************************
	select all strings between positions 'start' and 'end' (inclusive)
*************************************************************************/
void Listbox::selectRange(size_t start, size_t end)
{
	// only continue if list has some items
	if (d_listItems.size() > 0)
	{
		// if start is out of range, start at begining.
		if (start > d_listItems.size())
		{
			start = 0;
		}

		// if end is out of range end at the last item.
		if (end >= d_listItems.size())
		{
			end = d_listItems.size() - 1;
		}

		// ensure start becomes before the end.
		if (start > end)
		{
			size_t tmp;
			tmp = start;
			start = end;
			end = tmp;
		}

		// perform selections
		for( ; start <= end; ++start)
		{
			d_listItems[start]->setSelected(true);
		}

	}

}


/*************************************************************************
	Return the sum of all item heights	
*************************************************************************/
float Listbox::getTotalItemsHeight(void) const
{
	float height = 0;

	for (size_t i = 0; i < getItemCount(); ++i)
	{
		height += d_listItems[i]->getPixelSize().d_height;
	}

	return height;
}


/*************************************************************************
	Return the width of the widest item
*************************************************************************/
float Listbox::getWidestItemWidth(void) const
{
	float widest = 0;

	for (size_t i = 0; i < getItemCount(); ++i)
	{
		float thisWidth = d_listItems[i]->getPixelSize().d_width;

		if (thisWidth > widest)
		{
			widest = thisWidth;
		}

	}

	return widest;
}


/*************************************************************************
	Clear the selected state for all items (implementation)
*************************************************************************/
bool Listbox::clearAllSelections_impl(void)
{
	// flag used so we can track if we did anything.
	bool modified = false;

	for (size_t index = 0; index < d_listItems.size(); ++index)
	{
		if (d_listItems[index]->isSelected())
		{
			d_listItems[index]->setSelected(false);
			modified = true;
		}

	}

	return modified;
}


/*************************************************************************
	Return the ListboxItem under the given window local pixel co-ordinate.
*************************************************************************/
ListboxItem* Listbox::getItemAtPoint(const Point& pt) const
{
	Rect renderArea(getListRenderArea());

	// point must be within the rendering area of the Listbox.
	if (renderArea.isPointInRect(pt))
	{
		float y = renderArea.d_top - d_vertScrollbar->getScrollPosition();

		// test if point is above first item
		if (pt.d_y >= y)
		{
			for (size_t i = 0; i < getItemCount(); ++i)
			{
				y += d_listItems[i]->getPixelSize().d_height;

				if (pt.d_y < y)
				{
					return d_listItems[i];
				}

			}
		}
	}

	return NULL;
}


/*************************************************************************
	Add list box specific events
*************************************************************************/
void Listbox::addListboxEvents(bool bCommon)
{
	if ( bCommon == false )	addEvent(EventListContentsChanged);
	if ( bCommon == false )	addEvent(EventSelectionChanged);
	if ( bCommon == false )	addEvent(EventSortModeChanged);
	if ( bCommon == false )	addEvent(EventMultiselectModeChanged);
	if ( bCommon == false )	addEvent(EventVertScrollbarModeChanged);
	if ( bCommon == false )	addEvent(EventHorzScrollbarModeChanged);
}


/*************************************************************************
	Handler called internally when the list contents are changed	
*************************************************************************/
void Listbox::onListContentsChanged(WindowEventArgs& e)
{
	configureScrollbars();
	requestRedraw();
	fireEvent(EventListContentsChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called internally when the currently selected item or items
	changes.
*************************************************************************/
void Listbox::onSelectionChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventSelectionChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called internally when the sort mode setting changes.
*************************************************************************/
void Listbox::onSortModeChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventSortModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called internally when the multi-select mode setting changes.
*************************************************************************/
void Listbox::onMultiselectModeChanged(WindowEventArgs& e)
{
	fireEvent(EventMultiselectModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called internally when the forced display of the vertical
	scroll bar setting changes.
*************************************************************************/
void Listbox::onVertScrollbarModeChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventVertScrollbarModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called internally when the forced display of the horizontal
	scroll bar setting changes.
*************************************************************************/
void Listbox::onHorzScrollbarModeChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(EventHorzScrollbarModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when we are sized
*************************************************************************/
void Listbox::onSized(WindowEventArgs& e)
{
	// base class handling
	Window::onSized(e);

	configureScrollbars();

	e.handled = true;
}


/*************************************************************************
	Handler for when mouse button is pressed
*************************************************************************/
void Listbox::onMouseButtonDown(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseButtonDown(e);

	if (e.button == LeftButton)
	{
		bool modified = false;

		// clear old selections if no control key is pressed or if multi-select is off
		if (!(e.sysKeys & Control) || !d_multiselect)
		{
			modified = clearAllSelections_impl();
		}

		Point localPos(screenToWindow(e.position));

		if (getMetricsMode() == Relative)
		{
			localPos = relativeToAbsolute(localPos);
		}

		ListboxItem* item = getItemAtPoint(localPos);

		if (item != NULL)
		{
			modified = true;

			// select range or item, depending upon keys and last selected item
			if (((e.sysKeys & Shift) && (d_lastSelected != NULL)) && d_multiselect)
			{
				selectRange(getItemIndex(item), getItemIndex(d_lastSelected));
			}
			else
			{
				item->setSelected(item->isSelected() ^ true);
			}

			// update last selected item
			d_lastSelected = item->isSelected() ? item : NULL;
		}

		// fire event if needed
		if (modified)
		{
			WindowEventArgs args(this);
			onSelectionChanged(args);
		}
		
		e.handled = true;
	}

}


/*************************************************************************
	Handler for mouse wheel changes
*************************************************************************/
void Listbox::onMouseWheel(MouseEventArgs& e)
{
	// base class processing.
	Window::onMouseWheel(e);

	if (d_vertScrollbar->isVisible() && (d_vertScrollbar->getDocumentSize() > d_vertScrollbar->getPageSize()))
	{
		d_vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition() + d_vertScrollbar->getStepSize() * -e.wheelChange);
	}
	else if (d_horzScrollbar->isVisible() && (d_horzScrollbar->getDocumentSize() > d_horzScrollbar->getPageSize()))
	{
		d_horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition() + d_horzScrollbar->getStepSize() * -e.wheelChange);
	}

	e.handled = true;
}

/*************************************************************************
    Handler for mouse movement
*************************************************************************/
void Listbox::onMouseMove(MouseEventArgs& e)
{
    if (d_itemTooltips)
    {
        static ListboxItem* lastItem = NULL;

        Point posi = relativeToAbsolute(screenToWindow(e.position));
        ListboxItem* item = getItemAtPoint(posi);
        if (item != lastItem)
        {
            if (item != NULL)
            {
                setTooltipText(item->getTooltipText());
            }
            else
            {
                setTooltipText("");
            }
            lastItem = item;
        }

        // must check the result from getTooltip(), as the tooltip object could
        // be 0 at any time for various reasons.
        Tooltip* tooltip = getTooltip();

        if (tooltip)
        {
            if (tooltip->getTargetWindow() != this)
                tooltip->setTargetWindow(this);
            else
                tooltip->positionSelf();
        }
    }

    Window::onMouseMove(e);
}


/*************************************************************************
	Ensure the item at the specified index is visible within the list box.	
*************************************************************************/
void Listbox::ensureItemIsVisible(size_t item_index)
{
	// handle simple "scroll to the bottom" case
	if (item_index >= getItemCount())
	{
		d_vertScrollbar->setScrollPosition(d_vertScrollbar->getDocumentSize() - d_vertScrollbar->getPageSize());
	}
	else
	{
		float bottom;
		float listHeight = getListRenderArea().getHeight();
		float top = 0;

		// get height to top of item
		size_t i;
		for (i = 0; i < item_index; ++i)
		{
			top += d_listItems[i]->getPixelSize().d_height;
		}

		// calculate height to bottom of item
		bottom = top + d_listItems[i]->getPixelSize().d_height;

		// account for current scrollbar value
		float currPos = d_vertScrollbar->getScrollPosition();
		top		-= currPos;
		bottom	-= currPos;

		// if top is above the view area, or if item is too big to fit
		if ((top < 0.0f) || ((bottom - top) > listHeight))
		{
			// scroll top of item to top of box.
			d_vertScrollbar->setScrollPosition(currPos + top);
		}
		// if bottom is below the view area
		else if (bottom >= listHeight)
		{
			// position bottom of item at the bottom of the list
			d_vertScrollbar->setScrollPosition(currPos + bottom - listHeight);		
		}
		
		// Item is already fully visible - nothing more to do.
	}

}


/*************************************************************************
	Ensure the item at the specified index is visible within the list box.
*************************************************************************/
void Listbox::ensureItemIsVisible(const ListboxItem* item)
{
	ensureItemIsVisible(getItemIndex(item));
}


/*************************************************************************
	Return whether the vertical scroll bar is always shown.	
*************************************************************************/
bool Listbox::isVertScrollbarAlwaysShown(void) const
{
	return d_forceVertScroll;
}


/*************************************************************************
	Return whether the horizontal scroll bar is always shown.	
*************************************************************************/
bool Listbox::isHorzScrollbarAlwaysShown(void) const
{
	return d_forceHorzScroll;
}

/*************************************************************************
	Add properties for this class
*************************************************************************/
void Listbox::addListboxProperties( bool bCommon )
{
	if ( bCommon == false )   addProperty(&d_sortProperty);
	if ( bCommon == false )   addProperty(&d_multiSelectProperty);
	if ( bCommon == false )   addProperty(&d_forceHorzProperty);
	if ( bCommon == false )   addProperty(&d_forceVertProperty);
	if ( bCommon == false )   addProperty(&d_itemTooltipsProperty);
}


/*************************************************************************
	Remove all items from the list.
*************************************************************************/
bool Listbox::resetList_impl(void)
{
	// just return false if the list is already empty
	if (getItemCount() == 0)
	{
		return false;
	}
	// we have items to be removed and possible deleted
	else
	{
		// delete any items we are supposed to
		for (size_t i = 0; i < getItemCount(); ++i)
		{
			// if item is supposed to be deleted by us
			if (d_listItems[i]->isAutoDeleted())
			{
				// clean up this item.
				delete d_listItems[i];
			}

		}

		// clear out the list.
		d_listItems.clear();

		d_lastSelected = NULL;

		return true;
	}

}

/*************************************************************************
    Handler for scroll position changes.
*************************************************************************/
bool Listbox::handle_scrollChange(const EventArgs& args)
{
    // simply trigger a redraw of the Listbox.
    requestRedraw();
    return true;
}


//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Functions used for predicates in std algorithms
*************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	used for < comparisons between ListboxItem pointers
*************************************************************************/
bool lbi_less(const ListboxItem* a, const ListboxItem* b)
{
	return *a < *b;
}

/*************************************************************************
	used for > comparisons between ListboxItem pointers
*************************************************************************/
bool lbi_greater(const ListboxItem* a, const ListboxItem* b)
{
	return *a > *b;
}

} // End of  CEGUI namespace section
