/***********************************************************************
    created:    Jan 11 2006
    author:     Tomas Lindquist Olsen

    purpose:    Implements interface for the WindowRenderer base class
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
#include "CEGUI/WindowRenderer.h"
#include "CEGUI/falagard/WidgetLookManager.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/************************************************************************
    Constructor
*************************************************************************/
WindowRenderer::WindowRenderer(const String& name, const String& class_name) :
    d_window(0),
    d_name(name),
    d_class(class_name)
{
}

/************************************************************************
    Destructor
*************************************************************************/
WindowRenderer::~WindowRenderer()
{
}

/************************************************************************
    Get the Look'N'Feel assigned to our window
*************************************************************************/
const WidgetLookFeel& WindowRenderer::getLookNFeel() const
{
    return WidgetLookManager::getSingleton().getWidgetLook(d_window->getLookNFeel());
}

/************************************************************************
    Get unclipped inner rectangle.
*************************************************************************/
Rectf WindowRenderer::getUnclippedInnerRect() const
{
    const WidgetLookFeel& lf(getLookNFeel());

    if (lf.isNamedAreaDefined("inner_rect"))
        return lf.getNamedArea("inner_rect").getArea().
            getPixelRect(*d_window, d_window->getUnclippedOuterRect().get());
    else
        return d_window->getUnclippedOuterRect().get();
}

/************************************************************************
    Register property with window renderer
*************************************************************************/
void WindowRenderer::registerProperty(Property* property,
                                      const bool ban_from_xml)
{
    d_properties.push_back(std::make_pair(property, ban_from_xml));
}

//----------------------------------------------------------------------------//
void WindowRenderer::registerProperty(Property* property)
{
    registerProperty(property, false);
}

/************************************************************************
    On attached to window
*************************************************************************/
void WindowRenderer::onAttach()
{
    PropertyList::iterator i = d_properties.begin();
    while (i != d_properties.end())
    {
        d_window->addProperty((*i).first);
        // ban from xml if neccessary
        if ((*i).second)
            d_window->banPropertyFromXML((*i).first);

        ++i;
    }
}

/************************************************************************
    On detached from window
*************************************************************************/
void WindowRenderer::onDetach()
{
    PropertyList::reverse_iterator i = d_properties.rbegin();
    while (i != d_properties.rend())
    {
        // unban from xml if neccessary
        if ((*i).second)
            d_window->unbanPropertyFromXML((*i).first);

        d_window->removeProperty((*i).first->getName());
        ++i;
    }
}

//----------------------------------------------------------------------------//
void WindowRenderer::getRenderingContext(RenderingContext& ctx) const
{
    // default just calls back to the window implementation version.
    d_window->getRenderingContext_impl(ctx);
}

//----------------------------------------------------------------------------//
bool WindowRenderer::handleFontRenderSizeChange(const Font* const font)
{
    const WidgetLookFeel& lf(getLookNFeel());
    return lf.handleFontRenderSizeChange(*d_window, font);
}

//----------------------------------------------------------------------------//

} // End of CEGUI namespace
