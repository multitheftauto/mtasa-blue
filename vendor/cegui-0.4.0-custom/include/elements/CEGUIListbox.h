/************************************************************************
	filename: 	CEGUIListbox.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for Listbox widget
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
#ifndef _CEGUIListbox_h_
#define _CEGUIListbox_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUIListboxProperties.h"
#include <vector>


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Base class for standard Listbox widget.
*/
class CEGUIEXPORT Listbox : public Window
{
public:
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Constants
	*************************************************************************/
	// event names
	static const String EventListContentsChanged;			//!< Event triggered when the contents of the list is changed.
	static const String EventSelectionChanged;			//!< Event triggered when there is a change to the currently selected item(s).
	static const String EventSortModeChanged;				//!< Event triggered when the sort mode setting changes.
	static const String EventMultiselectModeChanged;		//!< Event triggered when the multi-select mode setting changes.
	static const String EventVertScrollbarModeChanged;	//!< Event triggered when the vertical scroll bar 'force' setting changes.
	static const String EventHorzScrollbarModeChanged;	//!< Event triggered when the horizontal scroll bar 'force' setting changes.


	/*************************************************************************
		Accessor Methods
	*************************************************************************/
	/*!
	\brief
		Return number of items attached to the list box

	\return
		the number of items currently attached to this list box.
	*/
	size_t	getItemCount(void) const		{return d_listItems.size();}

	
	/*!
	\brief
		Return the number of selected items in the list box.

	\return
		Total number of attached items that are in the selected state.
	*/
	size_t	getSelectedCount(void) const;


	/*!
	\brief
		Return a pointer to the first selected item.

	\return
		Pointer to a ListboxItem based object that is the first selected item in the list.  will return NULL if
		no item is selected.
	*/
	ListboxItem*	getFirstSelectedItem(void) const;


	/*!
	\brief
		Return a pointer to the next selected item after item \a start_item

	\param start_item
		Pointer to the ListboxItem where the search for the next selected item is to begin.  If this
		parameter is NULL, the search will begin with the first item in the list box.

	\return
		Pointer to a ListboxItem based object that is the next selected item in the list after
		the item specified by \a start_item.  Will return NULL if no further items were selected.

	\exception	InvalidRequestException	thrown if \a start_item is not attached to this list box.
	*/
	ListboxItem*	getNextSelected(const ListboxItem* start_item) const;


	/*!
	\brief
		Return the item at index position \a index.

	\param index
		Zero based index of the item to be returned.

	\return
		Pointer to the ListboxItem at index position \a index in the list box.

	\exception	InvalidRequestException	thrown if \a index is out of range.
	*/
	ListboxItem*	getListboxItemFromIndex(size_t index) const;


	/*!
	\brief
		Return the index of ListboxItem \a item

	\param item
		Pointer to a ListboxItem whos zero based index is to be returned.

	\return
		Zero based index indicating the position of ListboxItem \a item in the list box.

	\exception	InvalidRequestException	thrown if \a item is not attached to this list box.
	*/
	size_t	getItemIndex(const ListboxItem* item) const;


	/*!
	\brief
		return whether list sorting is enabled

	\return
		true if the list is sorted, false if the list is not sorted
	*/
	bool	isSortEnabled(void) const		{return d_sorted;}

	/*!
	\brief
		return whether multi-select is enabled

	\return
		true if multi-select is enabled, false if multi-select is not enabled.
	*/
	bool	isMultiselectEnabled(void) const	{return d_multiselect;}

	bool	isItemTooltipsEnabled(void) const	{return d_itemTooltips;}

	/*!
	\brief
		return whether the string at index position \a index is selected

	\param index
		Zero based index of the item to be examined.

	\return
		true if the item at \a index is selected, false if the item at \a index is not selected.

	\exception	InvalidRequestException	thrown if \a index is out of range.
	*/
	bool	isItemSelected(size_t index) const;


	/*!
	\brief
		Search the list for an item with the specified text

	\param text
		String object containing the text to be searched for.

	\param start_item
		ListboxItem where the search is to begin, the search will not include \a item.  If \a item is
		NULL, the search will begin from the first item in the list.

	\return
		Pointer to the first ListboxItem in the list after \a item that has text matching \a text.  If
		no item matches the criteria NULL is returned.

	\exception	InvalidRequestException	thrown if \a item is not attached to this list box.
	*/
	ListboxItem*	findItemWithText(const String& text, const ListboxItem* start_item);


	/*!
	\brief
		Return whether the specified ListboxItem is in the List

	\return
		true if ListboxItem \a item is in the list, false if ListboxItem \a item is not in the list.
	*/
	bool	isListboxItemInList(const ListboxItem* item) const;


	/*!
	\brief
		Return whether the vertical scroll bar is always shown.

	\return
		- true if the scroll bar will always be shown even if it is not required.
		- false if the scroll bar will only be shown when it is required.
	*/
	bool	isVertScrollbarAlwaysShown(void) const;


	/*!
	\brief
		Return whether the horizontal scroll bar is always shown.

	\return
		- true if the scroll bar will always be shown even if it is not required.
		- false if the scroll bar will only be shown when it is required.
	*/
	bool	isHorzScrollbarAlwaysShown(void) const;


	/*************************************************************************
		Manipulator Methods
	*************************************************************************/
	/*!
	\brief
		Initialise the Window based object ready for use.

	\note
		This must be called for every window created.  Normally this is handled automatically by the WindowFactory for each Window type.

	\return
		Nothing
	*/
	virtual void	initialise(void);


	/*!
	\brief
		Remove all items from the list.

		Note that this will cause 'AutoDelete' items to be deleted.
	*/
	void	resetList(void);


	/*!
	\brief
		Add the given ListboxItem to the list.

	\param item
		Pointer to the ListboxItem to be added to the list.  Note that it is the passed object that is added to the
		list, a copy is not made.  If this parameter is NULL, nothing happens.

	\return
		Nothing.
	*/
	void	addItem(ListboxItem* item);


	/*!
	\brief
		Insert an item into the list box after a specified item already in the list.

		Note that if the list is sorted, the item may not end up in the requested position.

	\param item
		Pointer to the ListboxItem to be inserted.  Note that it is the passed object that is added to the
		list, a copy is not made.  If this parameter is NULL, nothing happens.

	\param position
		Pointer to a ListboxItem that \a item is to be inserted after.  If this parameter is NULL, the item is
		inserted at the start of the list.

	\return
		Nothing.
	
	\exception InvalidRequestException	thrown if no ListboxItem \a position is attached to this list box.
	*/
	void	insertItem(ListboxItem* item, const ListboxItem* position);


	/*!
	\brief
		Removes the given item from the list box.  If the item is has the auto delete state set, the item will be deleted.

	\param item
		Pointer to the ListboxItem that is to be removed.  If \a item is not attached to this list box then nothing
		will happen.

	\return
		Nothing.
	*/
	void	removeItem(const ListboxItem* item);


	/*!
	\brief
		Clear the selected state for all items.

	\return
		Nothing.
	*/
	void	clearAllSelections(void);


	/*!
	\brief
		Set whether the list should be sorted.

	\param setting
		true if the list should be sorted, false if the list should not be sorted.

	\return
		Nothing.
	*/
	void	setSortingEnabled(bool setting);

	
	/*!
	\brief
		Set whether the list should allow multiple selections or just a single selection

	\param  setting
		true if the widget should allow multiple items to be selected, false if the widget should only allow
		a single selection.

	\return
		Nothing.
	*/
	void	setMultiselectEnabled(bool setting);


	/*!
	\brief
		Set whether the vertical scroll bar should always be shown.

	\param setting
		true if the vertical scroll bar should be shown even when it is not required.  false if the vertical
		scroll bar should only be shown when it is required.

	\return
		Nothing.
	*/
	void	setShowVertScrollbar(bool setting);


	/*!
	\brief
		Set whether the horizontal scroll bar should always be shown.

	\param setting
		true if the horizontal scroll bar should be shown even when it is not required.  false if the horizontal
		scroll bar should only be shown when it is required.

	\return
		Nothing.
	*/
	void	setShowHorzScrollbar(bool setting);

	void	setItemTooltipsEnabled(bool setting);
	/*!
	\brief
		Set the select state of an attached ListboxItem.

		This is the recommended way of selecting and deselecting items attached to a list box as it respects the
		multi-select mode setting.  It is possible to modify the setting on ListboxItems directly, but that approach
		does not respect the settings of the list box.

	\param item
		The ListboxItem to be affected.  This item must be attached to the list box.

	\param state
		true to select the item, false to de-select the item.

	\return
		Nothing.
	
	\exception	InvalidRequestException	thrown if \a item is not attached to this list box.
	*/
	void	setItemSelectState(ListboxItem* item, bool state);


	/*!
	\brief
		Set the select state of an attached ListboxItem.

		This is the recommended way of selecting and deselecting items attached to a list box as it respects the
		multi-select mode setting.  It is possible to modify the setting on ListboxItems directly, but that approach
		does not respect the settings of the list box.

	\param item_index
		The zero based index of the ListboxItem to be affected.  This must be a valid index (0 <= index < getItemCount())

	\param state
		true to select the item, false to de-select the item.

	\return
		Nothing.
	
	\exception	InvalidRequestException	thrown if \a item_index is out of range for the list box
	*/
	void	setItemSelectState(size_t item_index, bool state);


	/*!
	\brief
		Causes the list box to update it's internal state after changes have been made to one or more
		attached ListboxItem objects.

		Client code must call this whenever it has made any changes to ListboxItem objects already attached to the
		list box.  If you are just adding items, or removed items to update them prior to re-adding them, there is
		no need to call this method.

	\return
		Nothing.
	*/
	void	handleUpdatedItemData(void);


	/*!
	\brief
		Ensure the item at the specified index is visible within the list box.

	\param item_index
		Zero based index of the item to be made visible in the list box.  If this value is out of range, the
		list is always scrolled to the bottom.

	\return
		Nothing.
	*/
	void	ensureItemIsVisible(size_t item_index);


	/*!
	\brief
		Ensure the item at the specified index is visible within the list box.

	\param item
		Pointer to the ListboxItem to be made visible in the list box.

	\return
		Nothing.

	\exception	InvalidRequestException	thrown if \a item is not attached to this list box.
	*/
	void	ensureItemIsVisible(const ListboxItem* item);


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for Listbox base class.
	*/
	Listbox(const String& type, const String& name);


	/*!
	\brief
		Destructor for Listbox base class.
	*/
	virtual ~Listbox(void);


protected:
	/*************************************************************************
		Abstract Implementation Functions (must be provided by derived class)
	*************************************************************************/
	/*!
	\brief
		Return a Rect object describing, in un-clipped pixels, the window relative area
		that is to be used for rendering list items.

	\return
		Rect object describing the area of the Window to be used for rendering
		list box items.
	*/
	virtual	Rect	getListRenderArea(void) const		= 0;


	/*!
	\brief
		create and return a pointer to a Scrollbar widget for use as vertical scroll bar

	\param name
	   String holding the name to be given to the created widget component.

	\return
		Pointer to a Scrollbar to be used for scrolling the list vertically.
	*/
	virtual Scrollbar*	createVertScrollbar(const String& name) const		= 0;
 

	/*!
	\brief
		create and return a pointer to a Scrollbar widget for use as horizontal scroll bar

	\param name
	   String holding the name to be given to the created widget component.

	\return
		Pointer to a Scrollbar to be used for scrolling the list horizontally.
	*/
	virtual Scrollbar*	createHorzScrollbar(const String& name) const		= 0;


	/*!
	\brief
		Perform caching of the widget control frame and other 'static' areas.  This
		method should not render the actual items.  Note that the items are typically
		rendered to layer 3, other layers can be used for rendering imagery behind and
		infront of the items.

	\return
		Nothing.
	*/
	virtual	void cacheListboxBaseImagery() = 0;


	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Add list box specific events
	*/
	void	addListboxEvents(void);


	/*!
	\brief
		display required integrated scroll bars according to current state of the list box and update their values.
	*/
	void	configureScrollbars(void);

	/*!
	\brief
		select all strings between positions \a start and \a end.  (inclusive)
		including \a end.
	*/
	void	selectRange(size_t start, size_t end);


	/*!
	\brief
		Return the sum of all item heights
	*/
	float	getTotalItemsHeight(void) const;


	/*!
	\brief
		Return the width of the widest item
	*/
	float	getWidestItemWidth(void) const;


	/*!
	\brief
		Clear the selected state for all items (implementation)

	\return
		true if some selections were cleared, false nothing was changed.
	*/
	bool	clearAllSelections_impl(void);


	/*!
	\brief
		Return the ListboxItem under the given window local pixel co-ordinate.

	\return
		ListboxItem that is under window pixel co-ordinate \a pt, or NULL if no
		item is under that position.
	*/
	ListboxItem*	getItemAtPoint(const Point& pt) const;


	/*!
	\brief
		Remove all items from the list.

	\note
		Note that this will cause 'AutoDelete' items to be deleted.

	\return
		- true if the list contents were changed.
		- false if the list contents were not changed (list already empty).
	*/
	bool	resetList_impl(void);


	/*!
	\brief
		Return whether this window was inherited from the given class name at some point in the inheritance heirarchy.

	\param class_name
		The class name that is to be checked.

	\return
		true if this window was inherited from \a class_name. false if not.
	*/
	virtual bool	testClassName_impl(const String& class_name) const
	{
		if (class_name==(const utf8*)"Listbox")	return true;
		return Window::testClassName_impl(class_name);
	}

	/*!
	\brief
	   Internal handler that is triggered when the user interacts with the scrollbars.
    */
    bool handle_scrollChange(const EventArgs& args);

    // overridden from Window base class.
    void populateRenderCache();


	/*************************************************************************
		New event handlers
	*************************************************************************/
	/*!
	\brief
		Handler called internally when the list contents are changed
	*/
	virtual	void	onListContentsChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the currently selected item or items changes.
	*/
	virtual	void	onSelectionChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the sort mode setting changes.
	*/
	virtual	void	onSortModeChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the multi-select mode setting changes.
	*/
	virtual	void	onMultiselectModeChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the forced display of the vertical scroll bar setting changes.
	*/
	virtual	void	onVertScrollbarModeChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the forced display of the horizontal scroll bar setting changes.
	*/
	virtual	void	onHorzScrollbarModeChanged(WindowEventArgs& e);


	/*************************************************************************
		Overridden Event handlers
	*************************************************************************/
	virtual void	onSized(WindowEventArgs& e);
	virtual void	onMouseButtonDown(MouseEventArgs& e);
	virtual	void	onMouseWheel(MouseEventArgs& e);
	virtual void	onMouseMove(MouseEventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	typedef	std::vector<ListboxItem*>	LBItemList;
	bool	d_sorted;				//!< true if list is sorted
	bool	d_multiselect;			//!< true if multi-select is enabled
	bool	d_forceVertScroll;		//!< true if vertical scrollbar should always be displayed
	bool	d_forceHorzScroll;		//!< true if horizontal scrollbar should always be displayed
	bool	d_itemTooltips;			//!< true if each item should have an individual tooltip
	Scrollbar*	d_vertScrollbar;	//!< vertical scroll-bar widget
	Scrollbar*	d_horzScrollbar;	//!< horizontal scroll-bar widget
	LBItemList	d_listItems;		//!< list of items in the list box.
	ListboxItem*	d_lastSelected;	//!< holds pointer to the last selected item (used in range selections)


private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static ListboxProperties::Sort					d_sortProperty;
	static ListboxProperties::MultiSelect			d_multiSelectProperty;
	static ListboxProperties::ForceVertScrollbar	d_forceVertProperty;
	static ListboxProperties::ForceHorzScrollbar	d_forceHorzProperty;
	static ListboxProperties::ItemTooltips			d_itemTooltipsProperty;

	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addListboxProperties(void);
};


/*!
\brief
	Helper function used in sorting to compare two list box item text strings
	via the ListboxItem pointers and return if \a a is less than \a b.
*/
bool lbi_less(const ListboxItem* a, const ListboxItem* b);


/*!
\brief
	Helper function used in sorting to compare two list box item text strings
	via the ListboxItem pointers and return if \a a is greater than \a b.
*/
bool lbi_greater(const ListboxItem* a, const ListboxItem* b);

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIListbox_h_
