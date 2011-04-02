/************************************************************************
	filename: 	CEGUITabControl.cpp
	created:	08/08/2004
	author:		Steve Streeting
	
	purpose:	Implementation of Tab Control widget base class
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
#include "CEGUIExceptions.h"
#include "elements/CEGUITabControl.h"
#include "elements/CEGUITabButton.h"
#include "elements/CEGUIStatic.h"
#include "elements/CEGUIGUISheet.h"
#include "CEGUIFont.h"
#include "CEGUIWindowManager.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String TabControl::EventNamespace("TabControl");

/*************************************************************************
	Definition of Properties for this class
*************************************************************************/
TabControlProperties::TabHeight		            TabControl::d_tabHeightProperty;
TabControlProperties::AbsoluteTabHeight	        TabControl::d_absoluteTabHeightProperty;
TabControlProperties::RelativeTabHeight	        TabControl::d_relativeTabHeightProperty;
TabControlProperties::TabTextPadding		    TabControl::d_tabTextPaddingProperty;
TabControlProperties::AbsoluteTabTextPadding	TabControl::d_absoluteTabTextPaddingProperty;
TabControlProperties::RelativeTabTextPadding	TabControl::d_relativeTabTextPaddingProperty;

/*************************************************************************
	Constants
*************************************************************************/
// event names
const String TabControl::EventSelectionChanged( (utf8*)"TabSelectionChanged" );

	
/*************************************************************************
	Constructor for TabControl base class.
*************************************************************************/
TabControl::TabControl(const String& type, const String& name)
	: Window(type, name),
    d_tabButtonPane(NULL),
    d_tabContentPane(NULL),
    d_nextTabIndex(0)
{
	addTabControlEvents();
	addTabControlProperties();
    setRelativeTabHeight(0.05f);
    setAbsoluteTabTextPadding(5);
}


/*************************************************************************
	Destructor for TabControl base class.
*************************************************************************/
TabControl::~TabControl(void)
{
    // Should be handled in superclass (all child windows)
}

/*************************************************************************
	Initialise the Window based object ready for use.
*************************************************************************/
void TabControl::initialise(void)
{
	// create the component sub-widgets
	d_tabContentPane = createTabContentPane(getName() + "__auto_TabPane__");
    d_tabButtonPane = createTabButtonPane(getName() + "__auto_TabPane__Buttons");

	addChildWindow(d_tabContentPane);
    addChildWindow(d_tabButtonPane);

	performChildWindowLayout();
}
/*************************************************************************
Get the number of tabs
*************************************************************************/
uint TabControl::getTabCount(void) const
{   
    return d_tabContentPane->getChildCount();
}
/*************************************************************************
Get the tab with a given name
*************************************************************************/
Window*	TabControl::getTabContents(const String& name) const
{
    return d_tabContentPane->getChild(name);
}
/*************************************************************************
Get the tab at a given ID
*************************************************************************/
Window*	TabControl::getTabContents(uint ID) const
{
    return d_tabContentPane->getChild(ID);
}
/*************************************************************************
Get the tab for the given index
*************************************************************************/
Window*	TabControl::getTabContentsAtIndex(uint index) const
{
    return d_tabButtonIndexMap.find(index)->second->getTargetWindow();
}

/*************************************************************************
Return whether the tab content window is currently selected.
*************************************************************************/
bool TabControl::isTabContentsSelected(Window* wnd) const
{
	TabButton* button = getButtonForTabContents(wnd);
	return button->isSelected();
}

/*************************************************************************
Return whether the tab content window is currently selected.
*************************************************************************/
uint TabControl::getSelectedTabIndex() const
{
    uint index;
    TabButtonIndexMap::const_iterator i, iend;
    iend = d_tabButtonIndexMap.end();
    for (i = d_tabButtonIndexMap.begin(); i != iend; ++i)
    {
        // get corresponding tab button and content window
        TabButton* tb = i->second;
        if (tb->isSelected())
        {
            index = i->first;
			break;
        }
	}
	return index;
}

/*************************************************************************
Set the selected tab by window name
*************************************************************************/
void TabControl::setSelectedTab(const String &name)
{
    // get window
    Window* wnd = d_tabContentPane->getChild(name);

    selectTab_impl(wnd);
}
/*************************************************************************
Set the selected tab by window ID
*************************************************************************/
void TabControl::setSelectedTab(uint ID)
{
    // get window
    Window* wnd = d_tabContentPane->getChild(ID);

    selectTab_impl(wnd);
}
/*************************************************************************
Set the selected tab by window name
*************************************************************************/
void TabControl::setSelectedTabAtIndex(uint index)
{
	Window* wnd = getTabContentsAtIndex(index);
	selectTab_impl(wnd);
}
/*************************************************************************
Get the tab height
*************************************************************************/
float TabControl::getTabHeight(void) const
{
    MetricsMode mode = getMetricsMode();
    if (mode == Relative)
    {
        return d_rel_tabHeight;
    }
    else
    {
        return d_abs_tabHeight;
    }
}

/*************************************************************************
Set the tab height
*************************************************************************/
void TabControl::setRelativeTabHeight(float height)
{
    d_rel_tabHeight = height;
    d_abs_tabHeight = relativeToAbsoluteY(height);

    performChildWindowLayout();
}
/*************************************************************************
Set the tab height
*************************************************************************/
void TabControl::setAbsoluteTabHeight(float height)
{
    d_abs_tabHeight = height;
    d_rel_tabHeight = absoluteToRelativeY(height);

    performChildWindowLayout();
}
/*************************************************************************
Set the tab height
*************************************************************************/
void TabControl::setTabHeight(float height)
{
    if (getMetricsMode() == Relative)
    {
        setRelativeTabHeight(height);
    }
    else
    {
        setAbsoluteTabHeight(height);
    }
}
/*************************************************************************
Get the tab text padding
*************************************************************************/
float TabControl::getTabTextPadding(void) const
{
    MetricsMode mode = getMetricsMode();
    if (mode == Relative)
    {
        return d_rel_tabPadding;
    }
    else
    {
        return d_abs_tabPadding;
    }
}

/*************************************************************************
Set the tab text padding
*************************************************************************/
void TabControl::setRelativeTabTextPadding(float height)
{
    d_rel_tabPadding = height;
    d_abs_tabPadding = relativeToAbsoluteY(height);

    performChildWindowLayout();
}
/*************************************************************************
Set the tab text padding
*************************************************************************/
void TabControl::setAbsoluteTabTextPadding(float height)
{
    d_abs_tabPadding = height;
    d_rel_tabPadding = absoluteToRelativeY(height);

    performChildWindowLayout();
}
/*************************************************************************
Set the tab text padding
*************************************************************************/
void TabControl::setTabTextPadding(float height)
{
    if (getMetricsMode() == Relative)
    {
        setRelativeTabTextPadding(height);
    }
    else
    {
        setAbsoluteTabTextPadding(height);
    }
}

/*************************************************************************
Add a new tab
*************************************************************************/
void TabControl::addTab(Window* wnd)
{
    // Create a new TabButton
    addButtonForTabContent(wnd);
    // Add the window to the content pane
    d_tabContentPane->addChildWindow(wnd);
    // Auto-select?
    if (getTabCount() == 1)
    {
        setSelectedTab(wnd->getName());
    }
    else
    {
        // initialise invisible content
        wnd->setVisible(false);
    }
    // Just request redraw
    performChildWindowLayout();
    requestRedraw();
    // Subscribe to text changed event so that we can resize as needed
    wnd->subscribeEvent(Window::EventTextChanged, 
        Event::Subscriber(&TabControl::handleContentWindowTextChanged, this));

}
/*************************************************************************
Remove a tab
*************************************************************************/
void TabControl::removeTab(const String& name)
{
    // do nothing if given window is not attached as a tab.
    if (!d_tabContentPane->isChild(name))
        return;

    Window* wnd = d_tabContentPane->getChild(name);
    // Was this selected?
    bool reselect = wnd->isVisible();
    // Tab buttons are the 2nd onward children
    d_tabContentPane->removeChildWindow(name);

    // remove button too
    removeButtonForTabContent(wnd);

    if (reselect)
    {
        // Select another tab
        if (getTabCount() > 0)
        {
            setSelectedTab(d_tabContentPane->getChildAtIdx(0)->getName());
        }
    }

    performChildWindowLayout();

    requestRedraw();
}
/*************************************************************************
Remove a tab by ID
*************************************************************************/
void TabControl::removeTab(uint ID)
{
    // do nothing if given window is not attached as a tab.
    if (!d_tabContentPane->isChild(ID))
        return;

    Window* wnd = d_tabContentPane->getChild(ID);
    // Was this selected?
    bool reselect = wnd->isVisible();
    // Tab buttons are the 2nd onward children
    d_tabContentPane->removeChildWindow(ID);

    // remove button too
    removeButtonForTabContent(wnd);

    if (reselect)
    {
        // Select another tab
        if (getTabCount() > 0)
        {
            setSelectedTab(d_tabContentPane->getChildAtIdx(0)->getName());
        }
    }

    performChildWindowLayout();

    requestRedraw();

}
/*************************************************************************
Add tab button
*************************************************************************/
void TabControl::addButtonForTabContent(Window* wnd)
{
    // Create the button
    TabButton* tb = createTabButton(makeButtonName(wnd));
    // Copy font
    tb->setFont(getFont());
    // Set target window
    tb->setTargetWindow(wnd);
    // Set index
    tb->setTabIndex(d_nextTabIndex++);
    // Instert into map
    d_tabButtonIndexMap.insert(
        TabButtonIndexMap::value_type(tb->getTabIndex(), tb));
    // add the button
    d_tabButtonPane->addChildWindow(tb);
    // Subscribe to clicked event so that we can change tab
    tb->subscribeEvent(TabButton::EventClicked, 
        Event::Subscriber(&TabControl::handleTabButtonClicked, this));

}

/*************************************************************************
	Return the tab button for the given tab content window
*************************************************************************/
TabButton* TabControl::getButtonForTabContents(Window* wnd) const
{
    TabButtonIndexMap::const_iterator i, iend;
    iend = d_tabButtonIndexMap.end();
    for (i = d_tabButtonIndexMap.begin(); i != iend; ++i)
    {
        // get corresponding tab button and content window
        TabButton* tb = i->second;
        Window* child = tb->getTargetWindow();
        if (child == wnd)
        {
			return tb;
        }
	}
	throw UnknownObjectException((utf8*)"TabControl::getButtonForTabContents - The Window object is not a tab contents.");
}
/*************************************************************************
	Calculate size and position for a tab button
*************************************************************************/
void TabControl::calculateTabButtonSizePosition(TabButton* btn, uint targetIndex)
{
    // relative height is always 1.0 for buttons since they are embedded in a
    // panel of the correct height already
    btn->setHeight(Relative, 1.0f);
    btn->setYPosition(Relative, 0.0f);
    // x position is based on previous button
    if (targetIndex > 0)
    {
		TabButtonIndexMap::iterator iter = d_tabButtonIndexMap.begin();
		std::advance(iter, targetIndex - 1);
		Window* prevButton = iter->second;

		// position is prev pos + width
        btn->setXPosition(Relative, 
            prevButton->getXPosition(Relative) 
            + prevButton->getWidth(Relative));
    }
    else
    {
        // First button
        btn->setXPosition(Relative, 0);
    }
    // Width is based on font size (expressed as absolute)
    const Font* fnt = btn->getFont();
    btn->setWidth(Absolute, 
        fnt->getTextExtent(btn->getText()) + getAbsoluteTabTextPadding()*2);
    btn->requestRedraw();
}
/*************************************************************************
Remove tab button
*************************************************************************/
void TabControl::removeButtonForTabContent(Window* wnd)
{
    // get
    TabButton* tb = static_cast<TabButton*>(
        d_tabButtonPane->getChild(makeButtonName(wnd)));
    // remove
    d_tabButtonIndexMap.erase(tb->getTabIndex());
    d_tabButtonPane->removeChildWindow(tb);
	// destroy
	WindowManager::getSingleton().destroyWindow(tb);
}
/*************************************************************************
Remove tab button
*************************************************************************/
String TabControl::makeButtonName(Window* wnd)
{
    // derive button name
    String buttonName = (utf8*)"__auto_btn";
    buttonName.append(wnd->getName());
    return buttonName;
}
/*************************************************************************
Select tab implementation
*************************************************************************/
void TabControl::selectTab_impl(Window* wnd)
{
    bool modified = false;
    bool foundSelected = false;
    // Iterate in order of tab index
    TabButtonIndexMap::iterator i, iend;
    iend = d_tabButtonIndexMap.end();
    for (i = d_tabButtonIndexMap.begin(); i != iend; ++i)
    {
        // get corresponding tab button and content window
        TabButton* tb = i->second;
        Window* child = tb->getTargetWindow();
        // Should we be selecting?
        bool selectThis = (child == wnd);
        // Are we modifying this tab?
        modified = modified || (tb->isSelected() != selectThis);
        foundSelected = foundSelected || selectThis;
        // Select tab & set visible if this is the window, not otherwise
        tb->setSelected(selectThis);
        tb->setRightOfSelected(foundSelected);
        child->setVisible(selectThis);
    }
    // Trigger event?
    if (modified)
    {
        WindowEventArgs args(this);
        onSelectionChanged(args);
    }
}
/*************************************************************************
Add tab control properties
*************************************************************************/
void TabControl::addTabControlProperties( bool bCommon )
{
    if ( bCommon == false )
    {
        addProperty(&d_tabHeightProperty);
        addProperty(&d_relativeTabHeightProperty);
        addProperty(&d_absoluteTabHeightProperty);
        addProperty(&d_tabTextPaddingProperty);
        addProperty(&d_relativeTabTextPaddingProperty);
        addProperty(&d_absoluteTabTextPaddingProperty);
    }
}
/*************************************************************************
Internal version of adding a child window
*************************************************************************/
void TabControl::addChild_impl(Window* wnd)
{
    // Look for __auto_TabPane__ in the name (hopefully no-one will use this!)
    if (wnd->getName().find((const utf8*)"__auto_TabPane__") != String::npos)
    {
        // perform normal addChild
        Window::addChild_impl(wnd);
    }
    else
    {
        // This is another control, therefore add as a tab
        addTab(wnd);
    }
}
/*************************************************************************
Internal version of removing a child window
*************************************************************************/
void TabControl::removeChild_impl(Window* wnd)
{
    // protect against possible null pointers
    if (!wnd) return;

    // Look for __auto_TabPane__ in the name (hopefully no-one will use this!)
    if (wnd->getName().find((const utf8*)"__auto_TabPane__") != String::npos)
    {
        // perform normal removeChild
        Window::removeChild_impl(wnd);
    }
    else
    {
        // This is some user window, therefore remove as a tab
        removeTab(wnd->getName());
    }
}
/*************************************************************************
Selection changed event
*************************************************************************/
void TabControl::onSelectionChanged(WindowEventArgs& e)
{
    requestRedraw();
    fireEvent(EventSelectionChanged, e, EventNamespace);
}
/*************************************************************************
Font changed event
*************************************************************************/
void TabControl::onFontChanged(WindowEventArgs& e)
{
    // Propagate font change to buttons
    TabButtonIndexMap::iterator i, iend;
    iend = d_tabButtonIndexMap.end();
    for (i = d_tabButtonIndexMap.end(); i != iend; ++i)
    {
        i->second->setFont(getFont());
    }
}
/*************************************************************************
Add events for this class
*************************************************************************/
void TabControl::addTabControlEvents(bool bCommon)
{
    if ( bCommon == true )	addEvent(EventSelectionChanged);
}
/*************************************************************************
Layout the widgets
*************************************************************************/
void TabControl::performChildWindowLayout()
{
    Window::performChildWindowLayout();

    if (d_tabButtonPane)
    {
        // Set the size of the tab button area (full width, height from tab height)
        d_tabButtonPane->setSize(Relative, Size(1.0f, d_rel_tabHeight) );
        //d_tabButtonPane->setPosition(Relative, Point(0.0f, 0.0f) );
        // Calculate the positions and sizes of the tab buttons
        TabButtonIndexMap::iterator i, iend;
        iend = d_tabButtonIndexMap.end();
        uint x = 0;
        for (i = d_tabButtonIndexMap.begin(); i != iend; ++i)
        {
            TabButton* btn = i->second;
            if ( btn->isVisible() )
            {
                calculateTabButtonSizePosition(btn, x);
                ++x;
            }
        }
    }
    if (d_tabContentPane)
    {
        // Set the size of the content area
        d_tabContentPane->setSize(Relative, Size(1.0f, 1.0f - d_rel_tabHeight) );
        d_tabContentPane->setPosition(Relative, Point(0.0f, d_rel_tabHeight) );
    }

}
/*************************************************************************
Create tab button pane
*************************************************************************/
Window*	TabControl::createTabButtonPane(const String& name) const
{
	return WindowManager::getSingleton().createWindow(GUISheet::WidgetTypeName, name);
}
/*************************************************************************
Text changed on a content window
*************************************************************************/
bool TabControl::handleContentWindowTextChanged(const EventArgs& args)
{
    // update text
    const WindowEventArgs& wargs = static_cast<const WindowEventArgs&>(args);
    Window* tabButton = d_tabButtonPane->getChild(
        makeButtonName(wargs.window));
    tabButton->setText(wargs.window->getText());
    // sort out the layout
    performChildWindowLayout();
	requestRedraw();

	return true;
}
/*************************************************************************
Tab button clicked
*************************************************************************/
bool TabControl::handleTabButtonClicked(const EventArgs& args)
{
    const WindowEventArgs& wargs = static_cast<const WindowEventArgs&>(args);
    TabButton* tabButton = static_cast<TabButton*>(wargs.window);
    setSelectedTab(tabButton->getTargetWindow()->getName());

	return true;
}

int TabControl::writeChildWindowsXML(OutStream& out_stream) const
{
    int childOutputCount = Window::writeChildWindowsXML(out_stream);

    // since TabControl content is actually added to the component tab
    // content pane window, this overridden function exists to dump those
    // out as if they were our own children.
    for (uint i = 0; i < getTabCount(); ++i)
    {
        getTabContentsAtIndex(i)->writeXMLToStream(out_stream);
        ++childOutputCount;
    }

    return childOutputCount;
}

} // End of  CEGUI namespace section
