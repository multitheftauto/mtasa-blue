/***********************************************************************
    created:    12/3/2005
    author:     Paul D Turner
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
#include "CEGUI/XMLParser.h"
#include "CEGUI/DataContainer.h"
#include "CEGUI/System.h"
#include "CEGUI/ResourceProvider.h"
#include "CEGUI/Logger.h"

#include <locale.h>  

// Start of CEGUI namespace section
namespace CEGUI
{

    XMLParser::XMLParser(void) :
            d_identifierString("Unknown XML parser (vendor did not set the ID string!)"),
            d_initialised(false)
    {}

    XMLParser::~XMLParser(void)
    {}

    bool XMLParser::initialise(void)
    {
        // do this to ensure only one initialise call is made
        if (!d_initialised)
        {
            d_initialised = initialiseImpl();
        }

        return d_initialised;
    }

    void XMLParser::parseXMLFile(XMLHandler& handler, const String& filename, const String& schemaName, const String& resourceGroup)
    {
        //TODO: Once we replace all C parsing functions (sscanf and whatever is used) by the superiour std::regex and std's streams,
        // this locale-check becomes redundant and needs to be removed as well.
        char* localeAll = setlocale(LC_ALL, NULL);
        char* localeNumeric = setlocale(LC_NUMERIC, NULL);

        if( (*localeAll != 'C') || (*localeNumeric != 'C') )
        {
            // Only throw this if only numeric locale is "wrong"
            Logger::getSingleton().logEvent(
                "The C locale for LC_NUMERIC and/or LC_ALL is not set to \"C\". However, CEGUI is only ensured to parse strings and numbers correctly under the condition that the "
                "locale is set to \"C\". This is required, for example, when parsing files or converting from a (property-) value to a String (or vice-versa). If your code or one of your libraries change "
                "the locale (typically, by using the function \"setlocale\"), please set the locale back to the default everytime before a relevant call to CEGUI is issued to ensure "
                "that the parsing will be functional and correct. "
                "If you know what you are doing and there are no issues whatsoever, you may ignore this error message."
                , Errors);
        }


        // Acquire resource using CEGUI ResourceProvider
        RawDataContainer rawXMLData;
        System::getSingleton().getResourceProvider()->loadRawDataContainer(filename, rawXMLData, resourceGroup);

        CEGUI_TRY
        {
            // The actual parsing action (this is overridden and depends on the specific parser)
            parseXML(handler, rawXMLData, schemaName);
        }
        CEGUI_CATCH (const Exception&)
        {
            // hint the related file name in the log
            Logger::getSingleton().logEvent("The last thrown exception was related to XML file '" +
                                            filename + "' from resource group '" + resourceGroup + "'.", Errors);

            // exception safety
            System::getSingleton().getResourceProvider()->unloadRawDataContainer(rawXMLData);

            CEGUI_RETHROW;
        }

        // Release resource
        System::getSingleton().getResourceProvider()->unloadRawDataContainer(rawXMLData);
    }

    void XMLParser::parseXMLString(XMLHandler& handler, const String& source, const String& schemaName)
    {
        // Put the source string into a RawDataContainer
        RawDataContainer rawXMLData;

        const char* c_str = source.c_str();
        rawXMLData.setData((uint8*)c_str);
        rawXMLData.setSize(strlen(c_str));

        CEGUI_TRY
        {
        	// The actual parsing action (this is overridden and depends on the specific parser)
        	parseXML(handler, rawXMLData, schemaName);
        }
        CEGUI_CATCH(...)
        {
        	// make sure we don't allow rawXMLData to release String owned data no matter what!
        	rawXMLData.setData(0);
			rawXMLData.setSize(0);

			CEGUI_RETHROW;
        }

        // !!! We must not allow DataContainer to delete String owned data,
        //     therefore, we set it's data to 0 to avoid double-deletion
        rawXMLData.setData(0);
        rawXMLData.setSize(0);
    }

    void XMLParser::cleanup(void)
    {
        if (d_initialised)
        {
            cleanupImpl();
            d_initialised = false;
        }
    }

    const String& XMLParser::getIdentifierString() const
    {
        return d_identifierString;
    }

} // End of  CEGUI namespace section
