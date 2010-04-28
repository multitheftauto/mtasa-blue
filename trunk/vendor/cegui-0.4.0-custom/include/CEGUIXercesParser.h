/************************************************************************
    filename:   CEGUIXercesParser.h
    created:    Sat Mar 12 2005
    author:     Paul D Turner
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
#ifndef _CEGUIXercesParser_h_
#define _CEGUIXercesParser_h_

#include "CEGUIXMLParser.h"

// Xerces-C includes
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

// Start of CEGUI namespace section
namespace CEGUI
{
    class XercesHandler : public XERCES_CPP_NAMESPACE::DefaultHandler
    {
    public:
        XercesHandler(XMLHandler& handler);
        ~XercesHandler(void);

        // Implementation of methods in Xerces DefaultHandler.
        void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE::Attributes& attrs);
        void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
        void warning (const XERCES_CPP_NAMESPACE::SAXParseException &exc);
        void error (const XERCES_CPP_NAMESPACE::SAXParseException &exc);
        void fatalError (const XERCES_CPP_NAMESPACE::SAXParseException &exc);

    protected:
        XMLHandler& d_handler;      //!< This is the 'real' CEGUI based handler which we interface via.
    };

    /*!
    \brief
        Implementation of XMLParser using Xerces-C++
     */
    class XercesParser : public XMLParser
    {
    public:
        XercesParser(void);
        ~XercesParser(void);

        // Implementation of public abstract interface
        void parseXMLFile(XMLHandler& handler, const String& filename, const String& schemaName, const String& resourceGroup);

        // Internal methods
        /*!
        \brief
            Populate the CEGUI::XMLAttributes object with attribute data from the Xerces attributes block.
         */
        static void populateAttributesBlock(const XERCES_CPP_NAMESPACE::Attributes& src, XMLAttributes& dest);

        /*!
        \brief
            Return a CEGUI::String containing the Xerces XMLChar string data in \a xmlch_str.
         */
        static String transcodeXmlCharToString(const XMLCh* const xmlch_str);

    protected:
        static void initialiseSchema(XERCES_CPP_NAMESPACE::SAX2XMLReader* reader, const String& schemaName, const String& xmlFilename, const String& resourceGroup);
        static XERCES_CPP_NAMESPACE::SAX2XMLReader* createReader(XERCES_CPP_NAMESPACE::DefaultHandler& handler);
        static void doParse(XERCES_CPP_NAMESPACE::SAX2XMLReader* parser, const String& xmlFilename, const String& resourceGroup);

        // Implementation of abstract interface.
        bool initialiseImpl(void);
        void cleanupImpl(void);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIXercesParser_h_
