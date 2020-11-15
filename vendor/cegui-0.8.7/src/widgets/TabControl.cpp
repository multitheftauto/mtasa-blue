/***********************************************************************
	created:	08/08/2004
	author:		Steve Streeting

	purpose:	Implementation of Tab Control widget base class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2015 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/widgets/TabControl.h"
#include "CEGUI/widgets/TabButton.h"
#include "CEGUI/widgets/PushButton.h"
#include "CEGUI/widgets/DefaultWindow.h"
#include "CEGUI/Font.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/CoordConverter.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String TabControl::EventNamespace("TabControl");
const String TabControl::WidgetTypeName("CEGUI/TabControl");

/*************************************************************************
    TabControlWindowRenderer
*************************************************************************/
TabControlWindowRenderer::TabControlWindowRenderer(const String& name) :
    WindowRenderer(name, TabControl::EventNamespace)
{
}

/*************************************************************************
	Constants
*************************************************************************/
// event names
const String TabControl::EventSelectionChanged( "SelectionChanged" );

/*************************************************************************
    Child Widget name constants
*************************************************************************/
const String TabControl::ContentPaneName( "__auto_TabPane__" );
const String TabControl::TabButtonName( "__auto_btn" );
const String TabControl::TabButtonPaneName( "__auto_TabPane__Buttons" );
const String TabControl::ButtonScrollLeft( "__auto_TabPane__ScrollLeft" );
const String TabControl::ButtonScrollRight( "__auto_TabPane__ScrollRight" );

/*************************************************************************
    Miscelaneous private strings
*************************************************************************/
static const String EnableTop = "EnableTop";
static const String EnableBottom = "EnableBottom";
static const String n0 = "0";
static const String n1 = "1";

/*************************************************************************
	Constructor for TabControl base class.
*************************************************************************/
TabControl::TabControl(const String& type, const String& name)
    : Window(type, name),
    d_tabHeight(0, -1), // means 'to be initialized later'
    d_tabPadding(0, 5),
    d_firstTabOffset(0),
    d_tabPanePos(Top)
{
	addTabControlProperties();
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
void TabControl::initialiseComponents(void)
{
    // ban properties forwarded from here
    if (isChild(ButtonScrollLeft))
    {
        CEGUI::Window* buttonScrollLeft = getChild(ButtonScrollLeft);
        buttonScrollLeft->banPropertyFromXML(Window::VisiblePropertyName);
        buttonScrollLeft->banPropertyFromXML(Window::WantsMultiClickEventsPropertyName);
    }
    if (isChild(ButtonScrollRight))
    {
        CEGUI::Window* buttonScrollRight = getChild(ButtonScrollRight);
        buttonScrollRight->banPropertyFromXML(Window::VisiblePropertyName);
        buttonScrollRight->banPropertyFromXML(Window::WantsMultiClickEventsPropertyName);
    }

	performChildWindowLayout();

    if (isChild(ButtonScrollLeft))
        getChild(ButtonScrollLeft)->subscribeEvent (
            PushButton::EventClicked, Event::Subscriber(
                &CEGUI::TabControl::handleScrollPane, this));

    if (isChild(ButtonScrollRight))
        getChild(ButtonScrollRight)->subscribeEvent (
            PushButton::EventClicked, Event::Subscriber(
                &CEGUI::TabControl::handleScrollPane, this));
}
/*************************************************************************
Get the number of tabs
*************************************************************************/
size_t TabControl::getTabCount(void) const
{
    return getTabPane()->getChildCount();
}
/*************************************************************************
Get the tab with a given name
*************************************************************************/
Window*	TabControl::getTabContents(const String& name) const
{
    return getTabPane()->getChild(name);
}
/*************************************************************************
Get the tab at a given ID
*************************************************************************/
Window*	TabControl::getTabContents(uint ID) const
{
    return getTabPane()->getChild(ID);
}
/*************************************************************************
Get the tab for the given index
*************************************************************************/
Window*	TabControl::getTabContentsAtIndex(size_t index) const
{
    if (index >= d_tabButtonVector.size ())
        return 0;
    return d_tabButtonVector [index]->getTargetWindow();
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
size_t TabControl::getSelectedTabIndex() const
{
    for (size_t i = 0; i < d_tabButtonVector.size (); ++i)
        if (d_tabButtonVector [i]->isSelected ())
            return i;

	CEGUI_THROW(UnknownObjectException("Current tab not in list?"));
}

/*************************************************************************
Set the selected tab by window name
*************************************************************************/
void TabControl::setSelectedTab(const String &name)
{
    selectTab_impl(getTabPane()->getChild(name));
}
/*************************************************************************
Set the selected tab by window ID
*************************************************************************/
void TabControl::setSelectedTab(uint ID)
{
    selectTab_impl(getTabPane()->getChild(ID));
}
/*************************************************************************
Set the selected tab by window name
*************************************************************************/
void TabControl::setSelectedTabAtIndex(size_t index)
{
	selectTab_impl(getTabContentsAtIndex(index));
}

/*************************************************************************
Make the tab by window name visible
*************************************************************************/
void TabControl::makeTabVisible(const String &name)
{
    makeTabVisible_impl(getTabPane()->getChild(name));
}
/*************************************************************************
Make the tab by window ID visible
*************************************************************************/
void TabControl::makeTabVisible(uint ID)
{
    makeTabVisible_impl(getTabPane()->getChild(ID));
}
/*************************************************************************
Make the tab by window name visible
*************************************************************************/
void TabControl::makeTabVisibleAtIndex(size_t index)
{
	makeTabVisible_impl(getTabContentsAtIndex(index));
}

/*************************************************************************
Set the tab height
*************************************************************************/
void TabControl::setTabHeight(const UDim& height)
{
    d_tabHeight = height;

    performChildWindowLayout();
}

/*************************************************************************
Set the tab text padding
*************************************************************************/
void TabControl::setTabTextPadding(const UDim& padding)
{
    d_tabPadding = padding;

    performChildWindowLayout();
}

/*************************************************************************
Add a new tab
*************************************************************************/
void TabControl::addTab(Window* wnd)
{
    // abort attempts to add null window pointers, but log it for tracking.
    if (!wnd)
    {
        Logger::getSingleton().logEvent("Attempt to add null window pointer as "
            "tab to TabControl '" + getName() + "'.  Ignoring!", Informative);

        return;
    }

    // Create a new TabButton
    addButtonForTabContent(wnd);
    // Add the window to the content pane
    getTabPane()->addChild(wnd);
    // Auto-select?
    if (getTabCount() == 1)
        setSelectedTab(wnd->getName());
    else
        // initialise invisible content
        wnd->setVisible(false);

	// when adding the 1st page, autosize tab pane height
    if (d_tabHeight.d_scale == 0 && d_tabHeight.d_offset == -1)
        d_tabHeight.d_offset = 8 + getFont()->getFontHeight ();

    // Just request redraw
    performChildWindowLayout();
    invalidate();
    // Subscribe to text changed event so that we can resize as needed
    d_eventConnections[wnd] =
        wnd->subscribeEvent(Window::EventTextChanged,
            Event::Subscriber(&TabControl::handleContentWindowTextChanged,
                              this));
}
/*************************************************************************
Remove a tab
*************************************************************************/
void TabControl::removeTab(const String& name)
{
    // do nothing if given window is not attached as a tab.
    if (getTabPane()->isChild(name))
        removeTab_impl(getTabPane()->getChild(name));
}
/*************************************************************************
Remove a tab by ID
*************************************************************************/
void TabControl::removeTab(uint ID)
{
    // do nothing if given window is not attached as a tab.
    if (getTabPane()->isChild(ID))
        removeTab_impl(getTabPane()->getChild(ID));
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
    // Instert into map
    d_tabButtonVector.push_back(tb);
    // add the button
    getTabButtonPane()->addChild(tb);
    // Subscribe to clicked event so that we can change tab
    tb->subscribeEvent(TabButton::EventClicked,
        Event::Subscriber(&TabControl::handleTabButtonClicked, this));
    tb->subscribeEvent(TabButton::EventDragged,
        Event::Subscriber(&TabControl::handleDraggedPane, this));
    tb->subscribeEvent(TabButton::EventScrolled,
        Event::Subscriber(&TabControl::handleWheeledPane, this));
}

/*************************************************************************
	Return the tab button for the given tab content window
*************************************************************************/
TabButton* TabControl::getButtonForTabContents(Window* wnd) const
{
    for (size_t i = 0; i < d_tabButtonVector.size (); ++i)
        if (d_tabButtonVector [i]->getTargetWindow () == wnd)
            return d_tabButtonVector [i];

	CEGUI_THROW(UnknownObjectException(
        "The Window object is not a tab contents."));
}
/*************************************************************************
Remove tab button
*************************************************************************/
void TabControl::removeButtonForTabContent(Window* wnd)
{
    // get
    TabButton* tb = static_cast<TabButton*>(
        getTabButtonPane()->getChild(makeButtonName(wnd)));
    // remove
    TabButtonVector::iterator i;
    for (i = d_tabButtonVector.begin (); i < d_tabButtonVector.end (); ++i)
        if (*i == tb)
        {
            d_tabButtonVector.erase (i);
            break;
        }
    getTabButtonPane()->removeChild(tb);
	// destroy
	WindowManager::getSingleton().destroyWindow(tb);
}
/*************************************************************************
Remove tab button
*************************************************************************/
String TabControl::makeButtonName(Window* wnd)
{
    return TabButtonName + wnd->getName();
}
/*************************************************************************
Select tab implementation
*************************************************************************/
void TabControl::selectTab_impl(Window* wnd)
{
    makeTabVisible_impl(wnd);

    bool modified = false;
    // Iterate in order of tab index
    for (size_t i = 0; i < d_tabButtonVector.size(); ++i)
    {
        // get corresponding tab button and content window
        TabButton* tb = d_tabButtonVector [i];
        Window* child = tb->getTargetWindow();
        // Should we be selecting?
        bool selectThis = (child == wnd);
        // Are we modifying this tab?
        modified = modified || (tb->isSelected() != selectThis);
        // Select tab & set visible if this is the window, not otherwise
        tb->setSelected(selectThis);
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
Make tab visible implementation
*************************************************************************/
void TabControl::makeTabVisible_impl(Window* wnd)
{
    TabButton *tb = 0;

    for (size_t i = 0; i < d_tabButtonVector.size (); ++i)
    {
        // get corresponding tab button and content window
        tb = d_tabButtonVector [i];
        Window* child = tb->getTargetWindow();
        if (child == wnd)
            break;
        tb = 0;
    }

    if (!tb)
        return;

    float ww = getPixelSize().d_width;
    float x = CoordConverter::asAbsolute(tb->getPosition().d_x, ww);
    float w = tb->getPixelSize().d_width;
    float lx = 0, rx = ww;

    Window *scrollLeftBtn = 0, *scrollRightBtn = 0;
    if (isChild(ButtonScrollLeft))
    {
        scrollLeftBtn = getChild(ButtonScrollLeft);
        lx = CoordConverter::asAbsolute(scrollLeftBtn->getArea().d_max.d_x, ww);
        scrollLeftBtn->setWantsMultiClickEvents(false);
    }

    if (isChild(ButtonScrollRight))
    {
        scrollRightBtn = getChild(ButtonScrollRight);
        rx = CoordConverter::asAbsolute(scrollRightBtn->getPosition().d_x, ww);
        scrollRightBtn->setWantsMultiClickEvents(false);
    }

    if (x < lx)
        d_firstTabOffset += lx - x;
    else
    {
        if (x + w <= rx)
            return; // nothing to do

        d_firstTabOffset += rx - (x + w);
    }

    performChildWindowLayout ();
}
/*************************************************************************
Add tab control properties
*************************************************************************/
void TabControl::addTabControlProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;

    CEGUI_DEFINE_PROPERTY(TabControl, UDim,
        "TabHeight", "Property to get/set the height of the tabs.",
        &TabControl::setTabHeight, &TabControl::getTabHeight, UDim(0.05f,0.0f)
    );
    
    CEGUI_DEFINE_PROPERTY(TabControl, UDim,
        "TabTextPadding", "Property to get/set the padding either side of the tab buttons.",
        &TabControl::setTabTextPadding, &TabControl::getTabTextPadding, UDim(0.0f,0.5f)
    );
    
    CEGUI_DEFINE_PROPERTY(TabControl, TabPanePosition,
        "TabPanePosition", "Property to get/set the position of the buttons pane.",
        &TabControl::setTabPanePosition, &TabControl::getTabPanePosition, TabControl::Top
    );
}
/*************************************************************************
Internal version of adding a child window
*************************************************************************/
void TabControl::addChild_impl(Element* element)
{
    Window* wnd = dynamic_cast<Window*>(element);
    
    if (!wnd)
        CEGUI_THROW(InvalidRequestException(
            "TabControl can only have Elements of type Window added as "
            "children (Window path: " + getNamePath() + ")."));
    
    if (wnd->isAutoWindow())
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
void TabControl::removeChild_impl(Element* element)
{
    Window* wnd = static_cast<Window*>(element);
    
    // protect against possible null pointers
    if (!wnd) return;

    if (wnd->isAutoWindow())
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
    invalidate();
    fireEvent(EventSelectionChanged, e, EventNamespace);
}
/*************************************************************************
Font changed event
*************************************************************************/
void TabControl::onFontChanged(WindowEventArgs&)
{
    // Propagate font change to buttons
    for (size_t i = 0; i < d_tabButtonVector.size(); ++i)
        d_tabButtonVector [i]->setFont (getFont ());
}
/*************************************************************************
	Calculate size and position for a tab button
*************************************************************************/
void TabControl::calculateTabButtonSizePosition(size_t index)
{
    TabButton* btn = d_tabButtonVector [index];
    // relative height is always 1.0 for buttons since they are embedded in a
    // panel of the correct height already
    UVector2 position(cegui_absdim(0.0f), cegui_absdim(0.0f));
    USize size(cegui_absdim(0.0f), cegui_reldim(1.0f));
    
    // x position is based on previous button
    if (!index)
    {
        // First button
        position.d_x = cegui_absdim(d_firstTabOffset);
    }
    else
    {
        Window* prevButton = d_tabButtonVector [index - 1];

        // position is prev pos + width
        position.d_x = prevButton->getArea().d_max.d_x;
    }
   
    size.d_width = 
        cegui_absdim(btn->getRenderedString().getHorizontalExtent(btn)) +
            getTabTextPadding() + getTabTextPadding();
 
    btn->setPosition(position);
    btn->setSize(size);

    const float left_x = position.d_x.d_offset;
    btn->setVisible ((left_x < getPixelSize ().d_width) &&
                     (left_x + btn->getPixelSize ().d_width > 0));
    btn->invalidate();
}
/*************************************************************************
Layout the widgets
*************************************************************************/
void TabControl::performChildWindowLayout(bool nonclient_sized_hint,
                                          bool client_sized_hint)
{
    Window* tabButtonPane = getTabButtonPane();
    Window* tabContentPane = getTabPane();

    // Enable top/bottom edges of the tabPane control,
    // if supported by looknfeel
    if (tabContentPane->isPropertyPresent (EnableTop))
        tabContentPane->setProperty (EnableTop, (d_tabPanePos == Top) ? n0 : n1);
    if (tabContentPane->isPropertyPresent (EnableBottom))
        tabContentPane->setProperty (EnableBottom, (d_tabPanePos == Top) ? n1 : n0);
    if (tabButtonPane->isPropertyPresent (EnableTop))
        tabButtonPane->setProperty (EnableTop, (d_tabPanePos == Top) ? n0 : n1);
    if (tabButtonPane->isPropertyPresent (EnableBottom))
        tabButtonPane->setProperty (EnableBottom, (d_tabPanePos == Top) ? n1 : n0);

    Window::performChildWindowLayout(nonclient_sized_hint,
                                     client_sized_hint);

    // Calculate the size & position of the tab scroll buttons
    Window *scrollLeftBtn = 0, *scrollRightBtn = 0;
    if (isChild(ButtonScrollLeft))
        scrollLeftBtn = getChild(ButtonScrollLeft);

    if (isChild(ButtonScrollRight))
        scrollRightBtn = getChild(ButtonScrollRight);

    // Calculate the positions and sizes of the tab buttons
    if (d_firstTabOffset > 0)
        d_firstTabOffset = 0;

    for (;;)
    {
        size_t i;
        for (i = 0; i < d_tabButtonVector.size (); ++i)
            calculateTabButtonSizePosition (i);

        if (d_tabButtonVector.empty ())
        {
            if (scrollRightBtn)
                scrollRightBtn->setVisible (false);
            if (scrollLeftBtn)
                scrollLeftBtn->setVisible (false);
            break;
        }

        // Now check if tab pane wasn't scrolled too far
        --i;
        float xmax = d_tabButtonVector [i]->getXPosition ().d_offset +
            d_tabButtonVector [i]->getPixelSize ().d_width;
        float width = tabButtonPane->getPixelSize ().d_width;

        // If right button margin exceeds right window margin,
        // or leftmost button is at offset 0, finish
        if ((xmax > (width - 0.5)) || (d_firstTabOffset == 0))
        {
            if (scrollLeftBtn)
                scrollLeftBtn->setVisible (d_firstTabOffset < 0);
            if (scrollRightBtn)
                scrollRightBtn->setVisible (xmax > width);
            break;
        }

        // Scroll the tab pane until the rightmost button
        // touches the right margin
        d_firstTabOffset += width - xmax;
        // If we scrolled it too far, set leftmost button offset to 0
        if (d_firstTabOffset > 0)
            d_firstTabOffset = 0;
    }
}
/*************************************************************************
Text changed on a content window
*************************************************************************/
bool TabControl::handleContentWindowTextChanged(const EventArgs& args)
{
    // update text
    const WindowEventArgs& wargs = static_cast<const WindowEventArgs&>(args);
    Window* tabButton = getTabButtonPane()->getChild(
        makeButtonName(wargs.window));
    tabButton->setText(wargs.window->getText());
    // sort out the layout
    performChildWindowLayout();
	invalidate();

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

/*************************************************************************
    Return a pointer to the tab button pane (Window)for
    this TabControl.
*************************************************************************/
Window* TabControl::getTabButtonPane() const
{
    return getChild(TabButtonPaneName);
}

/*************************************************************************
    Return a pointer to the TabPane component widget for
    this TabControl.
*************************************************************************/
Window* TabControl::getTabPane() const
{
    return getChild(ContentPaneName);
}

int TabControl::writeChildWindowsXML(XMLSerializer& xml_stream) const
{
    // This is an easy and safe workaround for not writing out the buttonPane and contentPane. While in fact
    // we would eventually want to write these two to XML themselves, we do not want to write out their children
    // but there is no way to control this from inside these windows and currently there is also no way to do it
    // from the outside. This was determined to be the best solution, others would break ABI or are too hacky
    // Negative side-effects: any changes to AutoWindows (properties etc) will be lost in the output
    bool wasButtonPaneWritingAllowed = getTabButtonPane()->isWritingXMLAllowed();
    bool wasContentPaneWritingAllowed = getTabPane()->isWritingXMLAllowed();

    getTabButtonPane()->setWritingXMLAllowed(false);
    getTabPane()->setWritingXMLAllowed(false);

    int childOutputCount = Window::writeChildWindowsXML(xml_stream);

    getTabButtonPane()->setWritingXMLAllowed(wasButtonPaneWritingAllowed);
    getTabPane()->setWritingXMLAllowed(wasContentPaneWritingAllowed);

    // since TabControl content is actually added to the component tab
    // content pane window, this overridden function exists to dump those
    // out as if they were our own children.
    for (size_t i = 0; i < getTabCount(); ++i)
    {
        getTabContentsAtIndex(i)->writeXMLToStream(xml_stream);
        ++childOutputCount;
    }

    return childOutputCount;
}

bool TabControl::validateWindowRenderer(const WindowRenderer* renderer) const
{
	return dynamic_cast<const TabControlWindowRenderer*>(renderer) != 0;
}

/*************************************************************************
    create and return a pointer to a TabButton widget for use as a
    clickable tab header
*************************************************************************/
TabButton* TabControl::createTabButton(const String& name) const
{
    if (d_windowRenderer != 0)
    {
        TabControlWindowRenderer* wr = (TabControlWindowRenderer*)d_windowRenderer;
        return wr->createTabButton(name);
    }
    else
    {
        //return createTabButton_impl(name);
        CEGUI_THROW(InvalidRequestException(
            "This function must be implemented by the window renderer module"));
    }
}

/*************************************************************************
	change the positioning of the tab pane.
*************************************************************************/
void TabControl::setTabPanePosition(TabPanePosition pos)
{
	d_tabPanePos = pos;
    performChildWindowLayout();
}

/*************************************************************************
    scroll the tab pane to left or right depending of the clicked button
*************************************************************************/
bool TabControl::handleScrollPane(const EventArgs& e)
{
    const WindowEventArgs& wargs = static_cast<const WindowEventArgs&>(e);

    size_t i;
    float delta = 0;
    // Find the leftmost visible button
    for (i = 0; i < d_tabButtonVector.size(); ++i)
    {
        if (d_tabButtonVector [i]->isVisible())
            break;
        delta = d_tabButtonVector [i]->getPixelSize ().d_width;
    }

    if (wargs.window->getName () == ButtonScrollLeft)
    {
        if (delta == 0.0f && i < d_tabButtonVector.size ())
            delta = d_tabButtonVector [i]->getPixelSize ().d_width;

        // scroll button pane to the right
        d_firstTabOffset += delta;
    }
    else if (i < d_tabButtonVector.size ())
        // scroll button pane to left
        d_firstTabOffset -= d_tabButtonVector [i]->getPixelSize ().d_width;

    performChildWindowLayout();
	return true;
}

bool TabControl::handleDraggedPane(const EventArgs& e)
{
    const MouseEventArgs& me = static_cast<const MouseEventArgs&>(e);

    if (me.button == MiddleButton)
    {
        // This is the middle-mouse-click event, remember initial drag position
        Window *but_pane = getTabButtonPane();
        d_btGrabPos = (me.position.d_x -
            but_pane->getOuterRectClipper().d_min.d_x) -
            d_firstTabOffset;
    }
    else if (me.button == NoButton)
    {
        // Regular mouse move event
        Window *but_pane = getTabButtonPane();
        float new_to = (me.position.d_x -
            but_pane->getOuterRectClipper().d_min.d_x) -
            d_btGrabPos;
        if ((new_to < d_firstTabOffset - 0.9) ||
            (new_to > d_firstTabOffset + 0.9))
        {
            d_firstTabOffset = new_to;
            performChildWindowLayout();
        }
    }

    return true;
}

bool TabControl::handleWheeledPane(const EventArgs& e)
{
    const MouseEventArgs& me = static_cast<const MouseEventArgs&>(e);

    Window *but_pane = getTabButtonPane();
    float delta = but_pane->getOuterRectClipper().getWidth () / 20;

    d_firstTabOffset += me.wheelChange * delta;
    performChildWindowLayout();

    return true;
}

void TabControl::removeTab_impl(Window* window)
{
    // silently abort if window to be removed is 0.
    if (!window)
        return;

    // delete connection to event we subscribed earlier
    d_eventConnections.erase(window);
    // Was this selected?
    bool reselect = window->isEffectiveVisible();
    // Tab buttons are the 2nd onward children
    getTabPane()->removeChild(window);

    // remove button too
    removeButtonForTabContent(window);

    if (reselect && (getTabCount() > 0))
        // Select another tab
        setSelectedTab(getTabPane()->getChildAtIdx(0)->getName());

    performChildWindowLayout();

    invalidate();
}

NamedElement* TabControl::getChildByNamePath_impl(const String& name_path) const
{
    // FIXME: This is horrible
    //
    if (name_path.substr(0, 7) == "__auto_")
        return Window::getChildByNamePath_impl(name_path);
    else
        return Window::getChildByNamePath_impl(ContentPaneName + '/' + name_path);
}

} // End of  CEGUI namespace section
