/***********************************************************************
	created:	13/4/2004
	author:		Paul D Turner

	purpose:	Implementation of Listbox widget base class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/Exceptions.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/widgets/Listbox.h"
#include "CEGUI/widgets/ListboxItem.h"
#include "CEGUI/widgets/Scrollbar.h"
#include "CEGUI/widgets/Tooltip.h"
#include "CEGUI/CoordConverter.h"
#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
const String Listbox::EventNamespace("Listbox");
const String Listbox::WidgetTypeName("CEGUI/Listbox");

/*************************************************************************
    ListboxWindowRenderer
*************************************************************************/
ListboxWindowRenderer::ListboxWindowRenderer(const String& name) :
    WindowRenderer(name, Listbox::EventNamespace)
{
}

/*************************************************************************
	Constants
*************************************************************************/
// event names
const String Listbox::EventListContentsChanged( "ListContentsChanged" );
const String Listbox::EventSelectionChanged( "SelectionChanged" );
const String Listbox::EventSortModeChanged( "SortModeChanged" );
const String Listbox::EventMultiselectModeChanged( "MultiselectModeChanged" );
const String Listbox::EventVertScrollbarModeChanged( "VertScrollbarModeChanged" );
const String Listbox::EventHorzScrollbarModeChanged( "HorzScrollbarModeChanged" );

/*************************************************************************
    Child Widget name constants
*************************************************************************/
const String Listbox::VertScrollbarName( "__auto_vscrollbar__" );
const String Listbox::HorzScrollbarName( "__auto_hscrollbar__" );

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
	d_lastSelected(0)
{
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
void Listbox::initialiseComponents(void)
{
	// get the component sub-widgets
	Scrollbar* vertScrollbar = getVertScrollbar();
	Scrollbar* horzScrollbar = getHorzScrollbar();

    vertScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&Listbox::handle_scrollChange, this));
    horzScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&Listbox::handle_scrollChange, this));

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
	return getNextSelected(0);
}


/*************************************************************************
	Return a pointer to the next selected item after item 'start_item'
*************************************************************************/
ListboxItem* Listbox::getNextSelected(const ListboxItem* start_item) const
{
	// if start_item is NULL begin search at begining, else start at item after start_item
	size_t index = (start_item == 0) ? 0 : (getItemIndex(start_item) + 1);

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
	return 0;
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
		CEGUI_THROW(InvalidRequestException(
            "the specified index is out of range for this Listbox."));
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
		CEGUI_THROW(InvalidRequestException(
            "the specified ListboxItem is not attached to this Listbox."));
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
		CEGUI_THROW(InvalidRequestException(
            "the specified index is out of range for this Listbox."));
	}

}


/*************************************************************************
	Search the list for an item with the specified text
*************************************************************************/
ListboxItem* Listbox::findItemWithText(const String& text, const ListboxItem* start_item)
{
	// if start_item is NULL begin search at begining, else start at item after start_item
	size_t index = (!start_item) ? 0 : (getItemIndex(start_item) + 1);

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
	return 0;
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
	if (item)
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
	else if (item)
	{
		// establish ownership
		item->setOwnerWindow(this);

		// if position is NULL begin insert at begining, else insert after item 'position'
		LBItemList::iterator ins_pos;

		if (!position)
		{
			ins_pos = d_listItems.begin();
		}
		else
		{
			ins_pos = std::find(d_listItems.begin(), d_listItems.end(), position);

			// throw if item 'position' is not in the list
			if (ins_pos == d_listItems.end())
			{
				CEGUI_THROW(InvalidRequestException(
                    "the specified ListboxItem for parameter 'position' is not attached to this Listbox."));
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
	if (item)
	{
		LBItemList::iterator pos = std::find(d_listItems.begin(), d_listItems.end(), item);

		// if item is in the list
		if (pos != d_listItems.end())
		{
			// disown item
			(*pos)->setOwnerWindow(0);

			// remove item
			d_listItems.erase(pos);

			// if item was the last selected item, reset that to NULL
			if (item == d_lastSelected)
			{
				d_lastSelected = 0;
			}

			// if item is supposed to be deleted by us
			if (item->isAutoDeleted())
			{
				// clean up this item.
				CEGUI_DELETE_AO item;
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
            resortList();
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
		CEGUI_THROW(InvalidRequestException(
            "the specified ListboxItem is not attached to this Listbox."));
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
		CEGUI_THROW(InvalidRequestException(
            "the value passed in the 'item_index' parameter is out of range for this Listbox."));
	}

}


/*************************************************************************
	Causes the list box to update it's internal state after changes have
	been made to one or more attached ListboxItem objects.
*************************************************************************/
void Listbox::handleUpdatedItemData(void)
{
    if (d_sorted)
        resortList();

	configureScrollbars();
	invalidate();
}


/*************************************************************************
	display required integrated scroll bars according to current state
	of the list box and update their values.
*************************************************************************/
void Listbox::configureScrollbars(void)
{
    Scrollbar* vertScrollbar = getVertScrollbar();
    Scrollbar* horzScrollbar = getHorzScrollbar();

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
	Rectf renderArea(getListRenderArea());

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
	if (!d_listItems.empty())
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
	Return the ListboxItem under the given screen pixel co-ordinate.
*************************************************************************/
ListboxItem* Listbox::getItemAtPoint(const Vector2f& pt) const
{
    const Vector2f local_pos(CoordConverter::screenToWindow(*this, pt));
	const Rectf renderArea(getListRenderArea());

	// point must be within the rendering area of the Listbox.
	if (renderArea.isPointInRect(local_pos))
	{
		float y = renderArea.d_min.d_x - getVertScrollbar()->getScrollPosition();

		// test if point is above first item
		if (local_pos.d_y >= y)
		{
			for (size_t i = 0; i < getItemCount(); ++i)
			{
				y += d_listItems[i]->getPixelSize().d_height;

				if (local_pos.d_y < y)
				{
					return d_listItems[i];
				}

			}
		}
	}

	return 0;
}


/*************************************************************************
	Handler called internally when the list contents are changed
*************************************************************************/
void Listbox::onListContentsChanged(WindowEventArgs& e)
{
	configureScrollbars();
	invalidate();
	fireEvent(EventListContentsChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called internally when the currently selected item or items
	changes.
*************************************************************************/
void Listbox::onSelectionChanged(WindowEventArgs& e)
{
	invalidate();
	fireEvent(EventSelectionChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called internally when the sort mode setting changes.
*************************************************************************/
void Listbox::onSortModeChanged(WindowEventArgs& e)
{
	invalidate();
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
	invalidate();
	fireEvent(EventVertScrollbarModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called internally when the forced display of the horizontal
	scroll bar setting changes.
*************************************************************************/
void Listbox::onHorzScrollbarModeChanged(WindowEventArgs& e)
{
	invalidate();
	fireEvent(EventHorzScrollbarModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when we are sized
*************************************************************************/
void Listbox::onSized(ElementEventArgs& e)
{
	// base class handling
	Window::onSized(e);

	configureScrollbars();

	++e.handled;
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

		ListboxItem* item = getItemAtPoint(e.position);

		if (item)
		{
			modified = true;

			// select range or item, depending upon keys and last selected item
			if (((e.sysKeys & Shift) && (d_lastSelected != 0)) && d_multiselect)
			{
				selectRange(getItemIndex(item), getItemIndex(d_lastSelected));
			}
			else
			{
				item->setSelected(item->isSelected() ^ true);
			}

			// update last selected item
			d_lastSelected = item->isSelected() ? item : 0;
		}

		// fire event if needed
		if (modified)
		{
			WindowEventArgs args(this);
			onSelectionChanged(args);
		}

		++e.handled;
	}

}


/*************************************************************************
	Handler for mouse wheel changes
*************************************************************************/
void Listbox::onMouseWheel(MouseEventArgs& e)
{
	// base class processing.
	Window::onMouseWheel(e);

    Scrollbar* vertScrollbar = getVertScrollbar();
    Scrollbar* horzScrollbar = getHorzScrollbar();

	if (vertScrollbar->isEffectiveVisible() && (vertScrollbar->getDocumentSize() > vertScrollbar->getPageSize()))
	{
		vertScrollbar->setScrollPosition(vertScrollbar->getScrollPosition() + vertScrollbar->getStepSize() * -e.wheelChange);
	}
	else if (horzScrollbar->isEffectiveVisible() && (horzScrollbar->getDocumentSize() > horzScrollbar->getPageSize()))
	{
		horzScrollbar->setScrollPosition(horzScrollbar->getScrollPosition() + horzScrollbar->getStepSize() * -e.wheelChange);
	}

	++e.handled;
}

/*************************************************************************
    Handler for mouse movement
*************************************************************************/
void Listbox::onMouseMove(MouseEventArgs& e)
{
    if (d_itemTooltips)
    {
        static ListboxItem* lastItem = 0;

        ListboxItem* item = getItemAtPoint(e.position);
        if (item != lastItem)
        {
            if (item)
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
    Scrollbar* vertScrollbar = getVertScrollbar();

	// handle simple "scroll to the bottom" case
	if (item_index >= getItemCount())
	{
		vertScrollbar->setScrollPosition(vertScrollbar->getDocumentSize() - vertScrollbar->getPageSize());
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
		float currPos = vertScrollbar->getScrollPosition();
		top		-= currPos;
		bottom	-= currPos;

		// if top is above the view area, or if item is too big to fit
		if ((top < 0.0f) || ((bottom - top) > listHeight))
		{
			// scroll top of item to top of box.
			vertScrollbar->setScrollPosition(currPos + top);
		}
		// if bottom is below the view area
		else if (bottom >= listHeight)
		{
			// position bottom of item at the bottom of the list
			vertScrollbar->setScrollPosition(currPos + bottom - listHeight);
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
void Listbox::addListboxProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;
    
    CEGUI_DEFINE_PROPERTY(Listbox, bool,
        "Sort","Property to get/set the sort setting of the list box.  Value is either \"true\" or \"false\".",
        &Listbox::setSortingEnabled, &Listbox::isSortEnabled, false /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(Listbox, bool,
        "MultiSelect","Property to get/set the multi-select setting of the list box.  Value is either \"true\" or \"false\".",
        &Listbox::setMultiselectEnabled, &Listbox::isMultiselectEnabled, false /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(Listbox, bool,
        "ForceVertScrollbar","Property to get/set the 'always show' setting for the vertical scroll bar of the list box.  Value is either \"true\" or \"false\".",
        &Listbox::setShowVertScrollbar, &Listbox::isVertScrollbarAlwaysShown, false /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(Listbox, bool,
        "ForceHorzScrollbar","Property to get/set the 'always show' setting for the horizontal scroll bar of the list box.  Value is either \"true\" or \"false\".",
        &Listbox::setShowHorzScrollbar, &Listbox::isHorzScrollbarAlwaysShown, false /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(Listbox, bool,
        "ItemTooltips","Property to access the show item tooltips setting of the list box.  Value is either \"true\" or \"false\".",
        &Listbox::setItemTooltipsEnabled, &Listbox::isItemTooltipsEnabled, false /* TODO: Inconsistency */
    );
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
				CEGUI_DELETE_AO d_listItems[i];
			}

		}

		// clear out the list.
		d_listItems.clear();

		d_lastSelected = 0;

		return true;
	}

}

/*************************************************************************
    Handler for scroll position changes.
*************************************************************************/
bool Listbox::handle_scrollChange(const EventArgs&)
{
    // simply trigger a redraw of the Listbox.
    invalidate();
    return true;
}

/*************************************************************************
    Return a pointer to the vertical scrollbar component widget for this
    Listbox.
*************************************************************************/
Scrollbar* Listbox::getVertScrollbar() const
{
    return static_cast<Scrollbar*>(getChild(VertScrollbarName));
}

/*************************************************************************
    Return a pointer to the horizontal scrollbar component widget for this
    Listbox.
*************************************************************************/
Scrollbar* Listbox::getHorzScrollbar() const
{
    return static_cast<Scrollbar*>(getChild(HorzScrollbarName));
}

/*************************************************************************
    Return a Rect object describing, in un-clipped pixels, the window
    relative area that is to be used for rendering list items.
*************************************************************************/
Rectf Listbox::getListRenderArea() const
{
    if (d_windowRenderer != 0)
    {
        ListboxWindowRenderer* wr = (ListboxWindowRenderer*)d_windowRenderer;
        return wr->getListRenderArea();
    }
    else
    {
        CEGUI_THROW(InvalidRequestException(
            "This function must be implemented by the window renderer module"));
    }
}

bool Listbox::validateWindowRenderer(const WindowRenderer* renderer) const
{
	return dynamic_cast<const ListboxWindowRenderer*>(renderer) != 0;
}

/*************************************************************************
    Function to resort the list data.
*************************************************************************/
void Listbox::resortList()
{
    std::sort(d_listItems.begin(), d_listItems.end(), &lbi_less);
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
