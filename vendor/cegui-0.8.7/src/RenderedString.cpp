/***********************************************************************
    created:    25/05/2009
    author:     Paul Turner
 *************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2009 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/RenderedString.h"
#include "CEGUI/RenderedStringComponent.h"
#include "CEGUI/Exceptions.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
RenderedString::RenderedString()
{
    // set up initial line info
    appendLineBreak();
}

//----------------------------------------------------------------------------//
RenderedString::~RenderedString()
{
    clearComponents();
}

//----------------------------------------------------------------------------//
void RenderedString::appendComponent(const RenderedStringComponent& component)
{
    d_components.push_back(component.clone());
    ++d_lines.back().second;
}

//----------------------------------------------------------------------------//
void RenderedString::clearComponents()
{
    clearComponentList(d_components);
    d_lines.clear();
}

//----------------------------------------------------------------------------//
size_t RenderedString::getComponentCount() const
{
    return d_components.size();
}

//----------------------------------------------------------------------------//
RenderedString::RenderedString(const RenderedString& other)
{
    cloneComponentList(other.d_components);
    d_lines = other.d_lines;
}

//----------------------------------------------------------------------------//
RenderedString& RenderedString::operator=(const RenderedString& rhs)
{
    cloneComponentList(rhs.d_components);
    d_lines = rhs.d_lines;
    return *this;
}

//----------------------------------------------------------------------------//
void RenderedString::cloneComponentList(const ComponentList& list)
{
    clearComponentList(d_components);

    for (size_t i = 0; i < list.size(); ++i)
        d_components.push_back(list[i]->clone());    
}

//----------------------------------------------------------------------------//
void RenderedString::clearComponentList(ComponentList& list)
{
    for (size_t i = 0; i < list.size(); ++i)
        CEGUI_DELETE_AO list[i];

    list.clear();
}

//----------------------------------------------------------------------------//
void RenderedString::split(const Window* ref_wnd, const size_t line,
                           float split_point, RenderedString& left)
{
    // FIXME: This function is big and nasty; it breaks all the rules for a
    // 'good' function and desperately needs some refactoring work done to it.
    // On the plus side, it does seem to work though ;)

    if (line >= getLineCount())
        CEGUI_THROW(InvalidRequestException(
            "line number specified is invalid."));

    left.clearComponents();

    if (d_components.empty())
        return;

    // move all components in lines prior to the line being split to the left
    if (line > 0)
    {
        // calculate size of range
        const size_t sz = d_lines[line - 1].first + d_lines[line - 1].second;
        // range start
        ComponentList::iterator cb = d_components.begin();
        // range end (exclusive)
        ComponentList::iterator ce = cb + sz;
        // copy components to left side
        left.d_components.assign(cb, ce);
        // erase components from this side.
        d_components.erase(cb, ce);

        LineList::iterator lb = d_lines.begin();
        LineList::iterator le = lb + line;
        // copy lines to left side
        left.d_lines.assign(lb, le);
        // erase lines from this side
        d_lines.erase(lb, le);
    }

    // find the component where the requested split point lies.
    float partial_extent = 0;

    size_t idx = 0;
    const size_t last_component = d_lines[0].second;
    for (; idx < last_component; ++idx)
    {
        partial_extent += d_components[idx]->getPixelSize(ref_wnd).d_width;

        if (split_point <= partial_extent)
            break;
    }

    // case where split point is past the end
    if (idx >= last_component)
    {
        // transfer this line's components to the 'left' string.
        //
        // calculate size of range
        const size_t sz = d_lines[0].second;
        // range start
        ComponentList::iterator cb = d_components.begin();
        // range end (exclusive)
        ComponentList::iterator ce = cb + sz;
        // copy components to left side
        left.d_components.insert(left.d_components.end(), cb, ce);
        // erase components from this side.
        d_components.erase(cb, ce);

        // copy line info to left side
        left.d_lines.push_back(d_lines[0]);
        // erase line from this side
        d_lines.erase(d_lines.begin());

        // fix up lines in this object
        for (size_t comp = 0, i = 0; i < d_lines.size(); ++i)
        {
            d_lines[i].first = comp;
            comp += d_lines[i].second;
        }

        return;
    }

    left.appendLineBreak();
    const size_t left_line = left.getLineCount() - 1;
    // Everything up to 'idx' is xfered to 'left'
    for (size_t i = 0; i < idx; ++i)
    {
        left.d_components.push_back(d_components[0]);
        d_components.erase(d_components.begin());
        ++left.d_lines[left_line].second;
        --d_lines[0].second;
    }

    // now to split item 'idx' putting half in left and leaving half in this.
    RenderedStringComponent* c = d_components[0];
    if (c->canSplit())
    {
        RenderedStringComponent* lc = 
            c->split(ref_wnd,
                     split_point - (partial_extent - c->getPixelSize(ref_wnd).d_width),
                     idx == 0);

        if (lc)
        {
            left.d_components.push_back(lc);
            ++left.d_lines[left_line].second;
        }
    }
    // can't split, if component width is >= split_point xfer the whole
    // component to it's own line in the left part (FIX #306)
    else if (c->getPixelSize(ref_wnd).d_width >= split_point)
    {
        left.appendLineBreak();
        left.d_components.push_back(d_components[0]);
        d_components.erase(d_components.begin());
        ++left.d_lines[left_line + 1].second;
        --d_lines[0].second;
    }

    // fix up lines in this object
    for (size_t comp = 0, i = 0; i < d_lines.size(); ++i)
    {
        d_lines[i].first = comp;
        comp += d_lines[i].second;
    }
}

//----------------------------------------------------------------------------//
void RenderedString::appendLineBreak()
{
    const size_t first_component = d_lines.empty() ? 0 :
        d_lines.back().first + d_lines.back().second;

    d_lines.push_back(LineInfo(first_component, 0));
}

//----------------------------------------------------------------------------//
size_t RenderedString::getLineCount() const
{
    return d_lines.size();
}

//----------------------------------------------------------------------------//
Sizef RenderedString::getPixelSize(const Window* ref_wnd,
                                   const size_t line) const
{
    if (line >= getLineCount())
        CEGUI_THROW(InvalidRequestException(
            "line number specified is invalid."));

    Sizef sz(0, 0);

    const size_t end_component = d_lines[line].first + d_lines[line].second;
    for (size_t i = d_lines[line].first; i < end_component; ++i)
    {
        const Sizef comp_sz(d_components[i]->getPixelSize(ref_wnd));
        sz.d_width += comp_sz.d_width;

        if (comp_sz.d_height > sz.d_height)
            sz.d_height = comp_sz.d_height;
    }

    return sz;
}

//----------------------------------------------------------------------------//
size_t RenderedString::getSpaceCount(const size_t line) const
{
    if (line >= getLineCount())
        CEGUI_THROW(InvalidRequestException(
            "line number specified is invalid."));

    size_t space_count = 0;

    const size_t end_component = d_lines[line].first + d_lines[line].second;
    for (size_t i = d_lines[line].first; i < end_component; ++i)
        space_count += d_components[i]->getSpaceCount();

    return space_count;
}

//----------------------------------------------------------------------------//
void RenderedString::draw(const Window* ref_wnd, const size_t line,
                          GeometryBuffer& buffer, const Vector2f& position,
                          const ColourRect* mod_colours, const Rectf* clip_rect,
                          const float space_extra) const
{
    if (line >= getLineCount())
        CEGUI_THROW(InvalidRequestException(
            "line number specified is invalid."));

    const float render_height = getPixelSize(ref_wnd, line).d_height;

    Vector2f comp_pos(position);

    const size_t end_component = d_lines[line].first + d_lines[line].second;
    for (size_t i = d_lines[line].first; i < end_component; ++i)
    {
        d_components[i]->draw(ref_wnd, buffer, comp_pos, mod_colours, clip_rect,
                              render_height, space_extra);
        comp_pos.d_x += d_components[i]->getPixelSize(ref_wnd).d_width;
    }
}

//----------------------------------------------------------------------------//
void RenderedString::setSelection(const Window* ref_wnd, float start, float end)
{
    const size_t last_component = d_lines[0].second;
    float partial_extent = 0;
    size_t idx = 0;

    // clear last selection from all components
    for (size_t i = 0; i < d_components.size(); i++)
        d_components[i]->setSelection(ref_wnd, 0, 0);

    for (; idx < last_component; ++idx)
    {
        if (start <= partial_extent + d_components[idx]->getPixelSize(ref_wnd).d_width)
            break;
         partial_extent += d_components[idx]->getPixelSize(ref_wnd).d_width;
    }

    start -= partial_extent;
    end -= partial_extent;

    while (end > 0.0f)
    {
        const float comp_extent = d_components[idx]->getPixelSize(ref_wnd).d_width;
        d_components[idx]->setSelection(ref_wnd,
                                        start,
                                        (end >= comp_extent) ? comp_extent : end);
        start = 0;
        end -= comp_extent;
        ++idx;
    }
}

//----------------------------------------------------------------------------//
float RenderedString::getHorizontalExtent(const Window* ref_wnd) const
{
    float w = 0.0f;
    for (size_t i = 0; i < d_lines.size(); ++i)
    {
        const float this_width = getPixelSize(ref_wnd, i).d_width;
        if (this_width > w)
            w = this_width;
    }

    return w;
}

//----------------------------------------------------------------------------//
float RenderedString::getVerticalExtent(const Window* ref_wnd) const
{
    float h = 0.0f;
    for (size_t i = 0; i < d_lines.size(); ++i)
        h += getPixelSize(ref_wnd, i).d_height;

    return h;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
