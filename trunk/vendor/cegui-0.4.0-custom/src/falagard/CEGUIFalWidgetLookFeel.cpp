/************************************************************************
    filename:   CEGUIFalWidgetLookFeel.cpp
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
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "CEGUIExceptions.h"
#include "CEGUILogger.h"
#include "CEGUIWindowManager.h"
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{
    WidgetLookFeel::WidgetLookFeel(const String& name) :
        d_lookName(name)
    {}

    const StateImagery& WidgetLookFeel::getStateImagery(const CEGUI::String& state) const
    {
        StateList::const_iterator imagery = d_stateImagery.find(state);

        if (imagery == d_stateImagery.end())
        {
            throw UnknownObjectException("WidgetLookFeel::getStateImagery - unknown state '" + state + "' in look '" + d_lookName + "'.");
        }

        return (*imagery).second;
    }


    const ImagerySection& WidgetLookFeel::getImagerySection(const CEGUI::String& section) const
    {
        ImageryList::const_iterator imgSect = d_imagerySections.find(section);

        if (imgSect == d_imagerySections.end())
        {
            throw UnknownObjectException("WidgetLookFeel::getImagerySection - unknown imagery section '" + section +  "' in look '" + d_lookName + "'.");
        }

        return (*imgSect).second;
    }


    const String& WidgetLookFeel::getName() const
    {
        return d_lookName;
    }

    void WidgetLookFeel::addImagerySection(const ImagerySection& section)
    {
        if (d_imagerySections.find(section.getName()) != d_imagerySections.end())
        {
            Logger::getSingleton().logEvent(
                "WidgetLookFeel::addImagerySection - Defintion for imagery section '" + section.getName() + "' already exists.  Replacing previous definition.");
        }

        d_imagerySections[section.getName()] = section;
    }

    void WidgetLookFeel::addWidgetComponent(const WidgetComponent& widget)
    {
        d_childWidgets.push_back(widget);
    }

    void WidgetLookFeel::addStateSpecification(const StateImagery& state)
    {
        if (d_stateImagery.find(state.getName()) != d_stateImagery.end())
        {
            Logger::getSingleton().logEvent(
                "WidgetLookFeel::addStateSpecification - Defintion for state '" + state.getName() + "' already exists.  Replacing previous definition.");
        }

        d_stateImagery[state.getName()] = state;
    }

    void WidgetLookFeel::addPropertyInitialiser(const PropertyInitialiser& initialiser)
    {
        d_properties.push_back(initialiser);
    }

    void WidgetLookFeel::clearImagerySections()
    {
        d_imagerySections.clear();
    }

    void WidgetLookFeel::clearWidgetComponents()
    {
        d_childWidgets.clear();
    }

    void WidgetLookFeel::clearStateSpecifications()
    {
        d_stateImagery.clear();
    }

    void WidgetLookFeel::clearPropertyInitialisers()
    {
        d_properties.clear();
    }

    void WidgetLookFeel::initialiseWidget(Window& widget) const
    {
        // add new property definitions first
        for(PropertyDefinitionList::iterator propdef = d_propertyDefinitions.begin(); propdef != d_propertyDefinitions.end(); ++propdef)
        {
            // add the property to the window
            widget.addProperty(&(*propdef));
            // write default value to get things set up properly
            widget.setProperty((*propdef).getName(), (*propdef).getDefault(&widget));
        }

        // apply properties to the parent window
        for(PropertyList::const_iterator prop = d_properties.begin(); prop != d_properties.end(); ++prop)
        {
            (*prop).apply(widget);
        }

        // add required child widgets
        for(WidgetList::const_iterator curr = d_childWidgets.begin(); curr != d_childWidgets.end(); ++curr)
        {
            (*curr).create(widget);
        }
    }

    bool WidgetLookFeel::isStateImageryPresent(const String& state) const
    {
        return d_stateImagery.find(state) != d_stateImagery.end();
    }

    void WidgetLookFeel::addNamedArea(const NamedArea& area)
    {
        if (d_namedAreas.find(area.getName()) != d_namedAreas.end())
        {
            Logger::getSingleton().logEvent(
                "WidgetLookFeel::addNamedArea - Defintion for area '" + area.getName() + "' already exists.  Replacing previous definition.");
        }

        d_namedAreas[area.getName()] = area;
    }

    void WidgetLookFeel::clearNamedAreas()
    {
        d_namedAreas.clear();
    }

    const NamedArea& WidgetLookFeel::getNamedArea(const String& name) const
    {
        NamedAreaList::const_iterator area = d_namedAreas.find(name);

        if (area == d_namedAreas.end())
        {
            throw UnknownObjectException("WidgetLookFeel::getNamedArea - unknown named area: '" + name +  "' in look '" + d_lookName + "'.");
        }

        return (*area).second;
    }

    bool WidgetLookFeel::isNamedAreaDefined(const String& name) const
    {
        return d_namedAreas.find(name) != d_namedAreas.end();
    }

    void WidgetLookFeel::layoutChildWidgets(const Window& owner) const
    {
        // apply properties to the parent window
        for(WidgetList::const_iterator wdgt = d_childWidgets.begin(); wdgt != d_childWidgets.end(); ++wdgt)
        {
            (*wdgt).layout(owner);
        }
    }

    void WidgetLookFeel::addPropertyDefinition(const PropertyDefinition& propdef)
    {
        d_propertyDefinitions.push_back(propdef);
    }

    void WidgetLookFeel::clearPropertyDefinitions()
    {
        d_propertyDefinitions.clear();
    }

    void WidgetLookFeel::writeXMLToStream(OutStream& out_stream) const
    {
        out_stream << "<WidgetLook name=\"" << d_lookName << "\">" << std::endl;

        // These sub-scobes of the loops avoid the "'curr'-already-initialized" compile error on VC6++
        {
          // output property definitions
          for (PropertyDefinitionList::const_iterator curr = d_propertyDefinitions.begin(); curr != d_propertyDefinitions.end(); ++curr)
              (*curr).writeXMLToStream(out_stream);
        }

        {
          // output property initialisers.
          for (PropertyList::const_iterator curr = d_properties.begin(); curr != d_properties.end(); ++curr)
              (*curr).writeXMLToStream(out_stream);
        }

        {
          // output named areas
          for (NamedAreaList::const_iterator curr = d_namedAreas.begin(); curr != d_namedAreas.end(); ++curr)
              (*curr).second.writeXMLToStream(out_stream);
        }

        {
          // output child widgets
          for (WidgetList::const_iterator curr = d_childWidgets.begin(); curr != d_childWidgets.end(); ++curr)
              (*curr).writeXMLToStream(out_stream);
        }

        {
          // output imagery sections
          for (ImageryList::const_iterator curr = d_imagerySections.begin(); curr != d_imagerySections.end(); ++curr)
              (*curr).second.writeXMLToStream(out_stream);
        }

        {
          // output states
          for (StateList::const_iterator curr = d_stateImagery.begin(); curr != d_stateImagery.end(); ++curr)
              (*curr).second.writeXMLToStream(out_stream);
        }

        out_stream << "</WidgetLook>" << std::endl;
    }

    void WidgetLookFeel::renameChildren(const Window& widget, const String& newBaseName) const
    {
        WindowManager& winMgr = WindowManager::getSingleton();

        for(WidgetList::const_iterator curr = d_childWidgets.begin(); curr != d_childWidgets.end(); ++curr)
            winMgr.renameWindow(widget.getName() + (*curr).getWidgetNameSuffix(),
                                newBaseName + (*curr).getWidgetNameSuffix());
    }

} // End of  CEGUI namespace section
