/***********************************************************************
    created:    Mon Jul 18 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "CEGUI/falagard/ComponentBase.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/Colour.h"
#include <iostream>

namespace CEGUI
{
//----------------------------------------------------------------------------//
FalagardComponentBase::FalagardComponentBase() :
    d_colours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)
{}

//----------------------------------------------------------------------------//
FalagardComponentBase::~FalagardComponentBase()
{}

//----------------------------------------------------------------------------//
void FalagardComponentBase::render(Window& srcWindow,
                                   const CEGUI::ColourRect* modColours,
                                   const Rectf* clipper,
                                   bool clipToDisplay) const
{
    Rectf dest_rect(d_area.getPixelRect(srcWindow));

    if (!clipper)
        clipper = &dest_rect;

    const Rectf final_clip_rect(dest_rect.getIntersection(*clipper));
    render_impl(srcWindow, dest_rect, modColours,
                &final_clip_rect, clipToDisplay);
}

//----------------------------------------------------------------------------//
void FalagardComponentBase::render(Window& srcWindow,
                                   const Rectf& baseRect,
                                   const CEGUI::ColourRect* modColours,
                                   const Rectf* clipper,
                                   bool clipToDisplay) const
{
    Rectf dest_rect(d_area.getPixelRect(srcWindow, baseRect));

    if (!clipper)
        clipper = &dest_rect;

    const Rectf final_clip_rect(dest_rect.getIntersection(*clipper));
    render_impl(srcWindow, dest_rect, modColours,
                &final_clip_rect, clipToDisplay);
}

//----------------------------------------------------------------------------//
const ComponentArea& FalagardComponentBase::getComponentArea() const
{
    return d_area;
}

//----------------------------------------------------------------------------//
void FalagardComponentBase::setComponentArea(const ComponentArea& area)
{
    d_area = area;
}

//----------------------------------------------------------------------------//
const ColourRect& FalagardComponentBase::getColours() const
{
    return d_colours;
}

//----------------------------------------------------------------------------//
void FalagardComponentBase::setColours(const ColourRect& cols)
{
    d_colours = cols;
}

//----------------------------------------------------------------------------//
const String& FalagardComponentBase::getColoursPropertySource() const
{
    return d_colourPropertyName;
}

//----------------------------------------------------------------------------//
void FalagardComponentBase::setColoursPropertySource(const String& property)
{
    d_colourPropertyName = property;
}

//----------------------------------------------------------------------------//
void FalagardComponentBase::initColoursRect(const Window& wnd,
                                            const ColourRect* modCols,
                                            ColourRect& cr) const
{
    // if colours come via a colour property
    if (!d_colourPropertyName.empty())
    {
        // if property accesses a ColourRect or a colour
        cr = wnd.getProperty<ColourRect>(d_colourPropertyName);
    }
    // use explicit ColourRect.
    else
        cr = d_colours;

    if (modCols)
        cr *= *modCols;
}

//----------------------------------------------------------------------------//
bool FalagardComponentBase::writeColoursXML(XMLSerializer& xml_stream) const
{

    if (!d_colourPropertyName.empty())
    {
        xml_stream.openTag(Falagard_xmlHandler::ColourRectPropertyElement);
        xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_colourPropertyName)
            .closeTag();
    }
    else if (!d_colours.isMonochromatic() ||
              d_colours.d_top_left != Colour(1,1,1,1))
    {
        xml_stream.openTag(Falagard_xmlHandler::ColoursElement)
            .attribute(Falagard_xmlHandler::TopLeftAttribute,
                PropertyHelper<Colour>::toString(d_colours.d_top_left))
            .attribute(Falagard_xmlHandler::TopRightAttribute,
                PropertyHelper<Colour>::toString(d_colours.d_top_right))
            .attribute(Falagard_xmlHandler::BottomLeftAttribute,
                PropertyHelper<Colour>::toString(d_colours.d_bottom_left))
            .attribute(Falagard_xmlHandler::BottomRightAttribute,
                PropertyHelper<Colour>::toString(d_colours.d_bottom_right))
            .closeTag();
    }
    else
        return false;

    return true;
}

//----------------------------------------------------------------------------//
bool FalagardComponentBase::handleFontRenderSizeChange(Window& window,
                                                       const Font* font) const
{
    return d_area.handleFontRenderSizeChange(window, font);
}

//----------------------------------------------------------------------------//

}

