/************************************************************************
    filename:   CEGUIFalWidgetComponent.cpp
    created:    Mon Jun 13 2005
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
 
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "StdInc.h"
#include "falagard/CEGUIFalWidgetComponent.h"
#include "falagard/CEGUIFalXMLEnumHelper.h"
#include "CEGUIWindowManager.h"
#include "CEGUIExceptions.h"
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{
    WidgetComponent::WidgetComponent(const String& type, const String& look, const String& suffix) :
        d_baseType(type),
        d_imageryName(look),
        d_nameSuffix(suffix),
        d_vertAlign(VA_TOP),
        d_horzAlign(HA_LEFT)
    {}

    void WidgetComponent::create(Window& parent) const
    {
        // build final name and create widget.
        String widgetName = parent.getName() + d_nameSuffix;
        Window* widget = WindowManager::getSingleton().createWindow(d_baseType, widgetName);

        // set the widget look
        if (!d_imageryName.empty())
            widget->setLookNFeel("", d_imageryName);

        // add the new widget to its parent
        parent.addChildWindow(widget);

        // set alignment options
        widget->setVerticalAlignment(d_vertAlign);
        widget->setHorizontalAlignment(d_horzAlign);

        // TODO: We still need code to specify position and size.  Due to the advanced
        // TODO: possibilities, this is better handled via a 'layout' method instead of
        // TODO: setting this once and forgetting about it.

        // initialise properties.  This is done last so that these properties can
        // override properties specified in the look assigned to the created widget.
        for(PropertiesList::const_iterator curr = d_properties.begin(); curr != d_properties.end(); ++curr)
        {
            (*curr).apply(*widget);
        }
    }

    const ComponentArea& WidgetComponent::getComponentArea() const
    {
        return d_area;
    }

    void WidgetComponent::setComponentArea(const ComponentArea& area)
    {
        d_area = area;
    }

    const String& WidgetComponent::getBaseWidgetType() const
    {
        return d_baseType;
    }

    void WidgetComponent::setBaseWidgetType(const String& type)
    {
        d_baseType = type;
    }

    const String& WidgetComponent::getWidgetLookName() const
    {
        return d_imageryName;
    }

    void WidgetComponent::setWidgetLookName(const String& look)
    {
        d_imageryName = look;
    }

    const String& WidgetComponent::getWidgetNameSuffix() const
    {
        return d_nameSuffix;
    }

    void WidgetComponent::setWidgetNameSuffix(const String& suffix)
    {
        d_nameSuffix = suffix;
    }

    VerticalAlignment WidgetComponent::getVerticalWidgetAlignemnt() const
    {
        return d_vertAlign;
    }

    void WidgetComponent::setVerticalWidgetAlignment(VerticalAlignment alignment)
    {
        d_vertAlign = alignment;
    }

    HorizontalAlignment WidgetComponent::getHorizontalWidgetAlignemnt() const
    {
        return d_horzAlign;
    }

    void WidgetComponent::setHorizontalWidgetAlignemnt(HorizontalAlignment alignment)
    {
        d_horzAlign = alignment;
    }

    void WidgetComponent::addPropertyInitialiser(const PropertyInitialiser& initialiser)
    {
        d_properties.push_back(initialiser);
    }

    void WidgetComponent::clearPropertyInitialisers()
    {
        d_properties.clear();
    }

    void WidgetComponent::layout(const Window& owner) const
    {
        try
        {
            WindowManager::getSingleton().getWindow(owner.getName() + d_nameSuffix)->setRect(Absolute, d_area.getPixelRect(owner));
        }
        catch (UnknownObjectException)
        {}
    }

    void WidgetComponent::writeXMLToStream(OutStream& out_stream) const
    {
        // output opening tag
        out_stream << "<Child type=\"" << d_baseType << "\" nameSuffix=\"" << d_nameSuffix << "\"";

        if (!d_imageryName.empty())
            out_stream << " look=\"" << d_imageryName << "\"";

        out_stream << ">" << std::endl;

        // output target area
        d_area.writeXMLToStream(out_stream);

        // output vertical alignment
        out_stream << "<VertAlignment type=\"" << FalagardXMLHelper::vertAlignmentToString(d_vertAlign) << "\" />" << std::endl;

        // output horizontal alignment
        out_stream << "<HorzAlignment type=\"" << FalagardXMLHelper::horzAlignmentToString(d_horzAlign) << "\" />" << std::endl;

        //output property initialisers
        for (PropertiesList::const_iterator prop = d_properties.begin(); prop != d_properties.end(); ++prop)
        {
            (*prop).writeXMLToStream(out_stream);
        }

        // output closing tag
        out_stream << "</Child>" << std::endl;
    }

} // End of  CEGUI namespace section
