/************************************************************************
	filename: 	CEGUICombobox.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Combobox base class
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
#include "elements/CEGUICombobox.h"
#include "elements/CEGUIEditbox.h"
#include "elements/CEGUIPushButton.h"
#include "elements/CEGUIComboDropList.h"
#include "elements/CEGUIListboxItem.h"


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

/*************************************************************************
	Definitions for Properties for this class
*************************************************************************/
ComboboxProperties::ReadOnly					Combobox::d_readOnlyProperty;
ComboboxProperties::ValidationString			Combobox::d_validationStringProperty;
ComboboxProperties::CaratIndex					Combobox::d_caratIndexProperty;
ComboboxProperties::EditSelectionStart			Combobox::d_selStartProperty;
ComboboxProperties::EditSelectionLength			Combobox::d_selLengthProperty;
ComboboxProperties::MaxEditTextLength			Combobox::d_maxTextLengthProperty;
ComboboxProperties::NormalEditTextColour		Combobox::d_normalTextColourProperty;
ComboboxProperties::SelectedEditTextColour		Combobox::d_selectedTextColourProperty;
ComboboxProperties::ActiveEditSelectionColour	Combobox::d_activeSelectionColourProperty;
ComboboxProperties::InactiveEditSelectionColour	Combobox::d_inactiveSelectionColourProperty;
ComboboxProperties::SortList					Combobox::d_sortProperty;
ComboboxProperties::ForceVertScrollbar			Combobox::d_forceVertProperty;
ComboboxProperties::ForceHorzScrollbar			Combobox::d_forceHorzProperty;
ComboboxProperties::SingleClickMode				Combobox::d_singleClickOperationProperty;


/*************************************************************************
	Constants
*************************************************************************/
// event names from edit box
const String Combobox::EventReadOnlyModeChanged( (utf8*)"ReadOnlyChanged" );
const String Combobox::EventValidationStringChanged( (utf8*)"ValidationStringChanged" );
const String Combobox::EventMaximumTextLengthChanged( (utf8*)"MaximumTextLengthChanged" );
const String Combobox::EventTextInvalidated( (utf8*)"TextInvalidatedEvent" );
const String Combobox::EventInvalidEntryAttempted( (utf8*)"InvalidEntryAttempted" );
const String Combobox::EventCaratMoved( (utf8*)"CaratMoved" );
const String Combobox::EventTextSelectionChanged( (utf8*)"TextSelectionChanged" );
const String Combobox::EventEditboxFull( (utf8*)"EditboxFullEvent" );
const String Combobox::EventTextAccepted( (utf8*)"TextAcceptedEvent" );

// event names from list box
const String Combobox::EventListContentsChanged( (utf8*)"ListContentsChanged" );
const String Combobox::EventListSelectionChanged( (utf8*)"ListSelectionChanged" );
const String Combobox::EventSortModeChanged( (utf8*)"SortModeChanged" );
const String Combobox::EventVertScrollbarModeChanged( (utf8*)"VertScrollbarModeChanged" );
const String Combobox::EventHorzScrollbarModeChanged( (utf8*)"HorzScrollbarModeChanged" );

// events we produce / generate ourselves
const String Combobox::EventDropListDisplayed( (utf8*)"DropListDisplayed" );
const String Combobox::EventDropListRemoved( (utf8*)"DropListRemoved" );
const String Combobox::EventListSelectionAccepted( (utf8*)"ListSelectionAccepted" );

	
/*************************************************************************
	Constructor for Combobox base class
*************************************************************************/
Combobox::Combobox(const String& type, const String& name) :
	Window(type, name)
{
	d_singleClickOperation = false;

	addComboboxEvents();
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
void Combobox::initialise(void)
{
	d_editbox	= createEditbox(getName() + "__auto_editbox__");
	d_droplist	= createDropList(getName() + "__auto_droplist__");
	d_button	= createPushButton(getName() + "__auto_button__");
    d_droplist->setFont(getFont());
    d_editbox->setFont(getFont());

	addChildWindow(d_editbox);
	addChildWindow(d_droplist);
	addChildWindow(d_button);

	// internal event wiring
	d_button->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&CEGUI::Combobox::button_PressHandler, this));
	d_droplist->subscribeEvent(ComboDropList::EventListSelectionAccepted, Event::Subscriber(&CEGUI::Combobox::droplist_SelectionAcceptedHandler, this));
	d_droplist->subscribeEvent(Window::EventHidden, Event::Subscriber(&CEGUI::Combobox::droplist_HiddenHandler, this));
	d_editbox->subscribeEvent(Window::EventMouseButtonDown, Event::Subscriber(&CEGUI::Combobox::editbox_MouseDownHandler, this));

	// event forwarding setup
	d_editbox->subscribeEvent(Editbox::EventReadOnlyModeChanged, Event::Subscriber(&CEGUI::Combobox::editbox_ReadOnlyChangedHandler, this));
	d_editbox->subscribeEvent(Editbox::EventValidationStringChanged, Event::Subscriber(&CEGUI::Combobox::editbox_ValidationStringChangedHandler, this));
	d_editbox->subscribeEvent(Editbox::EventMaximumTextLengthChanged, Event::Subscriber(&CEGUI::Combobox::editbox_MaximumTextLengthChangedHandler, this));
	d_editbox->subscribeEvent(Editbox::EventTextInvalidated, Event::Subscriber(&CEGUI::Combobox::editbox_TextInvalidatedEventHandler, this));
	d_editbox->subscribeEvent(Editbox::EventInvalidEntryAttempted, Event::Subscriber(&CEGUI::Combobox::editbox_InvalidEntryAttemptedHandler, this));
	d_editbox->subscribeEvent(Editbox::EventCaratMoved, Event::Subscriber(&CEGUI::Combobox::editbox_CaratMovedHandler, this));
	d_editbox->subscribeEvent(Editbox::EventTextSelectionChanged, Event::Subscriber(&CEGUI::Combobox::editbox_TextSelectionChangedHandler, this));
	d_editbox->subscribeEvent(Editbox::EventEditboxFull, Event::Subscriber(&CEGUI::Combobox::editbox_EditboxFullEventHandler, this));
	d_editbox->subscribeEvent(Editbox::EventTextAccepted, Event::Subscriber(&CEGUI::Combobox::editbox_TextAcceptedEventHandler, this));
	d_editbox->subscribeEvent(Editbox::EventTextChanged, Event::Subscriber(&CEGUI::Combobox::editbox_TextChangedEventHandler, this));
	d_droplist->subscribeEvent(Listbox::EventListContentsChanged, Event::Subscriber(&CEGUI::Combobox::listbox_ListContentsChangedHandler, this));
	d_droplist->subscribeEvent(Listbox::EventSelectionChanged, Event::Subscriber(&CEGUI::Combobox::listbox_ListSelectionChangedHandler, this));
	d_droplist->subscribeEvent(Listbox::EventSortModeChanged, Event::Subscriber(&CEGUI::Combobox::listbox_SortModeChangedHandler, this));
	d_droplist->subscribeEvent(Listbox::EventVertScrollbarModeChanged, Event::Subscriber(&CEGUI::Combobox::listbox_VertScrollModeChangedHandler, this));
	d_droplist->subscribeEvent(Listbox::EventHorzScrollbarModeChanged, Event::Subscriber(&CEGUI::Combobox::listbox_HorzScrollModeChangedHandler, this));

	// put components in their initial positions
	performChildWindowLayout();
}


/*************************************************************************
	Show the drop-down list
*************************************************************************/
void Combobox::showDropList(void)
{
	// Display the box
	d_droplist->show();
	d_droplist->activate();
	d_droplist->captureInput();

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
	d_droplist->releaseInput();
}


/*************************************************************************
	return true if the Editbox has input focus.
*************************************************************************/
bool Combobox::hasInputFocus(void) const
{
	return d_editbox->hasInputFocus();
}


/*************************************************************************
	return true if the Editbox is read-only.
*************************************************************************/
bool Combobox::isReadOnly(void) const
{
	return d_editbox->isReadOnly();
}


/*************************************************************************
	return true if the Editbox text is valid given the currently set
	validation string.
*************************************************************************/
bool Combobox::isTextValid(void) const
{
	return d_editbox->isTextValid();
}


/*************************************************************************
	return the currently set validation string
*************************************************************************/
const String& Combobox::getValidationString(void) const
{
	return d_editbox->getValidationString();
}


/*************************************************************************
	return the current position of the carat.
*************************************************************************/
size_t Combobox::getCaratIndex(void) const
{
	return d_editbox->getCaratIndex();
}


/*************************************************************************
	return the current selection start point.
*************************************************************************/
size_t Combobox::getSelectionStartIndex(void) const
{
	return d_editbox->getSelectionStartIndex();
}


/*************************************************************************
	return the current selection end point.
*************************************************************************/
size_t Combobox::getSelectionEndIndex(void) const
{
	return d_editbox->getSelectionEndIndex();
}


/*************************************************************************
	return the length of the current selection (in code points / characters).
*************************************************************************/
size_t Combobox::getSelectionLength(void) const
{
	return d_editbox->getSelectionLength();
}


/*************************************************************************
	return the maximum text length set for this Editbox.
*************************************************************************/
size_t Combobox::getMaxTextLength(void) const
{
	return d_editbox->getMaxTextLength();
}


/*************************************************************************
	return the currently set colour to be used for rendering Editbox text
	in the normal, unselected state.
*************************************************************************/
colour Combobox::getNormalTextColour(void) const	
{
	return d_editbox->getNormalTextColour();
}


/*************************************************************************
	return the currently set colour to be used for rendering the Editbox
	text when within the selected region.
*************************************************************************/
colour Combobox::getSelectedTextColour(void) const
{
	return d_editbox->getSelectedTextColour();
}


/*************************************************************************
	return the currently set colour to be used for rendering the Editbox
	selection highlight when the Editbox is active.
*************************************************************************/
colour Combobox::getNormalSelectBrushColour(void) const
{
	return d_editbox->getNormalSelectBrushColour();
}


/*************************************************************************
	return the currently set colour to be used for rendering the Editbox
	selection highlight when the Editbox is inactive.
*************************************************************************/
colour Combobox::getInactiveSelectBrushColour(void) const
{
	return d_editbox->getInactiveSelectBrushColour();
}


/*************************************************************************
	Specify whether the Editbox is read-only.
*************************************************************************/
void Combobox::setReadOnly(bool setting)
{
	d_editbox->setReadOnly(setting);
}


/*************************************************************************
	Set the text validation string.
*************************************************************************/
void Combobox::setValidationString(const String& validation_string)
{
	d_editbox->setValidationString(validation_string);
}


/*************************************************************************
	Set the current position of the carat.
*************************************************************************/
void Combobox::setCaratIndex(size_t carat_pos)
{
	d_editbox->setCaratIndex(carat_pos);
}


/*************************************************************************
	Define the current selection for the Editbox
*************************************************************************/
void Combobox::setSelection(size_t start_pos, size_t end_pos)
{
	d_editbox->setSelection(start_pos, end_pos);
}


/*************************************************************************
	set the maximum text length for this Editbox.
*************************************************************************/
void Combobox::setMaxTextLength(size_t max_len)
{
	d_editbox->setMaxTextLength(max_len);
}


/*************************************************************************
	Set the colour to be used for rendering Editbox text in the normal,
	unselected state.
*************************************************************************/
void Combobox::setNormalTextColour(colour col)
{
	d_editbox->setNormalTextColour(col);
}


/*************************************************************************
	Set the colour to be used for rendering the Editbox text when within
	the selected region.	
*************************************************************************/
void Combobox::setSelectedTextColour(colour col)
{
	d_editbox->setSelectedTextColour(col);
}


/*************************************************************************
	Set the colour to be used for rendering the Editbox selection
	highlight when the Editbox is active.
*************************************************************************/
void Combobox::setNormalSelectBrushColour(colour col)
{
	d_editbox->setNormalSelectBrushColour(col);
}


/*************************************************************************
	Set the colour to be used for rendering the Editbox selection
	highlight when the Editbox is inactive.
*************************************************************************/
void Combobox::setInactiveSelectBrushColour(colour col)
{
	d_editbox->setInactiveSelectBrushColour(col);
}


/*************************************************************************
	Return number of items attached to the list box	
*************************************************************************/
size_t Combobox::getItemCount(void) const
{
	return d_droplist->getItemCount();
}


/*************************************************************************
	Return a pointer to the currently selected item.	
*************************************************************************/
ListboxItem* Combobox::getSelectedItem(void) const
{
	return d_droplist->getFirstSelectedItem();
}


/*************************************************************************
	Return the item at index position \a index.
*************************************************************************/
ListboxItem* Combobox::getListboxItemFromIndex(size_t index) const
{
	return d_droplist->getListboxItemFromIndex(index);
}


/*************************************************************************
	Return the index of ListboxItem 'item'
*************************************************************************/
size_t Combobox::getItemIndex(const ListboxItem* item) const
{
	return d_droplist->getItemIndex(item);
}


/*************************************************************************
	return whether list sorting is enabled	
*************************************************************************/
bool Combobox::isSortEnabled(void) const
{
	return d_droplist->isSortEnabled();
}


/*************************************************************************
	return whether the string at index position 'index' is selected	
*************************************************************************/
bool Combobox::isItemSelected(size_t index) const
{
	return d_droplist->isItemSelected(index);
}


/*************************************************************************
	Search the list for an item with the specified text	
*************************************************************************/
ListboxItem* Combobox::findItemWithText(const String& text, const ListboxItem* start_item)
{
	return d_droplist->findItemWithText(text, start_item);
}


/*************************************************************************
	Return whether the specified ListboxItem is in the List	
*************************************************************************/
bool Combobox::isListboxItemInList(const ListboxItem* item) const
{
	return d_droplist->isListboxItemInList(item);
}


/*************************************************************************
	Remove all items from the list.	
*************************************************************************/
void Combobox::resetList(void)
{
	d_droplist->resetList();
}


/*************************************************************************
	Add the given ListboxItem to the list.	
*************************************************************************/
void Combobox::addItem(ListboxItem* item)
{
	d_droplist->addItem(item);
}


/*************************************************************************
	Insert an item into the list box after a specified item already in
	the list.	
*************************************************************************/
void Combobox::insertItem(ListboxItem* item, const ListboxItem* position)
{
	d_droplist->insertItem(item, position);
}


/*************************************************************************
	Removes the given item from the list box.	
*************************************************************************/
void Combobox::removeItem(const ListboxItem* item)
{
	d_droplist->removeItem(item);
}


/*************************************************************************
	Clear the selected state for all items.	
*************************************************************************/
void Combobox::clearAllSelections(void)
{
	d_droplist->clearAllSelections();
}


/*************************************************************************
	Set whether the list should be sorted.	
*************************************************************************/
void Combobox::setSortingEnabled(bool setting)
{
	d_droplist->setSortingEnabled(setting);
}


/*************************************************************************
	Set whether the vertical scroll bar should always be shown.	
*************************************************************************/
void Combobox::setShowVertScrollbar(bool setting)
{
	d_droplist->setShowVertScrollbar(setting);
}


/*************************************************************************
	Set whether the horizontal scroll bar should always be shown.	
*************************************************************************/
void Combobox::setShowHorzScrollbar(bool setting)
{
	d_droplist->setShowHorzScrollbar(setting);
}


/*************************************************************************
	Set the select state of an attached ListboxItem.	
*************************************************************************/
void Combobox::setItemSelectState(ListboxItem* item, bool state)
{
	d_droplist->setItemSelectState(item, state);
}


/*************************************************************************
	Set the select state of an attached ListboxItem.	
*************************************************************************/
void Combobox::setItemSelectState(size_t item_index, bool state)
{
	d_droplist->setItemSelectState(item_index, state);
}


/*************************************************************************
	Causes the list box to update it's internal state after changes have
	been made to one or more attached ListboxItem objects.
*************************************************************************/
void Combobox::handleUpdatedListItemData(void)
{
	d_droplist->handleUpdatedItemData();
}


/*************************************************************************
	Add combo box specific events
*************************************************************************/
void Combobox::addComboboxEvents(bool bCommon)
{
    if ( bCommon == false )
    {
        addEvent(EventReadOnlyModeChanged);
        addEvent(EventValidationStringChanged);
        addEvent(EventMaximumTextLengthChanged);
        addEvent(EventTextInvalidated);
        addEvent(EventInvalidEntryAttempted);
        addEvent(EventCaratMoved);
        addEvent(EventTextSelectionChanged);
        addEvent(EventEditboxFull);
        addEvent(EventListContentsChanged);
        addEvent(EventListSelectionChanged);
        addEvent(EventSortModeChanged);
        addEvent(EventVertScrollbarModeChanged);
        addEvent(EventHorzScrollbarModeChanged);
        addEvent(EventDropListDisplayed);
        addEvent(EventDropListRemoved);
        addEvent(EventListSelectionAccepted);
    }
    else
    {
        addEvent(EventTextAccepted);
    }
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
void Combobox::onTextInvalidatedEvent(WindowEventArgs& e)
{
	fireEvent(EventTextInvalidated, e, EventNamespace);
}


/*************************************************************************
	Handler for when 
*************************************************************************/
void Combobox::onInvalidEntryAttempted(WindowEventArgs& e)
{
	fireEvent(EventInvalidEntryAttempted, e, EventNamespace);
}


/*************************************************************************
	Handler for when 
*************************************************************************/
void Combobox::onCaratMoved(WindowEventArgs& e)
{
	fireEvent(EventCaratMoved, e, EventNamespace);
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
	fireEvent(EventDropListDisplayed, e, EventNamespace);
}


/*************************************************************************
	Handler for when 
*************************************************************************/
void Combobox::onDroplistRemoved(WindowEventArgs& e)
{
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
    d_editbox->setFont(getFont());
    d_droplist->setFont(getFont());

    // Call base class handler
    Window::onFontChanged(e);
}


/*************************************************************************
	Handler for when text changes
*************************************************************************/
void Combobox::onTextChanged(WindowEventArgs& e)
{
	// update ourselves only if needed (prevents perpetual event loop & stack overflow)
	if (d_editbox->getText() != d_text)
	{
		// done before doing base class processing so event subscribers see 'updated' version of this.
		d_editbox->setText(d_text);
		e.handled = true;

		Window::onTextChanged(e);
	}

}



/*************************************************************************
	Handler function for button clicks.
*************************************************************************/
bool Combobox::button_PressHandler(const EventArgs& e)
{
	showDropList();

	// if there is an item with the same text as the edit box, pre-select it
	ListboxItem* item = d_droplist->findItemWithText(d_editbox->getText(), NULL);

	if (item != NULL)
	{
		d_droplist->setItemSelectState(item, true);
		d_droplist->ensureItemIsVisible(item);
	}
	// no matching item, so select nothing
	else
	{
		d_droplist->clearAllSelections();
	}

	return true;
}


/*************************************************************************
	Handler for selections made in the drop-list
*************************************************************************/
bool Combobox::droplist_SelectionAcceptedHandler(const EventArgs& e)
{
	// copy the text from the selected item into the edit box
	ListboxItem* item = ((ComboDropList*)((WindowEventArgs&)e).window)->getFirstSelectedItem();

	if (item != NULL)
	{
		// Put the text from the list item into the edit box
		d_editbox->setText(item->getText());

		// select text if it's editable, and move carat to end
		if (!isReadOnly())
		{
			d_editbox->setSelection(0, item->getText().length());
			d_editbox->setCaratIndex(item->getText().length());
		}

		d_editbox->setCaratIndex(0);

		// fire off an event of our own
		WindowEventArgs args(this);
		onListSelectionAccepted(args);

		// finally, activate the edit box
		d_editbox->activate();
	}

	return true;
}


/*************************************************************************
	Handler for when drop-list hides itself
*************************************************************************/
bool Combobox::droplist_HiddenHandler(const EventArgs& e)
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
		// if edit box is read-only, show list
		if (d_editbox->isReadOnly())
		{
			showDropList();

			// if there is an item with the same text as the edit box, pre-select it
			ListboxItem* item = d_droplist->findItemWithText(d_editbox->getText(), NULL);

			if (item != NULL)
			{
				d_droplist->setItemSelectState(item, true);
				d_droplist->ensureItemIsVisible(item);
			}
			// no matching item, so select nothing
			else
			{
				d_droplist->clearAllSelections();
			}

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
	return d_droplist->isVertScrollbarAlwaysShown();
}


/*************************************************************************
	Return whether the horizontal scroll bar is always shown.	
*************************************************************************/
bool Combobox::isHorzScrollbarAlwaysShown(void) const
{
	return d_droplist->isHorzScrollbarAlwaysShown();
}


/*************************************************************************
	Add properties for this class
*************************************************************************/
void Combobox::addComboboxProperties( bool bCommon )
{
    if ( bCommon == false )
    {
        addProperty(&d_sortProperty);
        addProperty(&d_forceHorzProperty);
        addProperty(&d_forceVertProperty);
        addProperty(&d_readOnlyProperty);
        addProperty(&d_validationStringProperty);
        addProperty(&d_maxTextLengthProperty);
        addProperty(&d_selStartProperty);
        addProperty(&d_selLengthProperty);
        addProperty(&d_selectedTextColourProperty);
        addProperty(&d_activeSelectionColourProperty);
        addProperty(&d_inactiveSelectionColourProperty);
        addProperty(&d_caratIndexProperty);
        addProperty(&d_singleClickOperationProperty);
    }
    else
    {
        addProperty(&d_normalTextColourProperty);
    }
}


/*************************************************************************
	Activate the edit box component of the Combobox.
*************************************************************************/
void Combobox::activateEditbox(void)
{
	if (!d_editbox->isActive())
	{
		d_editbox->activate();
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
	return d_droplist->isVisible();
}


/*************************************************************************
	Set the operation mode for the combo box.
*************************************************************************/
void Combobox::setSingleClickEnabled(bool setting)
{
	d_singleClickOperation = setting;
	d_droplist->setAutoArmEnabled(setting);
}


//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Handlers to relay child widget events so they appear to come from us
*************************************************************************/
//////////////////////////////////////////////////////////////////////////
bool Combobox::editbox_ReadOnlyChangedHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onReadOnlyChanged(args);

	return true;
}


bool Combobox::editbox_ValidationStringChangedHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onValidationStringChanged(args);

	return true;
}


bool Combobox::editbox_MaximumTextLengthChangedHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onMaximumTextLengthChanged(args);

	return true;
}


bool Combobox::editbox_TextInvalidatedEventHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onTextInvalidatedEvent(args);

	return true;
}


bool Combobox::editbox_InvalidEntryAttemptedHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onInvalidEntryAttempted(args);

	return true;
}


bool Combobox::editbox_CaratMovedHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onCaratMoved(args);

	return true;
}


bool Combobox::editbox_TextSelectionChangedHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onTextSelectionChanged(args);

	return true;
}


bool Combobox::editbox_EditboxFullEventHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onEditboxFullEvent(args);

	return true;
}


bool Combobox::editbox_TextAcceptedEventHandler(const EventArgs& e)
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


bool Combobox::listbox_ListContentsChangedHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onListContentsChanged(args);

	return true;
}


bool Combobox::listbox_ListSelectionChangedHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onListSelectionChanged(args);

	return true;
}


bool Combobox::listbox_SortModeChangedHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onSortModeChanged(args);

	return true;
}


bool Combobox::listbox_VertScrollModeChangedHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onVertScrollbarModeChanged(args);

	return true;
}


bool Combobox::listbox_HorzScrollModeChangedHandler(const EventArgs& e)
{
	WindowEventArgs	args(this);
	onHorzScrollbarModeChanged(args);

	return true;
}

} // End of  CEGUI namespace section
