/***********************************************************************
    created:    18/8/2011
    author:     Martin Preisler

    purpose:    Implements the Element class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
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
#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include "CEGUI/Element.h"
#include "CEGUI/CoordConverter.h"
#include "CEGUI/System.h"
#include "CEGUI/Logger.h"

#include <algorithm>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4355)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

const String Element::EventNamespace("Element");

const String Element::EventSized("Sized");
const String Element::EventParentSized("ParentSized");
const String Element::EventMoved("Moved");
const String Element::EventHorizontalAlignmentChanged("HorizontalAlignmentChanged");
const String Element::EventVerticalAlignmentChanged("VerticalAlignmentChanged");
const String Element::EventRotated("Rotated");
const String Element::EventChildAdded("ChildAdded");
const String Element::EventChildRemoved("ChildRemoved");
const String Element::EventZOrderChanged("ZOrderChanged");
const String Element::EventNonClientChanged("NonClientChanged");

//----------------------------------------------------------------------------//
Element::Element():
    d_parent(0),

    d_nonClient(false),

    d_area(cegui_reldim(0), cegui_reldim(0), cegui_reldim(0), cegui_reldim(0)),
    d_horizontalAlignment(HA_LEFT),
    d_verticalAlignment(VA_TOP),
    d_minSize(cegui_reldim(0), cegui_reldim(0)),
    d_maxSize(cegui_reldim(0), cegui_reldim(0)),
    d_aspectMode(AM_IGNORE),
    d_aspectRatio(1.0 / 1.0),
    d_pixelAligned(true),
    d_pixelSize(0.0f, 0.0f),
    d_rotation(Quaternion::IDENTITY),

    d_unclippedOuterRect(this, &Element::getUnclippedOuterRect_impl),
    d_unclippedInnerRect(this, &Element::getUnclippedInnerRect_impl)
{
    addElementProperties();
}

//----------------------------------------------------------------------------//
Element::~Element()
{}

//----------------------------------------------------------------------------//
Element::Element(const Element& other):
    EventSet(other),
    d_unclippedOuterRect(this, &Element::getUnclippedOuterRect_impl),
    d_unclippedInnerRect(this, &Element::getUnclippedInnerRect_impl)
{}

//----------------------------------------------------------------------------//
void Element::setArea(const UVector2& pos, const USize& size)
{
    setArea_impl(pos, size);
}

//----------------------------------------------------------------------------//
void Element::notifyScreenAreaChanged(bool recursive /* = true */)
{
    d_unclippedOuterRect.invalidateCache();
    d_unclippedInnerRect.invalidateCache();

    // inform children that their screen area must be updated
    if (recursive)
    {
        const size_t child_count = getChildCount();
        for (size_t i = 0; i < child_count; ++i)
            d_children[i]->notifyScreenAreaChanged();
    }
}

//----------------------------------------------------------------------------//
void Element::setHorizontalAlignment(const HorizontalAlignment alignment)
{
    if (d_horizontalAlignment == alignment)
        return;

    d_horizontalAlignment = alignment;

    ElementEventArgs args(this);
    onHorizontalAlignmentChanged(args);
}

//----------------------------------------------------------------------------//
void Element::setVerticalAlignment(const VerticalAlignment alignment)
{
    if (d_verticalAlignment == alignment)
        return;

    d_verticalAlignment = alignment;

    ElementEventArgs args(this);
    onVerticalAlignmentChanged(args);
}

//----------------------------------------------------------------------------//
void Element::setMinSize(const USize& size)
{
    d_minSize = size;

    // TODO: Perhaps we could be more selective and skip this if min size won't
    //       affect the size
    setSize(getSize());
}

//----------------------------------------------------------------------------//
void Element::setMaxSize(const USize& size)
{
    d_maxSize = size;

    // TODO: Perhaps we could be more selective and skip this if min size won't
    //       affect the size
    setSize(getSize());
}

//----------------------------------------------------------------------------//
void Element::setAspectMode(AspectMode mode)
{
    if (d_aspectMode == mode)
        return;

    d_aspectMode = mode;

    // TODO: We want an Event and more smart rect update handling

    // Ensure the area is calculated with the new aspect mode
    // TODO: This potentially wastes effort, we should just mark it as dirty
    //       and postpone the calculation for as long as possible
    setArea(getArea());
}

//----------------------------------------------------------------------------//
void Element::setAspectRatio(float ratio)
{
    if (d_aspectRatio == ratio)
        return;

    d_aspectRatio = ratio;

    // TODO: We want an Event and more smart rect update handling

    // Ensure the area is calculated with the new aspect ratio
    // TODO: This potentially wastes effort, we should just mark it as dirty
    //       and postpone the calculation for as long as possible
    setArea(getArea());
}

//----------------------------------------------------------------------------//
void Element::setPixelAligned(const bool setting)
{
    if (d_pixelAligned == setting)
        return;

    d_pixelAligned = setting;

    // TODO: We want an Event and more smart rect update handling

    // Ensure the area is calculated with the new pixel aligned setting
    // TODO: This potentially wastes effort, we should just mark it as dirty
    //       and postpone the calculation for as long as possible
    setArea(getArea());
}

//----------------------------------------------------------------------------//
Sizef Element::calculatePixelSize(bool skipAllPixelAlignment) const
{
    // calculate pixel sizes for everything, so we have a common format for
    // comparisons.
    Sizef absMin(CoordConverter::asAbsolute(d_minSize,
        getRootContainerSize(), false));
    Sizef absMax(CoordConverter::asAbsolute(d_maxSize,
        getRootContainerSize(), false));

    Sizef base_size;
    if (skipAllPixelAlignment)
    {
        base_size = Sizef((d_parent && !d_nonClient) ?
                           d_parent->getUnclippedInnerRect().getFresh(true).getSize() :
                           getParentPixelSize(true));
    }
    else
    {
        base_size = Sizef((d_parent && !d_nonClient) ?
                           d_parent->getUnclippedInnerRect().get().getSize() :
                           getParentPixelSize());
    }

    Sizef ret = CoordConverter::asAbsolute(d_area.getSize(), base_size, false);

    // in case absMin components are larger than absMax ones,
    // max size takes precedence
    if (absMax.d_width != 0.0f && absMin.d_width > absMax.d_width)
    {
        absMin.d_width = absMax.d_width;
        CEGUI_LOGINSANE("MinSize resulted in an absolute pixel size with "
                        "width larger than what MaxSize resulted in");
    }

    if (absMax.d_height != 0.0f && absMin.d_height > absMax.d_height)
    {
        absMin.d_height = absMax.d_height;
        CEGUI_LOGINSANE("MinSize resulted in an absolute pixel size with "
                        "height larger than what MaxSize resulted in");
    }

    // limit new pixel size to: minSize <= newSize <= maxSize
    if (ret.d_width < absMin.d_width)
        ret.d_width = absMin.d_width;
    else if (absMax.d_width != 0.0f && ret.d_width > absMax.d_width)
        ret.d_width = absMax.d_width;

    if (ret.d_height < absMin.d_height)
        ret.d_height = absMin.d_height;
    else if (absMax.d_height != 0.0f && ret.d_height > absMax.d_height)
        ret.d_height = absMax.d_height;

    if (d_aspectMode != AM_IGNORE)
    {
        // make sure we respect current aspect mode and ratio
        ret.scaleToAspect(d_aspectMode, d_aspectRatio);

        // make sure we haven't blown any of the hard limits
        // still maintain the aspect when we do this
        if (d_aspectMode == AM_SHRINK)
        {
            float ratio = 1.0f;
            // check that we haven't blown the min size
            if (ret.d_width < absMin.d_width)
            {
                ratio = absMin.d_width / ret.d_width;
            }
            if (ret.d_height < absMin.d_height)
            {
                const float newRatio = absMin.d_height / ret.d_height;
                if (newRatio > ratio)
                    ratio = newRatio;
            }

            ret.d_width *= ratio;
            ret.d_height *= ratio;
        }
        else if (d_aspectMode == AM_EXPAND)
        {
            float ratio = 1.0f;
            // check that we haven't blown the min size
            if (absMax.d_width != 0.0f && ret.d_width > absMax.d_width)
            {
                ratio = absMax.d_width / ret.d_width;
            }
            if (absMax.d_height != 0.0f && ret.d_height > absMax.d_height)
            {
                const float newRatio = absMax.d_height / ret.d_height;
                if (newRatio > ratio)
                    ratio = newRatio;
            }

            ret.d_width *= ratio;
            ret.d_height *= ratio;
        }
        // NOTE: When the hard min max limits are unsatisfiable with the aspect lock mode,
        //       the result won't be limited by both limits!
    }

    if (d_pixelAligned)
    {
        ret.d_width = CoordConverter::alignToPixels(ret.d_width);
        ret.d_height = CoordConverter::alignToPixels(ret.d_height);
    }

    return ret;
}

//----------------------------------------------------------------------------//
Sizef Element::getParentPixelSize(bool skipAllPixelAlignment) const
{
    if (d_parent)
    {
        return skipAllPixelAlignment ?
            d_parent->calculatePixelSize(true) : d_parent->getPixelSize();
    }
    else
    {
        return getRootContainerSize();
    }
}

//----------------------------------------------------------------------------//
const Sizef& Element::getRootContainerSize() const
{
    return System::getSingleton().getRenderer()->getDisplaySize();
}

//----------------------------------------------------------------------------//
void Element::setRotation(const Quaternion& rotation)
{
    d_rotation = rotation;

    ElementEventArgs args(this);
    onRotated(args);
}

//----------------------------------------------------------------------------//
void Element::addChild(Element* element)
{
    if (!element)
        CEGUI_THROW(
                InvalidRequestException("Can't add NULL to Element as a child!"));

    if (element == this)
        CEGUI_THROW(
                InvalidRequestException("Can't make element its own child - "
                                        "this->addChild(this); is forbidden."));

    addChild_impl(element);
    ElementEventArgs args(element);
    onChildAdded(args);
}

//----------------------------------------------------------------------------//
void Element::removeChild(Element* element)
{
    if (!element)
        CEGUI_THROW(
                InvalidRequestException("NULL can't be a child of any Element, "
                                        "it makes little sense to ask for its "
                                        "removal"));

    removeChild_impl(element);
    ElementEventArgs args(element);
    onChildRemoved(args);
}

//----------------------------------------------------------------------------//
bool Element::isChild(const Element* element) const
{
    return std::find(d_children.begin(), d_children.end(), element) != d_children.end();
}

//----------------------------------------------------------------------------//
bool Element::isAncestor(const Element* element) const
{
    if (!d_parent)
    {
        // no parent, no ancestor, nothing can be our ancestor
        return false;
    }

    return d_parent == element || d_parent->isAncestor(element);
}

//----------------------------------------------------------------------------//
void Element::setNonClient(const bool setting)
{
    if (setting == d_nonClient)
    {
        return;
    }

    d_nonClient = setting;

    ElementEventArgs args(this);
    onNonClientChanged(args);
}

//----------------------------------------------------------------------------//
const Element::CachedRectf& Element::getClientChildContentArea() const
{
    return getUnclippedInnerRect();
}

//----------------------------------------------------------------------------//
const Element::CachedRectf& Element::getNonClientChildContentArea() const
{
    return getUnclippedOuterRect();
}

//----------------------------------------------------------------------------//
void Element::addElementProperties()
{
    const String propertyOrigin("Element");

    CEGUI_DEFINE_PROPERTY(Element, URect,
        "Area", "Property to get/set the unified area rectangle. Value is a \"URect\".",
        &Element::setArea, &Element::getArea, URect(UDim(0, 0), UDim(0, 0), UDim(0, 0), UDim(0, 0))
    );

    CEGUI_DEFINE_PROPERTY_NO_XML(Element, UVector2,
        "Position", "Property to get/set the unified position. Value is a \"UVector2\".",
        &Element::setPosition, &Element::getPosition, UVector2(UDim(0, 0), UDim(0, 0))
    );

    CEGUI_DEFINE_PROPERTY(Element, VerticalAlignment,
        "VerticalAlignment", "Property to get/set the vertical alignment.  Value is one of \"Top\", \"Centre\" or \"Bottom\".",
        &Element::setVerticalAlignment, &Element::getVerticalAlignment, VA_TOP
    );

    CEGUI_DEFINE_PROPERTY(Element, HorizontalAlignment,
        "HorizontalAlignment", "Property to get/set the horizontal alignment.  Value is one of \"Left\", \"Centre\" or \"Right\".",
        &Element::setHorizontalAlignment, &Element::getHorizontalAlignment, HA_LEFT
    );

    CEGUI_DEFINE_PROPERTY_NO_XML(Element, USize,
        "Size", "Property to get/set the unified size. Value is a \"USize\".",
        &Element::setSize, &Element::getSize, USize(UDim(0, 0), UDim(0, 0))
    );

    CEGUI_DEFINE_PROPERTY(Element, USize,
        "MinSize", "Property to get/set the unified minimum size. Value is a \"USize\".",
        &Element::setMinSize, &Element::getMinSize, USize(UDim(0, 0), UDim(0, 0))
    );

    CEGUI_DEFINE_PROPERTY(Element, USize, "MaxSize",
        "Property to get/set the unified maximum size. Value is a \"USize\". "
        "Note that zero means no maximum size.",
        &Element::setMaxSize, &Element::getMaxSize, USize(UDim(0, 0), UDim(0, 0))
    );

    CEGUI_DEFINE_PROPERTY(Element, AspectMode,
        "AspectMode", "Property to get/set the 'aspect mode' setting. Value is either \"Ignore\", \"Shrink\" or \"Expand\".",
        &Element::setAspectMode, &Element::getAspectMode, AM_IGNORE
    );

    CEGUI_DEFINE_PROPERTY(Element, float,
        "AspectRatio", "Property to get/set the aspect ratio. Only applies when aspect mode is not \"Ignore\".",
        &Element::setAspectRatio, &Element::getAspectRatio, 1.0 / 1.0
    );

    CEGUI_DEFINE_PROPERTY(Element, bool,
        "PixelAligned", "Property to get/set whether the Element's size and position should be pixel aligned. "
        "Value is either \"true\" or \"false\".",
        &Element::setPixelAligned, &Element::isPixelAligned, true
    );

    CEGUI_DEFINE_PROPERTY(Element, Quaternion,
        "Rotation", "Property to get/set the Element's rotation. Value is a quaternion: "
        "\"w:[w_float] x:[x_float] y:[y_float] z:[z_float]\""
        "or \"x:[x_float] y:[y_float] z:[z_float]\" to convert from Euler angles (in degrees).",
        &Element::setRotation, &Element::getRotation, Quaternion(1.0,0.0,0.0,0.0)
    );

    CEGUI_DEFINE_PROPERTY(Element, bool,
        "NonClient", "Property to get/set whether the Element is 'non-client'. "
        "Value is either \"true\" or \"false\".",
        &Element::setNonClient, &Element::isNonClient, false
    );
}

//----------------------------------------------------------------------------//
void Element::setArea_impl(const UVector2& pos, const USize& size,
                        bool topLeftSizing, bool fireEvents)
{
    // we make sure the screen areas are recached when this is called as we need
    // it in most cases
    d_unclippedOuterRect.invalidateCache();
    d_unclippedInnerRect.invalidateCache();

    // save original size so we can work out how to behave later on
    const Sizef oldSize(d_pixelSize);

    d_area.setSize(size);
    d_pixelSize = calculatePixelSize();

    // have we resized the element?
    const bool sized = (d_pixelSize != oldSize);

    // If this is a top/left edge sizing op, only modify position if the size
    // actually changed.  If it is not a sizing op, then position may always
    // change.
    const bool moved = (!topLeftSizing || sized) && pos != d_area.d_min;

    if (moved)
        d_area.setPosition(pos);

    if (fireEvents)
        fireAreaChangeEvents(moved, sized);
}

//----------------------------------------------------------------------------//
void Element::fireAreaChangeEvents(const bool moved, const bool sized)
{
    if (moved)
    {
        ElementEventArgs args(this);
        onMoved(args);
    }

    if (sized)
    {
        ElementEventArgs args(this);
        onSized(args);
    }
}

//----------------------------------------------------------------------------//
void Element::setParent(Element* parent)
{
    d_parent = parent;
}

//----------------------------------------------------------------------------//
void Element::addChild_impl(Element* element)
{
    // if element is attached elsewhere, detach it first (will fire normal events)
    Element* const old_parent = element->getParentElement();
    if (old_parent)
        old_parent->removeChild(element);

    // add element to child list
    d_children.push_back(element);

    // set the parent element
    element->setParent(this);

    // update area rects and content for the added element
    element->notifyScreenAreaChanged(true);

    // correctly call parent sized notification if needed.
    if (!old_parent || old_parent->getPixelSize() != getPixelSize())
    {
        ElementEventArgs args(this);
        element->onParentSized(args);
    }
}

//----------------------------------------------------------------------------//
void Element::removeChild_impl(Element* element)
{
    // find this element in the child list
    ChildList::iterator it = std::find(d_children.begin(), d_children.end(), element);

    // if the element was found in the child list
    if (it != d_children.end())
    {
        // remove element from child list
        d_children.erase(it);
        // reset element's parent so it's no longer this element.
        element->setParent(0);
    }
}

//----------------------------------------------------------------------------//
Rectf Element::getUnclippedOuterRect_impl(bool skipAllPixelAlignment) const
{
    const Sizef pixel_size = skipAllPixelAlignment ?
        calculatePixelSize(true) : getPixelSize();
    Rectf ret(Vector2f(0, 0), pixel_size);

    const Element* parent = getParentElement();

    Rectf parent_rect;
    if (parent)
    {
        const CachedRectf& base = parent->getChildContentArea(isNonClient());
        parent_rect = skipAllPixelAlignment ? base.getFresh(true) : base.get();
    }
    else
    {
        parent_rect = Rectf(Vector2f(0, 0), getRootContainerSize());
    }

    const Sizef parent_size = parent_rect.getSize();

    Vector2f offset = parent_rect.d_min + CoordConverter::asAbsolute(getArea().d_min, parent_size, false);

    switch (getHorizontalAlignment())
    {
        case HA_CENTRE:
            offset.d_x += (parent_size.d_width - pixel_size.d_width) * 0.5f;
            break;
        case HA_RIGHT:
            offset.d_x += parent_size.d_width - pixel_size.d_width;
            break;
        default:
            break;
    }

    switch (getVerticalAlignment())
    {
        case VA_CENTRE:
            offset.d_y += (parent_size.d_height - pixel_size.d_height) * 0.5f;
            break;
        case VA_BOTTOM:
            offset.d_y += parent_size.d_height - pixel_size.d_height;
            break;
        default:
            break;
    }

    if (d_pixelAligned && !skipAllPixelAlignment)
    {
        offset = Vector2f(CoordConverter::alignToPixels(offset.d_x),
                          CoordConverter::alignToPixels(offset.d_y));
    }

    ret.offset(offset);
    return ret;
}

//----------------------------------------------------------------------------//
Rectf Element::getUnclippedInnerRect_impl(bool skipAllPixelAlignment) const
{
    return skipAllPixelAlignment ? getUnclippedOuterRect().getFresh(true) : getUnclippedOuterRect().get();
}

//----------------------------------------------------------------------------//
void Element::onSized(ElementEventArgs& e)
{
    notifyScreenAreaChanged(false);
    notifyChildrenOfSizeChange(true, true);

    fireEvent(EventSized, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Element::notifyChildrenOfSizeChange(const bool non_client,
                                         const bool client)
{
    const size_t child_count = getChildCount();
    for (size_t i = 0; i < child_count; ++i)
    {
        Element * const child = d_children[i];

        if ((non_client && child->isNonClient()) ||
            (client && !child->isNonClient()))
        {
            ElementEventArgs args(this);
            d_children[i]->onParentSized(args);
        }
    }
}

//----------------------------------------------------------------------------//
void Element::onParentSized(ElementEventArgs& e)
{
    d_unclippedOuterRect.invalidateCache();
    d_unclippedInnerRect.invalidateCache();

    const Sizef oldSize(d_pixelSize);
    d_pixelSize = calculatePixelSize();
    const bool sized = (d_pixelSize != oldSize) || isInnerRectSizeChanged();

    const bool moved =
        ((d_area.d_min.d_x.d_scale != 0) || (d_area.d_min.d_y.d_scale != 0) ||
         (d_horizontalAlignment != HA_LEFT) || (d_verticalAlignment != VA_TOP));

    fireAreaChangeEvents(moved, sized);

    fireEvent(EventParentSized, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Element::onMoved(ElementEventArgs& e)
{
    notifyScreenAreaChanged();

    fireEvent(EventMoved, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Element::onHorizontalAlignmentChanged(ElementEventArgs& e)
{
    notifyScreenAreaChanged();

    fireEvent(EventHorizontalAlignmentChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Element::onVerticalAlignmentChanged(ElementEventArgs& e)
{
    notifyScreenAreaChanged();

    fireEvent(EventVerticalAlignmentChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Element::onRotated(ElementEventArgs& e)
{
    fireEvent(EventRotated, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Element::onChildAdded(ElementEventArgs& e)
{
    fireEvent(EventChildAdded, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Element::onChildRemoved(ElementEventArgs& e)
{
    fireEvent(EventChildRemoved, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void Element::onNonClientChanged(ElementEventArgs& e)
{
    // TODO: Be less wasteful with this update
    setArea(getArea());

    fireEvent(EventNonClientChanged, e, EventNamespace);
}

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

} // End of CEGUI namespace section
