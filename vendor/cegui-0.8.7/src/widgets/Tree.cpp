/***********************************************************************
     created:   5-13-07
     author:    Jonathan Welch (Based on Code by David Durant)
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
#include "CEGUI/WindowManager.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/widgets/TreeItem.h"
#include "CEGUI/widgets/Tree.h"
#include "CEGUI/widgets/Scrollbar.h"
#include "CEGUI/widgets/Tooltip.h"

#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
const String Tree::EventNamespace("Tree");
const String Tree::WidgetTypeName("CEGUI/Tree");
/*************************************************************************
    Constants
*************************************************************************/
// event names
const String Tree::EventListContentsChanged( "ListContentsChanged" );
const String Tree::EventSelectionChanged( "SelectionChanged" );
const String Tree::EventSortModeChanged( "SortModeChanged" );
const String Tree::EventMultiselectModeChanged( "MultiselectModeChanged" );
const String Tree::EventVertScrollbarModeChanged( "VertScrollbarModeChanged" );
const String Tree::EventHorzScrollbarModeChanged( "HorzScrollbarModeChanged" );
const String Tree::EventBranchOpened( "BranchOpened" );
const String Tree::EventBranchClosed( "BranchClosed" );

/*************************************************************************
    Constructor for Tree base class.
*************************************************************************/
Tree::Tree(const String& type, const String& name) :
    Window(type, name),
    d_sorted(false),
    d_multiselect(false),
    d_forceVertScroll(false),
    d_forceHorzScroll(false),
    d_itemTooltips(false),
    d_vertScrollbar(0),
    d_horzScrollbar(0),
    d_lastSelected(0),
    d_openButtonImagery(0),
    d_closeButtonImagery(0),
    d_itemArea(0, 0, 0, 0)
{
    // add new events specific to tree.
    addTreeEvents();
    
    addTreeProperties();
}

void Tree::setLookNFeel(const String& look)
{
    Window::setLookNFeel( look );
    initialise();
}

/*************************************************************************
    Destructor for Tree base class.
*************************************************************************/
Tree::~Tree(void)
{
    resetList_impl();
}

/*************************************************************************
    Initialise the Window based object ready for use.
*************************************************************************/
void Tree::initialise(void)
{
    // get WidgetLookFeel for the assigned look.
    const WidgetLookFeel &wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
    const ImagerySection &tempOpenImagery = wlf.getImagerySection("OpenTreeButton");
    const ImagerySection &tempCloseImagery = wlf.getImagerySection("CloseTreeButton");
    d_openButtonImagery = &tempOpenImagery;
    d_closeButtonImagery = &tempCloseImagery;
    
    // create the component sub-widgets
    d_vertScrollbar = createVertScrollbar("__auto_vscrollbar__");
    d_horzScrollbar = createHorzScrollbar("__auto_hscrollbar__");
    
    addChild(d_vertScrollbar);
    addChild(d_horzScrollbar);
    
    d_vertScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&Tree::handle_scrollChange, this));
    d_horzScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&Tree::handle_scrollChange, this));
    
    configureScrollbars();
    performChildWindowLayout();
}

/*************************************************************************
    Return the number of selected items in the tree.
*************************************************************************/
size_t Tree::getSelectedCount(void) const
{
    size_t count = 0;
    
    for (size_t index = 0; index < d_listItems.size(); ++index)
    {
        if (d_listItems[index]->isSelected())
            count++;
    }
    
    return count;
}

/*************************************************************************
    Return a pointer to the first selected item.
*************************************************************************/
TreeItem* Tree::getFirstSelectedItem(void) const
{
    bool found_first = true;
    return getNextSelectedItemFromList(d_listItems, 0, found_first);
}

/*************************************************************************
    Return a pointer to the next selected item after item 'start_item'
*************************************************************************/
TreeItem* Tree::getNextSelected(const TreeItem* start_item) const
{
    bool found_first = (start_item == 0);
    return getNextSelectedItemFromList(d_listItems, start_item, found_first);
}

// Recursive!
TreeItem* Tree::getNextSelectedItemFromList(const LBItemList &itemList,
                                            const TreeItem* startItem,
                                            bool& foundStartItem) const
{
    size_t itemCount = itemList.size();
    
    for (size_t index = 0; index < itemCount; ++index)
    {
        if (foundStartItem == true)
        {
            // Already found the startItem, now looking for next selected item.
            if (itemList[index]->isSelected())
                return itemList[index];
        }
        else
        {
            // Still looking for startItem.  Is this it?
            if (itemList[index] == startItem)
                foundStartItem = true;
        }
        
        if (itemList[index]->getItemCount() > 0)
        {
            if (itemList[index]->getIsOpen())
            {
                TreeItem *foundSelectedTree;
                foundSelectedTree = getNextSelectedItemFromList(itemList[index]->getItemList(), startItem, foundStartItem);
                if (foundSelectedTree != 0)
                    return foundSelectedTree;
            }
        }
    }
    
    return 0;
}

// Recursive!
TreeItem* Tree::findItemWithTextFromList(const LBItemList &itemList, const String& text, const TreeItem* startItem, bool foundStartItem)
{
    size_t itemCount = itemList.size();
    
    for (size_t index = 0; index < itemCount; ++index)
    {
        if (foundStartItem == true)
        {
            // Already found the startItem, now looking for the actual text.
            if (itemList[index]->getText() == text)
                return itemList[index];
        }
        else
        {
            // Still looking for startItem.  Is this it?
            if (itemList[index] == startItem)
                foundStartItem = true;
        }
        
        if (itemList[index]->getItemCount() > 0)
        {
            // Search the current item's itemList regardless if it's open or not.
            TreeItem *foundSelectedTree;
            foundSelectedTree = findItemWithTextFromList(itemList[index]->getItemList(), text, startItem, foundStartItem);
            if (foundSelectedTree != 0)
                return foundSelectedTree;
        }
    }
    
    return 0;
}

/*************************************************************************
    Search the list for an item with the specified text
*************************************************************************/
TreeItem* Tree::findNextItemWithText(const String& text, const TreeItem* start_item)
{
    if (start_item == 0)
        return findItemWithTextFromList(d_listItems, text, 0, true);
    else
        return findItemWithTextFromList(d_listItems, text, start_item, false);
}


TreeItem* Tree::findFirstItemWithText(const String& text)
{
    return findItemWithTextFromList(d_listItems, text, 0, true);
}

// Recursive!
TreeItem* Tree::findItemWithIDFromList(const LBItemList &itemList, uint searchID, const TreeItem* startItem, bool foundStartItem)
{
    size_t itemCount = itemList.size();
    
    for (size_t index = 0; index < itemCount; ++index)
    {
        if (foundStartItem == true)
        {
            // Already found the startItem, now looking for the actual text.
            if (itemList[index]->getID() == searchID)
                return itemList[index];
        }
        else
        {
            // Still looking for startItem.  Is this it?
            if (itemList[index] == startItem)
                foundStartItem = true;
        }
        
        if (itemList[index]->getItemCount() > 0)
        {
            // Search the current item's itemList regardless if it's open or not.
            TreeItem *foundSelectedTree;
            foundSelectedTree = findItemWithIDFromList(itemList[index]->getItemList(), searchID, startItem, foundStartItem);
            if (foundSelectedTree != 0)
                return foundSelectedTree;
        }
    }
    
    return 0;
}

TreeItem* Tree::findNextItemWithID(uint searchID, const TreeItem* start_item)
{
    if (start_item == 0)
        return findItemWithIDFromList(d_listItems, searchID, 0, true);
    else
        return findItemWithIDFromList(d_listItems, searchID, start_item, false);
}

TreeItem* Tree::findFirstItemWithID(uint searchID)
{
    return findItemWithIDFromList(d_listItems, searchID, 0, true);
}

/*************************************************************************
    Return whether the specified TreeItem is in the List
*************************************************************************/
bool Tree::isTreeItemInList(const TreeItem* item) const
{
    return std::find(d_listItems.begin(), d_listItems.end(), item) != d_listItems.end();
}

/*************************************************************************
    Remove all items from the list.
*************************************************************************/
void Tree::resetList(void)
{
    if (resetList_impl())
    {
        WindowEventArgs args(this);
        onListContentsChanged(args);
    }
}

/*************************************************************************
    Add the given TreeItem to the list.
*************************************************************************/
void Tree::addItem(TreeItem* item)
{
    if (item != 0)
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
    Insert an item into the tree after a specified item already in
    the list.
*************************************************************************/
void Tree::insertItem(TreeItem* item, const TreeItem* position)
{
    // if the list is sorted, it's the same as a normal add operation
    if (isSortEnabled())
    {
        addItem(item);
    }
    else if (item != 0)
    {
        // establish ownership
        item->setOwnerWindow(this);
        
        // if position is NULL begin insert at begining, else insert after item 'position'
        LBItemList::iterator ins_pos;
        
        if (position == 0)
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
                    "the specified TreeItem for parameter 'position' is not attached to this Tree."));
            }
        }
        
        d_listItems.insert(ins_pos, item);
        
        WindowEventArgs args(this);
        onListContentsChanged(args);
    }
}

/*************************************************************************
    Removes the given item from the tree.
*************************************************************************/
void Tree::removeItem(const TreeItem* item)
{
    if (item != 0)
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
void Tree::clearAllSelections(void)
{
    // only fire events and update if we actually made any changes
    if (clearAllSelections_impl())
    {
        TreeEventArgs args(this);
        onSelectionChanged(args);
    }
}

/*************************************************************************
    Set whether the list should be sorted.
*************************************************************************/
void Tree::setSortingEnabled(bool setting)
{
    // only react if the setting will change
    if (d_sorted != setting)
    {
        d_sorted = setting;
        
        // if we are enabling sorting, we need to sort the list
        if (d_sorted)
        {
            std::sort(d_listItems.begin(), d_listItems.end(), &lbi_less);
        }
        
        WindowEventArgs args(this);
        onSortModeChanged(args);
    }
}

/*************************************************************************
    Set whether the list should allow multiple selections or just a
    single selection
*************************************************************************/
void Tree::setMultiselectEnabled(bool setting)
{
    // only react if the setting is changed
    if (d_multiselect != setting)
    {
        d_multiselect = setting;
        
        // if we change to single-select, deselect all except the first selected item.
        TreeEventArgs args(this);
        if ((!d_multiselect) && (getSelectedCount() > 1))
        {
            TreeItem* itm = getFirstSelectedItem();
            
            while ((itm = getNextSelected(itm)))
            {
                itm->setSelected(false);
            }
            
            onSelectionChanged(args);
        }
        
        onMultiselectModeChanged(args);
    }
}


void Tree::setItemTooltipsEnabled(bool setting)
{
    d_itemTooltips = setting;
}

/*************************************************************************
    Set whether the vertical scroll bar should always be shown.
*************************************************************************/
void Tree::setShowVertScrollbar(bool setting)
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
void Tree::setShowHorzScrollbar(bool setting)
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
    Set the select state of an attached TreeItem.
*************************************************************************/
void Tree::setItemSelectState(TreeItem* item, bool state)
{
    if (containsOpenItemRecursive(d_listItems, item))
    {
        TreeEventArgs args(this);
        args.treeItem = item;

        if (state && !d_multiselect)
            clearAllSelections_impl();

        item->setSelected(state);
        d_lastSelected = item->isSelected() ? item : 0;
        onSelectionChanged(args);
    }
    else
    {
        CEGUI_THROW(InvalidRequestException("the specified TreeItem is not "
            "attached to this Tree or not visible."));
    }
    }

//----------------------------------------------------------------------------//
bool Tree::containsOpenItemRecursive(const LBItemList& itemList, TreeItem* item)
{
    size_t itemCount = itemList.size();
    for (size_t index = 0; index < itemCount; ++index)
    {
        if (itemList[index] == item)
            return true;

        if (itemList[index]->getItemCount() > 0)
        {
            if (itemList[index]->getIsOpen())
            {
                if (containsOpenItemRecursive(itemList[index]->getItemList(), item))
                    return true;
            }
        }
    }

    return false;
}

/*************************************************************************
    Set the select state of an attached TreeItem.
*************************************************************************/
void Tree::setItemSelectState(size_t item_index, bool state)
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
            TreeEventArgs args(this);
            args.treeItem = d_listItems[item_index];
            onSelectionChanged(args);
        }
    }
    else
    {
        CEGUI_THROW(InvalidRequestException(
            "the value passed in the 'item_index' parameter is out of range for this Tree."));
    }
    
}

/*************************************************************************
    Causes the tree to update it's internal state after changes have
    been made to one or more attached TreeItem objects.
*************************************************************************/
void Tree::handleUpdatedItemData(void)
{
    configureScrollbars();
    invalidate();
}

/*************************************************************************
    Perform the actual rendering for this Window.
*************************************************************************/
void Tree::populateGeometryBuffer()
{
    // get the derived class to render general stuff before we handle the items
    cacheTreeBaseImagery();
    
    // Render list items
    Vector2f itemPos;
    float    widest = getWidestItemWidth();
    
    // calculate position of area we have to render into
    //Rect itemsArea(getTreeRenderArea());
    //Rect itemsArea(0,0,500,500);
    
    // set up some initial positional details for items
    itemPos.d_x = d_itemArea.left() - d_horzScrollbar->getScrollPosition();
    itemPos.d_y = d_itemArea.top() - d_vertScrollbar->getScrollPosition();
    
    drawItemList(d_listItems, d_itemArea, widest, itemPos, *d_geometry,
                 getEffectiveAlpha());
}

// Recursive!
void Tree::drawItemList(LBItemList& itemList, Rectf& itemsArea, float widest,
                        Vector2f& itemPos, GeometryBuffer& geometry, float alpha)
{
    if (itemList.empty())
        return;
    
    // loop through the items
    Sizef   itemSize;
    Rectf   itemClipper, itemRect;
    size_t   itemCount = itemList.size();
    bool     itemIsVisible;
    for (size_t i = 0; i < itemCount; ++i)
    {
        itemSize.d_height = itemList[i]->getPixelSize().d_height;
        
        // allow item to have full width of box if this is wider than items
        itemSize.d_width = ceguimax(itemsArea.getWidth(), widest);
        
        // calculate destination area for this item.
        itemRect.left(itemPos.d_x);
        itemRect.top(itemPos.d_y);
        itemRect.setSize(itemSize);
        itemClipper = itemRect.getIntersection(itemsArea);
        itemRect.d_min.d_x += 20; // start text past open/close buttons
        
        if (itemClipper.getHeight() > 0)
        {
            itemIsVisible = true;
            itemList[i]->draw(geometry, itemRect, alpha, &itemClipper);
        }
        else
        {
            itemIsVisible = false;
        }
        
        // Process this item's list if it has items in it.
        if (itemList[i]->getItemCount() > 0)
        {
            Rectf buttonRenderRect;
            buttonRenderRect.left(itemPos.d_x);
            buttonRenderRect.right(buttonRenderRect.left() + 10);
            buttonRenderRect.top(itemPos.d_y);
            buttonRenderRect.bottom(buttonRenderRect.top() + 10);
            itemList[i]->setButtonLocation(buttonRenderRect);
            
            if (itemList[i]->getIsOpen())
            {
                // Draw the Close button
                if (itemIsVisible)
                    d_closeButtonImagery->render(*this, buttonRenderRect, 0, &itemClipper);
                
                // update position ready for next item
                itemPos.d_y += itemSize.d_height;
                
                itemPos.d_x += 20;
                drawItemList(itemList[i]->getItemList(), itemsArea, widest,
                             itemPos, geometry, alpha);
                itemPos.d_x -= 20;
            }
            else
            {
                // Draw the Open button
                if (itemIsVisible)
                    d_openButtonImagery->render(*this, buttonRenderRect, 0, &itemClipper);
                
                // update position ready for next item
                itemPos.d_y += itemSize.d_height;
            }
        }
        else
        {
            // update position ready for next item
            itemPos.d_y += itemSize.d_height;
        }
    }
    
    // Successfully drew all items, so vertical scrollbar not needed.
    //   setShowVertScrollbar(false);
}

#define HORIZONTAL_STEP_SIZE_DIVISOR   20.0f

/*************************************************************************
    display required integrated scroll bars according to current state
    of the tree and update their values.
*************************************************************************/
void Tree::configureScrollbars(void)
{
    Rectf renderArea(getTreeRenderArea());
    
    //This is becuase CEGUI IS GAY! and fires events before the item is initialized
    if(!d_vertScrollbar)
        d_vertScrollbar = createVertScrollbar("__auto_vscrollbar__");
    if(!d_horzScrollbar)
        d_horzScrollbar = createHorzScrollbar("__auto_hscrollbar__");
    
    float totalHeight = getTotalItemsHeight();
    float widestItem  = getWidestItemWidth() + 20;
    
    //
    // First show or hide the scroll bars as needed (or requested)
    //
    // show or hide vertical scroll bar as required (or as specified by option)
    if ((totalHeight > renderArea.getHeight()) || d_forceVertScroll)
    {
        d_vertScrollbar->show();
        renderArea.d_max.d_x -= d_vertScrollbar->getWidth().d_offset + d_vertScrollbar->getXPosition().d_offset;
        // show or hide horizontal scroll bar as required (or as specified by option)
        if ((widestItem > renderArea.getWidth()) || d_forceHorzScroll)
        {
            d_horzScrollbar->show();
            renderArea.d_max.d_y -= d_horzScrollbar->getHeight().d_offset;
        }
        else
        {
            d_horzScrollbar->hide();
            d_horzScrollbar->setScrollPosition(0);
        }
    }
    else
    {
        // show or hide horizontal scroll bar as required (or as specified by option)
        if ((widestItem > renderArea.getWidth()) || d_forceHorzScroll)
        {
            d_horzScrollbar->show();
            renderArea.d_max.d_y -= d_vertScrollbar->getHeight().d_offset;
            
            // show or hide vertical scroll bar as required (or as specified by option)
            if ((totalHeight > renderArea.getHeight()) || d_forceVertScroll)
            {
                d_vertScrollbar->show();
                //            renderArea.d_right -= d_vertScrollbar->getAbsoluteWidth();
                renderArea.d_max.d_x -= d_vertScrollbar->getWidth().d_offset;
            }
            else
            {
                d_vertScrollbar->hide();
                d_vertScrollbar->setScrollPosition(0);
            }
        }
        else
        {
            d_vertScrollbar->hide();
            d_vertScrollbar->setScrollPosition(0);
            d_horzScrollbar->hide();
            d_horzScrollbar->setScrollPosition(0);
        }
    }
    
    //
    // Set up scroll bar values
    //
    
    float itemHeight;
    if (!d_listItems.empty())
        itemHeight = d_listItems[0]->getPixelSize().d_height;
    else
        itemHeight = 10;
    
    d_vertScrollbar->setDocumentSize(totalHeight);
    d_vertScrollbar->setPageSize(renderArea.getHeight());
    d_vertScrollbar->setStepSize(ceguimax(1.0f, renderArea.getHeight() / itemHeight));
    d_vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition());
    
    d_horzScrollbar->setDocumentSize(widestItem + d_vertScrollbar->getWidth().d_offset);
    //   d_horzScrollbar->setDocumentSize(widestItem + d_vertScrollbar->getAbsoluteWidth());
    d_horzScrollbar->setPageSize(renderArea.getWidth());
    d_horzScrollbar->setStepSize(ceguimax(1.0f, renderArea.getWidth() / HORIZONTAL_STEP_SIZE_DIVISOR));
    d_horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition());
}

/*************************************************************************
    select all strings between positions 'start' and 'end' (inclusive)
*************************************************************************/
void Tree::selectRange(size_t start, size_t end)
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
float Tree::getTotalItemsHeight(void) const
{
    float heightSum = 0;
    
    getTotalItemsInListHeight(d_listItems, &heightSum);
    return heightSum;
}

// Recursive!
void Tree::getTotalItemsInListHeight(const LBItemList &itemList, float *heightSum) const
{
    size_t itemCount = itemList.size();
    for (size_t index = 0; index < itemCount; ++index)
    {
        *heightSum += itemList[index]->getPixelSize().d_height;
        if (itemList[index]->getIsOpen() && (itemList[index]->getItemCount() > 0))
            getTotalItemsInListHeight(itemList[index]->getItemList(), heightSum);
    }
}

/*************************************************************************
    Return the width of the widest item, including any white space to the left
    due to indenting.
*************************************************************************/
float Tree::getWidestItemWidth(void) const
{
    float widest = 0;
    
    getWidestItemWidthInList(d_listItems, 0, &widest);
    return widest;
}

// Recursive!
void Tree::getWidestItemWidthInList(const LBItemList &itemList, int itemDepth, float *widest) const
{
    size_t itemCount = itemList.size();
    for (size_t index = 0; index < itemCount; ++index)
    {
        Rectf buttonLocation = itemList[index]->getButtonLocation();
        float thisWidth = itemList[index]->getPixelSize().d_width +
        buttonLocation.getWidth() +
        (d_horzScrollbar->getScrollPosition() / HORIZONTAL_STEP_SIZE_DIVISOR) +
        (itemDepth * 20);
        
        if (thisWidth > *widest)
            *widest = thisWidth;
        
        if (itemList[index]->getIsOpen() && (itemList[index]->getItemCount() > 0))
            getWidestItemWidthInList(itemList[index]->getItemList(), itemDepth + 1, widest);
    }
}

/*************************************************************************
    Clear the selected state for all items (implementation)
*************************************************************************/
bool Tree::clearAllSelections_impl(void)
{
    return clearAllSelectionsFromList(d_listItems);
}

// Recursive!
bool Tree::clearAllSelectionsFromList(const LBItemList &itemList)
{
    // flag used so we can track if we did anything.
    bool modified = false;
    
    for (size_t index = 0; index < itemList.size(); ++index)
    {
        if (itemList[index]->isSelected())
        {
            itemList[index]->setSelected(false);
            modified = true;
        }
        
        if (itemList[index]->getItemCount() > 0)
        {
            bool modifiedSubList = clearAllSelectionsFromList(itemList[index]->getItemList());
            if (modifiedSubList)
                modified = true;
        }
    }
    
    return modified;
}

/*************************************************************************
    Return the TreeItem under the given window local pixel co-ordinate.
*************************************************************************/
TreeItem* Tree::getItemAtPoint(const Vector2f& pt) const
{
    Rectf renderArea(getTreeRenderArea());
    
    // point must be within the rendering area of the Tree.
    if (renderArea.isPointInRect(pt))
    {
        float y = renderArea.top() - d_vertScrollbar->getScrollPosition();
        
        // test if point is above first item
        if (pt.d_y >= y)
            return getItemFromListAtPoint(d_listItems, &y, pt);
    }
    
    return 0;
}

// Recursive!
TreeItem* Tree::getItemFromListAtPoint(const LBItemList &itemList, float *bottomY, const Vector2f& pt) const
{
    size_t itemCount = itemList.size();
    
    for (size_t i = 0; i < itemCount; ++i)
    {
        *bottomY += itemList[i]->getPixelSize().d_height;
        if (pt.d_y < *bottomY)
            return itemList[i];
        
        if (itemList[i]->getItemCount() > 0)
        {
            if (itemList[i]->getIsOpen())
            {
                TreeItem *foundPointedAtTree;
                foundPointedAtTree = getItemFromListAtPoint(itemList[i]->getItemList(), bottomY, pt);
                if (foundPointedAtTree != 0)
                    return foundPointedAtTree;
            }
        }
    }
    
    return 0;
}

/*************************************************************************
    Add tree specific events
*************************************************************************/
void Tree::addTreeEvents(void)
{
    addEvent(EventListContentsChanged);
    addEvent(EventSelectionChanged);
    addEvent(EventSortModeChanged);
    addEvent(EventMultiselectModeChanged);
    addEvent(EventVertScrollbarModeChanged);
    addEvent(EventHorzScrollbarModeChanged);
    addEvent(EventBranchOpened);
    addEvent(EventBranchClosed);
}

/*************************************************************************
    Handler called internally when the list contents are changed
*************************************************************************/
void Tree::onListContentsChanged(WindowEventArgs& e)
{
    configureScrollbars();
    invalidate();
    fireEvent(EventListContentsChanged, e, EventNamespace);
}

/*************************************************************************
    Handler called internally when the currently selected item or items
    changes.
*************************************************************************/
void Tree::onSelectionChanged(TreeEventArgs& e)
{
    invalidate();
    fireEvent(EventSelectionChanged, e, EventNamespace);
}

/*************************************************************************
    Handler called internally when the sort mode setting changes.
*************************************************************************/
void Tree::onSortModeChanged(WindowEventArgs& e)
{
    invalidate();
    fireEvent(EventSortModeChanged, e, EventNamespace);
}

/*************************************************************************
    Handler called internally when the multi-select mode setting changes.
*************************************************************************/
void Tree::onMultiselectModeChanged(WindowEventArgs& e)
{
    fireEvent(EventMultiselectModeChanged, e, EventNamespace);
}

/*************************************************************************
    Handler called internally when the forced display of the vertical
    scroll bar setting changes.
*************************************************************************/
void Tree::onVertScrollbarModeChanged(WindowEventArgs& e)
{
    invalidate();
    fireEvent(EventVertScrollbarModeChanged, e, EventNamespace);
}

/*************************************************************************
    Handler called internally when the forced display of the horizontal
    scroll bar setting changes.
*************************************************************************/
void Tree::onHorzScrollbarModeChanged(WindowEventArgs& e)
{
    invalidate();
    fireEvent(EventHorzScrollbarModeChanged, e, EventNamespace);
}

/*************************************************************************
    Handler called internally when the forced display of the horizontal
    scroll bar setting changes.
*************************************************************************/
void Tree::onBranchOpened(TreeEventArgs& e)
{
    invalidate();
    fireEvent(EventBranchOpened, e, EventNamespace);
}

/*************************************************************************
    Handler called internally when the forced display of the horizontal
    scroll bar setting changes.
*************************************************************************/
void Tree::onBranchClosed(TreeEventArgs& e)
{
    invalidate();
    fireEvent(EventBranchClosed, e, EventNamespace);
}

/*************************************************************************
    Handler for when we are sized
*************************************************************************/
void Tree::onSized(ElementEventArgs& e)
{
    // base class handling
    Window::onSized(e);
    
    configureScrollbars();
    
    ++e.handled;
}

/*************************************************************************
    Handler for when mouse button is pressed
*************************************************************************/
void Tree::onMouseButtonDown(MouseEventArgs& e)
{
    // base class processing
    // populateGeometryBuffer();
    Window::onMouseButtonDown(e);
    
    if (e.button == LeftButton)
    {
        //bool modified = false;
        
        Vector2f localPos(CoordConverter::screenToWindow(*this, e.position));
        //      Point localPos(screenToWindow(e.position));
        
        TreeItem* item = getItemAtPoint(localPos);
        
        if (item != 0)
        {
            //modified = true;
            TreeEventArgs args(this);
            args.treeItem = item;
            populateGeometryBuffer();
            Rectf buttonLocation = item->getButtonLocation();
            if ((localPos.d_x >= buttonLocation.left()) && (localPos.d_x <= buttonLocation.right()) &&
                (localPos.d_y >= buttonLocation.top()) && (localPos.d_y <= buttonLocation.bottom()))
            {
                item->toggleIsOpen();
                if (item->getIsOpen())
                {
                    TreeItem *lastItemInList = item->getTreeItemFromIndex(item->getItemCount() - 1);
                    ensureItemIsVisible(lastItemInList);
                    ensureItemIsVisible(item);
                    onBranchOpened(args);
                }
                else
                {
                    onBranchClosed(args);
                }
                
                // Update the item screen locations, needed to update the scrollbars.
                //	populateGeometryBuffer();
                
                // Opened or closed a tree branch, so must update scrollbars.
                configureScrollbars();
            }
            else
            {
                // clear old selections if no control key is pressed or if multi-select is off
                if (!(e.sysKeys & Control) || !d_multiselect)
                    clearAllSelections_impl();
                
                // select range or item, depending upon keys and last selected item
#if 0 // TODO: fix this
                if (((e.sysKeys & Shift) && (d_lastSelected != 0)) && d_multiselect)
                    selectRange(getItemIndex(item), getItemIndex(d_lastSelected));
                else
#endif
                    item->setSelected(item->isSelected() ^ true);
                
                // update last selected item
                d_lastSelected = item->isSelected() ? item : 0;
                onSelectionChanged(args);
            }
        }
        else
        {
            // clear old selections if no control key is pressed or if multi-select is off
            if (!(e.sysKeys & Control) || !d_multiselect)
            {
                if (clearAllSelections_impl())
                {
                    // Changes to the selections were actually made
                    TreeEventArgs args(this);
                    args.treeItem = item;
                    onSelectionChanged(args);
                }
            }
        }
        
        
        ++e.handled;
    }
}

/*************************************************************************
    Handler for mouse wheel changes
*************************************************************************/
void Tree::onMouseWheel(MouseEventArgs& e)
{
    // base class processing.
    Window::onMouseWheel(e);
    
    if (d_vertScrollbar->isEffectiveVisible() && (d_vertScrollbar->getDocumentSize() > d_vertScrollbar->getPageSize()))
        d_vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition() + d_vertScrollbar->getStepSize() * -e.wheelChange);
    else if (d_horzScrollbar->isEffectiveVisible() && (d_horzScrollbar->getDocumentSize() > d_horzScrollbar->getPageSize()))
        d_horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition() + d_horzScrollbar->getStepSize() * -e.wheelChange);
    
    ++e.handled;
}

/*************************************************************************
    Handler for mouse movement
*************************************************************************/
void Tree::onMouseMove(MouseEventArgs& e)
{
    if (d_itemTooltips)
    {
        static TreeItem* lastItem = 0;
        
        Vector2f posi(CoordConverter::screenToWindow(*this, e.position));
        //      Point posi = relativeToAbsolute(CoordConverter::screenToWindow(*this, e.position));
        TreeItem* item = getItemAtPoint(posi);
        if (item != lastItem)
        {
            if (item != 0)
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

// Recursive!
bool Tree::getHeightToItemInList(const LBItemList &itemList, const TreeItem *treeItem, int itemDepth, float *height) const
{
    size_t itemCount = itemList.size();
    for (size_t index = 0; index < itemCount; ++index)
    {
        if (treeItem == itemList[index])
            return true;
        
        *height += itemList[index]->getPixelSize().d_height;
        
        if (itemList[index]->getIsOpen() && (itemList[index]->getItemCount() > 0))
        {
            if (getHeightToItemInList(itemList[index]->getItemList(), treeItem, itemDepth + 1, height))
                return true;
        }
    }
    
    return false;
}

/*************************************************************************
    Ensure the specified item is visible within the tree.
*************************************************************************/
void Tree::ensureItemIsVisible(const TreeItem *treeItem)
{
    if (!treeItem)
        return;
    
    float top = 0;
    if (!getHeightToItemInList(d_listItems, treeItem, 0, &top))
        return;  // treeItem wasn't found by getHeightToItemInList
    
    // calculate height to bottom of item
    float bottom = top + treeItem->getPixelSize().d_height;
    
    // account for current scrollbar value
    const float currPos = d_vertScrollbar->getScrollPosition();
    top      -= currPos;
    bottom   -= currPos;
    
    const float listHeight = getTreeRenderArea().getHeight();
    
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
}

/*************************************************************************
    Return whether the vertical scroll bar is always shown.
*************************************************************************/
bool Tree::isVertScrollbarAlwaysShown(void) const
{
    return d_forceVertScroll;
}

/*************************************************************************
    Return whether the horizontal scroll bar is always shown.
*************************************************************************/
bool Tree::isHorzScrollbarAlwaysShown(void) const
{
    return d_forceHorzScroll;
}

/*************************************************************************
    Add properties for this class
*************************************************************************/
void Tree::addTreeProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;

    CEGUI_DEFINE_PROPERTY(Tree, bool,
        "Sort", "Property to get/set the sort setting of the tree.  "
        "Value is either \"true\" or \"false\".",
        &Tree::setSortingEnabled, &Tree::isSortEnabled, false /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(Tree, bool,
        "MultiSelect", "Property to get/set the multi-select setting of the tree.  "
        "Value is either \"true\" or \"false\".",
        &Tree::setMultiselectEnabled, &Tree::isMultiselectEnabled, false /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(Tree, bool,
        "ForceVertScrollbar", "Property to get/set the 'always show' setting for the vertical scroll "
        "bar of the tree.  Value is either \"true\" or \"false\".",
        &Tree::setShowVertScrollbar, &Tree::isVertScrollbarAlwaysShown, false /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(Tree, bool,
        "ForceHorzScrollbar", "Property to get/set the 'always show' setting for the horizontal "
        "scroll bar of the tree.  Value is either \"true\" or \"false\".",
        &Tree::setShowHorzScrollbar, &Tree::isHorzScrollbarAlwaysShown, false /* TODO: Inconsistency */
    );
    
    CEGUI_DEFINE_PROPERTY(Tree, bool,
        "ItemTooltips", "Property to access the show item tooltips setting of the tree.  "
        "Value is either \"true\" or \"false\".",
        &Tree::setItemTooltipsEnabled, &Tree::isItemTooltipsEnabled, false /* TODO: Inconsistency */
    );
}

/*************************************************************************
    Remove all items from the list.
*************************************************************************/
bool Tree::resetList_impl(void)
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
bool Tree::handle_scrollChange(const EventArgs&)
{
    // simply trigger a redraw of the Tree.
    invalidate();
    return true;
}

bool Tree::handleFontRenderSizeChange(const EventArgs& args)
{
    bool res = Window::handleFontRenderSizeChange(args);

    if (!res)
    {
        const Font* const font = static_cast<const FontEventArgs&>(args).font;

        for (size_t i = 0; i < getItemCount(); ++i)
            res |= d_listItems[i]->handleFontRenderSizeChange(font);

        if (res)
            invalidate();
    }

    return res;
}

//////////////////////////////////////////////////////////////////////////
/*************************************************************************
    Functions used for predicates in std algorithms
*************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*************************************************************************
    used for < comparisons between TreeItem pointers
*************************************************************************/
bool lbi_less(const TreeItem* a, const TreeItem* b)
{
    return *a < *b;
}

/*************************************************************************
    used for > comparisons between TreeItem pointers
*************************************************************************/
bool lbi_greater(const TreeItem* a, const TreeItem* b)
{
    return *a > *b;
}

} // End of  CEGUI namespace section
