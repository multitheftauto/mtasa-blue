/***********************************************************************
    created:    Fri Jun 15 2012
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
#include "CEGUI/falagard/FormattingSetting.h"
#include "CEGUI/falagard/XMLHandler.h"

namespace CEGUI
{
//----------------------------------------------------------------------------//
template<>
void FormattingSetting<VerticalFormatting>::writeXMLTagToStream(XMLSerializer& xml_stream) const
{
    if (d_propertySource.empty())
        xml_stream.openTag(Falagard_xmlHandler::VertFormatElement);
    else
        xml_stream.openTag(Falagard_xmlHandler::VertFormatPropertyElement);
}

//----------------------------------------------------------------------------//
template<>
void FormattingSetting<VerticalFormatting>::writeXMLAttributesToStream(XMLSerializer& xml_stream) const
{
    if (d_propertySource.empty())
        xml_stream.attribute(Falagard_xmlHandler::TypeAttribute,
            FalagardXMLHelper<VerticalFormatting>::toString(d_value));
    else
        xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_propertySource);
}

//----------------------------------------------------------------------------//
template<>
void FormattingSetting<HorizontalFormatting>::writeXMLTagToStream(XMLSerializer& xml_stream) const
{
    if (d_propertySource.empty())
        xml_stream.openTag(Falagard_xmlHandler::HorzFormatElement);
    else
        xml_stream.openTag(Falagard_xmlHandler::HorzFormatPropertyElement);
}

//----------------------------------------------------------------------------//
template<>
void FormattingSetting<HorizontalFormatting>::writeXMLAttributesToStream(XMLSerializer& xml_stream) const
{
    if (d_propertySource.empty())
        xml_stream.attribute(Falagard_xmlHandler::TypeAttribute,
            FalagardXMLHelper<HorizontalFormatting>::toString(d_value));
    else
        xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_propertySource);
}

//----------------------------------------------------------------------------//
template<>
void FormattingSetting<VerticalTextFormatting>::writeXMLTagToStream(XMLSerializer& xml_stream) const
{
    if (d_propertySource.empty())
        xml_stream.openTag(Falagard_xmlHandler::VertFormatElement);
    else
        xml_stream.openTag(Falagard_xmlHandler::VertFormatPropertyElement);
}

//----------------------------------------------------------------------------//
template<>
void FormattingSetting<VerticalTextFormatting>::writeXMLAttributesToStream(XMLSerializer& xml_stream) const
{
    if (d_propertySource.empty())
        xml_stream.attribute(Falagard_xmlHandler::TypeAttribute,
            FalagardXMLHelper<VerticalTextFormatting>::toString(d_value));
    else
        xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_propertySource);
}

//----------------------------------------------------------------------------//
template<>
void FormattingSetting<HorizontalTextFormatting>::writeXMLTagToStream(XMLSerializer& xml_stream) const
{
    if (d_propertySource.empty())
        xml_stream.openTag(Falagard_xmlHandler::HorzFormatElement);
    else
        xml_stream.openTag(Falagard_xmlHandler::HorzFormatPropertyElement);
}

//----------------------------------------------------------------------------//
template<>
void FormattingSetting<HorizontalTextFormatting>::writeXMLAttributesToStream(XMLSerializer& xml_stream) const
{
    if (d_propertySource.empty())
        xml_stream.attribute(Falagard_xmlHandler::TypeAttribute,
            FalagardXMLHelper<HorizontalTextFormatting>::toString(d_value));
    else
        xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_propertySource);
}

//----------------------------------------------------------------------------//

}

