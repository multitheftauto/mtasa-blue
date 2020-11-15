/************************************************************************
    created:    Tue Sep 27 2005
    author:     Tomas Lindquist Olsen
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
#include "CEGUI/widgets/ItemListbox.h"
#include "CEGUI/Exceptions.h"

// begin CEGUI namespace
namespace CEGUI
{

/*************************************************************************
    Constants
*************************************************************************/
// event strings
const String ItemListbox::EventNamespace("ItemListbox");
const String ItemListbox::WidgetTypeName("CEGUI/ItemListbox");
const String ItemListbox::EventSelectionChanged("SelectionChanged");
const String ItemListbox::EventMultiSelectModeChanged("MultiSelectModeChanged");

/************************************************************************
    Constructor
************************************************************************/
ItemListbox::ItemListbox(const String& type, const String& name) :
    ScrolledItemListBase(type, name),
    d_multiSelect(false),
    d_lastSelected(0),
    d_nextSelectionIndex(0)
{
    addItemListboxProperties();
}

/************************************************************************
    Layout items
************************************************************************/
void ItemListbox::layoutItemWidgets()
{
    float y = 0;
    float widest = 0;

    ItemEntryList::iterator i = d_listItems.begin();
    ItemEntryList::iterator end = d_listItems.end();

    while (i!=end)
    {
        ItemEntry* entry = *i;
        const Sizef pxs = entry->getItemPixelSize();
        if (pxs.d_width > widest)
        {
            widest = pxs.d_width;
        }

        entry->setArea(URect(
            UVector2(cegui_absdim(0), cegui_absdim(y)),
            UVector2(cegui_reldim(1), cegui_absdim(y + pxs.d_height))
            ));

        y += pxs.d_height;

        ++i;
    }

    // reconfigure scrollbars
    configureScrollbars(Sizef(widest, y));
}

/************************************************************************
    Get size of items
************************************************************************/
Sizef ItemListbox::getContentSize() const
{
    float h = 0;

    ItemEntryList::const_iterator i = d_listItems.begin();
    ItemEntryList::const_iterator end = d_listItems.end();
    while (i!=end)
    {
        h += (*i)->getItemPixelSize().d_height;
        ++i;
    }

    return Sizef(getItemRenderArea().getWidth(), h);
}

/************************************************************************
    Get the number of selected items
************************************************************************/
size_t ItemListbox::getSelectedCount() const
{
    if (!d_multiSelect)
    {
        return d_lastSelected ? 1 : 0;
    }

    size_t count = 0;
    size_t max = d_listItems.size();
    for (size_t i=0; i<max; ++i)
    {
        if (d_listItems[i]->isSelected())
        {
            ++count;
        }
    }

    return count;
}

/************************************************************************
    Get a pointer to the first selected item starting from the given index
************************************************************************/
ItemEntry* ItemListbox::findSelectedItem(size_t start_index) const
{
    const size_t max = d_listItems.size();

    for (size_t i = start_index; i < max; ++i)
    {
        ItemEntry* li = d_listItems[i];
        if (li->isSelected())
        {
            d_nextSelectionIndex = i + 1;
            return li;
        }
    }

    return 0;
}

/************************************************************************
    Get a pointer to the first selected item
************************************************************************/
ItemEntry* ItemListbox::getFirstSelectedItem(size_t start_index) const
{
    if (!d_multiSelect)
    {
        return d_lastSelected;
    }
    return findSelectedItem(start_index);
}

/************************************************************************
    Get a pointer to the next selected item using internal counter
************************************************************************/
ItemEntry* ItemListbox::getNextSelectedItem() const
{
    if (!d_multiSelect)
    {
        return 0;
    }
    return findSelectedItem(d_nextSelectionIndex);
}

/************************************************************************
    Get a pointer to the next selected item after the given item
************************************************************************/
ItemEntry* ItemListbox::getNextSelectedItemAfter(const ItemEntry* start_item) const
{
    if (start_item==0||!d_multiSelect)
    {
        return 0;
    }

    size_t max = d_listItems.size();
    size_t i = getItemIndex(start_item);

    while (i<max)
    {
        ItemEntry* li = d_listItems[i];
        if (li->isSelected())
        {
            return li;
        }
        ++i;
    }

    return 0;
}

/************************************************************************
    Set whether multiple selections should be allowed
************************************************************************/
void ItemListbox::setMultiSelectEnabled(bool state)
{
    if (state != d_multiSelect)
    {
        d_multiSelect = state;
        WindowEventArgs e(this);
        onMultiSelectModeChanged(e);
    }
}

/************************************************************************
    Notify item clicked
************************************************************************/
void ItemListbox::notifyItemClicked(ItemEntry* li)
{
    bool sel_state = !(li->isSelected() && d_multiSelect);
    bool skip = false;

    // multiselect enabled
    if (d_multiSelect)
    {
        uint syskeys = getGUIContext().getSystemKeys().get();
        ItemEntry* last = d_lastSelected;

        // no Control? clear others
        if (!(syskeys & Control))
        {
            clearAllSelections();
            if (!sel_state)
            {
                sel_state=true;
            }
        }

        // select range if Shift if held, and we have a 'last selection'
        if (last && (syskeys & Shift))
        {
            selectRange(getItemIndex(last),getItemIndex(li));
            skip = true;
        }
    }
    else
    {
        clearAllSelections();
    }

    if (!skip)
    {
        li->setSelected_impl(sel_state,false);
        if (sel_state)
        {
            d_lastSelected = li;
        }
        else if (d_lastSelected == li)
        {
            d_lastSelected = 0;
        }
    }

    WindowEventArgs e(this);
    onSelectionChanged(e);
}

/************************************************************************
    Notify item select state change
************************************************************************/
void ItemListbox::notifyItemSelectState(ItemEntry* li, bool state)
{
    // deselect
    if (!state)
    {
        // clear last selection if this one was it
        if (d_lastSelected == li)
        {
            d_lastSelected = 0;
        }
    }
    // if we dont support multiselect, we must clear all the other selections
    else if (!d_multiSelect)
    {
        clearAllSelections();
        li->setSelected_impl(true,false);
        d_lastSelected = li;
    }

    WindowEventArgs e(this);
    onSelectionChanged(e);
}

/*************************************************************************
    Add ItemListbox specific properties
*************************************************************************/
void ItemListbox::addItemListboxProperties()
{
    const String propertyOrigin("ItemListbox");
    CEGUI_DEFINE_PROPERTY(ItemListbox, bool,
        "MultiSelect","Property to get/set the state of the multiselect enabled setting for the ItemListbox.  Value is either \"true\" or \"false\".",
        &ItemListbox::setMultiSelectEnabled, &ItemListbox::isMultiSelectEnabled, false
    );
}

/*************************************************************************
    Query item selection state
*************************************************************************/
bool ItemListbox::isItemSelected(size_t index) const
{
    if (index >= d_listItems.size())
    {
        CEGUI_THROW(InvalidRequestException(
            "The index given is out of range for this ItemListbox"));
    }
    ItemEntry *li = d_listItems[index];
    return li->isSelected();
}

/*************************************************************************
    Clear all selections
*************************************************************************/
void ItemListbox::clearAllSelections()
{
    size_t max = d_listItems.size();
    for (size_t i=0; i<max; ++i)
    {
        d_listItems[i]->setSelected_impl(false,false);
    }
    d_lastSelected = 0;

    WindowEventArgs e(this);
    onSelectionChanged(e);
}

/*************************************************************************
    Select range of items
*************************************************************************/
void ItemListbox::selectRange(size_t a, size_t z)
{
    // do nothing if the list is empty
    if (d_listItems.empty())
    {
        return;
    }

    size_t max = d_listItems.size();
    if (a >= max)
    {
        a = 0;
    }
    if (z >= max)
    {
        z = max-1;
    }

    if (a>z)
    {
        size_t tmp = a;
        a = z;
        z = tmp;
    }

    for (size_t i=a; i<=z; ++i)
    {
        d_listItems[i]->setSelected_impl(true,false);
    }
    d_lastSelected = d_listItems[z];
    

    WindowEventArgs e(this);
    onSelectionChanged(e);
}

/************************************************************************
    Select all items if allowed
************************************************************************/
void ItemListbox::selectAllItems()
{
    if (!d_multiSelect)
    {
        return;
    }

    size_t max = d_listItems.size();
    for (size_t i=0; i<max; ++i)
    {
        d_lastSelected = d_listItems[i];
        d_lastSelected->setSelected_impl(true,false);
    }

    WindowEventArgs e(this);
    onSelectionChanged(e);
}

/************************************************************************
    Handle selection changed
************************************************************************/
void ItemListbox::onSelectionChanged(WindowEventArgs& e)
{
    fireEvent(EventSelectionChanged, e);
}

/************************************************************************
    Handle multiselect mode changed
************************************************************************/
void ItemListbox::onMultiSelectModeChanged(WindowEventArgs& e)
{
    fireEvent(EventMultiSelectModeChanged, e);
}

/************************************************************************
    Handle key down event
************************************************************************/
void ItemListbox::onKeyDown(KeyEventArgs& e)
{
    ScrolledItemListBase::onKeyDown(e);

    // select all (if allowed) on Ctrl+A
    if (d_multiSelect)
    {
        uint sysKeys = getGUIContext().getSystemKeys().get();
        if (e.scancode == Key::A && (sysKeys&Control))
        {
            selectAllItems();
            ++e.handled;
        }
    }
}

//----------------------------------------------------------------------------//
bool ItemListbox::handle_PaneChildRemoved(const EventArgs& e)
{
    ItemListBase::handle_PaneChildRemoved(e);

    // get the window that's being removed
    const Window* w = static_cast<const WindowEventArgs&>(e).window;
    // Clear last selected pointer if that item was just removed.
    if (w == d_lastSelected)
        d_lastSelected = 0;

    return true;
}
//----------------------------------------------------------------------------//

} // end CEGUI namespace
