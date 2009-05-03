/************************************************************************
	filename: 	CEGUICombobox.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for Combobox widget
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
#ifndef _CEGUICombobox_h_
#define _CEGUICombobox_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUIComboboxProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Base class for the Combobox widget
*/
class CEGUIEXPORT Combobox : public Window
{
public:
	static const String EventNamespace;				//!< Namespace for global events

	/*************************************************************************
		Constants
	*************************************************************************/
	// event names from edit box
	static const String EventReadOnlyModeChanged;			//!< The read-only mode for the edit box has been changed.
	static const String EventValidationStringChanged;		//!< The validation string has been changed.
	static const String EventMaximumTextLengthChanged;	//!< The maximum allowable string length has been changed.
	static const String EventTextInvalidated;				//!< Some operation has made the current text invalid with regards to the validation string.
	static const String EventInvalidEntryAttempted;		//!< The user attempted to modify the text in a way that would have made it invalid.
	static const String EventCaratMoved;					//!< The text carat (insert point) has changed.
	static const String EventTextSelectionChanged;		//!< The current text selection has changed.
	static const String EventEditboxFull;					//!< The number of characters in the edit box has reached the current maximum.
	static const String EventTextAccepted;				//!< The user has accepted the current text by pressing Return, Enter, or Tab.

	// event names from list box
	static const String EventListContentsChanged;			//!< Event triggered when the contents of the list is changed.
	static const String EventListSelectionChanged;		//!< Event triggered when there is a change to the currently selected item(s).
	static const String EventSortModeChanged;				//!< Event triggered when the sort mode setting changes.
	static const String EventVertScrollbarModeChanged;	//!< Event triggered when the vertical scroll bar 'force' setting changes.
	static const String EventHorzScrollbarModeChanged;	//!< Event triggered when the horizontal scroll bar 'force' setting changes.

	// events we produce / generate ourselves
	static const String EventDropListDisplayed;			//!< Event triggered when the drop-down list is displayed
	static const String EventDropListRemoved;				//!< Event triggered when the drop-down list is removed / hidden.
	static const String EventListSelectionAccepted;		//!< Event triggered when the user accepts a selection from the drop-down list


    /*!
    \brief
        get the combobox chidren
        lil_Toady: That are custom functions i added

    \param
        none
    
    \return
        children
    */

    Editbox*		getEditBox(void)                    {return d_editbox;}
    ComboDropList*	getDropList(void)                   {return d_droplist;}
    PushButton*		getPushButton(void)                 {return d_button;}

	/*!
	\brief
		check if the given position would hit this window.

	\param position
		Point object describing the position to check in screen pixels

	\return
		true if \a position 'hits' this Window, else false.
	*/
	virtual bool	isHit(const Point& position) const		{return false;}


	/*!
	\brief
		returns the mode of operation for the combo box.

	\return
		- true if the user can show the list and select an item with a single mouse click.
		- false if the user must click to show the list and then click again to select an item.
	*/
	bool	getSingleClickEnabled(void) const;


	/*!
	\brief
		returns true if the drop down list is visible.

	\return
		true if the drop down list is visible, false otherwise.
	*/
	bool	isDropDownListVisible(void) const;


	/*************************************************************************
		Editbox Accessors
	*************************************************************************/
	/*!
	\brief
		return true if the Editbox has input focus.

	\return
		true if the Editbox has keyboard input focus, false if the Editbox does not have keyboard input focus.
	*/
	bool	hasInputFocus(void) const;


	/*!
	\brief
		return true if the Editbox is read-only.

	\return
		true if the Editbox is read only and can't be edited by the user, false if the Editbox is not
		read only and may be edited by the user.
	*/
	bool	isReadOnly(void) const;


	/*!
	\brief
		return true if the Editbox text is valid given the currently set validation string.

	\note
		It is possible to programmatically set 'invalid' text for the Editbox by calling setText.  This has certain
		implications since if invalid text is set, whatever the user types into the box will be rejected when the input
		is validated.

	\note
		Validation is performed by means of a regular expression.  If the text matches the regex, the text is said to have passed
		validation.  If the text does not match with the regex then the text fails validation.

	\return
		true if the current Editbox text passes validation, false if the text does not pass validation.
	*/
	bool	isTextValid(void) const;


	/*!
	\brief
		return the currently set validation string

	\note
		Validation is performed by means of a regular expression.  If the text matches the regex, the text is said to have passed
		validation.  If the text does not match with the regex then the text fails validation.

	\return
		String object containing the current validation regex data
	*/
	const String&	getValidationString(void) const;


	/*!
	\brief
		return the current position of the carat.

	\return
		Index of the insert carat relative to the start of the text.
	*/
	size_t	getCaratIndex(void) const;


	/*!
	\brief
		return the current selection start point.

	\return
		Index of the selection start point relative to the start of the text.  If no selection is defined this function returns
		the position of the carat.
	*/
	size_t	getSelectionStartIndex(void) const;


	/*!
	\brief
		return the current selection end point.

	\return
		Index of the selection end point relative to the start of the text.  If no selection is defined this function returns
		the position of the carat.
	*/
	size_t	getSelectionEndIndex(void) const;

	
	/*!
	\brief
		return the length of the current selection (in code points / characters).

	\return
		Number of code points (or characters) contained within the currently defined selection.
	*/
	size_t	getSelectionLength(void) const;


	/*!
	\brief
		return the maximum text length set for this Editbox.

	\return
		The maximum number of code points (characters) that can be entered into this Editbox.

	\note
		Depending on the validation string set, the actual length of text that can be entered may be less than the value
		returned here (it will never be more).
	*/
	size_t	getMaxTextLength(void) const;


	/*!
	\brief
		return the currently set colour to be used for rendering Editbox text in the
		normal, unselected state.

	\return
		colour value describing the ARGB colour that is currently set.
	*/
	colour	getNormalTextColour(void) const;


	/*!
	\brief
		return the currently set colour to be used for rendering the Editbox text when within the
		selected region.

	\return
		colour value describing the ARGB colour that is currently set.
	*/
	colour	getSelectedTextColour(void) const;


	/*!
	\brief
		return the currently set colour to be used for rendering the Editbox selection highlight
		when the Editbox is active.

	\return
		colour value describing the ARGB colour that is currently set.
	*/
	colour	getNormalSelectBrushColour(void) const;


	/*!
	\brief
		return the currently set colour to be used for rendering the Editbox selection highlight
		when the Editbox is inactive.

	\return
		colour value describing the ARGB colour that is currently set.
	*/
	colour	getInactiveSelectBrushColour(void) const;


	/*************************************************************************
		List Accessors
	*************************************************************************/
	/*!
	\brief
		Return number of items attached to the list box

	\return
		the number of items currently attached to this list box.
	*/
	size_t	getItemCount(void) const;

	
	/*!
	\brief
		Return a pointer to the currently selected item.

	\return
		Pointer to a ListboxItem based object that is the selected item in the list.  will return NULL if
		no item is selected.
	*/
	ListboxItem*	getSelectedItem(void) const;


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
	bool	isSortEnabled(void) const;


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
		Combobox Manipulators
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
		Show the drop-down list

	\return
		Nothing
	*/
	void	showDropList(void);


	/*!
	\brief
		Hide the drop-down list

	\return
		Nothing.
	*/
	void	hideDropList(void);


	/*!
	\brief
		Set the mode of operation for the combo box.

	\param setting
		- true if the user should be able to show the list and select an item with a single mouse click.
		- false if the user must click to show the list and then click again to select an item.

	\return
		Nothing.
	*/
	void	setSingleClickEnabled(bool setting);


	/*************************************************************************
		Editbox Manipulators
	*************************************************************************/
	/*!
	\brief
		Specify whether the Editbox is read-only.

	\param setting
		true if the Editbox is read only and can't be edited by the user, false if the Editbox is not
		read only and may be edited by the user.

	\return
		Nothing.
	*/
	void	setReadOnly(bool setting);


	/*!
	\brief
		Set the text validation string.

	\note
		Validation is performed by means of a regular expression.  If the text matches the regex, the text is said to have passed
		validation.  If the text does not match with the regex then the text fails validation.

	\param validation_string
		String object containing the validation regex data to be used.

	\return
		Nothing.
	*/
	void	setValidationString(const String& validation_string);


	/*!
	\brief
		Set the current position of the carat.

	\param carat_pos
		New index for the insert carat relative to the start of the text.  If the value specified is greater than the
		number of characters in the Editbox, the carat is positioned at the end of the text.

	\return
		Nothing.
	*/
	void	setCaratIndex(size_t carat_pos);


	/*!
	\brief
		Define the current selection for the Editbox

	\param start_pos
		Index of the starting point for the selection.  If this value is greater than the number of characters in the Editbox, the
		selection start will be set to the end of the text.

	\param end_pos
		Index of the ending point for the selection.  If this value is greater than the number of characters in the Editbox, the
		selection start will be set to the end of the text.

	\return
		Nothing.
	*/
	void	setSelection(size_t start_pos, size_t end_pos);
	

	/*!
	\brief
		set the maximum text length for this Editbox.

	\param max_len
		The maximum number of code points (characters) that can be entered into this Editbox.

	\note
		Depending on the validation string set, the actual length of text that can be entered may be less than the value
		set here (it will never be more).

	\return
		Nothing.
	*/
	void	setMaxTextLength(size_t max_len);


	/*!
	\brief
		Set the colour to be used for rendering Editbox text in the normal, unselected state.

	\param col
		colour value describing the ARGB colour that is to be used.

	\return
		Nothing.
	*/
	void	setNormalTextColour(colour col);


	/*!
	\brief
		Set the colour to be used for rendering the Editbox text when within the
		selected region.

	\return
		colour value describing the ARGB colour that is currently set.
	*/
	void	setSelectedTextColour(colour col);


	/*!
	\brief
		Set the colour to be used for rendering the Editbox selection highlight
		when the Editbox is active.

	\param col
		colour value describing the ARGB colour that is to be used.

	\return
		Nothing.
	*/
	void	setNormalSelectBrushColour(colour col);


	/*!
	\brief
		Set the colour to be used for rendering the Editbox selection highlight
		when the Editbox is inactive.

	\param col
		colour value describing the ARGB colour that is to be used.

	\return
		Nothing.
	*/
	void	setInactiveSelectBrushColour(colour col);


	/*!
	\brief
		Activate the edit box component of the Combobox.

	\return
		Nothing.
	*/
	void	activateEditbox(void);


	/*************************************************************************
		List Manipulators
	*************************************************************************/
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
	*/
	void	insertItem(ListboxItem* item, const ListboxItem* position);


	/*!
	\brief
		Removes the given item from the list box.

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
	void	handleUpdatedListItemData(void);


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for Combobox base class
	*/
	Combobox(const String& type, const String& name);


	/*!
	\brief
		Destructor for Combobox base class
	*/
	virtual ~Combobox(void);


protected:
	/*************************************************************************
		Implementation Methods
	*************************************************************************/
	/*!
	\brief
		Add combo box specific events
	*/
	void	addComboboxEvents(void);


	/*!
	\brief
		Create, initialise, and return a pointer to an Editbox widget to be used as part
		of this Combobox.

	\return
		Pointer to an Editbox derived class.
	*/
	virtual	Editbox*	createEditbox(const String& name) const		= 0;


	/*!
	\brief
		Create, initialise, and return a pointer to a PushButton widget to be used as part
		of this Combobox.

	\return
		Pointer to a PushButton derived class.
	*/
	virtual	PushButton*	createPushButton(const String& name) const	= 0;


	/*!
	\brief
		Create, initialise, and return a pointer to a ComboDropList widget to be used as part
		of this Combobox.

	\return
		Pointer to a ComboDropList derived class.
	*/
	virtual	ComboDropList*	createDropList(const String& name) const	= 0;


	/*!
	\brief
		Handler function for button clicks.
	*/
	bool	button_PressHandler(const EventArgs& e);


	/*!
	\brief
		Handler for selections made in the drop-list
	*/
	bool	droplist_SelectionAcceptedHandler(const EventArgs& e);


	/*!
	\brief
		Handler for when drop-list hides itself
	*/
	bool	droplist_HiddenHandler(const EventArgs& e);


	/*!
	\brief
		Mouse button down handler attached to edit box
	*/
	bool	editbox_MouseDownHandler(const EventArgs& e);


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
		if (class_name==(const utf8*)"Combobox")	return true;
		return Window::testClassName_impl(class_name);
	}


	/*************************************************************************
		Handlers to relay child widget events so they appear to come from us
	*************************************************************************/
	bool editbox_ReadOnlyChangedHandler(const EventArgs& e);
	bool editbox_ValidationStringChangedHandler(const EventArgs& e);
	bool editbox_MaximumTextLengthChangedHandler(const EventArgs& e);
	bool editbox_TextInvalidatedEventHandler(const EventArgs& e);
	bool editbox_InvalidEntryAttemptedHandler(const EventArgs& e);
	bool editbox_CaratMovedHandler(const EventArgs& e);
	bool editbox_TextSelectionChangedHandler(const EventArgs& e);
	bool editbox_EditboxFullEventHandler(const EventArgs& e);
	bool editbox_TextAcceptedEventHandler(const EventArgs& e);
	bool editbox_TextChangedEventHandler(const EventArgs& e);
	bool listbox_ListContentsChangedHandler(const EventArgs& e);
	bool listbox_ListSelectionChangedHandler(const EventArgs& e);
	bool listbox_SortModeChangedHandler(const EventArgs& e);
	bool listbox_VertScrollModeChangedHandler(const EventArgs& e);
	bool listbox_HorzScrollModeChangedHandler(const EventArgs& e);

	
	/*************************************************************************
		New Events for Combobox
	*************************************************************************/
	/*!
	\brief
		Handler called internally when the read only state of the Combobox's Editbox has been changed.
	*/
	virtual	void	onReadOnlyChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the Combobox's Editbox validation string has been changed.
	*/
	virtual	void	onValidationStringChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the Combobox's Editbox maximum text length is changed.
	*/
	virtual	void	onMaximumTextLengthChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the Combobox's Editbox text has been invalidated.
	*/
	virtual	void	onTextInvalidatedEvent(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when an invalid entry was attempted in the Combobox's Editbox.
	*/
	virtual	void	onInvalidEntryAttempted(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the carat in the Comboxbox's Editbox moves.
	*/
	virtual	void	onCaratMoved(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the selection within the Combobox's Editbox changes.
	*/
	virtual	void	onTextSelectionChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the maximum length is reached for text in the Combobox's Editbox.
	*/
	virtual	void	onEditboxFullEvent(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the text in the Combobox's Editbox is accepted (by various means).
	*/
	virtual	void	onTextAcceptedEvent(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the Combobox's Drop-down list contents are changed.
	*/
	virtual	void	onListContentsChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the selection within the Combobox's drop-down list changes
		(this is not the 'final' accepted selection, just the currently highlighted item).
	*/
	virtual	void	onListSelectionChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called  fired internally when the sort mode for the Combobox's drop-down list is changed.
	*/
	virtual	void	onSortModeChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the 'force' setting for the vertical scrollbar within the Combobox's
		drop-down list is changed.
	*/
	virtual	void	onVertScrollbarModeChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the 'force' setting for the horizontal scrollbar within the Combobox's
		drop-down list is changed.
	*/
	virtual	void	onHorzScrollbarModeChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the Combobox's drop-down list has been displayed.
	*/
	virtual	void	onDropListDisplayed(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the Combobox's drop-down list has been hidden.
	*/
	virtual	void	onDroplistRemoved(WindowEventArgs& e);


	/*!
	\brief
		Handler called internally when the user has confirmed a selection within the Combobox's drop-down list.
	*/
	virtual	void	onListSelectionAccepted(WindowEventArgs& e);


	/*************************************************************************
		Overridden Event handlers
	*************************************************************************/
	virtual	void	onFontChanged(WindowEventArgs& e);
	virtual void	onTextChanged(WindowEventArgs& e);
	virtual void	onActivated(ActivationEventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	Editbox*		d_editbox;		//!< Editbox widget sub-component.
	ComboDropList*	d_droplist;		//!< ComboDropList widget sub-component.	
	PushButton*		d_button;		//!< PushButton widget sub-component.
	bool			d_singleClickOperation;		//!< true if user can show and select from list in a single click.


private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static ComboboxProperties::ReadOnly						d_readOnlyProperty;
	static ComboboxProperties::ValidationString				d_validationStringProperty;
	static ComboboxProperties::CaratIndex					d_caratIndexProperty;
	static ComboboxProperties::EditSelectionStart			d_selStartProperty;
	static ComboboxProperties::EditSelectionLength			d_selLengthProperty;
	static ComboboxProperties::MaxEditTextLength			d_maxTextLengthProperty;
	static ComboboxProperties::NormalEditTextColour			d_normalTextColourProperty;
	static ComboboxProperties::SelectedEditTextColour		d_selectedTextColourProperty;
	static ComboboxProperties::ActiveEditSelectionColour	d_activeSelectionColourProperty;
	static ComboboxProperties::InactiveEditSelectionColour	d_inactiveSelectionColourProperty;
	static ComboboxProperties::SortList						d_sortProperty;
	static ComboboxProperties::ForceVertScrollbar			d_forceVertProperty;
	static ComboboxProperties::ForceHorzScrollbar			d_forceHorzProperty;
	static ComboboxProperties::SingleClickMode				d_singleClickOperationProperty;

	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addComboboxProperties(void);
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUICombobox_h_
