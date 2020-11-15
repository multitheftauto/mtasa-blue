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
#include "CEGUI/XMLHandler.h"
#include "CEGUI/System.h"
#include "CEGUI/XMLParser.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    // default handler does nothing.

    XMLHandler::XMLHandler(void)
    {}

    XMLHandler::~XMLHandler(void)
    {}

    const String& XMLHandler::getSchemaName() const
    {
        static String ret = "";

        // by default, don't use XML schemas
        return ret;
    }

    void XMLHandler::handleContainer(const RawDataContainer& source)
    {
        System::getSingleton().getXMLParser()->parseXML(
                    *this, source, getSchemaName());
    }

    void XMLHandler::handleFile(const String& fileName, const String& resourceGroup)
    {
        System::getSingleton().getXMLParser()->parseXMLFile(
                    *this, fileName, getSchemaName(),
                    resourceGroup.empty() ? getDefaultResourceGroup() :
                                             resourceGroup);
    }

    void XMLHandler::handleString(const String& source)
    {
        System::getSingleton().getXMLParser()->parseXMLString(
                    *this, source, getSchemaName());
    }

    void XMLHandler::elementStart(const String&, const XMLAttributes&)
    {}

    void XMLHandler::elementEnd(const String&)
    {}
  
    void XMLHandler::text(const String&)
    {}
    
        
} // End of  CEGUI namespace section

