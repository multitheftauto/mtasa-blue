/***********************************************************************
    created:    21/2/2004
    author:     Paul D Turner

    purpose:    Implements the Window base class
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
#include "CEGUI/Window.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/System.h"
#include "CEGUI/FontManager.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/MouseCursor.h"
#include "CEGUI/CoordConverter.h"
#include "CEGUI/WindowRendererManager.h"
#include "CEGUI/WindowFactoryManager.h"
#include "CEGUI/widgets/Tooltip.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/falagard/WidgetComponent.h"
#include "CEGUI/GeometryBuffer.h"
#include "CEGUI/GUIContext.h"
#include "CEGUI/RenderingContext.h"
#include "CEGUI/RenderingWindow.h"
#include "CEGUI/GlobalEventSet.h"
#include <algorithm>
#include <iterator>
#include <cmath>
#include <stdio.h>
#include <queue>

#if defined (CEGUI_USE_FRIBIDI)
    #include "CEGUI/FribidiVisualMapping.h"
#elif defined (CEGUI_USE_MINIBIDI)
    #include "CEGUI/MinibidiVisualMapping.h"
#else
    #include "CEGUI/BidiVisualMapping.h"
#endif

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4355)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
const String Window::AlphaPropertyName("Alpha");
const String Window::AlwaysOnTopPropertyName("AlwaysOnTop");
const String Window::ClippedByParentPropertyName("ClippedByParent");
const String Window::DestroyedByParentPropertyName("DestroyedByParent");
const String Window::DisabledPropertyName("Disabled");
const String Window::FontPropertyName("Font");
const String Window::IDPropertyName("ID");
const String Window::InheritsAlphaPropertyName("InheritsAlpha");
const String Window::MouseCursorImagePropertyName("MouseCursorImage");
const String Window::VisiblePropertyName("Visible");
const String Window::RestoreOldCapturePropertyName("RestoreOldCapture");
const String Window::TextPropertyName("Text");
const String Window::ZOrderingEnabledPropertyName("ZOrderingEnabled");
const String Window::WantsMultiClickEventsPropertyName("WantsMultiClickEvents");
const String Window::MouseAutoRepeatEnabledPropertyName("MouseAutoRepeatEnabled");
const String Window::AutoRepeatDelayPropertyName("AutoRepeatDelay");
const String Window::AutoRepeatRatePropertyName("AutoRepeatRate");
const String Window::DistributeCapturedInputsPropertyName("DistributeCapturedInputs");
const String Window::TooltipTypePropertyName("TooltipType");
const String Window::TooltipTextPropertyName("TooltipText");
const String Window::InheritsTooltipTextPropertyName("InheritsTooltipText");
const String Window::RiseOnClickEnabledPropertyName("RiseOnClickEnabled");
const String Window::MousePassThroughEnabledPropertyName("MousePassThroughEnabled");
const String Window::DragDropTargetPropertyName("DragDropTarget");
const String Window::AutoRenderingSurfacePropertyName("AutoRenderingSurface");
const String Window::TextParsingEnabledPropertyName("TextParsingEnabled");
const String Window::MarginPropertyName("MarginProperty");
const String Window::UpdateModePropertyName("UpdateMode");
const String Window::MouseInputPropagationEnabledPropertyName("MouseInputPropagationEnabled");
const String Window::AutoWindowPropertyName("AutoWindow");
//----------------------------------------------------------------------------//
const String Window::EventNamespace("Window");
const String Window::EventUpdated ("Updated");
const String Window::EventTextChanged("TextChanged");
const String Window::EventFontChanged("FontChanged");
const String Window::EventAlphaChanged("AlphaChanged");
const String Window::EventIDChanged("IDChanged");
const String Window::EventActivated("Activated");
const String Window::EventDeactivated("Deactivated");
const String Window::EventShown("Shown");
const String Window::EventHidden("Hidden");
const String Window::EventEnabled("Enabled");
const String Window::EventDisabled("Disabled");
const String Window::EventClippedByParentChanged( "ClippedByParentChanged" );
const String Window::EventDestroyedByParentChanged("DestroyedByParentChanged");
const String Window::EventInheritsAlphaChanged( "InheritsAlphaChanged" );
const String Window::EventAlwaysOnTopChanged("AlwaysOnTopChanged");
const String Window::EventInputCaptureGained( "InputCaptureGained" );
const String Window::EventInputCaptureLost( "InputCaptureLost" );
const String Window::EventInvalidated( "Invalidated" );
const String Window::EventRenderingStarted( "RenderingStarted" );
const String Window::EventRenderingEnded( "RenderingEnded" );
const String Window::EventDestructionStarted( "DestructionStarted" );
const String Window::EventDragDropItemEnters("DragDropItemEnters");
const String Window::EventDragDropItemLeaves("DragDropItemLeaves");
const String Window::EventDragDropItemDropped("DragDropItemDropped");
const String Window::EventWindowRendererAttached("WindowRendererAttached");
const String Window::EventWindowRendererDetached("WindowRendererDetached");
const String Window::EventTextParsingChanged("TextParsingChanged");
const String Window::EventMarginChanged("MarginChanged");
const String Window::EventMouseEntersArea("MouseEntersArea");
const String Window::EventMouseLeavesArea("MouseLeavesArea");
const String Window::EventMouseEntersSurface( "MouseEntersSurface" );
const String Window::EventMouseLeavesSurface( "MouseLeavesSurface" );
const String Window::EventMouseMove("MouseMove");
const String Window::EventMouseWheel("MouseWheel");
const String Window::EventMouseButtonDown("MouseButtonDown");
const String Window::EventMouseButtonUp("MouseButtonUp");
const String Window::EventMouseClick("MouseClick");
const String Window::EventMouseDoubleClick("MouseDoubleClick");
const String Window::EventMouseTripleClick("MouseTripleClick");
const String Window::EventKeyDown("KeyDown");
const String Window::EventKeyUp("KeyUp");
const String Window::EventCharacterKey("CharacterKey");

//----------------------------------------------------------------------------//
// XML element and attribute names that relate to Window.
const String Window::WindowXMLElementName("Window");
const String Window::AutoWindowXMLElementName("AutoWindow");
const String Window::UserStringXMLElementName("UserString");
const String Window::WindowTypeXMLAttributeName("type");
const String Window::WindowNameXMLAttributeName("name");
const String Window::AutoWindowNamePathXMLAttributeName("namePath");
const String Window::UserStringNameXMLAttributeName("name");
const String Window::UserStringValueXMLAttributeName("value");

//----------------------------------------------------------------------------//
const String Window::TooltipNameSuffix("__auto_tooltip__");

//----------------------------------------------------------------------------//
BasicRenderedStringParser Window::d_basicStringParser;
DefaultRenderedStringParser Window::d_defaultStringParser;

//----------------------------------------------------------------------------//
Window::WindowRendererProperty Window::d_windowRendererProperty;
Window::LookNFeelProperty Window::d_lookNFeelProperty;

//----------------------------------------------------------------------------//
Window::WindowRendererProperty::WindowRendererProperty() : TplWindowProperty<Window,String>(
    "WindowRenderer",
    "Property to get/set the assigned WindowRenderer object type. For "
    "advanced use only. See the api reference for Window::setWindowRenderer "
    "for more information. "
    "Value is a string.",
    "Window", &Window::setWindowRenderer,&Window::getWindowRendererName, "")
    {}

//----------------------------------------------------------------------------//
void Window::WindowRendererProperty::writeXMLToStream(const PropertyReceiver* receiver,
                                                      XMLSerializer& xml_stream) const
{
    const Window* wnd = static_cast<const Window*>(receiver);

    if (!WindowFactoryManager::getSingleton().isFalagardMappedType(wnd->getType()))
        Property::writeXMLToStream(receiver, xml_stream);
}

//----------------------------------------------------------------------------//
Window::LookNFeelProperty::LookNFeelProperty() : TplWindowProperty<Window,String>(
    "LookNFeel",
    "Property to get/set the assigned look'n'feel. For advanced use only. See "
    "the api reference for Window::setLookNFeel for more informaton. "
    "Value is a string.",
    "Window", &Window::setLookNFeel,&Window::getLookNFeel, "")
    {}

//----------------------------------------------------------------------------//
void Window::LookNFeelProperty::writeXMLToStream(const PropertyReceiver* receiver,
                                                 XMLSerializer& xml_stream) const
{
    const Window* wnd = static_cast<const Window*>(receiver);

    if (!WindowFactoryManager::getSingleton().isFalagardMappedType(wnd->getType()))
        Property::writeXMLToStream(receiver, xml_stream);
}

//----------------------------------------------------------------------------//
Window::Window(const String& type, const String& name):
    NamedElement(name),

    // basic types and initial window name
    d_type(type),
    d_autoWindow(false),

    // basic state
    d_initialising(false),
    d_destructionStarted(false),
    d_enabled(true),
    d_visible(true),
    d_active(false),

    // parent related fields
    d_destroyedByParent(true),

    // clipping options
    d_clippedByParent(true),

    // rendering components and options
    d_windowRenderer(0),
    d_geometry(&System::getSingleton().getRenderer()->createGeometryBuffer()),
    d_surface(0),
    d_needsRedraw(true),
    d_autoRenderingWindow(false),
    d_mouseCursor(0),

    // alpha transparency set up
    d_alpha(1.0f),
    d_inheritsAlpha(true),

    // mouse input capture set up
    d_oldCapture(0),
    d_restoreOldCapture(false),
    d_distCapturedInputs(false),

    // text system set up
    d_font(0),
#ifndef CEGUI_BIDI_SUPPORT
    d_bidiVisualMapping(0),
#elif defined (CEGUI_USE_FRIBIDI)
    d_bidiVisualMapping(CEGUI_NEW_AO FribidiVisualMapping),
#elif defined (CEGUI_USE_MINIBIDI)
    d_bidiVisualMapping(CEGUI_NEW_AO MinibidiVisualMapping),
#else
    #error "BIDI Configuration is inconsistant, check your config!"
#endif
    d_bidiDataValid(false),
    d_renderedStringValid(false),
    d_customStringParser(0),
    d_textParsingEnabled(true),

    // margin
    d_margin(UBox(UDim(0, 0))),

    // user specific data
    d_ID(0),
    d_userData(0),

    // z-order related options
    d_alwaysOnTop(false),
    d_riseOnClick(true),
    d_zOrderingEnabled(true),

    // mouse input options
    d_wantsMultiClicks(true),
    d_mousePassThroughEnabled(false),
    d_autoRepeat(false),
    d_repeatDelay(0.3f),
    d_repeatRate(0.06f),
    d_repeatButton(NoButton),
    d_repeating(false),
    d_repeatElapsed(0.0f),

    // drag and drop
    d_dragDropTarget(true),

    // tool tip related
    d_customTip(0),
    d_weOwnTip(false),
    d_inheritsTipText(true),

    // XML writing options
    d_allowWriteXML(true),

    // initialise area cache rects
    d_outerRectClipper(0, 0, 0, 0),
    d_innerRectClipper(0, 0, 0, 0),
    d_hitTestRect(0, 0, 0, 0),

    // cached pixel rect validity flags
    d_outerRectClipperValid(false),
    d_innerRectClipperValid(false),
    d_hitTestRectValid(false),

    // Initial update mode
    d_updateMode(WUM_VISIBLE),

    // Don't propagate mouse inputs by default.
    d_propagateMouseInputs(false),

    d_guiContext(0),

    d_containsMouse(false),

    d_fontRenderSizeChangeConnection(
        GlobalEventSet::getSingleton().subscribeEvent(
            "Font/RenderSizeChanged",
            Event::Subscriber(&Window::handleFontRenderSizeChange, this)))
{
    // add properties
    addWindowProperties();
}

//----------------------------------------------------------------------------//
Window::~Window(void)
{
    // most cleanup actually happened earlier in Window::destroy.

    System::getSingleton().getRenderer()->destroyGeometryBuffer(*d_geometry);
    CEGUI_DELETE_AO d_bidiVisualMapping;
}

//----------------------------------------------------------------------------//
const String& Window::getType(void) const
{
    return d_falagardType.empty() ? d_type : d_falagardType;
}

//----------------------------------------------------------------------------//
bool Window::isDisabled() const
{
    return !d_enabled;
}

//----------------------------------------------------------------------------//
bool Window::isEffectiveDisabled() const
{
    const bool parent_disabled = !d_parent ? false : getParent()->isEffectiveDisabled();

    return !d_enabled || parent_disabled;
}

//----------------------------------------------------------------------------//
bool Window::isVisible() const
{
    return d_visible;
}

//----------------------------------------------------------------------------//
bool Window::isEffectiveVisible() const
{
    const bool parent_visible = !d_parent ? true : getParent()->isEffectiveVisible();

    return d_visible && parent_visible;
}

//----------------------------------------------------------------------------//
bool Window::isActive(void) const
{
    const bool parent_active = !d_parent ? true : getParent()->isActive();

    return d_active && parent_active;
}

//----------------------------------------------------------------------------//
bool Window::isChild(uint ID) const
{
    const size_t child_count = getChildCount();

    for (size_t i = 0; i < child_count; ++i)
        if (getChildAtIdx(i)->getID() == ID)
            return true;

    return false;
}

//----------------------------------------------------------------------------//
bool Window::isChildRecursive(uint ID) const
{
    const size_t child_count = getChildCount();

    for (size_t i = 0; i < child_count; ++i)
        if (getChildAtIdx(i)->getID() == ID || getChildAtIdx(i)->isChildRecursive(ID))
            return true;

    return false;
}

//----------------------------------------------------------------------------//
Window* Window::getChild(uint ID) const
{
    const size_t child_count = getChildCount();

    for (size_t i = 0; i < child_count; ++i)
        if (getChildAtIdx(i)->getID() == ID)
            return getChildAtIdx(i);

    char strbuf[16];
    sprintf(strbuf, "%X", ID);
    CEGUI_THROW(UnknownObjectException("A Window with ID: '" +
        String(strbuf) + "' is not attached to Window '" + d_name + "'."));
}

//----------------------------------------------------------------------------//
Window* Window::getChildRecursive(uint ID) const
{
    const size_t child_count = getChildCount();

    std::queue<Element*> ElementsToSearch;

    for (size_t i = 0; i < child_count; ++i) // load all children into the queue
    {
        Element* child = getChildElementAtIdx(i);
        ElementsToSearch.push(child);
    }

    while (!ElementsToSearch.empty()) // breadth-first search for the child to find
    {
        Element* child = ElementsToSearch.front();
        ElementsToSearch.pop();

        Window* window = dynamic_cast<Window*>(child);
        if (window)
        {
            if (window->getID() == ID)
            {
                return window;
            }
        }

        const size_t element_child_count = child->getChildCount();
        for(size_t i = 0; i < element_child_count; ++i)
        {
            ElementsToSearch.push(child->getChildElementAtIdx(i));
        }
    }

    return 0;
}

//----------------------------------------------------------------------------//
Window* Window::getActiveChild(void)
{
    return const_cast<Window*>(
        static_cast<const Window*>(this)->getActiveChild());
}

//----------------------------------------------------------------------------//
const Window* Window::getActiveChild(void) const
{
    // are children can't be active if we are not
    if (!isActive())
        return 0;

    for (ChildDrawList::const_reverse_iterator it = d_drawList.rbegin(); it != d_drawList.rend(); ++it)
    {
        // don't need full backward scan for activeness as we already know
        // 'this' is active.  NB: This uses the draw-ordered child list, as that
        // should be quicker in most cases.
        
        const Window* wnd = *it;
        if (wnd->d_active)
            return wnd->getActiveChild();
    }

    // no child was active, therefore we are the topmost active window
    return this;
}

//----------------------------------------------------------------------------//
bool Window::isAncestor(uint ID) const
{
    // return false if we have no ancestor
    if (!d_parent)
        return false;

    // check our immediate parent
    if (getParent()->getID() == ID)
        return true;

    // not our parent, check back up the family line
    return getParent()->isAncestor(ID);
}

//----------------------------------------------------------------------------//
const Font* Window::getFont(bool useDefault) const
{
    if (!d_font)
        return useDefault ? getGUIContext().getDefaultFont() : 0;

    return d_font;
}

//----------------------------------------------------------------------------//
float Window::getEffectiveAlpha(void) const
{
    if (!d_parent || !inheritsAlpha())
        return d_alpha;

    return d_alpha * getParent()->getEffectiveAlpha();
}

//----------------------------------------------------------------------------//
const Rectf& Window::getOuterRectClipper() const
{
    if (!d_outerRectClipperValid)
    {
        d_outerRectClipper = getOuterRectClipper_impl();
        d_outerRectClipperValid = true;
    }

    return d_outerRectClipper;
}

//----------------------------------------------------------------------------//
const Rectf& Window::getInnerRectClipper() const
{
    if (!d_innerRectClipperValid)
    {
        d_innerRectClipper = getInnerRectClipper_impl();
        d_innerRectClipperValid = true;
    }

    return d_innerRectClipper;
}

//----------------------------------------------------------------------------//
const Rectf& Window::getClipRect(const bool non_client) const
{
    return non_client ? getOuterRectClipper() : getInnerRectClipper();
}

//----------------------------------------------------------------------------//
const Rectf& Window::getHitTestRect() const
{
    if (!d_hitTestRectValid)
    {
        d_hitTestRect = getHitTestRect_impl();
        d_hitTestRectValid = true;
    }

    return d_hitTestRect;
}

//----------------------------------------------------------------------------//
Rectf Window::getParentElementClipIntersection(const Rectf& unclipped_area) const
{
    return unclipped_area.getIntersection(
        (d_parent && d_clippedByParent) ?
            getParent()->getClipRect(isNonClient()) :
            Rectf(Vector2f(0, 0), getRootContainerSize()));
}

//----------------------------------------------------------------------------//
Rectf Window::getUnclippedInnerRect_impl(bool skipAllPixelAlignment) const
{
    // TODO: skip all pixel alignment!
    return d_windowRenderer ? d_windowRenderer->getUnclippedInnerRect() :
                              (skipAllPixelAlignment ? getUnclippedOuterRect().getFresh(true) : getUnclippedOuterRect().get());
}

//----------------------------------------------------------------------------//
Rectf Window::getOuterRectClipper_impl() const
{
    return (d_surface && d_surface->isRenderingWindow()) ?
        getUnclippedOuterRect().get() :
        getParentElementClipIntersection(getUnclippedOuterRect().get());
}

//----------------------------------------------------------------------------//
Rectf Window::getInnerRectClipper_impl() const
{
    return (d_surface && d_surface->isRenderingWindow()) ?
        getUnclippedInnerRect().get() :
        getParentElementClipIntersection(getUnclippedInnerRect().get());
}

//----------------------------------------------------------------------------//
Rectf Window::getHitTestRect_impl() const
{
    // if clipped by parent wnd, hit test area is the intersection of our
    // outer rect with the parent's hit test area intersected with the
    // parent's clipper.
    if (d_parent && d_clippedByParent)
    {
        return getUnclippedOuterRect().get().getIntersection(
            getParent()->getHitTestRect().getIntersection(
                getParent()->getClipRect(isNonClient())));
    }
    // not clipped to parent wnd, so get intersection with screen area.
    else
    {
        return getUnclippedOuterRect().get().getIntersection(
            Rectf(Vector2f(0, 0), getRootContainerSize()));
    }
}

//----------------------------------------------------------------------------//
bool Window::isHit(const Vector2f& position, const bool allow_disabled) const
{
    // cannot be hit if we are disabled.
    if (!allow_disabled && isEffectiveDisabled())
        return false;

    const Rectf test_area(getHitTestRect());

    if ((test_area.getWidth() == 0.0f) || (test_area.getHeight() == 0.0f))
        return false;

    return test_area.isPointInRect(position);
}

//----------------------------------------------------------------------------//
Window* Window::getChildAtPosition(const Vector2f& position) const
{
    return getChildAtPosition(position, &Window::isHit);
}

//----------------------------------------------------------------------------//
Window* Window::getChildAtPosition(const Vector2f& position,
                    bool (Window::*hittestfunc)(const Vector2f&, bool) const,
                    bool allow_disabled) const
{
    Vector2f p;
    // if the window has RenderingWindow backing
    if (d_surface && d_surface->isRenderingWindow())
        static_cast<RenderingWindow*>(d_surface)->unprojectPoint(position, p);
    else
        p = position;

    const ChildDrawList::const_reverse_iterator end = d_drawList.rend();
    ChildDrawList::const_reverse_iterator child;

    for (child = d_drawList.rbegin(); child != end; ++child)
    {
        if ((*child)->isEffectiveVisible())
        {
            // recursively scan for hit on children of this child window...
            if (Window* const wnd = (*child)->getChildAtPosition(p, hittestfunc, allow_disabled))
                return wnd;
            // see if this child is hit and return it's pointer if it is
            else if (((*child)->*hittestfunc)(p, allow_disabled))
                return *child;
        }
    }

    // nothing hit
    return 0;
}

//----------------------------------------------------------------------------//
Window* Window::getTargetChildAtPosition(const Vector2f& position,
                                         const bool allow_disabled) const
{
    return getChildAtPosition(position, &Window::isHitTargetWindow, allow_disabled);
}

//----------------------------------------------------------------------------//
bool Window::isHitTargetWindow(const Vector2f& position, bool allow_disabled) const
{
    return !isMousePassThroughEnabled() && isHit(position, allow_disabled);
}

//----------------------------------------------------------------------------//
void Window::setAlwaysOnTop(bool setting)
{
    // only react to an actual change
    if (isAlwaysOnTop() == setting)
        return;

    d_alwaysOnTop = setting;

    // move us in front of sibling windows with the same 'always-on-top'
    // setting as we have.
    if (d_parent)
    {
        Window* const org_parent = getParent();

        org_parent->removeChild_impl(this);
        org_parent->addChild_impl(this);

        onZChange_impl();
    }

    WindowEventArgs args(this);
    onAlwaysOnTopChanged(args);
}

//----------------------------------------------------------------------------//
void Window::setEnabled(bool setting)
{
    // only react if setting has changed
    if (d_enabled == setting)
        return;

    d_enabled = setting;
    WindowEventArgs args(this);

    if (d_enabled)
    {
        // check to see if the window is actually enabled (which depends
        // upon all ancestor windows being enabled) we do this so that
        // events we fire give an accurate indication of the state of a
        // window.
        if ((d_parent && !getParent()->isDisabled()) || !d_parent)
            onEnabled(args);
    }
    else
    {
        onDisabled(args);
    }

    getGUIContext().updateWindowContainingMouse();
}

//----------------------------------------------------------------------------//
void Window::setDisabled(bool setting)
{
    setEnabled(!setting);
}

//----------------------------------------------------------------------------//
void Window::setVisible(bool setting)
{
    // only react if setting has changed
    if (d_visible == setting)
        return;

    d_visible = setting;
    WindowEventArgs args(this);
    d_visible ? onShown(args) : onHidden(args);

    getGUIContext().updateWindowContainingMouse();
}

//----------------------------------------------------------------------------//
void Window::activate(void)
{
    // exit if the window is not visible, since a hidden window may not be the
    // active window.
    if (!isEffectiveVisible())
        return;

    // force complete release of input capture.
    // NB: This is not done via releaseCapture() because that has
    // different behaviour depending on the restoreOldCapture setting.
    if (getCaptureWindow() && getCaptureWindow() != this)
    {
        Window* const tmpCapture = getCaptureWindow();
        getGUIContext().setInputCaptureWindow(0);

        WindowEventArgs args(0);
        tmpCapture->onCaptureLost(args);
    }

    moveToFront();
}

//----------------------------------------------------------------------------//
void Window::deactivate(void)
{
    ActivationEventArgs args(this);
    args.otherWindow = 0;
    onDeactivated(args);
}

//----------------------------------------------------------------------------//
void Window::setClippedByParent(bool setting)
{
    // only react if setting has changed
    if (d_clippedByParent == setting)
        return;

    d_clippedByParent = setting;
    WindowEventArgs args(this);
    onClippingChanged(args);
}

//----------------------------------------------------------------------------//
void Window::setText(const String& text)
{
    d_textLogical = text;
    d_renderedStringValid = false;
    d_bidiDataValid = false;

    WindowEventArgs args(this);
    onTextChanged(args);
}

//----------------------------------------------------------------------------//
void Window::setFont(const Font* font)
{
    d_font = font;
    d_renderedStringValid = false;
    WindowEventArgs args(this);
    onFontChanged(args);
}

//----------------------------------------------------------------------------//
void Window::setFont(const String& name)
{
    setFont(!name.empty() ? &FontManager::getSingleton().get(name) : 0);
}

//----------------------------------------------------------------------------//
void Window::removeChild(uint ID)
{
    const size_t child_count = getChildCount();

    for (size_t i = 0; i < child_count; ++i)
    {
        if (getChildAtIdx(i)->getID() == ID)
        {
            removeChild(d_children[i]);
            return;
        }

    }
}

//----------------------------------------------------------------------------//
Window* Window::createChild(const String& type, const String& name)
{
    Window* ret = WindowManager::getSingleton().createWindow(type, name);
    addChild(ret);

    return ret;
}

//----------------------------------------------------------------------------//
void Window::destroyChild(Window* wnd)
{
    assert(isChild(wnd) && "Window you are attempting to destroy is not a child!");

    WindowManager::getSingleton().destroyWindow(wnd);
}

//----------------------------------------------------------------------------//
void Window::destroyChild(const String& name_path)
{
    destroyChild(getChild(name_path));
}

//----------------------------------------------------------------------------//
void Window::moveToFront()
{
    moveToFront_impl(false);
}

//----------------------------------------------------------------------------//
bool Window::moveToFront_impl(bool wasClicked)
{
    bool took_action = false;

    // if the window has no parent then we can have no siblings
    if (!d_parent)
    {
        // perform initial activation if required.
        if (!isActive())
        {
            took_action = true;
            ActivationEventArgs args(this);
            args.otherWindow = 0;
            onActivated(args);
        }

        return took_action;
    }

    // bring parent window to front of it's siblings
    took_action = getParent()->moveToFront_impl(wasClicked);

    // get immediate child of parent that is currently active (if any)
    Window* const activeWnd = getActiveSibling();

    // if a change in active window has occurred
    if (activeWnd != this)
    {
        took_action = true;

        // notify ourselves that we have become active
        ActivationEventArgs args(this);
        args.otherWindow = activeWnd;
        onActivated(args);

        // notify any previously active window that it is no longer active
        if (activeWnd)
        {
            args.window = activeWnd;
            args.otherWindow = this;
            args.handled = 0;
            activeWnd->onDeactivated(args);
        }
    }

    // bring us to the front of our siblings
    if (d_zOrderingEnabled &&
        (!wasClicked || d_riseOnClick) &&
        !isTopOfZOrder())
    {
        took_action = true;

        // remove us from our parent's draw list
        getParent()->removeWindowFromDrawList(*this);
        // re-attach ourselves to our parent's draw list which will move us in
        // front of sibling windows with the same 'always-on-top' setting as we
        // have.
        getParent()->addWindowToDrawList(*this);
        // notify relevant windows about the z-order change.
        onZChange_impl();
    }

    return took_action;
}

//----------------------------------------------------------------------------//
void Window::moveToBack()
{
    // if the window is active, de-activate it.
    if (isActive())
    {
        ActivationEventArgs args(this);
        args.otherWindow = 0;
        onDeactivated(args);
    }

    // we only proceed if we have a parent (otherwise we can have no siblings)
    if (d_parent)
    {
        if (d_zOrderingEnabled)
        {
            // remove us from our parent's draw list
            getParent()->removeWindowFromDrawList(*this);
            // re-attach ourselves to our parent's draw list which will move us
            // in behind sibling windows with the same 'always-on-top' setting
            // as we have.
            getParent()->addWindowToDrawList(*this, true);
            // notify relevant windows about the z-order change.
            onZChange_impl();
        }

        getParent()->moveToBack();
    }
}

//----------------------------------------------------------------------------//
bool Window::captureInput(void)
{
    // we can only capture if we are the active window (LEAVE THIS ALONE!)
    if (!isActive())
        return false;

    if (!isCapturedByThis())
    {
        Window* const current_capture = getCaptureWindow();
        getGUIContext().setInputCaptureWindow(this);
        WindowEventArgs args(this);

        // inform window which previously had capture that it doesn't anymore.
        if (current_capture && current_capture != this && !d_restoreOldCapture)
            current_capture->onCaptureLost(args);

        if (d_restoreOldCapture)
            d_oldCapture = current_capture;

        onCaptureGained(args);
    }

    return true;
}

//----------------------------------------------------------------------------//
void Window::releaseInput(void)
{
    // if we are not the window that has capture, do nothing
    if (!isCapturedByThis())
        return;

    // restore old captured window if that mode is set
    if (d_restoreOldCapture)
    {
        getGUIContext().setInputCaptureWindow(d_oldCapture);

        // check for case when there was no previously captured window
        if (d_oldCapture)
        {
            d_oldCapture = 0;
            getCaptureWindow()->moveToFront();
        }

    }
    else
        getGUIContext().setInputCaptureWindow(0);

    WindowEventArgs args(this);
    onCaptureLost(args);
}

//----------------------------------------------------------------------------//
void Window::setRestoreOldCapture(bool setting)
{
    d_restoreOldCapture = setting;

    const size_t child_count = getChildCount();

    for (size_t i = 0; i < child_count; ++i)
        getChildAtIdx(i)->setRestoreOldCapture(setting);
}

//----------------------------------------------------------------------------//
void Window::setAlpha(const float alpha)
{
    // clamp this to the valid range [0.0, 1.0]
    float clampedAlpha = ceguimax(ceguimin(alpha, 1.0f), 0.0f);

    // Ensure that the new alpha value is actually different from the currently set one
    // to avoid unnecessary invalidating and re-caching of textures and children
    if (d_alpha == clampedAlpha)
        return;

    d_alpha = clampedAlpha;

    WindowEventArgs args(this);
    onAlphaChanged(args);
}

//----------------------------------------------------------------------------//
void Window::setInheritsAlpha(bool setting)
{
    if (d_inheritsAlpha != setting)
    {
        // store old effective alpha so we can test if alpha value changes due
        // to new setting.
        const float oldAlpha = getEffectiveAlpha();

        // notify about the setting change.
        d_inheritsAlpha = setting;

        WindowEventArgs args(this);
        onInheritsAlphaChanged(args);

        // if effective alpha has changed fire notification about that too
        if (oldAlpha != getEffectiveAlpha())
        {
            args.handled = 0;
            onAlphaChanged(args);
        }
    }
}

//----------------------------------------------------------------------------//
void Window::invalidate(void)
{
    invalidate(false);
}

//----------------------------------------------------------------------------//
void Window::invalidate(const bool recursive)
{
    invalidate_impl(recursive);
    getGUIContext().markAsDirty();
}

//----------------------------------------------------------------------------//
void Window::invalidate_impl(const bool recursive)
{
    d_needsRedraw = true;
    invalidateRenderingSurface();

    WindowEventArgs args(this);
    onInvalidated(args);

    if (recursive)
    {
        const size_t child_count = getChildCount();
        for (size_t i = 0; i < child_count; ++i)
            getChildAtIdx(i)->invalidate_impl(true);
    }
}

//----------------------------------------------------------------------------//
void Window::render()
{
    // don't do anything if window is not visible
    if (!isEffectiveVisible())
        return;
    
    // get rendering context
    RenderingContext ctx;
    getRenderingContext(ctx);

    // clear geometry from surface if it's ours
    if (ctx.owner == this)
        ctx.surface->clearGeometry();

    // redraw if no surface set, or if surface is invalidated
    if (!d_surface || d_surface->isInvalidated())
    {
        // perform drawing for 'this' Window
        drawSelf(ctx);

        // render any child windows
        for (ChildDrawList::iterator it = d_drawList.begin(); it != d_drawList.end(); ++it)
        {
            (*it)->render();
        }
    }

    // do final rendering for surface if it's ours
    if (ctx.owner == this)
        ctx.surface->draw();
}

//----------------------------------------------------------------------------//
void Window::drawSelf(const RenderingContext& ctx)
{
    bufferGeometry(ctx);
    queueGeometry(ctx);
}

//----------------------------------------------------------------------------//
void Window::bufferGeometry(const RenderingContext&)
{
    if (d_needsRedraw)
    {
        // dispose of already cached geometry.
        d_geometry->reset();

        // signal rendering started
        WindowEventArgs args(this);
        onRenderingStarted(args);

        // HACK: ensure our rendered string content is up to date
        getRenderedString();

        // get derived class or WindowRenderer to re-populate geometry buffer.
        if (d_windowRenderer)
            d_windowRenderer->render();
        else
            populateGeometryBuffer();

        // signal rendering ended
        args.handled = 0;
        onRenderingEnded(args);

        // mark ourselves as no longer needed a redraw.
        d_needsRedraw = false;
    }
}

//----------------------------------------------------------------------------//
void Window::queueGeometry(const RenderingContext& ctx)
{
    // add geometry so that it gets drawn to the target surface.
    ctx.surface->addGeometryBuffer(ctx.queue, *d_geometry);
}

//----------------------------------------------------------------------------//
void Window::setParent(Element* parent)
{
    Element::setParent(parent);
    syncTargetSurface();
}

//----------------------------------------------------------------------------//
void Window::syncTargetSurface()
{
    // if we do not have a surface, xfer any surfaces from our children to
    // whatever our target surface now is.
    if (!d_surface)
        transferChildSurfaces();
    // else, since we have a surface, child surfaces stay with us.  Though we
    // must now ensure /our/ surface is xferred if it is a RenderingWindow.
    else if (d_surface->isRenderingWindow())
    {
        // target surface is eihter the parent's target, or the gui context.
        RenderingSurface& tgt = d_parent ?
            getParent()->getTargetRenderingSurface() : getGUIContext();

        tgt.transferRenderingWindow(static_cast<RenderingWindow&>(*d_surface));
    }
}

//----------------------------------------------------------------------------//
void Window::cleanupChildren(void)
{
    while(getChildCount() != 0)
    {
        Window* wnd = static_cast<Window*>(d_children[0]);

        // always remove child
        removeChild(wnd);

        // destroy child if that is required
        if (wnd->isDestroyedByParent())
            WindowManager::getSingleton().destroyWindow(wnd);
    }
}

//----------------------------------------------------------------------------//
void Window::addChild_impl(Element* element)
{
    Window* wnd = dynamic_cast<Window*>(element);
    
    if (!wnd)
        CEGUI_THROW(InvalidRequestException(
            "Window can only have Elements of type Window added as children "
            "(Window path: " + getNamePath() + ")."));
    
    // if the element is already a child of this Window, this is a NOOP
    if (isChild(element))
        return;
        
    NamedElement::addChild_impl(wnd);

    addWindowToDrawList(*wnd);

    wnd->invalidate(true);

    wnd->onZChange_impl();
}

//----------------------------------------------------------------------------//
void Window::removeChild_impl(Element* element)
{
    Window* wnd = static_cast<Window*>(element);

    Window* const capture_wnd = getCaptureWindow();
    if ((capture_wnd && wnd) &&
        (capture_wnd == wnd || capture_wnd->isAncestor(wnd)))
            getCaptureWindow()->releaseInput();

    // remove from draw list
    removeWindowFromDrawList(*wnd);

    Element::removeChild_impl(wnd);
    
    // find this window in the child list
    const ChildList::iterator position =
        std::find(d_children.begin(), d_children.end(), wnd);

    // if the window was found in the child list
    if (position != d_children.end())
    {
        // unban properties window could write as a root window
        wnd->unbanPropertyFromXML(RestoreOldCapturePropertyName);
    }
    
    wnd->onZChange_impl();

    // Removed windows should not be active anymore (they are not attached
    // to anything so this would not make sense)
    if(wnd->isActive())
    {
        wnd->deactivate();
    }
}

//----------------------------------------------------------------------------//
void Window::onZChange_impl(void)
{
    if (!d_parent)
    {
        WindowEventArgs args(this);
        onZChanged(args);
    }
    else
    {
        const size_t child_count = d_parent->getChildCount();

        for (size_t i = 0; i < child_count; ++i)
        {
            WindowEventArgs args(getParent()->getChildAtIdx(i));
            getParent()->getChildAtIdx(i)->onZChanged(args);
        }

    }

    getGUIContext().updateWindowContainingMouse();
}

//----------------------------------------------------------------------------//
const Image* Window::getMouseCursor(bool useDefault) const
{
    if (d_mouseCursor)
        return d_mouseCursor;
    else
        return useDefault ? getGUIContext().getMouseCursor().getDefaultImage() : 0;
}

//----------------------------------------------------------------------------//
void Window::setMouseCursor(const String& name)
{
    setMouseCursor(
        &ImageManager::getSingleton().get(name));
}

//----------------------------------------------------------------------------//
void Window::setMouseCursor(const Image* image)
{
    d_mouseCursor = image;

    if (getGUIContext().getWindowContainingMouse() == this)
        getGUIContext().getMouseCursor().setImage(image);
}

//----------------------------------------------------------------------------//
void Window::setID(uint ID)
{
    if (d_ID == ID)
        return;

    d_ID = ID;

    WindowEventArgs args(this);
    onIDChanged(args);
}

//----------------------------------------------------------------------------//
void Window::setDestroyedByParent(bool setting)
{
    if (d_destroyedByParent == setting)
        return;

    d_destroyedByParent = setting;

    WindowEventArgs args(this);
    onParentDestroyChanged(args);
}

//----------------------------------------------------------------------------//
void Window::generateAutoRepeatEvent(MouseButton button)
{
    MouseEventArgs ma(this);
    ma.position = getUnprojectedPosition(
        getGUIContext().getMouseCursor().getPosition());
    ma.moveDelta = Vector2f(0.0f, 0.0f);
    ma.button = button;
    ma.sysKeys = getGUIContext().getSystemKeys().get();
    ma.wheelChange = 0;
    onMouseButtonDown(ma);
}

//----------------------------------------------------------------------------//
void Window::addWindowProperties(void)
{
    const String propertyOrigin("Window");

    CEGUI_DEFINE_PROPERTY(Window, float,
        AlphaPropertyName, "Property to get/set the alpha value of the Window. Value is floating point number.",
        &Window::setAlpha, &Window::getAlpha, 1.0f
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        AlwaysOnTopPropertyName, "Property to get/set the 'always on top' setting for the Window. Value is either \"true\" or \"false\".",
        &Window::setAlwaysOnTop, &Window::isAlwaysOnTop, false
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        ClippedByParentPropertyName, "Property to get/set the 'clipped by parent' setting for the Window. Value is either \"true\" or \"false\".",
        &Window::setClippedByParent, &Window::isClippedByParent, true
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        DestroyedByParentPropertyName, "Property to get/set the 'destroyed by parent' setting for the Window. Value is either \"true\" or \"false\".",
        &Window::setDestroyedByParent, &Window::isDestroyedByParent, true
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        DisabledPropertyName, "Property to get/set the 'disabled state' setting for the Window.  Value is either \"true\" or \"false\".",
        &Window::setDisabled, &Window::isDisabled, false
    );

    CEGUI_DEFINE_PROPERTY(Window, Font*,
        FontPropertyName,"Property to get/set the font for the Window.  Value is the name of the font to use (must be loaded already).",
        &Window::setFont, &Window::property_getFont, 0
    );

    CEGUI_DEFINE_PROPERTY(Window, uint,
        IDPropertyName, "Property to get/set the ID value of the Window. Value is an unsigned integer number.",
        &Window::setID, &Window::getID, 0
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        InheritsAlphaPropertyName, "Property to get/set the 'inherits alpha' setting for the Window. Value is either \"true\" or \"false\".",
        &Window::setInheritsAlpha, &Window::inheritsAlpha, true
    );

    CEGUI_DEFINE_PROPERTY(Window, Image*,
        MouseCursorImagePropertyName,"Property to get/set the mouse cursor image for the Window.  Value should be \"<image name>\".",
        &Window::setMouseCursor, &Window::property_getMouseCursor, 0
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        VisiblePropertyName, "Property to get/set the 'visible state' setting for the Window. Value is either \"true\" or \"false\".",
        &Window::setVisible, &Window::isVisible, true
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        RestoreOldCapturePropertyName, "Property to get/set the 'restore old capture' setting for the Window. Value is either \"true\" or \"false\".",
        &Window::setRestoreOldCapture, &Window::restoresOldCapture, false
    );

    CEGUI_DEFINE_PROPERTY(Window, String,
        TextPropertyName, "Property to get/set the text / caption for the Window. Value is the text string to use. Meaning of this property heavily depends on the type of the Window.",
        &Window::setText, &Window::getText, ""
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        ZOrderingEnabledPropertyName, "Property to get/set the 'z-order changing enabled' setting for the Window. Value is either \"true\" or \"false\".",
        &Window::setZOrderingEnabled, &Window::isZOrderingEnabled, true
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        WantsMultiClickEventsPropertyName, "Property to get/set whether the window will receive double-click and triple-click events. Value is either \"true\" or \"false\".",
        &Window::setWantsMultiClickEvents, &Window::wantsMultiClickEvents, true
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        MouseAutoRepeatEnabledPropertyName, "Property to get/set whether the window will receive autorepeat mouse button down events. Value is either \"true\" or \"false\".",
        &Window::setMouseAutoRepeatEnabled, &Window::isMouseAutoRepeatEnabled, false
    );

    CEGUI_DEFINE_PROPERTY(Window, float,
        AutoRepeatDelayPropertyName, "Property to get/set the autorepeat delay. Value is a floating point number indicating the delay required in seconds.",
        &Window::setAutoRepeatDelay, &Window::getAutoRepeatDelay, 0.3f
    );

    CEGUI_DEFINE_PROPERTY(Window, float,
        AutoRepeatRatePropertyName, "Property to get/set the autorepeat rate. Value is a floating point number indicating the rate required in seconds.",
        &Window::setAutoRepeatRate, &Window::getAutoRepeatRate, 0.06f
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        DistributeCapturedInputsPropertyName, "Property to get/set whether captured inputs are passed to child windows. Value is either \"true\" or \"false\".",
        &Window::setDistributesCapturedInputs, &Window::distributesCapturedInputs, false
    );

    CEGUI_DEFINE_PROPERTY(Window, String,
        TooltipTypePropertyName, "Property to get/set the custom tooltip for the window. Value is the type name of the custom tooltip. If \"\", the default System tooltip is used.",
        &Window::setTooltipType, &Window::getTooltipType, ""
    );

    CEGUI_DEFINE_PROPERTY(Window, String,
        TooltipTextPropertyName, "Property to get/set the tooltip text for the window. Value is the tooltip text for the window.",
        &Window::setTooltipText, &Window::getTooltipText, ""
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        InheritsTooltipTextPropertyName, "Property to get/set whether the window inherits its parents tooltip text when it has none of its own. Value is either \"true\" or \"false\".",
        &Window::setInheritsTooltipText, &Window::inheritsTooltipText, true
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        RiseOnClickEnabledPropertyName, "Property to get/set whether the window will come to the top of the Z-order when clicked. Value is either \"true\" or \"false\".",
        &Window::setRiseOnClickEnabled, &Window::isRiseOnClickEnabled, true
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        MousePassThroughEnabledPropertyName, "Property to get/set whether the window ignores mouse events and pass them through to any windows behind it. Value is either \"true\" or \"false\".",
        &Window::setMousePassThroughEnabled, &Window::isMousePassThroughEnabled, false
    );
    
    addProperty(&d_windowRendererProperty);
    addProperty(&d_lookNFeelProperty);

    CEGUI_DEFINE_PROPERTY(Window, bool,
        DragDropTargetPropertyName, "Property to get/set whether the Window will receive drag and drop related notifications.  Value is either \"true\" or \"false\".",
        &Window::setDragDropTarget, &Window::isDragDropTarget, true
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        AutoRenderingSurfacePropertyName, "Property to get/set whether the Window will automatically attempt to "
        "use a full imagery caching RenderingSurface (if supported by the "
        "renderer).  Here, full imagery caching usually will mean caching a "
        "window's representation onto a texture (although no such "
        "implementation requirement is specified.)"
        "  Value is either \"true\" or \"false\".",
        &Window::setUsingAutoRenderingSurface, &Window::isUsingAutoRenderingSurface, false /* TODO: Inconsistency*/
    );
    
    CEGUI_DEFINE_PROPERTY(Window, bool,
        TextParsingEnabledPropertyName, "Property to get/set the text parsing setting for the Window.  "
        "Value is either \"true\" or \"false\".",
        &Window::setTextParsingEnabled, &Window::isTextParsingEnabled, true
    );
   
    CEGUI_DEFINE_PROPERTY(Window, UBox,
        MarginPropertyName, "Property to get/set margin for the Window. Value format:"
        "{top:{[tops],[topo]},left:{[lefts],[lefto]},bottom:{[bottoms],[bottomo]},right:{[rights],[righto]}}.",
        &Window::setMargin, &Window::getMargin, UBox(UDim(0, 0))
    );
   
    CEGUI_DEFINE_PROPERTY(Window, WindowUpdateMode,
        UpdateModePropertyName, "Property to get/set the window update mode setting.  "
        "Value is one of \"Always\", \"Never\" or \"Visible\".",
        &Window::setUpdateMode,&Window::getUpdateMode, WUM_VISIBLE
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        MouseInputPropagationEnabledPropertyName, "Property to get/set whether unhandled mouse inputs should be "
        "propagated back to the Window's parent.  "
        "Value is either \"true\" or \"false\".",
        &Window::setMouseInputPropagationEnabled, &Window::isMouseInputPropagationEnabled, false
    );

    CEGUI_DEFINE_PROPERTY(Window, bool,
        AutoWindowPropertyName, "Property to get/set whether the system considers this window to be an "
        "automatically created sub-component window."
        "Value is either \"true\" or \"false\".",
        &Window::setAutoWindow, &Window::isAutoWindow, false
    );
}

//----------------------------------------------------------------------------//
bool Window::isZOrderingEnabled(void) const
{
    return d_zOrderingEnabled;
}

//----------------------------------------------------------------------------//
void Window::setZOrderingEnabled(bool setting)
{
    d_zOrderingEnabled = setting;
}

//----------------------------------------------------------------------------//
bool Window::wantsMultiClickEvents(void) const
{
    return d_wantsMultiClicks;
}

//----------------------------------------------------------------------------//
void Window::setWantsMultiClickEvents(bool setting)
{
    d_wantsMultiClicks = setting;
}

//----------------------------------------------------------------------------//
bool Window::isMouseAutoRepeatEnabled(void) const
{
    return d_autoRepeat;
}

//----------------------------------------------------------------------------//
float Window::getAutoRepeatDelay(void) const
{
    return d_repeatDelay;
}

//----------------------------------------------------------------------------//
float Window::getAutoRepeatRate(void) const
{
    return d_repeatRate;
}

//----------------------------------------------------------------------------//
void Window::setMouseAutoRepeatEnabled(bool setting)
{
    if (d_autoRepeat == setting)
        return;

    d_autoRepeat = setting;
    d_repeatButton = NoButton;

    // FIXME: There is a potential issue here if this setting is
    // FIXME: changed _while_ the mouse is auto-repeating, and
    // FIXME: the 'captured' state of input could get messed up.
    // FIXME: The alternative is to always release here, but that
    // FIXME: has a load of side effects too - so for now nothing
    // FIXME: is done.  This whole aspect of the system needs a
    // FIXME: review an reworking - though such a change was
    // FIXME: beyond the scope of the bug-fix that originated this
    // FIXME: comment block.  PDT - 30/10/06
}

//----------------------------------------------------------------------------//
void Window::setAutoRepeatDelay(float delay)
{
    d_repeatDelay = delay;
}

//----------------------------------------------------------------------------//
void Window::setAutoRepeatRate(float rate)
{
    d_repeatRate = rate;
}

//----------------------------------------------------------------------------//
void Window::update(float elapsed)
{       
    // perform update for 'this' Window
    updateSelf(elapsed);

    // update underlying RenderingWinodw if needed
    if (d_surface && d_surface->isRenderingWindow())
        static_cast<RenderingWindow*>(d_surface)->update(elapsed);

    UpdateEventArgs e(this,elapsed);
    fireEvent(EventUpdated,e,EventNamespace);

    // update child windows
    for (size_t i = 0; i < getChildCount(); ++i)
    {
        // update children based on their WindowUpdateMode setting.
        if (getChildAtIdx(i)->d_updateMode == WUM_ALWAYS ||
                (getChildAtIdx(i)->d_updateMode == WUM_VISIBLE &&
                 getChildAtIdx(i)->isVisible()))
        {
            getChildAtIdx(i)->update(elapsed);
        }
    }
}

//----------------------------------------------------------------------------//
void Window::updateSelf(float elapsed)
{
    // Mouse button autorepeat processing.
    if (d_autoRepeat && d_repeatButton != NoButton)
    {
        d_repeatElapsed += elapsed;

        if (d_repeating)
        {
            if (d_repeatElapsed > d_repeatRate)
            {
                d_repeatElapsed -= d_repeatRate;
                // trigger the repeated event
                generateAutoRepeatEvent(d_repeatButton);
            }
        }
        else
        {
            if (d_repeatElapsed > d_repeatDelay)
            {
                d_repeatElapsed = 0;
                d_repeating = true;
                // trigger the repeated event
                generateAutoRepeatEvent(d_repeatButton);
            }
        }
    }

    // allow for updates within an assigned WindowRenderer
    if (d_windowRenderer)
        d_windowRenderer->update(elapsed);
}

//----------------------------------------------------------------------------//
bool Window::performCopy(Clipboard& /*clipboard*/)
{
    // deny copying by default
    return false;
}

//----------------------------------------------------------------------------//
bool Window::performCut(Clipboard& /*clipboard*/)
{
    // deny cutting by default
    return false;
}

//----------------------------------------------------------------------------//
bool Window::performPaste(Clipboard& /*clipboard*/)
{
    // deny pasting by default
    return false;
}

//----------------------------------------------------------------------------//
bool Window::distributesCapturedInputs(void) const
{
    return d_distCapturedInputs;
}

//----------------------------------------------------------------------------//
void Window::setDistributesCapturedInputs(bool setting)
{
    d_distCapturedInputs = setting;
}

//----------------------------------------------------------------------------//
void Window::notifyDragDropItemEnters(DragContainer* item)
{
    if (!item)
        return;

    DragDropEventArgs args(this);
    args.dragDropItem = item;
    onDragDropItemEnters(args);
}

//----------------------------------------------------------------------------//
void Window::notifyDragDropItemLeaves(DragContainer* item)
{
    if (!item)
        return;

    DragDropEventArgs args(this);
    args.dragDropItem = item;
    onDragDropItemLeaves(args);
}

//----------------------------------------------------------------------------//
void Window::notifyDragDropItemDropped(DragContainer* item)
{
    if (!item)
        return;

    DragDropEventArgs args(this);
    args.dragDropItem = item;
    onDragDropItemDropped(args);
}

//----------------------------------------------------------------------------//
void Window::destroy(void)
{
    // because we know that people do not read the API ref properly,
    // here is some protection to ensure that WindowManager does the
    // destruction and not anyone else.
    WindowManager& wmgr = WindowManager::getSingleton();

    if (wmgr.isAlive(this))
    {
        wmgr.destroyWindow(this);

        // now return, the rest of what we need to do will happen
        // once WindowManager re-calls this method.
        return;
    }

    // signal our imminent destruction
    WindowEventArgs args(this);
    onDestructionStarted(args);

    // Check we are detached from parent
    if (d_parent)
        d_parent->removeChild(this);

    releaseInput();

    // let go of the tooltip if we have it
    Tooltip* const tip = getTooltip();
    if (tip && tip->getTargetWindow()==this)
        tip->setTargetWindow(0);

    // ensure custom tooltip is cleaned up
    setTooltip(static_cast<Tooltip*>(0));
    


    // clean up looknfeel related things
    if (!d_lookName.empty())
    {
        d_windowRenderer->onLookNFeelUnassigned();
        WidgetLookManager::getSingleton().getWidgetLook(d_lookName).
            cleanUpWidget(*this);
    }

    // free any assigned WindowRenderer
    if (d_windowRenderer != 0)
    {
        d_windowRenderer->onDetach();
        WindowRendererManager::getSingleton().
            destroyWindowRenderer(d_windowRenderer);
        d_windowRenderer = 0;
    }

    cleanupChildren();

    releaseRenderingWindow();
    invalidate();
}

//----------------------------------------------------------------------------//
bool Window::isUsingDefaultTooltip(void) const
{
    return d_customTip == 0;
}

//----------------------------------------------------------------------------//
Tooltip* Window::getTooltip(void) const
{
    return isUsingDefaultTooltip() ? 
        getGUIContext().getDefaultTooltipObject(): d_customTip;
}

//----------------------------------------------------------------------------//
void Window::setTooltip(Tooltip* tooltip)
{
    // destroy current custom tooltip if one exists and we created it
    if (d_customTip && d_weOwnTip)
        WindowManager::getSingleton().destroyWindow(d_customTip);

    // set new custom tooltip
    d_weOwnTip = false;
    d_customTip = tooltip;
}

//----------------------------------------------------------------------------//
void Window::setTooltipType(const String& tooltipType)
{
    // destroy current custom tooltip if one exists and we created it
    if (d_customTip && d_weOwnTip)
        WindowManager::getSingleton().destroyWindow(d_customTip);

    if (tooltipType.empty())
    {
        d_customTip = 0;
        d_weOwnTip = false;
    }
    else
    {
        CEGUI_TRY
        {
            d_customTip = static_cast<Tooltip*>(
                WindowManager::getSingleton().createWindow(
                    tooltipType, getName() + TooltipNameSuffix));
            d_customTip->setAutoWindow(true);
            d_weOwnTip = true;
        }
        CEGUI_CATCH (UnknownObjectException&)
        {
            d_customTip = 0;
            d_weOwnTip = false;
        }
    }
}

//----------------------------------------------------------------------------//
String Window::getTooltipType(void) const
{
    return isUsingDefaultTooltip() ? String("") : d_customTip->getType();
}

//----------------------------------------------------------------------------//
void Window::setTooltipText(const String& tip)
{
    d_tooltipText = tip;

    Tooltip* const tooltip = getTooltip();

    if (tooltip && tooltip->getTargetWindow() == this)
        tooltip->setText(tip);
}

//----------------------------------------------------------------------------//
const String& Window::getTooltipText(void) const
{
    return d_tooltipText;
}

//----------------------------------------------------------------------------//
const String& Window::getTooltipTextIncludingInheritance(void) const
{
    if (d_inheritsTipText && d_parent && d_tooltipText.empty())
        return getParent()->getTooltipText();
    else
        return d_tooltipText;
}

//----------------------------------------------------------------------------//
bool Window::inheritsTooltipText(void) const
{
    return d_inheritsTipText;
}

//----------------------------------------------------------------------------//
void Window::setInheritsTooltipText(bool setting)
{
    d_inheritsTipText = setting;
}

//----------------------------------------------------------------------------//
void Window::setArea_impl(const UVector2& pos, const USize& size,
                          bool topLeftSizing, bool fireEvents)
{
    markCachedWindowRectsInvalid();
    Element::setArea_impl(pos, size, topLeftSizing, fireEvents);

    //if (moved || sized)
    // FIXME: This is potentially wasteful
    getGUIContext().updateWindowContainingMouse();

    // update geometry position and clipping if nothing from above appears to
    // have done so already (NB: may be occasionally wasteful, but fixes bugs!)
    if (!d_unclippedOuterRect.isCacheValid())
        updateGeometryRenderSettings();
}

//----------------------------------------------------------------------------//
void Window::markCachedWindowRectsInvalid()
{
    d_outerRectClipperValid = false;
    d_innerRectClipperValid = false;
    d_hitTestRectValid = false;
}

//----------------------------------------------------------------------------//
const String& Window::getLookNFeel() const
{
    return d_lookName;
}

//----------------------------------------------------------------------------//
void Window::setLookNFeel(const String& look)
{
    if (d_lookName == look)
        return;

    if (!d_windowRenderer)
        CEGUI_THROW(NullObjectException("There must be a "
            "window renderer assigned to the window '" + d_name +
            "' to set its look'n'feel"));

    WidgetLookManager& wlMgr = WidgetLookManager::getSingleton();
    if (!d_lookName.empty())
    {
        d_windowRenderer->onLookNFeelUnassigned();
        const WidgetLookFeel& wlf = wlMgr.getWidgetLook(d_lookName);
        wlf.cleanUpWidget(*this);
    }

    d_lookName = look;
    Logger::getSingleton().logEvent("Assigning LookNFeel '" + look +
        "' to window '" + d_name + "'.", Informative);

    // Work to initialise the look and feel...
    const WidgetLookFeel& wlf = wlMgr.getWidgetLook(look);
    // Get look and feel to initialise the widget as it needs.
    wlf.initialiseWidget(*this);
    // do the necessary binding to the stuff added by the look and feel
    initialiseComponents();
    // let the window renderer know about this
    d_windowRenderer->onLookNFeelAssigned();

    invalidate();
    performChildWindowLayout();
}

//----------------------------------------------------------------------------//
void Window::setModalState(bool state)
{
    // do nothing if state isn't changing
    if (getModalState() == state)
        return;

    // if going modal
    if (state)
    {
        activate();
        getGUIContext().setModalWindow(this);
    }
    // clear the modal target
    else
        getGUIContext().setModalWindow(0);
}

//----------------------------------------------------------------------------//
void Window::performChildWindowLayout(const bool nonclient_sized_hint,
                                      const bool client_sized_hint)
{
    const Sizef old_size(d_pixelSize);
    d_pixelSize = calculatePixelSize();

    layoutLookNFeelChildWidgets();

    const bool outer_changed = nonclient_sized_hint || d_pixelSize != old_size;
    const bool inner_changed = client_sized_hint || isInnerRectSizeChanged();

    d_outerRectClipperValid &= !outer_changed;
    d_innerRectClipperValid &= !inner_changed;

    if (d_windowRenderer)
        d_windowRenderer->performChildWindowLayout();

    notifyChildrenOfSizeChange(outer_changed, inner_changed);
}

//----------------------------------------------------------------------------//
void Window::layoutLookNFeelChildWidgets()
{
    if (d_lookName.empty())
        return;

    CEGUI_TRY
    {
        WidgetLookManager::getSingleton().
            getWidgetLook(d_lookName).layoutChildWidgets(*this);
    }
    CEGUI_CATCH (UnknownObjectException&)
    {
        Logger::getSingleton().logEvent(
            "Window::layoutLookNFeelChildWidgets: "
            "WidgetLook '" + d_lookName + "' was not found.", Errors);
    }
}

//----------------------------------------------------------------------------//
const String& Window::getUserString(const String& name) const
{
    UserStringMap::const_iterator iter = d_userStrings.find(name);

    if (iter == d_userStrings.end())
        CEGUI_THROW(UnknownObjectException(
            "a user string named '" + name + "' is not defined for Window '" +
            d_name + "'."));

    return (*iter).second;
}

//----------------------------------------------------------------------------//
bool Window::isUserStringDefined(const String& name) const
{
     return d_userStrings.find(name) != d_userStrings.end();
}

//----------------------------------------------------------------------------//
void Window::setUserString(const String& name, const String& value)
{
    d_userStrings[name] = value;
}

//----------------------------------------------------------------------------//
void Window::writeXMLToStream(XMLSerializer& xml_stream) const
{
    // just stop now if we are'nt allowed to write XML
    if (!d_allowWriteXML)
        return;

    // output opening Window tag
    xml_stream.openTag(WindowXMLElementName)
        .attribute(WindowTypeXMLAttributeName, getType());
    // write name if not auto-generated
    if (getName().compare(0, WindowManager::GeneratedWindowNameBase.length(),
                          WindowManager::GeneratedWindowNameBase) != 0)
    {
        xml_stream.attribute(WindowNameXMLAttributeName, getName());
    }
    // write out properties.
    writePropertiesXML(xml_stream);
    // write out attached child windows.
    writeChildWindowsXML(xml_stream);
    // now ouput closing Window tag
    xml_stream.closeTag();
}

//----------------------------------------------------------------------------//
int Window::writePropertiesXML(XMLSerializer& xml_stream) const
{
    int propertiesWritten = 0;
    PropertySet::PropertyIterator iter =  getPropertyIterator();

    while(!iter.isAtEnd())
    {
        // first we check to make sure the property is'nt banned from XML
        if (!isPropertyBannedFromXML(iter.getCurrentValue()))
        {
            CEGUI_TRY
            {
                // only write property if it's not at the default state
                if (!isPropertyAtDefault(iter.getCurrentValue()))
                {
                    iter.getCurrentValue()->writeXMLToStream(this, xml_stream);
                    ++propertiesWritten;
                }
            }
            CEGUI_CATCH (InvalidRequestException&)
            {
                // This catches errors from the MultiLineColumnList for example
                Logger::getSingleton().logEvent(
                    "Window::writePropertiesXML: property receiving failed.  "
                    "Continuing...", Errors);
            }
        }

        ++iter;
    }

    return propertiesWritten;
}

//----------------------------------------------------------------------------//
int Window::writeChildWindowsXML(XMLSerializer& xml_stream) const
{
    int windowsWritten = 0;

    for (uint i = 0; i < getChildCount(); ++i)
    {
        const Window* const child = getChildAtIdx(i);

        // conditional to ensure that auto created windows are handled
        // seperately.
        if (!child->isAutoWindow())
        {
            child->writeXMLToStream(xml_stream);
            ++windowsWritten;
        }
        // this is one of those auto created windows so we do some special
        // handling
        else if (child->writeAutoChildWindowXML(xml_stream))
        {
            ++windowsWritten;
        }
    }

    return windowsWritten;
}

//----------------------------------------------------------------------------//
bool Window::writeAutoChildWindowXML(XMLSerializer& xml_stream) const
{

    // just stop now if we are'nt allowed to write XML
    if (!d_allowWriteXML)
        return false;

    // we temporarily output to this string stream to see if have to do the tag
    // at all.  Make sure this stream does UTF-8
    std::ostringstream ss;
    XMLSerializer xml(ss);
    xml.openTag(AutoWindowXMLElementName);
    // Create the XML Child Tree
    // write out properties.
    writePropertiesXML(xml);
    // write out attached child windows.
    writeChildWindowsXML(xml);
    xml.closeTag();
    if (xml.getTagCount() <= 1)
        return false;

    // output opening AutoWindow tag
    xml_stream.openTag(AutoWindowXMLElementName);
    // write name suffix attribute
    xml_stream.attribute(AutoWindowNamePathXMLAttributeName, getName());
    // Inefficient : do the XML serialization again
    // write out properties.
    writePropertiesXML(xml_stream);
    // write out attached child windows.
    writeChildWindowsXML(xml_stream);
    xml_stream.closeTag();

    return true;
}

//----------------------------------------------------------------------------//
void Window::addWindowToDrawList(Window& wnd, bool at_back)
{
    // add behind other windows in same group
    if (at_back)
    {
         // calculate position where window should be added for drawing
        ChildDrawList::iterator pos = d_drawList.begin();
        if (wnd.isAlwaysOnTop())
        {
            // find first topmost window
            while ((pos != d_drawList.end()) && (!(*pos)->isAlwaysOnTop()))
                ++pos;
        }
        // add window to draw list
        d_drawList.insert(pos, &wnd);
    }
    // add in front of other windows in group
    else
    {
        // calculate position where window should be added for drawing
        ChildDrawList::reverse_iterator position = d_drawList.rbegin();
        if (!wnd.isAlwaysOnTop())
        {
            // find last non-topmost window
            while ((position != d_drawList.rend()) && ((*position)->isAlwaysOnTop()))
                ++position;
        }
        // add window to draw list
        d_drawList.insert(position.base(), &wnd);
    }
}

//----------------------------------------------------------------------------//
void Window::removeWindowFromDrawList(const Window& wnd)
{
    // if draw list is not empty
    if (!d_drawList.empty())
    {
        // attempt to find the window in the draw list
        const ChildDrawList::iterator position =
            std::find(d_drawList.begin(), d_drawList.end(), &wnd);

        // remove the window if it was found in the draw list
        if (position != d_drawList.end())
            d_drawList.erase(position);
    }
}

//----------------------------------------------------------------------------//
Window* Window::getActiveSibling()
{
    // initialise with this if we are active, else 0
    Window* activeWnd = isActive() ? this : 0;

    // if active window not already known, and we have a parent window
    if (!activeWnd && d_parent)
    {
        // scan backwards through the draw list, as this will
        // usually result in the fastest result.
        for (ChildDrawList::reverse_iterator it = getParent()->d_drawList.rbegin(); it != getParent()->d_drawList.rend(); ++it)
        {
            Window* wnd = *it;
            // if this child is active
            if (wnd->isActive())
            {
                // set the return value
                activeWnd = wnd;
                // exit loop early, as we have found what we needed
                break;
            }
        }
    }

    // return whatever we discovered
    return activeWnd;
}

//----------------------------------------------------------------------------//
void Window::onSized(ElementEventArgs& e)
{
    /*
     * Why are we not calling Element::onSized?  It's because that function
     * always calls the onParentSized notification for all children - we really
     * want that to be done via performChildWindowLayout instead and we
     * definitely don't want it done twice.
     *
     * (The other option was to add an Element::performChildLayout function -
     * maybe we should consider that).
    */

    // resize the underlying RenderingWindow if we're using such a thing
    if (d_surface && d_surface->isRenderingWindow())
        static_cast<RenderingWindow*>(d_surface)->setSize(getPixelSize());

    // screen area changes when we're resized.
    // NB: Called non-recursive since the performChildWindowLayout call should
    // have dealt more selectively with child Window cases.
    notifyScreenAreaChanged(false);
    performChildWindowLayout(true, true);

    invalidate();

    fireEvent(EventSized, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onMoved(ElementEventArgs& e)
{
    Element::onMoved(e);
    
    // handle invalidation of surfaces and trigger needed redraws
    if (d_parent)
    {
        getParent()->invalidateRenderingSurface();
        // need to redraw some geometry if parent uses a caching surface
        if (getParent()->getTargetRenderingSurface().isRenderingWindow())
            getGUIContext().markAsDirty();
    }
}

//----------------------------------------------------------------------------//
void Window::onTextChanged(WindowEventArgs& e)
{
    invalidate();
    fireEvent(EventTextChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onFontChanged(WindowEventArgs& e)
{
    // This was added to enable the Falagard FontDim to work
    // properly.  A better, more selective, solution would
    // probably be to do something funky with events ;)
    performChildWindowLayout();

    invalidate();
    fireEvent(EventFontChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onAlphaChanged(WindowEventArgs& e)
{
    // scan child list and call this method for all children that inherit alpha
    const size_t child_count = getChildCount();

    for (size_t i = 0; i < child_count; ++i)
    {
        if (getChildAtIdx(i)->inheritsAlpha())
        {
            WindowEventArgs args(getChildAtIdx(i));
            getChildAtIdx(i)->onAlphaChanged(args);
        }

    }

    invalidate();
    fireEvent(EventAlphaChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onIDChanged(WindowEventArgs& e)
{
    fireEvent(EventIDChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onShown(WindowEventArgs& e)
{
    invalidate();
    fireEvent(EventShown, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onHidden(WindowEventArgs& e)
{
    // first deactivate window if it is the active window.
    if (isActive())
        deactivate();

    invalidate();
    fireEvent(EventHidden, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onEnabled(WindowEventArgs& e)
{
    // signal all non-disabled children that they are now enabled
    // (via inherited state)
    const size_t child_count = getChildCount();
    for (size_t i = 0; i < child_count; ++i)
    {
        if (getChildAtIdx(i)->d_enabled)
        {
            WindowEventArgs args(getChildAtIdx(i));
            getChildAtIdx(i)->onEnabled(args);
        }
    }

    invalidate();
    fireEvent(EventEnabled, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onDisabled(WindowEventArgs& e)
{
    // signal all non-disabled children that they are now disabled
    // (via inherited state)
    const size_t child_count = getChildCount();
    for (size_t i = 0; i < child_count; ++i)
    {
        if (getChildAtIdx(i)->d_enabled)
        {
            WindowEventArgs args(getChildAtIdx(i));
            getChildAtIdx(i)->onDisabled(args);
        }
    }

    invalidate();
    fireEvent(EventDisabled, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onClippingChanged(WindowEventArgs& e)
{
    invalidate();
    notifyClippingChanged();
    fireEvent(EventClippedByParentChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onParentDestroyChanged(WindowEventArgs& e)
{
    fireEvent(EventDestroyedByParentChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onInheritsAlphaChanged(WindowEventArgs& e)
{
    invalidate();
    fireEvent(EventInheritsAlphaChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onAlwaysOnTopChanged(WindowEventArgs& e)
{
    // we no longer want a total redraw here, instead we just get each window
    // to resubmit it's imagery to the Renderer.
    getGUIContext().markAsDirty();
    fireEvent(EventAlwaysOnTopChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onCaptureGained(WindowEventArgs& e)
{
    fireEvent(EventInputCaptureGained, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onCaptureLost(WindowEventArgs& e)
{
    // reset auto-repeat state
    d_repeatButton = NoButton;

    // handle restore of previous capture window as required.
    if (d_restoreOldCapture && (d_oldCapture != 0)) {
        d_oldCapture->onCaptureLost(e);
        d_oldCapture = 0;
    }

    // handle case where mouse is now in a different window
    // (this is a bit of a hack that uses the mouse input injector to handle
    // this for us).
    getGUIContext().injectMouseMove(0, 0);

    fireEvent(EventInputCaptureLost, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onInvalidated(WindowEventArgs& e)
{
    fireEvent(EventInvalidated, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onRenderingStarted(WindowEventArgs& e)
{
    fireEvent(EventRenderingStarted, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onRenderingEnded(WindowEventArgs& e)
{
    fireEvent(EventRenderingEnded, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onZChanged(WindowEventArgs& e)
{
    // we no longer want a total redraw here, instead we just get each window
    // to resubmit it's imagery to the Renderer.
    getGUIContext().markAsDirty();
    fireEvent(EventZOrderChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onDestructionStarted(WindowEventArgs& e)
{
    d_destructionStarted = true;
    fireEvent(EventDestructionStarted, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onActivated(ActivationEventArgs& e)
{
    d_active = true;
    invalidate();
    fireEvent(EventActivated, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onDeactivated(ActivationEventArgs& e)
{
    // first de-activate all children
    const size_t child_count = getChildCount();
    for (size_t i = 0; i < child_count; ++i)
    {
        if (getChildAtIdx(i)->isActive())
        {
            // make sure the child gets itself as the .window member
            ActivationEventArgs child_e(getChildAtIdx(i));
            child_e.otherWindow = e.otherWindow;
            getChildAtIdx(i)->onDeactivated(child_e);
        }

    }

    d_active = false;
    invalidate();
    fireEvent(EventDeactivated, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onParentSized(ElementEventArgs& e)
{
    Element::onParentSized(e);

    // if we were not moved or sized, do child layout anyway!
    // URGENT FIXME
    //if (!(moved || sized))
    performChildWindowLayout();
}

//----------------------------------------------------------------------------//
void Window::onChildAdded(ElementEventArgs& e)
{
    // we no longer want a total redraw here, instead we just get each window
    // to resubmit it's imagery to the Renderer.
    getGUIContext().markAsDirty();
    
    Element::onChildAdded(e);
}

//----------------------------------------------------------------------------//
void Window::onChildRemoved(ElementEventArgs& e)
{
    // we no longer want a total redraw here, instead we just get each window
    // to resubmit it's imagery to the Renderer.
    getGUIContext().markAsDirty();
    // Though we do need to invalidate the rendering surface!
    getTargetRenderingSurface().invalidate();
    
    Element::onChildRemoved(e);
}

//----------------------------------------------------------------------------//
void Window::onMouseEntersArea(MouseEventArgs& e)
{
    d_containsMouse = true;
    fireEvent(EventMouseEntersArea, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onMouseLeavesArea(MouseEventArgs& e)
{
    d_containsMouse = false;
    fireEvent(EventMouseLeavesArea, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onMouseEnters(MouseEventArgs& e)
{
    // set the mouse cursor
    getGUIContext().
        getMouseCursor().setImage(getMouseCursor());

    // perform tooltip control
    Tooltip* const tip = getTooltip();
    if (tip && !isAncestor(tip))
        tip->setTargetWindow(this);

    fireEvent(EventMouseEntersSurface, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onMouseLeaves(MouseEventArgs& e)
{
    // perform tooltip control
    const Window* const mw = getGUIContext().getWindowContainingMouse();
    Tooltip* const tip = getTooltip();
    if (tip && mw != tip && !(mw && mw->isAncestor(tip)))
        tip->setTargetWindow(0);

    fireEvent(EventMouseLeavesSurface, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onMouseMove(MouseEventArgs& e)
{
    // perform tooltip control
    Tooltip* const tip = getTooltip();
    if (tip)
        tip->resetTimer();

    fireEvent(EventMouseMove, e, EventNamespace);

    // optionally propagate to parent
    if (!e.handled && d_propagateMouseInputs &&
        d_parent && this != getGUIContext().getModalWindow())
    {
        e.window = getParent();
        getParent()->onMouseMove(e);

        return;
    }

    // by default we now mark mouse events as handled
    // (derived classes may override, of course!)
    ++e.handled;
}

//----------------------------------------------------------------------------//
void Window::onMouseWheel(MouseEventArgs& e)
{
    fireEvent(EventMouseWheel, e, EventNamespace);

    // optionally propagate to parent
    if (!e.handled && d_propagateMouseInputs &&
        d_parent && this != getGUIContext().getModalWindow())
    {
        e.window = getParent();
        getParent()->onMouseWheel(e);

        return;
    }

    // by default we now mark mouse events as handled
    // (derived classes may override, of course!)
    ++e.handled;
}

//----------------------------------------------------------------------------//
void Window::onMouseButtonDown(MouseEventArgs& e)
{
    // perform tooltip control
    Tooltip* const tip = getTooltip();
    if (tip)
        tip->setTargetWindow(0);

    if ((e.button == LeftButton) && moveToFront_impl(true))
        ++e.handled;

    // if auto repeat is enabled and we are not currently tracking
    // the button that was just pushed (need this button check because
    // it could be us that generated this event via auto-repeat).
    if (d_autoRepeat)
    {
        if (d_repeatButton == NoButton)
            captureInput();

        if ((d_repeatButton != e.button) && isCapturedByThis())
        {
            d_repeatButton = e.button;
            d_repeatElapsed = 0;
            d_repeating = false;
        }
    }

    fireEvent(EventMouseButtonDown, e, EventNamespace);

    // optionally propagate to parent
    if (!e.handled && d_propagateMouseInputs &&
        d_parent && this != getGUIContext().getModalWindow())
    {
        e.window = getParent();
        getParent()->onMouseButtonDown(e);

        return;
    }

    // by default we now mark mouse events as handled
    // (derived classes may override, of course!)
    ++e.handled;
}

//----------------------------------------------------------------------------//
void Window::onMouseButtonUp(MouseEventArgs& e)
{
    // reset auto-repeat state
    if (d_autoRepeat && d_repeatButton != NoButton)
    {
        releaseInput();
        d_repeatButton = NoButton;
    }

    fireEvent(EventMouseButtonUp, e, EventNamespace);

    // optionally propagate to parent
    if (!e.handled && d_propagateMouseInputs &&
        d_parent && this != getGUIContext().getModalWindow())
    {
        e.window = getParent();
        getParent()->onMouseButtonUp(e);

        return;
    }

    // by default we now mark mouse events as handled
    // (derived classes may override, of course!)
    ++e.handled;
}

//----------------------------------------------------------------------------//
void Window::onMouseClicked(MouseEventArgs& e)
{
    fireEvent(EventMouseClick, e, EventNamespace);

    // optionally propagate to parent
    if (!e.handled && d_propagateMouseInputs &&
        d_parent && this != getGUIContext().getModalWindow())
    {
        e.window = getParent();
        getParent()->onMouseClicked(e);

        return;
    }

    // if event was directly injected, mark as handled to be consistent with
    // other mouse button injectors
    if (!getGUIContext().isMouseClickEventGenerationEnabled())
        ++e.handled;
}

//----------------------------------------------------------------------------//
void Window::onMouseDoubleClicked(MouseEventArgs& e)
{
    fireEvent(EventMouseDoubleClick, e, EventNamespace);

    // optionally propagate to parent
    if (!e.handled && d_propagateMouseInputs &&
        d_parent && this != getGUIContext().getModalWindow())
    {
        e.window = getParent();
        getParent()->onMouseDoubleClicked(e);

        return;
    }

    ++e.handled;
}

//----------------------------------------------------------------------------//
void Window::onMouseTripleClicked(MouseEventArgs& e)
{
    fireEvent(EventMouseTripleClick, e, EventNamespace);

    // optionally propagate to parent
    if (!e.handled && d_propagateMouseInputs &&
        d_parent && this != getGUIContext().getModalWindow())
    {
        e.window = getParent();
        getParent()->onMouseTripleClicked(e);

        return;
    }

    ++e.handled;
}

//----------------------------------------------------------------------------//
void Window::onKeyDown(KeyEventArgs& e)
{
    fireEvent(EventKeyDown, e, EventNamespace);

    // As of 0.7.0 CEGUI::System no longer does input event propogation, so by
    // default we now do that here.  Generally speaking key handling widgets
    // may need to override this behaviour to halt further propogation.
    if (!e.handled && d_parent &&
        this != getGUIContext().getModalWindow())
    {
        e.window = getParent();
        getParent()->onKeyDown(e);
    }
}

//----------------------------------------------------------------------------//
void Window::onKeyUp(KeyEventArgs& e)
{
    fireEvent(EventKeyUp, e, EventNamespace);

    // As of 0.7.0 CEGUI::System no longer does input event propogation, so by
    // default we now do that here.  Generally speaking key handling widgets
    // may need to override this behaviour to halt further propogation.
    if (!e.handled && d_parent &&
        this != getGUIContext().getModalWindow())
    {
        e.window = getParent();
        getParent()->onKeyUp(e);
    }
}

//----------------------------------------------------------------------------//
void Window::onCharacter(KeyEventArgs& e)
{
    fireEvent(EventCharacterKey, e, EventNamespace);

    // As of 0.7.0 CEGUI::System no longer does input event propogation, so by
    // default we now do that here.  Generally speaking key handling widgets
    // may need to override this behaviour to halt further propogation.
    if (!e.handled && d_parent &&
        this != getGUIContext().getModalWindow())
    {
        e.window = getParent();
        getParent()->onCharacter(e);
    }
}

//----------------------------------------------------------------------------//
void Window::onDragDropItemEnters(DragDropEventArgs& e)
{
    fireEvent(EventDragDropItemEnters, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onDragDropItemLeaves(DragDropEventArgs& e)
{
    fireEvent(EventDragDropItemLeaves, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onDragDropItemDropped(DragDropEventArgs& e)
{
    fireEvent(EventDragDropItemDropped, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::setWindowRenderer(const String& name)
{
    if (d_windowRenderer && d_windowRenderer->getName() == name)
        return;

    WindowRendererManager& wrm = WindowRendererManager::getSingleton();
    if (d_windowRenderer != 0)
    {
        // Allow reset of renderer
        if (d_windowRenderer->getName() == name)
            return;

        WindowEventArgs e(this);
        onWindowRendererDetached(e);
        wrm.destroyWindowRenderer(d_windowRenderer);
    }

    if (!name.empty())
    {
        Logger::getSingleton().logEvent("Assigning the window renderer '" +
            name + "' to the window '" + d_name + "'", Informative);
        d_windowRenderer = wrm.createWindowRenderer(name);
        WindowEventArgs e(this);
        onWindowRendererAttached(e);
    }
    else
        CEGUI_THROW(InvalidRequestException(
            "Attempt to assign a 'null' window renderer to window '" +
            d_name + "'."));
}

//----------------------------------------------------------------------------//
WindowRenderer* Window::getWindowRenderer(void) const
{
    return d_windowRenderer;
}

//----------------------------------------------------------------------------//
void Window::onWindowRendererAttached(WindowEventArgs& e)
{
    if (!validateWindowRenderer(d_windowRenderer))
        CEGUI_THROW(InvalidRequestException(
            "The window renderer '" + d_windowRenderer->getName() + "' is not "
            "compatible with this widget type (" + getType() + ")"));

    d_windowRenderer->d_window = this;
    d_windowRenderer->onAttach();
    fireEvent(EventWindowRendererAttached, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onWindowRendererDetached(WindowEventArgs& e)
{
    d_windowRenderer->onDetach();
    d_windowRenderer->d_window = 0;
    fireEvent(EventWindowRendererDetached, e, EventNamespace);
}

//----------------------------------------------------------------------------//
bool Window::validateWindowRenderer(const WindowRenderer*) const
{
    return true;
}

//----------------------------------------------------------------------------//
const String& Window::getWindowRendererName() const
{
    if (d_windowRenderer)
        return d_windowRenderer->getName();

    static String empty("");
    return empty;
}


//----------------------------------------------------------------------------//
void Window::banPropertyFromXML(const String& property_name)
{
	Property* instance = getPropertyInstance(property_name);
	if (!instance->isWritable())
	{
		Logger::getSingleton().logEvent("Property '" + property_name + "' "
				"is not writable so it's implicitly banned from XML. No need "
				"to ban it manually", Warnings);

		return;
	}

    // check if the insertion failed
    if (!d_bannedXMLProperties.insert(property_name).second)
        // just log the incidence
        CEGUI_LOGINSANE("Window::banPropertyFromXML: The property '" +
            property_name + "' is already banned in window '" + d_name + "'");
}

//----------------------------------------------------------------------------//
void Window::banPropertyFromXMLRecursive(const String& property_name)
{
    banPropertyFromXML(property_name);

    const size_t childCount = getChildCount();
    for(size_t i = 0; i < childCount; ++i)
    {
        getChildAtIdx(i)->banPropertyFromXMLRecursive(property_name);
    }
}

//----------------------------------------------------------------------------//
void Window::unbanPropertyFromXML(const String& property_name)
{
    d_bannedXMLProperties.erase(property_name);
}

//----------------------------------------------------------------------------//
void Window::unbanPropertyFromXMLRecursive(const String& property_name)
{
    unbanPropertyFromXML(property_name);

    const size_t childCount = getChildCount();
    for(size_t i = 0; i < childCount; ++i)
    {
        getChildAtIdx(i)->unbanPropertyFromXMLRecursive(property_name);
    }
}

//----------------------------------------------------------------------------//
bool Window::isPropertyBannedFromXML(const String& property_name) const
{
    const BannedXMLPropertySet::const_iterator i =
        d_bannedXMLProperties.find(property_name);

    // generally, there will always less banned properties than all properties,
    // so it makes sense to check that first before querying the property instance
    if (i != d_bannedXMLProperties.end())
    {
    	return true;
    }

    // properties that don't write any XML code are implicitly banned

    // read-only properties are implicitly banned
    // (such stored information wouldn't be of any value in the XML anyways,
    // no way to apply it to the widget)
    Property* instance = getPropertyInstance(property_name);
    return (!instance->doesWriteXML() || !instance->isWritable());
}

//----------------------------------------------------------------------------//
void Window::banPropertyFromXML(const Property* property)
{
    if (property)
        banPropertyFromXML(property->getName());
}

//----------------------------------------------------------------------------//
void Window::unbanPropertyFromXML(const Property* property)
{
    if (property)
        unbanPropertyFromXML(property->getName());
}

//----------------------------------------------------------------------------//
bool Window::isPropertyBannedFromXML(const Property* property) const
{
    if (property)
        return isPropertyBannedFromXML(property->getName());
    else
        return false;
}

//----------------------------------------------------------------------------//
bool Window::isPropertyAtDefault(const Property* property) const
{
    // if we have a looknfeel we examine it for defaults
    if (!d_lookName.empty())
    {
        if (d_parent && !getParent()->getLookNFeel().empty())
        {
            const WidgetLookFeel& wlf =
                WidgetLookManager::getSingleton().
                    getWidgetLook(getParent()->getLookNFeel());

            // if this property is a target of a PropertyLink, we always report
            // as being at default.  NB: This check is only performed on the
            // immediate parent.
            const WidgetLookFeel::PropertyLinkDefinitionList& pldl(wlf.getPropertyLinkDefinitions());
            WidgetLookFeel::PropertyLinkDefinitionList::const_iterator i = pldl.begin();
            for (; i != pldl.end(); ++i)
            {
                if ((*i)->getPropertyName() == property->getName())
                    return true;
            }

            // for an auto-window see if the property is is set via a Property
            // tag within the WidgetComponent that defines it.
            if (d_autoWindow)
            {
                // find the widget component if possible
                const WidgetComponent* const wc = wlf.findWidgetComponent(getName());
                if (wc)
                {
                    const PropertyInitialiser* const propinit =
                        wc->findPropertyInitialiser(property->getName());

                    if (propinit)
                        return (getProperty(property->getName()) ==
                                propinit->getInitialiserValue());
                }
            }
        }

        // if the looknfeel has a new default for this property we compare
        // against that
        const WidgetLookFeel& wlf =
            WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        const PropertyInitialiser* const propinit =
            wlf.findPropertyInitialiser(property->getName());
        if (propinit)
            return (getProperty(property->getName()) ==
                    propinit->getInitialiserValue());
    }

    // we don't have a looknfeel with a new value for this property so we rely
    // on the hardcoded default
    return property->isDefault(this);
}

//----------------------------------------------------------------------------//
void Window::notifyClippingChanged(void)
{
    markCachedWindowRectsInvalid();

    // inform children that their clipped screen areas must be updated
    const size_t num = d_children.size();
    for (size_t i=0; i<num; ++i)
        if (getChildAtIdx(i)->isClippedByParent())
            getChildAtIdx(i)->notifyClippingChanged();
}

//----------------------------------------------------------------------------//
void Window::notifyScreenAreaChanged(bool recursive /* = true */)
{
    markCachedWindowRectsInvalid();
    Element::notifyScreenAreaChanged(recursive);

    updateGeometryRenderSettings();
}

//----------------------------------------------------------------------------//
void Window::updateGeometryRenderSettings()
{
    RenderingContext ctx;
    getRenderingContext(ctx);

    // move the underlying RenderingWindow if we're using such a thing
    if (ctx.owner == this && ctx.surface->isRenderingWindow())
    {
        static_cast<RenderingWindow*>(ctx.surface)->
            setPosition(getUnclippedOuterRect().get().getPosition());
        static_cast<RenderingWindow*>(d_surface)->setPivot(
            Vector3f(d_pixelSize.d_width / 2.0f,
                    d_pixelSize.d_height / 2.0f,
                    0.0f));
        d_geometry->setTranslation(Vector3f(0.0f, 0.0f, 0.0f));
    }
    // if we're not texture backed, update geometry position.
    else
    {
        // position is the offset of the window on the dest surface.
        const Rectf ucrect(getUnclippedOuterRect().get());
        d_geometry->setTranslation(Vector3f(ucrect.d_min.d_x - ctx.offset.d_x,
                                            ucrect.d_min.d_y - ctx.offset.d_y, 0.0f));
    }
    initialiseClippers(ctx);
}

//----------------------------------------------------------------------------//
bool Window::isDragDropTarget() const
{
    return d_dragDropTarget;
}

//----------------------------------------------------------------------------//
void Window::setDragDropTarget(bool setting)
{
    d_dragDropTarget = setting;
}

//-----------------------------------------------------------------------
void Window::setFalagardType(const String& type, const String& rendererType)
{
    // Retrieve the new widget look
    const String separator("/");
    String::size_type pos = type.find(separator);
    const String newLook(type, 0, pos);

    // Check if old one is the same. If so, ignore since we don't need to do
    // anything (type is already assigned)
    pos = d_falagardType.find(separator);
    String oldLook(d_falagardType, 0, pos);
    if(oldLook == newLook)
        return;

    // Obtain widget kind
    String widget(d_falagardType, pos + 1);

    // Build new type (look/widget)
    d_falagardType = newLook + separator + widget;

    // Set new renderer
    if(rendererType.length() > 0)
        setWindowRenderer(rendererType);

    // Apply the new look to the widget
    setLookNFeel(type);
}

//----------------------------------------------------------------------------//
bool Window::isTopOfZOrder() const
{
    // if not attached, then always on top!
    if (!d_parent)
        return true;

    // get position of window at top of z-order in same group as this window
    ChildDrawList::reverse_iterator pos = getParent()->d_drawList.rbegin();
    if (!d_alwaysOnTop)
    {
        // find last non-topmost window
        while ((pos != getParent()->d_drawList.rend()) && (*pos)->isAlwaysOnTop())
            ++pos;
    }

    // return whether the window at the top of the z order is us
    return *pos == this;
}

//----------------------------------------------------------------------------//
void Window::insertText(const String& text, const String::size_type position)
{
    d_textLogical.insert(position, text);
    d_renderedStringValid = false;
    d_bidiDataValid = false;

    WindowEventArgs args(this);
    onTextChanged(args);
}

//----------------------------------------------------------------------------//
void Window::appendText(const String& text)
{
    d_textLogical.append(text);
    d_renderedStringValid = false;
    d_bidiDataValid = false;

    WindowEventArgs args(this);
    onTextChanged(args);
}

//----------------------------------------------------------------------------//
GeometryBuffer& Window::getGeometryBuffer()
{
    return *d_geometry;
}

//----------------------------------------------------------------------------//
void Window::getRenderingContext(RenderingContext& ctx) const
{
    if (d_windowRenderer)
        d_windowRenderer->getRenderingContext(ctx);
    else
        getRenderingContext_impl(ctx);
}

//----------------------------------------------------------------------------//
void Window::getRenderingContext_impl(RenderingContext& ctx) const
{
    if (d_surface)
    {
        ctx.surface = d_surface;
        ctx.owner = this;
        ctx.offset = getUnclippedOuterRect().get().getPosition();
        ctx.queue = RQ_BASE;
    }
    else if (d_parent)
    {
        getParent()->getRenderingContext(ctx);
    }
    else
    {
        ctx.surface = &getGUIContext();
        ctx.owner = 0;
        ctx.offset = Vector2f(0, 0);
        ctx.queue = RQ_BASE;
    }
}

//----------------------------------------------------------------------------//
RenderingSurface* Window::getRenderingSurface() const
{
    return d_surface;
}

//----------------------------------------------------------------------------//
RenderingSurface& Window::getTargetRenderingSurface() const
{
    if (d_surface)
        return *d_surface;
    else if (d_parent)
        return getParent()->getTargetRenderingSurface();
    else
        return getGUIContext();
}

//----------------------------------------------------------------------------//
void Window::setRenderingSurface(RenderingSurface* surface)
{
    if (d_surface == surface)
        return;

    if (d_autoRenderingWindow)
        setUsingAutoRenderingSurface(false);

    d_surface = surface;

    // transfer child surfaces to this new surface
    if (d_surface)
    {
        transferChildSurfaces();
        notifyScreenAreaChanged();
    }
}

//----------------------------------------------------------------------------//
void Window::invalidateRenderingSurface()
{
    // invalidate our surface chain if we have one
    if (d_surface)
        d_surface->invalidate();
    // else look through the hierarchy for a surface chain to invalidate.
    else if (d_parent)
        getParent()->invalidateRenderingSurface();
}

//----------------------------------------------------------------------------//
const Window* Window::getRootWindow() const
{
    return d_parent ? getParent()->getRootWindow() : this;
}

//----------------------------------------------------------------------------//
Window* Window::getRootWindow()
{
    return const_cast<Window*>(
        static_cast<const Window*>(this)->getRootWindow());
}

//----------------------------------------------------------------------------//
bool Window::isUsingAutoRenderingSurface() const
{
    return d_autoRenderingWindow;
}

//----------------------------------------------------------------------------//
void Window::setUsingAutoRenderingSurface(bool setting)
{
    if (setting)
    {
        allocateRenderingWindow();
    }
    else
    {
        releaseRenderingWindow();

        // make sure we set this because releaseRenderingWindow won't do it
        // unless the surface was already initialised
        d_autoRenderingWindow = setting;
    }

    // while the actal area on screen may not have changed, the arrangement of
    // surfaces and geometry did...
    notifyScreenAreaChanged();
}

//----------------------------------------------------------------------------//
void Window::allocateRenderingWindow()
{
    if (!d_autoRenderingWindow)
    {
        d_autoRenderingWindow = true;

        TextureTarget* const t =
            System::getSingleton().getRenderer()->createTextureTarget();

        // TextureTargets may not be available, so check that first.
        if (!t)
        {
            Logger::getSingleton().logEvent("Window::allocateRenderingWindow - "
                "Failed to create a suitable TextureTarget for use by Window '"
                + d_name + "'", Errors);

            d_surface = 0;
            return;
        }

        d_surface = &getTargetRenderingSurface().createRenderingWindow(*t);
        transferChildSurfaces();

        // set size and position of RenderingWindow
        static_cast<RenderingWindow*>(d_surface)->setSize(getPixelSize());
        static_cast<RenderingWindow*>(d_surface)->
            setPosition(getUnclippedOuterRect().get().getPosition());

        getGUIContext().markAsDirty();
    }
}

//----------------------------------------------------------------------------//
void Window::releaseRenderingWindow()
{
    if (d_autoRenderingWindow && d_surface)
    {
        RenderingWindow* const old_surface =
            static_cast<RenderingWindow*>(d_surface);
        d_autoRenderingWindow = false;
        d_surface = 0;
        // detach child surfaces prior to destroying the owning surface
        transferChildSurfaces();
        // destroy surface and texture target it used
        TextureTarget* tt = &old_surface->getTextureTarget();
        old_surface->getOwner().destroyRenderingWindow(*old_surface);
        System::getSingleton().getRenderer()->destroyTextureTarget(tt);

        getGUIContext().markAsDirty();
    }
}

//----------------------------------------------------------------------------//
void Window::transferChildSurfaces()
{
    RenderingSurface& s = getTargetRenderingSurface();

    const size_t child_count = getChildCount();
    for (size_t i = 0; i < child_count; ++i)
    {
        Window* const c = getChildAtIdx(i);

        if (c->d_surface && c->d_surface->isRenderingWindow())
            s.transferRenderingWindow(
                *static_cast<RenderingWindow*>(c->d_surface));
        else
            c->transferChildSurfaces();
    }
}

//----------------------------------------------------------------------------//
void Window::initialiseClippers(const RenderingContext& ctx)
{
    if (ctx.surface->isRenderingWindow() && ctx.owner == this)
    {
        RenderingWindow* const rendering_window =
            static_cast<RenderingWindow*>(ctx.surface);

        if (d_clippedByParent && d_parent)
            rendering_window->setClippingRegion(
                getParent()->getClipRect(d_nonClient));
        else
            rendering_window->setClippingRegion(
                Rectf(Vector2f(0, 0), getRootContainerSize()));

        d_geometry->setClippingRegion(Rectf(Vector2f(0, 0), d_pixelSize));
    }
    else
    {
        Rectf geo_clip(getOuterRectClipper());

        if (geo_clip.getWidth() != 0.0f && geo_clip.getHeight() != 0.0f)
            geo_clip.offset(Vector2f(-ctx.offset.d_x, -ctx.offset.d_y));

        d_geometry->setClippingRegion(geo_clip);
    }
}

//----------------------------------------------------------------------------//
void Window::onRotated(ElementEventArgs& e)
{
    Element::onRotated(e);

    // TODO: Checking quaternion for equality with IDENTITY is stupid,
    //       change this to something else, checking with tolerance.
    if (d_rotation != Quaternion::IDENTITY && !d_surface)
    {
        // if we have no surface set and the rotation differs from identity,
        // enable the auto surface

        Logger::getSingleton().logEvent("Window::setRotation - "
            "Activating AutoRenderingSurface on Window '" + d_name +
            "' to enable rotation support.");

        setUsingAutoRenderingSurface(true);

        // still no surface?  Renderer or HW must not support what we need :(
        if (!d_surface)
        {
            Logger::getSingleton().logEvent("Window::setRotation - "
                "Failed to obtain a suitable ReneringWindow surface for "
                "Window '" + d_name + "'.  Rotation will not be available.",
                Errors);

            return;
        }
    }

    if (d_surface)
    {
        // ensure surface we have is the right type
        if (!d_surface->isRenderingWindow())
        {
            Logger::getSingleton().logEvent("Window::setRotation - "
                "Window '" + d_name + "' has a manual RenderingSurface that is not "
                "a RenderingWindow.  Rotation will not be available.", Errors);

            return;
        }

        // Checks / setup complete!  Now we can finally set the rotation.
        static_cast<RenderingWindow*>(d_surface)->setRotation(d_rotation);
        static_cast<RenderingWindow*>(d_surface)->setPivot(
            Vector3f(d_pixelSize.d_width / 2.0f, d_pixelSize.d_height / 2.0f, 0.0f));
    }
}

//----------------------------------------------------------------------------//
const RenderedString& Window::getRenderedString() const
{
    if (!d_renderedStringValid)
    {
        d_renderedString = getRenderedStringParser().parse(
            getTextVisual(), 0, 0);
        d_renderedStringValid = true;
    }

    return d_renderedString;
}

//----------------------------------------------------------------------------//
RenderedStringParser* Window::getCustomRenderedStringParser() const
{
    return d_customStringParser;
}

//----------------------------------------------------------------------------//
void Window::setCustomRenderedStringParser(RenderedStringParser* parser)
{
    d_customStringParser = parser;
    d_renderedStringValid = false;
}

//----------------------------------------------------------------------------//
RenderedStringParser& Window::getRenderedStringParser() const
{
    // if parsing is disabled, we use a DefaultRenderedStringParser that creates
    // a RenderedString to renderi the input text verbatim (i.e. no parsing).
    if (!d_textParsingEnabled)
        return d_defaultStringParser;

    // Next prefer a custom RenderedStringParser assigned to this Window.
    if (d_customStringParser)
        return *d_customStringParser;

    // Next prefer any globally set RenderedStringParser.
    RenderedStringParser* const global_parser =
        CEGUI::System::getSingleton().getDefaultCustomRenderedStringParser();
    if (global_parser)
        return *global_parser;

    // if parsing is enabled and no custom RenderedStringParser is set anywhere,
    // use the system's BasicRenderedStringParser to do the parsing.
    return d_basicStringParser;
}

//----------------------------------------------------------------------------//
Vector2f Window::getUnprojectedPosition(const Vector2f& pos) const
{
    RenderingSurface* rs = &getTargetRenderingSurface();

    // if window is not backed by RenderingWindow, return same pos.
    if (!rs->isRenderingWindow())
        return pos;

    // get first target RenderingWindow
    RenderingWindow* rw = static_cast<RenderingWindow*>(rs);

    // setup for loop
    Vector2f out_pos(pos);

    // while there are rendering windows
    while (rw)
    {
        // unproject the point for the current rw
        const Vector2f in_pos(out_pos);
        rw->unprojectPoint(in_pos, out_pos);

        // get next rendering window, if any
        rw = (rs = &rw->getOwner())->isRenderingWindow() ?
                static_cast<RenderingWindow*>(rs) : 0;
    }

    return out_pos;
}

//----------------------------------------------------------------------------//
const String& Window::getTextVisual() const
{
    // no bidi support
    if (!d_bidiVisualMapping)
        return d_textLogical;

    if (!d_bidiDataValid)
    {
        d_bidiVisualMapping->updateVisual(d_textLogical);
        d_bidiDataValid = true;
    }

    return d_bidiVisualMapping->getTextVisual();
}

//----------------------------------------------------------------------------//
bool Window::isTextParsingEnabled() const
{
    return d_textParsingEnabled;
}

//----------------------------------------------------------------------------//
void Window::setTextParsingEnabled(const bool setting)
{
    d_textParsingEnabled = setting;
    d_renderedStringValid = false;

    WindowEventArgs args(this);
    onTextParsingChanged(args);
}

//----------------------------------------------------------------------------//
void Window::setMargin(const UBox& margin)
{
    d_margin = margin;

    WindowEventArgs args(this);
    onMarginChanged(args);
}

//----------------------------------------------------------------------------//
const UBox& Window::getMargin() const
{
    return d_margin;
}

//----------------------------------------------------------------------------//
void Window::onTextParsingChanged(WindowEventArgs& e)
{
    fireEvent(EventTextParsingChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::onMarginChanged(WindowEventArgs& e)
{
    fireEvent(EventMarginChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Window::moveInFront(const Window* const window)
{
    if (!window || !window->d_parent || window->d_parent != d_parent ||
        window == this || window->d_alwaysOnTop != d_alwaysOnTop ||
        !d_zOrderingEnabled)
            return;

    // find our position in the parent child draw list
    const ChildDrawList::iterator p(std::find(getParent()->d_drawList.begin(),
                                              getParent()->d_drawList.end(),
                                              this));
    // sanity checK that we were attached to our parent.
    assert(p != getParent()->d_drawList.end());

    // erase us from our current position
    getParent()->d_drawList.erase(p);

    // find window we're to be moved in front of in parent's draw list
    ChildDrawList::iterator i(std::find(getParent()->d_drawList.begin(),
                                        getParent()->d_drawList.end(),
                                        window));
    // sanity check that target window was also attached to correct parent.
    assert(i != getParent()->d_drawList.end());

    // reinsert ourselves at the right location
    getParent()->d_drawList.insert(++i, this);

    // handle event notifications for affected windows.
    onZChange_impl();
}

//----------------------------------------------------------------------------//
void Window::moveBehind(const Window* const window)
{
    if (!window || !window->d_parent || window->d_parent != d_parent ||
        window == this || window->d_alwaysOnTop != d_alwaysOnTop ||
        !d_zOrderingEnabled)
            return;

    // find our position in the parent child draw list
    const ChildDrawList::iterator p(std::find(getParent()->d_drawList.begin(),
                                              getParent()->d_drawList.end(),
                                              this));
    // sanity checK that we were attached to our parent.
    assert(p != getParent()->d_drawList.end());

    // erase us from our current position
    getParent()->d_drawList.erase(p);

    // find window we're to be moved in front of in parent's draw list
    const ChildDrawList::iterator i(std::find(getParent()->d_drawList.begin(),
                                              getParent()->d_drawList.end(),
                                              window));
    // sanity check that target window was also attached to correct parent.
    assert(i != getParent()->d_drawList.end());

    // reinsert ourselves at the right location
    getParent()->d_drawList.insert(i, this);

    // handle event notifications for affected windows.
    onZChange_impl();
}

//----------------------------------------------------------------------------//
void Window::setUpdateMode(const WindowUpdateMode mode)
{
    d_updateMode = mode;
}

//----------------------------------------------------------------------------//
WindowUpdateMode Window::getUpdateMode() const
{
    return d_updateMode;
}

//----------------------------------------------------------------------------//
void Window::setMouseInputPropagationEnabled(const bool enabled)
{
    d_propagateMouseInputs = enabled;
}

//----------------------------------------------------------------------------//
bool Window::isMouseInputPropagationEnabled() const
{
    return d_propagateMouseInputs;
}

//----------------------------------------------------------------------------//
Window* Window::clone(const bool deepCopy) const
{
    Window* ret =
        WindowManager::getSingleton().createWindow(getType(), getName());

    // always copy properties
    clonePropertiesTo(*ret);

    // if user requested deep copy, we should copy children as well
    if (deepCopy)
        cloneChildWidgetsTo(*ret);

    return ret;
}

//----------------------------------------------------------------------------//
void Window::clonePropertiesTo(Window& target) const
{
    for (PropertySet::PropertyIterator propertyIt = getPropertyIterator();
         !propertyIt.isAtEnd();
         ++propertyIt) 
    {
        const String& propertyName = propertyIt.getCurrentKey();
        const String propertyValue = getProperty(propertyName);

        // we never copy stuff that doesn't get written into XML
        if (isPropertyBannedFromXML(propertyName))
            continue;

        // some cases when propertyValue is "" could lead to exception throws
        if (propertyValue.empty())
        {
            // special case, this causes exception throw when no window renderer
            // is assigned to the window
            if (propertyName == "LookNFeel")
                continue;

            // special case, this causes exception throw because we are setting
            // 'null' window renderer
            if (propertyName == "WindowRenderer")
                continue;
        }

        target.setProperty(propertyName, propertyValue);
    }
}

//----------------------------------------------------------------------------//
void Window::cloneChildWidgetsTo(Window& target) const
{
    // todo: ChildWindowIterator?
    for (size_t childI = 0; childI < getChildCount(); ++childI)
    {
        Window* child = getChildAtIdx(childI);
        if (child->isAutoWindow())
        {
            // we skip auto windows, they are already created
            // automatically

            // note: some windows store non auto windows inside auto windows,
            //       standard solution is to copy these non-auto windows to
            //       the parent window
            //
            //       If you need alternative behaviour, you have to override
            //       this method!

            // so just copy it's child widgets
            child->cloneChildWidgetsTo(target);
            // and skip the auto widget
            continue;
        }

        Window* newChild = child->clone(true);
        target.addChild(newChild);
    }
}

//----------------------------------------------------------------------------//
size_t Window::getZIndex() const
{
    if (!d_parent)
        return 0;

    ChildDrawList::iterator i = std::find(
        getParent()->d_drawList.begin(),
        getParent()->d_drawList.end(),
        this);

    if (i == getParent()->d_drawList.end())
        CEGUI_THROW(InvalidRequestException(
            "Window is not in its parent's draw list."));

    return std::distance(getParent()->d_drawList.begin(), i);
}

//----------------------------------------------------------------------------//
bool Window::isInFront(const Window& wnd) const
{
    // children are always in front of their ancestors
    if (isAncestor(&wnd))
        return true;

    // conversely, ancestors are always behind their children
    if (wnd.isAncestor(this))
        return false;

    const Window* const w1 = getWindowAttachedToCommonAncestor(wnd);

    // seems not to be in same window hierarchy
    if (!w1)
        return false;

    const Window* const w2 = wnd.getWindowAttachedToCommonAncestor(*this);

    // at this point, w1 and w2 share the same parent.
    return w2->getZIndex() > w1->getZIndex();
}

//----------------------------------------------------------------------------//
const Window* Window::getWindowAttachedToCommonAncestor(const Window& wnd) const
{
    const Window* w = &wnd;
    const Window* tmp = w->getParent();

    while (tmp)
    {
        if (isAncestor(tmp))
            break;

        w = tmp;
        tmp = tmp->getParent();
    }

    return tmp ? w : 0;
}

//----------------------------------------------------------------------------//
bool Window::isBehind(const Window& wnd) const
{
    return !isInFront(wnd);
}

//----------------------------------------------------------------------------//
const Font* Window::property_getFont() const
{
    // This is changed behaviour when compared to 0.7, we return the Font set
    // for this window but we don't return name of the default font when
    // no font is set. This is IMO more practical. User can always use
    // getFont() directly to get 0.7 behaviour.
    
    return getFont(false);
}

//----------------------------------------------------------------------------//
const Image* Window::property_getMouseCursor() const
{
    return getMouseCursor();
}

//----------------------------------------------------------------------------//
GUIContext& Window::getGUIContext() const
{
    // GUIContext is always the one on the root window, we do not allow parts
    // of a hierarchy to be drawn to separate contexts (which is not much of
    // a limitation).
    //
    // ISSUE: if root has no GUIContext set for it, should we return 0 or
    //        System::getDefaultGUIContext?  Come to IRC and argue about it!
    if (getParent() != 0)
    {
        return getParent()->getGUIContext();
    }
    else
    {
        if (d_guiContext)
            return *d_guiContext;
        else
            return System::getSingleton().getDefaultGUIContext();
    }
}

//----------------------------------------------------------------------------//
void Window::setGUIContext(GUIContext* context)
{
    if (d_guiContext == context)
        return;

    d_guiContext = context;
    syncTargetSurface();
}

//----------------------------------------------------------------------------//
const Sizef& Window::getRootContainerSize() const
{
    return getGUIContext().getSurfaceSize();
}

//----------------------------------------------------------------------------//
void Window::setAutoWindow(bool is_auto)
{
    d_autoWindow = is_auto;

    if (d_autoWindow)
        banPropertiesForAutoWindow();
}

//----------------------------------------------------------------------------//
void Window::banPropertiesForAutoWindow()
{
    banPropertyFromXML("AutoWindow"); // :-D
    banPropertyFromXML("DestroyedByParent");
    banPropertyFromXML("VerticalAlignment");
    banPropertyFromXML("HorizontalAlignment");
    banPropertyFromXML("Area");
    banPropertyFromXML("Position");
    banPropertyFromXML("Size");
    banPropertyFromXML("MinSize");
    banPropertyFromXML("MaxSize");
    banPropertyFromXML(&d_windowRendererProperty);
    banPropertyFromXML(&d_lookNFeelProperty);
}

//----------------------------------------------------------------------------//
bool Window::handleFontRenderSizeChange(const EventArgs& args)
{
    if (!d_windowRenderer)
        return false;

    return d_windowRenderer->handleFontRenderSizeChange(
        static_cast<const FontEventArgs&>(args).font);
}

//----------------------------------------------------------------------------//
bool Window::isMouseContainedInArea() const
{
    return d_containsMouse;
}

//----------------------------------------------------------------------------//

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

} // End of  CEGUI namespace section
