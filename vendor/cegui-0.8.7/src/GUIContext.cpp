/***********************************************************************
    created:    Mon Jan 12 2009
    author:     Paul D Turner
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2012 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/GUIContext.h"
#include "CEGUI/RenderTarget.h"
#include "CEGUI/RenderingWindow.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/FontManager.h"
#include "CEGUI/Window.h"
#include "CEGUI/widgets/Tooltip.h"
#include "CEGUI/SimpleTimer.h"

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4355)
#endif

namespace CEGUI
{
/*!
\brief
    Implementation structure used in tracking up & down mouse button inputs in
    order to generate click, double-click, and triple-click events.
*/
struct MouseClickTracker :
    public AllocatedObject<MouseClickTracker>
{
    MouseClickTracker() :
        d_click_count(0),
        d_click_area(0, 0, 0, 0)
    {}

    //! Timer used to track clicks for this button.
    SimpleTimer d_timer;
    //! count of clicks made so far.
    int d_click_count;
    //! area used to detect multi-clicks
    Rectf d_click_area;
    //! target window for any events generated.
    Window* d_target_window;
};

//----------------------------------------------------------------------------//
const float GUIContext::DefaultMouseButtonClickTimeout = 0.0f;
const float GUIContext::DefaultMouseButtonMultiClickTimeout = 0.3333f;
const Sizef GUIContext::DefaultMouseButtonMultiClickTolerance(12.0f, 12.0f);

const String GUIContext::EventRootWindowChanged("RootWindowChanged");
const String GUIContext::EventMouseMoveScalingFactorChanged("MouseMoveScalingFactorChanged");
const String GUIContext::EventMouseButtonClickTimeoutChanged("MouseButtonClickTimeoutChanged" );
const String GUIContext::EventMouseButtonMultiClickTimeoutChanged("MouseButtonMultiClickTimeoutChanged" );
const String GUIContext::EventMouseButtonMultiClickToleranceChanged("MouseButtonMultiClickToleranceChanged" );
const String GUIContext::EventRenderTargetChanged("RenderTargetChanged");
const String GUIContext::EventDefaultFontChanged("DefaultFontChanged");

//----------------------------------------------------------------------------//
GUIContext::GUIContext(RenderTarget& target) :
    RenderingSurface(target),
    d_rootWindow(0),
    d_isDirty(false),
    d_mouseMovementScalingFactor(1.0f),
    d_generateMouseClickEvents(true),
    d_mouseButtonClickTimeout(DefaultMouseButtonClickTimeout),
    d_mouseButtonMultiClickTimeout(DefaultMouseButtonMultiClickTimeout),
    d_mouseButtonMultiClickTolerance(DefaultMouseButtonMultiClickTolerance),
    d_defaultTooltipObject(0),
    d_weCreatedTooltipObject(false),
    d_defaultFont(0),
    d_surfaceSize(target.getArea().getSize()),
    d_modalWindow(0),
    d_captureWindow(0),
    d_mouseClickTrackers(new MouseClickTracker[MouseButtonCount]),
    d_areaChangedEventConnection(
        target.subscribeEvent(
            RenderTarget::EventAreaChanged,
            Event::Subscriber(&GUIContext::areaChangedHandler, this))),
    d_windowDestroyedEventConnection(
        WindowManager::getSingleton().subscribeEvent(
            WindowManager::EventWindowDestroyed,
            Event::Subscriber(&GUIContext::windowDestroyedHandler, this)))
{
    resetWindowContainingMouse();
}

//----------------------------------------------------------------------------//
void GUIContext::resetWindowContainingMouse()
{
    d_windowContainingMouse = 0;
    d_windowContainingMouseIsUpToDate = true;
}

//----------------------------------------------------------------------------//
GUIContext::~GUIContext()
{
    destroyDefaultTooltipWindowInstance();

    if (d_rootWindow)
        d_rootWindow->setGUIContext(0);

    delete[] d_mouseClickTrackers;
}

//----------------------------------------------------------------------------//
Window* GUIContext::getRootWindow() const
{
    return d_rootWindow;
}

//----------------------------------------------------------------------------//
void GUIContext::setRootWindow(Window* new_root)
{
    if (d_rootWindow == new_root)
        return;

    if (d_rootWindow)
        d_rootWindow->setGUIContext(0);

    WindowEventArgs args(d_rootWindow);

    d_rootWindow = new_root;

    if (d_rootWindow)
    {
        d_rootWindow->setGUIContext(this);
        d_rootWindow->syncTargetSurface();
    }

    onRootWindowChanged(args);
}

//----------------------------------------------------------------------------//
void GUIContext::updateRootWindowAreaRects() const
{
    ElementEventArgs args(0);
    d_rootWindow->onParentSized(args);
}

//----------------------------------------------------------------------------//
Window* GUIContext::getInputCaptureWindow() const
{
    return d_captureWindow;
}

//----------------------------------------------------------------------------//
void GUIContext::setInputCaptureWindow(Window* window)
{
    d_captureWindow = window;
}

//----------------------------------------------------------------------------//
void GUIContext::setDefaultTooltipObject(Tooltip* tooltip)
{
    destroyDefaultTooltipWindowInstance();

    d_defaultTooltipObject = tooltip;

    if (d_defaultTooltipObject)
        d_defaultTooltipObject->setWritingXMLAllowed(false);
}

//----------------------------------------------------------------------------//
void GUIContext::setDefaultTooltipType(const String& tooltip_type)
{
    destroyDefaultTooltipWindowInstance();

    d_defaultTooltipType = tooltip_type;
}

//----------------------------------------------------------------------------//
void GUIContext::destroyDefaultTooltipWindowInstance()
{
    if (d_defaultTooltipObject && d_weCreatedTooltipObject)
    {
        WindowManager::getSingleton().destroyWindow(d_defaultTooltipObject);
        d_defaultTooltipObject = 0;
    }

    d_weCreatedTooltipObject = false;
}

//----------------------------------------------------------------------------//
Tooltip* GUIContext::getDefaultTooltipObject() const
{
    if (!d_defaultTooltipObject && !d_defaultTooltipType.empty())
        createDefaultTooltipWindowInstance();

    return d_defaultTooltipObject;
}

//----------------------------------------------------------------------------//
void GUIContext::createDefaultTooltipWindowInstance() const
{
    WindowManager& winmgr(WindowManager::getSingleton());

    if (winmgr.isLocked())
        return;

    d_defaultTooltipObject = dynamic_cast<Tooltip*>(
        winmgr.createWindow(d_defaultTooltipType,
                            "CEGUI::System::default__auto_tooltip__"));
    d_defaultTooltipObject->setAutoWindow(true);

    if (d_defaultTooltipObject)
    {
        d_defaultTooltipObject->setWritingXMLAllowed(false);
        d_weCreatedTooltipObject = true;
    }
}

//----------------------------------------------------------------------------//
void GUIContext::setModalWindow(Window* window)
{
    d_modalWindow = window;
}

//----------------------------------------------------------------------------//
Window* GUIContext::getModalWindow() const
{
    return d_modalWindow;
}

//----------------------------------------------------------------------------//
Window* GUIContext::getWindowContainingMouse() const
{
    if (!d_windowContainingMouseIsUpToDate)
    {
        updateWindowContainingMouse_impl();
        d_windowContainingMouseIsUpToDate = true;
    }

    return d_windowContainingMouse;
}

//----------------------------------------------------------------------------//
const Sizef& GUIContext::getSurfaceSize() const
{
    return d_surfaceSize;
}

//----------------------------------------------------------------------------//
const SystemKeys& GUIContext::getSystemKeys() const
{
    return d_systemKeys;
}

//----------------------------------------------------------------------------//
void GUIContext::markAsDirty()
{
    d_isDirty = true;
}

//----------------------------------------------------------------------------//
bool GUIContext::isDirty() const
{
    return d_isDirty;
}

//----------------------------------------------------------------------------//
void GUIContext::draw()
{
    if (d_isDirty)
        drawWindowContentToTarget();

    RenderingSurface::draw();
}

//----------------------------------------------------------------------------//
void GUIContext::drawContent()
{
    RenderingSurface::drawContent();

    d_mouseCursor.draw();
}

//----------------------------------------------------------------------------//
void GUIContext::drawWindowContentToTarget()
{
    if (d_rootWindow)
        renderWindowHierarchyToSurfaces();
    else
        clearGeometry();

    d_isDirty = false;
}

//----------------------------------------------------------------------------//
void GUIContext::renderWindowHierarchyToSurfaces()
{
    RenderingSurface& rs = d_rootWindow->getTargetRenderingSurface();
    rs.clearGeometry();

    if (rs.isRenderingWindow())
        static_cast<RenderingWindow&>(rs).getOwner().clearGeometry();

    d_rootWindow->render();
}

//----------------------------------------------------------------------------//
MouseCursor& GUIContext::getMouseCursor()
{
    return const_cast<MouseCursor&>(
        static_cast<const GUIContext*>(this)->getMouseCursor());
}

//----------------------------------------------------------------------------//
const MouseCursor& GUIContext::getMouseCursor() const
{
    return d_mouseCursor;
}
    
//----------------------------------------------------------------------------//
void GUIContext::setMouseMoveScalingFactor(float factor)
{
    d_mouseMovementScalingFactor = factor;

    GUIContextEventArgs args(this);
    onMouseMoveScalingFactorChanged(args);
}

//----------------------------------------------------------------------------//
float GUIContext::getMouseMoveScalingFactor() const
{
    return d_mouseMovementScalingFactor;
}

//----------------------------------------------------------------------------//
void GUIContext::setMouseButtonClickTimeout(float seconds)
{
    d_mouseButtonClickTimeout = seconds;

    GUIContextEventArgs args(this);
    onMouseButtonClickTimeoutChanged(args);
}

//----------------------------------------------------------------------------//
float GUIContext::getMouseButtonClickTimeout() const
{
    return d_mouseButtonClickTimeout;
}

//----------------------------------------------------------------------------//
void GUIContext::setMouseButtonMultiClickTimeout(float seconds)
{
    d_mouseButtonMultiClickTimeout = seconds;

    GUIContextEventArgs args(this);
    onMouseButtonMultiClickTimeoutChanged(args);
}

//----------------------------------------------------------------------------//
float GUIContext::getMouseButtonMultiClickTimeout() const
{
    return d_mouseButtonMultiClickTimeout;
}

//----------------------------------------------------------------------------//
void GUIContext::setMouseButtonMultiClickTolerance(const Sizef& sz)
{
    d_mouseButtonMultiClickTolerance = sz;

    GUIContextEventArgs args(this);
    onMouseButtonMultiClickToleranceChanged(args);
}

//----------------------------------------------------------------------------//
const Sizef& GUIContext::getMouseButtonMultiClickTolerance() const
{
    return d_mouseButtonMultiClickTolerance;
}

//----------------------------------------------------------------------------//
void GUIContext::setMouseClickEventGenerationEnabled(const bool enable)
{
    d_generateMouseClickEvents = enable;
}

//----------------------------------------------------------------------------//
bool GUIContext::isMouseClickEventGenerationEnabled() const
{
    return d_generateMouseClickEvents;
}

//----------------------------------------------------------------------------//
bool GUIContext::areaChangedHandler(const EventArgs&)
{
    d_surfaceSize = d_target->getArea().getSize();
    d_mouseCursor.notifyDisplaySizeChanged(d_surfaceSize);

    if (d_rootWindow)
        updateRootWindowAreaRects();

    return true;
}

//----------------------------------------------------------------------------//
bool GUIContext::windowDestroyedHandler(const EventArgs& args)
{
    const Window* const window =
        static_cast<const WindowEventArgs&>(args).window;

    if (window == d_rootWindow)
        d_rootWindow = 0;

    if (window == getWindowContainingMouse())
        resetWindowContainingMouse();

    if (window == d_modalWindow)
        d_modalWindow = 0;

    if (window == d_captureWindow)
        d_captureWindow = 0;

    if (window == d_defaultTooltipObject)
    {
        d_defaultTooltipObject = 0;
        d_weCreatedTooltipObject = false;
    }

    return true;
}

//----------------------------------------------------------------------------//
void GUIContext::onRootWindowChanged(WindowEventArgs& args)
{
    if (d_rootWindow)
        updateRootWindowAreaRects();

    markAsDirty();

    fireEvent(EventRootWindowChanged, args);
}

//----------------------------------------------------------------------------//
void GUIContext::onMouseMoveScalingFactorChanged(GUIContextEventArgs& args)
{
    fireEvent(EventMouseMoveScalingFactorChanged, args);
}

//----------------------------------------------------------------------------//
void GUIContext::onMouseButtonClickTimeoutChanged(GUIContextEventArgs& args)
{
    fireEvent(EventMouseButtonClickTimeoutChanged, args);
}

//----------------------------------------------------------------------------//
void GUIContext::onMouseButtonMultiClickTimeoutChanged(GUIContextEventArgs& args)
{
    fireEvent(EventMouseButtonMultiClickTimeoutChanged, args);
}

//----------------------------------------------------------------------------//
void GUIContext::onMouseButtonMultiClickToleranceChanged(GUIContextEventArgs& args)
{
    fireEvent(EventMouseButtonMultiClickToleranceChanged, args);
}

//----------------------------------------------------------------------------//
bool GUIContext::injectMouseMove(float delta_x, float delta_y)
{
    MouseEventArgs ma(0);
    ma.moveDelta.d_x = delta_x * d_mouseMovementScalingFactor;
    ma.moveDelta.d_y = delta_y * d_mouseMovementScalingFactor;

    // no movement means no event
    if ((ma.moveDelta.d_x == 0) && (ma.moveDelta.d_y == 0))
        return false;

    d_mouseCursor.offsetPosition(ma.moveDelta);

    ma.position = d_mouseCursor.getPosition();
    ma.sysKeys = d_systemKeys.get();
    ma.wheelChange = 0;
    ma.clickCount = 0;
    ma.button = NoButton;

    return mouseMoveInjection_impl(ma);
}

//----------------------------------------------------------------------------//
bool GUIContext::mouseMoveInjection_impl(MouseEventArgs& ma)
{
    updateWindowContainingMouse();

    // input can't be handled if there is no window to handle it.
    if (!getWindowContainingMouse())
        return false;

    // make mouse position sane for this target window
    ma.position = getWindowContainingMouse()->getUnprojectedPosition(ma.position);
    // inform window about the input.
    ma.window = getWindowContainingMouse();
    ma.handled = 0;
    ma.window->onMouseMove(ma);

    // return whether window handled the input.
    return ma.handled != 0;
}

//----------------------------------------------------------------------------//
void GUIContext::updateWindowContainingMouse()
{
    d_windowContainingMouseIsUpToDate = false;
}

//----------------------------------------------------------------------------//
bool GUIContext::updateWindowContainingMouse_impl() const
{
    MouseEventArgs ma(0);
    const Vector2f mouse_pos(d_mouseCursor.getPosition());

    Window* const curr_wnd_with_mouse = getTargetWindow(mouse_pos, true);

    // exit if window containing mouse has not changed.
    if (curr_wnd_with_mouse == d_windowContainingMouse)
        return false;

    ma.sysKeys = d_systemKeys.get();
    ma.wheelChange = 0;
    ma.clickCount = 0;
    ma.button = NoButton;

    Window* oldWindow = d_windowContainingMouse;
    d_windowContainingMouse = curr_wnd_with_mouse;

    // inform previous window the mouse has left it
    if (oldWindow)
    {
        ma.window = oldWindow;
        ma.position = oldWindow->getUnprojectedPosition(mouse_pos);
        oldWindow->onMouseLeaves(ma);
    }

    // inform window containing mouse that mouse has entered it
    if (d_windowContainingMouse)
    {
        ma.handled = 0;
        ma.window = d_windowContainingMouse;
        ma.position = d_windowContainingMouse->getUnprojectedPosition(mouse_pos);
        d_windowContainingMouse->onMouseEnters(ma);
    }

    // do the 'area' version of the events
    Window* root = getCommonAncestor(oldWindow, d_windowContainingMouse);

    if (oldWindow)
        notifyMouseTransition(root, oldWindow, &Window::onMouseLeavesArea, ma);

    if (d_windowContainingMouse)
        notifyMouseTransition(root, d_windowContainingMouse, &Window::onMouseEntersArea, ma);

    return true;
}

//----------------------------------------------------------------------------//
Window* GUIContext::getCommonAncestor(Window* w1, Window* w2) const 
{
    if (!w2)
        return w2;

    if (w1 == w2)
        return w1;

    // make sure w1 is always further up
    if (w1 && w1->isAncestor(w2))
        return w2;

    while (w1)
    {
        if (w2->isAncestor(w1))
            break;

        w1 = w1->getParent();
    }

    return w1;
}

//----------------------------------------------------------------------------//
void GUIContext::notifyMouseTransition(Window* top, Window* bottom,
                                    void (Window::*func)(MouseEventArgs&),
                                    MouseEventArgs& args) const
{
    if (top == bottom)
        return;
    
    Window* const parent = bottom->getParent();

    if (parent && parent != top)
        notifyMouseTransition(top, parent, func, args);

    args.handled = 0;
    args.window = bottom;

    (bottom->*func)(args);
}

//----------------------------------------------------------------------------//
Window* GUIContext::getTargetWindow(const Vector2f& pt,
                                 const bool allow_disabled) const
{
    // if there is no GUI sheet visible, then there is nowhere to send input
    if (!d_rootWindow || !d_rootWindow->isEffectiveVisible())
        return 0;

    Window* dest_window = d_captureWindow;

    if (!dest_window)
    {
        dest_window = d_rootWindow->
            getTargetChildAtPosition(pt, allow_disabled);

        if (!dest_window)
            dest_window = d_rootWindow;
    }
    else
    {
        if (dest_window->distributesCapturedInputs())
        {
            Window* child_window = dest_window->
                getTargetChildAtPosition(pt, allow_disabled);

            if (child_window)
                dest_window = child_window;
        }
    }

    // modal target overrules
    if (d_modalWindow && dest_window != d_modalWindow)
        if (!dest_window->isAncestor(d_modalWindow))
            dest_window = d_modalWindow;

    return dest_window;
}

//----------------------------------------------------------------------------//
Window* GUIContext::getKeyboardTargetWindow() const
{
    // if no active sheet, there is no target widow.
    if (!d_rootWindow || !d_rootWindow->isEffectiveVisible())
        return 0;

    // handle normal non-modal situations
    if (!d_modalWindow)
        return d_rootWindow->getActiveChild();

    // handle possible modal window.
    Window* const target = d_modalWindow->getActiveChild();
    return target ? target : d_modalWindow;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectMouseLeaves(void)
{
    if (!getWindowContainingMouse())
        return false;

    MouseEventArgs ma(0);
    ma.position = getWindowContainingMouse()->getUnprojectedPosition(
        d_mouseCursor.getPosition());
    ma.moveDelta = Vector2f(0.0f, 0.0f);
    ma.button = NoButton;
    ma.sysKeys = d_systemKeys.get();
    ma.wheelChange = 0;
    ma.window = getWindowContainingMouse();
    ma.clickCount = 0;

    getWindowContainingMouse()->onMouseLeaves(ma);
    resetWindowContainingMouse();

    return ma.handled != 0;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectMouseButtonDown(MouseButton button)
{
    d_systemKeys.mouseButtonPressed(button);

    MouseEventArgs ma(0);
    ma.position = d_mouseCursor.getPosition();
    ma.moveDelta = Vector2f(0.0f, 0.0f);
    ma.button = button;
    ma.sysKeys = d_systemKeys.get();
    ma.wheelChange = 0;
    ma.window = getTargetWindow(ma.position, false);
    // make mouse position sane for this target window
    if (ma.window)
        ma.position = ma.window->getUnprojectedPosition(ma.position);

    //
    // Handling for multi-click generation
    //
    MouseClickTracker& tkr = d_mouseClickTrackers[button];

    tkr.d_click_count++;

    // if multi-click requirements are not met
    if (((d_mouseButtonMultiClickTimeout > 0) && (tkr.d_timer.elapsed() > d_mouseButtonMultiClickTimeout)) ||
        (!tkr.d_click_area.isPointInRect(ma.position)) ||
        (tkr.d_target_window != ma.window) ||
        (tkr.d_click_count > 3))
    {
        // reset to single down event.
        tkr.d_click_count = 1;

        // build new allowable area for multi-clicks
        tkr.d_click_area.setPosition(ma.position);
        tkr.d_click_area.setSize(d_mouseButtonMultiClickTolerance);
        tkr.d_click_area.offset(Vector2f(-(d_mouseButtonMultiClickTolerance.d_width / 2),
                                         -(d_mouseButtonMultiClickTolerance.d_height / 2)));

        // set target window for click events on this tracker
        tkr.d_target_window = ma.window;
    }

    // set click count in the event args
    ma.clickCount = tkr.d_click_count;

    if (ma.window)
    {
        if (d_generateMouseClickEvents && ma.window->wantsMultiClickEvents())
        {
            switch (tkr.d_click_count)
            {
            case 1:
                ma.window->onMouseButtonDown(ma);
                break;

            case 2:
                ma.window->onMouseDoubleClicked(ma);
                break;

            case 3:
                ma.window->onMouseTripleClicked(ma);
                break;
            }
        }
        // click generation disabled, or current target window does not want
        // multi-clicks, so just send a mouse down event instead.
        else
        {
            ma.window->onMouseButtonDown(ma);
        }
    }

    // reset timer for this tracker.
    tkr.d_timer.restart();

    return ma.handled != 0;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectMouseButtonUp(MouseButton button)
{
    d_systemKeys.mouseButtonReleased(button);

    MouseEventArgs ma(0);
    ma.position = d_mouseCursor.getPosition();
    ma.moveDelta = Vector2f(0.0f, 0.0f);
    ma.button = button;
    ma.sysKeys = d_systemKeys.get();
    ma.wheelChange = 0;
    ma.window = getTargetWindow(ma.position, false);
    // make mouse position sane for this target window
    if (ma.window)
        ma.position = ma.window->getUnprojectedPosition(ma.position);

    // get the tracker that holds the number of down events seen so far for this button
    MouseClickTracker& tkr = d_mouseClickTrackers[button];
    // set click count in the event args
    ma.clickCount = tkr.d_click_count;

    // if there is no window, inputs can not be handled.
    if (!ma.window)
        return false;

    // store original window becase we re-use the event args.
    Window* const tgt_wnd = ma.window;

    // send 'up' input to the window
    ma.window->onMouseButtonUp(ma);
    // store whether the 'up' part was handled so we may reuse the EventArgs
    const uint upHandled = ma.handled;

    // restore target window (because Window::on* may have propagated input)
    ma.window = tgt_wnd;

    // send MouseClicked event if the requirements for that were met
    if (d_generateMouseClickEvents &&
        ((d_mouseButtonClickTimeout == 0) || (tkr.d_timer.elapsed() <= d_mouseButtonClickTimeout)) &&
        (tkr.d_click_area.isPointInRect(ma.position)) &&
        (tkr.d_target_window == ma.window))
    {
        ma.handled = 0;
        ma.window->onMouseClicked(ma);
    }

    return (ma.handled + upHandled) != 0;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectKeyDown(Key::Scan scan_code)
{
    d_systemKeys.keyPressed(scan_code);

    KeyEventArgs args(getKeyboardTargetWindow());

    // if there's no destination window, input can't be handled.
    if (!args.window)
        return false;

    args.scancode = scan_code;
    args.sysKeys = d_systemKeys.get();

    args.window->onKeyDown(args);
    return args.handled != 0;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectKeyUp(Key::Scan scan_code)
{
    d_systemKeys.keyReleased(scan_code);

    KeyEventArgs args(getKeyboardTargetWindow());

    // if there's no destination window, input can't be handled.
    if (!args.window)
        return false;

    args.scancode = scan_code;
    args.sysKeys = d_systemKeys.get();

    args.window->onKeyUp(args);
    return args.handled != 0;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectChar(String::value_type code_point)
{
    KeyEventArgs args(getKeyboardTargetWindow());

    // if there's no destination window, input can't be handled.
    if (!args.window)
        return false;

    args.codepoint = code_point;
    args.sysKeys = d_systemKeys.get();

    args.window->onCharacter(args);
    return args.handled != 0;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectMouseWheelChange(float delta)
{
    MouseEventArgs ma(0);
    ma.position = d_mouseCursor.getPosition();
    ma.moveDelta = Vector2f(0.0f, 0.0f);
    ma.button = NoButton;
    ma.sysKeys = d_systemKeys.get();
    ma.wheelChange = delta;
    ma.clickCount = 0;
    ma.window = getTargetWindow(ma.position, false);
    // make mouse position sane for this target window
    if (ma.window)
        ma.position = ma.window->getUnprojectedPosition(ma.position);

    // if there is no target window, input can not be handled.
    if (!ma.window)
        return false;

    ma.window->onMouseWheel(ma);
    return ma.handled != 0;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectMousePosition(float x_pos, float y_pos)
{
    const Vector2f new_position(x_pos, y_pos);

    // setup mouse movement event args object.
    MouseEventArgs ma(0);
    ma.moveDelta = new_position - d_mouseCursor.getPosition();

    // no movement means no event
    if ((ma.moveDelta.d_x == 0) && (ma.moveDelta.d_y == 0))
        return false;

    ma.sysKeys = d_systemKeys.get();
    ma.wheelChange = 0;
    ma.clickCount = 0;
    ma.button = NoButton;

    // move mouse cursor to new position
    d_mouseCursor.setPosition(new_position);
    // update position in args (since actual position may be constrained)
    ma.position = d_mouseCursor.getPosition();

    return mouseMoveInjection_impl(ma);
}

//----------------------------------------------------------------------------//
bool GUIContext::injectTimePulse(float timeElapsed)
{
    // if no visible active sheet, input can't be handled
    if (!d_rootWindow || !d_rootWindow->isEffectiveVisible())
        return false;

    // ensure window containing mouse is now valid
    getWindowContainingMouse();

    // else pass to sheet for distribution.
    d_rootWindow->update(timeElapsed);
    // this input is then /always/ considered handled.
    return true;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectMouseButtonClick(const MouseButton button)
{
    MouseEventArgs ma(0);
    ma.position = d_mouseCursor.getPosition();
    ma.window = getTargetWindow(ma.position, false);

    if (ma.window)
    {
        // initialise remainder of args struct.
        ma.moveDelta = Vector2f(0.0f, 0.0f);
        ma.button = button;
        ma.sysKeys = d_systemKeys.get();
        ma.wheelChange = 0;
        // make mouse position sane for this target window
        ma.position = ma.window->getUnprojectedPosition(ma.position);
        // tell the window about the event.
        ma.window->onMouseClicked(ma);
    }

    return ma.handled != 0;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectMouseButtonDoubleClick(const MouseButton button)
{
    MouseEventArgs ma(0);
    ma.position = d_mouseCursor.getPosition();
    ma.window = getTargetWindow(ma.position, false);

    if (ma.window && ma.window->wantsMultiClickEvents())
    {
        // initialise remainder of args struct.
        ma.moveDelta = Vector2f(0.0f, 0.0f);
        ma.button = button;
        ma.sysKeys = d_systemKeys.get();
        ma.wheelChange = 0;
        // make mouse position sane for this target window
        ma.position = ma.window->getUnprojectedPosition(ma.position);
        // tell the window about the event.
        ma.window->onMouseDoubleClicked(ma);
    }

    return ma.handled != 0;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectMouseButtonTripleClick(const MouseButton button)
{
    MouseEventArgs ma(0);
    ma.position = d_mouseCursor.getPosition();
    ma.window = getTargetWindow(ma.position, false);

    if (ma.window && ma.window->wantsMultiClickEvents())
    {
        // initialise remainder of args struct.
        ma.moveDelta = Vector2f(0.0f, 0.0f);
        ma.button = button;
        ma.sysKeys = d_systemKeys.get();
        ma.wheelChange = 0;
        // make mouse position sane for this target window
        ma.position = ma.window->getUnprojectedPosition(ma.position);
        // tell the window about the event.
        ma.window->onMouseTripleClicked(ma);
    }

    return ma.handled != 0;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectCopyRequest()
{
    Window* source = getKeyboardTargetWindow();
    return source ? source->performCopy(*System::getSingleton().getClipboard()) : false;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectCutRequest()
{
    Window* source = getKeyboardTargetWindow();
    return source ? source->performCut(*System::getSingleton().getClipboard()) : false;
}

//----------------------------------------------------------------------------//
bool GUIContext::injectPasteRequest()
{
    Window* target = getKeyboardTargetWindow();
    return target ? target->performPaste(*System::getSingleton().getClipboard()) : false;
}

//----------------------------------------------------------------------------//
void GUIContext::setRenderTarget(RenderTarget& target)
{
    if (d_target == &target)
        return;

    RenderTarget* const old_target = d_target;
    d_target = &target;

    if (d_rootWindow)
        d_rootWindow->syncTargetSurface();

    d_areaChangedEventConnection.disconnect();
    d_areaChangedEventConnection = d_target->subscribeEvent(
            RenderTarget::EventAreaChanged,
            Event::Subscriber(&GUIContext::areaChangedHandler, this));

    EventArgs area_args;
    areaChangedHandler(area_args);

    GUIContextRenderTargetEventArgs change_args(this, old_target);
    onRenderTargetChanged(change_args);
}

//----------------------------------------------------------------------------//
void GUIContext::onRenderTargetChanged(GUIContextRenderTargetEventArgs& args)
{
    fireEvent(EventRenderTargetChanged, args, EventNamespace);
}

//----------------------------------------------------------------------------//
void GUIContext::setDefaultFont(const String& name)
{
    if (name.empty())
        setDefaultFont(0);
    else
        setDefaultFont(&FontManager::getSingleton().get(name));
}

//----------------------------------------------------------------------------//
void GUIContext::setDefaultFont(Font* font)
{
    d_defaultFont = font;

    EventArgs args;
    onDefaultFontChanged(args);
}

//----------------------------------------------------------------------------//
Font* GUIContext::getDefaultFont() const
{
    if (d_defaultFont)
        return d_defaultFont;

    // if no explicit default, we will return the first font we can get from
    // the font manager
    FontManager::FontIterator iter = FontManager::getSingleton().getIterator();

    return (!iter.isAtEnd()) ? *iter : 0;
}

//----------------------------------------------------------------------------//
void GUIContext::onDefaultFontChanged(EventArgs& args)
{
    if (d_rootWindow)
        notifyDefaultFontChanged(d_rootWindow);

    fireEvent(EventDefaultFontChanged, args, EventNamespace);
}

//----------------------------------------------------------------------------//
void GUIContext::notifyDefaultFontChanged(Window* hierarchy_root) const
{
    WindowEventArgs evt_args(hierarchy_root);

    if (!hierarchy_root->getFont(false))
        hierarchy_root->onFontChanged(evt_args);

    for (size_t i = 0; i < hierarchy_root->getChildCount(); ++i)
        notifyDefaultFontChanged(hierarchy_root->getChildAtIdx(i));
}

//----------------------------------------------------------------------------//

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

}

