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
#include "CEGUI/falagard/WidgetComponent.h"
#include "CEGUI/falagard/XMLEnumHelper.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/Exceptions.h"
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{
    //! Default values
    const HorizontalAlignment WidgetComponent::HorizontalAlignmentDefault(HA_LEFT);
    const VerticalAlignment WidgetComponent::VerticalAlignmentDefault(VA_TOP);


    WidgetComponent::WidgetComponent(const String& type,
                                     const String& look,
                                     const String& suffix,
                                     const String& renderer,
                                     bool autoWindow) :
        d_baseType(type),
        d_imageryName(look),
        d_name(suffix),
        d_rendererType(renderer),
        d_autoWindow(autoWindow),
        d_vertAlign(VerticalAlignmentDefault),
        d_horzAlign(HorizontalAlignmentDefault)
    {}

    void WidgetComponent::create(Window& parent) const
    {
        Window* widget = WindowManager::getSingleton().createWindow(d_baseType, d_name);
        widget->setAutoWindow(d_autoWindow);

        // set the window renderer
        if (!d_rendererType.empty())
            widget->setWindowRenderer(d_rendererType);

        // set the widget look
        if (!d_imageryName.empty())
            widget->setLookNFeel(d_imageryName);

        // add the new widget to its parent
        parent.addChild(widget);

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

        // link up the event actions
        for (EventActionList::const_iterator i = d_eventActions.begin();
             i != d_eventActions.end();
             ++i)
        {
            (*i).initialiseWidget(*widget);
        }
    }

    void WidgetComponent::cleanup(Window& parent) const
    {
        if (!parent.isChild(getWidgetName()))
            return;

        Window* widget = parent.getChild(getWidgetName());
        // clean up up the event actions
        for (EventActionList::const_iterator i = d_eventActions.begin();
             i != d_eventActions.end();
             ++i)
        {
            (*i).cleanupWidget(*widget);
        }

        parent.destroyChild(widget);
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

    const String& WidgetComponent::getWidgetName() const
    {
        return d_name;
    }

    void WidgetComponent::setWidgetName(const String& name)
    {
        d_name= name;
    }

    const String& WidgetComponent::getWindowRendererType() const
    {
        return d_rendererType;
    }

    void WidgetComponent::setWindowRendererType(const String& type)
    {
        d_rendererType = type;
    }

    VerticalAlignment WidgetComponent::getVerticalWidgetAlignment() const
    {
        return d_vertAlign;
    }

    void WidgetComponent::setVerticalWidgetAlignment(VerticalAlignment alignment)
    {
        d_vertAlign = alignment;
    }

    HorizontalAlignment WidgetComponent::getHorizontalWidgetAlignment() const
    {
        return d_horzAlign;
    }

    void WidgetComponent::setHorizontalWidgetAlignment(HorizontalAlignment alignment)
    {
        d_horzAlign = alignment;
    }

    void WidgetComponent::addPropertyInitialiser(const PropertyInitialiser& initialiser)
    {
        d_properties.push_back(initialiser);
    }

    void WidgetComponent::removePropertyInitialiser(const String& name)
    {
        for(PropertiesList::iterator i = d_properties.begin();
                i < d_properties.end();
                ++i)
            if(i->getTargetPropertyName() == name)
                d_properties.erase(i);
    }

    void WidgetComponent::clearPropertyInitialisers()
    {
        d_properties.clear();
    }

    void WidgetComponent::setAutoWindow(bool is_auto)
    {
        d_autoWindow = is_auto;
    }

    bool WidgetComponent::isAutoWindow() const
    {
        return d_autoWindow;
    }
        
    void WidgetComponent::addEventAction(const EventAction& event_action)
    {
        d_eventActions.push_back(event_action);
    }

    void WidgetComponent::clearEventActions()
    {
        d_eventActions.clear();
    }

    void WidgetComponent::layout(const Window& owner) const
    {
        CEGUI_TRY
        {
            Rectf pixelArea(d_area.getPixelRect(owner));
            URect window_area(cegui_absdim(pixelArea.left()),
                              cegui_absdim(pixelArea.top()),
                              cegui_absdim(pixelArea.right()),
                              cegui_absdim(pixelArea.bottom()));

            Window* wnd = owner.getChild(d_name);
            wnd->setArea(window_area);
            wnd->notifyScreenAreaChanged();
        }
        CEGUI_CATCH (UnknownObjectException&)
        {}
    }

    void WidgetComponent::writeXMLToStream(XMLSerializer& xml_stream) const
    {
        // output opening tag
        xml_stream.openTag(Falagard_xmlHandler::ChildElement)
            .attribute(Falagard_xmlHandler::NameSuffixAttribute, d_name)
            .attribute(Falagard_xmlHandler::TypeAttribute, d_baseType);

        if (!d_imageryName.empty())
            xml_stream.attribute(Falagard_xmlHandler::LookAttribute, d_imageryName);

        if (!d_rendererType.empty())
            xml_stream.attribute(Falagard_xmlHandler::RendererAttribute, d_rendererType);

        if (!d_autoWindow)
            xml_stream.attribute(Falagard_xmlHandler::AutoWindowAttribute, PropertyHelper<bool>::False);

        // Output <EventAction> elements
        for (EventActionList::const_iterator i = d_eventActions.begin();
             i != d_eventActions.end();
             ++i)
        {
            (*i).writeXMLToStream(xml_stream);
        }

        // output target area
        d_area.writeXMLToStream(xml_stream);

        // output vertical alignment if not-default
        if(d_vertAlign != VerticalAlignmentDefault)
        {
            xml_stream.openTag(Falagard_xmlHandler::VertAlignmentElement);
            xml_stream.attribute(Falagard_xmlHandler::TypeAttribute, FalagardXMLHelper<VerticalAlignment>::toString(d_vertAlign));
            xml_stream.closeTag();
        }

        // output horizontal alignment if not-default
        if(d_horzAlign != HorizontalAlignmentDefault)
        {
            xml_stream.openTag(Falagard_xmlHandler::HorzAlignmentElement);
            xml_stream.attribute(Falagard_xmlHandler::TypeAttribute, FalagardXMLHelper<HorizontalAlignment>::toString(d_horzAlign));
            xml_stream.closeTag();
        }

        //output property initialisers
        for (PropertiesList::const_iterator prop = d_properties.begin(); prop != d_properties.end(); ++prop)
        {
            (*prop).writeXMLToStream(xml_stream);
        }

        // output closing tag
        xml_stream.closeTag();
    }

    const PropertyInitialiser* WidgetComponent::findPropertyInitialiser(const String& propertyName) const
    {
        PropertiesList::const_reverse_iterator i = d_properties.rbegin();
        while (i != d_properties.rend())
        {
            if ((*i).getTargetPropertyName() == propertyName)
                return &(*i);
            ++i;
        }
        return 0;
    }


    WidgetComponent::PropertyIterator WidgetComponent::getPropertyIterator() const
    {
        return PropertyIterator(d_properties.begin(),d_properties.end());
    }

    WidgetComponent::EventActionIterator
    WidgetComponent::getEventActionIterator() const
    {
        return EventActionIterator(d_eventActions.begin(),
                                   d_eventActions.end());
    }

    bool WidgetComponent::handleFontRenderSizeChange(Window& window,
                                                     const Font* font) const
    {
        if (d_area.handleFontRenderSizeChange(window, font))
        {
            window.performChildWindowLayout();
            return true;
        }

        return false;
    }

} // End of  CEGUI namespace section
