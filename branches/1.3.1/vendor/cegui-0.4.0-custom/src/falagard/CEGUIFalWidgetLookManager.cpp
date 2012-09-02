/************************************************************************
    filename:   CEGUIFalWidgetLookManager.cpp
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
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalagard_xmlHandler.h"
#include "CEGUIResourceProvider.h"
#include "CEGUIXMLParser.h"
#include "CEGUIExceptions.h"
#include "CEGUILogger.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    ////////////////////////////////////////////////////////////////////////////////
    // Static data definitions.
    template<> WidgetLookManager* Singleton<WidgetLookManager>::ms_Singleton = 0;
    const String WidgetLookManager::FalagardSchemaName("Falagard.xsd");
    ////////////////////////////////////////////////////////////////////////////////

    WidgetLookManager::WidgetLookManager()
    {
        Logger::getSingleton().logEvent("CEGUI::WidgetLookManager singleton created.");
    }

    WidgetLookManager::~ WidgetLookManager()
    {
        Logger::getSingleton().logEvent("CEGUI::WidgetLookManager singleton destroyed.");
    }

    /*************************************************************************
		Return singleton object
	  *************************************************************************/
	  WidgetLookManager&	WidgetLookManager::getSingleton(void)
	  {
		  return Singleton<WidgetLookManager>::getSingleton();
	  }

	  /*************************************************************************
		  Return singleton pointer
	  *************************************************************************/
	  WidgetLookManager*	WidgetLookManager::getSingletonPtr(void)
	  {
		  return Singleton<WidgetLookManager>::getSingletonPtr();
  	}

    void WidgetLookManager::parseLookNFeelSpecification(const String& filename, const String& resourceGroup)
    {
        // valid filenames are required!
        if (filename.empty())
        {
            throw InvalidRequestException("WidgetLookManager::parseLookNFeelSpecification - Filename supplied for look & feel file must be valid");
        }

        // create handler object
        Falagard_xmlHandler handler(this);

        // perform parse of XML data
        try
        {
            System::getSingleton().getXMLParser()->parseXMLFile(handler, filename, FalagardSchemaName, resourceGroup);
        }
        catch(...)
        {
            Logger::getSingleton().logEvent("WidgetLookManager::parseLookNFeelSpecification - loading of look and feel data from file '" + filename +"' has failed.", Errors);
            throw;
        }
    }

    bool WidgetLookManager::isWidgetLookAvailable(const String& widget) const
    {
        return d_widgetLooks.find(widget) != d_widgetLooks.end();
    }

    const WidgetLookFeel& WidgetLookManager::getWidgetLook(const String& widget) const
    {
        WidgetLookList::const_iterator wlf = d_widgetLooks.find(widget);

        if (wlf != d_widgetLooks.end())
        {
            return (*wlf).second;
        }

        throw UnknownObjectException("WidgetLookManager::getWidgetLook - Widget look and feel '" + widget + "' does not exist.");
    }

    void WidgetLookManager::eraseWidgetLook(const String& widget)
    {
        WidgetLookList::iterator wlf = d_widgetLooks.find(widget);
        if (wlf != d_widgetLooks.end())
        {
            d_widgetLooks.erase(wlf);
        }
        else
        {
            Logger::getSingleton().logEvent(
                "WidgetLookManager::eraseWidgetLook - Widget look and feel '" + widget + "' did not exist.");
        }
    }

    void WidgetLookManager::addWidgetLook(const WidgetLookFeel& look)
    {
        if (isWidgetLookAvailable(look.getName()))
        {
            Logger::getSingleton().logEvent(
                "WidgetLookManager::addWidgetLook - Widget look and feel '" + look.getName() + "' already exists.  Replacing previous definition.");
        }

        d_widgetLooks[look.getName()] = look;
    }

    void WidgetLookManager::writeFalagardXMLHeadToStream(OutStream& out_stream) const
    {
        // output xml header
        out_stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        // output root element
        out_stream << "<Falagard>" << std::endl;
    }

    void WidgetLookManager::writeFalagardXMLTailToStream(OutStream& out_stream) const
    {
        // close root element
        out_stream << "</Falagard>" << std::endl;
    }

    void WidgetLookManager::writeWidgetLookToStream(const String& name, OutStream& out_stream) const
    {
        // start of file
        writeFalagardXMLHeadToStream(out_stream);

        try
        {
            // output the desired widget look data
            getWidgetLook(name).writeXMLToStream(out_stream);
        }
        catch (UnknownObjectException)
        {
            Logger::getSingleton().logEvent("WidgetLookManager::writeWidgetLookToStream - Failed to write widget look XML data to stream.", Errors);
        }

        // close the root tags to terminate the file
        writeFalagardXMLTailToStream(out_stream);
    }

    void WidgetLookManager::writeWidgetLookSeriesToStream(const String& prefix, OutStream& out_stream) const
    {
        // start of file
        writeFalagardXMLHeadToStream(out_stream);

        for (WidgetLookList::const_iterator curr = d_widgetLooks.begin(); curr != d_widgetLooks.end(); ++curr)
        {
            if ((*curr).first.compare(0, prefix.length(), prefix) == 0)
                (*curr).second.writeXMLToStream(out_stream);
        }

        // close the root tags to terminate the file
        writeFalagardXMLTailToStream(out_stream);
    }

} // End of  CEGUI namespace section
