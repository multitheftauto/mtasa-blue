/************************************************************************
    filename:   CEGUIXercesParser.cpp
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
#include "CEGUIXercesParser.h"
#include "CEGUIString.h"
#include "CEGUIExceptions.h"
#include "CEGUILogger.h"
#include "CEGUIResourceProvider.h"
#include "CEGUISystem.h"
#include "CEGUIXMLHandler.h"
#include "CEGUIXMLAttributes.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    ////////////////////////////////////////////////////////////////////////////////
    //
    // XercesParser methods
    //
    ////////////////////////////////////////////////////////////////////////////////

    XercesParser::XercesParser(void)
    {
        // set ID string
        d_identifierString = "CEGUI::XercesParser - Official Xerces-C++ based parser module for CEGUI";
    }

    XercesParser::~XercesParser(void)
    {}

    void XercesParser::parseXMLFile(XMLHandler& handler, const String& filename, const String& schemaName, const String& resourceGroup)
    {
        XERCES_CPP_NAMESPACE_USE;

        XercesHandler xercesHandler(handler);

        // create parser
        SAX2XMLReader* reader = createReader(xercesHandler);

        try
        {
            // set up schema
            initialiseSchema(reader, schemaName, filename, resourceGroup);
            // do parse
            doParse(reader, filename, resourceGroup);
        }
        catch(const XMLException& exc)
        {
            if (exc.getCode() != XMLExcepts::NoError)
            {
                delete reader;

                char* excmsg = XMLString::transcode(exc.getMessage());
                String message((utf8*)"XercesParser::parseXMLFile - An error occurred at line nr. " + PropertyHelper::uintToString((uint)exc.getSrcLine()) + " while parsing XML file '" + filename + "'.  Additional information: ");
                message += excmsg;
                XMLString::release(&excmsg);

                throw FileIOException(message);
            }

        }
        catch(const SAXParseException& exc)
        {
            delete reader;

            char* excmsg = XMLString::transcode(exc.getMessage());
            String message((utf8*)"XercesParser::parseXMLFile - An error occurred at line nr. " + PropertyHelper::uintToString((uint)exc.getLineNumber()) + " while parsing XML file '" + filename + "'.  Additional information: ");
            message += excmsg;
            XMLString::release(&excmsg);

            throw FileIOException(message);
        }
        catch(...)
        {
            delete reader;

            Logger::getSingleton().logEvent("XercesParser::parseXMLFile - An unexpected error occurred while parsing XML file '" + filename + "'.", Errors);
            throw;
        }

        // cleanup
        delete reader;
    }

    bool XercesParser::initialiseImpl(void)
    {
        XERCES_CPP_NAMESPACE_USE;

        // initialise Xerces-C XML system
        try
        {
            XMLPlatformUtils::Initialize();
        }
        catch(XMLException& exc)
        {
            // prepare a message about the failure
            char* excmsg = XMLString::transcode(exc.getMessage());
            String message((utf8*)"An exception occurred while initialising the Xerces-C XML system.  Additional information: ");
            message += (utf8*)excmsg;
            XMLString::release(&excmsg);

            // throw a std::exception (because it won't try and use logger, which may not be available)
            throw message.c_str();
        }

        return true;
    }

    void XercesParser::cleanupImpl(void)
    {
        // cleanup XML stuff
        XERCES_CPP_NAMESPACE_USE;
        XMLPlatformUtils::Terminate();
    }
    
    void XercesParser::populateAttributesBlock(const XERCES_CPP_NAMESPACE::Attributes& src, XMLAttributes& dest)
    {
        String attributeName;
        String attributeValue;

        for (uint i = 0; i < src.getLength(); ++i)
        {
            attributeName = transcodeXmlCharToString(src.getLocalName(i));
            attributeValue = transcodeXmlCharToString(src.getValue(i));
            dest.add(attributeName, attributeValue);
        }
    }

    String XercesParser::transcodeXmlCharToString(const XMLCh* const xmlch_str)
    {
        XERCES_CPP_NAMESPACE_USE;

        XMLTransService::Codes  res;
        XMLTranscoder* transcoder = XMLPlatformUtils::fgTransService->makeNewTranscoderFor(XMLRecognizer::UTF_8, res, 4096, XMLPlatformUtils::fgMemoryManager );

        if (res == XMLTransService::Ok)
        {
            String out;
            utf8 outBuff[128];
            unsigned int outputLength;
            unsigned int eaten = 0;
            unsigned int offset = 0;
            unsigned int inputLength = XMLString::stringLen(xmlch_str);

            while (inputLength)
            {
                outputLength = transcoder->transcodeTo(xmlch_str + offset, inputLength, outBuff, 128, eaten, XMLTranscoder::UnRep_RepChar);
                out.append(outBuff, outputLength);
                offset += eaten;
                inputLength -= eaten;
            }

            delete transcoder;

            return out;
        }
        else
        {
            throw GenericException((utf8*)"XercesParser::transcodeXmlCharToString - Internal Error: Could not create UTF-8 string transcoder.");
        }

    }

    void XercesParser::initialiseSchema(XERCES_CPP_NAMESPACE::SAX2XMLReader* reader, const String& schemaName, const String& xmlFilename, const String& resourceGroup)
    {
        XERCES_CPP_NAMESPACE_USE;

        // enable schema use and set validation options
        reader->setFeature(XMLUni::fgXercesSchema, true);
        reader->setFeature(XMLUni::fgSAX2CoreValidation, true);
        reader->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);

        // load in the raw schema data
        RawDataContainer rawSchemaData;
        // try base filename first
        try
        {
            Logger::getSingleton().logEvent("XercesParser::initialiseSchema - Attempting to load schema from file '" + schemaName + "'.");
            System::getSingleton().getResourceProvider()->loadRawDataContainer(schemaName, rawSchemaData, resourceGroup);
        }
        // oops, no file.  Try an alternative instead...
        catch(InvalidRequestException)
        {
            // get path from filename
            String schemaFilename;
            size_t pos = xmlFilename.rfind("/");
            if (pos == String::npos) pos = xmlFilename.rfind("\\");
            if (pos != String::npos) schemaFilename.assign(xmlFilename, 0, pos + 1);
            // append schema filename
            schemaFilename += schemaName;
            // re-try the load operation.
            Logger::getSingleton().logEvent("XercesParser::initialiseSchema - Attempting to load schema from file '" + schemaFilename + "'.");
            System::getSingleton().getResourceProvider()->loadRawDataContainer(schemaFilename, rawSchemaData, resourceGroup);
        }
        // wrap schema data in a xerces MemBufInputSource object
        MemBufInputSource  schemaData(
            rawSchemaData.getDataPtr(),
            static_cast<const unsigned int>(rawSchemaData.getSize()),
            schemaName.c_str(),
            false);
        reader->loadGrammar(schemaData, Grammar::SchemaGrammarType, true);
        // enable grammar reuse
        reader->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);

        // set schema for usage
        XMLCh* pval = XMLString::transcode(schemaName.c_str());
        reader->setProperty(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, pval);
        XMLString::release(&pval);
        Logger::getSingleton().logEvent("XercesParser::initialiseSchema - XML schema file '" + schemaName + "' has been initialised.");

        // use resource provider to release loaded schema data (if it supports this)
        System::getSingleton().getResourceProvider()->unloadRawDataContainer(rawSchemaData);
    }

    XERCES_CPP_NAMESPACE::SAX2XMLReader* XercesParser::createReader(XERCES_CPP_NAMESPACE::DefaultHandler& handler)
    {
        XERCES_CPP_NAMESPACE_USE;

        SAX2XMLReader* reader = XMLReaderFactory::createXMLReader();

        // set basic settings we want from parser
        reader->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);

        // set handlers
        reader->setContentHandler(&handler);
        reader->setErrorHandler(&handler);

        return reader;
    }

    void XercesParser::doParse(XERCES_CPP_NAMESPACE::SAX2XMLReader* parser, const String& xmlFilename, const String& resourceGroup)
    {
        XERCES_CPP_NAMESPACE_USE;

        // use resource provider to load file data
        RawDataContainer rawXMLData;
        System::getSingleton().getResourceProvider()->loadRawDataContainer(xmlFilename, rawXMLData, resourceGroup);
        MemBufInputSource  fileData(
            rawXMLData.getDataPtr(),
            static_cast<const unsigned int>(rawXMLData.getSize()),
            xmlFilename.c_str(),
            false);

         // perform parse
         try
         {
             parser->parse(fileData);
         }
         catch(...)
         {
             // use resource provider to release loaded XML source (if it supports this)
             System::getSingleton().getResourceProvider()->unloadRawDataContainer(rawXMLData);

             throw;
         }

         // use resource provider to release loaded XML source (if it supports this)
         System::getSingleton().getResourceProvider()->unloadRawDataContainer(rawXMLData);
    }

    
    ////////////////////////////////////////////////////////////////////////////////
    //
    // XercesHandler methods
    //
    ////////////////////////////////////////////////////////////////////////////////

    XercesHandler::XercesHandler(XMLHandler& handler) :
            d_handler(handler)
    {}

    XercesHandler::~XercesHandler(void)
    {}

    void XercesHandler::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE::Attributes& attrs)
    {
        XMLAttributes cegui_attributes;
        XercesParser::populateAttributesBlock(attrs, cegui_attributes);

        String element(XercesParser::transcodeXmlCharToString(localname));

        d_handler.elementStart(element, cegui_attributes);
    }

    void XercesHandler::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
    {
        String element(XercesParser::transcodeXmlCharToString(localname));

        d_handler.elementEnd(element);
    }

    void XercesHandler::warning (const XERCES_CPP_NAMESPACE::SAXParseException &exc)
    {
        XERCES_CPP_NAMESPACE_USE;

        // prepare a message about the warning
        char* excmsg = XMLString::transcode(exc.getMessage());
        String message("Xerces warning: ");
        message += (utf8*)excmsg;
        XMLString::release(&excmsg);
        Logger::getSingleton().logEvent(message);
    }

    void XercesHandler::error (const XERCES_CPP_NAMESPACE::SAXParseException &exc)
    {
        throw exc;
    }

    void XercesHandler::fatalError (const XERCES_CPP_NAMESPACE::SAXParseException &exc)
    {
        throw exc;
    }

} // End of  CEGUI namespace section
