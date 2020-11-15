/***********************************************************************
	created:	13/4/2004
	author:		Paul D Turner

	purpose:	Implementation of Combobox base class
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
#include "CEGUI/widgets/Combobox.h"
#include "CEGUI/widgets/Editbox.h"
#include "CEGUI/widgets/PushButton.h"
#include "CEGUI/widgets/ComboDropList.h"
#include "CEGUI/widgets/ListboxItem.h"
#include "CEGUI/WindowManager.h"

/*************************************************************************
	General information.

	The Combobox class is, for the most part, a huge proxy to the
	component Editbox and ComboDropList (Listbox) widgets.  The Combobox
	widget itself actually does very little.
*************************************************************************/

// Start of CEGUI namespace section
namespace CEGUI
{
const String Combobox::EventNamespace("Combobox");
const String Combobox::WidgetTypeName("CEGUI/Combobox");

/*************************************************************************
	Constants
*************************************************************************/
// event names from edit box
const String Combobox::EventReadOnlyModeChanged( "ReadOnlyModeChanged" );
const String Combobox::EventValidationStringChanged( "ValidationStringChanged" );
const String Combobox::EventMaximumTextLengthChanged( "MaximumTextLengthChanged" );
const String Combobox::EventTextValidityChanged( "TextValidityChanged" );
const String Combobox::EventCaretMoved( "CaretMoved" );
const String Combobox::EventTextSelectionChanged( "TextSelectionChanged" );
const String Combobox::EventEditboxFull( "EditboxFull" );
const String Combobox::EventTextAccepted( "TextAccepted" );

// event names from list box
const String Combobox::EventListContentsChanged( "ListContentsChanged" );
const String Combobox::EventListSelectionChanged( "ListSelectionChanged" );
const String Combobox::EventSortModeChanged( "SortModeChanged" );
const String Combobox::EventVertScrollbarModeChanged( "VertScrollbarModeChanged" );
const String Combobox::EventHorzScrollbarModeChanged( "HorzScrollbarModeChanged" );

// events we produce / generate ourselves
const String Combobox::EventDropListDisplayed( "DropListDisplayed" );
const String Combobox::EventDropListRemoved( "DropListRemoved" );
const String Combobox::EventListSelectionAccepted( "ListSelectionAccepted" );

/*************************************************************************
    Child Widget name constants
*************************************************************************/
const String Combobox::EditboxName( "__auto_editbox__" );
const String Combobox::DropListName( "__auto_droplist__" );
const String Combobox::ButtonName( "__auto_button__" );


/*************************************************************************
	Constructor for Combobox base class
*************************************************************************/
Combobox::Combobox(const String& type, const String& name) :
	Window(type, name),
    d_singleClickOperation(false),
    d_autoSizeHeight(false),
    d_autoSizeWidth(false)
{
	addComboboxProperties();
}


/*************************************************************************
	Destructor for Combobox base class
*************************************************************************/
Combobox::~Combobox(void)
{
}


/*************************************************************************
	Initialise the Window based object ready for use.
*************************************************************************/
void Combobox::initialiseComponents(void)
{
	Editbox* editbox        = getEditbox();
	ComboDropList* droplist = getDropList();
	PushButton* button      = getPushButton();
    droplist->setFont(d_font);
    editbox->setFont(d_font);

    // ban properties forwarded from here
    droplist->banPropertyFromXML(Window::VisiblePropertyName);
    editbox->banPropertyFromXML("MaxTextLength");
    editbox->banPropertyFromXML("ReadOnly");

	// internal event wiring
	button->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&CEGUI::Combobox::button_PressHandler, this));
	droplist->subscribeEvent(ComboDropList::EventListSelectionAccepted, Event::Subscriber(&CEGUI::Combobox::droplist_SelectionAcceptedHandler, this));
	droplist->subscribeEvent(Window::EventHidden, Event::Subscriber(&CEGUI::Combobox::droplist_HiddenHandler, this));
	editbox->subscribeEvent(Window::EventMouseButtonDown, Event::Subscriber(&CEGUI::Combobox::editbox_MouseDownHandler, this));

	// event forwarding setup
	editbox->subscribeEvent(Editbox::EventReadOnlyModeChanged, Event::Subscriber(&CEGUI::Combobox::editbox_ReadOnlyChangedHandler, this));
	editbox->subscribeEvent(Editbox::EventValidationStringChanged, Event::Subscriber(&CEGUI::Combobox::editbox_ValidationStringChangedHandler, this));
	editbox->subscribeEvent(Editbox::EventMaximumTextLengthChanged, Event::Subscriber(&CEGUI::Combobox::editbox_MaximumTextLengthChangedHandler, this));
	editbox->subscribeEvent(Editbox::EventTextValidityChanged, Event::Subscriber(&CEGUI::Combobox::editbox_TextValidityChangedHandler, this));
	editbox->subscribeEvent(Editbox::EventCaretMoved, Event::Subscriber(&CEGUI::Combobox::editbox_CaretMovedHandler, this));
	editbox->subscribeEvent(Editbox::EventTextSelectionChanged, Event::Subscriber(&CEGUI::Combobox::editbox_TextSelectionChangedHandler, this));
	editbox->subscribeEvent(Editbox::EventEditboxFull, Event::Subscriber(&CEGUI::Combobox::editbox_EditboxFullEventHandler, this));
	editbox->subscribeEvent(Editbox::EventTextAccepted, Event::Subscriber(&CEGUI::Combobox::editbox_TextAcceptedEventHandler, this));
	editbox->subscribeEvent(Editbox::EventTextChanged, Event::Subscriber(&CEGUI::Combobox::editbox_TextChangedEventHandler, this));
	droplist->subscribeEvent(Listbox::EventListContentsChanged, Event::Subscriber(&CEGUI::Combobox::listbox_ListContentsChangedHandler, this));
	droplist->subscribeEvent(Listbox::EventSelectionChanged, Event::Subscriber(&CEGUI::Combobox::listbox_ListSelectionChangedHandler, this));
	droplist->subscribeEvent(Listbox::EventSortModeChanged, Event::Subscriber(&CEGUI::Combobox::listbox_SortModeChangedHandler, this));
	droplist->subscribeEvent(Listbox::EventVertScrollbarModeChanged, Event::Subscriber(&CEGUI::Combobox::listbox_VertScrollModeChangedHandler, this));
	droplist->subscribeEvent(Listbox::EventHorzScrollbarModeChanged, Event::Subscriber(&CEGUI::Combobox::listbox_HorzScrollModeChangedHandler, this));

	// put components in their initial positions
	performChildWindowLayout();
}


/*************************************************************************
	Show the drop-down list
*************************************************************************/
void Combobox::showDropList(void)
{
    updateAutoSizedDropList();

	// Display the box
    ComboDropList* droplist = getDropList();
	droplist->show();
	droplist->activate();
	droplist->captureInput();

	// Fire off event
	WindowEventArgs args(this);
	onDropListDisplayed(args);
}


/*************************************************************************
	Hide the drop-down list
*************************************************************************/
void Combobox::hideDropList(void)
{
	// the natural order of things when this happens will ensure the list is
	// hidden and events are fired.
	getDropList()->releaseInput();
}


/*************************************************************************
	return true if the Editbox has input focus.
*************************************************************************/
bool Combobox::hasInputFocus(void) const
{
	return getEditbox()->hasInputFocus();
}


/*************************************************************************
	return true if the Editbox is read-only.
*************************************************************************/
bool Combobox::isReadOnly(void) const
{
	return getEditbox()->isReadOnly();
}


/*************************************************************************
	return true if the Editbox text is valid given the currently set
	validation string.
*************************************************************************/
Combobox::MatchState Combobox::getTextMatchState() const
{
	return getEditbox()->getTextMatchState();
}


/*************************************************************************
	return the currently set validation string
*************************************************************************/
const String& Combobox::getValidationString(void) const
{
	return getEditbox()->getValidationString();
}


/*************************************************************************
	return the current position of the caret.
*************************************************************************/
size_t Combobox::getCaretIndex(void) const
{
	return getEditbox()->getCaretIndex();
}


/*************************************************************************
	return the current selection start point.
*************************************************************************/
size_t Combobox::getSelectionStartIndex(void) const
{
	return getEditbox()->getSelectionStartIndex();
}


/*************************************************************************
	return the current selection end point.
*************************************************************************/
size_t Combobox::getSelectionEndIndex(void) const
{
	return getEditbox()->getSelectionEndIndex();
}


/*************************************************************************
	return the length of the current selection (in code points / characters).
*************************************************************************/
size_t Combobox::getSelectionLength(void) const
{
	return getEditbox()->getSelectionLength();
}


/*************************************************************************
	return the maximum text length set for this Editbox.
*************************************************************************/
size_t Combobox::getMaxTextLength(void) const
{
	return getEditbox()->getMaxTextLength();
}


/*************************************************************************
	Specify whether the Editbox is read-only.
*************************************************************************/
void Combobox::setReadOnly(bool setting)
{
	getEditbox()->setReadOnly(setting);
}


/*************************************************************************
	Set the text validation string.
*************************************************************************/
void Combobox::setValidationString(const String& validation_string)
{
	getEditbox()->setValidationString(validation_string);
}


/*************************************************************************
	Set the current position of the caret.
*************************************************************************/
void Combobox::setCaretIndex(size_t caret_pos)
{
	getEditbox()->setCaretIndex(caret_pos);
}


/*************************************************************************
	Define the current selection for the Editbox
*************************************************************************/
void Combobox::setSelection(size_t start_pos, size_t end_pos)
{
	getEditbox()->setSelection(start_pos, end_pos);
}

/*************************************************************************
	Define the current selectionStart for the Editbox
*************************************************************************/
void Combobox::setSelectionStart(size_t start_pos)
{
	getEditbox()->setSelectionStart(start_pos);
}
/*************************************************************************
	Define the current selectionLength for the Editbox
*************************************************************************/
void Combobox::setSelectionLength(size_t length)
{
	getEditbox()->setSelectionLength(length);
}


/*************************************************************************
	set the maximum text length for this Editbox.
*************************************************************************/
void Combobox::setMaxTextLength(size_t max_len)
{
	getEditbox()->setMaxTextLength(max_len);
}


/*************************************************************************
	Return number of items attached to the list box
*************************************************************************/
size_t Combobox::getItemCount(void) const
{
	return getDropList()->getItemCount();
}


/*************************************************************************
	Return a pointer to the currently selected item.
*************************************************************************/
ListboxItem* Combobox::getSelectedItem(void) const
{
	return getDropList()->getFirstSelectedItem();
}


/*************************************************************************
	Return the item at index position \a index.
*************************************************************************/
ListboxItem* Combobox::getListboxItemFromIndex(size_t index) const
{
	return getDropList()->getListboxItemFromIndex(index);
}


/*************************************************************************
	Return the index of ListboxItem 'item'
*************************************************************************/
size_t Combobox::getItemIndex(const ListboxItem* item) const
{
	return getDropList()->getItemIndex(item);
}


/*************************************************************************
	return whether list sorting is enabled
*************************************************************************/
bool Combobox::isSortEnabled(void) const
{
	return getDropList()->isSortEnabled();
}


/*************************************************************************
	return whether the string at index position 'index' is selected
*************************************************************************/
bool Combobox::isItemSelected(size_t index) const
{
	return getDropList()->isItemSelected(index);
}


/*************************************************************************
	Search the list for an item with the specified text
*************************************************************************/
ListboxItem* Combobox::findItemWithText(const String& text, const ListboxItem* start_item)
{
	return getDropList()->findItemWithText(text, start_item);
}


/*************************************************************************
	Return whether the specified ListboxItem is in the List
*************************************************************************/
bool Combobox::isListboxItemInList(const ListboxItem* item) const
{
	return getDropList()->isListboxItemInList(item);
}


/*************************************************************************
	Remove all items from the list.
*************************************************************************/
void Combobox::resetList(void)
{
	getDropList()->resetList();
}


/*************************************************************************
	Add the given ListboxItem to the list.
*************************************************************************/
void Combobox::addItem(ListboxItem* item)
{
	getDropList()->addItem(item);
}


/*************************************************************************
	Insert an item into the list box after a specified item already in
	the list.
*************************************************************************/
void Combobox::insertItem(ListboxItem* item, const ListboxItem* position)
{
	getDropList()->insertItem(item, position);
}


/*************************************************************************
	Removes the given item from the list box.
*************************************************************************/
void Combobox::removeItem(const ListboxItem* item)
{
	getDropList()->removeItem(item);
}


/*************************************************************************
	Clear the selected state for all items.
*************************************************************************/
void Combobox::clearAllSelections(void)
{
	getDropList()->clearAllSelections();
}


/*************************************************************************
	Set whether the list should be sorted.
*************************************************************************/
void Combobox::setSortingEnabled(bool setting)
{
	getDropList()->setSortingEnabled(setting);
}


/*************************************************************************
	Set whether the vertical scroll bar should always be shown.
*************************************************************************/
void Combobox::setShowVertScrollbar(bool setting)
{
	getDropList()->setShowVertScrollbar(setting);
}


/*************************************************************************
	Set whether the horizontal scroll bar should always be shown.
*************************************************************************/
void Combobox::setShowHorzScrollbar(bool setting)
{
	getDropList()->setShowHorzScrollbar(setting);
}


/*************************************************************************
	Set the select state of an attached ListboxItem.
*************************************************************************/
void Combobox::setItemSelectState(ListboxItem* item, bool state)
{
    bool was_selected = (item && item->isSelected());

	getDropList()->setItemSelectState(item, state);

    itemSelectChangeTextUpdate(item, state, was_selected);
}


/*************************************************************************
	Set the select state of an attached ListboxItem.
*************************************************************************/
void Combobox::setItemSelectState(size_t item_index, bool state)
{
    ComboDropList* droplist = getDropList();

    ListboxItem* item = (droplist->getItemCount() > item_index) ?
                            droplist->getListboxItemFromIndex(item_index) :
                            0;

    bool was_selected = (item && item->isSelected());

    droplist->setItemSelectState(item_index, state);

    itemSelectChangeTextUpdate(item, state, was_selected);
}


/*************************************************************************
	Causes the list box to update it's internal state after changes have
	been made to one or more attached ListboxItem objects.
*************************************************************************/
void Combobox::handleUpdatedListItemData(void)
{
    getDropList()->handleUpdatedItemData();
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onReadOnlyChanged(WindowEventArgs& e)
{
	fireEvent(EventReadOnlyModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onValidationStringChanged(WindowEventArgs& e)
{
	fireEvent(EventValidationStringChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onMaximumTextLengthChanged(WindowEventArgs& e)
{
	fireEvent(EventMaximumTextLengthChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onTextValidityChanged(RegexMatchStateEventArgs& e)
{
	fireEvent(EventTextValidityChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onCaretMoved(WindowEventArgs& e)
{
	fireEvent(EventCaretMoved, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onTextSelectionChanged(WindowEventArgs& e)
{
	fireEvent(EventTextSelectionChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onEditboxFullEvent(WindowEventArgs& e)
{
	fireEvent(EventEditboxFull, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onTextAcceptedEvent(WindowEventArgs& e)
{
    selectListItemWithEditboxText();
	fireEvent(EventTextAccepted, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onListContentsChanged(WindowEventArgs& e)
{
	fireEvent(EventListContentsChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onListSelectionChanged(WindowEventArgs& e)
{
	fireEvent(EventListSelectionChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onSortModeChanged(WindowEventArgs& e)
{
	fireEvent(EventSortModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onVertScrollbarModeChanged(WindowEventArgs& e)
{
	fireEvent(EventVertScrollbarModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onHorzScrollbarModeChanged(WindowEventArgs& e)
{
	fireEvent(EventHorzScrollbarModeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onDropListDisplayed(WindowEventArgs& e)
{
    getGUIContext().updateWindowContainingMouse();
    getPushButton()->setPushedState(true);
	fireEvent(EventDropListDisplayed, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onDroplistRemoved(WindowEventArgs& e)
{
    getGUIContext().updateWindowContainingMouse();
    getPushButton()->setPushedState(false);
	fireEvent(EventDropListRemoved, e, EventNamespace);
}


/*************************************************************************
	Handler for when
*************************************************************************/
void Combobox::onListSelectionAccepted(WindowEventArgs& e)
{
	fireEvent(EventListSelectionAccepted, e, EventNamespace);
}


/*************************************************************************
	Handler for when widget font is changed
*************************************************************************/
void Combobox::onFontChanged(WindowEventArgs& e)
{
    // Propagate to children
    getEditbox()->setFont(d_font);
    getDropList()->setFont(d_font);

    // Call base class handler
    Window::onFontChanged(e);
}


/*************************************************************************
	Handler for when text changes
*************************************************************************/
void Combobox::onTextChanged(WindowEventArgs& e)
{
    Editbox* editbox = getEditbox();

	// update ourselves only if needed (prevents perpetual event loop & stack overflow)
    if (editbox->getText() != getText())
	{
		// done before doing base class processing so event subscribers see 'updated' version of this.
        editbox->setText(getText());
		++e.handled;

        selectListItemWithEditboxText();

		Window::onTextChanged(e);
	}

}



/*************************************************************************
	Handler function for button clicks.
*************************************************************************/
bool Combobox::button_PressHandler(const EventArgs&)
{
    selectListItemWithEditboxText();
    showDropList();

	return true;
}

//----------------------------------------------------------------------------//
void Combobox::selectListItemWithEditboxText()
{
    ComboDropList* const droplist = getDropList();

    if (ListboxItem* item = droplist->findItemWithText(getEditbox()->getText(), 0))
    {
        droplist->setItemSelectState(item, true);
        droplist->ensureItemIsVisible(item);
    }
    else
        droplist->clearAllSelections();
}

//----------------------------------------------------------------------------//
bool Combobox::getAutoSizeListHeightToContent() const
{
    return d_autoSizeHeight;
}

//----------------------------------------------------------------------------//
bool Combobox::getAutoSizeListWidthToContent() const
{
    return d_autoSizeWidth;
}

//----------------------------------------------------------------------------//
void Combobox::setAutoSizeListHeightToContent(bool auto_size)
{
    d_autoSizeHeight = auto_size;

    if (d_autoSizeHeight && isDropDownListVisible())
        updateAutoSizedDropList();
}

//----------------------------------------------------------------------------//
void Combobox::setAutoSizeListWidthToContent(bool auto_size)
{
    d_autoSizeWidth = auto_size;

    if (d_autoSizeWidth && isDropDownListVisible())
        updateAutoSizedDropList();
}

//----------------------------------------------------------------------------//
void Combobox::updateAutoSizedDropList()
{
    getDropList()->resizeToContent(d_autoSizeWidth, d_autoSizeHeight);
}

/*************************************************************************
	Handler for selections made in the drop-list
*************************************************************************/
bool Combobox::droplist_SelectionAcceptedHandler(const EventArgs& e)
{
	// copy the text from the selected item into the edit box
	ListboxItem* item = ((ComboDropList*)((WindowEventArgs&)e).window)->getFirstSelectedItem();

	if (item)
	{
        Editbox* editbox = getEditbox();
		// Put the text from the list item into the edit box
		editbox->setText(item->getText());

		// select text if it's editable, and move caret to end
		if (!isReadOnly())
		{
			editbox->setSelection(0, item->getText().length());
			editbox->setCaretIndex(item->getText().length());
		}

		editbox->setCaretIndex(0);
		editbox->activate();

		// fire off an event of our own
		WindowEventArgs args(this);
		onListSelectionAccepted(args);
	}

	return true;
}


/*************************************************************************
	Handler for when drop-list hides itself
*************************************************************************/
bool Combobox::droplist_HiddenHandler(const EventArgs&)
{
	WindowEventArgs args(this);
	onDroplistRemoved(args);

	return true;
}


/*************************************************************************
	Handler for mouse button down events in editbox
*************************************************************************/
bool Combobox::editbox_MouseDownHandler(const EventArgs& e)
{
	// only interested in left button
	if (((const MouseEventArgs&)e).button == LeftButton)
	{
        Editbox* editbox = getEditbox();

		// if edit box is read-only, show list
		if (editbox->isReadOnly())
		{
            ComboDropList* droplist = getDropList();

			// if there is an item with the same text as the edit box, pre-select it
			ListboxItem* item = droplist->findItemWithText(editbox->getText(), 0);

			if (item)
			{
				droplist->setItemSelectState(item, true);
				droplist->ensureItemIsVisible(item);
			}
			// no matching item, so select nothing
			else
			{
				droplist->clearAllSelections();
			}

            showDropList();

			return true;
		}
	}

	return false;
}


/*************************************************************************
	Return whether the vertical scroll bar is always shown.
*************************************************************************/
bool Combobox::isVertScrollbarAlwaysShown(void) const
{
	return getDropList()->isVertScrollbarAlwaysShown();
}


/*************************************************************************
	Return whether the horizontal scroll bar is always shown.
*************************************************************************/
bool Combobox::isHorzScrollbarAlwaysShown(void) const
{
	return getDropList()->isHorzScrollbarAlwaysShown();
}


/*************************************************************************
	Add properties for this class
*************************************************************************/
void Combobox::addComboboxProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;
    
    CEGUI_DEFINE_PROPERTY(Combobox, bool,
          "ReadOnly","Property to get/set the read-only setting for the Editbox.  Value is either \"true\" or \"false\".",
          &Combobox::setReadOnly, &Combobox::isReadOnly, false
    );
    CEGUI_DEFINE_PROPERTY(Combobox, String,
          "ValidationString","Property to get/set the validation string Editbox.  Value is a text string.",
          &Combobox::setValidationString, &Combobox::getValidationString, ".*"
    );
    CEGUI_DEFINE_PROPERTY(Combobox, size_t,
          "CaretIndex","Property to get/set the current caret index.  Value is \"[uint]\".",
          &Combobox::setCaretIndex, &Combobox::getCaretIndex, 0
    );
    CEGUI_DEFINE_PROPERTY(Combobox, size_t,
          "SelectionStart","Property to get/set the zero based index of the selection start position within the text.  Value is \"[uint]\".",
          &Combobox::setSelectionStart, &Combobox::getSelectionStartIndex, 0
    );
    CEGUI_DEFINE_PROPERTY(Combobox, size_t,
          "SelectionLength","Property to get/set the length of the selection (as a count of the number of code points selected).  Value is \"[uint]\".",
          &Combobox::setSelectionLength, &Combobox::getSelectionLength, 0
    );
    CEGUI_DEFINE_PROPERTY(Combobox, size_t,
          "MaxTextLength","Property to get/set the the maximum allowed text length (as a count of code points).  Value is \"[uint]\".",
          &Combobox::setMaxTextLength, &Combobox::getMaxTextLength, String().max_size()
    );
    CEGUI_DEFINE_PROPERTY(Combobox, bool,
          "SortList","Property to get/set the sort setting of the list box.  Value is either \"true\" or \"false\".",
          &Combobox::setSortingEnabled, &Combobox::isSortEnabled, false /* TODO: Inconsistency between setter, getter and property name */
    );
    CEGUI_DEFINE_PROPERTY(Combobox, bool,
          "ForceVertScrollbar", "Property to get/set the 'always show' setting for the vertical scroll bar of the list box.  Value is either \"true\" or \"false\".",
          &Combobox::setShowVertScrollbar, &Combobox::isVertScrollbarAlwaysShown, false /* TODO: Inconsistency between setter, getter and property name */
    );
    CEGUI_DEFINE_PROPERTY(Combobox, bool,
          "ForceHorzScrollbar","Property to get/set the 'always show' setting for the horizontal scroll bar of the list box.  Value is either \"true\" or \"false\".",
          &Combobox::setShowHorzScrollbar, &Combobox::isHorzScrollbarAlwaysShown, false /* TODO: Inconsistency between setter, getter and property name */
    );
    CEGUI_DEFINE_PROPERTY(Combobox, bool,
          "SingleClickMode","Property to get/set the 'single click mode' setting for the combo box.  Value is either \"true\" or \"false\".",
          &Combobox::setSingleClickEnabled, &Combobox::getSingleClickEnabled, false /* TODO: Inconsistency between setter, getter and property name */
    );
    CEGUI_DEFINE_PROPERTY(Combobox, bool,
          "AutoSizeListHeight",
          "Property to get/set whether the drop down list will vertically "
          "auto-size itself to fit it's content. "
          "Value is either \"true\" or \"false\".",
          &Combobox::setAutoSizeListHeightToContent,
          &Combobox::getAutoSizeListHeightToContent, false
    );
    CEGUI_DEFINE_PROPERTY(Combobox, bool,
          "AutoSizeListWidth",
          "Property to get/set whether the drop down list will horizontally "
          "auto-size itself to fit it's content. "
          "Value is either \"true\" or \"false\".",
          &Combobox::setAutoSizeListWidthToContent,
          &Combobox::getAutoSizeListWidthToContent, false
    );
}


/*************************************************************************
	Activate the edit box component of the Combobox.
*************************************************************************/
void Combobox::activateEditbox(void)
{
    Editbox* editbox = getEditbox();

	if (!editbox->isActive())
	{
		editbox->activate();
	}
}



/*************************************************************************
	Widget activation handler
*************************************************************************/
void Combobox::onActivated(ActivationEventArgs& e)
{
	if (!isActive())
	{
		Window::onActivated(e);
		activateEditbox();
	}

}

//----------------------------------------------------------------------------//
void Combobox::onSized(ElementEventArgs& e)
{
    if (isDropDownListVisible())
        updateAutoSizedDropList();

    Window::onSized(e);
}


/*************************************************************************
	Return operation mode for the combo box
*************************************************************************/
bool Combobox::getSingleClickEnabled(void) const
{
	return d_singleClickOperation;
}


/*************************************************************************
	Return whether drop-list is visible.
*************************************************************************/
bool Combobox::isDropDownListVisible(void) const
{
	return getDropList()->isEffectiveVisible();
}


/*************************************************************************
	Set the operation mode for the combo box.
*************************************************************************/
void Combobox::setSingleClickEnabled(bool setting)
{
	d_singleClickOperation = setting;
	getDropList()->setAutoArmEnabled(setting);
}


/************************************************************************
    Return a pointer to the Editbox component widget for this Combobox.
************************************************************************/
Editbox* Combobox::getEditbox() const
{
    return static_cast<Editbox*>(getChild(EditboxName));
}

/************************************************************************
    Return a pointer to the PushButton component widget for this Combobox.
************************************************************************/
PushButton* Combobox::getPushButton() const
{
    return static_cast<PushButton*>(getChild(ButtonName));
}

/************************************************************************
    Return a pointer to the ComboDropList component widget for this
    Combobox.
************************************************************************/
ComboDropList* Combobox::getDropList() const
{
    return static_cast<ComboDropList*>(getChild(DropListName));
}

//----------------------------------------------------------------------------//
void Combobox::itemSelectChangeTextUpdate(const ListboxItem* const item,
    bool new_state, bool old_state)
{
    if (!new_state)
    {
        if (getText() == item->getText())
            setText("");
    }
    else
    {
        if (!old_state)
            setText(item->getText());
    }
}

//----------------------------------------------------------------------------//
bool Combobox::isHit(const Vector2f& /*position*/,
                     const bool /*allow_disabled*/) const
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Handlers to relay child widget events so they appear to come from us
*************************************************************************/
//////////////////////////////////////////////////////////////////////////
bool Combobox::editbox_ReadOnlyChangedHandler(const EventArgs&)
{
	WindowEventArgs	args(this);
	onReadOnlyChanged(args);

	return true;
}


bool Combobox::editbox_ValidationStringChangedHandler(const EventArgs&)
{
	WindowEventArgs	args(this);
	onValidationStringChanged(args);

	return true;
}


bool Combobox::editbox_MaximumTextLengthChangedHandler(const EventArgs&)
{
	WindowEventArgs	args(this);
	onMaximumTextLengthChanged(args);

	return true;
}


bool Combobox::editbox_TextValidityChangedHandler(const EventArgs& e)
{
	RegexMatchStateEventArgs args(
        this, static_cast<const RegexMatchStateEventArgs&>(e).matchState);
	onTextValidityChanged(args);

	return args.handled > 0;
}


bool Combobox::editbox_CaretMovedHandler(const EventArgs&)
{
	WindowEventArgs	args(this);
	onCaretMoved(args);

	return true;
}


bool Combobox::editbox_TextSelectionChangedHandler(const EventArgs&)
{
	WindowEventArgs	args(this);
	onTextSelectionChanged(args);

	return true;
}


bool Combobox::editbox_EditboxFullEventHandler(const EventArgs&)
{
	WindowEventArgs	args(this);
	onEditboxFullEvent(args);

	return true;
}


bool Combobox::editbox_TextAcceptedEventHandler(const EventArgs&)
{
	WindowEventArgs	args(this);
	onTextAcceptedEvent(args);

	return true;
}


bool Combobox::editbox_TextChangedEventHandler(const EventArgs& e)
{
	// set this windows text to match
	setText(((const WindowEventArgs&)e).window->getText());

	return true;
}


bool Combobox::listbox_ListContentsChangedHandler(const EventArgs&)
{
    if (isDropDownListVisible())
        updateAutoSizedDropList();

	WindowEventArgs	args(this);
	onListContentsChanged(args);

	return true;
}


bool Combobox::listbox_ListSelectionChangedHandler(const EventArgs&)
{
	WindowEventArgs	args(this);
	onListSelectionChanged(args);

	return true;
}


bool Combobox::listbox_SortModeChangedHandler(const EventArgs&)
{
	WindowEventArgs	args(this);
	onSortModeChanged(args);

	return true;
}


bool Combobox::listbox_VertScrollModeChangedHandler(const EventArgs&)
{
	WindowEventArgs	args(this);
	onVertScrollbarModeChanged(args);

	return true;
}


bool Combobox::listbox_HorzScrollModeChangedHandler(const EventArgs&)
{
	WindowEventArgs	args(this);
	onHorzScrollbarModeChanged(args);

	return true;
}

} // End of  CEGUI namespace section
