/***********************************************************************
    created:    Mon Jun 13 2005
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
#include "CEGUI/falagard/PropertyInitialiser.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/Exceptions.h"
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{
    PropertyInitialiser::PropertyInitialiser(const String& property, const String& value) :
        d_propertyName(property),
        d_propertyValue(value)
    {}

    void PropertyInitialiser::apply(CEGUI::PropertySet& target) const
    {
        CEGUI_TRY
        {
            target.setProperty(d_propertyName, d_propertyValue);
        }
        // allow 'missing' properties
        CEGUI_CATCH (UnknownObjectException&)
        {}
    }

    void PropertyInitialiser::setTargetPropertyName(const String& name)
    {
        d_propertyName = name;
    }

    const String& PropertyInitialiser::getTargetPropertyName() const
    {
        return d_propertyName;
    }

    void PropertyInitialiser::setInitialiserValue(const String& value)
    {
        d_propertyValue = value;
    }
    const String& PropertyInitialiser::getInitialiserValue() const
    {
        return d_propertyValue;
    }

    void PropertyInitialiser::writeXMLToStream(XMLSerializer& xml_stream) const
    {
        xml_stream.openTag(Falagard_xmlHandler::PropertyElement)
            .attribute(Falagard_xmlHandler::NameAttribute, d_propertyName)
            .attribute(Falagard_xmlHandler::ValueAttribute, d_propertyValue)
            .closeTag();
    }

} // End of  CEGUI namespace section
