/***********************************************************************
    created:    Sat Mar 12 2005
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
#include "CEGUI/XMLAttributes.h"
#include "CEGUI/Exceptions.h"
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
            CEGUI_THROW(InvalidRequestException(
                "The specified index is out of range for this XMLAttributes block."));
        }

        AttributeMap::const_iterator iter = d_attrs.begin();
        std::advance(iter, index);

        return (*iter).first;
    }

    const String& XMLAttributes::getValue(size_t index) const
    {
        if (index >= d_attrs.size())
        {
            CEGUI_THROW(InvalidRequestException(
                "The specified index is out of range for this XMLAttributes block."));
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
            CEGUI_THROW(UnknownObjectException(
                "no value exists for an attribute named '" + attrName + "'."));
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

        if (val == "false" || val == "False" || val == "0")
        {
            return false;
        }
        else if (val == "true" || val == "True" || val == "1")
        {
            return true;
        }
        else
        {
            CEGUI_THROW(InvalidRequestException(
                "failed to convert attribute '" + attrName + "' with value '" + getValue(attrName) + "' to bool."));
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

        // Check for success and end-of-file
        if(strm.fail() || !strm.eof())
        {
            CEGUI_THROW(InvalidRequestException(
                "failed to convert attribute '" + attrName + "' with value '" + getValue(attrName) + "' to integer."));
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

        // Check for success and end-of-file
        if(strm.fail() || !strm.eof())
        {
            CEGUI_THROW(InvalidRequestException(
                "failed to convert attribute '" + attrName + "' with value '" + getValue(attrName) + "' to float."));
        }

        return val;
    }



} // End of  CEGUI namespace section
