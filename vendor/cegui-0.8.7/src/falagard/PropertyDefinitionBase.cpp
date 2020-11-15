/***********************************************************************
    created:    Sat Jun 16 2012
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
#include "CEGUI/falagard/PropertyDefinitionBase.h"
#include "CEGUI/XMLSerializer.h"
#include "CEGUI/falagard/XMLHandler.h"

namespace CEGUI
{

const String PropertyDefinitionBase::UserStringNameSuffix("_fal_auto_prop__");

//----------------------------------------------------------------------------//
PropertyDefinitionBase::PropertyDefinitionBase(const String& name,
                                               const String& help,
                                               const String& initialValue,
                                               bool redrawOnWrite,
                                               bool layoutOnWrite,
                                               const String& fireEvent,
                                               const String& eventNamespace) :
    d_propertyName(name),
    d_initialValue(initialValue),
    d_helpString(help),
    d_writeCausesRedraw(redrawOnWrite),
    d_writeCausesLayout(layoutOnWrite),
    d_eventFiredOnWrite(fireEvent),
    d_eventNamespace(eventNamespace)
{
}

//----------------------------------------------------------------------------//
PropertyDefinitionBase::~PropertyDefinitionBase()
{
}

//----------------------------------------------------------------------------//
const String& PropertyDefinitionBase::getPropertyName() const
{
    return d_propertyName;
}

//----------------------------------------------------------------------------//
void PropertyDefinitionBase::setPropertyName(const String& name)
{
    d_propertyName = name;
}

//----------------------------------------------------------------------------//
const String& PropertyDefinitionBase::getInitialValue() const
{
    return d_initialValue;
}

//----------------------------------------------------------------------------//
void PropertyDefinitionBase::setInitialValue(const String& value)
{
    d_initialValue = value;
}

//----------------------------------------------------------------------------//
const String& PropertyDefinitionBase::getHelpString() const
{
    return d_helpString;
}

//----------------------------------------------------------------------------//
void PropertyDefinitionBase::setHelpString(const String& help)
{
    d_helpString = help;
}

//----------------------------------------------------------------------------//
bool PropertyDefinitionBase::isRedrawOnWrite() const
{
    return d_writeCausesRedraw;
}

//----------------------------------------------------------------------------//
void PropertyDefinitionBase::setRedrawOnWrite(bool value)
{
    d_writeCausesRedraw = value;
}

//----------------------------------------------------------------------------//
bool PropertyDefinitionBase::isLayoutOnWrite() const
{
    return d_writeCausesLayout;
}

//----------------------------------------------------------------------------//
void PropertyDefinitionBase::setLayoutOnWrite(bool value)
{
    d_writeCausesLayout = value;
}

//----------------------------------------------------------------------------//
const String& PropertyDefinitionBase::getEventFiredOnWrite() const
{
    return d_eventFiredOnWrite;
}

//----------------------------------------------------------------------------//
void PropertyDefinitionBase::setEventFiredOnWrite(const String& eventName)
{
    d_eventFiredOnWrite = eventName;
}

//----------------------------------------------------------------------------//
const String& PropertyDefinitionBase::getEventNamespace() const
{
    return d_eventNamespace;
}

//----------------------------------------------------------------------------//
void PropertyDefinitionBase::setEventNamespace(const String& eventNamespace)
{
    d_eventNamespace = eventNamespace;
}

//----------------------------------------------------------------------------//
void PropertyDefinitionBase::writeDefinitionXMLToStream(
                                            XMLSerializer& xml_stream) const
{
    writeDefinitionXMLElementType(xml_stream);
    writeDefinitionXMLAttributes(xml_stream);
    xml_stream.closeTag();
}

//----------------------------------------------------------------------------//
void PropertyDefinitionBase::writeDefinitionXMLAttributes(
                                            XMLSerializer& xml_stream) const
{
    xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_propertyName);

    if (!d_initialValue.empty())
        xml_stream.attribute(Falagard_xmlHandler::InitialValueAttribute, d_initialValue);

    if (d_writeCausesRedraw)
        xml_stream.attribute(Falagard_xmlHandler::RedrawOnWriteAttribute, PropertyHelper<bool>::True);

    if (d_writeCausesLayout)
        xml_stream.attribute(Falagard_xmlHandler::LayoutOnWriteAttribute, PropertyHelper<bool>::True);

    if (!d_eventFiredOnWrite.empty())
        xml_stream.attribute(Falagard_xmlHandler::FireEventAttribute, d_eventFiredOnWrite);
}

//----------------------------------------------------------------------------//

}

