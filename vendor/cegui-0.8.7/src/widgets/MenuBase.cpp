/***********************************************************************
    created:    5/4/2005
    author:     Tomas Lindquist Olsen (based on code by Paul D Turner)

    purpose:    Implementation of MenuBase widget base class
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
#include "CEGUI/widgets/MenuBase.h"
#include "CEGUI/widgets/PopupMenu.h"
#include "CEGUI/widgets/MenuItem.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
    Constants
*************************************************************************/
// event strings
const String MenuBase::EventNamespace("MenuBase");
const String MenuBase::EventPopupOpened("PopupOpened");
const String MenuBase::EventPopupClosed("PopupClosed");

/*************************************************************************
    Constructor for MenuBase base class.
*************************************************************************/
MenuBase::MenuBase(const String& type, const String& name)
    : ItemListBase(type, name),
      d_itemSpacing(0.0f),
      d_popupItem(0),
      d_allowMultiplePopups(false),
      d_autoCloseNestedPopups(false)
{
    // add properties for MenuBase class
    addMenuBaseProperties();
}

/*************************************************************************
    Destructor for MenuBase base class.
*************************************************************************/
MenuBase::~MenuBase(void)
{
}

/*************************************************************************
    Change the currently open MenuItem PopupMenu
*************************************************************************/
void MenuBase::changePopupMenuItem(MenuItem* item)
{
    if (!d_allowMultiplePopups && d_popupItem == item)
        return;

    if (!d_allowMultiplePopups && d_popupItem != 0)
    {
        WindowEventArgs we(d_popupItem->getPopupMenu());
        d_popupItem->closePopupMenu(false);
        d_popupItem = 0;
        onPopupClosed(we);
    }

    if (item)
    {
        d_popupItem = item;
        d_popupItem->openPopupMenu(false);
        WindowEventArgs we(d_popupItem->getPopupMenu());
        onPopupOpened(we);
    }

}


/*************************************************************************
    handler invoked internally when the a MenuItem attached to this
    MenuBase opens its popup.
*************************************************************************/
void MenuBase::onPopupOpened(WindowEventArgs& e)
{
    fireEvent(EventPopupOpened, e, EventNamespace);
}


/*************************************************************************
    handler invoked internally when the a MenuItem attached to this
    MenuBase closes its popup.
*************************************************************************/
void MenuBase::onPopupClosed(WindowEventArgs& e)
{
    fireEvent(EventPopupClosed, e, EventNamespace);
}


/************************************************************************
    Add properties for this widget
*************************************************************************/
void MenuBase::addMenuBaseProperties(void)
{
    const String propertyOrigin = "CEGUI/MenuBase";

    CEGUI_DEFINE_PROPERTY(MenuBase, float,
        "ItemSpacing", "Property to get/set the item spacing of the menu.  Value is a float.",
        &MenuBase::setItemSpacing, &MenuBase::getItemSpacing, 10.0f
    );
    
    CEGUI_DEFINE_PROPERTY(MenuBase, bool,
        "AllowMultiplePopups", "Property to get/set the state of the allow multiple popups setting for the menu.  Value is either \"true\" or \"false\".",
        &MenuBase::setAllowMultiplePopups, &MenuBase::isMultiplePopupsAllowed, false /* TODO: Inconsistency and awful English */
    );
    
    CEGUI_DEFINE_PROPERTY(MenuBase, bool,
        "AutoCloseNestedPopups", "Property to set if the menu should close all its open child popups, when it gets hidden. Value is either \"true\" or \"false\".",
        &MenuBase::setAutoCloseNestedPopups, &MenuBase::getAutoCloseNestedPopups, false
    );
}


/************************************************************************
    Set if multiple child popup menus are allowed simultaneously
*************************************************************************/
void MenuBase::setAllowMultiplePopups(bool setting)
{
    if (d_allowMultiplePopups != setting)
    {
        // TODO :
        // close all popups except perhaps the last one opened!
        d_allowMultiplePopups = setting;
    }
}

void MenuBase::setPopupMenuItemClosing()
{
    if (d_popupItem)
    {
        d_popupItem->startPopupClosing();
    }
}

//----------------------------------------------------------------------------//
void MenuBase::onChildRemoved(ElementEventArgs& e)
{
    // if the removed window was our tracked popup item, zero ptr to it.
    if (static_cast<Window*>(e.element) == d_popupItem)
        d_popupItem = 0;

    // base class version
    ItemListBase::onChildRemoved(e);
}

void MenuBase::onHidden(WindowEventArgs&)
{
    if (!getAutoCloseNestedPopups())
        return;

    changePopupMenuItem(0);

    if (d_allowMultiplePopups)
    {
        for (size_t i = 0; i < d_listItems.size(); ++i)
        {
            if (!d_listItems[i])
                continue;

            MenuItem* menuItem = dynamic_cast<MenuItem*>(d_listItems[i]);
            if (!menuItem)
                continue;

            if (!menuItem->getPopupMenu())
                continue;

            WindowEventArgs we(menuItem->getPopupMenu());
            menuItem->closePopupMenu(false);
            onPopupClosed(we);
        }
    }
}
//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
