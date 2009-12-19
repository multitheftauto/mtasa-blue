/************************************************************************
    filename:   CEGUIXMLAttributes.cpp
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
#include "StdInc.h"
#include "CEGUIXMLAttributes.h"
#include "CEGUIExceptions.h"
#include <sstream>
#include <iterator>

// Start of CEGUI namespace section
namespace CEGUI
{
    XMLAttributes::XMLAttributes(void)
    {}

    XMLAttributes::~XMLAttributes(void)
    {}

    void XMLAttributes::add(const String& attrName, const String& attrValue)
    {
        d_attrs[attrName] = attrValue;
    }

    void XMLAttributes::remove(const String& attrName)
    {
        AttributeMap::iterator pos = d_attrs.find(attrName);

        if (pos != d_attrs.end())
            d_attrs.erase(pos);
    }

    bool XMLAttributes::exists(const String& attrName) const
    {
        return d_attrs.find(attrName) != d_attrs.end();
    }

    size_t XMLAttributes::getCount(void) const
    {
        return d_attrs.size();
    }

    const String& XMLAttributes::getName(size_t index) const
    {
        if (index >= d_attrs.size())
        {
            throw InvalidRequestException("XMLAttributes::getName - The specified index is out of range for this XMLAttributes block.");
        }

        AttributeMap::const_iterator iter = d_attrs.begin();
        std::advance(iter, index);

        return (*iter).first;
    }

    const String& XMLAttributes::getValue(size_t index) const
    {
        if (index >= d_attrs.size())
        {
            throw InvalidRequestException("XMLAttributes::getValue - The specified index is out of range for this XMLAttributes block.");
        }

        AttributeMap::const_iterator iter = d_attrs.begin();
        std::advance(iter, index);

        return (*iter).second;
    }

    const String& XMLAttributes::getValue(const String& attrName) const
    {
        AttributeMap::const_iterator pos = d_attrs.find(attrName);

        if (pos != d_attrs.end())
        {
            return (*pos).second;
        }
        else
        {
            throw UnknownObjectException("XMLAttributes::getValue - no value exists for an attribute named '" + attrName + "'.");
        }
    }

    const String& XMLAttributes::getValueAsString(const String& attrName, const String& def) const
    {
        return (exists(attrName)) ? getValue(attrName) : def;
    }


    bool XMLAttributes::getValueAsBool(const String& attrName, bool def) const
    {
        if (!exists(attrName))
        {
            return def;
        }

        const String& val = getValue(attrName);

        if (val == "false" || val == "0")
        {
            return false;
        }
        else if (val == "true" || val == "1")
        {
            return true;
        }
        else
        {
            throw InvalidRequestException("XMLAttributes::getValueAsInteger - failed to convert attribute '" + attrName + "' with value '" + getValue(attrName) + "' to bool.");
        }
    }

    int XMLAttributes::getValueAsInteger(const String& attrName, int def) const
    {
        if (!exists(attrName))
        {
            return def;
        }

        int val;
        std::istringstream strm(getValue(attrName).c_str());

        strm >> val;

        // success?
        if (strm.fail())
        {
            throw InvalidRequestException("XMLAttributes::getValueAsInteger - failed to convert attribute '" + attrName + "' with value '" + getValue(attrName) + "' to integer.");
        }

        return val;
    }

    float XMLAttributes::getValueAsFloat(const String& attrName, float def) const
    {
        if (!exists(attrName))
        {
            return def;
        }

        float val;
        std::istringstream strm(getValue(attrName).c_str());

        strm >> val;

        // success?
        if (strm.fail())
        {
            throw InvalidRequestException("XMLAttributes::getValueAsInteger - failed to convert attribute '" + attrName + "' with value '" + getValue(attrName) + "' to float.");
        }

        return val;
    }



} // End of  CEGUI namespace section
