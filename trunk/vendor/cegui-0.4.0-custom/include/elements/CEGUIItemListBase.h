/************************************************************************
	filename: 	CEGUIItemListBase.h
	created:	31/3/2005
	author:		Tomas Lindquist Olsen (based on original Listbox code by Paul D Turner)
	
	purpose:	Interface to base class for ItemListBase widgets
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
#ifndef _CEGUIItemListBase_h_
#define _CEGUIItemListBase_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUIItemListBaseProperties.h"
#include "elements/CEGUIItemEntry.h"

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
	Base class for item list widgets.
*/
class CEGUIEXPORT ItemListBase : public Window
{
public:
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Constants
	*************************************************************************/
	// event names
	static const String EventListContentsChanged;			//!< Event triggered when the contents of the list is changed.


	/*************************************************************************
		Accessor Methods
	*************************************************************************/
	/*!
	\brief
		Return number of items attached to the list

	\return
		the number of items currently attached to this list.
	*/
	size_t	getItemCount(void) const		{return d_listItems.size();}


	/*!
	\brief
		Return the item at index position \a index.

	\param index
		Zero based index of the item to be returned.

	\return
		Pointer to the ItemEntry at index position \a index in the list.

	\exception	InvalidRequestException	thrown if \a index is out of range.
	*/
	ItemEntry*	getItemFromIndex(size_t index) const;


	/*!
	\brief
		Return the index of ItemEntry \a item

	\param item
		Pointer to a ItemEntry whos zero based index is to be returned.

	\return
		Zero based index indicating the position of ItemEntry \a item in the list.

	\exception	InvalidRequestException	thrown if \a item is not attached to this list.
	*/
	size_t	getItemIndex(const ItemEntry* item) const;


	/*!
	\brief
		Search the list for an item with the specified text

	\param text
		String object containing the text to be searched for.

	\param start_item
		ItemEntry where the search is to begin, the search will not include \a item.  If \a item is
		NULL, the search will begin from the first item in the list.

	\return
		Pointer to the first ItemEntry in the list after \a item that has text matching \a text.  If
		no item matches the criteria NULL is returned.

	\exception	InvalidRequestException	thrown if \a item is not attached to this list box.
	*/
	ItemEntry*	findItemWithText(const String& text, const ItemEntry* start_item);


	/*!
	\brief
		Return whether the specified ItemEntry is in the List

	\return
		true if ItemEntry \a item is in the list, false if ItemEntry \a item is not in the list.
	*/
	bool	isItemInList(const ItemEntry* item) const;


	/*!
	\brief
		Return wheter this window is automatically resized to fit its content.

	\return
		true if automatic resizing is enabled, false if it is disabled.
	*/
	bool isAutoResizeEnabled() const		{return d_autoResize;}


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
		Add the given ItemEntry to the list.

	\param item
		Pointer to the ItemEntry to be added to the list.  Note that it is the passed object that is added to the
		list, a copy is not made.  If this parameter is NULL, nothing happens.

	\return
		Nothing.
	*/
	void	addItem(ItemEntry* item);


	/*!
	\brief
		Insert an item into the list after a specified item already in the list.

		Note that if the list is sorted, the item may not end up in the requested position.

	\param item
		Pointer to the ItemEntry to be inserted.  Note that it is the passed object that is added to the
		list, a copy is not made.  If this parameter is NULL, nothing happens.

	\param position
		Pointer to a ItemEntry that \a item is to be inserted after.  If this parameter is NULL, the item is
		inserted at the start of the list.

	\return
		Nothing.
	*/
	void	insertItem(ItemEntry* item, const ItemEntry* position);


	/*!
	\brief
		Removes the given item from the list.  If the item is has the auto delete state set, the item will be deleted.

	\param item
		Pointer to the ItemEntry that is to be removed.  If \a item is not attached to this list then nothing
		will happen.

	\return
		Nothing.
	*/
	void	removeItem(ItemEntry* item);


	/*!
	\brief
		Causes the list to update it's internal state after changes have been made to one or more
		attached ItemEntry objects.

		Client code must call this whenever it has made any changes to ItemEntry objects already attached to the
		list.  If you are just adding items, or removed items to update them prior to re-adding them, there is
		no need to call this method.

	\return
		Nothing.
	*/
	void	handleUpdatedItemData(void);


	/*!
	\brief
		Set whether or not this ItemListBase widget should automatically resize to fit its content.

	\param setting
		Boolean value that if true enables automatic resizing, if false disables automatic resizing.

	\return
		Nothing.
	*/
	void setAutoResizeEnabled(bool setting);


	/*!
	\brief
	Resize the ItemListBase to exactly fit the content that is attached to it.
	Return a Rect object describing, in un-clipped pixels, the window relative area
	that is to be used for rendering items.

	\return
	Nothing
	*/
	virtual	void	sizeToContent(void)		{sizeToContent_impl();}


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for ItemListBase base class.
	*/
	ItemListBase(const String& type, const String& name);


	/*!
	\brief
		Destructor for ItemListBase base class.
	*/
	virtual ~ItemListBase(void);


protected:
	/*************************************************************************
		Abstract Implementation Functions (must be provided by derived class)
	*************************************************************************/
	/*!
	\brief
		Resize the ItemListBase to exactly fit the content that is attached to it.
		Return a Rect object describing, in un-clipped pixels, the window relative area
		that is to be used for rendering items.

	\return
		Nothing
	*/
	virtual	void	sizeToContent_impl(void)		= 0;


	/*!
	\brief
		Returns the Size in unclipped pixels of the content attached to this ItemListBase that is attached to it.

	\return
		Nothing.
	*/
	virtual Size getContentSize()		= 0;


	/*!
	\brief
		Return a Rect object describing, in un-clipped pixels, the window relative area
		that is to be used for rendering list items.

	\return
		Rect object describing the window relative area of the that is to be used for rendering
		the items.
	*/
	virtual	Rect	getItemRenderArea(void) const		= 0;


	/*!
	\brief
		Setup size and position for the item widgets attached to this ItemListBase

	\return
		Nothing.
	*/
	virtual void	layoutItemWidgets()	= 0;


	/*!
	\brief
		Perform the actual rendering for this Window.

	\return
		Nothing
	*/
	virtual	void	populateRenderCache() = 0;


	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Add list box specific events
	*/
	void	addItemListBaseEvents(void);


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
		if (class_name==(const utf8*)"ItemListBase")	return true;
		return Window::testClassName_impl(class_name);
	}

	/*************************************************************************
		New event handlers
	*************************************************************************/
	/*!
	\brief
		Handler called internally when the list contents are changed
	*/
	virtual	void	onListContentsChanged(WindowEventArgs& e);


	/*************************************************************************
		Overridden Event handlers
	*************************************************************************/
	virtual void	onSized(WindowEventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	typedef	std::vector<ItemEntry*>	ItemEntryList;
	ItemEntryList	d_listItems;		//!< list of items in the list.

	// boolean telling if this ItemListBase widget should automatically resize to fit its content.
	bool d_autoResize;

private:
	/*************************************************************************
	Static Properties for this class
	*************************************************************************/
	static ItemListBaseProperties::AutoResizeEnabled	d_autoResizeEnabledProperty;


	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addItemListBaseProperties(void);


	/*!
	\brief
		Add given window to child list at an appropriate position
	*/
	virtual void	addChild_impl(Window* wnd);
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIItemListBase_h_
