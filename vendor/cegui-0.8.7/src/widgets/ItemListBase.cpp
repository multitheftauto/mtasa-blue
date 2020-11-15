/***********************************************************************
	created:	31/3/2005
	author:		Tomas Lindquist Olsen (based on original Listbox code by Paul D Turner)

	purpose:	Implementation of ItemListBase widget base class
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
#include "CEGUI/CoordConverter.h"
#include "CEGUI/widgets/ItemListBase.h"
#include "CEGUI/widgets/ItemEntry.h"

#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
const String ItemListBase::EventNamespace("ItemListBase");

/*************************************************************************
    ItemListBaseWindowRenderer
*************************************************************************/
ItemListBaseWindowRenderer::ItemListBaseWindowRenderer(const String& name) :
    WindowRenderer(name, ItemListBase::EventNamespace)
{
}


/************************************************************************/


/*************************************************************************
    used for < comparisons between ItemEntry pointers
*************************************************************************/
static bool ItemEntry_less(const ItemEntry* a, const ItemEntry* b)
{
    return a->getText() < b->getText();
}


/*************************************************************************
    used for > comparisons between ItemEntry pointers
*************************************************************************/
static bool ItemEntry_greater(const ItemEntry* a, const ItemEntry* b)
{
    return (a->getText() > b->getText());
}


/************************************************************************/

/*************************************************************************
	Constants
*************************************************************************/
// event names
const String ItemListBase::EventListContentsChanged( "ListContentsChanged" );
const String ItemListBase::EventSortEnabledChanged("SortEnabledChanged");
const String ItemListBase::EventSortModeChanged("SortModeChanged");

/*************************************************************************
	Constructor for ItemListBase base class.
*************************************************************************/
ItemListBase::ItemListBase(const String& type, const String& name)
	: Window(type, name),
	d_autoResize(false),
	d_sortEnabled(false),
	d_sortMode(Ascending),
	d_sortCallback(0),
	d_resort(false)
{
    // by default we dont have a content pane, but to make sure things still work
    // we "emulate" it by setting it to this
    d_pane = this;

	// add properties for ItemListBase class
	addItemListBaseProperties();
}


/*************************************************************************
	Destructor for ItemListBase base class.
*************************************************************************/
ItemListBase::~ItemListBase(void)
{
    //resetList_impl();
}


/*************************************************************************
	Initialise components
*************************************************************************/
void ItemListBase::initialiseComponents(void)
{
    // this pane may be ourselves, and in fact is by default...
    d_pane->subscribeEvent(Window::EventChildRemoved,
        Event::Subscriber(&ItemListBase::handle_PaneChildRemoved, this));
}


/*************************************************************************
	Return the item at index position 'index'.
*************************************************************************/
ItemEntry* ItemListBase::getItemFromIndex(size_t index) const
{
	if (index < d_listItems.size())
	{
		return d_listItems[index];
	}
	else
	{
		CEGUI_THROW(InvalidRequestException(
            "the specified index is out of range for this ItemListBase."));
	}
}


/*************************************************************************
	Return the index of ItemEntry \a item
*************************************************************************/
size_t ItemListBase::getItemIndex(const ItemEntry* item) const
{
	ItemEntryList::const_iterator pos = std::find(d_listItems.begin(), d_listItems.end(), item);

	if (pos != d_listItems.end())
	{
		return std::distance(d_listItems.begin(), pos);
	}
	else
	{
		CEGUI_THROW(InvalidRequestException(
            "the specified ItemEntry is not attached to this ItemListBase."));
	}
}


/*************************************************************************
	Search the list for an item with the specified text
*************************************************************************/
ItemEntry* ItemListBase::findItemWithText(const String& text, const ItemEntry* start_item)
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
	Return whether the specified ItemEntry is in the List
*************************************************************************/
bool ItemListBase::isItemInList(const ItemEntry* item) const
{
	//return std::find(d_listItems.begin(), d_listItems.end(), item) != d_listItems.end();
	return (item->d_ownerList == this);
}


/*************************************************************************
	Remove all items from the list.
*************************************************************************/
void ItemListBase::resetList(void)
{
	if (resetList_impl())
	{
		handleUpdatedItemData();
	}
}


/*************************************************************************
	Add the given ItemEntry to the list.
*************************************************************************/
void ItemListBase::addItem(ItemEntry* item)
{
    // make sure the item is valid and that we dont already have it in our list
	if (item && item->d_ownerList != this)
	{
	    // if sorting is enabled, re-sort the list
        if (d_sortEnabled)
        {
            d_listItems.insert(
                std::upper_bound(d_listItems.begin(), d_listItems.end(), item, getRealSortCallback()),
                item);
        }
        // just stick it on the end.
        else
        {
            d_listItems.push_back(item);
        }
        // make sure it gets added properly
		item->d_ownerList = this;
		addChild(item);
		handleUpdatedItemData();
	}
}


/*************************************************************************
	Insert an item into the list box after a specified item already in
	the list.
*************************************************************************/
void ItemListBase::insertItem(ItemEntry* item, const ItemEntry* position)
{
    if (d_sortEnabled)
    {
        addItem(item);
    }
	else if (item && item->d_ownerList != this)
	{
		// if position is NULL begin insert at begining, else insert after item 'position'
		ItemEntryList::iterator ins_pos;

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
                    "the specified ItemEntry for parameter 'position' is not attached to this ItemListBase."));
			}

		}

		d_listItems.insert(ins_pos, item);
		item->d_ownerList = this;
		addChild(item);

		handleUpdatedItemData();
	}
}


/*************************************************************************
	Removes the given item from the list box.
*************************************************************************/
void ItemListBase::removeItem(ItemEntry* item)
{
	if (item && item->d_ownerList == this)
	{
	    d_pane->removeChild(item);
	    if (item->isDestroyedByParent())
	    {
	        WindowManager::getSingleton().destroyWindow(item);
	    }
	}
}


/*************************************************************************
	Set wheter or not this ItemListBase widget should automatically
	resize to fit its content.
*************************************************************************/
void ItemListBase::setAutoResizeEnabled(bool setting)
{
	bool old = d_autoResize;
	d_autoResize = setting;

	// if not already enabled, trigger a resize - only if not currently initialising
	if ( d_autoResize && !old && !d_initialising)
	{
		sizeToContent();
	}
}


/*************************************************************************
	Causes the list box to update it's internal state after changes have
	been made to one or more attached ItemEntry objects.
*************************************************************************/
void ItemListBase::handleUpdatedItemData(bool resort)
{
    if (!d_destructionStarted)
    {
        d_resort |= resort;
        WindowEventArgs args(this);
        onListContentsChanged(args);
    }
}


/*************************************************************************
	Handler called internally when the list contents are changed
*************************************************************************/
void ItemListBase::onListContentsChanged(WindowEventArgs& e)
{
    // if we are not currently initialising we might have things todo
	if (!d_initialising)
	{
	    invalidate();

	    // if auto resize is enabled - do it
	    if (d_autoResize)
		    sizeToContent();

	    // resort list if requested and enabled
        if (d_resort && d_sortEnabled)
            sortList(false);
        d_resort = false;

	    // redo the item layout and fire our event
	    layoutItemWidgets();
	    fireEvent(EventListContentsChanged, e, EventNamespace);
	}
}

//----------------------------------------------------------------------------//
void ItemListBase::onParentSized(ElementEventArgs& e)
{
    Window::onParentSized(e);

    if (d_autoResize)
        sizeToContent();
}

//----------------------------------------------------------------------------//

/************************************************************************
    Handler for when a child is removed
*************************************************************************/
/*void ItemListBase::onChildRemoved(WindowEventArgs& e)
{
    // if destruction has already begun, we don't need to do anything.
    // everything has to go anyway
    // make sure it is removed from the itemlist if we have an ItemEntry
    if (!d_destructionStarted && e.window->testClassName("ItemEntry"))
    {
        ItemEntryList::iterator pos = std::find(d_listItems.begin(), d_listItems.end(), e.window);

        // if item is in the list
        if (pos != d_listItems.end())
        {
            // remove item
            (*pos)->d_ownerList = 0;
            d_listItems.erase(pos);
            // trigger list update
            handleUpdatedItemData();
        }
    }

    // base class handling
    Window::onChildRemoved(e);
}*/

/************************************************************************
    Handler for when the window initiates destruction
*************************************************************************/
/*void ItemListBase::onDestructionStarted(WindowEventArgs& e)
{
    // base class handling
    Window::onDestructionStarted(e);

    // remove everything from the list properly
    resetList_impl();
}*/


/*************************************************************************
	Remove all items from the list.
*************************************************************************/
bool ItemListBase::resetList_impl(void)
{
	// just return false if the list is already empty
	if (d_listItems.empty())
	{
		return false;
	}
	// we have items to be removed and possible deleted
	else
	{
		// delete any items we are supposed to
		while (!d_listItems.empty())
		{
		    ItemEntry* item = d_listItems[0];
			d_pane->removeChild(item);
			if (item->isDestroyedByParent())
			{
			    WindowManager::getSingleton().destroyWindow(item);
			}
		}

		// list is cleared by the removeChild calls
		return true;
	}
}


/*************************************************************************
	Add ItemListBase specific properties
*************************************************************************/
void ItemListBase::addItemListBaseProperties(void)
{
    const String propertyOrigin("ItemListBase");

        CEGUI_DEFINE_PROPERTY(ItemListBase, bool,
            "AutoResizeEnabled", "Property to get/set the state of the auto resizing enabled setting for the ItemListBase.  Value is either \"true\" or \"false\".",
            &ItemListBase::setAutoResizeEnabled, &ItemListBase::isAutoResizeEnabled, false
        );
        CEGUI_DEFINE_PROPERTY(ItemListBase, bool,
            "SortEnabled", "Property to get/set the state of the sorting enabled setting for the ItemListBase.  Value is either \"true\" or \"false\".",
            &ItemListBase::setSortEnabled, &ItemListBase::isSortEnabled, false
        );
        CEGUI_DEFINE_PROPERTY(ItemListBase, ItemListBase::SortMode,
            "SortMode", "Property to get/set the sorting mode for the ItemListBase.  Value is either \"Ascending\", \"Descending\" or \"UserSort\".",
            &ItemListBase::setSortMode, &ItemListBase::getSortMode, ItemListBase::Ascending
        );
}


/*************************************************************************
	Internal version of adding a child window.
*************************************************************************/
void ItemListBase::addChild_impl(Element* element)
{
    ItemEntry* item = dynamic_cast<ItemEntry*>(element);
    
    // if this is an ItemEntry we add it like one, but only if it is not already in the list!
    if (item)
    {
        // add to the pane if we have one
        if (d_pane != this)
        {
            d_pane->addChild(item);
        }
        // add item directly to us
        else
        {
            Window::addChild_impl(item);
        }

	    if (item->d_ownerList != this)
	    {
	        // perform normal addItem
	        // if sorting is enabled, re-sort the list
            if (d_sortEnabled)
            {
                d_listItems.insert(
                    std::upper_bound(d_listItems.begin(), d_listItems.end(), item, getRealSortCallback()),
                    item);
            }
            // just stick it on the end.
            else
            {
                d_listItems.push_back(item);
            }
	        item->d_ownerList = this;
		    handleUpdatedItemData();
	    }
	}
	// otherwise it's base class processing
    else
    {
        Window::addChild_impl(element);
    }
}


/************************************************************************
    Initialisation done
*************************************************************************/
void ItemListBase::endInitialisation(void)
{
    Window::endInitialisation();
    handleUpdatedItemData(true);
}


/************************************************************************
	Perform child window layout
************************************************************************/
void ItemListBase::performChildWindowLayout(bool nonclient_sized_hint,
                                            bool client_sized_hint)
{
	Window::performChildWindowLayout(nonclient_sized_hint,
                                     client_sized_hint);
	// if we are not currently initialising
	if (!d_initialising)
	{
	    // Redo the item layout.
	    // We don't just call handleUpdateItemData, as that could trigger a resize,
	    // which is not what is being requested.
	    // It would also cause infinite recursion... so lets just avoid that :)
	    layoutItemWidgets();
	}
}

/************************************************************************
    Resize to fit content
************************************************************************/
void ItemListBase::sizeToContent_impl(void)
{
    Rectf renderArea(getItemRenderArea());
    Rectf wndArea(CoordConverter::asAbsolute(getArea(), getParentPixelSize()));

    // get size of content
    Sizef sz(getContentSize());

    // calculate the full size with the frame accounted for and resize the window to this
    sz.d_width  += wndArea.getWidth() - renderArea.getWidth();
    sz.d_height += wndArea.getHeight() - renderArea.getHeight();
    setSize(USize(cegui_absdim(sz.d_width), cegui_absdim(sz.d_height)));
}

/************************************************************************
    Get item render area
************************************************************************/
Rectf ItemListBase::getItemRenderArea(void) const
{
    if (d_windowRenderer != 0)
    {
        ItemListBaseWindowRenderer* wr = (ItemListBaseWindowRenderer*)d_windowRenderer;
        return wr->getItemRenderArea();
    }
    else
    {
        //return getItemRenderArea_impl();
        CEGUI_THROW(InvalidRequestException(
            "This function must be implemented by the window renderer module"));
    }
}

/************************************************************************
    Handler to manage items being removed from the content pane
************************************************************************/
bool ItemListBase::handle_PaneChildRemoved(const EventArgs& e)
{
    Window* wnd = static_cast<const WindowEventArgs&>(e).window;

    // make sure it is removed from the itemlist if we have an ItemEntry
    ItemEntry* item = dynamic_cast<ItemEntry*>(wnd);
    if (item)
    {
        ItemEntryList::iterator pos = std::find(d_listItems.begin(), d_listItems.end(), item);

        // if item is in the list
        if (pos != d_listItems.end())
        {
            // make sure the item is no longer related to us
            (*pos)->d_ownerList = 0;
            // remove item
            d_listItems.erase(pos);
            // trigger list update
            handleUpdatedItemData();
        }
    }

    return false;
}

/************************************************************************
    Set sorting enabled state
************************************************************************/
void ItemListBase::setSortEnabled(bool setting)
{
    if (d_sortEnabled != setting)
    {
        d_sortEnabled = setting;

        if (d_sortEnabled && !d_initialising)
        {
            sortList();
        }

        WindowEventArgs e(this);
        onSortEnabledChanged(e);
    }
}

/************************************************************************
    Set the user sorting callback
************************************************************************/
void ItemListBase::setSortCallback(SortCallback cb)
{
    if (d_sortCallback != cb)
    {
        d_sortCallback = cb;
        if (d_sortEnabled && !d_initialising)
        {
            sortList();
        }
        handleUpdatedItemData(true);
    }
}

/************************************************************************
    Handle sort enabled changed
************************************************************************/
void ItemListBase::onSortEnabledChanged(WindowEventArgs& e)
{
    fireEvent(EventSortEnabledChanged, e);
}

/************************************************************************
    Handle sort mode changed
************************************************************************/
void ItemListBase::onSortModeChanged(WindowEventArgs& e)
{
    fireEvent(EventSortModeChanged, e);
}

/************************************************************************
    Sort list
************************************************************************/
void ItemListBase::sortList(bool relayout)
{
    std::sort(d_listItems.begin(), d_listItems.end(), getRealSortCallback());
    if (relayout)
    {
        layoutItemWidgets();
    }
}

bool ItemListBase::validateWindowRenderer(const WindowRenderer* renderer) const
{
	return dynamic_cast<const ItemListBaseWindowRenderer*>(renderer) != 0;
}

/************************************************************************
    Get the real function pointer to use for the sorting operation
************************************************************************/
ItemListBase::SortCallback ItemListBase::getRealSortCallback() const
{
    switch (d_sortMode)
    {
    case Ascending:
        return &ItemEntry_less;

    case Descending:
        return &ItemEntry_greater;

    case UserSort:
        return (d_sortCallback!=0) ? d_sortCallback : &ItemEntry_less;

    // we default to ascending sorting
    default:
        return &ItemEntry_less;
    }
}

/************************************************************************
    Set sort mode
************************************************************************/
void ItemListBase::setSortMode(SortMode mode)
{
    if (d_sortMode != mode)
    {
        d_sortMode = mode;
        if (d_sortEnabled && !d_initialising)
            sortList();

        WindowEventArgs e(this);
        onSortModeChanged(e);
    }
}

} // End of  CEGUI namespace section
