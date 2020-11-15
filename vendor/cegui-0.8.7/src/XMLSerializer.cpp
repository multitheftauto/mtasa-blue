/***********************************************************************
    created:    Thue May 16 2006
    author:     Olivier Delannoy 
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
#include "CEGUI/XMLSerializer.h"
#include <iostream>
// Start of CEGUI namespace section 
namespace CEGUI 
{
XMLSerializer::XMLSerializer(OutStream& out, size_t indentSpace)
    : d_error(false), d_tagCount(0), d_depth(0), d_indentSpace(indentSpace), 
      d_needClose(false), d_lastIsText(false), d_stream(out)
{
    d_stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    d_error = ! d_stream;
}


XMLSerializer::~XMLSerializer(void)
{
    if (!d_error || !d_tagStack.empty())
    {
        d_stream << std::endl;
    }
}



XMLSerializer& XMLSerializer::openTag(const String& name)
{
    if (! d_error)
    {
        ++d_tagCount;
        if (d_needClose)
        {
            d_stream << '>';	  
        }
        if (!d_lastIsText)
        {
            d_stream << std::endl;
            indentLine();
        }
        d_stream << '<' << name.c_str() << ' ';
        d_tagStack.push_back(name);
        ++d_depth;
        d_needClose = true;
        d_lastIsText = false;
        d_error = ! d_stream;
    }
    return *this;
}

XMLSerializer& XMLSerializer::closeTag(void)
{
    String back =  d_tagStack.back();
    if (! d_error)
    {
        --d_depth;
        if (d_needClose)
        {
            d_stream << "/>";
        }
        else if (! d_lastIsText)
        {
            d_stream << std::endl;
            indentLine();
            d_stream << "</" << back.c_str() << '>';
        }
        else
        {
            d_stream << "</" << back.c_str() << '>';
        }
        d_lastIsText = false;
        d_needClose = false;
        d_tagStack.pop_back();
        d_error = ! d_stream;
    }
    return *this;
}


XMLSerializer& XMLSerializer::attribute(const String& name, const String& value)
{
    if (!d_needClose)
    {
        d_error = true;
    }
    if (!d_error)
    {
        d_stream << name.c_str() << "=\""
            << convertEntityInAttribute(value).c_str()
            << "\" ";
        d_lastIsText = false;
        d_error = ! d_stream;
    }
    return *this;
}


XMLSerializer& XMLSerializer::text(const String& text)
{
    if (! d_error)
    {
        if (d_needClose)
        {
            d_stream << '>';
            d_needClose = false;
        }
        d_stream << convertEntityInText(text).c_str();
        d_lastIsText = true;
        d_error = ! d_stream;
    }
    return *this;
}

unsigned int XMLSerializer::getTagCount() const
{
    return d_tagCount;
}

void XMLSerializer::indentLine(void)
{
    size_t spaceCount = d_depth * d_indentSpace;
    // There's for sure a best way to do this but it works 
    for (size_t i = 0 ; i < spaceCount ; ++i)
    {
        d_stream << ' ';
    }  
}

String XMLSerializer::convertEntityInText(const String& text)
{
    String res;
    res.reserve(text.size()*2);
    const String::const_iterator iterEnd = text.end();
    for (String::const_iterator iter = text.begin(); iter != iterEnd ; ++iter)
    {
        switch(*iter)
        {  
            case '<':
                res += "&lt;";
                break;
      
            case '>':
                res += "&gt;";
                break;
        
            case '&':
                res += "&amp;";
                break;
        
            case '\'':
                res += "&apos;";
                break;

            case '"':
                res += "&quot;";
                break;
      
            default:
                res += *iter;
        }
    }
    return res;
}

String XMLSerializer::convertEntityInAttribute(const String& attributeValue)
{
    // Reserve a lot of space 
    String res;
    res.reserve(attributeValue.size()*2);
    const String::const_iterator iterEnd = attributeValue.end();
    for (String::const_iterator iter = attributeValue.begin(); iter != iterEnd ; ++iter)
    {
        switch(*iter)
        {  
            case '<':
                res += "&lt;";
                break;
      
            case '>':
                res += "&gt;";
                break;
        
            case '&':
                res += "&amp;";
                break;
        
            case '\'':
                res += "&apos;";
                break;

            case '"':
                res += "&quot;";
                break;
      
            case '\n':
                res += "\\n";
                break;

            default:
                res += *iter;
        }
    }

    return res;
}

} // End of CEGUI Namespace 
