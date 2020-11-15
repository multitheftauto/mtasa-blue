/***********************************************************************
    created:    Sun Jun 26 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "CEGUI/falagard/NamedArea.h"
#include "CEGUI/falagard/XMLHandler.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    NamedArea::NamedArea(const String& name) :
        d_name(name)
    {
    }

    const String& NamedArea::getName() const
    {
        return d_name;
    }

    void NamedArea::setName(const String& name)
    {
        d_name = name;
    }

    const ComponentArea& NamedArea::getArea() const
    {
        return d_area;
    }

    void NamedArea::setArea(const ComponentArea& area)
    {
        d_area = area;
    }

    void NamedArea::writeXMLToStream(XMLSerializer& xml_stream) const
    {
        xml_stream.openTag(Falagard_xmlHandler::NamedAreaElement)
            .attribute(Falagard_xmlHandler::NameAttribute, d_name);
        d_area.writeXMLToStream(xml_stream);
        xml_stream.closeTag();
    }

    bool NamedArea::handleFontRenderSizeChange(Window& window,
                                               const Font* font) const
    {
        return d_area.handleFontRenderSizeChange(window, font);
    }

} // End of  CEGUI namespace section
